#include "AltbotCombatLog.h"
#include "AltbotConfig.h"
#include "Log.h"
#include "Player.h"
#include "SharedDefines.h"
#include "SpellInfo.h"
#include "SpellMgr.h"
#include "Timer.h"
#include "Unit.h"
#include <algorithm>
#include <cstdio>
#include <sstream>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

namespace
{
    // Per-bot accumulators. Lives keyed by bot GUID (raw uint64) so we don't
    // hold cached Player* pointers across teleports / logout. Cleared on
    // bot login or on reset between fights.
    struct CombatLogState
    {
        // Aggregate fight totals. Reset on OnCombatEnter.
        uint32 castsAttempted = 0;
        uint32 castsOk        = 0;
        uint32 castsFailed    = 0;
        uint64 damageDealt    = 0;
        uint32 noTierTicks    = 0;

        // result-code histogram (only populated on failures).
        std::unordered_map<uint32 /*SpellCastResult*/, uint32> failsByCode;

        // top-N spell histogram. Map cap'd by truncating before output.
        std::unordered_map<uint32 /*spellId*/, uint32> castsBySpell;

        // Resource floor + ceiling — cheap min/max sampling for mana sustain
        // diagnostics.
        float minResourcePct = 100.0f;
        float maxResourcePct = 0.0f;
    };

    std::unordered_map<uint64, CombatLogState> g_state;

    std::unordered_set<uint64> g_tracedBots;

    thread_local int          tl_currentTierIdx  = -1;
    thread_local char const*  tl_currentTierName = "maint";

    // Compile-time-ish gate. `Altbot.Telemetry.Enabled` defaults to false so
    // the entire surface is dormant until explicitly turned on.
    bool TelemetryEnabled()        { return sAltbotConfig->TelemetryEnabled();      }
    bool TelemetryWhisper()        { return sAltbotConfig->TelemetryWhisperSummary();}
    bool TelemetryPerCast()        { return sAltbotConfig->TelemetryPerCastLog();   }
    uint32 TelemetryMinFightMs()   { return sAltbotConfig->TelemetryMinFightMs();   }

    char const* SafeName(Unit const* u) { return u ? u->GetName().c_str() : "(null)"; }

    char const* SchoolName(uint32 schoolMask)
    {
        // Single-school best-fit; multi-school spells log the lowest matching.
        if (schoolMask & SPELL_SCHOOL_MASK_HOLY)    return "holy";
        if (schoolMask & SPELL_SCHOOL_MASK_FIRE)    return "fire";
        if (schoolMask & SPELL_SCHOOL_MASK_NATURE)  return "nature";
        if (schoolMask & SPELL_SCHOOL_MASK_FROST)   return "frost";
        if (schoolMask & SPELL_SCHOOL_MASK_SHADOW)  return "shadow";
        if (schoolMask & SPELL_SCHOOL_MASK_ARCANE)  return "arcane";
        if (schoolMask & SPELL_SCHOOL_MASK_NORMAL)  return "phys";
        return "?";
    }

    // Resource percent for the bot's primary power. Mana for casters,
    // rage/energy/runic-power/focus for melee/hunter — `GetPower` /
    // `GetMaxPower` already covers all.
    float ResourcePctFor(Player const* bot)
    {
        if (!bot) return 0.0f;
        Powers p = bot->GetPowerType();
        uint32 cur = bot->GetPower(p);
        uint32 max = bot->GetMaxPower(p);
        if (max == 0) return 0.0f;
        return (float(cur) / float(max)) * 100.0f;
    }

    char const* ResourceKindFor(Player const* bot)
    {
        if (!bot) return "?";
        switch (bot->GetPowerType())
        {
            case POWER_MANA:        return "mana";
            case POWER_RAGE:        return "rage";
            case POWER_FOCUS:       return "focus";
            case POWER_ENERGY:      return "energy";
            case POWER_RUNIC_POWER: return "rp";
            default:                return "other";
        }
    }
}

namespace AltbotCombatLog
{

void SetCurrentTier(int idx, char const* name)
{
    tl_currentTierIdx  = idx;
    tl_currentTierName = (name && *name) ? name : "tier";
}

void ClearCurrentTier()
{
    tl_currentTierIdx  = -1;
    tl_currentTierName = "maint";
}

void SetTrace(ObjectGuid botGuid, bool enabled)
{
    if (enabled) g_tracedBots.insert(botGuid.GetRawValue());
    else         g_tracedBots.erase (botGuid.GetRawValue());
}

bool IsTraced(ObjectGuid botGuid)
{
    return g_tracedBots.find(botGuid.GetRawValue()) != g_tracedBots.end();
}

void OnCombatEnter(Player* bot)
{
    if (!TelemetryEnabled() || !bot) return;

    auto& s = g_state[bot->GetGUID().GetRawValue()];
    s = CombatLogState{};   // reset all fields

    if (TelemetryPerCast() || IsTraced(bot->GetGUID()))
    {
        TC_LOG_INFO("altbot.combat",
            "ts=%u | bot=%s | event=combat_enter | resource=%s | resource_pct=%.0f",
            getMSTime(), bot->GetName().c_str(),
            ResourceKindFor(bot), ResourcePctFor(bot));
    }
}

void OnCombatLeave(Player* bot, Player* master, uint32 combatElapsedMs)
{
    if (!TelemetryEnabled() || !bot) return;

    auto it = g_state.find(bot->GetGUID().GetRawValue());
    if (it == g_state.end())
        return;

    CombatLogState const& s = it->second;

    // Drop trash skirmishes below the configurable floor. Avoids per-pull
    // whisper spam during routine grinding.
    bool emit = combatElapsedMs >= TelemetryMinFightMs() || IsTraced(bot->GetGUID());
    if (emit)
    {
        // Compute DPS from accumulated OnDamage.
        double seconds = combatElapsedMs > 0 ? double(combatElapsedMs) / 1000.0 : 1.0;
        double dps     = double(s.damageDealt) / seconds;

        // Top 3 spells by cast count.
        std::vector<std::pair<uint32, uint32>> top(s.castsBySpell.begin(),
                                                   s.castsBySpell.end());
        std::sort(top.begin(), top.end(),
                  [](auto const& a, auto const& b) { return a.second > b.second; });
        std::ostringstream topStr;
        for (size_t i = 0; i < std::min<size_t>(top.size(), 3); ++i)
        {
            uint32 sid = top[i].first;
            char const* name = "?";
            if (SpellInfo const* info = sSpellMgr->GetSpellInfo(sid))
                if (info->SpellName)
                    name = info->SpellName;
            if (i) topStr << " ";
            topStr << name << "(" << top[i].second << ")";
        }
        if (top.empty())
            topStr << "(none)";

        // GCD utilization heuristic: every successful cast burns ~1.5s GCD on
        // a caster; treat (casts_ok * 1500) / combat_ms as utilization. Not
        // exact (instant casts overlap, hasted casts compress) but a useful
        // first-order signal.
        double gcdEstMs = double(s.castsOk) * 1500.0;
        double gcdPct   = combatElapsedMs > 0 ? (gcdEstMs / double(combatElapsedMs)) * 100.0 : 0.0;
        if (gcdPct > 100.0) gcdPct = 100.0;

        char summary[512];
        snprintf(summary, sizeof(summary),
            "[Altbot] %s — %.1fs | DPS %.0f | %u casts (%u ok, %u fail) | gcd~%.0f%% | top: %s | dmg %llu | no-tier %u",
            bot->GetName().c_str(),
            seconds, dps,
            s.castsAttempted, s.castsOk, s.castsFailed,
            gcdPct, topStr.str().c_str(),
            (unsigned long long)s.damageDealt, s.noTierTicks);

        TC_LOG_INFO("altbot.combat",
            "ts=%u | event=combat_leave | %s",
            getMSTime(), summary);

        if (TelemetryWhisper() && master && master->IsInWorld())
        {
            // Direct whisper from the bot to its master. The plan's multi-bot
            // batcher consolidates simultaneous summaries from a roster of
            // bots; v1 sends one per bot. Master's chat throttle keeps it
            // tolerable.
            bot->Whisper(summary, LANG_UNIVERSAL, master);
        }
    }

    // Always erase the per-fight state — even short fights — so we don't carry
    // forward partial data into the next pull.
    g_state.erase(it);

    // Reset thread-local tier so next combat starts clean.
    ClearCurrentTier();
}

void OnCastIssued(Player* bot, Unit* target, uint32 spellId,
                  char const* specLabel, int tierIdx, char const* tierName,
                  SpellCastResult result)
{
    if (!TelemetryEnabled() || !bot) return;

    // Use thread-local tier when caller didn't specify (defaults from the
    // 4-arg shim).
    if (tierIdx == -999)         tierIdx  = tl_currentTierIdx;
    if (!tierName || !*tierName) tierName = tl_currentTierName;

    auto& s = g_state[bot->GetGUID().GetRawValue()];
    s.castsAttempted += 1;
    if (result == SPELL_CAST_OK) s.castsOk     += 1;
    else                         s.castsFailed += 1;
    s.castsBySpell[spellId] += 1;
    if (result != SPELL_CAST_OK)
        s.failsByCode[uint32(result)] += 1;

    float pct = ResourcePctFor(bot);
    if (pct < s.minResourcePct) s.minResourcePct = pct;
    if (pct > s.maxResourcePct) s.maxResourcePct = pct;

    bool verbose = TelemetryPerCast() || IsTraced(bot->GetGUID());
    if (!verbose)
        return;

    SpellInfo const* info = sSpellMgr->GetSpellInfo(spellId);
    char const* spellName = (info && info->SpellName) ? info->SpellName : "?";
    char const* school    = info ? SchoolName(info->SchoolMask) : "?";

    // Pipe-separated key=value: greppable, importable, easy to parse.
    TC_LOG_INFO("altbot.combat",
        "ts=%u | bot=%s | spec=%s | tier_idx=%d | tier_name=%s | "
        "spell_id=%u | spell_name=%s | school=%s | "
        "target=%s | target_hp_pct=%.0f | bot_hp_pct=%.0f | "
        "resource_kind=%s | resource_pct=%.0f | "
        "result_code=%u",
        getMSTime(), bot->GetName().c_str(),
        specLabel ? specLabel : "?",
        tierIdx, tierName ? tierName : "?",
        spellId, spellName, school,
        SafeName(target),
        target ? target->GetHealthPct() : 0.0f,
        bot->GetHealthPct(),
        ResourceKindFor(bot), pct,
        uint32(result));
}

void OnNoTierFired(Player* bot, char const* /*specLabel*/)
{
    if (!TelemetryEnabled() || !bot) return;
    auto& s = g_state[bot->GetGUID().GetRawValue()];
    s.noTierTicks += 1;
}

void OnDamageDealt(ObjectGuid botGuid, uint32 damage)
{
    if (!TelemetryEnabled() || damage == 0) return;
    auto it = g_state.find(botGuid.GetRawValue());
    if (it == g_state.end())
    {
        // Damage outside the combat window — most commonly the bot's pet
        // applying a DoT before master reaches the threat-window edge. Skip
        // silently rather than allocate a fight-start record we can't
        // attribute.
        return;
    }
    it->second.damageDealt += damage;
}

} // namespace AltbotCombatLog

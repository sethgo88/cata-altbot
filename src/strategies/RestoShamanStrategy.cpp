#include "RestoShamanStrategy.h"
#include "AltbotTickContext.h"
#include "Log.h"
#include "ObjectAccessor.h"
#include "Player.h"
#include "SharedDefines.h"
#include "SpellHistory.h"
#include "SpellInfo.h"
#include "SpellMgr.h"
#include "StrategyUtil.h"
#include "Unit.h"

namespace
{
    constexpr float MANA_NORMAL_THRESHOLD   = 50.0f;
    constexpr float MANA_CRISIS_THRESHOLD   = 25.0f;

    constexpr float SELF_EMERGENCY_HP_PCT   = 35.0f;
    constexpr float TANK_EMERGENCY_HP_PCT   = 40.0f;
    constexpr float SPIRIT_LINK_TRIGGER_HP  = 40.0f;
    constexpr int   SPIRIT_LINK_MIN_LOW     = 3;
    constexpr float CHAIN_HEAL_TRIGGER_HP   = 90.0f;
    constexpr int   CHAIN_HEAL_MIN_INJURED  = 3;
    constexpr float GHW_TRIGGER_HP_PCT      = 60.0f;
    constexpr float HSURGE_NON_TANK_PCT     = 50.0f;
    constexpr float HW_FILLER_PCT           = 90.0f;
    constexpr float DPS_ALL_HEALTHY_PCT     = 95.0f;
    constexpr float RIPTIDE_FALLBACK_HP_PCT = 90.0f;

    // Threshold separating Healing Wave (low SpellLevel) from Greater Healing Wave
    // (SpellLevel 68 in Cata) when classifying direct heals with cast time > 2000ms.
    constexpr uint32 GHW_LEVEL_FLOOR = 50;

    constexpr uint32 THREAT_WINDOW_MS = 2000;

    constexpr char const* SPEC_LABEL = "RestoShaman";
} // anon namespace

RestoShamanStrategy::RestoShamanStrategy() = default;

void RestoShamanStrategy::Update(Player* bot, Player* master, AltbotTickContext const& ctx)
{
    if (!_cacheResolved)
    {
        ResolveSpellCache(bot);
        _cacheResolved = true;

        TC_LOG_DEBUG("altbot",
            "RestoShamanStrategy cache for '%s': "
            "HSurge=%u HW=%u GHW=%u Riptide=%u ChainHeal=%u "
            "EarthShield=%u WaterShield=%u SpiritLinkTotem=%u ManaTideTotem=%u LightningBolt=%u",
            bot->GetName().c_str(),
            GetSpell(Spell::HealingSurge), GetSpell(Spell::HealingWave),
            GetSpell(Spell::GreaterHealingWave), GetSpell(Spell::Riptide),
            GetSpell(Spell::ChainHeal), GetSpell(Spell::EarthShield),
            GetSpell(Spell::WaterShield), GetSpell(Spell::SpiritLinkTotem),
            GetSpell(Spell::ManaTideTotem), GetSpell(Spell::LightningBolt));
    }

    DoMaintenance(bot, master);

    ManaMode mode = GetManaMode(bot);
    CheckCooldowns(bot, mode);

    // Skip the rotation while a cast is in progress — Healing Wave (2.5s) and
    // Greater Healing Wave (3s) are the bulk of healer GCDs, and re-issuing a
    // heal every tick during the cast produces spurious
    // SPELL_FAILED_SPELL_IN_PROGRESS attempts and stops lower tiers from
    // running on the next tick.
    if (bot->HasUnitState(UNIT_STATE_CASTING) || bot->IsNonMeleeSpellCast(false))
        return;

    // Skip while the GCD is active — every tier would otherwise attempt
    // CastSpell and reject with SPELL_FAILED_NOT_READY (69). Probe with
    // Healing Wave (always known on a resto shaman; standard GCD category).
    if (uint32 hw = GetSpell(Spell::HealingWave))
    {
        SpellInfo const* hwInfo = sSpellMgr->GetSpellInfo(hw);
        if (hwInfo && bot->GetSpellHistory()->HasGlobalCooldown(hwInfo))
            return;
    }

    if (Tier1_SelfEmergency(bot))                     return;
    if (Tier2_TankEmergency(bot, master, mode))       return;
    if (Tier3_Riptide(bot, master))                   return;
    if (Tier4_SpiritLink(bot, master))                return;
    if (Tier5_ChainHeal(bot, master, mode))           return;
    if (Tier6_GHW(bot, master, mode))                 return;
    if (Tier7_HealingSurgeNonTank(bot, master, mode)) return;
    if (Tier8_HealingWaveFiller(bot, master))         return;
    Tier9_SupportDPS(bot, master, mode, ctx);
}

// Spell cache: classify each known spell by its effect signature so the
// strategy never embeds a hard-coded spell ID. Uses the predicates from
// the Phase 6 plan; SpiritLinkTotem and ManaTideTotem are deferred until
// totem disambiguation lands.
void RestoShamanStrategy::ResolveSpellCache(Player* bot)
{
    _cache.fill(0);

    struct Slot { uint32 id = 0; uint32 lvl = 0; };
    Slot bestHsurge;
    Slot bestHw;
    Slot bestGhw;
    Slot bestRiptide;
    Slot bestChainHeal;
    Slot bestEarthShield;
    Slot bestWaterShield;
    Slot bestLightningBolt;

    for (auto const& [spellId, playerSpell] : bot->GetSpellMap())
    {
        if (playerSpell.state == PLAYERSPELL_REMOVED || !playerSpell.active)
            continue;

        SpellInfo const* info = sSpellMgr->GetSpellInfo(spellId);
        if (!info)
            continue;

        bool isNature        = (info->SchoolMask & SPELL_SCHOOL_MASK_NATURE) != 0;
        bool hasDirectHeal   = false;
        bool hasPeriodicHeal = false;
        bool hasChainTargets = false;
        bool hasApplyAura    = false;
        bool hasSchoolDamage = false;

        for (uint8 i = 0; i < MAX_SPELL_EFFECTS; ++i)
        {
            uint32 effect = info->Effects[i].Effect;
            if (effect == SPELL_EFFECT_HEAL)
                hasDirectHeal = true;
            if (effect == SPELL_EFFECT_APPLY_AURA)
                hasApplyAura = true;
            if (effect == SPELL_EFFECT_SCHOOL_DAMAGE)
                hasSchoolDamage = true;
            if (info->Effects[i].ApplyAuraName == SPELL_AURA_PERIODIC_HEAL)
                hasPeriodicHeal = true;
            if (effect == SPELL_EFFECT_HEAL && info->Effects[i].ChainTarget > 0)
                hasChainTargets = true;
        }

        uint32 castMs   = info->CastTimeEntry ? uint32(info->CastTimeEntry->Base) : 0;
        uint32 spellLvl = info->SpellLevel;

        // Riptide: instant heal + HoT (both effects in the same spell)
        if (hasDirectHeal && hasPeriodicHeal && spellLvl > bestRiptide.lvl)
            bestRiptide = {spellId, spellLvl};

        // Chain Heal: direct heal that smart-jumps
        if (hasChainTargets && spellLvl > bestChainHeal.lvl)
            bestChainHeal = {spellId, spellLvl};

        // Direct heals split by cast-time band
        if (hasDirectHeal && !hasPeriodicHeal && !hasChainTargets)
        {
            if (castMs > 0 && castMs <= 2000)
            {
                if (spellLvl > bestHsurge.lvl)
                    bestHsurge = {spellId, spellLvl};
            }
            else if (castMs > 2000)
            {
                if (spellLvl >= GHW_LEVEL_FLOOR)
                {
                    if (spellLvl > bestGhw.lvl)
                        bestGhw = {spellId, spellLvl};
                }
                else
                {
                    if (spellLvl > bestHw.lvl)
                        bestHw = {spellId, spellLvl};
                }
            }
        }

        // Shield buffs (Earth/Water): nature, positive, APPLY_AURA.
        // Earth Shield carries a unit-target flag; Water Shield is self-cast.
        if (hasApplyAura && isNature && info->IsPositive())
        {
            bool unitTargeted = (info->ExplicitTargetMask & TARGET_FLAG_UNIT_MASK) != 0;
            if (unitTargeted)
            {
                if (spellLvl > bestEarthShield.lvl)
                    bestEarthShield = {spellId, spellLvl};
            }
            else
            {
                if (spellLvl > bestWaterShield.lvl)
                    bestWaterShield = {spellId, spellLvl};
            }
        }

        // Lightning Bolt: direct nature damage
        if (hasSchoolDamage && isNature && !info->IsPositive() && spellLvl > bestLightningBolt.lvl)
            bestLightningBolt = {spellId, spellLvl};
    }

    _cache[size_t(Spell::HealingSurge)]       = bestHsurge.id;
    _cache[size_t(Spell::HealingWave)]        = bestHw.id;
    _cache[size_t(Spell::GreaterHealingWave)] = bestGhw.id;
    _cache[size_t(Spell::Riptide)]            = bestRiptide.id;
    _cache[size_t(Spell::ChainHeal)]          = bestChainHeal.id;
    _cache[size_t(Spell::EarthShield)]        = bestEarthShield.id;
    _cache[size_t(Spell::WaterShield)]        = bestWaterShield.id;
    // SpiritLinkTotem / ManaTideTotem: SUMMON-effect classification cannot
    // distinguish them from other totems without DBC totem-slot inspection.
    // Left at 0; tiers that need them no-op until the disambiguation lands.
    _cache[size_t(Spell::SpiritLinkTotem)]    = 0;
    _cache[size_t(Spell::ManaTideTotem)]      = 0;
    _cache[size_t(Spell::LightningBolt)]      = bestLightningBolt.id;
}

RestoShamanStrategy::ManaMode RestoShamanStrategy::GetManaMode(Player* bot) const
{
    uint32 maxMana = bot->GetMaxPower(POWER_MANA);
    if (!maxMana)
        return ManaMode::Normal;
    float pct = 100.0f * float(bot->GetPower(POWER_MANA)) / float(maxMana);
    if (pct < MANA_CRISIS_THRESHOLD) return ManaMode::Crisis;
    if (pct < MANA_NORMAL_THRESHOLD) return ManaMode::Conserve;
    return ManaMode::Normal;
}

bool RestoShamanStrategy::IsOnCooldown(Player* bot, uint32 spellId) const
{
    if (!spellId)
        return true;
    SpellInfo const* info = sSpellMgr->GetSpellInfo(spellId);
    if (!info)
        return true;
    return bot->GetSpellHistory()->HasCooldown(info);
}

bool RestoShamanStrategy::TryCast(Player* bot, Unit* target, Spell s) const
{
    uint32 id = GetSpell(s);
    if (!id || !target)
        return false;
    SpellInfo const* info = sSpellMgr->GetSpellInfo(id);
    if (!info)
        return false;
    if (bot->GetSpellHistory()->HasCooldown(info))
        return false;
    return StrategyUtil::CastWithLog(bot, target, id, SPEC_LABEL) == SPELL_CAST_OK;
}

void RestoShamanStrategy::DoMaintenance(Player* bot, Player* master)
{
    uint32 ws = GetSpell(Spell::WaterShield);
    if (ws && !bot->HasAura(ws))
        TryCast(bot, bot, Spell::WaterShield);

    uint32 es = GetSpell(Spell::EarthShield);
    if (es)
    {
        Player* tank = StrategyUtil::FindTank(bot, master);
        if (tank && tank->IsAlive() && !tank->HasAura(es))
            TryCast(bot, tank, Spell::EarthShield);
    }
}

void RestoShamanStrategy::CheckCooldowns(Player* /*bot*/, ManaMode /*mode*/)
{
    // Mana Tide Totem at mana < 60% — DEFERRED until totem detection lands.
    // Spirit Link Totem cluster trigger fires from Tier 4 and is also deferred.
}

bool RestoShamanStrategy::Tier1_SelfEmergency(Player* bot) const
{
    if (bot->GetHealthPct() >= SELF_EMERGENCY_HP_PCT)
        return false;
    return TryCast(bot, bot, Spell::HealingSurge);
}

bool RestoShamanStrategy::Tier2_TankEmergency(Player* bot, Player* master, ManaMode mode) const
{
    Player* tank = StrategyUtil::FindTank(bot, master);
    if (!tank || !tank->IsAlive())
        return false;
    if (tank->GetHealthPct() >= TANK_EMERGENCY_HP_PCT)
        return false;

    if (mode == ManaMode::Normal && GetSpell(Spell::GreaterHealingWave))
        if (TryCast(bot, tank, Spell::GreaterHealingWave))
            return true;

    return TryCast(bot, tank, Spell::HealingSurge);
}

bool RestoShamanStrategy::Tier3_Riptide(Player* bot, Player* master) const
{
    uint32 rt = GetSpell(Spell::Riptide);
    if (!rt || IsOnCooldown(bot, rt))
        return false;

    Player* tank = StrategyUtil::FindTank(bot, master);
    if (tank && tank->IsAlive() && !tank->HasAura(rt))
        return TryCast(bot, tank, Spell::Riptide);

    Player* lowest = StrategyUtil::FindLowestHpAlly(bot, master);
    if (lowest && lowest->GetHealthPct() < RIPTIDE_FALLBACK_HP_PCT && !lowest->HasAura(rt))
        return TryCast(bot, lowest, Spell::Riptide);

    return false;
}

bool RestoShamanStrategy::Tier4_SpiritLink(Player* bot, Player* master) const
{
    uint32 slt = GetSpell(Spell::SpiritLinkTotem);
    if (!slt || IsOnCooldown(bot, slt))
        return false;
    if (StrategyUtil::CountInjured(bot, master, SPIRIT_LINK_TRIGGER_HP) < SPIRIT_LINK_MIN_LOW)
        return false;
    return TryCast(bot, bot, Spell::SpiritLinkTotem);
}

bool RestoShamanStrategy::Tier5_ChainHeal(Player* bot, Player* master, ManaMode mode) const
{
    if (mode == ManaMode::Crisis)
        return false;
    if (StrategyUtil::CountInjured(bot, master, CHAIN_HEAL_TRIGGER_HP) < CHAIN_HEAL_MIN_INJURED)
        return false;

    Player* lowest = StrategyUtil::FindLowestHpAlly(bot, master);
    if (!lowest)
        return false;
    return TryCast(bot, lowest, Spell::ChainHeal);
}

bool RestoShamanStrategy::Tier6_GHW(Player* bot, Player* master, ManaMode mode) const
{
    if (mode != ManaMode::Normal)
        return false;
    if (!GetSpell(Spell::GreaterHealingWave))
        return false;

    Player* lowest = StrategyUtil::FindLowestHpAlly(bot, master);
    if (!lowest || lowest->GetHealthPct() >= GHW_TRIGGER_HP_PCT)
        return false;
    return TryCast(bot, lowest, Spell::GreaterHealingWave);
}

bool RestoShamanStrategy::Tier7_HealingSurgeNonTank(Player* bot, Player* master, ManaMode mode) const
{
    if (mode == ManaMode::Crisis)
        return false;

    Player* tank   = StrategyUtil::FindTank(bot, master);
    Player* lowest = StrategyUtil::FindLowestHpAlly(bot, master, tank);
    if (!lowest || lowest->GetHealthPct() >= HSURGE_NON_TANK_PCT)
        return false;
    return TryCast(bot, lowest, Spell::HealingSurge);
}

bool RestoShamanStrategy::Tier8_HealingWaveFiller(Player* bot, Player* master) const
{
    Player* lowest = StrategyUtil::FindLowestHpAlly(bot, master);
    if (!lowest || lowest->GetHealthPct() >= HW_FILLER_PCT)
        return false;
    return TryCast(bot, lowest, Spell::HealingWave);
}

bool RestoShamanStrategy::Tier9_SupportDPS(Player* bot, Player* master, ManaMode mode, AltbotTickContext const& ctx) const
{
    if (ctx.combatElapsedMs < THREAT_WINDOW_MS)
        return false;
    if (mode != ManaMode::Normal)
        return false;
    if (!StrategyUtil::AllAtFullHp(bot, master, DPS_ALL_HEALTHY_PCT))
        return false;
    if (!GetSpell(Spell::LightningBolt))
        return false;

    Unit* target = ObjectAccessor::GetUnit(*bot, master->GetTarget());
    if (!target || !target->IsAlive() || !bot->IsValidAttackTarget(target))
        return false;

    if (!bot->IsInCombat())
        bot->Attack(target, true);

    return TryCast(bot, target, Spell::LightningBolt);
}

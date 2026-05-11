#include "FrostMageStrategy.h"
#include "AltbotCombatLog.h"
#include "AltbotPosition.h"
#include "AltbotPositionManager.h"
#include "AltbotTickContext.h"
#include "Log.h"
#include "ObjectAccessor.h"
#include "Pet.h"
#include "Player.h"
#include "SharedDefines.h"
#include "SpellAuraEffects.h"
#include "SpellAuras.h"
#include "SpellHistory.h"
#include "SpellInfo.h"
#include "SpellMgr.h"
#include "StrategyUtil.h"
#include "Unit.h"
#include <cstring>

namespace
{
    constexpr uint32 THREAT_WINDOW_MS = 2000;

    constexpr float CASTER_RANGE        = 25.0f;
    constexpr float MELEE_NEAR_RADIUS   = 8.0f;
    constexpr float CONE_RANGE          = 8.0f;
    constexpr int   MELEE_TRIGGER_COUNT = 2;
    constexpr float ESCAPE_HP_PCT       = 60.0f;
    constexpr float ICE_BLOCK_HP_PCT    = 20.0f;

    constexpr float AOE_RADIUS          = 10.0f;
    constexpr int   AOE_MIN_TARGETS     = 3;

    constexpr char const* SPEC_LABEL = "FrostMage";
}

namespace
{
    // Look up bot's aura by name (lazy resolution for proc auras whose IDs
    // weren't in the spellbook scan). Returns the aura if present.
    //
    // Passive talents share SpellName with their proc auras: 44544/74396 both
    // named "Fingers of Frost", 44546/57761 both named "Brain Freeze". Skip
    // passives so we only match the consumable proc.
    Aura* FindBotAuraByName(Player* bot, char const* name)
    {
        for (auto const& [spellId, app] : bot->GetAppliedAuras())
        {
            Aura* a = app->GetBase();
            if (!a)
                continue;
            SpellInfo const* info = a->GetSpellInfo();
            if (!info || !info->SpellName)
                continue;
            if (info->SpellFamilyName != SPELLFAMILY_MAGE)
                continue;
            if (info->IsPassive())
                continue;
            if (std::strcmp(info->SpellName, name) == 0)
                return a;
        }
        return nullptr;
    }

    bool HasFingersOfFrost(Player* bot) { return FindBotAuraByName(bot, "Fingers of Frost") != nullptr; }
    bool HasBrainFreeze   (Player* bot) { return FindBotAuraByName(bot, "Brain Freeze")    != nullptr; }
}

void FrostMageStrategy::Update(Player* bot, Player* master, AltbotTickContext const& ctx)
{
    if (!_cacheResolved)
    {
        ResolveSpellCache(bot);
        _cacheResolved = true;

        TC_LOG_DEBUG("altbot",
            "FrostMageStrategy cache for '%s': "
            "MoltenArmor=%u MageArmor=%u FrostArmor=%u Frostbolt=%u FFB=%u Fireball=%u "
            "IceLance=%u DeepFreeze=%u FrostNova=%u CoC=%u Blizzard=%u Pet=%u "
            "IceBlock=%u MageWard=%u Blink=%u",
            bot->GetName().c_str(),
            GetSpell(Spell::MoltenArmor), GetSpell(Spell::MageArmor),
            GetSpell(Spell::FrostArmor), GetSpell(Spell::Frostbolt),
            GetSpell(Spell::FrostfireBolt), GetSpell(Spell::Fireball),
            GetSpell(Spell::IceLance), GetSpell(Spell::DeepFreeze),
            GetSpell(Spell::FrostNova), GetSpell(Spell::ConeOfCold),
            GetSpell(Spell::Blizzard), GetSpell(Spell::SummonWaterElemental),
            GetSpell(Spell::IceBlock), GetSpell(Spell::MageWard),
            GetSpell(Spell::Blink));
    }

    Unit* target = ObjectAccessor::GetUnit(*bot, master->GetTarget());
    if (target && (!target->IsAlive() || !bot->IsValidAttackTarget(target)))
        target = nullptr;

    PetMaintenance(bot);
    DoMaintenance(bot);

    if (!master->IsInCombat())
        return;

    // Positioning is handled by AltbotPositionManager after Update returns.
    // Strategy declares its intent here (anchor + LOS + leash); the manager
    // runs LOS recovery, idle-pack avoidance, leash check, and chase.
    if (target && ctx.positionManager)
        ctx.positionManager->SetIntent(
            AltbotPositionManager::MakeRangedDpsIntent(target, master));
    else if (ctx.positionManager)
        ctx.positionManager->SetIntent(AltbotPositionManager::MakeFollowIntent());

    if (DoDefensives(bot)) { TC_LOG_INFO("altbot", "FrostMage[%s] tick: defensive", bot->GetName().c_str()); return; }

    if (ctx.combatElapsedMs < THREAT_WINDOW_MS)
    {
        TC_LOG_INFO("altbot", "FrostMage[%s] tick: threat-window (%u<%u)",
                    bot->GetName().c_str(), ctx.combatElapsedMs, THREAT_WINDOW_MS);
        return;
    }

    if (!target)
    {
        TC_LOG_INFO("altbot", "FrostMage[%s] tick: no target", bot->GetName().c_str());
        return;
    }

    if (!bot->IsInCombat())
        bot->Attack(target, false);

    bool casting = bot->HasUnitState(UNIT_STATE_CASTING);
    TC_LOG_INFO("altbot",
        "FrostMage[%s] tick: tgt='%s' dist=%.1f casting=%d FoF=%d BF=%d "
        "FB-id=%u IL-id=%u FFB-id=%u",
        bot->GetName().c_str(), target->GetName().c_str(),
        bot->GetDistance(target), casting ? 1 : 0,
        HasFingersOfFrost(bot) ? 1 : 0, HasBrainFreeze(bot) ? 1 : 0,
        GetSpell(Spell::Frostbolt), GetSpell(Spell::IceLance),
        GetSpell(Spell::FrostfireBolt));

    if (casting)
        return;

    // Skip the rotation while the GCD is active — every tier would otherwise
    // attempt CastSpell and reject with SPELL_FAILED_NOT_READY (69). Probe
    // with Frostbolt (always known on a frost mage; standard GCD category).
    if (uint32 fb = GetSpell(Spell::Frostbolt))
    {
        SpellInfo const* fbInfo = sSpellMgr->GetSpellInfo(fb);
        if (fbInfo && bot->GetSpellHistory()->HasGlobalCooldown(fbInfo))
            return;
    }

    if (Tier_DeepFreeze(bot, target))      { TC_LOG_DEBUG("altbot", "  -> DeepFreeze");    return; }
    if (Tier_FFB_BothProcs(bot, target))   { TC_LOG_DEBUG("altbot", "  -> FFB+BothProcs"); return; }
    if (Tier_FFB_BrainFreeze(bot, target)) { TC_LOG_DEBUG("altbot", "  -> FFB+BF");        return; }
    if (Tier_IceLance(bot, target))        { TC_LOG_DEBUG("altbot", "  -> IceLance");     return; }
    if (Tier_AoE(bot, target))             { TC_LOG_DEBUG("altbot", "  -> AoE");          return; }
    if (Tier_Frostbolt(bot, target))       { TC_LOG_DEBUG("altbot", "  -> Frostbolt");    return; }

    TC_LOG_INFO("altbot", "  -> NO TIER FIRED");
    AltbotCombatLog::OnNoTierFired(bot, "FrostMage");
}

void FrostMageStrategy::ResolveSpellCache(Player* bot)
{
    _cache.fill(0);
    auto find = [bot](char const* name) {
        return StrategyUtil::FindSpellByFamilyName(bot, SPELLFAMILY_MAGE, name);
    };

    _cache[size_t(Spell::MoltenArmor)]          = find("Molten Armor");
    _cache[size_t(Spell::MageArmor)]            = find("Mage Armor");
    _cache[size_t(Spell::FrostArmor)]           = find("Frost Armor");
    _cache[size_t(Spell::Frostbolt)]            = find("Frostbolt");
    _cache[size_t(Spell::FrostfireBolt)]        = find("Frostfire Bolt");
    _cache[size_t(Spell::Fireball)]             = find("Fireball");
    _cache[size_t(Spell::IceLance)]             = find("Ice Lance");
    _cache[size_t(Spell::DeepFreeze)]           = find("Deep Freeze");
    _cache[size_t(Spell::FrostNova)]            = find("Frost Nova");
    _cache[size_t(Spell::ConeOfCold)]           = find("Cone of Cold");
    _cache[size_t(Spell::Blizzard)]             = find("Blizzard");
    _cache[size_t(Spell::SummonWaterElemental)] = find("Summon Water Elemental");
    _cache[size_t(Spell::IceBlock)]             = find("Ice Block");
    _cache[size_t(Spell::MageWard)]             = find("Mage Ward");
    _cache[size_t(Spell::Blink)]                = find("Blink");

    // Proc auras (Brain Freeze, Fingers of Frost) aren't cacheable here —
    // they aren't in the bot's spellbook, only on `bot->GetAppliedAuras()`
    // when actually procced. The proc-checking tiers walk the applied-aura
    // map by name on each tick.
}

bool FrostMageStrategy::IsOnCooldown(Player* bot, uint32 spellId) const
{
    if (!spellId)
        return true;
    SpellInfo const* info = sSpellMgr->GetSpellInfo(spellId);
    if (!info)
        return true;
    return bot->GetSpellHistory()->HasCooldown(info);
}

bool FrostMageStrategy::TryCast(Player* bot, Unit* target, Spell s) const
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

void FrostMageStrategy::PetMaintenance(Player* bot)
{
    Pet* pet = bot->GetPet();
    if (pet && pet->IsAlive())
        return;

    uint32 summon = GetSpell(Spell::SummonWaterElemental);
    if (!summon || IsOnCooldown(bot, summon))
        return;
    if (bot->IsInCombat())
        return;
    StrategyUtil::CastWithLog(bot, bot, summon, SPEC_LABEL);
}

void FrostMageStrategy::DoMaintenance(Player* bot)
{
    // Default armor: Molten (PvE crit). Fall back to Mage Armor if Molten is
    // unknown, then Frost Armor below it. Don't reapply if any mage armor is
    // already up — switching is a deliberate decision (Mage Ward expiry,
    // mana stress) and we only run the default at startup.
    uint32 molten = GetSpell(Spell::MoltenArmor);
    uint32 mage   = GetSpell(Spell::MageArmor);
    uint32 frost  = GetSpell(Spell::FrostArmor);

    bool anyArmor = (molten && bot->HasAura(molten))
                 || (mage   && bot->HasAura(mage))
                 || (frost  && bot->HasAura(frost));

    if (!anyArmor)
    {
        uint32 prefer = molten ? molten : (mage ? mage : frost);
        if (prefer)
            StrategyUtil::CastWithLog(bot, bot, prefer, SPEC_LABEL);
    }

    // Mage Ward: kept up always (30s CD, always-better damage absorption).
    uint32 ward = GetSpell(Spell::MageWard);
    if (ward && !bot->HasAura(ward) && !IsOnCooldown(bot, ward))
        StrategyUtil::CastWithLog(bot, bot, ward, SPEC_LABEL);
}

bool FrostMageStrategy::DoDefensives(Player* bot)
{
    // Ice Block panic — full immunity at low HP.
    uint32 ib = GetSpell(Spell::IceBlock);
    if (ib && bot->GetHealthPct() < ICE_BLOCK_HP_PCT && !IsOnCooldown(bot, ib))
    {
        StrategyUtil::CastWithLog(bot, bot, ib, SPEC_LABEL);
        return true;
    }

    // Frost Nova → Blink chain only when actually pressured: multiple
    // meleers AND low HP. A single mob at high HP is a non-event — never
    // displace the bot reflexively, since stack mechanics (Bronjahm
    // Soulstorm, etc.) require the bot to stay with the group. The
    // pushback while casting through close-range single-target attackers
    // is intended; the rotation handles it (and Cone of Cold cycles as
    // part of Tier_AoE when the cluster check passes).
    int meleeNear = AltbotPosition::CountHostilesNear(bot, MELEE_NEAR_RADIUS);
    if (meleeNear >= MELEE_TRIGGER_COUNT && bot->GetHealthPct() < ESCAPE_HP_PCT)
    {
        uint32 fn    = GetSpell(Spell::FrostNova);
        uint32 blink = GetSpell(Spell::Blink);
        if (fn && !IsOnCooldown(bot, fn))
        {
            StrategyUtil::CastWithLog(bot, bot, fn, SPEC_LABEL);
            if (blink && !IsOnCooldown(bot, blink))
                StrategyUtil::CastWithLog(bot, bot, blink, SPEC_LABEL);
            return true;
        }
    }

    return false;
}

bool FrostMageStrategy::Tier_DeepFreeze(Player* bot, Unit* target) const
{
    uint32 df = GetSpell(Spell::DeepFreeze);
    if (!df || IsOnCooldown(bot, df))
        return false;
    if (!HasFingersOfFrost(bot))
        return false;
    return TryCast(bot, target, Spell::DeepFreeze);
}

bool FrostMageStrategy::Tier_FFB_BothProcs(Player* bot, Unit* target) const
{
    if (!GetSpell(Spell::FrostfireBolt))
        return false;
    if (!HasFingersOfFrost(bot) || !HasBrainFreeze(bot))
        return false;
    return TryCast(bot, target, Spell::FrostfireBolt);
}

bool FrostMageStrategy::Tier_FFB_BrainFreeze(Player* bot, Unit* target) const
{
    if (!HasBrainFreeze(bot))
        return false;
    // Prefer FFB; fall back to Fireball if FFB unknown (Brain Freeze allows both).
    // Consume the proc as soon as it's up — sitting on Brain Freeze is a flat
    // DPS loss, the proc has no special-case timing.
    if (GetSpell(Spell::FrostfireBolt))
        return TryCast(bot, target, Spell::FrostfireBolt);
    if (GetSpell(Spell::Fireball))
        return TryCast(bot, target, Spell::Fireball);
    return false;
}

bool FrostMageStrategy::Tier_IceLance(Player* bot, Unit* target) const
{
    if (!GetSpell(Spell::IceLance))
        return false;
    if (!HasFingersOfFrost(bot))
        return false;
    return TryCast(bot, target, Spell::IceLance);
}

bool FrostMageStrategy::Tier_AoE(Player* bot, Unit* target) const
{
    // Count clusters around the target — at 25y range, the bot's own
    // neighborhood is empty, so an around-bot count never trips on a
    // ranged caster.
    int nearCount = AltbotPosition::CountHostilesNearUnit(bot, target, AOE_RADIUS);
    if (nearCount < AOE_MIN_TARGETS)
        return false;

    // Cone of Cold prefers melee-range cluster. Use TryCast so a failure
    // (LoS, target moved out of cone) falls through to Blizzard / Frostbolt
    // instead of consuming the tier.
    if (bot->GetDistance(target) <= CONE_RANGE && TryCast(bot, target, Spell::ConeOfCold))
        return true;

    // Blizzard channel for sustained AoE
    if (TryCast(bot, target, Spell::Blizzard))
        return true;

    return false;
}

bool FrostMageStrategy::Tier_Frostbolt(Player* bot, Unit* target) const
{
    return TryCast(bot, target, Spell::Frostbolt);
}

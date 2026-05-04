#include "AffWarlockStrategy.h"
#include "AltbotPosition.h"
#include "AltbotPositionManager.h"
#include "AltbotTickContext.h"
#include "Log.h"
#include "ObjectAccessor.h"
#include "Pet.h"
#include "Player.h"
#include "SharedDefines.h"
#include "SpellAuras.h"
#include "SpellHistory.h"
#include "SpellInfo.h"
#include "SpellMgr.h"
#include "StrategyUtil.h"
#include "Timer.h"
#include "Unit.h"

namespace
{
    constexpr uint32 THREAT_WINDOW_MS = 2000;
    constexpr uint32 SOC_REUSE_MS     = 12 * IN_MILLISECONDS;

    constexpr float MANA_NORMAL_THRESHOLD = 50.0f;
    constexpr float MANA_CRISIS_THRESHOLD = 25.0f;

    constexpr float CASTER_RANGE         = 25.0f;
    constexpr float MELEE_NEAR_RADIUS    = 8.0f;
    constexpr float HOWL_RADIUS          = 10.0f;
    constexpr int   MELEE_ESCAPE_COUNT   = 2;
    constexpr float ESCAPE_HP_PCT        = 60.0f;
    constexpr float DEATH_COIL_HP_PCT    = 35.0f;
    constexpr float LIFE_TAP_HP_FLOOR    = 60.0f;
    constexpr float DRAIN_SOUL_EXEC_PCT  = 25.0f;
    constexpr int32 BANE_REFRESH_REMAIN  = 30 * IN_MILLISECONDS;

    constexpr float AOE_RADIUS           = 10.0f;
    constexpr int   AOE_MIN_TARGETS      = 3;

    constexpr uint32 PET_HEALTH_FUNNEL_PCT = 50;

    constexpr char const* SPEC_LABEL = "AffWarlock";
}

void AffWarlockStrategy::Update(Player* bot, Player* master, AltbotTickContext const& ctx)
{
    if (!_cacheResolved)
    {
        ResolveSpellCache(bot);
        _cacheResolved = true;

        TC_LOG_DEBUG("altbot",
            "AffWarlockStrategy cache for '%s': "
            "FelArmor=%u DemonArmor=%u CoE=%u Haunt=%u BaneDoom=%u BaneAgony=%u "
            "Corruption=%u UA=%u SB=%u DrainSoul=%u DrainLife=%u LifeTap=%u SoC=%u "
            "DCSummon=%u DCTeleport=%u HoT=%u DeathCoil=%u Felhunter=%u HealthFunnel=%u",
            bot->GetName().c_str(),
            GetSpell(Spell::FelArmor), GetSpell(Spell::DemonArmor),
            GetSpell(Spell::CurseOfTheElements), GetSpell(Spell::Haunt),
            GetSpell(Spell::BaneOfDoom), GetSpell(Spell::BaneOfAgony),
            GetSpell(Spell::Corruption), GetSpell(Spell::UnstableAffliction),
            GetSpell(Spell::ShadowBolt), GetSpell(Spell::DrainSoul),
            GetSpell(Spell::DrainLife), GetSpell(Spell::LifeTap),
            GetSpell(Spell::SeedOfCorruption), GetSpell(Spell::DemonicCircleSummon),
            GetSpell(Spell::DemonicCircleTeleport), GetSpell(Spell::HowlOfTerror),
            GetSpell(Spell::DeathCoil), GetSpell(Spell::SummonFelhunter),
            GetSpell(Spell::HealthFunnel));
    }

    // Resolve a target via master's victim. If master has no target, we still
    // run maintenance (armor, pet) but the rotation tiers will short-circuit.
    Unit* target = ObjectAccessor::GetUnit(*bot, master->GetTarget());
    if (target && (!target->IsAlive() || !bot->IsValidAttackTarget(target)))
        target = nullptr;

    PetMaintenance(bot);
    DoMaintenance(bot, target);

    // Combat-only: positioning + defensives + threat-window gate + rotation.
    // Out of combat we still maintain Fel Armor / pet / Demonic Circle anchor.
    if (!master->IsInCombat())
        return;

    // Positioning is owned by AltbotPositionManager (called from
    // AltbotCombat::Update after Update returns). Strategy declares its
    // intent here; manager handles LOS recovery, idle-pack avoidance, leash.
    if (target && ctx.positionManager)
        ctx.positionManager->SetIntent(
            AltbotPositionManager::MakeRangedDpsIntent(target, master));
    else if (ctx.positionManager)
        ctx.positionManager->SetIntent(AltbotPositionManager::MakeFollowIntent());

    if (DoDefensives(bot))
        return;

    if (ctx.combatElapsedMs < THREAT_WINDOW_MS)
        return;

    if (!target)
        return;

    if (!bot->IsInCombat())
        bot->Attack(target, false);

    // Skip the rotation while a cast or channel is in progress — re-issuing a
    // cast every tick during a 2.5s Shadow Bolt or 5s Drain Soul produces
    // spurious SPELL_FAILED_SPELL_IN_PROGRESS attempts and stops the tier
    // chain from descending to the next tier on the next tick.
    if (bot->HasUnitState(UNIT_STATE_CASTING) || bot->IsNonMeleeSpellCast(false))
        return;

    // Skip while the GCD is active — every tier would otherwise attempt
    // CastSpell and reject with SPELL_FAILED_NOT_READY (69). Probe with
    // Shadow Bolt (always known on a warlock; standard GCD category).
    if (uint32 sb = GetSpell(Spell::ShadowBolt))
    {
        SpellInfo const* sbInfo = sSpellMgr->GetSpellInfo(sb);
        if (sbInfo && bot->GetSpellHistory()->HasGlobalCooldown(sbInfo))
            return;
    }

    ManaMode mode = GetManaMode(bot);

    if (Tier_Haunt(bot, target))               return;
    if (Tier_BaneOfDoom(bot, target))          return;
    if (Tier_Corruption(bot, target))          return;
    if (Tier_UnstableAffliction(bot, target))  return;
    if (Tier_AoE(bot, target))                 return;
    if (Tier_DrainSoul(bot, target))           return;
    Tier_ShadowBolt(bot, target, mode);
}

void AffWarlockStrategy::ResolveSpellCache(Player* bot)
{
    _cache.fill(0);
    auto find = [bot](char const* name) {
        return StrategyUtil::FindSpellByFamilyName(bot, SPELLFAMILY_WARLOCK, name);
    };

    _cache[size_t(Spell::FelArmor)]              = find("Fel Armor");
    _cache[size_t(Spell::DemonArmor)]            = find("Demon Armor");
    _cache[size_t(Spell::CurseOfTheElements)]    = find("Curse of the Elements");
    _cache[size_t(Spell::Haunt)]                 = find("Haunt");
    _cache[size_t(Spell::BaneOfDoom)]            = find("Bane of Doom");
    _cache[size_t(Spell::BaneOfAgony)]           = find("Bane of Agony");
    _cache[size_t(Spell::Corruption)]            = find("Corruption");
    _cache[size_t(Spell::UnstableAffliction)]    = find("Unstable Affliction");
    _cache[size_t(Spell::ShadowBolt)]            = find("Shadow Bolt");
    _cache[size_t(Spell::DrainSoul)]             = find("Drain Soul");
    _cache[size_t(Spell::DrainLife)]             = find("Drain Life");
    _cache[size_t(Spell::LifeTap)]               = find("Life Tap");
    _cache[size_t(Spell::SeedOfCorruption)]      = find("Seed of Corruption");
    _cache[size_t(Spell::DemonicCircleSummon)]   = find("Demonic Circle: Summon");
    _cache[size_t(Spell::DemonicCircleTeleport)] = find("Demonic Circle: Teleport");
    _cache[size_t(Spell::HowlOfTerror)]          = find("Howl of Terror");
    _cache[size_t(Spell::DeathCoil)]             = find("Death Coil");
    _cache[size_t(Spell::SummonFelhunter)]       = find("Summon Felhunter");
    _cache[size_t(Spell::HealthFunnel)]          = find("Health Funnel");
}

AffWarlockStrategy::ManaMode AffWarlockStrategy::GetManaMode(Player* bot) const
{
    uint32 maxMana = bot->GetMaxPower(POWER_MANA);
    if (!maxMana)
        return ManaMode::Normal;
    float pct = 100.0f * float(bot->GetPower(POWER_MANA)) / float(maxMana);
    if (pct < MANA_CRISIS_THRESHOLD) return ManaMode::Crisis;
    if (pct < MANA_NORMAL_THRESHOLD) return ManaMode::Conserve;
    return ManaMode::Normal;
}

bool AffWarlockStrategy::IsOnCooldown(Player* bot, uint32 spellId) const
{
    if (!spellId)
        return true;
    SpellInfo const* info = sSpellMgr->GetSpellInfo(spellId);
    if (!info)
        return true;
    return bot->GetSpellHistory()->HasCooldown(info);
}

bool AffWarlockStrategy::TryCast(Player* bot, Unit* target, Spell s) const
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

void AffWarlockStrategy::PetMaintenance(Player* bot)
{
    Pet* pet = bot->GetPet();

    if (!pet || !pet->IsAlive())
    {
        // Out of combat only — summoning is a 6s cast and won't resolve mid-fight.
        if (!bot->IsInCombat() && GetSpell(Spell::SummonFelhunter)
            && !IsOnCooldown(bot, GetSpell(Spell::SummonFelhunter)))
        {
            StrategyUtil::CastWithLog(bot, bot, GetSpell(Spell::SummonFelhunter), SPEC_LABEL);
        }
        return;
    }

    // Health Funnel keeps the pet alive at low HP. Channel only when bot
    // itself is healthy enough to spare the HP cost.
    uint32 funnel = GetSpell(Spell::HealthFunnel);
    if (funnel && pet->GetHealthPct() < float(PET_HEALTH_FUNNEL_PCT)
        && bot->GetHealthPct() > LIFE_TAP_HP_FLOOR
        && !IsOnCooldown(bot, funnel)
        && !bot->IsNonMeleeSpellCast(false))
    {
        StrategyUtil::CastWithLog(bot, pet, funnel, SPEC_LABEL);
    }
}

void AffWarlockStrategy::DoMaintenance(Player* bot, Unit* target)
{
    // Armor: prefer Fel Armor; fall back to Demon Armor if Fel unknown.
    uint32 fel  = GetSpell(Spell::FelArmor);
    uint32 demo = GetSpell(Spell::DemonArmor);
    if (fel)
    {
        if (!bot->HasAura(fel))
            TryCast(bot, bot, Spell::FelArmor);
    }
    else if (demo && !bot->HasAura(demo))
    {
        TryCast(bot, bot, Spell::DemonArmor);
    }

    // Demonic Circle anchor — drop it when not already active so the teleport
    // defensive has somewhere to recall to.
    uint32 dcs = GetSpell(Spell::DemonicCircleSummon);
    if (dcs && !bot->HasAura(dcs) && !bot->IsInCombat() && !IsOnCooldown(bot, dcs))
        TryCast(bot, bot, Spell::DemonicCircleSummon);

    // Curse of the Elements on target if missing (debuff stacks with other
    // group magic-vuln effects but doesn't double-apply from same caster).
    if (target && bot->GetDistance(target) <= 40.0f)
    {
        uint32 coe = GetSpell(Spell::CurseOfTheElements);
        if (coe && !target->HasAura(coe, bot->GetGUID()))
            TryCast(bot, target, Spell::CurseOfTheElements);
    }
}

bool AffWarlockStrategy::DoDefensives(Player* bot)
{
    int meleeNear = AltbotPosition::CountHostilesNear(bot, MELEE_NEAR_RADIUS);
    int howlNear  = AltbotPosition::CountHostilesNear(bot, HOWL_RADIUS);

    // Demonic Circle Teleport: escape when meleed AND low HP. Requires the
    // anchor aura to exist (DCSummon already on bot).
    uint32 dct = GetSpell(Spell::DemonicCircleTeleport);
    uint32 dcs = GetSpell(Spell::DemonicCircleSummon);
    if (dct && dcs && bot->HasAura(dcs)
        && meleeNear >= MELEE_ESCAPE_COUNT
        && bot->GetHealthPct() < ESCAPE_HP_PCT
        && !IsOnCooldown(bot, dct))
    {
        StrategyUtil::CastWithLog(bot, bot, dct, SPEC_LABEL);
        return true;
    }

    // Howl of Terror: AoE fear when surrounded + low HP.
    uint32 hot = GetSpell(Spell::HowlOfTerror);
    if (hot && howlNear >= MELEE_ESCAPE_COUNT
        && bot->GetHealthPct() < ESCAPE_HP_PCT
        && !IsOnCooldown(bot, hot))
    {
        StrategyUtil::CastWithLog(bot, bot, hot, SPEC_LABEL);
        return true;
    }

    // Death Coil: emergency self-heal + 3s horror at low HP.
    uint32 dc = GetSpell(Spell::DeathCoil);
    if (dc && bot->GetHealthPct() < DEATH_COIL_HP_PCT && !IsOnCooldown(bot, dc))
    {
        if (Unit* victim = bot->GetVictim())
        {
            StrategyUtil::CastWithLog(bot, victim, dc, SPEC_LABEL);
            return true;
        }
    }

    return false;
}

bool AffWarlockStrategy::Tier_Haunt(Player* bot, Unit* target) const
{
    uint32 id = GetSpell(Spell::Haunt);
    if (!id || IsOnCooldown(bot, id))
        return false;
    if (target->HasAura(id, bot->GetGUID()))
        return false;
    return TryCast(bot, target, Spell::Haunt);
}

bool AffWarlockStrategy::Tier_BaneOfDoom(Player* bot, Unit* target) const
{
    // Prefer Bane of Doom (60s) over Bane of Agony (24s) when both are known.
    uint32 doom  = GetSpell(Spell::BaneOfDoom);
    uint32 agony = GetSpell(Spell::BaneOfAgony);
    Spell  slot  = doom ? Spell::BaneOfDoom : Spell::BaneOfAgony;
    uint32 id    = doom ? doom : agony;
    if (!id)
        return false;

    Aura* existing = target->GetAura(id, bot->GetGUID());
    if (existing && existing->GetDuration() > BANE_REFRESH_REMAIN)
        return false;

    return TryCast(bot, target, slot);
}

bool AffWarlockStrategy::Tier_Corruption(Player* bot, Unit* target) const
{
    uint32 id = GetSpell(Spell::Corruption);
    if (!id)
        return false;
    if (target->HasAura(id, bot->GetGUID()))
        return false;
    return TryCast(bot, target, Spell::Corruption);
}

bool AffWarlockStrategy::Tier_UnstableAffliction(Player* bot, Unit* target) const
{
    uint32 id = GetSpell(Spell::UnstableAffliction);
    if (!id)
        return false;
    if (target->HasAura(id, bot->GetGUID()))
        return false;
    return TryCast(bot, target, Spell::UnstableAffliction);
}

bool AffWarlockStrategy::Tier_AoE(Player* bot, Unit* target)
{
    uint32 seed = GetSpell(Spell::SeedOfCorruption);
    if (!seed)
        return false;
    if (target->HasAura(seed, bot->GetGUID()))
        return false;

    // Cluster check is around the target, not the bot — at 25y caster range
    // the bot's own neighborhood is empty even when the tank is sitting in a
    // 4-mob pull.
    int nearCount = AltbotPosition::CountHostilesNearUnit(bot, target, AOE_RADIUS);
    if (nearCount < AOE_MIN_TARGETS)
        return false;

    // Don't immediately recast after the seed explodes. Without this gate the
    // bot loops SoC every 2-3s on the same pack and never gets back to BoD /
    // Corruption / UA before the mobs die.
    uint32 now = getMSTime();
    if (_lastSoCMs && getMSTimeDiff(_lastSoCMs, now) < SOC_REUSE_MS)
        return false;

    if (!TryCast(bot, target, Spell::SeedOfCorruption))
        return false;
    _lastSoCMs = now;
    return true;
}

bool AffWarlockStrategy::Tier_DrainSoul(Player* bot, Unit* target) const
{
    uint32 id = GetSpell(Spell::DrainSoul);
    if (!id)
        return false;
    if (target->GetHealthPct() >= DRAIN_SOUL_EXEC_PCT)
        return false;
    return TryCast(bot, target, Spell::DrainSoul);
}

bool AffWarlockStrategy::Tier_ShadowBolt(Player* bot, Unit* target, ManaMode mode) const
{
    if (mode == ManaMode::Crisis)
    {
        // Crisis: refill mana via Life Tap if HP allows; otherwise filler.
        uint32 lt = GetSpell(Spell::LifeTap);
        if (lt && bot->GetHealthPct() > LIFE_TAP_HP_FLOOR && !IsOnCooldown(bot, lt))
        {
            StrategyUtil::CastWithLog(bot, bot, lt, SPEC_LABEL);
            return true;
        }
    }
    return TryCast(bot, target, Spell::ShadowBolt);
}

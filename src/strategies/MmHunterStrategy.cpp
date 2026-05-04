#include "MmHunterStrategy.h"
#include "AltbotPosition.h"
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
    constexpr float MELEE_NEAR_RADIUS   = 5.0f;
    constexpr int   DISENGAGE_TRIGGER   = 1;

    constexpr float AOE_RADIUS          = 8.0f;
    constexpr int   AOE_MIN_TARGETS     = 3;

    constexpr float DETERRENCE_HP_PCT   = 35.0f;
    constexpr float FEIGN_DEATH_HP_PCT  = 25.0f;
    constexpr float KILL_SHOT_HP_PCT    = 20.0f;
    constexpr float CAREFUL_AIM_HP_PCT  = 90.0f;

    constexpr int32 PET_MEND_HP_PCT     = 60;

    constexpr int32 AIMED_SHOT_FOCUS    = 65;
    constexpr int32 ARCANE_SHOT_FOCUS   = 50;
    constexpr int32 MULTI_SHOT_FOCUS    = 40;
    constexpr int32 STEADY_SHOT_FOCUS   = 0;

    constexpr float HUNTERS_MARK_HP_FLOOR = 1000000.0f;
}

void MmHunterStrategy::Update(Player* bot, Player* master, AltbotTickContext const& ctx)
{
    if (!_cacheResolved)
    {
        ResolveSpellCache(bot);
        _cacheResolved = true;

        TC_LOG_DEBUG("altbot",
            "MmHunterStrategy cache for '%s': "
            "Hawk=%u Mark=%u Serpent=%u Chimera=%u Aimed=%u Kill=%u Arcane=%u "
            "Steady=%u Multi=%u Misdirection=%u MendPet=%u CallPet=%u "
            "Disengage=%u FeignDeath=%u Deterrence=%u",
            bot->GetName().c_str(),
            GetSpell(Spell::AspectOfTheHawk), GetSpell(Spell::HuntersMark),
            GetSpell(Spell::SerpentSting), GetSpell(Spell::ChimeraShot),
            GetSpell(Spell::AimedShot), GetSpell(Spell::KillShot),
            GetSpell(Spell::ArcaneShot), GetSpell(Spell::SteadyShot),
            GetSpell(Spell::MultiShot), GetSpell(Spell::Misdirection),
            GetSpell(Spell::MendPet), GetSpell(Spell::CallPet1),
            GetSpell(Spell::Disengage), GetSpell(Spell::FeignDeath),
            GetSpell(Spell::Deterrence));
    }

    Unit* target = ObjectAccessor::GetUnit(*bot, master->GetTarget());
    if (target && (!target->IsAlive() || !bot->IsValidAttackTarget(target)))
        target = nullptr;

    PetMaintenance(bot);
    DoMaintenance(bot, master, target);

    if (!master->IsInCombat())
        return;

    if (target)
        AltbotPosition::MaintainRange(bot, target, CASTER_RANGE);

    // Misdirection on tank fires regardless of threat-window gate — it's
    // exactly the tool meant for this window. Defensives also pre-empt
    // the gate (survival > waiting for the tank).
    DoMisdirection(bot, master);

    if (DoDefensives(bot))
        return;

    if (ctx.combatElapsedMs < THREAT_WINDOW_MS)
        return;

    if (!target)
        return;

    if (!bot->IsInCombat())
        bot->Attack(target, false);

    // Skip the rotation while a cast is in progress — Steady Shot (2s) and
    // Aimed Shot (2.4s) are both castable, and re-issuing every tick produces
    // spurious SPELL_FAILED_SPELL_IN_PROGRESS attempts that stop the tier
    // chain from descending on the next tick.
    if (bot->HasUnitState(UNIT_STATE_CASTING) || bot->IsNonMeleeSpellCast(false))
        return;

    // Skip while the GCD is active — every tier would otherwise attempt
    // CastSpell and reject with SPELL_FAILED_NOT_READY (69). Probe with
    // Steady Shot (always known on a hunter; standard GCD category).
    if (uint32 ss = GetSpell(Spell::SteadyShot))
    {
        SpellInfo const* ssInfo = sSpellMgr->GetSpellInfo(ss);
        if (ssInfo && bot->GetSpellHistory()->HasGlobalCooldown(ssInfo))
            return;
    }

    if (Tier_AimedShotProc(bot, target))    return;
    if (Tier_SerpentSting(bot, target))     return;
    if (Tier_ChimeraShot(bot, target))      return;
    if (Tier_KillShot(bot, target))         return;
    if (Tier_AoE(bot, target))              return;
    if (Tier_AimedShotHard(bot, target))    return;
    if (Tier_ArcaneShot(bot, target))       return;
    Tier_SteadyShot(bot, target);
}

void MmHunterStrategy::ResolveSpellCache(Player* bot)
{
    _cache.fill(0);
    auto find = [bot](char const* name) {
        return StrategyUtil::FindSpellByFamilyName(bot, SPELLFAMILY_HUNTER, name);
    };

    _cache[size_t(Spell::AspectOfTheHawk)] = find("Aspect of the Hawk");
    _cache[size_t(Spell::HuntersMark)]     = find("Hunter's Mark");
    _cache[size_t(Spell::SerpentSting)]    = find("Serpent Sting");
    _cache[size_t(Spell::ChimeraShot)]     = find("Chimera Shot");
    _cache[size_t(Spell::AimedShot)]       = find("Aimed Shot");
    _cache[size_t(Spell::KillShot)]        = find("Kill Shot");
    _cache[size_t(Spell::ArcaneShot)]      = find("Arcane Shot");
    _cache[size_t(Spell::SteadyShot)]      = find("Steady Shot");
    _cache[size_t(Spell::MultiShot)]       = find("Multi-Shot");
    _cache[size_t(Spell::Misdirection)]    = find("Misdirection");
    _cache[size_t(Spell::MendPet)]         = find("Mend Pet");
    _cache[size_t(Spell::CallPet1)]        = find("Call Pet 1");
    _cache[size_t(Spell::Disengage)]       = find("Disengage");
    _cache[size_t(Spell::FeignDeath)]      = find("Feign Death");
    _cache[size_t(Spell::Deterrence)]      = find("Deterrence");
}

bool MmHunterStrategy::IsOnCooldown(Player* bot, uint32 spellId) const
{
    if (!spellId)
        return true;
    SpellInfo const* info = sSpellMgr->GetSpellInfo(spellId);
    if (!info)
        return true;
    return bot->GetSpellHistory()->HasCooldown(info);
}

bool MmHunterStrategy::TryCast(Player* bot, Unit* target, Spell s) const
{
    uint32 id = GetSpell(s);
    if (!id || !target)
        return false;
    SpellInfo const* info = sSpellMgr->GetSpellInfo(id);
    if (!info)
        return false;
    if (bot->GetSpellHistory()->HasCooldown(info))
        return false;
    SpellCastResult result = bot->CastSpell(target, id, false);
    if (result != SPELL_CAST_OK)
    {
        TC_LOG_INFO("altbot", "MmHunter[%s] CastSpell %u failed: SpellCastResult=%u",
                    bot->GetName().c_str(), id, uint32(result));
        return false;
    }
    return true;
}

void MmHunterStrategy::PetMaintenance(Player* bot)
{
    Pet* pet = bot->GetPet();

    if (!pet || !pet->IsAlive())
    {
        if (bot->IsInCombat())
            return;
        uint32 call = GetSpell(Spell::CallPet1);
        if (call && !IsOnCooldown(bot, call))
            bot->CastSpell(bot, call, false);
        return;
    }

    uint32 mend = GetSpell(Spell::MendPet);
    if (mend && pet->GetHealthPct() < float(PET_MEND_HP_PCT) && !IsOnCooldown(bot, mend))
        bot->CastSpell(pet, mend, false);
}

void MmHunterStrategy::DoMaintenance(Player* bot, Player* /*master*/, Unit* target)
{
    uint32 hawk = GetSpell(Spell::AspectOfTheHawk);
    if (hawk && !bot->HasAura(hawk))
        bot->CastSpell(bot, hawk, false);

    // Hunter's Mark on bosses (heuristic: very high max HP). Not on trash —
    // would clip GCD on every pull for no benefit.
    if (target && bot->IsInCombat())
    {
        uint32 mark = GetSpell(Spell::HuntersMark);
        if (mark && !target->HasAura(mark) && target->GetMaxHealth() > HUNTERS_MARK_HP_FLOOR)
            bot->CastSpell(target, mark, false);
    }
}

void MmHunterStrategy::DoMisdirection(Player* bot, Player* master)
{
    uint32 md = GetSpell(Spell::Misdirection);
    if (!md || IsOnCooldown(bot, md))
        return;

    Player* tank = StrategyUtil::FindTank(bot, master);
    if (!tank || !tank->IsAlive() || tank == bot)
        return;
    if (tank->HasAura(md, bot->GetGUID()))
        return;
    bot->CastSpell(tank, md, false);
}

bool MmHunterStrategy::DoDefensives(Player* bot)
{
    uint32 deter = GetSpell(Spell::Deterrence);
    if (deter && bot->GetHealthPct() < DETERRENCE_HP_PCT && !IsOnCooldown(bot, deter))
    {
        bot->CastSpell(bot, deter, false);
        return true;
    }

    uint32 fd = GetSpell(Spell::FeignDeath);
    if (fd && bot->GetHealthPct() < FEIGN_DEATH_HP_PCT && !IsOnCooldown(bot, fd))
    {
        bot->CastSpell(bot, fd, false);
        return true;
    }

    int meleeNear = AltbotPosition::CountHostilesNear(bot, MELEE_NEAR_RADIUS);
    uint32 dis = GetSpell(Spell::Disengage);
    if (dis && meleeNear >= DISENGAGE_TRIGGER && !IsOnCooldown(bot, dis))
    {
        bot->CastSpell(bot, dis, false);
        return true;
    }

    return false;
}

namespace
{
    Aura* FindHunterAuraByName(Player* bot, char const* name)
    {
        for (auto const& [spellId, app] : bot->GetAppliedAuras())
        {
            Aura* a = app->GetBase();
            if (!a)
                continue;
            SpellInfo const* info = a->GetSpellInfo();
            if (!info || !info->SpellName)
                continue;
            if (info->SpellFamilyName != SPELLFAMILY_HUNTER)
                continue;
            if (info->IsPassive())
                continue;
            if (std::strcmp(info->SpellName, name) == 0)
                return a;
        }
        return nullptr;
    }

    bool HasFireProc(Player* bot)
    {
        // Master Marksman 5-stack proc grants the "Fire!" buff for free instant
        // Aimed Shot. Match by name; the proc is in family Hunter.
        return FindHunterAuraByName(bot, "Fire!") != nullptr;
    }
}

bool MmHunterStrategy::Tier_AimedShotProc(Player* bot, Unit* target) const
{
    uint32 aimed = GetSpell(Spell::AimedShot);
    if (!aimed || IsOnCooldown(bot, aimed))
        return false;
    if (!HasFireProc(bot))
        return false;
    return TryCast(bot, target, Spell::AimedShot);
}

bool MmHunterStrategy::Tier_SerpentSting(Player* bot, Unit* target) const
{
    uint32 ss = GetSpell(Spell::SerpentSting);
    if (!ss)
        return false;
    if (target->HasAura(ss, bot->GetGUID()))
        return false;
    if (bot->GetPower(POWER_FOCUS) < ARCANE_SHOT_FOCUS)
        return false;
    return TryCast(bot, target, Spell::SerpentSting);
}

bool MmHunterStrategy::Tier_ChimeraShot(Player* bot, Unit* target) const
{
    uint32 cs = GetSpell(Spell::ChimeraShot);
    if (!cs || IsOnCooldown(bot, cs))
        return false;
    if (bot->GetPower(POWER_FOCUS) < ARCANE_SHOT_FOCUS)
        return false;
    return TryCast(bot, target, Spell::ChimeraShot);
}

bool MmHunterStrategy::Tier_KillShot(Player* bot, Unit* target) const
{
    uint32 ks = GetSpell(Spell::KillShot);
    if (!ks || IsOnCooldown(bot, ks))
        return false;
    if (target->GetHealthPct() > KILL_SHOT_HP_PCT)
        return false;
    return TryCast(bot, target, Spell::KillShot);
}

bool MmHunterStrategy::Tier_AoE(Player* bot, Unit* target) const
{
    // Cluster check is around the target — at 30y shot range the bot's own
    // neighborhood is empty, so an around-bot count never trips on a ranged DPS.
    int nearCount = AltbotPosition::CountHostilesNearUnit(bot, target, AOE_RADIUS);
    if (nearCount < AOE_MIN_TARGETS)
        return false;
    if (bot->GetPower(POWER_FOCUS) < MULTI_SHOT_FOCUS)
        return false;
    return TryCast(bot, target, Spell::MultiShot);
}

bool MmHunterStrategy::Tier_AimedShotHard(Player* bot, Unit* target) const
{
    uint32 aimed = GetSpell(Spell::AimedShot);
    if (!aimed || IsOnCooldown(bot, aimed))
        return false;
    if (bot->GetPower(POWER_FOCUS) < AIMED_SHOT_FOCUS)
        return false;
    if (target->GetHealthPct() < CAREFUL_AIM_HP_PCT)
        return false;
    return TryCast(bot, target, Spell::AimedShot);
}

bool MmHunterStrategy::Tier_ArcaneShot(Player* bot, Unit* target) const
{
    if (bot->GetPower(POWER_FOCUS) < ARCANE_SHOT_FOCUS)
        return false;
    return TryCast(bot, target, Spell::ArcaneShot);
}

bool MmHunterStrategy::Tier_SteadyShot(Player* bot, Unit* target) const
{
    (void)STEADY_SHOT_FOCUS;
    return TryCast(bot, target, Spell::SteadyShot);
}

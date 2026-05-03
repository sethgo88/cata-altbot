#include "FrostMageStrategy.h"
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
    constexpr float MELEE_NEAR_RADIUS   = 8.0f;
    constexpr float CONE_RANGE          = 8.0f;
    constexpr int   MELEE_TRIGGER_COUNT = 1;
    constexpr float ICE_BLOCK_HP_PCT    = 20.0f;

    constexpr float AOE_RADIUS          = 10.0f;
    constexpr int   AOE_MIN_TARGETS     = 3;

    constexpr int32 BRAIN_FREEZE_LOW_MS = 3000;
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

    if (target)
        AltbotPosition::MaintainRange(bot, target, CASTER_RANGE);

    if (DoDefensives(bot))
        return;

    if (ctx.combatElapsedMs < THREAT_WINDOW_MS)
        return;

    if (!target)
        return;

    if (!bot->IsInCombat())
        bot->Attack(target, false);

    if (Tier_DeepFreeze(bot, target))         return;
    if (Tier_FFB_BothProcs(bot, target))      return;
    if (Tier_FFB_BrainFreeze(bot, target))    return;
    if (Tier_IceLance(bot, target))           return;
    if (Tier_AoE(bot, target))                return;
    Tier_Frostbolt(bot, target);
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
    bot->CastSpell(target, id, false);
    return true;
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
    bot->CastSpell(bot, summon, false);
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
            bot->CastSpell(bot, prefer, false);
    }

    // Mage Ward: kept up always (30s CD, always-better damage absorption).
    uint32 ward = GetSpell(Spell::MageWard);
    if (ward && !bot->HasAura(ward) && !IsOnCooldown(bot, ward))
        bot->CastSpell(bot, ward, false);
}

bool FrostMageStrategy::DoDefensives(Player* bot)
{
    // Ice Block panic — full immunity at low HP.
    uint32 ib = GetSpell(Spell::IceBlock);
    if (ib && bot->GetHealthPct() < ICE_BLOCK_HP_PCT && !IsOnCooldown(bot, ib))
    {
        bot->CastSpell(bot, ib, false);
        return true;
    }

    int meleeNear = AltbotPosition::CountHostilesNear(bot, MELEE_NEAR_RADIUS);
    if (meleeNear < MELEE_TRIGGER_COUNT)
        return false;

    // Frost Nova → Blink chain. If Frost Nova is up, root the melee then jump.
    uint32 fn    = GetSpell(Spell::FrostNova);
    uint32 blink = GetSpell(Spell::Blink);
    if (fn && !IsOnCooldown(bot, fn))
    {
        bot->CastSpell(bot, fn, false);
        if (blink && !IsOnCooldown(bot, blink))
            bot->CastSpell(bot, blink, false);
        return true;
    }

    // Cone of Cold as a fallback slow when Frost Nova is on cooldown but
    // the bot's still meleed (also damages, so no rotation lost).
    uint32 coc = GetSpell(Spell::ConeOfCold);
    if (coc && !IsOnCooldown(bot, coc))
    {
        // Picks any frontal hostile in cone — pass bot itself for self-cast
        // origin; CoC applies to nearby enemies in front automatically.
        if (Unit* victim = bot->GetVictim())
            bot->CastSpell(victim, coc, false);
        return true;
    }

    return false;
}

namespace
{
    // Look up bot's aura by name (lazy resolution for proc auras whose IDs
    // weren't in the spellbook scan). Returns the aura if present.
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
            if (std::strcmp(info->SpellName, name) == 0)
                return a;
        }
        return nullptr;
    }

    bool HasFingersOfFrost(Player* bot) { return FindBotAuraByName(bot, "Fingers of Frost") != nullptr; }
    bool HasBrainFreeze   (Player* bot) { return FindBotAuraByName(bot, "Brain Freeze")    != nullptr; }
    int32 BrainFreezeLeft (Player* bot)
    {
        Aura* a = FindBotAuraByName(bot, "Brain Freeze");
        return a ? a->GetDuration() : 0;
    }
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
    int32 left = BrainFreezeLeft(bot);
    if (left <= 0 || left > BRAIN_FREEZE_LOW_MS)
        return false;
    // Prefer FFB; fall back to Fireball if FFB unknown (Brain Freeze allows both).
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
    int nearCount = AltbotPosition::CountHostilesNear(bot, AOE_RADIUS);
    if (nearCount < AOE_MIN_TARGETS)
        return false;

    // Cone of Cold prefers melee-range cluster
    uint32 coc = GetSpell(Spell::ConeOfCold);
    if (coc && !IsOnCooldown(bot, coc) && bot->GetDistance(target) <= CONE_RANGE)
    {
        bot->CastSpell(target, coc, false);
        return true;
    }

    // Blizzard channel for sustained AoE
    uint32 blz = GetSpell(Spell::Blizzard);
    if (blz && !IsOnCooldown(bot, blz))
    {
        bot->CastSpell(target, blz, false);
        return true;
    }
    return false;
}

bool FrostMageStrategy::Tier_Frostbolt(Player* bot, Unit* target) const
{
    return TryCast(bot, target, Spell::Frostbolt);
}

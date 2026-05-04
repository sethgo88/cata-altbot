#include "AltbotCombat.h"
#include "AltbotAssist.h"
#include "AltbotPositionManager.h"
#include "AltbotStrategy.h"
#include "AltbotTickContext.h"
#include "Log.h"
#include "Player.h"
#include "SharedDefines.h"
#include "SpellHistory.h"
#include "SpellInfo.h"
#include "SpellMgr.h"
#include "Timer.h"
#include "Unit.h"

static constexpr float HEAL_THRESHOLD_PCT = 70.0f;

namespace AltbotCombat
{

// Returns the highest-level healing spell the bot knows that is off cooldown.
// Never hardcodes spell IDs — uses effect type detection.
static uint32 FindBestHealSpell(Player* bot)
{
    uint32 bestSpell = 0;
    uint32 bestLevel = 0;

    for (auto const& [spellId, playerSpell] : bot->GetSpellMap())
    {
        // TC 4.3.4: PlayerSpell is a struct (not pointer), fields are lowercase
        if (playerSpell.state == PLAYERSPELL_REMOVED || !playerSpell.active)
            continue;

        SpellInfo const* info = sSpellMgr->GetSpellInfo(spellId);
        if (!info)
            continue;
        if (bot->GetSpellHistory()->HasCooldown(info))
            continue;

        bool isHeal = false;
        for (uint8 i = 0; i < MAX_SPELL_EFFECTS; ++i)
        {
            if (info->Effects[i].Effect == SPELL_EFFECT_HEAL ||
                info->Effects[i].ApplyAuraName == SPELL_AURA_PERIODIC_HEAL)
            {
                isHeal = true;
                break;
            }
        }

        if (!isHeal)
            continue;

        if (info->SpellLevel > bestLevel)
        {
            bestLevel = info->SpellLevel;
            bestSpell = spellId;
        }
    }

    return bestSpell;
}

// Returns the highest-level direct-damage spell the bot knows that is off cooldown.
static uint32 FindBestDamageSpell(Player* bot)
{
    uint32 bestSpell = 0;
    uint32 bestLevel = 0;

    for (auto const& [spellId, playerSpell] : bot->GetSpellMap())
    {
        // TC 4.3.4: PlayerSpell is a struct (not pointer), fields are lowercase
        if (playerSpell.state == PLAYERSPELL_REMOVED || !playerSpell.active)
            continue;

        SpellInfo const* info = sSpellMgr->GetSpellInfo(spellId);
        if (!info || info->IsPositive())
            continue;
        if (bot->GetSpellHistory()->HasCooldown(info))
            continue;

        bool isDamage = false;
        for (uint8 i = 0; i < MAX_SPELL_EFFECTS; ++i)
        {
            if (info->Effects[i].Effect == SPELL_EFFECT_SCHOOL_DAMAGE         ||
                info->Effects[i].Effect == SPELL_EFFECT_WEAPON_DAMAGE          ||
                info->Effects[i].Effect == SPELL_EFFECT_WEAPON_DAMAGE_NOSCHOOL ||
                info->Effects[i].Effect == SPELL_EFFECT_NORMALIZED_WEAPON_DMG)
            {
                isDamage = true;
                break;
            }
        }

        if (!isDamage)
            continue;

        if (info->SpellLevel > bestLevel)
        {
            bestLevel = info->SpellLevel;
            bestSpell = spellId;
        }
    }

    return bestSpell;
}

// Synthesize a default intent for bots without a spec-specific strategy.
// Caster classes get ranged-DPS positioning against master's victim; melee
// classes (and casters with no target) defer to follow logic.
static AltbotPosition::PositionIntent SynthesizeFallbackIntent(Player* bot, Player* master)
{
    Unit* target = master ? master->GetVictim() : nullptr;
    uint8 cls = bot->getClass();
    bool isCaster = (cls == CLASS_MAGE   || cls == CLASS_PRIEST  ||
                     cls == CLASS_WARLOCK || cls == CLASS_DRUID  ||
                     cls == CLASS_SHAMAN);
    if (isCaster && target)
        return AltbotPositionManager::MakeRangedDpsIntent(target, master);
    return AltbotPositionManager::MakeFollowIntent();
}

void Update(Player* bot, Player* master, AltbotState const& state,
            AltbotTickContext const& ctx, AltbotStrategy* strategy)
{
    // Only act when master is in combat
    if (!master->IsInCombat())
        return;

    uint32 nowMs = getMSTime();

    // A spec-specific strategy fully owns the tick when present; the generic
    // scan below is the fallback for specs without a strategy yet. Strategies
    // declare a PositionIntent inside Update; manager runs after to enact it.
    if (strategy)
    {
        strategy->Update(bot, master, ctx);
        if (ctx.positionManager)
            ctx.positionManager->Tick(master, ctx, nowMs);
        return;
    }

    // No strategy: synthesize a default intent and let the manager handle
    // positioning before the generic damage scan runs.
    if (ctx.positionManager)
    {
        ctx.positionManager->SetIntent(SynthesizeFallbackIntent(bot, master));
        ctx.positionManager->Tick(master, ctx, nowMs);
    }

    // Heal master if health is low
    if (master->GetHealthPct() < HEAL_THRESHOLD_PCT)
    {
        uint32 healSpell = FindBestHealSpell(bot);
        if (healSpell)
        {
            TC_LOG_DEBUG("altbot", "AltbotCombat: '%s' casting heal %u on '%s' (%.1f%% hp)",
                         bot->GetName().c_str(), healSpell,
                         master->GetName().c_str(), master->GetHealthPct());
            bot->CastSpell(master, healSpell, false);
            return;
        }
    }

    // DPS: target chosen via assist mode (skull / master's victim / off).
    Unit* target = AltbotAssist::SelectTarget(bot, master, state.assist);
    if (!target)
        return;

    if (!bot->IsInCombat())
        bot->Attack(target, true);

    uint32 dmgSpell = FindBestDamageSpell(bot);
    if (dmgSpell)
    {
        TC_LOG_DEBUG("altbot", "AltbotCombat: '%s' casting dmg %u on '%s'",
                     bot->GetName().c_str(), dmgSpell, target->GetName().c_str());
        bot->CastSpell(target, dmgSpell, false);
    }
}

} // namespace AltbotCombat

#include "AltbotMount.h"
#include "Log.h"
#include "Player.h"
#include "SharedDefines.h"
#include "SpellAuraDefines.h"
#include "SpellInfo.h"
#include "SpellMgr.h"

namespace AltbotMount
{

// Picks the bot's highest-level known mount whose mount-speed flag matches the
// master's current mount. We don't introspect the exact speed; we just prefer
// a flying mount when master is using a flying mount, otherwise ground.
static uint32 FindBestMountSpell(Player* bot, bool wantFlying)
{
    uint32 bestSpell = 0;
    uint32 bestLevel = 0;

    for (auto const& [spellId, ps] : bot->GetSpellMap())
    {
        if (ps->State == PLAYERSPELL_REMOVED || !ps->Active)
            continue;

        SpellInfo const* info = sSpellMgr->GetSpellInfo(spellId);
        if (!info)
            continue;

        bool isMount = false;
        bool isFly   = false;
        for (uint8 i = 0; i < MAX_SPELL_EFFECTS; ++i)
        {
            if (info->Effects[i].ApplyAuraName == SPELL_AURA_MOUNTED)
                isMount = true;
            if (info->Effects[i].ApplyAuraName == SPELL_AURA_MOD_FLIGHT_SPEED ||
                info->Effects[i].ApplyAuraName == SPELL_AURA_FLY ||
                info->Effects[i].ApplyAuraName == SPELL_AURA_MOD_INCREASE_VEHICLE_FLIGHT_SPEED)
                isFly = true;
        }

        if (!isMount)
            continue;
        if (wantFlying && !isFly)
            continue;
        if (!wantFlying && isFly)
            continue;   // ground mounts only when master isn't flying

        if (info->SpellLevel > bestLevel)
        {
            bestLevel = info->SpellLevel;
            bestSpell = spellId;
        }
    }

    return bestSpell;
}

void Tick(Player* bot, Player* master)
{
    bool masterMounted = master->IsMounted();
    bool botMounted    = bot->IsMounted();

    if (!masterMounted && botMounted)
    {
        bot->Dismount();
        bot->RemoveAurasByType(SPELL_AURA_MOUNTED);
        return;
    }

    if (!masterMounted)
        return;

    if (botMounted)
        return;

    // Don't mount in combat or while casting.
    if (bot->IsInCombat() || bot->IsNonMeleeSpellCast(false))
        return;

    bool masterFlying = master->IsFlying() || master->HasAuraType(SPELL_AURA_FLY) ||
                        master->HasAuraType(SPELL_AURA_MOD_FLIGHT_SPEED);

    uint32 mountSpell = FindBestMountSpell(bot, masterFlying);
    if (!mountSpell)
        return;

    bot->CastSpell(bot, mountSpell, false);

    TC_LOG_DEBUG("altbot", "AltbotMount: '%s' mounting (%s) via spell %u.",
        bot->GetName().c_str(), masterFlying ? "flying" : "ground", mountSpell);
}

} // namespace AltbotMount

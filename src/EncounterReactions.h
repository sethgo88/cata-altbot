#pragma once
#include "EncounterMechanics.h"
#include <cstdint>

class Player;
class Unit;

// Phase 3 reactions: the strategy-side opt-in helpers that read enemy
// casts / friendly debuffs / hostile buffs and fire the bot's matching
// interrupt / dispel / purge spell.
//
// Hooking pattern (per strategy `Update`, after the cast-in-progress + GCD
// guards, before the rotation tier dispatch):
//
//   if (uint32 kick = GetSpell(Spell::Counterspell))
//       if (EncounterReactions::TryInterruptNearbyCast(bot, kick, "FrostMage"))
//           return;
//
//   if (uint32 dispel = GetSpell(Spell::RemoveCurse))
//       if (EncounterReactions::TryDispelAlly(bot, master,
//               EncounterMechanics::DispelType::Curse, dispel, "FrostMage"))
//           return;
//
// Each helper:
//   - returns true when it fired a cast (caller skips the rest of the tick)
//   - returns false when nothing matched / nothing was off CD
//   - routes through `StrategyUtil::CastWithLog` with `tierIdx = -2,
//     tierName = "encounter"` so the cast lands in the telemetry pipeline
//     under a stable tier label.
namespace EncounterReactions
{
    // Walks hostile casters in `radius` yards; if any current cast / channel
    // is in the mechanic DB and its priority meets `minPriority`, casts
    // `interruptSpellId` on that target. Cross-bot duplicate kicks are
    // possible in v1 — claim queue is reserved for Phase 6.
    bool TryInterruptNearbyCast(Player* bot,
                                uint32 interruptSpellId,
                                char const* specLabel,
                                float radius = 30.0f,
                                EncounterMechanics::InterruptPriority minPriority =
                                    EncounterMechanics::InterruptPriority::ShouldInterrupt);

    // Walks the bot's party for an ally with an aura whose `dispel_type`
    // matches `removable`. Casts `dispelSpellId` on the first match. The
    // mechanic DB priority drives ordering once Phase 3 dispel rows land;
    // for now any matching aura fires.
    bool TryDispelAlly(Player* bot,
                       Player* master,
                       EncounterMechanics::DispelType removable,
                       uint32 dispelSpellId,
                       char const* specLabel);

    // Hostile-side mirror of TryDispelAlly — purge / Tranq Shot / Spellsteal.
    // Walks hostiles in `radius`; if any has an aura matching `removable`,
    // casts `purgeSpellId` on it.
    bool TryPurgeHostile(Player* bot,
                         EncounterMechanics::DispelType removable,
                         uint32 purgeSpellId,
                         char const* specLabel,
                         float radius = 35.0f);
}

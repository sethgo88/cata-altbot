#include "AltbotLoot.h"
#include "AltbotState.h"
#include "Group.h"
#include "Player.h"

namespace AltbotLoot
{

// Active loot pickup of nearby corpses.
//
// Implementation outline (deferred — needs TC-fork API verification):
//   1. Cell::VisitGridObjects within ~30y for dead, lootable creatures.
//   2. For each creature where bot is a tap holder OR group-tap is active:
//      a. Open loot via SendLoot(creature->GetGUID(), LOOT_CORPSE)
//      b. Iterate creature->loot.items, AutoStoreLoot to bot inventory
//      c. Pick up gold via ModifyMoney
//   3. Skip BoP soulbound items unless bot has a need/greed claim.
//
// AutoPass-on-roll outline (deferred — same TC-fork verification gate):
//   For each bot.GetGroup()->GetRolls() entry where the bot's vote is
//   NOT_EMITED_YET, call CountRollVote(bot->GetGUID(), itemGUID, ROLL_PASS).
//
// Both behaviors are toggle-gated already (state.autoLoot, state.autoPass).
// Wiring lands once the TC fork's Group::Rolls and Cell::VisitGridObjects
// accessors are confirmed against this codebase.
void Tick(Player* /*bot*/, Player* /*master*/)
{
    // Phase 5 stub — see comment above for the implementation outline.
}

} // namespace AltbotLoot

#include "AltbotLoot.h"
#include "AltbotState.h"
#include "Group.h"
#include "Loot.h"
#include "Log.h"
#include "Player.h"
#include <vector>

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
static void TickAutoLoot(Player* /*bot*/, Player* /*master*/)
{
    // Phase 5 stub.
}

// Auto-vote on pending group-loot rolls. Without this the bot's vote is
// never submitted and the entire group has to wait the full 60s timeout.
//
// Mode semantics:
//   Wait        — never called (state.lootRoll == Wait short-circuits below).
//   Pass        — vote PASS on every eligible roll.
//   Disenchant  — vote DISENCHANT when the roll allows it (group has an
//                 enchanter that meets the item's RequiredDisenchantSkill);
//                 otherwise vote PASS.
//
// Iteration safety: CountRollVote may call CountTheRoll which can erase the
// matching entry from the RollId list if our vote completes the roll.
// Snapshot the (itemGUID, vote) pairs first, then dispatch — so the live
// iteration over GetRollIdList() never observes a mutated container.
static void TickRollVote(Player* bot, AltbotLootRollMode mode)
{
    Group* group = bot->GetGroup();
    if (!group)
        return;

    ObjectGuid botGuid = bot->GetGUID();

    struct Pending { ObjectGuid itemGuid; uint8 vote; uint32 itemId; };
    std::vector<Pending> pending;

    for (auto const& rollPtr : group->GetRollIdList())
    {
        Roll* r = rollPtr.get();
        if (!r)
            continue;

        auto it = r->playerVote.find(botGuid);
        if (it == r->playerVote.end())
            continue;
        if (it->second != NOT_EMITED_YET)
            continue;

        uint8 vote = PASS;
        if (mode == AltbotLootRollMode::Disenchant
            && (r->rollVoteMask & ROLL_FLAG_TYPE_DISENCHANT))
        {
            vote = DISENCHANT;
        }

        pending.push_back({ r->itemGUID, vote, r->itemid });
    }

    for (Pending const& p : pending)
    {
        TC_LOG_DEBUG("altbot",
            "AltbotLoot: '%s' auto-voting %s on roll item=%u",
            bot->GetName().c_str(),
            p.vote == DISENCHANT ? "DISENCHANT" : "PASS",
            p.itemId);

        group->CountRollVote(botGuid, p.itemGuid, p.vote);
    }
}

void Tick(Player* bot, Player* master, AltbotState const& state)
{
    if (!bot)
        return;

    if (state.autoLoot)
        TickAutoLoot(bot, master);

    if (state.lootRoll != AltbotLootRollMode::Wait)
        TickRollVote(bot, state.lootRoll);
}

} // namespace AltbotLoot

#include "AltbotLoot.h"
#include "AltbotAI.h"
#include "AltbotAddonProto.h"
#include "AltbotState.h"
#include "Creature.h"
#include "Group.h"
#include "Loot.h"
#include "Log.h"
#include "MotionMaster.h"
#include "ObjectAccessor.h"
#include "Player.h"
#include <sstream>
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
// iteration over GetRolls() never observes a mutated container.
static void TickRollVote(Player* bot, AltbotLootRollMode mode)
{
    Group* group = bot->GetGroup();
    if (!group)
        return;

    ObjectGuid botGuid = bot->GetGUID();

    struct Pending { ObjectGuid itemGuid; uint8 vote; uint32 itemId; };
    std::vector<Pending> pending;

    for (auto const& rollPtr : group->GetRolls())
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

// Manual loot task driven by the addon LOOT verb. The verb handler has already
// validated lootability + tap eligibility, so this function only has to keep
// the bot honest while it walks to the corpse and drains it.
//
// Range: 4y matches Trinity's interact distance for "loot" interactions
// (CORPSE_LOOT_DISTANCE in Object.h). MovePoint to corpse coords; we don't use
// MoveChase because the corpse never moves and MoveChase keeps reissuing each
// time the chase generator decides it's "out of range" by its own threshold.
//
// Drain: ModifyMoney + StoreLootItem mirror the server-side packet handlers
// (HandleLootMoneyOpcode / HandleAutostoreLootItemOpcode). StoreLootItem does
// the inventory-fit check internally and marks the slot looted on success;
// items that don't fit are skipped (left on the corpse for someone else).
//
// Timeout: 30s give-up. Long enough to chase a corpse across a small room
// without the master watching it forever; short enough that "couldn't path
// to corpse" doesn't pin the bot indefinitely.
void TickPending(Player* bot, Player* master, AltbotAI& ai, uint32 diff)
{
    static constexpr float CORPSE_LOOT_RANGE = 4.0f;
    static constexpr uint32 LOOT_TIMEOUT_MS  = 30000;

    if (!bot)
        return;

    ai.AddPendingLootElapsedMs(diff);

    auto fail = [&](char const* code, char const* msg)
    {
        ai.ClearPendingLootTarget();
        if (!master)
            return;
        std::ostringstream out;
        out << "ERR|" << code << "|" << msg;
        AltbotAddonProto::Send(bot, master, out.str());
    };

    if (ai.GetPendingLootElapsedMs() > LOOT_TIMEOUT_MS)
    {
        fail("LOOT_TIMEOUT", "could not reach corpse");
        return;
    }

    Unit* unit = ObjectAccessor::GetUnit(*bot, ai.GetPendingLootTarget());
    Creature* creature = unit ? unit->ToCreature() : nullptr;

    // Target despawned mid-walk — silent clear, not really an error.
    if (!creature || creature->IsAlive())
    {
        ai.ClearPendingLootTarget();
        return;
    }

    // Map mismatch (master pulled bot into a different instance via summon
    // mid-walk) — silent clear; the master can re-issue if they still want it.
    if (creature->GetMapId() != bot->GetMapId())
    {
        ai.ClearPendingLootTarget();
        return;
    }

    // Lootable flag may have been cleared by another looter while we were
    // walking. Same silent-clear treatment as despawn.
    if (!creature->HasFlag(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_LOOTABLE))
    {
        ai.ClearPendingLootTarget();
        return;
    }

    float dist = bot->GetDistance(creature);
    if (dist > CORPSE_LOOT_RANGE)
    {
        // Issue MovePoint once per re-entry; MotionMaster::MovePoint replaces
        // the active generator, so spamming it every tick fights itself the
        // same way MoveChase does. UNIT_STATE_ROAMING_MOVE flips on while a
        // MovePoint is in flight.
        if (!bot->HasUnitState(UNIT_STATE_ROAMING_MOVE))
        {
            float x, y, z;
            creature->GetPosition(x, y, z);
            bot->GetMotionMaster()->MovePoint(0, x, y, z);
        }
        return;
    }

    // In range — drain. SendLoot populates creature->loot if it hasn't been
    // opened yet (the dyn-flag guarantees it has loot), and registers the bot
    // as a current looter so StoreLootItem won't reject the slot.
    bot->SendLoot(creature->GetGUID(), LOOT_CORPSE);

    Loot* loot = &creature->loot;

    if (loot->gold > 0)
    {
        bot->ModifyMoney(loot->gold);
        loot->gold = 0;
    }

    // Walk every populated slot once. StoreLootItem returns the affected
    // LootItem* on success and nullptr on failure (full inventory, BoP rules,
    // etc.). Items that fail are left on the corpse — partially-looted is OK.
    for (uint8 i = 0; i < uint8(loot->items.size()); ++i)
    {
        if (loot->items[i].is_looted)
            continue;
        bot->StoreLootItem(creature->GetGUID(), i, loot);
    }

    // Standard release flow — also handles the dyn-flag flip when isLooted().
    bot->GetSession()->DoLootRelease(creature->GetGUID());

    ai.ClearPendingLootTarget();

    TC_LOG_DEBUG("altbot",
        "AltbotLoot: '%s' manual-looted creature %u",
        bot->GetName().c_str(),
        creature->GetEntry());
}

} // namespace AltbotLoot

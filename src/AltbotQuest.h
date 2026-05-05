#pragma once
#include "ObjectGuid.h"
#include "QuestDef.h"
#include <map>

class AltbotAI;
class Player;
class Quest;

// Quest mirroring + visibility into bot-side quest progress.
//
// Master mirroring: when the master accepts or turns in a quest, every bot in
// master's group with the corresponding toggle on does the same — fired from
// PlayerScript::OnQuestStatusChange in AltbotQuest.cpp.
//
// Bot-side whispers: each bot polls its own quest log on the follow tick
// (TickProgress) and whispers the master on accepts, kill/item objective
// increments, completion (objectives satisfied), and turn-ins. Polling covers
// state transitions that don't fire OnQuestStatusChange — notably
// Player::CompleteQuest (kill credit auto-completion) and item-pickup objective
// updates, neither of which goes through the script hook.
//
// Force-NPC: skips the master-mirror path entirely. Master picks an NPC and
// each bot accepts everything that NPC offers + turns in everything that NPC
// involves. Lets the master fix bots that missed an earlier accept (joined the
// group late, were offline, autoQuestTake was off, etc.).
namespace AltbotQuest
{
    void OnMasterQuestAccept(Player* master, Quest const* quest);
    void OnMasterQuestReward(Player* master, Quest const* quest);

    // Per-bot snapshot of the active quest log for delta detection. Owned by
    // AltbotAI; passed in/out of TickProgress so the storage can live on the
    // AI instead of inside this namespace's globals.
    using SnapshotMap = std::map<uint32, QuestStatusData>;

    // Polled each follow tick (1s cadence). Compares the bot's live quest log
    // against `snapshot` and whispers the master for any deltas. Updates
    // `snapshot` to reflect post-tick state.
    void TickProgress(AltbotAI& ai, SnapshotMap& snapshot);

    // Force-take and force-turn-in every quest the targeted creature offers /
    // accepts that the bot is eligible for. Whispers the master with the count
    // taken and the count turned in.
    void InteractWithNpc(Player* master, AltbotAI* ai, ObjectGuid npcGuid);

    // Print the bot's active quest log to master's chat (PSendSysMessage).
    void ListBotQuests(Player* master, AltbotAI* ai);
}

void AddSC_AltbotQuest();

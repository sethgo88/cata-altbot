#include "AltbotQuest.h"
#include "AltbotAI.h"
#include "AltbotMgr.h"
#include "Chat.h"
#include "Creature.h"
#include "CreatureData.h"
#include "GameObjectData.h"
#include "Group.h"
#include "ItemTemplate.h"
#include "Log.h"
#include "ObjectAccessor.h"
#include "ObjectMgr.h"
#include "Player.h"
#include "QuestDef.h"
#include "ScriptMgr.h"
#include "WorldSession.h"
#include <algorithm>
#include <iterator>
#include <sstream>

namespace AltbotQuest
{

// ---- whisper helpers ------------------------------------------------------

// Bot whispers the master. Filtered by ChatFrame in the addon (CATABOT prefix
// is hidden), so a literal "[CataAltbot] " visual prefix on these payloads is
// what the user sees in their chat window. Falls back to a system message if
// the bot can't whisper for some reason (different map, sanctioned).
static void WhisperMaster(Player* bot, Player* master, std::string const& message)
{
    if (!bot || !master)
        return;

    std::string out = "[CataAltbot] " + message;
    bot->Whisper(out, LANG_UNIVERSAL, master);
}

// ---- master → bot mirroring -----------------------------------------------

// Walks master's group; returns AltbotAI* for any registered bot owned by master.
// Skips the master himself and any non-bot group members.
static std::vector<AltbotAI*> CollectBotsInGroup(Player* master)
{
    std::vector<AltbotAI*> out;

    Group* group = master->GetGroup();
    if (!group)
        return out;

    for (GroupReference* itr = group->GetFirstMember(); itr != nullptr; itr = itr->next())
    {
        Player* member = itr->GetSource();
        if (!member || member == master)
            continue;

        AltbotAI* ai = sAltbotMgr->FindBotAI(master->GetGUID(), member->GetGUID());
        if (ai)
            out.push_back(ai);
    }

    return out;
}

void OnMasterQuestAccept(Player* master, Quest const* quest)
{
    if (!quest)
        return;

    for (AltbotAI* ai : CollectBotsInGroup(master))
    {
        if (!ai->GetState().autoQuestTake)
            continue;

        Player* bot = ai->GetSession()->GetPlayer();
        if (!bot || !bot->IsInWorld())
            continue;

        // Skip if already on the quest or finished it.
        if (bot->GetQuestStatus(quest->GetQuestId()) != QUEST_STATUS_NONE)
            continue;

        if (!bot->CanTakeQuest(quest, false))
        {
            // The most common reason a bot silently doesn't take master's quest
            // — wrong level/class/race/skill/prereq. Whisper it so the master
            // knows the auto-take didn't fire (and why, in broad strokes).
            WhisperMaster(bot, master,
                "Can't take '" + quest->GetTitle() + "' (level/class/prereq mismatch).");
            continue;
        }

        bot->AddQuestAndCheckCompletion(quest, master);

        TC_LOG_INFO("altbot", "AltbotQuest: '%s' auto-took quest %u from master '%s'.",
            bot->GetName().c_str(), quest->GetQuestId(), master->GetName().c_str());
        // The bot's own OnQuestStatusChange handler emits the accept whisper —
        // don't double-whisper here.
    }
}

void OnMasterQuestReward(Player* master, Quest const* quest)
{
    if (!quest)
        return;

    for (AltbotAI* ai : CollectBotsInGroup(master))
    {
        if (!ai->GetState().autoQuestTurnIn)
            continue;

        Player* bot = ai->GetSession()->GetPlayer();
        if (!bot || !bot->IsInWorld())
            continue;

        // Bot must have completed the quest objectives.
        if (bot->GetQuestStatus(quest->GetQuestId()) != QUEST_STATUS_COMPLETE)
            continue;

        // Reward index 0 = first reward choice (no UI to pick from).
        bot->RewardQuest(quest, 0, master, false);

        TC_LOG_INFO("altbot", "AltbotQuest: '%s' auto-turned-in quest %u for master '%s'.",
            bot->GetName().c_str(), quest->GetQuestId(), master->GetName().c_str());
        // Same as accept: the bot's hook produces the turn-in whisper.
    }
}

// ---- bot-side progress polling --------------------------------------------

// QuestStatus → string for whisper output. We only emit meaningful transitions,
// so MAX_QUEST_STATUS / unused values don't need entries.
static char const* StatusLabel(QuestStatus s)
{
    switch (s)
    {
        case QUEST_STATUS_INCOMPLETE: return "in progress";
        case QUEST_STATUS_COMPLETE:   return "complete";
        case QUEST_STATUS_FAILED:     return "failed";
        case QUEST_STATUS_REWARDED:   return "rewarded";
        case QUEST_STATUS_NONE:       return "none";
        default:                      return "?";
    }
}

// Resolve the display name of a quest objective NPC/GO. RequiredNpcOrGo
// stores creature entry as positive, gameobject entry as negative.
static std::string ObjectiveTargetName(int32 npcOrGo)
{
    if (npcOrGo > 0)
    {
        if (CreatureTemplate const* ct = sObjectMgr->GetCreatureTemplate(uint32(npcOrGo)))
            return ct->Name;
        return "Creature " + std::to_string(npcOrGo);
    }
    else if (npcOrGo < 0)
    {
        uint32 entry = uint32(-npcOrGo);
        if (GameObjectTemplate const* gt = sObjectMgr->GetGameObjectTemplate(entry))
            return gt->name;
        return "Object " + std::to_string(entry);
    }
    return "?";
}

static std::string ItemName(uint32 itemId)
{
    if (ItemTemplate const* it = sObjectMgr->GetItemTemplate(itemId))
        if (char const* n = it->GetDefaultLocaleName())
            return n;
    return "Item " + std::to_string(itemId);
}

// Whisper kill/loot increments + completion transitions for a single quest.
// Called from TickProgress for each quest where the snapshot disagrees with
// the live status data.
static void EmitDeltas(Player* bot, Player* master, Quest const* quest,
                       QuestStatusData const& was, QuestStatusData const& now)
{
    std::string const& title = quest->GetTitle();

    // Kill / GO objectives.
    for (uint8 i = 0; i < QUEST_OBJECTIVES_COUNT; ++i)
    {
        int32 npcOrGo = quest->RequiredNpcOrGo[i];
        uint32 required = quest->RequiredNpcOrGoCount[i];
        if (!npcOrGo || !required)
            continue;

        uint16 prev = was.CreatureOrGOCount[i];
        uint16 cur  = now.CreatureOrGOCount[i];
        if (cur == prev)
            continue;

        std::ostringstream s;
        s << "'" << title << "' — " << cur << "/" << required << " "
          << ObjectiveTargetName(npcOrGo)
          << (npcOrGo > 0 ? " slain" : " used") << ".";
        WhisperMaster(bot, master, s.str());
    }

    // Item objectives.
    for (uint8 i = 0; i < QUEST_ITEM_OBJECTIVES_COUNT; ++i)
    {
        uint32 itemId   = quest->RequiredItemId[i];
        uint32 required = quest->RequiredItemCount[i];
        if (!itemId || !required)
            continue;

        uint16 prev = was.ItemCount[i];
        uint16 cur  = now.ItemCount[i];
        if (cur == prev)
            continue;

        std::ostringstream s;
        s << "'" << title << "' — " << cur << "/" << required << " "
          << ItemName(itemId) << " collected.";
        WhisperMaster(bot, master, s.str());
    }

    // Status transitions worth surfacing. INCOMPLETE→COMPLETE happens inside
    // Player::CompleteQuest (no script hook there) so polling is the only way
    // to see it. INCOMPLETE→FAILED via Player::FailQuest does fire a hook,
    // but covering it here too is harmless and keeps all status surfacing in
    // one place.
    if (was.Status != now.Status)
    {
        if (now.Status == QUEST_STATUS_COMPLETE)
            WhisperMaster(bot, master, "Ready to turn in '" + title + "'.");
        else if (now.Status == QUEST_STATUS_FAILED)
            WhisperMaster(bot, master, "Quest '" + title + "' failed.");
        // INCOMPLETE / REWARDED transitions are surfaced from the hook to keep
        // the wording consistent with the accept/turn-in messages.
    }
}

void TickProgress(AltbotAI& ai, SnapshotMap& snapshot)
{
    Player* bot = ai.GetSession() ? ai.GetSession()->GetPlayer() : nullptr;
    if (!bot || !bot->IsInWorld())
        return;

    Player* master = ObjectAccessor::FindPlayer(ai.GetMasterGuid());

    QuestStatusMap const& live = bot->getQuestStatusMap();

    // Detect adds (NONE → INCOMPLETE/COMPLETE) and per-quest deltas. The
    // existing OnQuestStatusChange hook already emits the accept whisper, so
    // we don't whisper "added" again here — but we do need to seed the
    // snapshot so future ticks have a baseline.
    for (auto const& kv : live)
    {
        uint32 questId = kv.first;
        QuestStatusData const& cur = kv.second;

        auto it = snapshot.find(questId);
        if (it == snapshot.end())
        {
            snapshot[questId] = cur;
            continue;
        }

        QuestStatusData const& prev = it->second;
        bool sameStatus = (prev.Status == cur.Status);
        bool sameCounters = std::equal(std::begin(prev.CreatureOrGOCount), std::end(prev.CreatureOrGOCount),
                                       std::begin(cur.CreatureOrGOCount))
                         && std::equal(std::begin(prev.ItemCount), std::end(prev.ItemCount),
                                       std::begin(cur.ItemCount));
        if (sameStatus && sameCounters)
            continue;

        if (master && master->IsInWorld())
        {
            if (Quest const* quest = sObjectMgr->GetQuestTemplate(questId))
                EmitDeltas(bot, master, quest, prev, cur);
        }

        it->second = cur;
    }

    // Drop snapshot rows for quests no longer in the live map (turned in,
    // abandoned). The accept/turn-in hooks have already whispered those, so
    // we don't need to emit anything here.
    for (auto it = snapshot.begin(); it != snapshot.end(); )
    {
        if (live.find(it->first) == live.end())
            it = snapshot.erase(it);
        else
            ++it;
    }
}

// ---- force-with-NPC -------------------------------------------------------

// Master-driven retroactive accept/turn-in. Iterates the NPC's quest relations
// and acts on every quest the bot is eligible for. Used to fix bots that
// missed an earlier accept (joined late, autoQuestTake was off, etc.) or to
// hand in quests the master no longer has but the bot still does.
void InteractWithNpc(Player* master, AltbotAI* ai, ObjectGuid npcGuid)
{
    if (!ai)
        return;
    Player* bot = ai->GetSession() ? ai->GetSession()->GetPlayer() : nullptr;
    if (!master || !bot || !bot->IsInWorld())
        return;

    ChatHandler chat(master->GetSession());

    Creature* npc = ObjectAccessor::GetCreature(*bot, npcGuid);
    if (!npc)
    {
        chat.PSendSysMessage("Altbot: %s can't see that NPC.", bot->GetName().c_str());
        return;
    }

    if (npc->GetMapId() != bot->GetMapId())
    {
        chat.PSendSysMessage("Altbot: %s and the NPC are on different maps.", bot->GetName().c_str());
        return;
    }

    // Cap interaction range to the standard quest-giver radius. Slightly
    // larger than the client's prompt range to handle bot positioning slop —
    // but tight enough that the master can't drag a bot across a zone to
    // accept a quest from a creature it isn't actually near.
    constexpr float kInteractRange = 15.0f;
    if (bot->GetDistance(npc) > kInteractRange)
    {
        chat.PSendSysMessage("Altbot: %s isn't close enough to %s.",
            bot->GetName().c_str(), npc->GetName().c_str());
        return;
    }

    uint32 accepted = 0;
    uint32 turnedIn = 0;

    // Offered quests — accept anything new + eligible.
    for (uint32 questId : sObjectMgr->GetCreatureQuestRelations(npc->GetEntry()))
    {
        Quest const* quest = sObjectMgr->GetQuestTemplate(questId);
        if (!quest)
            continue;

        if (bot->GetQuestStatus(questId) != QUEST_STATUS_NONE)
            continue;

        if (!bot->CanTakeQuest(quest, false))
            continue;

        bot->AddQuestAndCheckCompletion(quest, npc);
        ++accepted;
    }

    // Involved quests — turn in anything the bot has completed.
    for (uint32 questId : sObjectMgr->GetCreatureQuestInvolvedRelations(npc->GetEntry()))
    {
        Quest const* quest = sObjectMgr->GetQuestTemplate(questId);
        if (!quest)
            continue;

        if (bot->GetQuestStatus(questId) != QUEST_STATUS_COMPLETE)
            continue;

        bot->RewardQuest(quest, 0, npc, false);
        ++turnedIn;
    }

    chat.PSendSysMessage("Altbot: %s — %u quest(s) accepted, %u quest(s) turned in with %s.",
        bot->GetName().c_str(), accepted, turnedIn, npc->GetName().c_str());
}

// ---- list bot quests ------------------------------------------------------

void ListBotQuests(Player* master, AltbotAI* ai)
{
    if (!master || !ai)
        return;
    Player* bot = ai->GetSession() ? ai->GetSession()->GetPlayer() : nullptr;
    if (!bot || !bot->IsInWorld())
        return;

    ChatHandler chat(master->GetSession());

    QuestStatusMap const& quests = bot->getQuestStatusMap();
    if (quests.empty())
    {
        chat.PSendSysMessage("%s has no active quests.", bot->GetName().c_str());
        return;
    }

    chat.PSendSysMessage("%s's quest log (%zu):", bot->GetName().c_str(), quests.size());
    for (auto const& kv : quests)
    {
        uint32 questId = kv.first;
        QuestStatusData const& s = kv.second;

        Quest const* quest = sObjectMgr->GetQuestTemplate(questId);
        if (!quest)
            continue;

        // Build an inline objective summary so the master sees current/required
        // counts without needing a separate "details" command.
        std::ostringstream obj;
        bool hasObjectives = false;
        for (uint8 i = 0; i < QUEST_OBJECTIVES_COUNT; ++i)
        {
            int32 npcOrGo = quest->RequiredNpcOrGo[i];
            uint32 required = quest->RequiredNpcOrGoCount[i];
            if (!npcOrGo || !required)
                continue;
            if (hasObjectives) obj << ", ";
            obj << s.CreatureOrGOCount[i] << "/" << required << " "
                << ObjectiveTargetName(npcOrGo);
            hasObjectives = true;
        }
        for (uint8 i = 0; i < QUEST_ITEM_OBJECTIVES_COUNT; ++i)
        {
            uint32 itemId = quest->RequiredItemId[i];
            uint32 required = quest->RequiredItemCount[i];
            if (!itemId || !required)
                continue;
            if (hasObjectives) obj << ", ";
            obj << s.ItemCount[i] << "/" << required << " "
                << ItemName(itemId);
            hasObjectives = true;
        }

        chat.PSendSysMessage("  [%u] %s (lvl %d) — %s%s%s",
            questId,
            quest->GetTitle().c_str(),
            quest->GetQuestLevel(),
            StatusLabel(s.Status),
            hasObjectives ? " — " : "",
            hasObjectives ? obj.str().c_str() : "");
    }
}

// ---- PlayerScript hooks ---------------------------------------------------

// TC 4.3.4 fires OnQuestStatusChange from:
//   - Player::AddQuest             (after accept;        status = INCOMPLETE)
//   - Player::RewardQuest          (after turn-in;       status = REWARDED)
//   - Player::FailQuest            (status = FAILED)
//   - QuestHandler::HandleQuestPushResult / SpellEffects::EffectQuestComplete
//
// CompleteQuest (objective satisfaction, kill/item credit auto-completes) does
// NOT fire this hook in 4.3.4 — TickProgress polls for that transition.
class altbot_quest_playerscript : public PlayerScript
{
public:
    altbot_quest_playerscript() : PlayerScript("altbot_quest_playerscript") { }

    void OnQuestStatusChange(Player* player, uint32 questId) override
    {
        Quest const* quest = sObjectMgr->GetQuestTemplate(questId);
        if (!quest)
            return;
        QuestStatus status = player->GetQuestStatus(questId);

        // If the player is one of our bots, whisper their master. We do this
        // FIRST so master-driven mirrors (which call AddQuestAndCheckCompletion
        // on the bot, which re-enters this hook) produce a single whisper from
        // the bot's perspective regardless of whether the accept came from the
        // master's mirror or from manual dialog or from InteractWithNpc.
        if (AltbotAI* botAi = sAltbotMgr->FindAnyBotAI(player->GetGUID()))
        {
            if (Player* master = ObjectAccessor::FindPlayer(botAi->GetMasterGuid()))
            {
                if (master->IsInWorld())
                {
                    if (status == QUEST_STATUS_INCOMPLETE)
                        WhisperMaster(player, master,
                            "Took quest '" + quest->GetTitle() + "'.");
                    else if (status == QUEST_STATUS_REWARDED)
                        WhisperMaster(player, master,
                            "Turned in '" + quest->GetTitle() + "'.");
                    else if (status == QUEST_STATUS_FAILED)
                        WhisperMaster(player, master,
                            "Quest '" + quest->GetTitle() + "' failed.");
                }
            }
            return;
        }

        // Master-driven mirror: only fire when this is the master, not a bot.
        if (status == QUEST_STATUS_INCOMPLETE)
            OnMasterQuestAccept(player, quest);
        else if (status == QUEST_STATUS_REWARDED)
            OnMasterQuestReward(player, quest);
    }
};

} // namespace AltbotQuest

void AddSC_AltbotQuest()
{
    new AltbotQuest::altbot_quest_playerscript();
}

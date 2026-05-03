#include "AltbotQuest.h"
#include "AltbotAI.h"
#include "AltbotMgr.h"
#include "Chat.h"
#include "Group.h"
#include "Log.h"
#include "ObjectMgr.h"
#include "Player.h"
#include "QuestDef.h"
#include "ScriptMgr.h"
#include "WorldSession.h"

namespace AltbotQuest
{

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
            continue;

        bot->AddQuestAndCheckCompletion(quest, master);

        TC_LOG_INFO("altbot", "AltbotQuest: '%s' auto-took quest %u from master '%s'.",
            bot->GetName().c_str(), quest->GetQuestId(), master->GetName().c_str());
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
    }
}

// TC 4.3.4 PlayerScript quest hooks:
//   - There is no OnQuestAccept(Player*, Quest const*) 2-arg form on PlayerScript.
//     The only quest accept hook on PlayerScript is on Item scripts
//     (ItemScript::OnQuestAccept).
//   - Quest accept/reward from NPCs fires CreatureScript hooks, not PlayerScript.
//   - The available PlayerScript hook is OnQuestStatusChange(Player*, uint32 questId).
//
// We hook OnQuestStatusChange and fire our helpers when status transitions to
// QUEST_STATUS_COMPLETE (turn-in trigger) or when the master takes a new quest
// (QUEST_STATUS_INCOMPLETE / QUEST_STATUS_NONE transition is not visible here,
// so quest-take mirroring is handled at QUEST_STATUS_INCOMPLETE = quest accepted).
class altbot_quest_playerscript : public PlayerScript
{
public:
    altbot_quest_playerscript() : PlayerScript("altbot_quest_playerscript") { }

    // TC 4.3.4: PlayerScript::OnQuestStatusChange(Player*, uint32 questId)
    void OnQuestStatusChange(Player* player, uint32 questId) override
    {
        QuestStatus status = player->GetQuestStatus(questId);

        Quest const* quest = sObjectMgr->GetQuestTemplate(questId);
        if (!quest)
            return;

        if (status == QUEST_STATUS_INCOMPLETE)
        {
            // Player just accepted the quest — mirror to bots.
            OnMasterQuestAccept(player, quest);
        }
        else if (status == QUEST_STATUS_REWARDED)
        {
            // Player just turned in the quest — mirror to bots.
            OnMasterQuestReward(player, quest);
        }
    }
};

} // namespace AltbotQuest

void AddSC_AltbotQuest()
{
    new AltbotQuest::altbot_quest_playerscript();
}

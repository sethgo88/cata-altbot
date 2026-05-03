#include "AltbotQuest.h"
#include "AltbotAI.h"
#include "AltbotMgr.h"
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

// NOTE: PlayerScript hook signatures vary across TC forks. This file is
// written against the common "OnQuestAccept(Player*, Quest const*)" /
// "OnQuestReward(Player*, Quest const*)" 2-arg pattern. If the Cataclysm
// Preservation Project fork uses 3-arg signatures (e.g., with a questGiver
// Object* trailing arg, or OnQuestStatusChange(Player*, uint32, QuestStatus)),
// adjust the override declarations below and route through the same
// OnMasterQuestAccept / OnMasterQuestReward helpers above. CLAUDE.md
// "Pending — TC fork access required" tracks this verification item.
class altbot_quest_playerscript : public PlayerScript
{
public:
    altbot_quest_playerscript() : PlayerScript("altbot_quest_playerscript") { }

    void OnQuestAccept(Player* player, Quest const* quest) override
    {
        OnMasterQuestAccept(player, quest);
    }

    void OnQuestReward(Player* player, Quest const* quest) override
    {
        OnMasterQuestReward(player, quest);
    }
};

} // namespace AltbotQuest

void AddSC_AltbotQuest()
{
    new AltbotQuest::altbot_quest_playerscript();
}

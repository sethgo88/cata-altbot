#pragma once

class Player;
class Quest;

// Mirrors master's quest accept and quest reward to registered, group-member
// bots whose autoQuestTake / autoQuestTurnIn toggles are enabled.
//
// Wired via PlayerScript hooks in AltbotQuest.cpp (OnQuestAccept, OnQuestReward).
// Hook names need TC-fork verification — see CLAUDE.md "Pending" section.
namespace AltbotQuest
{
    void OnMasterQuestAccept(Player* master, Quest const* quest);
    void OnMasterQuestReward(Player* master, Quest const* quest);
}

void AddSC_AltbotQuest();

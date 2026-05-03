#include "AltbotTalents.h"
#include "Chat.h"
#include "Log.h"
#include "Player.h"
#include "WorldSession.h"
#include <cstdlib>
#include <string>

namespace AltbotTalents
{

void Show(Player* master, Player* bot)
{
    if (!bot)
        return;

    ChatHandler chat(master->GetSession());

    uint8  activeSpec = bot->GetActiveSpec();
    uint32 freePoints = bot->GetFreeTalentPoints();

    chat.PSendSysMessage("%s — active spec %u, %u free talent point(s).",
        bot->GetName().c_str(), uint32(activeSpec), freePoints);
    chat.SendSysMessage("Use 'learn <talentId> <rank>' to spend a point. Tree IDs in the talent calc URL.");
}

bool Learn(Player* master, Player* bot, uint32 talentId, uint32 rank)
{
    if (!bot)
        return false;

    ChatHandler chat(master->GetSession());

    if (talentId == 0)
    {
        chat.SendSysMessage("Invalid talent id.");
        return false;
    }

    if (bot->GetFreeTalentPoints() == 0)
    {
        chat.PSendSysMessage("%s has no free talent points.", bot->GetName().c_str());
        return false;
    }

    // LearnTalent validates: prereqs (tree row threshold), rank vs max,
    // available points, active spec. Returns void on TC; we re-check the
    // free-point delta to confirm success.
    uint32 before = bot->GetFreeTalentPoints();
    bot->LearnTalent(talentId, rank);
    uint32 after  = bot->GetFreeTalentPoints();

    if (after == before)
    {
        chat.PSendSysMessage("Failed to learn talent %u rank %u (prereq, rank cap, or no point spent).",
            talentId, rank);
        return false;
    }

    chat.PSendSysMessage("%s learned talent %u rank %u. %u point(s) remaining.",
        bot->GetName().c_str(), talentId, rank, after);
    return true;
}

bool LearnFromArgs(Player* master, Player* bot, std::string_view args)
{
    std::string s(args);
    auto space = s.find(' ');
    if (space == std::string::npos)
    {
        ChatHandler(master->GetSession()).SendSysMessage("Usage: learn <talentId> <rank>");
        return false;
    }

    uint32 talentId = static_cast<uint32>(std::strtoul(s.substr(0, space).c_str(), nullptr, 10));
    uint32 rank     = static_cast<uint32>(std::strtoul(s.substr(space + 1).c_str(), nullptr, 10));

    return Learn(master, bot, talentId, rank);
}

} // namespace AltbotTalents

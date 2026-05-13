#include "AltbotTalents.h"
#include "Chat.h"
#include "DBCStores.h"
#include "DBCStructure.h"
#include "Log.h"
#include "Player.h"
#include "SpellInfo.h"
#include "SpellMgr.h"
#include "WorldSession.h"
#include <algorithm>
#include <cstdlib>
#include <string>
#include <unordered_set>
#include <vector>

namespace AltbotTalents
{

void Show(Player* master, Player* bot)
{
    if (!bot)
        return;

    ChatHandler chat(master->GetSession());

    uint8  activeSpec = bot->GetActiveSpec();
    uint32 freePoints = bot->GetFreeTalentPoints();
    uint32 primaryTree = bot->GetPrimaryTalentTree(activeSpec);

    chat.PSendSysMessage("%s — active spec %u, %u free talent point(s).",
        bot->GetName().c_str(), uint32(activeSpec), freePoints);
    chat.SendSysMessage("Use 'learn <talentId> <rank>' to spend a point. Full build dumped to worldserver.log (grep 'altbot.talents').");

    // Build a set of currently-learned rank spell IDs in the active spec.
    std::unordered_set<uint32> learnedRankSpells;
    PlayerTalentMap const& talentMap = bot->GetTalentMap(activeSpec);
    for (auto const& kv : talentMap)
        if (kv.second.State != PLAYERSPELL_REMOVED)
            learnedRankSpells.insert(kv.first);

    struct Placement
    {
        uint32 tab;
        uint32 row;
        uint32 col;
        uint32 talentId;
        uint8  rank;       // 1-based
        uint8  maxRank;
        uint32 spellId;
        std::string spellName;
    };

    std::vector<Placement> placements;
    placements.reserve(learnedRankSpells.size());

    uint32 pointsUsed = 0;

    for (uint32 talentId = 0; talentId < sTalentStore.GetNumRows(); ++talentId)
    {
        TalentEntry const* talentInfo = sTalentStore.LookupEntry(talentId);
        if (!talentInfo)
            continue;

        TalentTabEntry const* tabInfo = sTalentTabStore.LookupEntry(talentInfo->TabID);
        if (!tabInfo)
            continue;
        if ((tabInfo->ClassMask & bot->getClassMask()) == 0)
            continue;

        uint8 maxRank = 0;
        for (uint8 r = 0; r < MAX_TALENT_RANK; ++r)
            if (talentInfo->SpellRank[r])
                maxRank = r + 1;

        for (uint8 r = 0; r < MAX_TALENT_RANK; ++r)
        {
            uint32 spellId = talentInfo->SpellRank[r];
            if (!spellId)
                continue;
            if (!learnedRankSpells.count(spellId))
                continue;

            Placement p;
            p.tab      = talentInfo->TabID;
            p.row      = talentInfo->TierID;
            p.col      = talentInfo->ColumnIndex;
            p.talentId = talentInfo->ID;
            p.rank     = r + 1;
            p.maxRank  = maxRank;
            p.spellId  = spellId;
            if (SpellInfo const* info = sSpellMgr->GetSpellInfo(spellId))
                p.spellName = info->SpellName ? info->SpellName : "?";
            else
                p.spellName = "?";

            placements.push_back(std::move(p));
            pointsUsed += p.rank;
            break;
        }
    }

    std::sort(placements.begin(), placements.end(),
        [](Placement const& a, Placement const& b)
        {
            if (a.tab != b.tab) return a.tab < b.tab;
            if (a.row != b.row) return a.row < b.row;
            return a.col < b.col;
        });

    TC_LOG_INFO("altbot.talents",
        "Talents %s spec=%u primaryTree=%u placements=%zu pointsUsed=%u pointsFree=%u",
        bot->GetName().c_str(), uint32(activeSpec), primaryTree,
        placements.size(), pointsUsed, freePoints);

    for (Placement const& p : placements)
        TC_LOG_INFO("altbot.talents",
            "Talents %s tab=%u row=%u col=%u talent=%u rank=%u/%u spell=%u name=\"%s\"",
            bot->GetName().c_str(), p.tab, p.row, p.col,
            p.talentId, uint32(p.rank), uint32(p.maxRank),
            p.spellId, p.spellName.c_str());
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

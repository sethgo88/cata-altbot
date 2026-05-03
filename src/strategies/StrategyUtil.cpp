#include "StrategyUtil.h"
#include "Group.h"
#include "Player.h"
#include "SpellInfo.h"
#include "SpellMgr.h"
#include <cstring>

namespace StrategyUtil
{

Player* FindTank(Player* bot, Player* master)
{
    Group* group = bot->GetGroup();
    if (!group)
        return master;

    Player* tank = master;
    uint32 maxHp = master ? master->GetMaxHealth() : 0;
    for (GroupReference* itr = group->GetFirstMember(); itr != nullptr; itr = itr->next())
    {
        Player* m = itr->GetSource();
        if (!m || !m->IsAlive() || m->GetMap() != bot->GetMap())
            continue;
        if (m == bot)
            continue;
        if (m->GetMaxHealth() > maxHp)
        {
            maxHp = m->GetMaxHealth();
            tank  = m;
        }
    }
    return tank;
}

Player* FindLowestHpAlly(Player* bot, Player* master, Player* exclude)
{
    Player* lowest    = nullptr;
    float   lowestPct = 1000.0f;

    auto consider = [&](Player* p)
    {
        if (!p || !p->IsAlive() || p->GetMap() != bot->GetMap())
            return;
        if (p == exclude)
            return;
        float pct = p->GetHealthPct();
        if (pct < lowestPct)
        {
            lowestPct = pct;
            lowest    = p;
        }
    };

    Group* group = bot->GetGroup();
    if (group)
    {
        for (GroupReference* itr = group->GetFirstMember(); itr != nullptr; itr = itr->next())
            consider(itr->GetSource());
    }
    else
    {
        consider(bot);
        if (master != bot)
            consider(master);
    }
    return lowest;
}

int CountInjured(Player* bot, Player* master, float thresholdPct)
{
    int count = 0;
    auto consider = [&](Player* p)
    {
        if (!p || !p->IsAlive() || p->GetMap() != bot->GetMap())
            return;
        if (p->GetHealthPct() < thresholdPct)
            ++count;
    };

    Group* group = bot->GetGroup();
    if (group)
    {
        for (GroupReference* itr = group->GetFirstMember(); itr != nullptr; itr = itr->next())
            consider(itr->GetSource());
    }
    else
    {
        consider(bot);
        if (master != bot)
            consider(master);
    }
    return count;
}

bool AllAtFullHp(Player* bot, Player* master, float minPct)
{
    bool allHealthy = true;
    auto consider = [&](Player* p)
    {
        if (!p || !p->IsAlive() || p->GetMap() != bot->GetMap())
            return;
        if (p->GetHealthPct() < minPct)
            allHealthy = false;
    };

    Group* group = bot->GetGroup();
    if (group)
    {
        for (GroupReference* itr = group->GetFirstMember(); itr != nullptr; itr = itr->next())
            consider(itr->GetSource());
    }
    else
    {
        consider(bot);
        if (master != bot)
            consider(master);
    }
    return allHealthy;
}

uint32 FindSpellByFamilyName(Player* bot, uint32 family, char const* name)
{
    if (!bot || !name)
        return 0;

    uint32 bestId  = 0;
    uint32 bestLvl = 0;

    for (auto const& [spellId, playerSpell] : bot->GetSpellMap())
    {
        if (playerSpell.state == PLAYERSPELL_REMOVED || !playerSpell.active)
            continue;

        SpellInfo const* info = sSpellMgr->GetSpellInfo(spellId);
        if (!info)
            continue;
        if (info->SpellFamilyName != family)
            continue;
        if (!info->SpellName || std::strcmp(info->SpellName, name) != 0)
            continue;

        if (info->SpellLevel > bestLvl)
        {
            bestLvl = info->SpellLevel;
            bestId  = spellId;
        }
    }
    return bestId;
}

} // namespace StrategyUtil

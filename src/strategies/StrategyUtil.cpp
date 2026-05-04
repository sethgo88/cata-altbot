#include "StrategyUtil.h"
#include "Group.h"
#include "Log.h"
#include "Player.h"
#include "SpellInfo.h"
#include "SpellMgr.h"
#include <cstring>
#include <sstream>
#include <vector>

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

SpellCastResult CastWithLog(Player* bot, Unit* target, uint32 spellId, char const* specLabel)
{
    SpellCastResult result = bot->CastSpell(target, spellId, false);
    if (result != SPELL_CAST_OK)
    {
        TC_LOG_INFO("altbot", "%s[%s] CastSpell %u failed: SpellCastResult=%u",
                    specLabel, bot->GetName().c_str(), spellId, uint32(result));
    }
    return result;
}

uint32 FindSpellByFamilyName(Player* bot, uint32 family, char const* name)
{
    if (!bot || !name)
        return 0;

    // Collect all name+family matches, then pick the right one.
    //
    // Two-pass logic: prefer a "real" cast (mana cost or cooldown). Fall back
    // to any match for free toggles like Aspect of the Hawk (Cata: no mana,
    // no cooldown, no cast time).
    //
    // Why: spells like "Deep Freeze" exist as both 44572 (the cast — 30s CD,
    // mana cost, applies stun) and 71757 (the damage trigger — no CD, no mana,
    // fired by 44572's script). Both are family MAGE, both named "Deep Freeze".
    // Without the castable filter the higher-SpellLevel one (the trigger) wins
    // and IsOnCooldown reads false every tick.
    struct Cand
    {
        uint32 spellId;
        uint32 spellLevel;
        bool   hasMana;
        bool   hasCooldown;
    };
    std::vector<Cand> cands;

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

        // Skip passive helpers that share the cast spell's name — e.g.
        // Molten Armor 34913 (the SPELL_ATTR0_PASSIVE on-attacker fire damage)
        // colliding with 30482 (the player-castable buff). The two-pass
        // "prefer castable" filter below doesn't disambiguate when neither has
        // mana cost or cooldown (Cata armor self-buffs are free toggles).
        if (info->IsPassive())
            continue;

        Cand c;
        c.spellId     = spellId;
        c.spellLevel  = info->SpellLevel;
        c.hasMana     = info->ManaCost > 0 || info->ManaCostPercentage > 0;
        c.hasCooldown = info->RecoveryTime > 0 || info->CategoryRecoveryTime > 0;
        cands.push_back(c);
    }

    if (cands.empty())
        return 0;

    // Diagnostic: when more than one candidate matches the same name we want
    // to know exactly what's in the spellmap and which one wins, so cache
    // resolution is debuggable from logs alone.
    if (cands.size() > 1)
    {
        std::ostringstream ss;
        ss << "FindSpellByFamilyName '" << name << "' (family=" << family
           << ") on '" << bot->GetName() << "' considered " << cands.size()
           << " candidates:";
        for (Cand const& c : cands)
            ss << " {id=" << c.spellId
               << " lvl=" << c.spellLevel
               << " mana=" << (c.hasMana ? 1 : 0)
               << " cd=" << (c.hasCooldown ? 1 : 0)
               << "}";
        TC_LOG_INFO("altbot", "%s", ss.str().c_str());
    }

    auto best = [&](bool requireCastable) -> uint32
    {
        uint32 bestId  = 0;
        uint32 bestLvl = 0;
        for (Cand const& c : cands)
        {
            if (requireCastable && !c.hasMana && !c.hasCooldown)
                continue;
            if (c.spellLevel >= bestLvl)
            {
                bestLvl = c.spellLevel;
                bestId  = c.spellId;
            }
        }
        return bestId;
    };

    if (uint32 id = best(true))
        return id;
    return best(false);
}

} // namespace StrategyUtil

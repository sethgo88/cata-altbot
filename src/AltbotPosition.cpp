#include "AltbotPosition.h"
#include "Cell.h"
#include "CellImpl.h"
#include "GridNotifiers.h"
#include "GridNotifiersImpl.h"
#include "MotionMaster.h"
#include "Player.h"
#include "Unit.h"
#include <list>

namespace AltbotPosition
{

void MaintainRange(Player* bot, Unit* target, float range)
{
    if (!bot || !target)
        return;

    // Only re-issue MoveChase when actually out of range. Each call mutates a
    // fresh ChaseMovementGenerator (replaces the active slot, re-initializes
    // pathing, flips UNIT_STATE_CHASE) — calling every tick while the bot is
    // already in range was producing SPELL_FAILED_MOVING (53) mid-cast on
    // 2-3s caster spells. The chase generator already holds at `range` once
    // started, so we just need to start it once.
    float dist = bot->GetDistance(target);
    if (dist <= range && bot->HasUnitState(UNIT_STATE_CHASE))
        return;

    bot->GetMotionMaster()->MoveChase(target, range);
}

int CountHostilesNear(Player* bot, float radius)
{
    if (!bot)
        return 0;

    std::list<Unit*> units;
    Trinity::AnyUnfriendlyUnitInObjectRangeCheck check(bot, bot, radius);
    Trinity::UnitListSearcher<Trinity::AnyUnfriendlyUnitInObjectRangeCheck> searcher(bot, units, check);
    Cell::VisitAllObjects(bot, searcher, radius);

    // Searcher already filters to alive + non-friendly + within range.
    // We additionally exclude critters/totems so a wisp doesn't trigger
    // "melee on caster" defensive moves.
    int count = 0;
    for (Unit* u : units)
    {
        if (!u || u->IsTotem() || u->IsCritter())
            continue;
        ++count;
    }
    return count;
}

int CountHostilesNearUnit(Player* bot, Unit* anchor, float radius)
{
    if (!bot || !anchor)
        return 0;

    std::list<Unit*> units;
    Trinity::AnyUnfriendlyUnitInObjectRangeCheck check(anchor, bot, radius);
    Trinity::UnitListSearcher<Trinity::AnyUnfriendlyUnitInObjectRangeCheck> searcher(anchor, units, check);
    Cell::VisitAllObjects(anchor, searcher, radius);

    int count = 0;
    for (Unit* u : units)
    {
        if (!u || u->IsTotem() || u->IsCritter())
            continue;
        ++count;
    }
    return count;
}

} // namespace AltbotPosition

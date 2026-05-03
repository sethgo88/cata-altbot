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

} // namespace AltbotPosition

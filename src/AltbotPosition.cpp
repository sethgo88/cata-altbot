#include "AltbotPosition.h"
#include "Cell.h"
#include "CellImpl.h"
#include "Creature.h"
#include "GridNotifiers.h"
#include "GridNotifiersImpl.h"
#include "MotionMaster.h"
#include "PathGenerator.h"
#include "Player.h"
#include "Unit.h"
#include <cmath>
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

void BackUpToRange(Player* bot, Unit* anchor, float desiredRange)
{
    if (!bot || !anchor)
        return;

    // Direction anchor → bot, normalized.
    float dx = bot->GetPositionX() - anchor->GetPositionX();
    float dy = bot->GetPositionY() - anchor->GetPositionY();
    float currentDist = std::sqrt(dx * dx + dy * dy);
    if (currentDist < 0.01f)
    {
        // Bot is directly on top of the anchor — pick a stable backward
        // direction from the bot's facing instead of dividing by ~zero.
        dx = std::cos(bot->GetOrientation() + float(M_PI));
        dy = std::sin(bot->GetOrientation() + float(M_PI));
        currentDist = 1.0f;
    }
    float scale = desiredRange / currentDist;
    float destX = anchor->GetPositionX() + dx * scale;
    float destY = anchor->GetPositionY() + dy * scale;
    float destZ = bot->GetPositionZ();
    bot->UpdateAllowedPositionZ(destX, destY, destZ);

    bot->GetMotionMaster()->MovePoint(0, destX, destY, destZ);
}

bool HasLineOfSight(Player* bot, Unit* target)
{
    if (!bot || !target)
        return false;
    return bot->IsWithinLOSInMap(target);
}

namespace
{
    // Wide grid scan around bot, returns hostile units (regardless of combat
    // state) so callers can re-filter. Search anchor must be a WorldObject;
    // we use the bot since ring samples are always within bot's map vicinity.
    void CollectNearbyHostiles(Player* bot, float radius, std::list<Unit*>& out)
    {
        Trinity::AnyUnfriendlyUnitInObjectRangeCheck check(bot, bot, radius);
        Trinity::UnitListSearcher<Trinity::AnyUnfriendlyUnitInObjectRangeCheck> searcher(bot, out, check);
        Cell::VisitAllObjects(bot, searcher, radius);
    }

    // Path validity: PATHFIND_NORMAL or PATHFIND_SHORTCUT only.
    // PATHFIND_INCOMPLETE / PATHFIND_NOPATH / FARFROMPOLY all reject so we
    // don't issue MovePoint into a wall, off a ledge, or across a chasm.
    bool IsPathReachable(Player* bot, float destX, float destY, float destZ)
    {
        if (!bot)
            return false;
        PathGenerator pg(bot);
        if (!pg.CalculatePath(destX, destY, destZ, false))
            return false;
        PathType type = pg.GetPathType();
        if (type & (PATHFIND_NOPATH | PATHFIND_INCOMPLETE | PATHFIND_FARFROMPOLY))
            return false;
        return (type & (PATHFIND_NORMAL | PATHFIND_SHORTCUT)) != 0;
    }
}

bool FindLOSPosition(Player* bot, Unit* target, float desiredRange,
                     float& outX, float& outY, float& outZ)
{
    if (!bot || !target)
        return false;

    float tx = target->GetPositionX();
    float ty = target->GetPositionY();
    // Anchor angle: from target toward bot, so sample 0 keeps us near where
    // we already are. Subsequent samples spiral outward via ±45°, ±90°, etc.
    float baseAngle = std::atan2(bot->GetPositionY() - ty, bot->GetPositionX() - tx);

    constexpr int   kSamples = 8;
    constexpr float kStep    = float(M_PI) / 4.0f; // 45°
    // Order: 0, +1, -1, +2, -2, +3, -3, +4 → traverses all 8 sectors with
    // preference for keeping the bot near its current angle.
    int order[kSamples] = {0, 1, -1, 2, -2, 3, -3, 4};

    for (int idx : order)
    {
        float angle = baseAngle + kStep * float(idx);
        float sx = tx + std::cos(angle) * desiredRange;
        float sy = ty + std::sin(angle) * desiredRange;
        float sz = bot->GetPositionZ();
        bot->UpdateAllowedPositionZ(sx, sy, sz);

        if (!target->IsWithinLOS(sx, sy, sz))
            continue;
        if (!IsPathReachable(bot, sx, sy, sz))
            continue;

        outX = sx;
        outY = sy;
        outZ = sz;
        return true;
    }
    return false;
}

int CountIdleHostilesNear(Player* bot, float x, float y, float z, float buffer)
{
    if (!bot)
        return 0;

    // Wide enough to cover any ring sample we'd test (max ~30y ring + 20y aggro).
    constexpr float kSearchRadius = 50.0f;
    std::list<Unit*> units;
    CollectNearbyHostiles(bot, kSearchRadius, units);

    int count = 0;
    for (Unit* u : units)
    {
        if (!u || u->IsTotem() || u->IsCritter())
            continue;
        if (!u->IsAlive())
            continue;
        if (u->IsInCombat())
            continue;
        if (!u->IsHostileTo(bot))
            continue;

        // Per-creature aggro radius; level-aware via TC. Non-creatures
        // (hostile players outside combat) get a flat 12y check.
        float aggro = 12.0f;
        if (Creature* c = u->ToCreature())
            aggro = c->GetAggroRange(bot);

        float dx = u->GetPositionX() - x;
        float dy = u->GetPositionY() - y;
        float dz = u->GetPositionZ() - z;
        float dist = std::sqrt(dx * dx + dy * dy + dz * dz);
        if (dist <= aggro + buffer)
            ++count;
    }
    return count;
}

bool IsPathSafe(Player* bot,
                float fromX, float fromY, float fromZ,
                float toX,   float toY,   float toZ)
{
    if (!bot)
        return false;

    // Sample 4 evenly-spaced points along the segment. End-points already
    // covered by the bot's current position and the destination's own
    // safety check at the caller, but we include them to be thorough.
    constexpr int kSamples = 5;
    for (int i = 0; i <= kSamples; ++i)
    {
        float t = float(i) / float(kSamples);
        float sx = fromX + (toX - fromX) * t;
        float sy = fromY + (toY - fromY) * t;
        float sz = fromZ + (toZ - fromZ) * t;
        if (CountIdleHostilesNear(bot, sx, sy, sz, 2.0f) > 0)
            return false;
    }
    return true;
}

bool FindSafeRetreatPosition(Player* bot, Unit* anchor, float desiredRange,
                             Unit* leashAnchor, float leashRange,
                             float& outX, float& outY, float& outZ)
{
    if (!bot || !anchor)
        return false;

    float ax = anchor->GetPositionX();
    float ay = anchor->GetPositionY();
    float bx = bot->GetPositionX();
    float by = bot->GetPositionY();

    constexpr int kSamples = 12;
    constexpr float kStep  = 2.0f * float(M_PI) / float(kSamples);

    int   bestScore = -1;
    float bestX = 0.0f, bestY = 0.0f, bestZ = 0.0f;

    for (int i = 0; i < kSamples; ++i)
    {
        float angle = kStep * float(i);
        float sx = ax + std::cos(angle) * desiredRange;
        float sy = ay + std::sin(angle) * desiredRange;
        float sz = bot->GetPositionZ();
        bot->UpdateAllowedPositionZ(sx, sy, sz);

        // Leash is a HARD constraint (not a score component). A sample that
        // violates the leash never qualifies — eat-the-fire fallback in the
        // caller relies on us returning false when no leash-compliant sample
        // exists.
        if (leashAnchor && leashRange > 0.0f)
        {
            float ldx = sx - leashAnchor->GetPositionX();
            float ldy = sy - leashAnchor->GetPositionY();
            if (std::sqrt(ldx * ldx + ldy * ldy) > leashRange)
                continue;
        }

        if (!IsPathReachable(bot, sx, sy, sz))
            continue;

        int score = 0;
        if (CountIdleHostilesNear(bot, sx, sy, sz, 2.0f) == 0) score += 4;
        if (anchor->IsWithinLOS(sx, sy, sz))                   score += 3;
        float ddx = sx - bx, ddy = sy - by;
        if (std::sqrt(ddx * ddx + ddy * ddy) > 3.0f)           score += 1;

        if (score > bestScore)
        {
            bestScore = score;
            bestX = sx; bestY = sy; bestZ = sz;
        }
    }

    if (bestScore < 0)
        return false;

    outX = bestX;
    outY = bestY;
    outZ = bestZ;
    return true;
}

} // namespace AltbotPosition

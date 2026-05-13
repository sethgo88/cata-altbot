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
#include <limits>
#include <list>

namespace AltbotPosition
{

namespace
{
    // A retreat / LOS / kite-out sample whose Z snaps more than this many
    // yards from the bot's current Z is almost certainly off a ledge: the
    // sample's XY landed in open space, and UpdateAllowedPositionZ pulled it
    // down to the next deck. Normal terrain bumps stay under 2y; platform
    // edges in Cata dungeons (Throne of the Tides upper deck → lower mire
    // floor) jump 40y+. 5y catches the bad case and tolerates real slopes.
    constexpr float kMaxRetreatDropY = 5.0f;

    bool ZDeltaSafe(Player* bot, float sampleZ)
    {
        if (!bot) return false;
        return std::fabs(sampleZ - bot->GetPositionZ()) <= kMaxRetreatDropY;
    }
}

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

    // Anchor-radial backup. If that direction snaps off a ledge, sweep
    // alternates so we don't stamp the bot 40y below the platform.
    // Throne of the Tides upper deck → lower mire floor was the canonical
    // failure: a hunter dead-zone backup picked a vector pointing at the
    // ledge, UpdateAllowedPositionZ snapped to the lower deck, and the bot
    // spent the rest of the fight firing LOS=49 shots from underneath.
    constexpr float kOffsets[] = { 0.0f, 0.524f, -0.524f, 1.047f, -1.047f }; // 0°, ±30°, ±60°
    float ax = anchor->GetPositionX();
    float ay = anchor->GetPositionY();
    float scale = desiredRange / currentDist;
    float baseDx = dx * scale;
    float baseDy = dy * scale;

    for (float off : kOffsets)
    {
        float cs = std::cos(off);
        float sn = std::sin(off);
        float destX = ax + baseDx * cs - baseDy * sn;
        float destY = ay + baseDx * sn + baseDy * cs;
        float destZ = bot->GetPositionZ();
        bot->UpdateAllowedPositionZ(destX, destY, destZ);

        if (!ZDeltaSafe(bot, destZ))
            continue;

        bot->GetMotionMaster()->MovePoint(0, destX, destY, destZ);
        return;
    }
    // No safe direction — better to stand still and eat pushback than to
    // walk off the deck. Strategy will retry next tick once the geometry
    // changes (target moves, bot rotates, etc.).
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

        if (!ZDeltaSafe(bot, sz))
            continue;
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

        // Z-delta is a HARD constraint, same priority as leash. A sample
        // whose Z snapped 40y down is over a ledge — picking it strands the
        // bot below the platform with LOS=49 on every shot. Better to eat
        // the fire than walk off.
        if (!ZDeltaSafe(bot, sz))
            continue;

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

bool FindStepOutOfPatch(Player* bot,
                        float patchX, float patchY, float patchZ,
                        float patchRadius,
                        Unit* leashAnchor, float leashRange,
                        float& outX, float& outY, float& outZ)
{
    if (!bot || patchRadius <= 0.0f)
        return false;

    // Step radius: just outside the patch with a small margin. 1.5y absorbs
    // float jitter, bot footprint, and one tick of drift before the move
    // resolves. Smaller margin = less displacement; we deliberately don't
    // overshoot.
    constexpr float kMargin = 1.5f;
    float ringRadius = patchRadius + kMargin;

    float bx = bot->GetPositionX();
    float by = bot->GetPositionY();

    // Anchor sample 0 along the (patch → bot) vector so the "step straight
    // out" geometry is sample 0. The rest spiral outward in alternating
    // ±directions from there. The min-displacement scoring will still
    // override if sample 0 violates a constraint and a different angle is
    // closer to current bot pos.
    float baseAngle = std::atan2(by - patchY, bx - patchX);

    constexpr int   kSamples = 12;
    constexpr float kStep    = 2.0f * float(M_PI) / float(kSamples);
    // Order: 0, +1, -1, +2, -2, ..., +6. Sample 0 = straight out.
    int order[kSamples];
    order[0] = 0;
    for (int i = 1; i <= 5; ++i) { order[2*i-1] = i; order[2*i] = -i; }
    order[11] = 6;

    float bestDisp = std::numeric_limits<float>::max();
    float bestX = 0.0f, bestY = 0.0f, bestZ = 0.0f;
    bool  haveSample = false;

    for (int idx : order)
    {
        float angle = baseAngle + kStep * float(idx);
        float sx = patchX + std::cos(angle) * ringRadius;
        float sy = patchY + std::sin(angle) * ringRadius;
        float sz = bot->GetPositionZ();
        bot->UpdateAllowedPositionZ(sx, sy, sz);

        if (!ZDeltaSafe(bot, sz))
            continue;

        if (leashAnchor && leashRange > 0.0f)
        {
            float ldx = sx - leashAnchor->GetPositionX();
            float ldy = sy - leashAnchor->GetPositionY();
            if (std::sqrt(ldx * ldx + ldy * ldy) > leashRange)
                continue;
        }

        if (!IsPathReachable(bot, sx, sy, sz))
            continue;

        // Don't step into an idle pack we'd then aggro.
        if (CountIdleHostilesNear(bot, sx, sy, sz, 2.0f) > 0)
            continue;

        float ddx = sx - bx;
        float ddy = sy - by;
        float disp = std::sqrt(ddx * ddx + ddy * ddy);
        if (disp < bestDisp)
        {
            bestDisp = disp;
            bestX = sx; bestY = sy; bestZ = sz;
            haveSample = true;
        }
    }

    (void)patchZ; // currently unused; the bot's Z + UpdateAllowedPositionZ is
                  // authoritative for the sample. Kept in the signature so a
                  // future multi-deck patch (Z-aware) can use it without API
                  // churn.

    if (!haveSample)
        return false;
    outX = bestX;
    outY = bestY;
    outZ = bestZ;
    return true;
}

} // namespace AltbotPosition

#pragma once

class Player;
class Unit;

namespace AltbotPosition
{
    // Drive ranged combat positioning: chase target keeping `range` distance.
    // Bots far away will close to range; bots in range stop and stand for casts.
    // Safe to call every combat tick — MotionMaster::MoveChase is idempotent
    // when the same target/range are already active.
    void MaintainRange(Player* bot, Unit* target, float range);

    // Count hostile, alive units (creatures + players) within `radius` of bot.
    // Used by defensive triggers: melee-on-caster detection for Frost Nova,
    // Disengage, Demonic Circle Teleport, Howl of Terror.
    int CountHostilesNear(Player* bot, float radius);

    // Count hostiles within `radius` of `anchor` (typically the bot's target).
    // For ranged AoE detection from 25y range — CountHostilesNear measures around
    // the bot, which always reads 0 for casters standing back from the cluster.
    // Faction reference is the bot.
    int CountHostilesNearUnit(Player* bot, Unit* anchor, float radius);

    // Move the bot to `desiredRange` yards from `anchor`, along the line
    // anchor→bot (i.e. away from anchor). Used by ranged DPS to escape the
    // melee dead-zone — hunter shots fail with SPELL_FAILED_TOO_CLOSE (130)
    // on this server when the target is within ~8y. Issued via MovePoint,
    // replacing any active chase generator.
    void BackUpToRange(Player* bot, Unit* anchor, float desiredRange);

    // True when bot has line-of-sight to target. Wraps WorldObject::IsWithinLOSInMap
    // with default checks. Cheap-but-not-free (VMAP raycast); manager throttles.
    bool HasLineOfSight(Player* bot, Unit* target);

    // Sample 8 points on a ring of radius `desiredRange` around `target`. For
    // each: snap Z to terrain, validate target-LOS, validate reachability via
    // PathGenerator. Returns first passing sample in (outX/Y/Z); false if none.
    // Start from the angle closest to current target→bot vector and rotate
    // outward in alternating ±directions (prefers staying near current spot).
    bool FindLOSPosition(Player* bot, Unit* target, float desiredRange,
                         float& outX, float& outY, float& outZ);

    // Count hostile creatures within their own (level-aware) aggro radius of
    // `point`, filtered to alive + not-in-combat + hostile-to-bot. `buffer` is
    // added to each creature's GetAggroRange(bot) for slack.
    // For idle-pack avoidance: a path crossing any nonzero count is unsafe.
    int CountIdleHostilesNear(Player* bot, float x, float y, float z, float buffer);

    // Sample 4 evenly-spaced points along (from→to) segment; reject if any
    // sample sees an idle hostile within its aggro range + 2y. Conservative
    // by design: false-positive = extra second of pathing, false-negative = wipe.
    bool IsPathSafe(Player* bot,
                    float fromX, float fromY, float fromZ,
                    float toX,   float toY,   float toZ);

    // Sample 12 ring points around `anchor` at `desiredRange`; score each on
    // {outside-idle-packs, has-LOS-to-anchor, inside-leash, moved-far-enough}.
    // Picks the highest-scoring sample. If best sample violates the leash,
    // returns false — caller should "eat the fire" rather than break the leash.
    bool FindSafeRetreatPosition(Player* bot, Unit* anchor, float desiredRange,
                                 Unit* leashAnchor, float leashRange,
                                 float& outX, float& outY, float& outZ);

    // Sample 12 ring points around the *patch center* (`patchX/Y/Z`) at
    // `patchRadius + margin`. Each sample by construction is just outside
    // the patch footprint, so any choice clears the hazard. Pick the one
    // with the smallest displacement from the bot's current position so
    // movement is minimal — for ground patches near the encounter target,
    // this naturally produces sidestep/orbital movement instead of a long
    // run across the boss. Same hard constraints as FindSafeRetreatPosition
    // (Z-delta, leash, reachable). Returns false when no constraint-
    // compliant sample exists; caller falls back to bot-anchored retreat.
    bool FindStepOutOfPatch(Player* bot,
                            float patchX, float patchY, float patchZ,
                            float patchRadius,
                            Unit* leashAnchor, float leashRange,
                            float& outX, float& outY, float& outZ);
}

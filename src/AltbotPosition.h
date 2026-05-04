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
}

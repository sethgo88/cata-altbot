#pragma once
#include <cstdint>

class AltbotPositionManager;

// Per-tick state passed from AltbotAI to AltbotStrategy::Update.
// Built fresh each tick from bot/master observation; not persisted.
struct AltbotTickContext
{
    uint32 combatElapsedMs = 0;  // 0 when master is not in combat; counts up while in combat
    bool   inDungeon       = false;
    bool   inRaid          = false;

    // Per-bot positioning manager — strategies declare a PositionIntent each
    // tick via positionManager->SetIntent(...) before tier dispatch. Manager
    // owns the actual MoveChase/MovePoint calls (LOS recovery, dead-zone,
    // fire retreat, leash, idle-pack avoidance). Always non-null in normal
    // flow; AltbotAI populates it before invoking AltbotCombat::Update.
    AltbotPositionManager* positionManager = nullptr;

    bool InInstance() const { return inDungeon || inRaid; }
};

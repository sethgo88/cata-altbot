#pragma once
#include <cstdint>

// Per-tick state passed from AltbotAI to AltbotStrategy::Update.
// Built fresh each tick from bot/master observation; not persisted.
struct AltbotTickContext
{
    uint32 combatElapsedMs = 0;  // 0 when master is not in combat; counts up while in combat
    bool   inDungeon       = false;
    bool   inRaid          = false;

    bool InInstance() const { return inDungeon || inRaid; }
};

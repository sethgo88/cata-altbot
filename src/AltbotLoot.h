#pragma once

class Player;

// Auto-loot of nearby corpses the bot is allowed to take from.
// Currently a stub — the actual loot transfer (scan-and-grab + tap validation)
// lands in Phase 5 alongside the OnLootRoll auto-pass hook.
namespace AltbotLoot
{
    void Tick(Player* bot, Player* master);
}

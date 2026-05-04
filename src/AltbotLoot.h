#pragma once

class Player;
struct AltbotState;

// Auto-loot of nearby corpses the bot is allowed to take from + auto-voting on
// pending group-loot rolls. Both behaviors gate internally on AltbotState
// (state.autoLoot for corpse loot, state.lootRoll for roll voting), so the
// caller can fire Tick unconditionally.
namespace AltbotLoot
{
    void Tick(Player* bot, Player* master, AltbotState const& state);
}

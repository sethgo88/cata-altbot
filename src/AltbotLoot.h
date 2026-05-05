#pragma once

class Player;
class AltbotAI;
struct AltbotState;

// Auto-loot of nearby corpses the bot is allowed to take from + auto-voting on
// pending group-loot rolls. Both behaviors gate internally on AltbotState
// (state.autoLoot for corpse loot, state.lootRoll for roll voting), so the
// caller can fire Tick unconditionally.
//
// TickPending drives the master-initiated single-corpse loot task (addon LOOT
// verb): walk the bot to the targeted corpse, then drain gold + items. The
// task is one-shot; it self-clears on success, target disappearance, or
// timeout. Validation (lootability, tap eligibility) happens upfront in the
// LOOT handler so the master sees an immediate ERR.
namespace AltbotLoot
{
    void Tick(Player* bot, Player* master, AltbotState const& state);
    void TickPending(Player* bot, Player* master, AltbotAI& ai, uint32 diff);
}

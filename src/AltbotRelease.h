#pragma once

class Player;

// Auto-revives a dead bot at the master's location.
// Gated by AltbotState::autoRelease. Polled — no death hook required.
namespace AltbotRelease
{
    void Tick(Player* bot, Player* master);
}

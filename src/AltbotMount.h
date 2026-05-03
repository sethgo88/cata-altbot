#pragma once

class Player;

// Mirrors master's mounted state: when master mounts, bot mounts; when master
// dismounts, bot dismounts. Gated by AltbotState::autoMount.
namespace AltbotMount
{
    void Tick(Player* bot, Player* master);
}

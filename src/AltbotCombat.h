#pragma once
#include "AltbotState.h"

class Player;

namespace AltbotCombat
{
    void Update(Player* bot, Player* master, AltbotState const& state);
}

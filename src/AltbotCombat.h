#pragma once
#include "AltbotState.h"

class AltbotStrategy;
class Player;

namespace AltbotCombat
{
    void Update(Player* bot, Player* master, AltbotState const& state, AltbotStrategy* strategy);
}

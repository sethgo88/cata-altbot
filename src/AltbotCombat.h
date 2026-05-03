#pragma once
#include "AltbotState.h"

class AltbotStrategy;
class Player;
struct AltbotTickContext;

namespace AltbotCombat
{
    void Update(Player* bot, Player* master, AltbotState const& state,
                AltbotTickContext const& ctx, AltbotStrategy* strategy);
}

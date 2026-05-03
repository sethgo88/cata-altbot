#pragma once
#include "AltbotState.h"

class Player;
class Unit;

// Selects an attack target for the bot based on AltbotAssistMode:
//   Off          -> nullptr
//   MasterTarget -> master->GetVictim() or master's hostile selection
//   SkullOnly    -> the unit currently flagged with the skull raid icon (slot 7)
//   Both         -> SkullOnly first, fallback to MasterTarget
namespace AltbotAssist
{
    Unit* SelectTarget(Player* bot, Player* master, AltbotAssistMode mode);
}

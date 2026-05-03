#pragma once
#include <memory>
#include <string>

class AltbotStrategy;
class Player;

namespace AltbotStrategyFactory
{
    // Returns a concrete strategy for the bot based on its class + talent tree,
    // or specOverride slug if non-empty. Returns nullptr if no strategy exists
    // for this spec (caller falls back to generic logic).
    std::unique_ptr<AltbotStrategy> Create(Player* bot, std::string const& specOverride);
}

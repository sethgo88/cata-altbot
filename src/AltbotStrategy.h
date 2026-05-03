#pragma once
#include "AltbotTickContext.h"
class Player;

class AltbotStrategy
{
public:
    virtual ~AltbotStrategy() = default;
    virtual void Update(Player* bot, Player* master, AltbotTickContext const& ctx) = 0;
    virtual char const* GetName() const = 0;
};

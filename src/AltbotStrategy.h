#pragma once
class Player;

class AltbotStrategy
{
public:
    virtual ~AltbotStrategy() = default;
    virtual void Update(Player* bot, Player* master) = 0;
    virtual char const* GetName() const = 0;
};

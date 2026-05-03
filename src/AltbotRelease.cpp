#include "AltbotRelease.h"
#include "Log.h"
#include "Player.h"

namespace AltbotRelease
{

void Tick(Player* bot, Player* master)
{
    // Skip the live case fast.
    if (bot->IsAlive())
        return;

    // ResurrectPlayer revives at the bot's current location with the supplied
    // health fraction; teleporting to master after avoids leaving the bot at
    // the corpse / graveyard.
    bot->ResurrectPlayer(1.0f);
    bot->SpawnCorpseBones();

    bot->TeleportTo(master->GetMapId(),
                    master->GetPositionX(),
                    master->GetPositionY(),
                    master->GetPositionZ(),
                    master->GetOrientation());

    TC_LOG_INFO("altbot", "AltbotRelease: '%s' auto-revived at master '%s'.",
        bot->GetName().c_str(), master->GetName().c_str());
}

} // namespace AltbotRelease

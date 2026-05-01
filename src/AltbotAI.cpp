#include "AltbotAI.h"
#include "AltbotFollow.h"
#include "ObjectAccessor.h"
#include "Player.h"
#include "WorldSession.h"

static constexpr uint32 FOLLOW_INTERVAL_MS = 1000;

AltbotAI::AltbotAI(WorldSession* botSession, ObjectGuid masterGuid)
    : _botSession(botSession), _masterGuid(masterGuid), _followTimer(FOLLOW_INTERVAL_MS)
{
}

void AltbotAI::Update(uint32 diff)
{
    // Bot player may still be loading from DB — wait until it's in the world
    Player* bot = _botSession->GetPlayer();
    if (!bot || !bot->IsInWorld())
        return;

    _followTimer = (_followTimer > diff) ? _followTimer - diff : 0;
    if (_followTimer > 0)
        return;
    _followTimer = FOLLOW_INTERVAL_MS;

    Player* master = ObjectAccessor::FindPlayer(_masterGuid);
    if (!master || !master->IsInWorld())
        return;

    AltbotFollow::Update(bot, master);
}

#include "AltbotAI.h"
#include "AltbotCombat.h"
#include "AltbotFollow.h"
#include "ObjectAccessor.h"
#include "Player.h"
#include "WorldSession.h"

static constexpr uint32 FOLLOW_INTERVAL_MS = 1000;
static constexpr uint32 COMBAT_INTERVAL_MS = 1500;

AltbotAI::AltbotAI(WorldSession* botSession, ObjectGuid masterGuid)
    : _botSession(botSession), _masterGuid(masterGuid),
      _followTimer(FOLLOW_INTERVAL_MS), _combatTimer(COMBAT_INTERVAL_MS),
      _mode(AltbotMode::Follow)
{
}

void AltbotAI::Update(uint32 diff)
{
    // Bot player may still be loading from DB — wait until it's in the world
    Player* bot = _botSession->GetPlayer();
    if (!bot || !bot->IsInWorld())
        return;

    Player* master = ObjectAccessor::FindPlayer(_masterGuid);
    if (!master || !master->IsInWorld())
        return;

    _followTimer = (_followTimer > diff) ? _followTimer - diff : 0;
    if (_followTimer == 0)
    {
        _followTimer = FOLLOW_INTERVAL_MS;
        if (_mode == AltbotMode::Follow)
            AltbotFollow::Update(bot, master);
    }

    _combatTimer = (_combatTimer > diff) ? _combatTimer - diff : 0;
    if (_combatTimer == 0)
    {
        _combatTimer = COMBAT_INTERVAL_MS;
        AltbotCombat::Update(bot, master);
    }
}

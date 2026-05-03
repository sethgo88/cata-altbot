#include "AltbotAI.h"
#include "AltbotCombat.h"
#include "AltbotFollow.h"
#include "AltbotLoot.h"
#include "AltbotMgr.h"
#include "AltbotMount.h"
#include "AltbotRelease.h"
#include "ObjectAccessor.h"
#include "Player.h"
#include "WorldSession.h"

static constexpr uint32 FOLLOW_INTERVAL_MS = 1000;
static constexpr uint32 COMBAT_INTERVAL_MS = 1500;

AltbotAI::AltbotAI(WorldSession* botSession, ObjectGuid masterGuid, ObjectGuid botGuid)
    : _botSession(botSession), _masterGuid(masterGuid), _botGuid(botGuid),
      _followTimer(FOLLOW_INTERVAL_MS), _combatTimer(COMBAT_INTERVAL_MS),
      _state()
{
}

void AltbotAI::MutateState(std::function<void(AltbotState&)> const& fn)
{
    fn(_state);
    sAltbotMgr->PersistState(*this);
}

void AltbotAI::SetMode(AltbotMode mode)
{
    MutateState([mode](AltbotState& s) { s.mode = mode; });
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

        // Release runs first so a freshly revived bot uses the same tick to follow.
        if (_state.autoRelease && !bot->IsAlive())
            AltbotRelease::Tick(bot, master);

        if (_state.autoMount && bot->IsAlive())
            AltbotMount::Tick(bot, master);

        if (_state.autoLoot && bot->IsAlive())
            AltbotLoot::Tick(bot, master);

        if (_state.mode == AltbotMode::Follow && bot->IsAlive())
            AltbotFollow::Update(bot, master);
    }

    _combatTimer = (_combatTimer > diff) ? _combatTimer - diff : 0;
    if (_combatTimer == 0)
    {
        _combatTimer = COMBAT_INTERVAL_MS;
        if (bot->IsAlive())
            AltbotCombat::Update(bot, master, _state);
    }
}

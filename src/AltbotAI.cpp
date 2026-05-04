#include "AltbotAI.h"
#include "AltbotCombat.h"
#include "AltbotFollow.h"
#include "AltbotInvite.h"
#include "AltbotLfg.h"
#include "AltbotLoot.h"
#include "AltbotMgr.h"
#include "AltbotMount.h"
#include "AltbotRelease.h"
#include "AltbotTickContext.h"
#include "Log.h"
#include "Map.h"
#include "MotionMaster.h"
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

void AltbotAI::SetSpecOverride(std::string const& spec)
{
    _specOverride      = spec;
    _strategy.reset();
    _strategyResolved  = false;
}

void AltbotAI::Update(uint32 diff)
{
    Player* bot = _botSession->GetPlayer();
    if (!bot)
        return;

    // Bots have no client to send teleport acks. If anything (LFG dungeon
    // teleport, .tele, scripted teleport, etc.) put the bot into a far-port
    // semaphore state, we drive the ack ourselves so the map swap completes
    // and IsInWorld() flips back to true.
    if (bot->IsBeingTeleportedFar())
        _botSession->HandleMoveWorldportAck();

    // Bot player may still be loading from DB — wait until it's in the world
    if (!bot->IsInWorld())
        return;

    Player* master = ObjectAccessor::FindPlayer(_masterGuid);
    if (!master || !master->IsInWorld())
        return;

    // Run the post-spawn auto invite + summon once both ends are loaded.
    // This is what makes a freshly logged-in bot land in your group at your
    // feet without you having to whisper invite/come.
    if (_pendingAutoInviteSummon)
    {
        _pendingAutoInviteSummon = false;
        AltbotInvite::Invite(master, this);
        AltbotInvite::Summon(master, this);
    }

    if (!_strategyResolved)
    {
        _strategy         = AltbotStrategyFactory::Create(bot, _specOverride);
        _strategyResolved = true;

        if (_strategy)
            TC_LOG_INFO("altbot",
                "AltbotAI: '%s' strategy='%s' (class=%u tree=%u override='%s')",
                bot->GetName().c_str(), _strategy->GetName(),
                bot->getClass(),
                bot->GetPrimaryTalentTree(bot->GetActiveSpec()),
                _specOverride.c_str());
        else
            TC_LOG_INFO("altbot",
                "AltbotAI: '%s' no strategy match (class=%u tree=%u override='%s'); "
                "falling back to generic damage scan",
                bot->GetName().c_str(), bot->getClass(),
                bot->GetPrimaryTalentTree(bot->GetActiveSpec()),
                _specOverride.c_str());
    }

    // Track combat enter/elapsed so the threat-window gate and follow gating
    // both see the same value. Rising edge resets the timer to 0; falling edge
    // (master leaves combat) clears both fields so the next pull starts fresh.
    if (master->IsInCombat())
    {
        if (_wasInCombat)
            _combatElapsedMs += diff;
        else
        {
            _combatElapsedMs = 0;
            _wasInCombat     = true;
        }
    }
    else
    {
        _wasInCombat     = false;
        _combatElapsedMs = 0;
    }

    AltbotTickContext ctx;
    ctx.combatElapsedMs = _combatElapsedMs;
    if (Map* map = bot->GetMap())
    {
        ctx.inDungeon = map->IsDungeon() && !map->IsRaid();
        ctx.inRaid    = map->IsRaid();
    }

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

        // LFG rolecheck/proposal accepts must fire even if the bot is dead —
        // a dead-but-in-world bot can still be in a queued group.
        AltbotLfg::Tick(bot, this);

        // In a dungeon or raid, hand positioning to the strategy once combat
        // starts so ranged casters don't get cleaved sitting on the master's
        // back. Out of instance, follow always (existing behavior).
        bool followGated = ctx.InInstance() && master->IsInCombat();

        // Rising edge: a MoveFollow was queued before the gate tripped and
        // MotionMaster keeps it active until something replaces it. Drop to
        // idle once so the bot stops chasing master toward the tank pile;
        // strategies with MaintainRange (casters/hunters) immediately push
        // their own MoveChase, healers/melee stay put until combat ends.
        if (followGated && !_followGatedLast && bot->IsAlive())
            bot->GetMotionMaster()->MoveIdle();
        _followGatedLast = followGated;

        if (_state.mode == AltbotMode::Follow && bot->IsAlive() && !followGated)
            AltbotFollow::Update(bot, master);
    }

    _combatTimer = (_combatTimer > diff) ? _combatTimer - diff : 0;
    if (_combatTimer == 0)
    {
        _combatTimer = COMBAT_INTERVAL_MS;
        if (bot->IsAlive())
            AltbotCombat::Update(bot, master, _state, ctx, _strategy.get());
    }
}

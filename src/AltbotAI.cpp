#include "AltbotAI.h"
#include "AltbotCombat.h"
#include "AltbotCombatLog.h"
#include "AltbotFollow.h"
#include "AltbotInvite.h"
#include "AltbotLfg.h"
#include "AltbotLoot.h"
#include "AltbotMgr.h"
#include "AltbotMount.h"
#include "AltbotQuest.h"
#include "AltbotRelease.h"
#include "AltbotTickContext.h"
#include "Creature.h"
#include "Log.h"
#include "Map.h"
#include "MotionMaster.h"
#include "ObjectAccessor.h"
#include "Player.h"
#include "Timer.h"
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
    if (_dead)
        return;

    // Trust the cached session pointer here. In this fork bot sessions live
    // in World::m_altbotSessions (keyed by character guid) and are never
    // destroyed via the standard account-collision eviction path. The
    // PlayerScript::OnLogout hook in AltbotLoader removes this AI from
    // AltbotMgr::_activeBots synchronously when LogoutPlayer fires (TC's
    // sScriptMgr->OnPlayerLogout call inside LogoutPlayer runs before the
    // WorldSession destructor would invalidate _botSession), so if
    // AltbotMgr::Update is still calling us, _botSession is valid.
    Player* bot = _botSession->GetPlayer();
    if (!bot)
    {
        // GetPlayer() returns null in two states:
        //   (a) The bot's async HandlePlayerLogin hasn't yet attached the
        //       Player to its session. PlayerLoading() returns true during
        //       this window. Skip the tick — the next one will see it
        //       attached. *Do not* mark dead here, or `.altbot add` will
        //       kill the AI before the bot ever finishes loading.
        //   (b) The session has been logged out via a path that did not fire
        //       OnPlayerLogout (e.g. session destroyed before its Player
        //       ever attached, so LogoutPlayer was never reached). Mark
        //       dead so AltbotMgr::Update reaps us. This is the genuine
        //       "session is gone" case the safety net is for.
        if (_botSession->PlayerLoading())
            return;

        // (b) — log on the rising edge only.
        if (!_dead)
            TC_LOG_WARN("altbot",
                "AltbotAI::Update: bot %s (master %s) has no Player and is not "
                "in PlayerLoading state — session looks logged out without our "
                "OnLogout hook firing (likely destroyed before its Player "
                "attached). Marking dead for AltbotMgr reap.",
                _botGuid.ToString().c_str(), _masterGuid.ToString().c_str());
        _dead = true;
        return;
    }

    // Bots have no client to send teleport acks. If anything (LFG dungeon
    // teleport, .tele, scripted teleport, etc.) put the bot into a far-port
    // semaphore state, we drive the ack ourselves so the map swap completes
    // and IsInWorld() flips back to true.
    if (bot->IsBeingTeleportedFar())
        _botSession->HandleMoveWorldportAck();

    // Bot player may still be loading from DB — wait until it's in the world
    if (!bot->IsInWorld())
        return;

    // Position manager is constructed before bot exists; bind on first valid
    // tick (and reset HP-delta state). Re-bind each tick afterwards so a
    // re-spawned Player* never leaves a stale pointer in the manager.
    if (!_positionManagerBound)
    {
        _positionManager.Bind(bot);
        _positionManager.Reset();
        _positionManagerBound = true;
    }
    else
    {
        _positionManager.Bind(bot);
    }

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
            // Rising edge — open a fight-summary window in the telemetry sink.
            AltbotCombatLog::OnCombatEnter(bot);
        }
    }
    else
    {
        if (_wasInCombat)
        {
            // Falling edge — close the window with the elapsed total. The
            // telemetry sink decides whether to whisper / log based on
            // MinFightMs and the per-bot trace flag.
            AltbotCombatLog::OnCombatLeave(bot, master, _combatElapsedMs);
        }
        _wasInCombat     = false;
        _combatElapsedMs = 0;
    }

    AltbotTickContext ctx;
    ctx.combatElapsedMs  = _combatElapsedMs;
    ctx.positionManager  = &_positionManager;
    if (Map* map = bot->GetMap())
    {
        ctx.inDungeon = map->IsDungeon() && !map->IsRaid();
        ctx.inRaid    = map->IsRaid();
    }

    // Phase 3: surface boss-state from master's current target. Most fights
    // have the tank/master targeting the boss; reading their target gives
    // us a reasonable bossNpcEntry / bossHpPct without hand-rolling
    // per-instance script bindings. Phase derivation lives in
    // EncounterPhases::Resolve and is deferred until the table grows past a
    // handful of entries.
    if (ctx.InInstance() && master->IsInCombat())
    {
        if (Unit* mTarget = master->GetSelectedUnit())
        {
            if (Creature* c = mTarget->ToCreature())
            {
                bool bossish = c->IsDungeonBoss() || c->isWorldBoss()
                               || c->GetCreatureTemplate()->rank == CREATURE_ELITE_WORLDBOSS
                               || c->GetCreatureTemplate()->rank == CREATURE_ELITE_RAREELITE;
                if (bossish)
                {
                    ctx.bossNpcEntry = c->GetEntry();
                    ctx.bossHpPct    = uint8(c->GetHealthPct());
                }
            }
        }
    }

    uint32 nowMs = getMSTime();

    // Decrement the post-summon pin so combat + follow ticks below skip
    // for the configured duration after a Summon. This is what makes
    // in-combat summons actually stick — without the pin, MaintainRange
    // re-chases the master's old target ~1.5s after the teleport.
    if (_summonPinRemainingMs > 0)
        _summonPinRemainingMs = (_summonPinRemainingMs > diff) ? _summonPinRemainingMs - diff : 0;

    // Manual loot task (addon LOOT verb). Runs ahead of follow/combat and
    // short-circuits both while active — the corpse walk would otherwise
    // fight MaintainRange and the follow generator. Internally clears the
    // pending guid when the loot is drained, the target disappears, or the
    // task times out.
    if (!_pendingLootTarget.IsEmpty() && bot->IsAlive())
    {
        AltbotLoot::TickPending(bot, master, *this, diff);
        if (!_pendingLootTarget.IsEmpty())
            return;   // still walking / draining; skip the rest of this tick
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

        // Both autoLoot (corpse pickup) and lootRoll (roll voting) gate
        // internally; AltbotLoot::Tick is a single entry point covering both.
        if (bot->IsAlive())
            AltbotLoot::Tick(bot, master, _state);

        // LFG rolecheck/proposal accepts must fire even if the bot is dead —
        // a dead-but-in-world bot can still be in a queued group.
        AltbotLfg::Tick(bot, this);

        // Quest progress polling: detects objective increments and the
        // INCOMPLETE→COMPLETE transition (CompleteQuest doesn't fire
        // OnQuestStatusChange). Whispers master via AltbotQuest helpers.
        AltbotQuest::TickProgress(*this, _questSnapshot);

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

        if (_state.mode == AltbotMode::Follow && bot->IsAlive() && !followGated
            && _summonPinRemainingMs == 0)
            AltbotFollow::Update(bot, master);

        // Fast positioning pass: only samples HP for the reactive-fire
        // detector. Lets us trip the fire flag at 1000ms cadence while the
        // expensive Tick (LOS / chase / leash) stays on the 1500ms combat tick.
        if (bot->IsAlive() && followGated && _summonPinRemainingMs == 0)
            _positionManager.FastTick(master, nowMs);
    }

    _combatTimer = (_combatTimer > diff) ? _combatTimer - diff : 0;
    if (_combatTimer == 0)
    {
        _combatTimer = COMBAT_INTERVAL_MS;
        if (bot->IsAlive() && _summonPinRemainingMs == 0)
            AltbotCombat::Update(bot, master, _state, ctx, _strategy.get());
    }
}

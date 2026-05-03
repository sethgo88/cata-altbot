#pragma once
#include "AltbotState.h"
#include "AltbotStrategy.h"
#include "AltbotStrategyFactory.h"
#include "ObjectGuid.h"
#include <cstdint>
#include <functional>
#include <memory>
#include <string>

class WorldSession;

class AltbotAI
{
public:
    AltbotAI(WorldSession* botSession, ObjectGuid masterGuid, ObjectGuid botGuid);

    void Update(uint32 diff);

    WorldSession* GetSession()    const { return _botSession; }
    ObjectGuid    GetMasterGuid() const { return _masterGuid; }
    ObjectGuid    GetBotGuid()    const { return _botGuid; }

    AltbotState const& GetState() const { return _state; }
    AltbotState&       MutableState()    { return _state; }

    // Apply a mutation to _state and persist the result via AltbotMgr::PersistState.
    // Use this for any state change driven by a command — never poke _state directly.
    void MutateState(std::function<void(AltbotState&)> const& fn);

    AltbotMode GetMode() const          { return _state.mode; }
    void       SetMode(AltbotMode mode);

    // Force a spec slug (e.g. "resto-shaman") instead of auto-detecting from
    // the talent tree. Empty string reverts to auto-detect. The strategy is
    // re-resolved on the next combat tick.
    void SetSpecOverride(std::string const& spec);
    std::string const& GetSpecOverride() const { return _specOverride; }

    // Set by AltbotMgr after a fresh spawn (AddAltbot / LoginBot). On the
    // first Update tick where both bot and master are in-world, the AI runs
    // AltbotInvite::Invite + AltbotInvite::Summon and clears the flag.
    void MarkPendingAutoInviteSummon() { _pendingAutoInviteSummon = true; }

    // Latch flipped once per LFG rolecheck so AltbotLfg::Tick fires the
    // UpdateRoleCheck call exactly once and resets when the rolecheck ends.
    bool HasLfgRoleResponded() const { return _lfgRoleResponded; }
    void MarkLfgRoleResponded()      { _lfgRoleResponded = true;  }
    void ClearLfgRoleResponded()     { _lfgRoleResponded = false; }

private:
    WorldSession* _botSession;    // non-owning; owned by World
    ObjectGuid    _masterGuid;
    ObjectGuid    _botGuid;       // stable across the bot's async login
    uint32        _followTimer;   // ms remaining until next follow check
    uint32        _combatTimer;   // ms remaining until next combat check
    AltbotState   _state;

    std::unique_ptr<AltbotStrategy> _strategy;
    bool        _strategyResolved = false;
    std::string _specOverride;

    bool        _pendingAutoInviteSummon = false;
    bool        _lfgRoleResponded        = false;
};

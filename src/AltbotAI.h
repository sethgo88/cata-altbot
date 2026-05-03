#pragma once
#include "AltbotState.h"
#include "ObjectGuid.h"
#include <cstdint>
#include <functional>

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

private:
    WorldSession* _botSession;    // non-owning; owned by World
    ObjectGuid    _masterGuid;
    ObjectGuid    _botGuid;       // stable across the bot's async login
    uint32        _followTimer;   // ms remaining until next follow check
    uint32        _combatTimer;   // ms remaining until next combat check
    AltbotState   _state;
};

#pragma once
#include "ObjectGuid.h"
#include <cstdint>

class WorldSession;

enum class AltbotMode
{
    Follow,
    Stay,
};

class AltbotAI
{
public:
    AltbotAI(WorldSession* botSession, ObjectGuid masterGuid);

    void Update(uint32 diff);

    WorldSession* GetSession() const { return _botSession; }
    ObjectGuid GetMasterGuid() const { return _masterGuid; }

    AltbotMode GetMode() const { return _mode; }
    void SetMode(AltbotMode mode) { _mode = mode; }

private:
    WorldSession* _botSession;    // non-owning; owned by World
    ObjectGuid    _masterGuid;
    uint32        _followTimer;   // ms remaining until next follow check
    uint32        _combatTimer;   // ms remaining until next combat check
    AltbotMode    _mode;
};

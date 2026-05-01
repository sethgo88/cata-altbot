#pragma once
#include "ObjectGuid.h"
#include <cstdint>

class WorldSession;

class AltbotAI
{
public:
    AltbotAI(WorldSession* botSession, ObjectGuid masterGuid);

    void Update(uint32 diff);

    WorldSession* GetSession() const { return _botSession; }
    ObjectGuid GetMasterGuid() const { return _masterGuid; }

private:
    WorldSession* _botSession;   // non-owning; owned by World
    ObjectGuid    _masterGuid;
    uint32        _followTimer;  // ms remaining until next follow check
};

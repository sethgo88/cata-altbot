#pragma once
#include "Define.h"
#include "ObjectGuid.h"
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

class AltbotAI;
class Player;
class WorldSession;

class AltbotMgr
{
public:
    static AltbotMgr* instance();

    bool AddAltbot(Player* master, std::string const& botName);
    void RemoveAltbot(Player* master, std::string const& botName);
    void Update(uint32 diff);

    AltbotAI* FindBotAI(ObjectGuid masterGuid, ObjectGuid botGuid);
    void SpawnBotsForMaster(ObjectGuid masterGuid);

private:
    AltbotMgr() = default;

    bool SpawnBot(ObjectGuid masterGuid, ObjectGuid botGuid);

    // masterGuid.GetRawValue() -> active bot AIs owned by that master
    std::unordered_map<uint64, std::vector<std::unique_ptr<AltbotAI>>> _activeBots;
};

#define sAltbotMgr AltbotMgr::instance()

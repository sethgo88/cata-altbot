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

struct RegisteredBot
{
    ObjectGuid  guid;
    std::string name;
    uint8       classId = 0;
    uint8       level   = 0;
    bool        active  = false;   // currently spawned this session
};

struct AvailableAlt
{
    ObjectGuid  guid;
    std::string name;
    uint8       classId    = 0;
    uint8       level      = 0;
    uint32      accountId  = 0;
    bool        registered = false;   // exists in character_altbot for this master
    bool        active     = false;   // currently spawned this session
};

class AltbotMgr
{
public:
    static AltbotMgr* instance();

    // Registration (durable). Writes/deletes character_altbot row + character_altbot_state row.
    bool AddAltbot(Player* master, std::string const& botName);
    void RemoveAltbot(Player* master, std::string const& botName);

    // Session lifecycle (transient). Spawns/despawns an already-registered bot.
    bool LoginBot (Player* master, std::string const& botName);
    void LogoutBot(Player* master, std::string const& botName);

    void Update(uint32 diff);

    AltbotAI* FindBotAI    (ObjectGuid masterGuid, ObjectGuid botGuid);
    AltbotAI* FindBotByName(ObjectGuid masterGuid, std::string const& botName);

    // Locate the AI for a bot by its guid alone — walks every master's roster.
    // Used by PlayerScript hooks that fire on the bot's session (quest events,
    // creature kills) where the master guid isn't in scope.
    AltbotAI* FindAnyBotAI (ObjectGuid botGuid);

    // Joins character_altbot with characters to return everything the master has registered.
    // `active` is filled by cross-referencing _activeBots.
    std::vector<RegisteredBot> ListRegistered(ObjectGuid masterGuid);

    // All characters on the master's account + linked accounts. `registered` and `active`
    // are filled per row. Used by `.altbot list` and the addon LIST verb.
    std::vector<AvailableAlt> ListAvailableAlts(Player* master);

    // True if `botGuid` belongs to master's own account or any of master's linked accounts.
    // Called by AddAltbot before allowing registration of a bot.
    bool IsAuthorizedAsBot(uint32 masterAccountId, ObjectGuid botGuid);

    // Per-bot state persistence (character_altbot_state).
    void PersistState(AltbotAI const& ai);
    void LoadState   (AltbotAI& ai);

    // Sets the spec override slug on the live AI and persists it to the
    // character_altbot row keyed by (masterGuid, botGuid). Empty string
    // reverts to auto-detect.
    void SetBotSpec(ObjectGuid masterGuid, ObjectGuid botGuid, std::string const& spec);

    // Called from PlayerScript::OnLogout (registered in AltbotLoader) for
    // every player that logs out — both clean .logout and forced session
    // destruction (account collision, shutdown). Two cases:
    //   (a) the player is a registered master  → tear down each of their
    //       bots cleanly via LogoutPlayer(true) so inventory + money save,
    //       then erase the master entry from _activeBots.
    //   (b) the player is a registered bot     → drop its AI from its
    //       master's list. The session is already mid-destruction; we
    //       just remove the now-unsafe cached pointer.
    void HandlePlayerLogout(ObjectGuid playerGuid);

private:
    AltbotMgr() = default;

    bool SpawnBot(ObjectGuid masterGuid, ObjectGuid botGuid);

    // masterGuid.GetRawValue() -> active bot AIs owned by that master
    std::unordered_map<uint64, std::vector<std::unique_ptr<AltbotAI>>> _activeBots;
};

#define sAltbotMgr AltbotMgr::instance()

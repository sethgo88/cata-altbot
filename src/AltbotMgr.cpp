#include "AltbotMgr.h"
#include "AltbotAI.h"
#include "CharacterCache.h"
#include "DatabaseEnv.h"
#include "Log.h"
#include "ObjectGuid.h"
#include "Player.h"
#include "World.h"
#include "WorldSession.h"
#include <algorithm>
#include <string>

AltbotMgr* AltbotMgr::instance()
{
    static AltbotMgr instance;
    return &instance;
}

// Core session + AI creation. Shared by AddAltbot() and SpawnBotsForMaster().
bool AltbotMgr::SpawnBot(ObjectGuid masterGuid, ObjectGuid botGuid)
{
    // Guard against double-spawn (e.g. master logs in while bot is already active)
    auto& bots = _activeBots[masterGuid.GetRawValue()];
    for (auto const& ai : bots)
    {
        Player* existing = ai->GetSession()->GetPlayer();
        if (existing && existing->GetGUID() == botGuid)
        {
            TC_LOG_DEBUG("altbot", "AltbotMgr::SpawnBot: bot %s already active for master %s — skipping.",
                botGuid.ToString().c_str(), masterGuid.ToString().c_str());
            return false;
        }
    }

    uint32 botAccountId = sCharacterCache->GetCharacterAccountIdByGuid(botGuid);
    if (!botAccountId)
    {
        TC_LOG_ERROR("altbot", "AltbotMgr::SpawnBot: No account for bot guid %s.",
            botGuid.ToString().c_str());
        return false;
    }

    WorldSession* botSession = new WorldSession(
        botAccountId,
        "",
        nullptr,
        SEC_PLAYER,
        EXPANSION_CATACLYSM,
        time_t(0),
        sWorld->GetDefaultDbcLocale(),
        0,
        false
    );

    botSession->AltbotLogin(botGuid);
    sWorld->AddSession(botSession);

    bots.push_back(std::make_unique<AltbotAI>(botSession, masterGuid));

    TC_LOG_INFO("altbot", "AltbotMgr::SpawnBot: bot guid %s (account %u) queued for master %s.",
        botGuid.ToString().c_str(), botAccountId, masterGuid.ToString().c_str());

    return true;
}

bool AltbotMgr::AddAltbot(Player* master, std::string const& botName)
{
    ObjectGuid botGuid = sCharacterCache->GetCharacterGuidByName(botName);
    if (botGuid.IsEmpty())
    {
        TC_LOG_ERROR("altbot", "AltbotMgr::AddAltbot: Character '%s' not found in cache.", botName.c_str());
        return false;
    }

    if (!SpawnBot(master->GetGUID(), botGuid))
        return false;

    // Persist so the bot respawns automatically on next master login
    CharacterDatabase.Execute(
        ("INSERT IGNORE INTO character_altbot (master_guid, bot_guid) VALUES (" +
         std::to_string(master->GetGUID().GetCounter()) + ", " +
         std::to_string(botGuid.GetCounter()) + ")").c_str());

    TC_LOG_INFO("altbot", "AltbotMgr::AddAltbot: '%s' added bot '%s'.",
        master->GetName().c_str(), botName.c_str());

    return true;
}

void AltbotMgr::RemoveAltbot(Player* master, std::string const& botName)
{
    ObjectGuid botGuid = sCharacterCache->GetCharacterGuidByName(botName);
    if (botGuid.IsEmpty())
    {
        TC_LOG_ERROR("altbot", "AltbotMgr::RemoveAltbot: Character '%s' not found.", botName.c_str());
        return;
    }

    auto& bots = _activeBots[master->GetGUID().GetRawValue()];
    auto it = std::find_if(bots.begin(), bots.end(), [&](std::unique_ptr<AltbotAI> const& ai)
    {
        Player* bot = ai->GetSession()->GetPlayer();
        return bot && bot->GetGUID() == botGuid;
    });

    if (it == bots.end())
    {
        TC_LOG_WARN("altbot", "AltbotMgr::RemoveAltbot: bot '%s' not active for master '%s'.",
            botName.c_str(), master->GetName().c_str());
        return;
    }

    Player* bot = (*it)->GetSession()->GetPlayer();
    if (bot && bot->IsInWorld())
        (*it)->GetSession()->LogoutPlayer(true);

    bots.erase(it);

    CharacterDatabase.Execute(
        ("DELETE FROM character_altbot WHERE master_guid = " +
         std::to_string(master->GetGUID().GetCounter()) +
         " AND bot_guid = " +
         std::to_string(botGuid.GetCounter())).c_str());

    TC_LOG_INFO("altbot", "AltbotMgr::RemoveAltbot: '%s' removed bot '%s'.",
        master->GetName().c_str(), botName.c_str());
}

void AltbotMgr::SpawnBotsForMaster(ObjectGuid masterGuid)
{
    QueryResult result = CharacterDatabase.Query(
        ("SELECT bot_guid FROM character_altbot WHERE master_guid = " +
         std::to_string(masterGuid.GetCounter())).c_str());

    if (!result)
        return;

    uint32 count = 0;
    do
    {
        Field* fields = result->Fetch();
        ObjectGuid botGuid(HighGuid::Player, fields[0].GetUInt32());
        if (SpawnBot(masterGuid, botGuid))
            ++count;
    } while (result->NextRow());

    if (count > 0)
        TC_LOG_INFO("altbot", "AltbotMgr::SpawnBotsForMaster: spawned %u bot(s) for master %s.",
            count, masterGuid.ToString().c_str());
}

AltbotAI* AltbotMgr::FindBotAI(ObjectGuid masterGuid, ObjectGuid botGuid)
{
    auto it = _activeBots.find(masterGuid.GetRawValue());
    if (it == _activeBots.end())
        return nullptr;

    for (auto& ai : it->second)
    {
        Player* bot = ai->GetSession()->GetPlayer();
        if (bot && bot->GetGUID() == botGuid)
            return ai.get();
    }

    return nullptr;
}

void AltbotMgr::Update(uint32 diff)
{
    for (auto& [masterRaw, bots] : _activeBots)
        for (auto& ai : bots)
            ai->Update(diff);
}

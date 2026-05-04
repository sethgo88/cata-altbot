#include "AltbotMgr.h"
#include "AltbotAI.h"
#include "AltbotAccountLink.h"
#include "AltbotState.h"
#include "CharacterCache.h"
#include "DatabaseEnv.h"
#include "Log.h"
#include "ObjectGuid.h"
#include "Player.h"
#include "World.h"
#include "WorldSession.h"
#include <algorithm>
#include <sstream>
#include <string>
#include <unordered_set>

AltbotMgr* AltbotMgr::instance()
{
    static AltbotMgr instance;
    return &instance;
}

// Core session + AI creation. Shared by AddAltbot() and LoginBot().
bool AltbotMgr::SpawnBot(ObjectGuid masterGuid, ObjectGuid botGuid)
{
    // Guard against double-spawn (e.g. Add followed by Login on the same bot)
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

    auto ai = std::make_unique<AltbotAI>(botSession, masterGuid, botGuid);
    LoadState(*ai);

    // Auto invite + summon on the bot's first in-world tick. Triggered for
    // every fresh spawn (Add and Login both flow through here) so the master
    // doesn't have to chase down a bot that just logged in halfway across
    // the map.
    ai->MarkPendingAutoInviteSummon();

    // Apply persisted spec override (if any) so the strategy resolves correctly
    // on the bot's first combat tick instead of falling back to talent-tree auto-detect.
    if (QueryResult specRow = CharacterDatabase.Query(
            ("SELECT spec_override FROM character_altbot WHERE master_guid = " +
             std::to_string(masterGuid.GetCounter()) +
             " AND bot_guid = " +
             std::to_string(botGuid.GetCounter())).c_str()))
    {
        std::string slug = specRow->Fetch()[0].GetString();
        if (!slug.empty())
            ai->SetSpecOverride(slug);
    }

    bots.push_back(std::move(ai));

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

    if (!IsAuthorizedAsBot(master->GetSession()->GetAccountId(), botGuid))
    {
        TC_LOG_WARN("altbot", "AltbotMgr::AddAltbot: '%s' tried to add '%s' from an unlinked account.",
            master->GetName().c_str(), botName.c_str());
        return false;
    }

    // Persist registration first so LoadState (called from SpawnBot) can read its row.
    CharacterDatabase.Execute(
        ("INSERT IGNORE INTO character_altbot (master_guid, bot_guid) VALUES (" +
         std::to_string(master->GetGUID().GetCounter()) + ", " +
         std::to_string(botGuid.GetCounter()) + ")").c_str());

    CharacterDatabase.Execute(
        ("INSERT IGNORE INTO character_altbot_state (bot_guid) VALUES (" +
         std::to_string(botGuid.GetCounter()) + ")").c_str());

    if (!SpawnBot(master->GetGUID(), botGuid))
        return false;

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

    if (it != bots.end())
    {
        Player* bot = (*it)->GetSession()->GetPlayer();
        if (bot && bot->IsInWorld())
            (*it)->GetSession()->LogoutPlayer(true);

        bots.erase(it);
    }

    CharacterDatabase.Execute(
        ("DELETE FROM character_altbot WHERE master_guid = " +
         std::to_string(master->GetGUID().GetCounter()) +
         " AND bot_guid = " +
         std::to_string(botGuid.GetCounter())).c_str());

    CharacterDatabase.Execute(
        ("DELETE FROM character_altbot_state WHERE bot_guid = " +
         std::to_string(botGuid.GetCounter())).c_str());

    TC_LOG_INFO("altbot", "AltbotMgr::RemoveAltbot: '%s' removed bot '%s'.",
        master->GetName().c_str(), botName.c_str());
}

bool AltbotMgr::LoginBot(Player* master, std::string const& botName)
{
    ObjectGuid botGuid = sCharacterCache->GetCharacterGuidByName(botName);
    if (botGuid.IsEmpty())
    {
        TC_LOG_ERROR("altbot", "AltbotMgr::LoginBot: Character '%s' not found.", botName.c_str());
        return false;
    }

    // Must be registered to this master.
    QueryResult registered = CharacterDatabase.Query(
        ("SELECT 1 FROM character_altbot WHERE master_guid = " +
         std::to_string(master->GetGUID().GetCounter()) +
         " AND bot_guid = " +
         std::to_string(botGuid.GetCounter())).c_str());

    if (!registered)
    {
        TC_LOG_WARN("altbot", "AltbotMgr::LoginBot: '%s' not registered for master '%s'.",
            botName.c_str(), master->GetName().c_str());
        return false;
    }

    return SpawnBot(master->GetGUID(), botGuid);
}

void AltbotMgr::LogoutBot(Player* master, std::string const& botName)
{
    ObjectGuid botGuid = sCharacterCache->GetCharacterGuidByName(botName);
    if (botGuid.IsEmpty())
    {
        TC_LOG_ERROR("altbot", "AltbotMgr::LogoutBot: Character '%s' not found.", botName.c_str());
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
        TC_LOG_DEBUG("altbot", "AltbotMgr::LogoutBot: '%s' not active for master '%s'.",
            botName.c_str(), master->GetName().c_str());
        return;
    }

    Player* bot = (*it)->GetSession()->GetPlayer();
    if (bot && bot->IsInWorld())
        (*it)->GetSession()->LogoutPlayer(true);

    bots.erase(it);

    TC_LOG_INFO("altbot", "AltbotMgr::LogoutBot: '%s' logged out bot '%s' (registration kept).",
        master->GetName().c_str(), botName.c_str());
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

AltbotAI* AltbotMgr::FindBotByName(ObjectGuid masterGuid, std::string const& botName)
{
    ObjectGuid botGuid = sCharacterCache->GetCharacterGuidByName(botName);
    if (botGuid.IsEmpty())
        return nullptr;
    return FindBotAI(masterGuid, botGuid);
}

std::vector<RegisteredBot> AltbotMgr::ListRegistered(ObjectGuid masterGuid)
{
    std::vector<RegisteredBot> out;

    QueryResult result = CharacterDatabase.Query(
        ("SELECT c.guid, c.name, c.class, c.level "
         "FROM character_altbot a "
         "JOIN characters c ON c.guid = a.bot_guid "
         "WHERE a.master_guid = " +
         std::to_string(masterGuid.GetCounter()) +
         " ORDER BY c.name").c_str());

    if (!result)
        return out;

    auto activeIt = _activeBots.find(masterGuid.GetRawValue());

    do
    {
        Field* fields = result->Fetch();
        RegisteredBot row;
        row.guid    = ObjectGuid(HighGuid::Player, fields[0].GetUInt32());
        row.name    = fields[1].GetString();
        row.classId = fields[2].GetUInt8();
        row.level   = fields[3].GetUInt8();

        if (activeIt != _activeBots.end())
        {
            for (auto const& ai : activeIt->second)
            {
                Player* bot = ai->GetSession()->GetPlayer();
                if (bot && bot->GetGUID() == row.guid)
                {
                    row.active = true;
                    break;
                }
            }
        }

        out.push_back(std::move(row));
    } while (result->NextRow());

    return out;
}

void AltbotMgr::PersistState(AltbotAI const& ai)
{
    if (ai.GetBotGuid().IsEmpty())
        return;

    AltbotState const& s = ai.GetState();

    CharacterDatabase.Execute(
        ("INSERT INTO character_altbot_state "
         "(bot_guid, mode, assist_mode, role_override, auto_loot, loot_roll, auto_mount, auto_release, auto_quest_take, auto_quest_turn_in) "
         "VALUES (" +
         std::to_string(ai.GetBotGuid().GetCounter()) + ", " +
         std::to_string(uint32(s.mode))            + ", " +
         std::to_string(uint32(s.assist))          + ", " +
         std::to_string(uint32(s.roleOverride))    + ", " +
         std::to_string(s.autoLoot         ? 1 : 0) + ", " +
         std::to_string(uint32(s.lootRoll))         + ", " +
         std::to_string(s.autoMount        ? 1 : 0) + ", " +
         std::to_string(s.autoRelease      ? 1 : 0) + ", " +
         std::to_string(s.autoQuestTake    ? 1 : 0) + ", " +
         std::to_string(s.autoQuestTurnIn  ? 1 : 0) + ") "
         "ON DUPLICATE KEY UPDATE "
         "mode               = VALUES(mode), "
         "assist_mode        = VALUES(assist_mode), "
         "role_override      = VALUES(role_override), "
         "auto_loot          = VALUES(auto_loot), "
         "loot_roll          = VALUES(loot_roll), "
         "auto_mount         = VALUES(auto_mount), "
         "auto_release       = VALUES(auto_release), "
         "auto_quest_take    = VALUES(auto_quest_take), "
         "auto_quest_turn_in = VALUES(auto_quest_turn_in)").c_str());
}

void AltbotMgr::SetBotSpec(ObjectGuid masterGuid, ObjectGuid botGuid, std::string const& spec)
{
    if (AltbotAI* ai = FindBotAI(masterGuid, botGuid))
        ai->SetSpecOverride(spec);

    // Escape single quotes in the slug to keep the UPDATE statement well-formed.
    std::string escaped;
    escaped.reserve(spec.size());
    for (char c : spec)
    {
        if (c == '\'') escaped += "''";
        else           escaped += c;
    }

    CharacterDatabase.Execute(
        ("UPDATE character_altbot SET spec_override = '" + escaped + "' "
         "WHERE master_guid = " + std::to_string(masterGuid.GetCounter()) +
         " AND bot_guid = "    + std::to_string(botGuid.GetCounter())).c_str());
}

void AltbotMgr::LoadState(AltbotAI& ai)
{
    QueryResult result = CharacterDatabase.Query(
        ("SELECT mode, assist_mode, role_override, auto_loot, loot_roll, auto_mount, auto_release, auto_quest_take, auto_quest_turn_in "
         "FROM character_altbot_state WHERE bot_guid = " +
         std::to_string(ai.GetBotGuid().GetCounter())).c_str());

    if (!result)
        return;

    Field* f = result->Fetch();
    AltbotState& s     = ai.MutableState();
    s.mode             = AltbotMode(f[0].GetUInt8());
    s.assist           = AltbotAssistMode(f[1].GetUInt8());
    s.roleOverride     = AltbotRoleOverride(f[2].GetUInt8());
    s.autoLoot         = f[3].GetUInt8() != 0;
    s.lootRoll         = AltbotLootRollMode(f[4].GetUInt8());
    s.autoMount        = f[5].GetUInt8() != 0;
    s.autoRelease      = f[6].GetUInt8() != 0;
    s.autoQuestTake    = f[7].GetUInt8() != 0;
    s.autoQuestTurnIn  = f[8].GetUInt8() != 0;
}

bool AltbotMgr::IsAuthorizedAsBot(uint32 masterAccountId, ObjectGuid botGuid)
{
    uint32 botAccountId = sCharacterCache->GetCharacterAccountIdByGuid(botGuid);
    if (!botAccountId)
        return false;

    if (botAccountId == masterAccountId)
        return true;

    for (uint32 linked : AltbotAccountLink::GetLinkedAccounts(masterAccountId))
        if (linked == botAccountId)
            return true;

    return false;
}

std::vector<AvailableAlt> AltbotMgr::ListAvailableAlts(Player* master)
{
    std::vector<AvailableAlt> out;

    uint32 masterAccount = master->GetSession()->GetAccountId();

    // Build the IN-list: own account + linked accounts.
    std::ostringstream accountList;
    accountList << masterAccount;
    for (uint32 linked : AltbotAccountLink::GetLinkedAccounts(masterAccount))
        accountList << "," << linked;

    QueryResult result = CharacterDatabase.Query(
        ("SELECT guid, name, class, level, account FROM characters "
         "WHERE account IN (" + accountList.str() + ") "
         "AND guid <> " + std::to_string(master->GetGUID().GetCounter()) + " "
         "ORDER BY name").c_str());

    if (!result)
        return out;

    // Pre-load registered bot guids for this master into a set.
    std::unordered_set<uint32> registeredGuids;
    if (QueryResult regs = CharacterDatabase.Query(
        ("SELECT bot_guid FROM character_altbot WHERE master_guid = " +
         std::to_string(master->GetGUID().GetCounter())).c_str()))
    {
        do { registeredGuids.insert(regs->Fetch()[0].GetUInt32()); } while (regs->NextRow());
    }

    auto activeIt = _activeBots.find(master->GetGUID().GetRawValue());

    do
    {
        Field* fields = result->Fetch();
        AvailableAlt row;
        row.guid       = ObjectGuid(HighGuid::Player, fields[0].GetUInt32());
        row.name       = fields[1].GetString();
        row.classId    = fields[2].GetUInt8();
        row.level      = fields[3].GetUInt8();
        row.accountId  = fields[4].GetUInt32();
        row.registered = registeredGuids.count(row.guid.GetCounter()) != 0;

        if (row.registered && activeIt != _activeBots.end())
        {
            for (auto const& ai : activeIt->second)
            {
                Player* bot = ai->GetSession()->GetPlayer();
                if (bot && bot->GetGUID() == row.guid)
                {
                    row.active = true;
                    break;
                }
            }
        }

        out.push_back(std::move(row));
    } while (result->NextRow());

    return out;
}

void AltbotMgr::Update(uint32 diff)
{
    for (auto& [masterRaw, bots] : _activeBots)
        for (auto& ai : bots)
            ai->Update(diff);
}

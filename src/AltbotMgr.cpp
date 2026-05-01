#include "AltbotMgr.h"
#include "AltbotAI.h"
#include "CharacterCache.h"
#include "Log.h"
#include "Player.h"
#include "SharedDefines.h"
#include "World.h"
#include "WorldSession.h"

AltbotMgr* AltbotMgr::instance()
{
    static AltbotMgr instance;
    return &instance;
}

bool AltbotMgr::AddAltbot(Player* master, std::string const& botName)
{
    // --- 1. Resolve bot character GUID and account --------------------------
    ObjectGuid botGuid = sCharacterCache->GetCharacterGuidByName(botName);
    if (botGuid.IsEmpty())
    {
        TC_LOG_ERROR("altbot", "AltbotMgr::AddAltbot: Character '%s' not found in cache.",
            botName.c_str());
        return false;
    }

    uint32 botAccountId = sCharacterCache->GetCharacterAccountIdByGuid(botGuid);
    if (!botAccountId)
    {
        TC_LOG_ERROR("altbot", "AltbotMgr::AddAltbot: No account found for character '%s'.",
            botName.c_str());
        return false;
    }

    // --- 2. Create a fake WorldSession with no real socket ------------------
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

    // --- 3. Trigger DB load via the core login flow -------------------------
    botSession->AltbotLogin(botGuid);

    // --- 4. Queue the session on the world ----------------------------------
    sWorld->AddSession(botSession);

    // --- 5. Create the AI driver and track it -------------------------------
    auto ai = std::make_unique<AltbotAI>(botSession, master->GetGUID());
    _activeBots[master->GetGUID().GetRawValue()].push_back(std::move(ai));

    TC_LOG_INFO("altbot", "AltbotMgr::AddAltbot: Queued login for bot '%s' "
        "(guid %s, account %u, master %s).",
        botName.c_str(),
        botGuid.ToString().c_str(),
        botAccountId,
        master->GetName().c_str());

    return true;
}

void AltbotMgr::RemoveAltbot(Player* /*master*/, std::string const& botName)
{
    // TODO Phase 5: look up bot by name, kick session, remove from _activeBots
    TC_LOG_INFO("altbot", "AltbotMgr::RemoveAltbot: '%s' — not yet implemented (Phase 5).",
        botName.c_str());
}

void AltbotMgr::Update(uint32 diff)
{
    for (auto& [masterGuidRaw, bots] : _activeBots)
        for (auto& ai : bots)
            ai->Update(diff);
}

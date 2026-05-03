#include "AltbotAccountLink.h"
#include "AccountMgr.h"
#include "Chat.h"
#include "DatabaseEnv.h"
#include "Log.h"
#include "Player.h"
#include "WorldSession.h"
#include <ctime>

static constexpr time_t LINK_TTL_SECONDS = 60;

AltbotAccountLink* AltbotAccountLink::instance()
{
    static AltbotAccountLink instance;
    return &instance;
}

void AltbotAccountLink::PurgeExpired()
{
    time_t now = ::time(nullptr);
    for (auto it = _pendingLinks.begin(); it != _pendingLinks.end(); )
    {
        if (it->second.expiresAt <= now)
            it = _pendingLinks.erase(it);
        else
            ++it;
    }
}

bool AltbotAccountLink::HasPending(uint32 masterAccountId)
{
    PurgeExpired();
    return _pendingLinks.find(masterAccountId) != _pendingLinks.end();
}

bool AltbotAccountLink::BeginLink(Player* master, std::string const& username)
{
    PurgeExpired();

    uint32 linkedAccount = AccountMgr::GetId(username);
    if (!linkedAccount)
    {
        ChatHandler(master->GetSession()).PSendSysMessage("Altbot: account '%s' not found.", username.c_str());
        return false;
    }

    uint32 masterAccount = master->GetSession()->GetAccountId();
    if (linkedAccount == masterAccount)
    {
        ChatHandler(master->GetSession()).SendSysMessage("Altbot: cannot link your own account to itself.");
        return false;
    }

    // Already linked? Idempotent — just report and exit.
    QueryResult existing = CharacterDatabase.Query(
        ("SELECT 1 FROM character_altbot_account_link WHERE master_account = " +
         std::to_string(masterAccount) + " AND linked_account = " +
         std::to_string(linkedAccount)).c_str());
    if (existing)
    {
        ChatHandler(master->GetSession()).PSendSysMessage("Altbot: account '%s' is already linked.", username.c_str());
        return false;
    }

    _pendingLinks[masterAccount] = { linkedAccount, ::time(nullptr) + LINK_TTL_SECONDS };

    ChatHandler(master->GetSession()).PSendSysMessage(
        "Altbot: to confirm linking '%s', whisper its password to one of your registered bots within 60 seconds.",
        username.c_str());
    return true;
}

bool AltbotAccountLink::CompleteLink(Player* master, std::string const& password)
{
    PurgeExpired();

    uint32 masterAccount = master->GetSession()->GetAccountId();
    auto it = _pendingLinks.find(masterAccount);
    if (it == _pendingLinks.end())
        return false;

    uint32 linkedAccount = it->second.linkedAccount;

    // Resolve the linked account's username for the password check.
    std::string username;
    if (!AccountMgr::GetName(linkedAccount, username))
    {
        TC_LOG_ERROR("altbot", "AltbotAccountLink::CompleteLink: linked account %u disappeared.", linkedAccount);
        _pendingLinks.erase(it);
        return false;
    }

    // TC 4.3.4: AccountMgr::CheckPassword returns bool, not AccountOpResult enum.
    if (!AccountMgr::CheckPassword(linkedAccount, password))
    {
        ChatHandler(master->GetSession()).SendSysMessage("Altbot: link failed — wrong password.");
        // Don't clear the pending entry on a wrong attempt; let the master retry within the TTL.
        return false;
    }

    CharacterDatabase.Execute(
        ("INSERT IGNORE INTO character_altbot_account_link (master_account, linked_account) VALUES (" +
         std::to_string(masterAccount) + ", " +
         std::to_string(linkedAccount) + ")").c_str());

    _pendingLinks.erase(it);

    ChatHandler(master->GetSession()).PSendSysMessage("Altbot: account '%s' linked.", username.c_str());
    TC_LOG_INFO("altbot", "AltbotAccountLink: master account %u linked account %u ('%s').",
        masterAccount, linkedAccount, username.c_str());
    return true;
}

bool AltbotAccountLink::Unlink(Player* master, std::string const& username)
{
    uint32 linkedAccount = AccountMgr::GetId(username);
    if (!linkedAccount)
    {
        ChatHandler(master->GetSession()).PSendSysMessage("Altbot: account '%s' not found.", username.c_str());
        return false;
    }

    uint32 masterAccount = master->GetSession()->GetAccountId();

    CharacterDatabase.Execute(
        ("DELETE FROM character_altbot_account_link WHERE master_account = " +
         std::to_string(masterAccount) + " AND linked_account = " +
         std::to_string(linkedAccount)).c_str());

    ChatHandler(master->GetSession()).PSendSysMessage("Altbot: account '%s' unlinked.", username.c_str());
    return true;
}

std::vector<uint32> AltbotAccountLink::GetLinkedAccounts(uint32 masterAccountId)
{
    std::vector<uint32> out;

    QueryResult result = CharacterDatabase.Query(
        ("SELECT linked_account FROM character_altbot_account_link WHERE master_account = " +
         std::to_string(masterAccountId)).c_str());

    if (!result)
        return out;

    do
    {
        out.push_back(result->Fetch()[0].GetUInt32());
    } while (result->NextRow());

    return out;
}

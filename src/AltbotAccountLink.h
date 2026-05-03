#pragma once
#include <cstdint>
#include <ctime>
#include <string>
#include <unordered_map>
#include <vector>

class Player;

struct PendingLink
{
    uint32 linkedAccount;
    time_t expiresAt;
};

// Bootstraps cross-account altbot ownership.
// Step 1: master types `.altbot link <username>` -> BeginLink creates a pending entry.
// Step 2: master whispers any of their registered bots the alt-account password
//         within 60s -> CompleteLink validates and writes the link row.
// AccountMgr::CheckPassword does the credential check (handles SHA1 vs SRP6).
class AltbotAccountLink
{
public:
    static AltbotAccountLink* instance();

    bool BeginLink   (Player* master, std::string const& username);
    bool CompleteLink(Player* master, std::string const& password);
    bool Unlink      (Player* master, std::string const& username);
    bool HasPending  (uint32 masterAccountId);

    // Returns the linked account IDs for a master. Empty vector if none.
    static std::vector<uint32> GetLinkedAccounts(uint32 masterAccountId);

private:
    AltbotAccountLink() = default;

    void PurgeExpired();

    // masterAccountId -> pending entry. Single in-flight link per master.
    std::unordered_map<uint32, PendingLink> _pendingLinks;
};

#define sAltbotAccountLink AltbotAccountLink::instance()

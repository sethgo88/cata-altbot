/**
 * AltbotLoader.cpp — script registration entry point.
 *
 * Add to server-core/src/server/scripts/Custom/CMakeLists.txt:
 *   add_subdirectory(cata-altbot)
 *
 * And call AddSC_AltbotLoader() from the Custom scripts loader.
 */

#include "ScriptMgr.h"
#include "AccountMgr.h"
#include "Chat.h"
#include "RBAC.h"
#include "AltbotAccountLink.h"
#include "AltbotConfig.h"
#include "AltbotMgr.h"
#include "Player.h"
#include <sstream>

class altbot_commandscript : public CommandScript
{
public:
    altbot_commandscript() : CommandScript("altbot_commandscript") { }

    std::vector<ChatCommand> GetCommands() const override
    {
        static std::vector<ChatCommand> altbotSubCommands =
        {
            // .altbot add <name>    — register a character as one of your altbots (durable)
            { "add",    rbac::RBAC_PERM_COMMAND_ALTBOT_ADD, false, &HandleAltbotAddCommand,    "" },
            // .altbot remove <name> — unregister a character (durable; deletes state row)
            { "remove", rbac::RBAC_PERM_COMMAND_ALTBOT_ADD, false, &HandleAltbotRemoveCommand, "" },
            // .altbot login <name>  — spawn an already-registered bot for this session
            { "login",  rbac::RBAC_PERM_COMMAND_ALTBOT_ADD, false, &HandleAltbotLoginCommand,  "" },
            // .altbot logout <name> — despawn a bot but keep its registration + state
            { "logout", rbac::RBAC_PERM_COMMAND_ALTBOT_ADD, false, &HandleAltbotLogoutCommand, "" },
            // .altbot list           — show all available alts (own + linked accounts)
            { "list",   rbac::RBAC_PERM_COMMAND_ALTBOT_ADD, false, &HandleAltbotListCommand,   "" },
            // .altbot link <user>    — begin cross-account link; whisper bot password to confirm
            { "link",   rbac::RBAC_PERM_COMMAND_ALTBOT_ADD, false, &HandleAltbotLinkCommand,   "" },
            // .altbot unlink <user>  — remove a cross-account link
            { "unlink", rbac::RBAC_PERM_COMMAND_ALTBOT_ADD, false, &HandleAltbotUnlinkCommand, "" },
            // .altbot links          — show currently-linked accounts
            { "links",  rbac::RBAC_PERM_COMMAND_ALTBOT_ADD, false, &HandleAltbotLinksCommand,  "" },
        };
        static std::vector<ChatCommand> commandTable =
        {
            { "altbot", rbac::RBAC_PERM_COMMAND_ALTBOT, true, nullptr, "", altbotSubCommands },
        };
        return commandTable;
    }

    static bool HandleAltbotAddCommand(ChatHandler* handler, char const* args)
    {
        if (!args || !*args)
        {
            handler->SendSysMessage("Usage: .altbot add <charactername>");
            handler->SetSentErrorMessage(true);
            return false;
        }

        Player* player = handler->GetSession()->GetPlayer();
        if (!player)
            return false;

        std::string botName(args);

        if (!sAltbotMgr->AddAltbot(player, botName))
        {
            handler->PSendSysMessage("Failed to add altbot '%s'. Check server log for details.",
                botName.c_str());
            handler->SetSentErrorMessage(true);
            return false;
        }

        handler->PSendSysMessage("Altbot '%s' loading...", botName.c_str());
        return true;
    }

    static bool HandleAltbotRemoveCommand(ChatHandler* handler, char const* args)
    {
        if (!args || !*args)
        {
            handler->SendSysMessage("Usage: .altbot remove <charactername>");
            handler->SetSentErrorMessage(true);
            return false;
        }

        Player* player = handler->GetSession()->GetPlayer();
        if (!player)
            return false;

        std::string botName(args);
        sAltbotMgr->RemoveAltbot(player, botName);
        handler->PSendSysMessage("Altbot '%s' removed.", botName.c_str());
        return true;
    }

    static bool HandleAltbotLoginCommand(ChatHandler* handler, char const* args)
    {
        if (!args || !*args)
        {
            handler->SendSysMessage("Usage: .altbot login <charactername>");
            handler->SetSentErrorMessage(true);
            return false;
        }

        Player* player = handler->GetSession()->GetPlayer();
        if (!player)
            return false;

        std::string botName(args);
        if (!sAltbotMgr->LoginBot(player, botName))
        {
            handler->PSendSysMessage("Failed to log in altbot '%s' — not registered or character not found.",
                botName.c_str());
            handler->SetSentErrorMessage(true);
            return false;
        }

        handler->PSendSysMessage("Altbot '%s' logging in...", botName.c_str());
        return true;
    }

    static bool HandleAltbotLogoutCommand(ChatHandler* handler, char const* args)
    {
        if (!args || !*args)
        {
            handler->SendSysMessage("Usage: .altbot logout <charactername>");
            handler->SetSentErrorMessage(true);
            return false;
        }

        Player* player = handler->GetSession()->GetPlayer();
        if (!player)
            return false;

        std::string botName(args);
        sAltbotMgr->LogoutBot(player, botName);
        handler->PSendSysMessage("Altbot '%s' logged out (registration kept).", botName.c_str());
        return true;
    }

    static bool HandleAltbotListCommand(ChatHandler* handler, char const* /*args*/)
    {
        Player* player = handler->GetSession()->GetPlayer();
        if (!player)
            return false;

        std::vector<AvailableAlt> alts = sAltbotMgr->ListAvailableAlts(player);
        if (alts.empty())
        {
            handler->SendSysMessage("Altbot: no available alts on your account or linked accounts.");
            return true;
        }

        handler->PSendSysMessage("Altbot: %zu available alt(s):", alts.size());
        for (auto const& a : alts)
        {
            char const* status = a.active     ? "ACTIVE"
                               : a.registered ? "registered"
                                              : "available";
            handler->PSendSysMessage("  %s (lvl %u, class %u, account %u) — %s",
                a.name.c_str(), a.level, a.classId, a.accountId, status);
        }
        return true;
    }

    static bool HandleAltbotLinkCommand(ChatHandler* handler, char const* args)
    {
        if (!args || !*args)
        {
            handler->SendSysMessage("Usage: .altbot link <username>");
            handler->SetSentErrorMessage(true);
            return false;
        }

        Player* player = handler->GetSession()->GetPlayer();
        if (!player)
            return false;

        std::string username(args);
        sAltbotAccountLink->BeginLink(player, username);
        return true;
    }

    static bool HandleAltbotUnlinkCommand(ChatHandler* handler, char const* args)
    {
        if (!args || !*args)
        {
            handler->SendSysMessage("Usage: .altbot unlink <username>");
            handler->SetSentErrorMessage(true);
            return false;
        }

        Player* player = handler->GetSession()->GetPlayer();
        if (!player)
            return false;

        std::string username(args);
        sAltbotAccountLink->Unlink(player, username);
        return true;
    }

    static bool HandleAltbotLinksCommand(ChatHandler* handler, char const* /*args*/)
    {
        Player* player = handler->GetSession()->GetPlayer();
        if (!player)
            return false;

        std::vector<uint32> accounts = AltbotAccountLink::GetLinkedAccounts(player->GetSession()->GetAccountId());
        if (accounts.empty())
        {
            handler->SendSysMessage("Altbot: no linked accounts.");
            return true;
        }

        std::ostringstream names;
        for (size_t i = 0; i < accounts.size(); ++i)
        {
            if (i) names << ", ";
            std::string username;
            if (AccountMgr::GetName(accounts[i], username))
                names << username;
            else
                names << "(account " << accounts[i] << ")";
        }
        handler->PSendSysMessage("Altbot: %zu linked account(s): %s", accounts.size(), names.str().c_str());
        return true;
    }
};

class altbot_worldscript : public WorldScript
{
public:
    altbot_worldscript() : WorldScript("altbot_worldscript") {}

    void OnConfigLoad(bool /*reload*/) override
    {
        sAltbotConfig->Reload();
    }

    void OnUpdate(uint32 diff) override
    {
        sAltbotMgr->Update(diff);
    }

    void OnShutdown() override
    {
        sAltbotMgr->ShutdownAllBots();
    }
};

// Fires from WorldSession::LogoutPlayer (TC: WorldSession.cpp ~line 653) for
// every player logout — clean .logout, kick, *and* forced session destruction
// via World::AddSession_ collision or ~WorldSession during shutdown.
//
// Closes the dangling-AltbotAI window: if the bot's session is destroyed
// externally, the cached _botSession pointer in AltbotAI becomes garbage on
// the very next OnUpdate tick. Routing through HandlePlayerLogout removes the
// AI from _activeBots before that tick arrives. Also handles the master-side
// case (tear down all of a master's bots when the master logs out).
class altbot_playerscript : public PlayerScript
{
public:
    altbot_playerscript() : PlayerScript("altbot_playerscript") {}

    void OnLogout(Player* player) override
    {
        if (!player)
            return;
        sAltbotMgr->HandlePlayerLogout(player->GetGUID());
    }
};

void AddSC_AltbotCommands();
void AddSC_AltbotQuest();

void AddSC_AltbotLoader()
{
    new altbot_commandscript();
    new altbot_worldscript();
    new altbot_playerscript();
    AddSC_AltbotCommands();
    AddSC_AltbotQuest();
}

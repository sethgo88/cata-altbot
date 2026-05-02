/**
 * AltbotLoader.cpp — script registration entry point.
 *
 * Add to server-core/src/server/scripts/Custom/CMakeLists.txt:
 *   add_subdirectory(cata-altbot)
 *
 * And call AddSC_AltbotLoader() from the Custom scripts loader.
 */

#include "ScriptMgr.h"
#include "Chat.h"
#include "RBAC.h"
#include "AltbotMgr.h"
#include "Player.h"

class altbot_commandscript : public CommandScript
{
public:
    altbot_commandscript() : CommandScript("altbot_commandscript") { }

    std::vector<ChatCommand> GetCommands() const override
    {
        static std::vector<ChatCommand> altbotSubCommands =
        {
            // .altbot add <name>    — load your character as an altbot
            { "add",    rbac::RBAC_PERM_COMMAND_ALTBOT_ADD, false, &HandleAltbotAddCommand,    "" },
            // .altbot remove <name> — remove an active altbot
            { "remove", rbac::RBAC_PERM_COMMAND_ALTBOT_ADD, false, &HandleAltbotRemoveCommand, "" },
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
};

class altbot_worldscript : public WorldScript
{
public:
    altbot_worldscript() : WorldScript("altbot_worldscript") {}

    void OnUpdate(uint32 diff) override
    {
        sAltbotMgr->Update(diff);
    }
};

void AddSC_AltbotCommands();

void AddSC_AltbotLoader()
{
    new altbot_commandscript();
    new altbot_worldscript();
    AddSC_AltbotCommands();
}

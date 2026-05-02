#include "AltbotAI.h"
#include "AltbotMgr.h"
#include "Chat.h"
#include "Player.h"
#include "ScriptMgr.h"
#include <algorithm>
#include <cctype>

class altbot_playerscript : public PlayerScript
{
public:
    altbot_playerscript() : PlayerScript("altbot_playerscript") { }

    void OnChat(Player* player, uint32 type, uint32 /*lang*/, std::string& msg, Player* receiver) override
    {
        if (type != CHAT_MSG_WHISPER || !receiver)
            return;

        AltbotAI* ai = sAltbotMgr->FindBotAI(player->GetGUID(), receiver->GetGUID());
        if (!ai)
            return;

        std::string cmd = msg;
        std::transform(cmd.begin(), cmd.end(), cmd.begin(), ::tolower);

        ChatHandler chat(player->GetSession());

        if (cmd == "follow")
        {
            ai->SetMode(AltbotMode::Follow);
            chat.PSendSysMessage("%s is now following you.", receiver->GetName().c_str());
        }
        else if (cmd == "stay")
        {
            ai->SetMode(AltbotMode::Stay);
            receiver->GetMotionMaster()->MoveIdle();
            chat.PSendSysMessage("%s will stay put.", receiver->GetName().c_str());
        }
        else if (cmd == "help")
        {
            chat.SendSysMessage("Altbot commands (whisper your bot): follow, stay, help");
        }
    }

    void OnLogin(Player* player, bool /*firstLogin*/) override
    {
        sAltbotMgr->SpawnBotsForMaster(player->GetGUID());
    }
};

void AddSC_AltbotCommands()
{
    new altbot_playerscript();
}

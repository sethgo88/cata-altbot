#include "AltbotAI.h"
#include "AltbotAccountLink.h"
#include "AltbotAddonProto.h"
#include "AltbotCommandTable.h"
#include "AltbotMgr.h"
#include "Chat.h"
#include "Player.h"
#include "ScriptMgr.h"
#include "WorldSession.h"

class altbot_playerscript : public PlayerScript
{
public:
    altbot_playerscript() : PlayerScript("altbot_playerscript") { }

    void OnChat(Player* player, uint32 type, uint32 /*lang*/, std::string& msg, Player* receiver) override
    {
        if (type != CHAT_MSG_WHISPER || !receiver)
            return;

        // Addon protocol: master self-whisper carries the CATABOT prefix when no
        // bot is online to act as transport. This is the bootstrap path — the
        // addon can fetch a roster, ADD, and LOGIN with zero bots active.
        // Dispatched with ai = nullptr; TryDispatch uses master as both sender
        // and reply transport.
        if (receiver == player)
        {
            if (AltbotAddonProto::TryDispatch(player, nullptr, msg))
                return;
        }

        AltbotAI* ai = sAltbotMgr->FindBotAI(player->GetGUID(), receiver->GetGUID());
        if (!ai)
            return;

        // If a cross-account link is pending for this master, treat the whisper
        // as the password and consume it. The check is gated on receiver being
        // one of master's bots (already enforced by the FindBotAI guard above)
        // so unrelated whispers don't accidentally consume the pending state.
        uint32 masterAccount = player->GetSession()->GetAccountId();
        if (sAltbotAccountLink->HasPending(masterAccount))
        {
            sAltbotAccountLink->CompleteLink(player, msg);
            return;
        }

        // Addon-protocol pipe-prefixed messages route to AltbotAddonProto
        // before falling through to the regular command table.
        if (AltbotAddonProto::TryDispatch(player, ai, msg))
            return;

        AltbotCommandTable::Dispatch(player, ai, msg);
    }

    void OnLogin(Player* player, bool /*firstLogin*/) override
    {
        // Pure-explicit login model: do NOT auto-spawn registered bots.
        // Instead, list them so the master can pick which to bring in this session.
        std::vector<RegisteredBot> registered = sAltbotMgr->ListRegistered(player->GetGUID());
        if (registered.empty())
            return;

        ChatHandler chat(player->GetSession());
        chat.PSendSysMessage("Altbot: %zu registered bot(s). Use '.altbot login <name>' to bring one in.",
                             registered.size());

        std::string list;
        for (auto const& bot : registered)
        {
            if (!list.empty())
                list += ", ";
            list += bot.name;
        }
        chat.PSendSysMessage("Altbot: %s", list.c_str());
    }

    // Fires from WorldSession::LogoutPlayer (TC: WorldSession.cpp ~line 653) for
    // every player logout — clean .logout, kick, *and* forced session destruction
    // via World::AddSession_ collision or ~WorldSession during shutdown.
    //
    // Closes the dangling-AltbotAI window: if the bot's session is destroyed
    // externally, the cached _botSession pointer in AltbotAI becomes garbage on
    // the very next OnUpdate tick. Routing through HandlePlayerLogout removes the
    // AI from _activeBots before that tick arrives. Also handles the master-side
    // case (tear down all of a master's bots when the master logs out).
    void OnLogout(Player* player) override
    {
        if (!player)
            return;
        sAltbotMgr->HandlePlayerLogout(player->GetGUID());
    }
};

void AddSC_AltbotCommands()
{
    new altbot_playerscript();
}

#pragma once
#include <string>
#include <string_view>

class Player;
class AltbotAI;

// Server-side endpoint for the CataAltbot client addon's pipe-delimited
// protocol. Piggybacks on whisper transport (no inbound packet hook
// required, no CHAT_MSG_ADDON support required) — every payload is a
// whisper with a "CATABOT|" prefix.
//
// Inbound  (master -> server): master's addon /script SendChatMessage("CATABOT|VERB|...", "WHISPER", nil, "BotName")
// Outbound (server -> master): bot->Whisper("CATABOT|VERB|...", LANG_UNIVERSAL, master)
//
// The CATABOT prefix is checked in AltbotCommands.cpp::OnChat before falling
// through to the regular whisper command table.
namespace AltbotAddonProto
{
    static constexpr char const* PREFIX        = "CATABOT|";
    static constexpr size_t      PREFIX_LEN    = 8;
    static constexpr uint32      PROTO_VERSION = 1;

    // Returns true if the whisper was an addon-protocol message (prefix matched
    // and was dispatched). False = pass through to regular command table.
    bool TryDispatch(Player* master, AltbotAI* ai, std::string_view msg);

    // Send an outgoing addon message from `sender` to `master`. `sender` is
    // usually one of master's bots, but during bootstrap (no bots online) it
    // is the master themselves self-whispering. `payload` is the verb-and-args
    // part WITHOUT the "CATABOT|" prefix; we add it.
    void Send(Player* sender, Player* master, std::string const& payload);

    // Push a STATE update for one bot's toggles to the master's addon. Called
    // from MutateState wiring (later: AltbotMgr::PersistState extends to also
    // push STATE when the master is online with the addon).
    void PushState(AltbotAI const& ai);
}

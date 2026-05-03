#pragma once
#include <string_view>

class Player;
class AltbotAI;

// Whisper-command dispatcher. Matches the leading verb of `msg` against the
// table in AltbotCommandTable.cpp and invokes the handler.
//
// Returns true if the verb was recognized (regardless of whether the action
// succeeded). Returns false if the verb is unknown, so callers can decide
// whether to ignore the whisper or pass it through to other handlers.
namespace AltbotCommandTable
{
    bool Dispatch(Player* master, AltbotAI* ai, std::string_view msg);
}

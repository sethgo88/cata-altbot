#pragma once
#include <cstdint>
#include <string_view>

class Player;

namespace AltbotTalents
{
    // Print active spec + free talent points to master via whisper.
    // Detailed per-talent breakdown is the addon's job (Phase 9 modal).
    void Show(Player* master, Player* bot);

    // Learn one talent rank. Wraps Player::LearnTalent which validates prereqs,
    // available points, and active spec. Sends success/failure feedback.
    bool Learn(Player* master, Player* bot, uint32 talentId, uint32 rank);

    // Whisper-command parser for "learn <talentId> <rank>".
    bool LearnFromArgs(Player* master, Player* bot, std::string_view args);
}

#pragma once
#include <cstdint>

enum class AltbotMode : uint8_t
{
    Follow = 0,
    Stay   = 1,
};

enum class AltbotAssistMode : uint8_t
{
    Off          = 0,
    MasterTarget = 1,
    SkullOnly    = 2,
    Both         = 3,
};

// Per-bot toggle state. Persisted in `character_altbot_state` keyed by bot_guid.
// Mutate via AltbotAI::MutateState(fn) so changes flow through AltbotMgr::PersistState.
struct AltbotState
{
    AltbotMode       mode             = AltbotMode::Follow;
    AltbotAssistMode assist           = AltbotAssistMode::Both;
    bool             autoLoot         = true;
    bool             autoPass         = false;
    bool             autoMount        = true;
    bool             autoRelease      = true;
    bool             autoQuestTake    = false;
    bool             autoQuestTurnIn  = false;
};

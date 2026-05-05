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

// Manual role override. None = derive from spec/talent tree.
// MainTank = Tank for LFG purposes, plus MEMBER_FLAG_MAINTANK on the group
// in raids so assist/healing logic can identify the primary tank.
enum class AltbotRoleOverride : uint8_t
{
    None     = 0,
    Tank     = 1,
    Healer   = 2,
    Damage   = 3,
    MainTank = 4,
};

// Group-loot roll behavior.
//   Wait       — bot does nothing, the roll waits the full 60s before timing out.
//   Pass       — bot immediately votes PASS on every roll it's eligible for.
//   Disenchant — bot votes DISENCHANT when offered (group has an enchanter that
//                meets the item's RequiredDisenchantSkill); otherwise votes PASS.
enum class AltbotLootRollMode : uint8_t
{
    Wait       = 0,
    Pass       = 1,
    Disenchant = 2,
};

// Per-bot toggle state. Persisted in `character_altbot_state` keyed by bot_guid.
// Mutate via AltbotAI::MutateState(fn) so changes flow through AltbotMgr::PersistState.
struct AltbotState
{
    AltbotMode         mode           = AltbotMode::Follow;
    AltbotAssistMode   assist         = AltbotAssistMode::Both;
    AltbotRoleOverride roleOverride   = AltbotRoleOverride::None;
    bool             autoLoot         = true;
    AltbotLootRollMode lootRoll       = AltbotLootRollMode::Wait;
    bool             autoMount        = true;
    bool             autoRelease      = true;
    bool             autoQuestTake    = true;
    bool             autoQuestTurnIn  = true;
};

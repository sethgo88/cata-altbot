#pragma once
#include <cstdint>

class AltbotPositionManager;

// Per-tick state passed from AltbotAI to AltbotStrategy::Update.
// Built fresh each tick from bot/master observation; not persisted.
struct AltbotTickContext
{
    uint32 combatElapsedMs = 0;  // 0 when master is not in combat; counts up while in combat
    bool   inDungeon       = false;
    bool   inRaid          = false;

    // Per-bot positioning manager — strategies declare a PositionIntent each
    // tick via positionManager->SetIntent(...) before tier dispatch. Manager
    // owns the actual MoveChase/MovePoint calls (LOS recovery, dead-zone,
    // fire retreat, leash, idle-pack avoidance). Always non-null in normal
    // flow; AltbotAI populates it before invoking AltbotCombat::Update.
    AltbotPositionManager* positionManager = nullptr;

    // Phase 3 boss-state surface. Populated in AltbotAI::Update from
    // master->GetMap()->ToInstanceMap()->GetInstanceScript() when both exist.
    // Strategies *may* branch on bossPhase / bossHpPct (additive — no existing
    // strategy is required to consume these fields).
    //
    // bossNpcEntry == 0 means "no boss state available" (out of instance,
    // pre-engagement, or instance script doesn't expose it). bossPhase
    // derivation is encounter-specific (`EncounterPhases::Resolve(npc, hp)`)
    // and starts at 1 once a boss has been engaged.
    uint32 bossNpcEntry  = 0;
    uint8  bossPhase     = 0;
    uint8  bossStateRaw  = 0;   // EncounterState enum value
    uint8  bossHpPct     = 0;

    bool InInstance() const { return inDungeon || inRaid; }
    bool BossEngaged() const { return bossNpcEntry != 0; }
};

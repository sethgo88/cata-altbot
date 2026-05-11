#pragma once
#include "ObjectGuid.h"
#include "SharedDefines.h"
#include <cstdint>

class Player;
class Unit;

// Telemetry sink for the altbot rotation pipeline.
//
// Phase 1 (this module) captures three things:
//   1. Every cast attempt — success or failure — issued through
//      `StrategyUtil::CastWithLog`.
//   2. Combat-start / combat-end edges, derived from `AltbotAI::Update`'s
//      master-in-combat tracking. On end, a per-fight summary line is logged
//      and (optionally) whispered to master.
//   3. Damage events, via the global `UnitScript::OnDamage` hook registered
//      in `AltbotLoader.cpp`. Filtered to altbots only via
//      `AltbotMgr::IsAltbot`.
//
// Output channels:
//   - log channel "altbot.combat" — per-cast trace + per-fight summary.
//     Per-cast trace is opt-in (`Altbot.Telemetry.PerCastLog`) since volume
//     is high; summary is always-on when telemetry is enabled.
//   - master whisper — per-fight summary line, gated on
//     `Altbot.Telemetry.WhisperSummary` and `Altbot.Telemetry.MinFightMs`.
//
// All entry points are no-ops when telemetry is disabled in altbot.conf.
namespace AltbotCombatLog
{
    // Single chokepoint for a cast attempt. Called from
    // `StrategyUtil::CastWithLog` after `Unit::CastSpell` returns. `tierIdx`
    // and `tierName` come from the thread-local `SetCurrentTier` state set
    // by strategy dispatch sites; defaults are -1 / "maint".
    //
    // Schema: `tierIdx=-1` = maintenance / defensive / pet, `tierIdx=-2` =
    // encounter-driven override (Phase 3 interrupt/dispel/purge), `tierIdx>=0`
    // = ordinary rotation tier index.
    void OnCastIssued(Player* bot, Unit* target, uint32 spellId,
                      char const* specLabel, int tierIdx, char const* tierName,
                      SpellCastResult result);

    // Combat-state edges. Caller (`AltbotAI::Update`) detects rising / falling
    // edge of `master->IsInCombat()` and forwards the edge here so the per-fight
    // summary aligns with the existing threat-window timing.
    void OnCombatEnter(Player* bot);
    void OnCombatLeave(Player* bot, Player* master, uint32 combatElapsedMs);

    // Counter for ticks where the rotation dispatch finished without firing
    // any tier. Surfaced in the combat summary; persistent non-zero values
    // indicate a rotation gap (filler missing / GCD probe wrong / etc.).
    void OnNoTierFired(Player* bot, char const* specLabel);

    // Called from `UnitScript::OnDamage` (registered in AltbotLoader). Already
    // pre-filtered to altbots — caller is responsible for the GUID check.
    void OnDamageDealt(ObjectGuid botGuid, uint32 damage);

    // Per-bot trace toggle. When `Altbot.Telemetry.PerCastLog` is off but a
    // single bot is being diagnosed, `.altbot trace <bot> on` flips this on
    // for that bot only — the per-cast trace fires for that bot, not the rest.
    void SetTrace(ObjectGuid botGuid, bool enabled);
    bool IsTraced(ObjectGuid botGuid);

    // Thread-local "current tier" hook. Strategy dispatch sites should set
    // this before each `if (Tier_X(...))` check so `OnCastIssued` can
    // attribute the cast that fires inside the tier. Defaults to (-1, "maint")
    // when not set; reset by `OnCombatEnter` / `OnCombatLeave`.
    //
    // Phase 1 ships the API but does NOT require strategies to call it —
    // tier_name will read "maint" everywhere until Phase 4 enrichment.
    void SetCurrentTier(int idx, char const* name);
    void ClearCurrentTier();
}

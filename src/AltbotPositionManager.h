#pragma once

#include "ObjectGuid.h"
#include <cstdint>

class Player;
class Unit;
struct AltbotTickContext;

namespace AltbotPosition
{
    enum class PositionRole : uint8_t
    {
        RangedDPS,    // chase anchor (target) at desiredRange, master-leash
        Healer,       // anchor = lowest-HP ally (or master fallback), tight leash
        FollowMaster, // out-of-combat or unimplemented melee/tank: hand off to follow
        Stationary,   // strategy is mid-channel/post-summon: do nothing this tick
    };

    struct PositionIntent
    {
        PositionRole role           = PositionRole::FollowMaster;
        Unit*  anchor               = nullptr;
        float  desiredRange         = 25.0f;
        Unit*  leashAnchor          = nullptr;     // typically master
        float  leashRange           = 12.0f;
        Unit*  losTarget            = nullptr;     // require LOS to this unit
        bool   canMoveWhileCasting  = false;       // manager flips this for fire emergencies
        bool   requireDeadZoneEscape = false;      // physical-shot specs (hunter)
        float  deadZoneInner        = 0.0f;
        float  deadZoneBackup       = 0.0f;
        float  masterStackRange     = 10.0f;       // skip dead-zone escape when master is stacked
    };
}

// Owned by AltbotAI (one per bot, lifetime-bound). Strategies push intent
// once per combat tick via SetIntent; manager runs Tick() after the strategy
// returns to enact the move (chase / LOS recover / dead-zone escape / fire
// retreat / leash check). FastTick() runs on the 1000ms follow tick to react
// to ground-effect damage faster than the 1500ms combat cadence.
class AltbotPositionManager
{
public:
    explicit AltbotPositionManager(Player* bot) : _bot(bot) {}

    // Bot pointer is populated lazily — AltbotAI is constructed before its
    // bot finishes async DB load. Caller invokes Bind() once IsInWorld().
    void Bind(Player* bot) { _bot = bot; }

    void Reset();

    void SetIntent(AltbotPosition::PositionIntent const& intent) { _intent = intent; }

    // Full pass: LOS / chase / dead-zone / leash / fire retreat. Called from
    // AltbotCombat::Update after strategy->Update returns (or after fallback
    // intent synthesis for strategy-less bots).
    void Tick(Player* master, AltbotTickContext const& ctx, uint32 nowMs);

    // Cheap pass: only sample HP and run the reactive-fire detector. Called
    // from the existing 1000ms follow block in AltbotAI::Update so we react
    // to fire faster than the 1500ms combat tick.
    void FastTick(Player* master, uint32 nowMs);

    // Convenience builders (defaults capture the role's typical params).
    static AltbotPosition::PositionIntent MakeRangedDpsIntent(Unit* target, Unit* master);
    static AltbotPosition::PositionIntent MakeHealerIntent(Unit* anchor, Unit* master, bool stack);
    static AltbotPosition::PositionIntent MakeFollowIntent();

private:
    Player* _bot;
    AltbotPosition::PositionIntent _intent;

    // Reactive-fire heuristic state
    uint32 _lastHpSampleMs      = 0;
    uint32 _lastHpAbs           = 0;
    uint32 _firstFireSeenMs     = 0;   // sticky; cleared after retreat
    uint32 _lastEmergencyMoveMs = 0;   // 3000ms cooldown to avoid re-spam

    // LOS cache (~750ms TTL keyed by target GUID)
    ObjectGuid _losCacheTarget;
    uint32     _lastLosCheckMs = 0;
    bool       _lastLosOk      = true;

    // Verbatim copy of the strategies' rotation guard. Movement that would
    // re-mutate a chase generator mid-cast trips SPELL_FAILED_MOVING (53).
    bool IsBotCasting() const;

    bool ShouldEscapeFire(uint32 nowMs) const;
    bool DetectUnexpectedDamage(uint32 nowMs);
    bool LeashOk(float destX, float destY) const;
    bool LosOk(Unit* losTarget, uint32 nowMs);
};

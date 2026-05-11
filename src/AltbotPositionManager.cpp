#include "AltbotPositionManager.h"
#include "AltbotPosition.h"
#include "AltbotTickContext.h"
#include "AreaTrigger.h"
#include "CellImpl.h"
#include "DynamicObject.h"
#include "EncounterMechanics.h"
#include "GridNotifiers.h"
#include "Log.h"
#include "MotionMaster.h"
#include "Player.h"
#include "Unit.h"
#include <cmath>

using namespace AltbotPosition;

namespace
{
    constexpr float kRangedLeashRange    = 12.0f;
    constexpr float kHealerLeashRange    = 6.0f;
    constexpr float kHealerSpreadRange   = 30.0f;
    constexpr float kHealerStackRange    = 8.0f;
    constexpr float kRangedDesiredRange  = 25.0f;

    constexpr float kFireDamagePctTrip   = 8.0f;
    constexpr uint32 kFireMoveCooldownMs = 3000;
    constexpr uint32 kLosCacheTtlMs      = 750;
    constexpr float kMeleeAttackerRadius = 8.0f;

    // Bot's hazard scan only walks its current grid cell — keep the radius
    // small so the cell-bounded VisitGridObjects walk stays cheap.
    constexpr float kHazardScanRadiusY   = 12.0f;
}

namespace
{
    // Visitor that records the smallest-radius `avoidable` mechanic the bot
    // is currently inside. Used by `DetectMechanicHazard`. Visits both grid
    // object types that carry a SpellId we can resolve: DynamicObject (for
    // ground-targeted spells like Blizzard / Crystal Storm) and AreaTrigger
    // (for area-bound effects like Noxious Mire patches).
    struct HazardVisitor
    {
        Player const* bot;
        bool          inHazard = false;
        uint32        worstSpellId = 0;
        float         worstRadius  = 0.0f;

        void Consider(uint32 spellId, float x, float y)
        {
            if (!spellId)
                return;
            float radius = EncounterMechanics::AvoidRadius(spellId);
            if (radius <= 0.0f)
                return;
            if (!EncounterMechanics::IsAvoidable(spellId))
                return;
            float dx = bot->GetPositionX() - x;
            float dy = bot->GetPositionY() - y;
            float dist2 = dx * dx + dy * dy;
            float r2    = radius * radius;
            if (dist2 > r2)
                return;
            // Prefer the entry with the largest radius — the spell whose
            // footprint we're most clearly inside. Logged once on retreat.
            if (radius > worstRadius)
            {
                worstSpellId = spellId;
                worstRadius  = radius;
            }
            inHazard = true;
        }

        // Cell visitor entry points. `Visit(...)` overloads are dispatched by
        // TypeContainerVisitor based on the GridTypeMapContainer typelist.
        void Visit(DynamicObjectMapType& m)
        {
            for (auto iter = m.begin(); iter != m.end(); ++iter)
            {
                DynamicObject* obj = iter->GetSource();
                if (!obj) continue;
                Consider(obj->GetSpellId(), obj->GetPositionX(), obj->GetPositionY());
            }
        }

        void Visit(AreaTriggerMapType& m)
        {
            for (auto iter = m.begin(); iter != m.end(); ++iter)
            {
                AreaTrigger* obj = iter->GetSource();
                if (!obj) continue;
                Consider(obj->GetSpellId(), obj->GetPositionX(), obj->GetPositionY());
            }
        }

        // No-op visits for the rest of GridTypeMapContainer; required so the
        // TypeContainerVisitor template instantiation succeeds.
        template <class T> void Visit(GridRefManager<T>&) {}
    };
}

void AltbotPositionManager::Reset()
{
    _intent = PositionIntent{};
    _lastHpSampleMs      = 0;
    _lastHpAbs           = 0;
    _firstFireSeenMs     = 0;
    _lastEmergencyMoveMs = 0;
    _losCacheTarget      = ObjectGuid::Empty;
    _lastLosCheckMs      = 0;
    _lastLosOk           = true;
}

bool AltbotPositionManager::IsBotCasting() const
{
    if (!_bot)
        return false;
    return _bot->HasUnitState(UNIT_STATE_CASTING) || _bot->IsNonMeleeSpellCast(false);
}

bool AltbotPositionManager::LeashOk(float destX, float destY) const
{
    if (!_intent.leashAnchor || _intent.leashRange <= 0.0f)
        return true;
    float dx = destX - _intent.leashAnchor->GetPositionX();
    float dy = destY - _intent.leashAnchor->GetPositionY();
    return std::sqrt(dx * dx + dy * dy) <= _intent.leashRange;
}

bool AltbotPositionManager::LosOk(Unit* losTarget, uint32 nowMs)
{
    if (!losTarget)
        return true;
    ObjectGuid g = losTarget->GetGUID();
    if (g == _losCacheTarget && (nowMs - _lastLosCheckMs) < kLosCacheTtlMs)
        return _lastLosOk;
    bool ok = AltbotPosition::HasLineOfSight(_bot, losTarget);
    _losCacheTarget = g;
    _lastLosCheckMs = nowMs;
    _lastLosOk      = ok;
    return ok;
}

bool AltbotPositionManager::DetectUnexpectedDamage(uint32 nowMs)
{
    if (!_bot || !_bot->IsAlive())
        return false;

    uint32 hpNow = _bot->GetHealth();
    if (_lastHpAbs == 0)
    {
        _lastHpAbs = hpNow;
        _lastHpSampleMs = nowMs;
        return false;
    }

    int32 delta = int32(_lastHpAbs) - int32(hpNow);
    _lastHpAbs = hpNow;
    _lastHpSampleMs = nowMs;
    if (delta <= 0)
        return false;

    uint32 maxHp = _bot->GetMaxHealth();
    if (!maxHp)
        return false;
    float pctOfMax = 100.0f * float(delta) / float(maxHp);
    if (pctOfMax < kFireDamagePctTrip)
        return false;

    // No melee attacker in 8y → call it environmental damage.
    Unit::AttackerSet const& atks = _bot->getAttackers();
    for (Unit* atk : atks)
    {
        if (atk && _bot->GetDistance(atk) < kMeleeAttackerRadius)
            return false;
    }

    TC_LOG_INFO("altbot",
        "AltbotPositionManager [%s]: emergency-fire signal (delta=%u hp / %.1f%% max, attackers_in_8y=0)",
        _bot->GetName().c_str(), uint32(delta), pctOfMax);
    return true;
}

bool AltbotPositionManager::ShouldEscapeFire(uint32 nowMs) const
{
    if (_firstFireSeenMs == 0)
        return false;
    if (_lastEmergencyMoveMs > 0 && (nowMs - _lastEmergencyMoveMs) < kFireMoveCooldownMs)
        return false;
    return true;
}

void AltbotPositionManager::FastTick(Player* master, uint32 nowMs)
{
    if (!_bot || !_bot->IsAlive() || !master)
        return;

    // Sample HP and trip the fire flag if a damage spike with no melee
    // attacker is detected. The actual retreat happens on the next combat
    // Tick (or here, if ShouldEscapeFire fires and a cooldown allows).
    if (DetectUnexpectedDamage(nowMs))
        _firstFireSeenMs = nowMs;

    // Phase 2 deterministic detector: scan grid for `avoidable` spells the
    // bot is currently standing inside. Trips the same retreat flag the HP
    // heuristic uses, so the existing FindSafeRetreatPosition path runs
    // without a separate code path. The HP heuristic stays as a fallback for
    // unmapped spells and for damage that doesn't come from a DynamicObject /
    // AreaTrigger.
    if (DetectMechanicHazard(nowMs))
        _firstFireSeenMs = nowMs;
}

bool AltbotPositionManager::DetectMechanicHazard(uint32 /*nowMs*/)
{
    if (!_bot || !_bot->IsAlive() || !_bot->IsInWorld())
        return false;

    HazardVisitor v;
    v.bot = _bot;
    Cell::VisitGridObjects(_bot, v, kHazardScanRadiusY);

    if (v.inHazard)
    {
        // Single log line on first detection — keeps the channel quiet on
        // sustained hazards. The retreat path will reset the fire flag;
        // re-entering an unrelated hazard re-trips this branch.
        if (_firstFireSeenMs == 0)
            TC_LOG_INFO("altbot",
                "AltbotPositionManager [%s]: hazard-detected spell %u (radius %.1fy) — retreating",
                _bot->GetName().c_str(), v.worstSpellId, v.worstRadius);
    }
    return v.inHazard;
}

void AltbotPositionManager::Tick(Player* master, AltbotTickContext const& ctx, uint32 nowMs)
{
    if (!_bot || !_bot->IsAlive() || !master)
        return;

    // Re-sample HP on the slow tick too — the fast tick may have skipped if
    // the bot only entered combat between fast ticks.
    if (DetectUnexpectedDamage(nowMs))
        _firstFireSeenMs = nowMs;
    // Likewise re-run the mechanic-hazard scan — the deterministic detector
    // can fire on the slow tick when the fast pass didn't (e.g. bot just
    // entered a fresh DynamicObject between sample windows).
    if (DetectMechanicHazard(nowMs))
        _firstFireSeenMs = nowMs;

    // Stationary intent: strategy explicitly asked us to not move. Used by
    // post-summon pin or mid-channel states the manager doesn't otherwise see.
    if (_intent.role == PositionRole::Stationary)
        return;

    // Out-of-combat or unimplemented melee: defer to follow logic in AltbotAI.
    if (_intent.role == PositionRole::FollowMaster)
        return;

    bool casting = IsBotCasting();

    // 1) Emergency fire retreat — highest priority, can move mid-cast.
    if (ShouldEscapeFire(nowMs))
    {
        Unit* anchor = _intent.anchor ? _intent.anchor : master;
        float retX = 0.0f, retY = 0.0f, retZ = 0.0f;
        bool found = AltbotPosition::FindSafeRetreatPosition(
            _bot, anchor, _intent.desiredRange,
            _intent.leashAnchor, _intent.leashRange,
            retX, retY, retZ);

        if (found)
        {
            TC_LOG_INFO("altbot",
                "AltbotPositionManager [%s]: emergency-fire-move to (%.1f,%.1f,%.1f) (casting=%d)",
                _bot->GetName().c_str(), retX, retY, retZ, int(casting));
            _bot->GetMotionMaster()->MovePoint(0, retX, retY, retZ);
            _firstFireSeenMs = 0;
            _lastEmergencyMoveMs = nowMs;
            return;
        }
        else
        {
            // No leash-compliant safe spot — eat the fire rather than break
            // the master-anchored stack.
            TC_LOG_INFO("altbot",
                "AltbotPositionManager [%s]: leash violation, eating fire",
                _bot->GetName().c_str());
            _firstFireSeenMs = 0;
            _lastEmergencyMoveMs = nowMs;
            // Fall through to other movement decisions — but they'll still be
            // gated by IsBotCasting() if we were mid-cast.
        }
    }

    // 2) From here on, anything that mutates a movement generator while we
    // are mid-cast trips SPELL_FAILED_MOVING (53). Skip non-emergency moves
    // until the cast completes.
    if (casting)
    {
        TC_LOG_DEBUG("altbot",
            "AltbotPositionManager [%s]: skip move (casting)",
            _bot->GetName().c_str());
        return;
    }

    // 3) Dead-zone escape (hunter): physical shots reject inside ~8y. Skip
    // when master is also at melee range with the target — stack mechanic.
    if (_intent.requireDeadZoneEscape && _intent.anchor && _intent.deadZoneInner > 0.0f)
    {
        float distToAnchor = _bot->GetDistance(_intent.anchor);
        bool masterAtRange = (!_intent.leashAnchor)
            || (_intent.leashAnchor->GetDistance(_intent.anchor) > _intent.masterStackRange);
        if (distToAnchor < _intent.deadZoneInner && masterAtRange)
        {
            TC_LOG_DEBUG("altbot",
                "AltbotPositionManager [%s]: dead-zone escape (dist=%.1f<%.1f)",
                _bot->GetName().c_str(), distToAnchor, _intent.deadZoneInner);
            AltbotPosition::BackUpToRange(_bot, _intent.anchor, _intent.deadZoneBackup);
            return;
        }
    }

    // 4) LOS recovery: if the strategy declared a losTarget and we're out of
    // sight, find a sample on the desiredRange ring with LOS + a clear path.
    if (_intent.losTarget && !LosOk(_intent.losTarget, nowMs))
    {
        float losX = 0.0f, losY = 0.0f, losZ = 0.0f;
        bool found = AltbotPosition::FindLOSPosition(
            _bot, _intent.losTarget, _intent.desiredRange, losX, losY, losZ);
        if (found && LeashOk(losX, losY))
        {
            TC_LOG_INFO("altbot",
                "AltbotPositionManager [%s]: LOS broken, repositioning to (%.1f,%.1f,%.1f)",
                _bot->GetName().c_str(), losX, losY, losZ);
            _bot->GetMotionMaster()->MovePoint(0, losX, losY, losZ);
            return;
        }
        // No leash-compliant LOS sample: fall through. The cast will fail
        // with SPELL_FAILED_LINE_OF_SIGHT (49) and the rotation retries.
    }

    // 5) Default chase: keep `desiredRange` from anchor. Idempotency-guarded
    // by MaintainRange itself (only re-issues if dist > range or no chase).
    if (!_intent.anchor)
        return;

    // Idle-pack avoidance: if the path to the chase anchor crosses a known
    // idle-aggro circle, sample an alt position on the ring. Only applies
    // when we're far enough to actually need to move.
    float curDist = _bot->GetDistance(_intent.anchor);
    if (curDist > _intent.desiredRange)
    {
        float botX = _bot->GetPositionX();
        float botY = _bot->GetPositionY();
        float botZ = _bot->GetPositionZ();

        // Approximate the chase target as a point at desiredRange from anchor
        // along the bot→anchor vector. The chase generator's actual landing
        // spot is similar in practice.
        float ax = _intent.anchor->GetPositionX();
        float ay = _intent.anchor->GetPositionY();
        float az = _intent.anchor->GetPositionZ();
        float dx = botX - ax;
        float dy = botY - ay;
        float vlen = std::sqrt(dx * dx + dy * dy);
        if (vlen > 0.01f)
        {
            float nx = dx / vlen;
            float ny = dy / vlen;
            float chaseX = ax + nx * _intent.desiredRange;
            float chaseY = ay + ny * _intent.desiredRange;
            float chaseZ = az;
            _bot->UpdateAllowedPositionZ(chaseX, chaseY, chaseZ);

            if (!AltbotPosition::IsPathSafe(_bot, botX, botY, botZ, chaseX, chaseY, chaseZ))
            {
                float altX = 0.0f, altY = 0.0f, altZ = 0.0f;
                bool found = AltbotPosition::FindSafeRetreatPosition(
                    _bot, _intent.anchor, _intent.desiredRange,
                    _intent.leashAnchor, _intent.leashRange,
                    altX, altY, altZ);
                if (found)
                {
                    TC_LOG_INFO("altbot",
                        "AltbotPositionManager [%s]: idle-pack avoid, alt (%.1f,%.1f,%.1f)",
                        _bot->GetName().c_str(), altX, altY, altZ);
                    _bot->GetMotionMaster()->MovePoint(0, altX, altY, altZ);
                    return;
                }
                // No safe alt found — fall through to plain chase. Better to
                // pull a side pack than to stand still and wipe to current pull.
            }
        }
    }

    AltbotPosition::MaintainRange(_bot, _intent.anchor, _intent.desiredRange);
}

PositionIntent AltbotPositionManager::MakeRangedDpsIntent(Unit* target, Unit* master)
{
    PositionIntent intent;
    intent.role         = PositionRole::RangedDPS;
    intent.anchor       = target;
    intent.desiredRange = kRangedDesiredRange;
    intent.leashAnchor  = master;
    intent.leashRange   = kRangedLeashRange;
    intent.losTarget    = target;
    return intent;
}

PositionIntent AltbotPositionManager::MakeHealerIntent(Unit* anchor, Unit* master, bool stack)
{
    PositionIntent intent;
    intent.role         = PositionRole::Healer;
    intent.anchor       = anchor ? anchor : master;
    intent.desiredRange = stack ? kHealerStackRange : kHealerSpreadRange;
    intent.leashAnchor  = master;
    intent.leashRange   = kHealerLeashRange;
    intent.losTarget    = intent.anchor;
    return intent;
}

PositionIntent AltbotPositionManager::MakeFollowIntent()
{
    PositionIntent intent;
    intent.role = PositionRole::FollowMaster;
    return intent;
}

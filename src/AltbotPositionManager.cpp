#include "AltbotPositionManager.h"
#include "AltbotPosition.h"
#include "AltbotTickContext.h"
#include "AreaTrigger.h"
#include "CellImpl.h"
#include "Creature.h"
#include "DynamicObject.h"
#include "GameObject.h"
#include "EncounterMechanics.h"
#include "GridNotifiers.h"
#include "Log.h"
#include "MotionMaster.h"
#include "Player.h"
#include "SpellAuras.h"
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

    // 5% single-sample trip catches Cata ground-patch tick rates (Noxious
    // Mire ~5k/s on an 85, ~4-5% of max HP per FastTick window). Bumped down
    // from 8% which was tuned against Wrath-era effects.
    constexpr float kFireDamagePctTrip   = 5.0f;
    // Cumulative trip: if the bot loses this much HP over the rolling window
    // below with no melee attacker in range, trip even when individual ticks
    // stayed under the per-sample threshold. Catches slow-bleed mechanics.
    constexpr float kFireCumulativePct   = 12.0f;
    constexpr uint32 kFireCumulativeWindowMs = 3000;
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

        // GameObjects sometimes carry spell-bound effects (some Cata ground
        // patches are GameObject-based instead of DynamicObject/Creature).
        // GameObject::GetSpellId returns the source spell when set.
        void Visit(GameObjectMapType& m)
        {
            for (auto iter = m.begin(); iter != m.end(); ++iter)
            {
                GameObject* obj = iter->GetSource();
                if (!obj) continue;
                Consider(obj->GetSpellId(), obj->GetPositionX(), obj->GetPositionY());
            }
        }

        // Creatures can host "ground patch" mechanics too — in TC 4.3.4 most
        // Cata encounter ground effects (Noxious Mire 77217, Quicksand,
        // Crystal Storm, etc.) are implemented as a summoned invisible
        // creature carrying a PERIODIC_TRIGGER_SPELL aura that hits anything
        // standing in radius. The DynamicObject scan misses those entirely.
        // For each nearby creature, check its applied auras against the
        // mechanic DB; if one matches, the bot is inside that mechanic's
        // footprint (the creature sits at the patch origin).
        //
        // Diagnostic note: when bots are dying to a ground patch but the
        // scan isn't tripping, enable trace and check this log line —
        // missing IDs in the DB show up as "creature aura X near bot Y" with
        // no corresponding `hazard-detected` line. Add the surfaced IDs to
        // the appropriate encounter.md and re-gen the DB.
        void Visit(CreatureMapType& m)
        {
            for (auto iter = m.begin(); iter != m.end(); ++iter)
            {
                Creature* c = iter->GetSource();
                if (!c || !c->IsAlive()) continue;
                float dx = bot->GetPositionX() - c->GetPositionX();
                float dy = bot->GetPositionY() - c->GetPositionY();
                float dist2 = dx * dx + dy * dy;
                if (dist2 > 100.0f) continue;   // 10y radius for the diagnostic
                for (auto const& [auraSpellId, app] : c->GetAppliedAuras())
                {
                    if (!app || !app->GetBase()) continue;
                    if (EncounterMechanics::IsAvoidable(auraSpellId))
                    {
                        Consider(auraSpellId, c->GetPositionX(), c->GetPositionY());
                        break;
                    }
                    // Diagnostic: surface candidate hazard auras the DB
                    // doesn't know about. Periodic-trigger auras (aura 23 =
                    // SPELL_AURA_PERIODIC_TRIGGER_SPELL) are the canonical
                    // signature for ground patches.
                    Aura const* a = app->GetBase();
                    SpellInfo const* info = a ? a->GetSpellInfo() : nullptr;
                    if (info)
                    {
                        for (uint8 i = 0; i < MAX_SPELL_EFFECTS; ++i)
                        {
                            if (info->Effects[i].ApplyAuraName == SPELL_AURA_PERIODIC_TRIGGER_SPELL)
                            {
                                TC_LOG_INFO("altbot",
                                    "AltbotPositionManager [%s]: NEAR creature '%s' (entry=%u) with"
                                    " periodic-trigger aura %u ('%s') trig=%u — not in mechanic DB",
                                    bot->GetName().c_str(), c->GetName().c_str(), c->GetEntry(),
                                    auraSpellId, info->SpellName ? info->SpellName : "?",
                                    info->Effects[i].TriggerSpell);
                                break;
                            }
                        }
                    }
                }
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
    for (size_t i = 0; i < kHpRingSize; ++i) _hpRing[i] = HpDelta{};
    _hpRingHead = 0;
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

    uint32 maxHp = _bot->GetMaxHealth();
    if (!maxHp)
        return false;

    // Always record into the ring (even on 0/negative deltas — we only sum
    // positive ones below). Drops stale entries naturally as we overwrite.
    if (delta > 0)
    {
        _hpRing[_hpRingHead] = HpDelta{ nowMs, uint32(delta) };
        _hpRingHead = (_hpRingHead + 1) % kHpRingSize;
    }

    auto meleeAttackerInRange = [this]()
    {
        for (Unit* atk : _bot->getAttackers())
            if (atk && _bot->GetDistance(atk) < kMeleeAttackerRadius)
                return true;
        return false;
    };

    float pctOfMax = 100.0f * float(std::max<int32>(delta, 0)) / float(maxHp);

    // Per-sample spike: existing trip path. Tightened to 5% so Cata ground-
    // patch ticks register reliably.
    if (pctOfMax >= kFireDamagePctTrip && !meleeAttackerInRange())
    {
        TC_LOG_INFO("altbot",
            "AltbotPositionManager [%s]: emergency-fire signal (spike delta=%u hp / %.1f%% max, attackers_in_8y=0)",
            _bot->GetName().c_str(), uint32(delta), pctOfMax);
        return true;
    }

    // Cumulative: sum deltas inside the rolling window. Catches the slow-bleed
    // pattern (3 × 4%-tick spread over 3 seconds) the spike test misses.
    uint32 sum = 0;
    for (size_t i = 0; i < kHpRingSize; ++i)
    {
        HpDelta const& d = _hpRing[i];
        if (d.ms != 0 && (nowMs - d.ms) <= kFireCumulativeWindowMs)
            sum += d.delta;
    }
    float cumPct = 100.0f * float(sum) / float(maxHp);

    // Standard cumulative trip — gated on no melee attacker, same as the
    // single-sample spike check. This catches the "I'm a ranged caster
    // sitting in a pool with nobody hitting me" case.
    if (cumPct >= kFireCumulativePct && !meleeAttackerInRange())
    {
        TC_LOG_INFO("altbot",
            "AltbotPositionManager [%s]: emergency-fire signal (cumulative %u hp / %.1f%% max over %ums, attackers_in_8y=0)",
            _bot->GetName().c_str(), sum, cumPct, kFireCumulativeWindowMs);
        return true;
    }

    // Severe cumulative loss overrides the melee-attacker gate. A bot bleeding
    // >20% in 3s while also being meleed is in environmental + melee damage
    // both — the melee alone wouldn't produce that rate (auto-attacks tick
    // ~2-3% per swing, ~4-5% combined over 3s). The excess almost always
    // means "I'm also standing in a ground patch."
    constexpr float kFireSevereCumulativePct = 20.0f;
    if (cumPct >= kFireSevereCumulativePct)
    {
        TC_LOG_INFO("altbot",
            "AltbotPositionManager [%s]: emergency-fire signal (SEVERE cumulative %u hp / %.1f%% max over %ums, overriding melee-attacker gate)",
            _bot->GetName().c_str(), sum, cumPct, kFireCumulativeWindowMs);
        return true;
    }

    return false;
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

void AltbotPositionManager::Tick(Player* master, AltbotTickContext const& /*ctx*/, uint32 nowMs)
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
        // Fire retreat geometry:
        //   - Sample a small ring around the BOT (not the target). The ground
        //     patch is at or near the bot's current position; we just need to
        //     step out by `kFireRetreatRadius`.
        //   - Drop the master-leash for this path (pass nullptr). The earlier
        //     code anchored samples at 25y from target — but for a ranged DPS
        //     the bot is ~20y from master while master is meleeing at 5y, so
        //     every sample violated the 12y leash and the retreat always
        //     ended in `leash violation, eating fire`. The leash is a normal-
        //     positioning constraint, not an emergency one — a bot bleeding
        //     in fire should always step out. MaintainRange re-establishes
        //     caster range on the next combat tick.
        constexpr float kFireRetreatRadius = 9.0f;
        float retX = 0.0f, retY = 0.0f, retZ = 0.0f;
        bool found = AltbotPosition::FindSafeRetreatPosition(
            _bot, _bot, kFireRetreatRadius,
            /*leashAnchor*/ nullptr, /*leashRange*/ 0.0f,
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
            // No reachable spot anywhere on the small retreat ring — usually
            // means terrain / pathing block. Logged but rare.
            TC_LOG_INFO("altbot",
                "AltbotPositionManager [%s]: no reachable retreat spot, eating fire",
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

    // 3) Dead-zone escape (hunter): physical shots reject inside ~8y. The
    // original `masterAtRange` gate suppressed the escape when the master
    // was *also* at melee range with the target — intended for stack
    // mechanics (Bronjahm Soulstorm), but in a normal tank pull the master
    // IS in melee with the mob, so the gate inverted and the hunter sat in
    // melee getting SPELL_FAILED_TOO_CLOSE (130) every shot. Default behavior
    // is now: if the hunter is too close, always back up. Stack mechanics
    // override per-encounter by clearing `requireDeadZoneEscape` on the intent.
    if (_intent.requireDeadZoneEscape && _intent.anchor && _intent.deadZoneInner > 0.0f)
    {
        float distToAnchor = _bot->GetDistance(_intent.anchor);
        if (distToAnchor < _intent.deadZoneInner)
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

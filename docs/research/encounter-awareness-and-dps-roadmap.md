# Plan — Make the bots act like real players (encounter-aware) + DPS properly

## Context

Two symptoms drove this plan:

1. **"They don't know what they're doing."** In Throne of the Tides the bots stand in the green Noxious Mire pools dropped by Naz'jar Sentinels. Root cause: the bot AI has **zero encounter awareness** — no instance-script reads, no spell-ID avoidance, no interrupt/dispel prioritization, no boss-phase reaction. The only "stay out of bad" logic is a heuristic in `AltbotPositionManager::DetectUnexpectedDamage` (`src/AltbotPositionManager.cpp:68`) that trips at ≥8% max-HP loss in a single ~1s sample — Noxious Mire ticks ~5k Nature/s, ~5% on a level-85 caster, below the threshold. Meanwhile we already have ~635 lines of YAML mechanic data in `docs/dungeons/{slug}/encounter.md` that nothing in the codebase reads.

2. **"DPS isn't quite right."** We can't actually tell why. Cast logging is failure-only (`StrategyUtil::CastWithLog` at `src/strategies/StrategyUtil.cpp:124`); there is no per-cast success trace, no per-fight summary, no DPS measurement, no observable tier-fire history. The master cannot see what the bot did or why it did it. On top of that, only 4 of 17 launch-Cata specs have real strategies — the other 13 fall through to `AltbotCombat`'s "highest-rank damage spell" generic scan (`src/AltbotCombat.cpp:22-105`).

User chose the maximalist scope: end-to-end sweep across all phases, full encounter pipeline (avoid + interrupt + dispel + phase) in the first encounter-awareness phase, both telemetry surfaces (whisper + dedicated log file), and an upfront DBC verification sprint to clean up ~122 UNVERIFIED spell IDs before code consumes them.

## Workstreams

Four streams, sequenced so the visible "stop standing in fire" win lands fast and the DPS work is data-driven:

```
Phase 0  Research — DBC verification sprint                    (1–2 days, research only)
Phase 1  Telemetry foundation                                  (~1 week)
Phase 2  Mechanic DB + ground-effect avoidance — ToTT fix      (~1 week)
Phase 3  Interrupts + dispels + boss-phase awareness           (~2 weeks)
Phase 4  DPS bug-hunt + tuning the 4 existing strategies       (~1–2 weeks, parallel-able with Phase 5)
Phase 5  Scaffold-driven onboarding of remaining 13 specs      (~6–10 weeks)
```

Phases 4 and 5 run after Phase 1 telemetry lands; everything before then is sequential because each phase de-risks the next.

---

## Phase 0 — DBC verification sprint *(research only, local sources only)*

**Why first.** The mechanic database in Phase 2 is generated from doc YAML keyed by spell ID. Landing it on top of 122 UNVERIFIED rows (and the 2 confirmed-wrong BRC IDs 75763 / 75476) bakes runtime bugs — bots either avoid wrong things or fail to avoid right ones. Cheaper to fix in docs once than chase in production logs.

**Local data available** (no Wowhead, no external lookups):
- DBC CSV exports at `~/web/wow/doc/dbc/`: `Spell.csv` (73k rows), `SpellEffect.csv` (97k), `SpellCooldowns.csv`, `SpellDuration.csv`, `SpellLevels.csv`, `SpellPower.csv`, `TalentTab.csv`. This is the same shape `tools/dbc-extract.py` was designed against — same WDBXEditor field conventions documented at the top of that script.
- TC source at `~/web/wow/src/server/scripts/...` — Cata instance scripts hardcode boss spell IDs and mob → spell mappings. This is the *authoritative* source for "which mob casts which ID" (DBC has the spell metadata but not the caster binding).
- The existing `tools/dbc-extract.py` (195 lines) already wired to read these same CSVs — extend it; don't replace it.

**Process:**
1. **Extend `tools/dbc-extract.py`** into a generalized verifier. New mode: read all `docs/dungeons/*/encounter.md` files, parse out every `spell_id:` (UNVERIFIED or otherwise), join against `Spell.csv` (name, family, school, attributes) + `SpellEffect.csv` (effect type, radius, target type) + `SpellCooldowns.csv` + `SpellDuration.csv`. Update DBC paths from the Windows hardcode to `~/web/wow/doc/dbc/`.
2. **Cross-reference TC scripts** for the mob-binding side: grep `server-core/src/server/scripts/EasternKingdoms/`, `Kalimdor/`, and the Cata-specific dungeon directories for each spell ID. Any encounter.md ID that doesn't appear in the corresponding boss/mob script is suspect (this is what catches the BRC 75763 / 75476 class of bug).
3. **Reconcile per encounter.md**: for each UNVERIFIED ID, the script emits one of: `verified` (DBC name matches doc + TC script casts it), `name-mismatch` (DBC has a different spell name → doc likely wrong), `unbound` (DBC matches but no TC script uses it → could be heroic-only, mob-AI-only, or wrong), or `not-found` (DBC has no row).
4. **Update each `encounter.md`** with verified IDs; for `not-found` or `name-mismatch` rows, swap in the correct ID if obvious from the script grep, otherwise mark `# SKIPPED-AWAITING-INGAME` so Phase 2 codegen drops it cleanly.
5. **Output**: `docs/data/dungeon-mechanic-db.csv` — the canonical, machine-readable mechanic table that Phase 2's codegen consumes. Schema: `spell_id, spell_name, dungeon_slug, source_npc_entry, mechanic_class (avoidable|interrupt|dispel|phase), interruptible, dispel_type, target_pattern, priority, avoid_radius_y, status (verified|skipped|name-mismatch|unbound), notes`.
6. **Update `docs/research/dbc-verification-checklist.md`** with the audit trail (per-ID disposition, source citations).

**Gating rule:** every row consumed by avoidance (`mechanic_class == avoidable`) must be `status == verified` before Phase 2 ships. Interrupt/dispel rows can land in incremental batches — a missing interrupt entry just means we don't kick that one cast, not that we step into a wrong pool.

**Honest unknowns:** a small residual set may remain `unbound` — e.g., a spell DBC says exists but no TC script casts it (could be heroic-only logic gated behind a difficulty flag, or a mob ability we haven't matched to its NPC entry). Per project rule "stick to local sources only," these are flagged for in-game verification rather than guessed at.

---

## Phase 1 — Telemetry foundation

Drop the failure-only cast log; replace with a single chokepoint that captures every cast attempt, every combat-window summary, and DPS via the `OnDamage` hook. Everything in Phases 2–5 will consume this.

**New module: `src/AltbotCombatLog.{h,cpp}`** — owns:
- `OnCastIssued(bot, target, spellId, tierIdx, tierName, result, ctx)` — per cast, called from inside `CastWithLog`
- `OnCombatEnter(bot)` / `OnCombatLeave(bot)` — driven from the existing combat-elapsed state machine in `AltbotAI::Update` (`src/AltbotAI.cpp:153–167`)
- `OnDamage(attacker, victim, damage)` — `UnitScript::OnDamage` hook, registered in `AltbotLoader.cpp` next to existing `AddSC_*` blocks. Filters on a `flat_set<ObjectGuid>` of registered altbot GUIDs maintained by `AltbotMgr`.
- `OnNoTierFired(bot, target, ctx, mode)` — strategies already log this (e.g. `FrostMageStrategy.cpp:158`); add a counter call.

**Cast-log site extension.** `CastWithLog` (`src/strategies/StrategyUtil.cpp:124`) is the one place every cast goes through. Extend signature:
```cpp
SpellCastResult CastWithLog(Player* bot, Unit* target, uint32 spellId,
                            char const* specLabel, int tierIdx, char const* tierName);
```
Keep a thin 4-arg shim that defaults `tierIdx=-1, tierName="maint"` for maintenance casts. Reserve `tierIdx=-2` for "encounter override" so Phase 3 doesn't churn the schema. Each strategy passes tier metadata when calling — the existing `Tier_X` methods already know which tier they are.

**Per-cast log line** (pipe-separated `key=value`, written via `TC_LOG_INFO("altbot.combat", ...)` to a new log channel):
```
ts | bot | spec | tier_idx | tier_name | spell_id | spell_name |
target_guid | target_hp_pct | bot_hp_pct |
bot_resource_kind | bot_resource_pct | gcd_remaining_ms |
combat_elapsed_ms | result_code
```
Pipe-separated is greppable, SimC-importable, and trivial to parse. JSON/CSV are overkill.

**Per-fight summary** (one whisper to master at combat-leave, gated on `combat_elapsed_ms >= MinFightMs`, default 15000ms):
```
[Altbot] FrostMage Pyrah — 47s | DPS 8421 | 38 casts (37 ok, 1 LoS) | gcd 78%
        | top: Frostbolt(18) IceLance(11) FFB(5) | mana 41–100 | no-tier 0
```

**Throttling.** With ≥2 bot whispers within 1500ms, `AltbotMgr` batches into a single multi-bot summary so a master with 4 bots gets one consolidated message per fight. Trash skirmishes under 15s drop their summaries.

**Toggles** in `altbot.conf` via `AltbotConfig`:
- `Altbot.Telemetry.Enabled` (default false)
- `Altbot.Telemetry.WhisperSummary` (default true)
- `Altbot.Telemetry.PerCastLog` (default false — high volume; opt-in)
- `Altbot.Telemetry.MinFightMs` (default 15000)

Per-bot override: chat command `.altbot trace <bot> on|off` so we can crank one bot to verbose without flooding the file.

**DPS measurement.** `UnitScript::OnDamage(attacker, victim, damage&)` fires before damage applies. Resolve `attacker->ToPlayer()` (or `attacker->GetOwner()->ToPlayer()` for pets), bail if not in `AltbotMgr::IsAltbot(player)`, accumulate `damageDealt += damage` on the bot's `CombatLogState`. Cost is one map lookup per damage event.

---

## Phase 2 — Mechanic DB + ground-effect avoidance *(ships the ToTT fix)*

**Codegen pipeline.**
- `tools/gen-mechanic-db.py` reads `docs/data/dungeon-mechanic-db.csv` (Phase 0 output) and emits `src/data/MechanicDb.gen.h` and `src/data/MechanicDb.gen.cpp` — a sorted-by-`spell_id` `constexpr` array with `lower_bound` lookup.
- Generated files are checked in (no CMake hook yet — small change cadence). Re-run the script when the CSV changes.

**New module: `src/EncounterMechanics.{h,cpp}`** — thin query API:
```cpp
MechanicEntry const* Lookup(uint32 spellId);
bool IsAvoidable(uint32 spellId);
DispelType GetDispelType(uint32 spellId);
InterruptPriority GetInterruptPriority(uint32 spellId);
```

**Avoidance hook in `AltbotPositionManager::FastTick`** (`src/AltbotPositionManager.cpp:117`). Add `DetectMechanicHazard(nowMs)`:
- `Cell::VisitGridObjects(_bot, hazardVisitor, 12.0f)` over `DynamicObject` and `AreaTriggerEntity`
- For each, read `GetSpellId()` and look up in the mechanic DB
- If `IsAvoidable && hostile_to_bot && bot inside avoid_radius_y`, set the existing `_firstFireSeenMs` flag

The retreat path in `Tick` is unchanged — the same `FindSafeRetreatPosition` → `MovePoint` machinery fires, just with a deterministic trigger instead of the 8% HP heuristic. The HP heuristic stays as a fallback for unmapped mechanics.

**Risk.** Confirm `AreaTriggerEntity::GetSpellId()` exists in TC 4.3.4. If not, fall back to `DynamicObject`-only — that's already enough to fix Noxious Mire (Volatile Bolt patches, Stonecore Crystal Storm, Halls Quicksand, etc. — most ground-effect spells use `DynamicObject`).

---

## Phase 3 — Interrupts + dispels + boss-phase reaction

**New module: `src/EncounterReactions.{h,cpp}`** — three helpers strategies opt into:

1. `TryInterruptNearbyCast(bot, radius=30y, minPriority=MUST_INTERRUPT)`. `Cell::VisitGridObjects` over hostiles in 30y, read `Unit::GetCurrentSpell(CURRENT_GENERIC_SPELL/CURRENT_CHANNELED_SPELL)`, look up DB priority. If high enough and the bot has an off-CD interrupt cached on the strategy, fire it via `CastWithLog(..., tierIdx=-2, tierName="interrupt")`. v1 accepts cross-bot duplicate-cast cost (two bots may both kick) — claim queue is Phase 6.

2. `TryDispelAlly(bot, party, specDispelMatrix)`. Spec-aware: Resto Sham `Cleanse Spirit/Purify Spirit` (magic+curse), Holy Pal `Cleanse` (poison+disease+magic on heroic), Mage `Remove Curse`, Druid `Remove Corruption / Nature's Cure`, Hunter `Tranq Shot`, Warlock pet `Devour Magic`. Walks `member->GetAppliedAuras()`, looks up DB dispel type, filters by what the bot can actually remove.

3. `TryPurgeHostile(bot, target)`. Same shape, hostile-target side (Spiritmender Healing Wave 76308 etc.) — only the specs with offensive purge.

**Hook points** (all after the standard cast-in-progress + GCD probe, before the rotation tier dispatch):

| Strategy | Interrupt slot | Dispel slot |
|---|---|---|
| FrostMage | After GCD probe (~line 149), before `Tier_DeepFreeze` | Above `Tier_Frostbolt` filler (Mage curse-remove only) |
| AffWarlock | After rotation guards, before highest DoT tier | n/a (no friendly dispel; Devour Magic is pet-driven) |
| MmHunter | Before `Tier_SteadyShot` filler | Above filler (Tranq Shot offensive purge) |
| RestoShaman | After GCD probe (~line 95), before `Tier1_SelfEmergency` | High-priority dispels (e.g. Stonecore Curse of Entropy, Ulthok Curse of Fatigue 76094) preempt `Tier1`; normal dispels run between Riptide and SpiritLink tiers |

**Boss-phase surfacing.** Extend `AltbotTickContext` with:
```cpp
uint32 bossNpcEntry  = 0;
uint8  bossPhase     = 0;
uint8  bossStateRaw  = 0;   // EncounterState enum value
uint8  bossHpPct     = 0;
```
Populated in `AltbotAI::Update` from `master->GetMap()->ToInstanceMap()->GetInstanceScript()`. Phase derivation via a small hardcoded `EncounterPhases::Resolve(npc, hpPct)` (e.g. Lady Naz'jar 60%/30% triggers). Strategies *may* branch on `ctx.bossPhase` — additive, no existing strategy required to consume it.

---

## Phase 4 — DPS bug-hunt + 4-strategy tuning

Driven by Phase 1 telemetry. **Questions answered from logs, in priority order:**

1. **Are bots casting every tier we expect?** Tier histogram. Anomalies: persistent `noTierTicks > 0`, or one tier hogging 90% of casts.
2. **GCD utilization?** Below ~85% on a stationary fight = DPS leak. Buckets: <60% broken, 60–80% pipeline issues, 80–95% acceptable.
3. **Procs burned same-tick or wasted?** Compare proc-aura-up tick count vs proc-consumed cast count. Frost Mage BF/FoF, MM Hunter Fire!, Aff Warlock Nightfall.
4. **Failed-cast retry storms?** Repeated `result=49` (LoS) → positioning wrong. Repeated `result=53` (MOVING) → MaintainRange chase racing the rotation. Repeated `result=69` (NOT_READY) → within-tier GCD lag.
5. **Resource floors**: does Aff actually enter Crisis mode? Is MM focus-starved into Steady-Shot loops?

**Concrete fixes per spec, impact-ordered:**

**Resto Shaman** — biggest known regressions:
- `RestoShamanStrategy.cpp:223–224` — Spirit Link Totem and Mana Tide Totem **stubbed at 0**. Resolve via DBC totem-slot inspection (research notes already in `docs/research/talent-aware-combat-design.md`). Add `Tier_ManaTide` (mana<60%) and `Tier_SpiritLink` (raid avg HP <40%).
- Tidal Waves haste tracking: cache the proc-aura ID in `ResolveSpellCache`; tier ordering becomes "Healing Surge if TW up else GHW".

**Frost Mage** (`src/strategies/FrostMageStrategy.cpp`):
- Add `Tier_OffensiveCDs` after `Tier_DeepFreeze`: Mirror Image, Icy Veins, Cold Snap (chained CDs), Time Warp aligned with master combat-elapsed > pull-window. New cache entries; same `TryCast` pattern.
- `Tier_Counterspell` — gated on master's target being a caster (`UNIT_STATE_CASTING` on target). May overlap with Phase 3's interrupt module — make Counterspell call into `TryInterruptNearbyCast`.
- `Tier_Evocation` — mana <30%, no incoming damage in last 2s.

**Aff Warlock** (`src/strategies/AffWarlockStrategy.cpp`):
- Promote Curse of the Elements into a tier-priority slot (currently maintenance-only), tier above Bane of Agony — debuff windowing matters for raid-buff stacking.
- Replace 12s SoC reuse-timer guess (`AffWarlockStrategy.cpp:360–361`) with aura-presence check on the target — Phase 1 cast log will show how often the heuristic gets it wrong.

**MM Hunter** (`src/strategies/MmHunterStrategy.cpp`):
- Kill Shot → Chimera chain: after KS fires below 20%, `Tier_ChimeraShot` runs next tick (just tier order — already cached).
- `Misdirection`-self bug: in `DoMisdirection` (`MmHunterStrategy.cpp:234–246`), guard with `if (tank == bot) return;` before casting. Master-fallback path lets `tank == bot` when solo.
- Careful Aim talent gating on Aimed Shot tier: `target_hp_pct > 90 || HasAura(rapid_fire)`.

**Cross-cutting framework** — add to `StrategyUtil`:
```cpp
SpellCastResult CastOnUseTrinket(Player* bot, char const* specLabel);  // walks EQUIPMENT_SLOT_TRINKET1/2
SpellCastResult CastRacial(Player* bot, char const* specLabel);        // switches on getRace()
SpellCastResult CastBloodlust(Player* bot, char const* specLabel);     // shaman-only — defer until Ele/Enh strats land
```
Each strategy registers `DoOnUseCDs(bot)` as a maintenance-tier call. Pre-pull sequence: `DoPrePull(bot, target)` runs once when `combatElapsedMs == 0`, covering pet-summon checks, armor/aspect verification, hunter Misdirection-on-tank.

---

## Phase 5 — Scaffold-driven onboarding of the remaining 13 specs

The existing 4 strategies are deeply hand-built one-offs. ~70% of each is boilerplate (cache resolve, `IsOnCooldown`/`TryCast`, cast-in-progress + GCD guards, `MaintainRange`, `DoMaintenance`, `DoDefensives`, the standard `Update` shape). The interesting ~30% is the Spell enum, tier methods, proc tracking, and class-specific gotchas.

**New tool: `tools/scaffold-strategy.py`.** Python 3, no third-party deps. Inputs:
```
--class WARLOCK --tree 867 --slug demonology-warlock --role ranged-dps
--spec-doc docs/specs/demonology-warlock.md --filler "Shadow Bolt"
```
Outputs:
- `src/strategies/DemonologyWarlockStrategy.{h,cpp}` — boilerplate filled, `// TODO(rotation):` markers where Tier methods go.
- A diff snippet for `AltbotStrategyFactory.cpp` (tree-ID constant + dispatch case + `#include`).
- A printed checklist: "Verify TalentTab.dbc 867; populate Tier_* per docs/specs/{slug}.md; add proc-aura name filters; run Phase-1 audit."

Templating: source the existing 4 strategies, strip class-specific bodies via Python `string.Template`, retain pattern.

**Estimated cost per spec:** research 1d (read survey) + scaffold 30min + rotation authoring 1–2d + audit/test 1d = **~3–4 days/spec**, vs. ~2 weeks for the existing one-offs.

**Onboarding order** (memory rule: tank specs come last; healer → ranged → melee → tank; tanks excluded entirely per project memory):
1. Disc Priest, 2. Holy Priest, 3. Resto Druid, 4. Holy Paladin
5. Shadow Priest, 6. Balance Druid, 7. Elemental Shaman *(unlocks Bloodlust framework)*
8. Enhancement Shaman, 9. Combat Rogue, 10. Feral Cat
11. Retribution Paladin, 12. Fury Warrior
13. Frost DK *(last — tank-adjacent)*

---

## Critical files

| File | Phase | Role |
|---|---|---|
| `tools/dbc-extract.py` | 0 | Extend to multi-table join for mechanic verification |
| `docs/data/dungeon-mechanic-db.csv` | 0 | New canonical source for mechanic DB codegen |
| `docs/dungeons/{slug}/encounter.md` | 0 | Verified spell IDs / `# SKIPPED-AWAITING-INGAME` markers |
| `src/strategies/StrategyUtil.cpp` | 1 | Extend `CastWithLog` signature with tier metadata; sole cast chokepoint |
| `src/AltbotCombatLog.{h,cpp}` | 1 | **New** — per-cast/per-fight capture, OnDamage hook, whisper batcher |
| `src/AltbotLoader.cpp` | 1 | Register the new `UnitScript` + combat-log script |
| `src/AltbotAI.cpp` | 1, 3 | Combat-enter/leave edges; populate boss-state ctx fields |
| `src/AltbotConfig.cpp` | 1 | Telemetry config keys |
| `src/AltbotMgr.cpp` | 1 | Whisper batcher; altbot GUID set for OnDamage filter |
| `src/AltbotTickContext.h` | 3 | Add bossNpcEntry / bossPhase / bossHpPct fields |
| `tools/gen-mechanic-db.py` | 2 | **New** — emits MechanicDb.gen.{h,cpp} from CSV |
| `src/data/MechanicDb.gen.{h,cpp}` | 2 | **New, generated** — `constexpr` sorted-by-id table |
| `src/EncounterMechanics.{h,cpp}` | 2 | **New** — thin lookup API |
| `src/AltbotPositionManager.cpp` | 2 | Hook `DetectMechanicHazard` into `FastTick` |
| `src/EncounterReactions.{h,cpp}` | 3 | **New** — interrupt / dispel / purge helpers |
| `src/strategies/{Frost,Aff,Mm,Resto}*Strategy.cpp` | 3, 4 | Slot interrupt/dispel tiers; apply DPS fixes |
| `src/strategies/RestoShamanStrategy.cpp:223–224` | 4 | Resolve Spirit Link / Mana Tide totem stubs |
| `src/strategies/MmHunterStrategy.cpp:234–246` | 4 | Misdirection-self guard |
| `tools/scaffold-strategy.py` | 5 | **New** — generates per-spec strategy scaffold |
| `src/AltbotStrategyFactory.cpp` | 5 | Tree-ID constants + dispatch cases for new specs |

**Functions/utilities to reuse (don't re-invent):**
- `StrategyUtil::CastWithLog` (`StrategyUtil.cpp:124`) — extend, don't replace
- `StrategyUtil::FindSpellByFamilyName` (`StrategyUtil.cpp:135`) — name-collision-safe lookup; keep using it
- `StrategyUtil::FindTank`, `FindLowestHpAlly`, `CountInjured`, `AllAtFullHp` — already-shared helpers
- `AltbotPositionManager::FindSafeRetreatPosition` (`src/AltbotPosition.cpp`) — retreat path; mechanic hazard detector reuses it via the `_firstFireSeenMs` flag
- `AltbotPosition::CountHostilesNearUnit` (target-anchored) vs `CountHostilesNear` (self-anchored) — preserve the distinction (CLAUDE.md gotcha #7)
- `Player::GetSpellHistory()->HasGlobalCooldown` — existing GCD probe
- `Cell::VisitGridObjects` — TC core grid query (CellImpl.h:168)
- `InstanceScript::GetBossState` (InstanceScript.h:231) — boss-state read
- `Unit::GetCurrentSpell(CURRENT_GENERIC_SPELL/CURRENT_CHANNELED_SPELL)` (Unit.h:1311) — read enemy casts for interrupt
- Existing `TC_LOG_INFO("altbot", ...)` channel; add a sibling `"altbot.combat"` channel for per-cast trace

---

## Verification

**Phase 0:** `dungeon-mechanic-db.csv` rows for `mechanic_class=avoidable` are 100% `status=verified`. BRC 75763/75476 corrected.

**Phase 1:** 4 bots on a target dummy 2 minutes. Inspect `logs/altbot-combat.log`: ≥ casts/min target for each spec, GCD utilization ≥85%, no `NO TIER FIRED` ticks once combat is engaged. Combat-leave whisper arrives within 1s. Telemetry-off build path: bot rotation behavior bit-identical to pre-Phase-1 (pure additive).

**Phase 2:** Run ToTT, pull Naz'jar Sentinels. Bots should step out of Noxious Mire patches within ~1 FastTick (1000ms) of standing in one. `altbot-combat.log` should show emergency-fire-move lines tagged with the Noxious Mire spell ID. Same test on Stonecore Crystal Storm.

**Phase 3:**
- Interrupt: ToTT trash Naz'jar Tempest Witch Lightning Bolt — bot kicks before it lands.
- Dispel (high-pri): Stonecore Ulthok Curse of Fatigue 76094 — Resto Shaman dispels within 1 tick of application.
- Purge offensive: ToTT Spiritmender Healing Wave 76308 — Hunter Tranq / Sham Purge fires.
- Phase reaction: Lady Naz'jar 60% transition surfaces in `ctx.bossPhase` (smoke check via log).

**Phase 4:** Per-bot per-spec damage from `OnDamage` totals exported as CSV; cross-reference SimC profiles cited in `docs/research/{spec}-guide-survey.md`. Bots within ±15% of sim-with-dummy baseline = green. Resto Sham mana sustainability over a 5-min boss.

**Phase 5:** Each new spec passes the same Phase 1 telemetry smoke test (casts/min, GCD util, no-tier=0) before being declared landed.

---

## Risks & speculative items

- **`OnDamage` hook performance.** Fires for every damage event server-wide. Mitigation: branch-predict-friendly early-out via `flat_set<ObjectGuid>` lookup keyed on attacker GUID. With ≤10 bots the set is tiny.
- **Per-cast log volume.** ~20 casts/sec × 10 bots × ~200 bytes = ~3.5GB/day. Mitigation: ship `PerCastLog=false` default; explicit opt-in for diagnostic sessions. Per-fight summary is always-on (cheap).
- **Whisper spam.** 4 bots × 1 fight per 30s on trash farming. `MinFightMs` threshold + multi-bot batch consolidation. Add `Altbot.Telemetry.WhisperBossesOnly` if needed.
- **`AreaTriggerEntity::GetSpellId()`.** Verify exists in TC 4.3.4. Fallback: `DynamicObject`-only avoidance, which still fixes Noxious Mire / Crystal Storm / Quicksand class.
- **`Spirit Link / Mana Tide` DBC disambiguation.** Flagged deferred at `RestoShamanStrategy.cpp:220–224`. ~1 day of unknowns; treat as a blocker for the Resto fixes within Phase 4 only.
- **Cross-bot duplicate interrupt/dispel.** Two bots may both kick the same cast. v1 accepts the cost; a claim queue in `AltbotMgr` is reserved for a Phase 6 refinement.
- **Phase-table generation.** Phase 3's `EncounterPhases::Resolve` is hand-coded for ToTT bosses initially; YAML-driven generation deferred until we see how many phase rules we actually need.
- **Re-planning.** Phases 4 and 5 will benefit from re-planning once Phase 1 telemetry is producing real data — the priority order of DPS fixes will shift based on what the logs actually show. Treat the Phase 4 fix list as the current best guess, not a contract.

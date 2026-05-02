# Cata Altbot Docs

Source-of-truth documentation for the bot's combat, healing, tanking, and DPS decisions. Each doc is **AI-consumable** — structured YAML blocks designed to lift cleanly into `static const` tables in `AltbotCombat.cpp` (and adjacent code) when the bot's combat layer is implemented.

This is not a player-facing strategy guide. The audience is the bot's logic and the engineer iterating on it.

---

## Layout

```
docs/
├── README.md                                ← this file
├── specs/                                   ← per-class-spec class data (rotation, CDs, mana, defensives, consumables, non-encounter content)
│   ├── resto-shaman.md
│   └── ... (one file per spec, healer-first ordering)
├── roles/                                   ← class-agnostic role-level patterns (cross-content)
│   ├── healer.md
│   ├── ranged-dps.md
│   ├── melee-dps.md
│   └── tank.md
├── dungeons/                                ← per-dungeon encounter facts + per-role response docs
│   ├── README.md
│   └── {dungeon-slug}/
│       ├── encounter.md                     ← role-neutral mechanic facts (phases, spell IDs, positioning, interrupts, dispels)
│       ├── healer.md                        ← healer responses
│       ├── ranged-dps.md                    ← ranged DPS responses
│       ├── melee-dps.md                     ← melee DPS responses
│       └── tank.md                          ← tank responses
├── raids/                                   ← per-zone raid bundles (consolidated at zone level, not per boss)
│   └── {zone-slug}/
│       ├── encounter.md                     ← top-level zone notes + H2 section per boss; 10/25 + N/H differences inline per boss
│       ├── healer.md                        ← healer responses across all bosses in the zone
│       ├── ranged-dps.md                    ← ranged DPS responses
│       ├── melee-dps.md                     ← melee DPS responses
│       └── tank.md                          ← tank responses
└── research/                                ← multi-guide reconciliation audit trails
    └── {topic}-guide-survey.md
```

---

## Layering model

The bot's logic for any decision is composed by layering four levels:

```
encounter facts (docs/dungeons/{dungeon}/encounter.md)
    ↓ overridden by
encounter responses per role (docs/dungeons/{dungeon}/{role}.md)
    ↓ which deviates from
role-level cross-content patterns (docs/roles/{role}.md)
    ↓ which is bound to class abilities by
per-spec class data (docs/specs/{spec}.md)
```

When the bot is in combat, the active layers are:
- **Always**: spec doc + role generalist doc
- **In a dungeon/raid encounter**: + encounter.md + dungeon role doc
- **Outside an encounter (questing, world combat, idle)**: spec doc's "Non-encounter content" section + role generalist doc only

A given decision (e.g., "what should I cast right now?") evaluates layers top-down, with later layers overriding earlier ones.

---

## Doc-count strategy

To keep documentation from getting confusing as it scales:

1. **Four levels max** — `specs/`, `roles/`, `dungeons/{dungeon}/`, `research/`. No further nesting.
2. **Reference, never duplicate** — every cross-link is by file path. If two docs share content, one references the other.
3. **READMEs at each tier** document the convention so the layout is self-explanatory.
4. **For raids (later)** — consolidate at the raid level (`docs/raids/{raid}/encounter.md` + 4 role docs), not per-boss, to keep doc count linear in raids rather than bosses.
5. **Stop signal**: if a doc repeats >30% of another doc's content, it's wrong — collapse or restructure.

---

## Research methodology

For each new doc:
- **Numerical facts** (spell IDs, mana costs, cast times, cooldowns, durations, item IDs): single canonical source — Wowhead's Cata 4.3.4 archive (`wowhead.com/cata/spell=<id>`).
- **Strategy / judgment content** (rotation priority, threshold values, cooldown triggers, target-selection rules): pull at least 4-5 distinct community guides and reconcile. Disagreements get flagged inline.
- **Audit trail**: every research-driven doc has a companion `docs/research/{topic}-guide-survey.md` documenting sources, consensus, disagreements, and which thresholds are author-judgment vs guide-derivable.

---

## Current coverage (last updated 2026-05-01)

| Tier | Doc | Status |
|---|---|---|
| roles/ | healer.md | complete (triage hierarchy, dispel taxonomy, mana pacing, **support DPS during downtime**, anti-patterns, class-spec mapping) |
| roles/ | ranged-dps.md | complete (range mgmt, target priority, interrupt rotation backup, AoE thresholds, mobility, anti-patterns) |
| roles/ | melee-dps.md | complete (rear-arc discipline, gap-closer pacing, interrupt as primary duty, cleave thresholds, anti-patterns) |
| roles/ | tank.md | complete (active mitigation cycle, threat-on-pull, cone facing, defensive CD pacing, kite path planning) |
| specs/ | **resto-shaman.md** | complete: encounter rotation (8 healing tiers + 1 support-DPS tier) · cooldown profile · proc reactions · mana mgmt · consumables · non-encounter content · leveling rotation (14 spell-unlock-bracketed sub-rotations) · UNVERIFIED registry |
| specs/ | holy-paladin.md | NOT STARTED — next per ordering |
| specs/ | holy-priest.md, disc-priest.md, resto-druid.md | NOT STARTED |
| specs/ | DPS specs | NOT STARTED (ranged before melee per project ordering) |
| specs/ | tank specs | NOT STARTED (last per project ordering — see memory) |
| dungeons/ | leveling.md | complete (cross-class era-by-era + per-role progression + mechanic catalog + LFD specifics + open module gaps) |
| dungeons/ | throne-of-the-tides/ (encounter + 4 role docs) | complete |
| dungeons/ | launch Cata 5-mans (BRC, Stonecore, VP, Lost City, HoO, Grim Batol) | NOT STARTED — full ToT-style treatment per dungeon (5 files + survey). 6 dungeons backlog. |
| dungeons/ | 4.1 troll heroics (Zul'Aman, Zul'Gurub) | NOT STARTED — full ToT-style treatment. |
| dungeons/ | 4.3 troll heroics (End Time, Hour of Twilight, Well of Eternity) | NOT STARTED — full ToT-style treatment. |
| dungeons/ | leveling-revamps-supplement.md (vanilla/TBC/WotLK dungeons hit during leveling) | NOT STARTED — single quick-coverage supplement, flag non-tank-and-spank only. |
| raids/ | T11 (Baradin Hold, BWD, BoT, ToFW), T12 (Firelands), T13 (Dragon Soul) | NOT STARTED — per-zone bundles (encounter.md with H2 boss sections + 4 role response docs + survey). 10/25 + N/H differences inline per boss, NOT separate docs. |
| research/ | resto-shaman-guide-survey.md | complete (max-level reconciliation + leveling-rotation reconciliation + Telluric Currents correction note) |
| research/ | leveling-dungeons-guide-survey.md | complete |
| research/ | throne-of-the-tides-guide-survey.md | complete |

---

## Backlog

Listed in suggested execution order. Each item names what to do AND why it's the next priority.

### Spec docs (in role order: healer → ranged → melee → tank)

1. **`docs/specs/holy-paladin.md`** — second healer spec, follows the `resto-shaman.md` template (12 sections + leveling rotation). Pairs cleanly with the existing `roles/healer.md` triage / dispel / mana / support-DPS patterns. Verify spell unlock levels via Wowhead Cata Classic; reconcile rotation across 4-5 guides per project research-depth standard.
2. **`docs/specs/holy-priest.md`** — third healer spec. Cross-link to `roles/healer.md` Chakra mode in the support-DPS section.
3. **`docs/specs/disc-priest.md`** — fourth healer spec. SPECIAL CASE in support DPS — Atonement Smite *is* the heal model, not downtime. The spec doc should reflect this (rotation tiers integrate Smite at higher priority than other healers, gated on Atonement target selection).
4. **`docs/specs/resto-druid.md`** — fifth healer spec. HoT-rolling philosophy differs from Resto Shaman's reactive style; rotation tier shape needs adjustment.
5. **Ranged DPS specs** — Frost Mage first (iconic for groups + Counterspell + Polymorph CC). Then Affliction Warlock, Marksmanship Hunter, Shadow Priest, Balance Druid, Elemental Shaman. Order is interrupt/CC value to a leveling group.
6. **Melee DPS specs** — Combat Rogue first (Kick is the shortest interrupt in 4.3.4). Then Fury Warrior, Ret Paladin, Frost DK, Enhancement Shaman, Feral Cat Druid.
7. **Tank specs** — Prot Warrior, Prot Paladin, Blood DK, Feral Tank Druid (Guardian-equivalent). **Last per project ordering** (see `~/.claude/projects/-Users-seth-oharra-web-cata-altbot/memory/feedback_role_ordering.md`).

**Survey-first rule (added 2026-05-01):** every spec doc is preceded by a `docs/research/{spec}-guide-survey.md` that reconciles 4-5 sources. For specs where the role has multiple competitive specs (Mage Arcane/Fire/Frost, Warlock Aff/Demo/Destro, Hunter BM/MM/SV, Rogue Combat/Assn/Sub, Warrior Arms/Fury, DK Frost/Unholy), the survey opens with a **Spec selection** section that picks the winning spec for 4.3.4 PVE. The chosen spec is what the spec doc covers.

### Cata 5-man dungeons (in scope as of 2026-05-01)

Each dungeon: write `docs/research/{slug}-guide-survey.md` first, then full `docs/dungeons/{slug}/` bundle (encounter.md + 4 role response docs) per the throne-of-the-tides template.

8. **Launch Cata 7** (ToT done): blackrock-caverns → the-stonecore → the-vortex-pinnacle → lost-city-of-the-tolvir → halls-of-origination → grim-batol.
9. **Patch 4.1 troll heroics**: zul-aman → zul-gurub.
10. **Patch 4.3 troll heroics** (gate Dragon Soul): end-time → hour-of-twilight → well-of-eternity.
11. **`docs/dungeons/leveling-revamps-supplement.md`** — single supplement covering vanilla/TBC/WotLK dungeons hit during 1-85 leveling (Cata Deadmines, Cata SFK, BRD, ScholoStrat, Mana-Tombs, AN, etc.). One paragraph per dungeon. Flag any non-tank-and-spank mechanic (kiting, dispel-required, vehicle/escort, multi-add CC); skip the rest.

### Cata raids (in scope as of 2026-05-01)

Per-zone format. For each zone: `docs/research/{zone-slug}-raid-survey.md` first (4-5 guides per zone, not per boss), then `docs/raids/{zone-slug}/encounter.md` (top-level zone notes, then H2 sections per boss with phases/spell IDs/interrupts/dispels/positioning + inline `### 10-man` / `### 25-man` / `### Heroic` notes only where mechanics actually diverge) + 4 role response docs covering all bosses in the zone with per-boss subsections only where the role's behavior changes.

12. **T11**: baradin-hold → blackwing-descent → bastion-of-twilight → throne-of-the-four-winds.
13. **T12**: firelands.
14. **T13**: dragon-soul.

### DBC verification pass — DEFERRED until server access available

The Resto Shaman spec doc has 9 UNVERIFIED items that need confirmation against TC's `Spell.dbc` files. **The full step-by-step checklist lives at `docs/research/dbc-verification-checklist.md`** — open that file when at the computer with TC source tree access. It contains:

- 3 ways to read DBC values (in-game GM lookup, WDBXEditor, sqlite-converted)
- Per-item: spell ID, what to check, current doc value, where to update, what to do with the result
- Post-verification cleanup steps (update spec doc, audit trail, delete the checklist file)

The 3 most impactful items are the **baseline-vs-talent ambiguities** for Spirit Link Totem, Mana Tide Totem, and Nature's Swiftness — these change whether those spells appear in the bot's rotation from level ~30-35 onward (talent path) or only at level 85 (baseline-spec path). Until verified, the docs default to treating them as "available when the talent tree could plausibly grant them" with UNVERIFIED markers throughout.

None of the 9 items block writing more spec docs or building out the doc tree. They become real blockers only when `AltbotCombat.cpp` is referencing these values as compile-time constants OR when the bot's level-gating logic needs to decide "talent or baseline."

### Implementation gaps in the bot module (NOT doc work)

Tracked in `docs/dungeons/leveling.md` Open Issues block. Reproduced here so they don't get lost:

| Issue | Module work needed | Workaround until fixed |
|---|---|---|
| Vehicle phase (Oculus drake fight) | Detect vehicle entry, pause AltbotAI until exit | Blacklist Oculus from bot LFD queue |
| Escort NPC follow logic (Halls of Stone Brann, CoS Arthas, Old Hillsbrad Thrall, Black Morass Medivh) | Extend `AltbotFollow.cpp` with `FollowNPCTarget(guid)` mode | Bot follows tank as usual; works imperfectly |
| Dungeon quest pickup/turn-in | New `AltbotQuestHandler.cpp` module | Manual via master |
| Heirloom auto-equip from master's bags | New gear-management module | Manual equip |
| LFD self-queue | New `AltbotLFG.cpp` module | Master queues bot manually |
| Vote-kick handling (accept deserter or refuse) | Bot-policy decision + handler | Default behavior is Blizzard-engine accept |

These are flagged so future implementation sessions know what's stub vs missing.

---

## Research source registry

Validated sources for the cata-altbot research corpus. Pre-vetted so future research dispatches don't need to rediscover them.

### Numerical-data sources (single-source allowed per project research-depth rule)

- **Wowhead Cata archive** — `https://www.wowhead.com/cata/spell=<id>` — primary source for spell IDs, mana cost, cast time, cooldown, duration, range, required level. Also `wowhead.com/cata/npc=<id>` for boss/mob abilities and `wowhead.com/cata/item=<id>` for consumable IDs. **Reliably resolves via WebFetch.**
- **TC `Spell.dbc`** — when implementing, the server's DBC files are the FINAL canonical source. Use Wowhead for design-time decisions; verify against DBC at integration time.

### Strategy-source rotation (require 4-5 reconciled per project rule)

Sources that reliably resolve via WebFetch:
- **Icy Veins Cata Classic** — `https://www.icy-veins.com/cataclysm-classic/<spec-or-dungeon>-guide` — editorial guides, clean rotation breakdowns. ✅ WebFetch-friendly.
- **Wowhead Cata Classic guides** — `https://www.wowhead.com/cata/guide/...` — complementary editorial coverage. ✅ Mostly WebFetch-friendly; some pages 403.
- **Warcraft Wiki** — `https://warcraft.wiki.gg/wiki/<topic>` — best for talent/ability mechanic verification (e.g., Telluric Currents details). ✅ WebFetch-friendly.
- **wowtbc.gg** — `https://wowtbc.gg/cata/class-guides/<spec>/` — community guides. ✅ WebFetch-friendly when up.

Sources that often 403 — try via web.archive.org instead:
- Wowpedia (`wowpedia.fandom.com/wiki/<topic>`) — rich content but anti-bot. Use `warcraft.wiki.gg` mirror instead.
- Warmane forum threads — direct fetch fails; can sometimes pull via search-result excerpts.
- Original 4.3.4-era blogs (Manalicious, Pocket Heals, etc.) — variable; some still accessible.

Web.archive.org snapshots — `https://web.archive.org/web/<year>/<url>` — fallback for any source that 404s or 403s. Use the 2010-2012 timeframe for original-Cata material.

### Calling cadence

Project rule (recorded in `~/.claude/projects/-Users-seth-oharra-web-cata-altbot/memory/feedback_research_depth.md`):
- Hard numerical facts: 1 source (Wowhead Cata) is sufficient.
- Strategy / judgment content: 4–5 reconciled distinct authors.

---

## Templates

Established structures every new doc should follow. Consistency makes it easier for the eventual `AltbotCombat.cpp` to consume the YAML and for engineers to find content.

### Spec doc (`docs/specs/{spec}.md`)

Required sections in order (verbatim from `resto-shaman.md`'s "Spec doc template note"):

1. Identity
2. Target taxonomy
3. Spell Catalog (encounter spells + non-encounter spells)
4. Maintenance pass
5. Rotation — Priority List (encounter)
6. Cooldown Profile
7. Proc Reactions
8. Mana Management
9. Consumables
10. Non-encounter content (solo rotation, travel, idle, world-boss scaling, disengage)
11. Leveling rotation (per-bracket sub-rotations bracketed at spell-unlock breakpoints)
12. UNVERIFIED items (DBC verification queue)
13. Open hooks for `AltbotCombat.cpp`

For the support-DPS rotation tier specifically, follow the Tier 9 pattern from `resto-shaman.md`:
- Lowest priority in the rotation
- Gated on: party.lowest_hp >= ~95%, self.mana >= ~75%, tank maintenance buff up, no scripted damage incoming
- Cast priority within the tier: instant DoTs first, instant nukes second, hard-casts last
- Cancel mid-cast on any triage tier 1-3 trigger or pending dispel

### Role doc (`docs/roles/{role}.md`)

Established by `roles/healer.md`. Required structural elements:
- Role identity / responsibility summary
- Class-agnostic patterns (triage / range mgmt / active mitigation / etc.)
- Cross-content principles (what's true regardless of dungeon / level)
- Anti-patterns block with `bot_implementation` notes
- Class-spec mapping table (rows = patterns, columns = each class spec in the role)
- "How dungeon-level docs reference this doc" preamble

### Dungeon doc folder (`docs/dungeons/{dungeon-slug}/`)

Established by `throne-of-the-tides/`. Required files:
- `encounter.md` — role-neutral mechanic facts (phases, spell IDs, positioning, interrupts, dispels)
- `healer.md`, `ranged-dps.md`, `melee-dps.md`, `tank.md` — per-role response logic, layered on top of `roles/{role}.md` and the spec doc

Each role doc should declare its layering at the top:
```markdown
**Layering**:
- Cross-content {role} patterns live in `docs/roles/{role}.md`
- Per-spec rotation / CD / mana model lives in `docs/specs/{spec}.md`
- This doc layers {dungeon}-specific deviations on top of those.
```

### Research survey (`docs/research/{topic}-guide-survey.md`)

Established by the existing 3 surveys. Required sections:
1. Sources table (# / Guide / URL / Author / Date / Confidence)
2. Strong consensus list (4+ guides agree → spec disposition)
3. Disagreements (with likely explanation, with spec disposition)
4. Author-judgment items table (NOT guide-derivable — bot-author defaults flagged so future tuning knows what's a guess vs evidence)
5. Suggested next research passes (when more data is available)

### YAML conventions used in spec / role / encounter docs

- Spell entries: `id`, `mana_pct`, `cast_time_ms`, `cooldown_ms`, `on_gcd`, `range`, `school`, `target_type`, `notes`
- Rotation tiers: `tier`, `name`, `when` (predicate list — ALL must hold), `cast`, `notes`, optional `target_resolution` and `use_cooldown_first`
- Auras: referenced by symbolic name (`EARTH_SHIELD`, `RIPTIDE`, etc.) — IDs are the join key in the Spell Catalog
- Predicate functions: `aura.active(target, SPELL)`, `aura.missing(target, SPELL)`, `aura.expires_in_ms(target, SPELL) < N`, `spell.off_cooldown(SPELL)`, `target.hp_pct < N`, `party.injured_count(radius=N, hp_pct_lt=N) >= N`
- UNVERIFIED markers: inline comment OR table entry in the doc's "UNVERIFIED items" section

---

## Conventions registry

These are the rules-of-the-road that govern every doc. Codified once here so future sessions don't re-litigate them.

| Rule | Where written | Why |
|---|---|---|
| Hard numbers from Wowhead Cata; strategy from 4-5 guides reconciled | `feedback_research_depth.md` memory | Quality bar split: facts cheap to verify, strategy needs triangulation |
| Tank specs/docs come last (healer → ranged → melee → tank) | `feedback_role_ordering.md` memory | User direction 2026-04-30 |
| Four doc-tier max (specs, roles, dungeons, research); raids consolidated at raid level not boss level | `docs/README.md` Doc-count strategy | Prevents per-boss explosion |
| Reference don't duplicate; layering model resolves overrides | `docs/README.md` Layering model | Single source of truth per pattern |
| Audit trail per major research deliverable | `docs/research/` | Future tuning knows what's evidence vs author-judgment |
| AI-consumable YAML over prose | every doc | Lifts cleanly into `AltbotCombat.cpp` constants |
| Anti-patterns include `bot_implementation` block | role docs + dungeon role docs | Implementation-ready, not just descriptive |
| Disc Priest is a special case in support-DPS rotation | `roles/healer.md` Support DPS class-spec table | Atonement Smite IS healing, not downtime — flag in spec doc when written |

---

## Hooks for `AltbotCombat.cpp`

The eventual code structure mirrors the doc layout:

| Doc tier | Code structure |
|---|---|
| `docs/specs/{spec}.md` Spell Catalog | `static const SpellEntry kSpec_Spells[]` |
| `docs/specs/{spec}.md` Rotation tiers | `static const RotationTier kSpec_Rotation[]` |
| `docs/specs/{spec}.md` Cooldown Profile | `static const CooldownTrigger kSpec_Cooldowns[]` |
| `docs/specs/{spec}.md` Mana Management | `ManaState` enum + threshold helpers |
| `docs/specs/{spec}.md` Leveling rotation | `LevelBracketRotation[]` keyed by character level |
| `docs/roles/{role}.md` patterns | `RoleStrategy` base classes (HealerStrategy, RangedDpsStrategy, etc.) |
| `docs/roles/{role}.md` anti-patterns | enforcement layer in `RoleStrategy::Tick()` |
| `docs/dungeons/{dungeon}/encounter.md` | `EncounterProfile` per dungeon — phases, mechanics, interrupts |
| `docs/dungeons/{dungeon}/{role}.md` | `EncounterRoleOverrides` injected on top of the active spec/role |
| `docs/dungeons/leveling.md` per-bracket overrides | `LevelingDungeonProfile` — applies per-bracket threshold offsets in `EncounterProfile::Resolve()` |

The condition predicates documented in YAML (`aura.active(...)`, `party.count_within(...)`, etc.) define the helper API the AI layer needs to expose. That API is implemented once and reused across all spec / role / encounter docs.

---

## Picking up cold — the 5-minute orientation

If you (or a future session) need to get up to speed quickly:

1. Read this README.
2. Read `docs/specs/resto-shaman.md` end-to-end — it's the canonical example of a spec doc.
3. Read `docs/roles/healer.md` — canonical example of a role generalist.
4. Read `docs/dungeons/throne-of-the-tides/encounter.md` + one role doc — canonical example of a dungeon doc set.
5. Skim one research survey (e.g., `docs/research/resto-shaman-guide-survey.md`) to see the audit-trail shape.
6. Check the **Backlog** above for the next intended task.
7. Check `~/.claude/projects/-Users-seth-oharra-web-cata-altbot/memory/MEMORY.md` for any feedback / project memories that govern behavior.

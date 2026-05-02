# Dungeon Strategy Docs

Layout convention for dungeon-specific bot strategy.

## Folder structure

```
docs/dungeons/
├── README.md                                 ← this file
└── {dungeon-slug}/
    ├── encounter.md                          ← shared, role-neutral mechanic facts
    ├── healer.md                             ← healer response logic
    ├── ranged-dps.md                         ← ranged DPS response logic
    ├── melee-dps.md                          ← melee DPS response logic
    └── tank.md                               ← tank response logic (authored last)
```

`encounter.md` is the source of truth for boss phases, mechanic spell IDs, positioning, interrupts, dispels, and trash mechanics. Per-role docs reference `encounter.md` mechanics by spell ID and only encode role-specific responses (cooldown timing, target priority, anti-patterns, threshold deviations from the spec doc baseline).

## Authoring order

When adding a new dungeon:

1. **Pull guides** — at least 4 (5 ideal) distinct sources covering both normal and heroic. Use the `general-purpose` Agent for this.
2. **Write `encounter.md`** first — role-neutral facts, all bosses, all trash with non-trivial mechanics.
3. **Write `docs/research/{dungeon-slug}-guide-survey.md`** — audit trail of sources, consensus, disagreements, UNCERTAIN items, and author-judgment thresholds.
4. **Write per-role docs in the configured order**: healer → ranged-dps → melee-dps → tank (tank last per project preference).
5. Each role doc references its base spec (e.g., healer.md references `docs/specs/resto-shaman.md`) and only documents *deviations* from the spec's baseline thresholds.

## Why this split?

- Boss mechanics are role-agnostic at the FACT level (spell ID 76008 is Shock Blast regardless of who's interrupting it). What changes per role is the RESPONSE.
- Factoring the encounter facts into a single file lets all 4 role docs share a source of truth — when a mechanic detail is corrected, every role doc benefits without re-editing.
- Per-role docs stay tight: only the role-specific response logic, no duplicated encounter description.

## How dungeon docs layer on the rest of the doc tree

Each per-role dungeon doc layers on TWO higher levels:

1. **`docs/roles/{role}.md`** — class-agnostic role-level patterns (triage, range management, active mitigation, etc.). All cross-content rules live here; dungeon docs do not duplicate them.
2. **`docs/specs/{spec}.md`** — per-spec class-level data (rotation, CDs, mana, defensives, consumables, non-encounter content). Specifies which class abilities map to the role-level patterns.

Then the dungeon doc itself only documents **deviations** — encounter-specific threshold overrides, fight-specific CD plans, dungeon-specific anti-patterns.

## Reference: existing docs

- `docs/roles/healer.md`, `ranged-dps.md`, `melee-dps.md`, `tank.md` — role generalists (4 docs)
- `docs/specs/resto-shaman.md` — first per-spec healer doc (with leveling rotation)
- `docs/dungeons/leveling.md` — cross-class leveling-dungeon patterns (era-by-era summary, per-role progression by level, common mechanics catalog, LFD specifics)
- `docs/dungeons/throne-of-the-tides/encounter.md` + 4 role docs — first per-dungeon walkthrough, all roles
- `docs/research/resto-shaman-guide-survey.md` — audit trail for the spec (incl. leveling rotation reconciliation)
- `docs/research/leveling-dungeons-guide-survey.md` — audit trail for the leveling-dungeons doc
- `docs/research/throne-of-the-tides-guide-survey.md` — audit trail for ToTT

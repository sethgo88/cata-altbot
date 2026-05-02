# Ranged DPS — Role Generalist

**Class-agnostic ranged DPS patterns** that apply across all content (dungeon, raid, world). Per-spec details (which spell, which CD name) live in `docs/specs/{spec}.md`. Per-encounter overrides (target priority, interrupt assignment) live in `docs/dungeons/{dungeon}/ranged-dps.md`.

This doc is the central reference for *how a ranged DPS thinks* — range management, target priority, interrupt rotation, AoE thresholds, mobility, anti-patterns. The bot's ranged DPS logic in `AltbotCombat.cpp` should be structured around the patterns documented here, with class-specific bindings injected from the spec doc.

---

## Range management

```yaml
range_management:
  # Where to stand relative to the target.

  default_range:
    melee_classes_min: 5      # not relevant to ranged DPS but for contrast
    ranged_optimal: 30        # typical ranged spell max effective range minus a buffer
    ranged_max: 35-40         # max cast range
  rationale: |
    Standing at max range gives maximum reaction time on AoE telegraphs and
    reduces incidental damage from boss melee cleave / cone abilities.

  edge_cases:
    short_range_specs:
      examples:
        - shadow_priest (Mind Flay 30y)
        - balance_druid (most spells 30y)
      adjustment: optimal range = 25-28 (cushion within max range)

    proximity_required:
      examples:
        - hunter (Aspect of the Fox in 4.3.4 — channeled raid buff requires positioning)
        - mage (Improved Counterspell range matches max cast range — no adjustment)
      adjustment: per-spec; flagged in spec doc

  spread_rules:
    general: maintain >=8y spread from other allies when no clustering mechanic is active
    rationale: avoid splash mechanics (Fungal Spores, Lightning Surge, Shadow Crash, etc.)
    cluster_exceptions:
      - smart-AoE-heal proximity (Chain Heal jump radius, Holy Radiance range, Wild Growth)
      - explicit cluster mechanics (Spirit Link Totem)
      - encounter docs override per-fight

position_response_to_target_movement:
  # When the target moves toward / away from the bot
  target_closes_in (becomes melee threat):
    options:
      - kite (slow + back away)
      - tank-taunt-request (if the bot is grouped with a tank, request taunt before kiting)
    fallback: defensive CD + close-range damage
  target_walks_away:
    response: re-position to maintain optimal range; do not chase into untanked threat zones
```

---

## Target priority

```yaml
target_priority:
  # Generic priority rules. Per-encounter docs override with specific add lists.

  pull_evaluation:
    when_engaging_a_pull:
      step_1: query encounter / dungeon doc for explicit target_priority
      step_2: if no override, apply defaults below

  defaults:
    - tier: 1
      name: high-priority interruptable casters
      criteria:
        - mob.is_casting AND mob.cast.is_interruptible
        - mob.cast IS NOT lethal_cast (handled by interrupt rotation, not target switch)
        - mob is a low-HP add (caster archetype)
      action: focus down to remove cast threat

    - tier: 2
      name: explicit target priority (encounter-specified)
      examples:
        - "kill Spiritmender first" (ToTT trash)
        - "kill Mindlasher before Behemoth" (ToTT Ozumat P1)
      source: docs/dungeons/{dungeon}/ranged-dps.md target_priority blocks

    - tier: 3
      name: special-purpose target switches
      examples:
        - enslaved ally requiring damage to break charm (ToTT Ghur'sha P2)
        - stationary boss-channel target (Sappers in ToTT Ozumat P2)
      action: |
        Override standard priority; cease damage on default boss target until
        switch target resolves.

    - tier: 4
      name: standard boss / primary target
      action: tunnel / sustain DPS

  cc_marking:
    # When the bot is the CC class for an encounter
    rules:
      - identify CC target BEFORE pulling
      - apply CC at spawn / pull moment
      - protect CC target from cleave / DoT spread
      - re-apply CC if it breaks (timer-based; some encounters break CC on damage)
    cc_class_tools:
      mage: Polymorph (humanoid + beast + non-elemental)
      hunter: Freezing Trap (humanoid + beast)
      shaman: Hex (humanoid + beast — Cata expansion)
      paladin: Repentance (humanoid + dragonkin + giant)
      warlock: Banish (demon + elemental ONLY); Fear (humanoid only)
      priest: Shackle Undead (undead only); Mind Control (specific use)
      druid: Hibernate (beast + dragonkin only)
      rogue: Sap (humanoid only, before combat)
```

---

## Interrupt rotation

```yaml
interrupt_rotation:
  # Ranged DPS is the BACKUP interrupt rotation behind melee.
  # Some encounters (Brain Spike on ToTT Ozumat) put ranged as PRIMARY because the
  # cast originates from a ranged-only target.

  pre_pull_setup:
    - identify all interruptible casts in the encounter (encounter.md mechanics)
    - assign rotation order across all available interrupt-capable allies
    - if multiple interrupters available, alternate to spread CD usage
    - flag MUST_INTERRUPT casts (ones that are lethal-to-tank or party-wide damage)

  during_combat:
    rule: |
      If `mob.casting(MUST_INTERRUPT_spell) AND interrupt.off_cooldown`:
        cancel current cast (if non-emergency) and fire interrupt.
      Else if `mob.casting(SECONDARY_INTERRUPT_spell) AND interrupt.off_cooldown AND no_emergency`:
        fire interrupt opportunistically.

  cd_pacing:
    rule: |
      Avoid double-interrupting the same cast — coordinate rotation. If two
      interrupts fire on the same cast, the second is wasted (the cast is
      already locked).
    bot_implementation: |
      Track recent_interrupts list with timestamp; before firing, check if
      another party member's interrupt is currently in flight.

interrupt_class_tools:
  mage: Counterspell (24s CD baseline; 4.3.4)
  hunter: Silencing Shot (24s CD; ranged interrupt)
  warlock: Spell Lock (Felhunter pet; 24s)
  shadow_priest: Silence (45s; party-wide)
  balance_druid: Solar Beam (60s — large CD; treat as major)
  elemental_shaman: Wind Shear (6s baseline)

interrupt_priority:
  # Generic priority; encounter docs may override
  - tier: 1: lethal-to-tank single-cast (e.g., Shock Blast)
  - tier: 2: party-wide damage cast (e.g., Brain Spike)
  - tier: 3: dispellable-or-interruptible offensive buff (e.g., Absorb Magic — dispel preferred but interrupt the cast if no purger)
  - tier: 4: secondary damage casts (e.g., Lava Bolt)
  - tier: 5: opportunistic — any interruptible cast if rotation has bandwidth
```

---

## AoE thresholds

```yaml
aoe_thresholds:
  # When to use AoE abilities vs single-target.

  default_threshold:
    aoe_cluster_count: 3      # 3+ enemies within AoE radius
    aoe_cluster_radius: 10    # typical AoE spell radius
  rationale: |
    Most AoE abilities have higher mana cost per cast and lower damage per
    target than single-target spells. Break-even is typically 3 targets.

  spec_overrides:
    arcane_mage: AoE break-even at 4-5 (Arcane Explosion is poor); use Blizzard for 3+ at range
    fire_mage: Living Bomb spread at 2+ (DoT-spread shape)
    affliction_warlock: DoT-spread at 2+ (Bane of Doom + Corruption)
    destruction_warlock: Rain of Fire at 4+
    balance_druid: Hurricane at 3+ but mana-heavy; Starfall as a CD on cluster
    elemental_shaman: Chain Lightning at 3+; Earthquake at 5+ (mana cost)

  encounter_overrides:
    # Some encounters force specific AoE choices
    "single-target focus through adds":
      example: ToTT Ozumat — burn Mindlashers, NOT AoE on murloc clouds
      rationale: priority-target damage > AoE damage on swarms

    "AoE-with-CC-protection":
      example: ToTT Naz'jar Waterspout — AoE adds but protect CC'd Tempest Witch
      rationale: cleave/AoE breaks CC on the marked target; use targeted abilities only

dot_management:
  # For DoT classes (Affliction, Balance, Shadow, Destro, etc.)
  rules:
    - apply DoTs to highest-HP-remaining targets first (DoT scales with target lifespan)
    - do not refresh DoTs on targets with <30% HP remaining if direct damage will kill them faster
    - on target switch, do NOT clip DoTs (let them tick out before re-applying — pandemic mechanics)
  spec_specific: per-spec doc declares pandemic / refresh window rules
```

---

## Mobility

```yaml
mobility:
  # Movement during cast / movement events.

  hard_cast_problem:
    issue: most ranged damage spells require a cast time; movement cancels casts
    solutions:
      - instant_spells (Flame Shock, Living Bomb, Insect Swarm, Mind Flay channels with movement glyphs, etc.)
      - movement_cooldowns (Blink for mage, Disengage for hunter, Burning Rush for warlock, Stampeding Roar for druid)
      - cast-while-moving cooldowns (Spiritwalker's Grace for shaman, Ice Floes for mage in 4.3.4)

  mobility_cd_pacing:
    principle: |
      Mobility CDs are precious — fight has 2-4 forced-movement events. Use
      mobility CDs to maintain cast uptime during these events, NOT to "move
      faster" during free-positioning windows.

  movement_event_response:
    detection: encounter.md flags movement-required mechanics (Geyser dodge, Fissure spawn, Blight pool)
    bot_action:
      step_1: identify next movement window in active encounter profile
      step_2: pre-pop mobility CD if cast is in flight and would clip
      step_3: prefer instant spells during movement; queue hard-casts after movement window

cast_time_optimization:
  # Spec-specific cast time reductions
  haste_breakpoints: per-spec; flagged in spec doc (e.g., Arcane Mage haste breakpoints for cast-time multiples of GCD)
  proc_consumption_priority: |
    When a proc grants instant-cast or cast-time-reduction (Tidal Waves equivalents
    for damage classes), use it during MOVEMENT windows preferentially.
```

---

## Spread / cluster discipline

```yaml
spread_cluster:
  spread_default: maintain >=8y from other allies when no cluster mechanic forces grouping

  splash_mechanics_to_avoid:
    # Shape of mechanic that requires spread
    - radius_aoe_on_random_player (e.g., Lightning Surge, Fungal Spores)
    - chain_jump (e.g., Chain Lightning from a hostile caster)
    - meteor_dropoff (raid-specific)

  cluster_required_mechanics:
    # When the bot SHOULD stack with allies
    - smart_AoE_heal (proximity to allies for Chain Heal jumps, etc.)
    - signature_emergency_CD (Spirit Link Totem requires 3+ allies in 10y)
    - boss_phase_3_stack_pattern (e.g., ToTT Ozumat P3 stack on boss)

  rule: |
    Each encounter has a default positioning regime (spread / cluster / mixed).
    Per-fight role docs declare the regime; ranged DPS adapts.
```

---

## Anti-patterns (cross-content)

```yaml
anti_patterns:
  - id: cleave_break_cc
    rule: do NOT cleave / DoT-spread / AoE onto a CC'd target
    rationale: breaking CC frees an unmarked add to cast / melee; party survival drops
    bot_implementation: |
      Maintain "cc_protected" flag on target IDs. Cleave/AoE/DoT-spread targeting
      logic excludes flagged targets.

  - id: cast_through_purgeable_shield
    rule: do NOT cast offensive spells while target has an active offensive shield (Absorb Magic, Spell Reflection)
    rationale: shield absorbs your damage AND heals the boss; every cast is a feed
    bot_implementation: |
      Gate offensive casts on `not aura.active(target, OFFENSIVE_ABSORB)`. If aura
      is active, prefer purge / spellsteal first; otherwise idle.

  - id: tunnel_during_movement_event
    rule: |
      Do NOT continue hard-casting through a movement-required mechanic. Bot
      death from a missed dodge > the value of one cast.
    bot_implementation: |
      Movement-event detection in encounter.md must trigger pre-emptive cast
      cancellation. Switch to instant spells during the event window.

  - id: ignore_cc_assignment
    rule: |
      If the bot is the assigned CC class, applying CC at pull is non-negotiable.
      Failing to CC = the marked add casts freely on the party.
    bot_implementation: |
      cc_assignment is a per-encounter override declared in the dungeon role doc.
      Bot pulls the CC spell into its rotation as tier-1 priority during pull moment.

  - id: aoe_below_threshold
    rule: do NOT AoE for the sake of AoE — single-target deals more damage at <3 cluster
    rationale: mana waste, lower DPS, no upside
    bot_implementation: |
      AoE spells gated on `enemies_in_radius >= aoe_cluster_count` (default 3,
      spec-overridable).

  - id: target_switch_with_active_dots
    rule: |
      DoT classes — do not switch targets immediately if the previous target had
      DoTs ticking. Let DoTs tick out if the previous target will die soon AND
      the new target is not lethal-priority.
    bot_implementation: |
      DoT-class spec doc declares clip_threshold rules (typical: don't clip if
      DoT.remaining > 4 sec AND target.hp_pct > 20).

  - id: chase_into_threat_zone
    rule: do NOT move into untanked aggro range while repositioning for cast uptime
    rationale: pulling threat = death of the ranged DPS
    bot_implementation: |
      Position-decision logic must respect threat-zone polygons. Do not enter
      areas with untanked enemy mobs even if it improves cast positioning.
```

---

## Class-spec mapping table

Quick reference: how each Cata 4.3.4 ranged DPS spec realizes the role-level patterns. Per-spec docs declare these bindings.

| Pattern | Frost Mage | Fire Mage | Arcane Mage | Affliction Lock | Destro Lock | Demo Lock | Marks Hunter | BM Hunter | Survival Hunter | Shadow Priest | Balance Druid | Elemental Shaman |
|---|---|---|---|---|---|---|---|---|---|---|---|---|
| Filler nuke | Frostbolt | Fireball / Pyroblast | Arcane Blast / Missiles | Shadow Bolt | Incinerate / Shadow Bolt | Shadow Bolt / Incinerate | Steady Shot | Steady Shot | Cobra Shot | Mind Flay | Wrath / Starfire | Lightning Bolt |
| Burst CD | Icy Veins + Trinket | Combustion + Trinket | Arcane Power + Trinket | Trinket + Bane of Havoc | Bane of Doom + Immolate optimization | Metamorphosis | Rapid Fire + Trinket | Rapid Fire + Bestial Wrath | Rapid Fire + Trinket | Shadowfiend | Starfall + Incarnation? (not Cata) | Elemental Mastery |
| Interrupt | Counterspell | Counterspell | Counterspell | Spell Lock (pet) | Spell Lock (pet) | Spell Lock (pet) | Silencing Shot | Silencing Shot | Silencing Shot | Silence | Solar Beam (60s — major) | Wind Shear |
| AoE | Blizzard / Frozen Orb | Flamestrike / Living Bomb spread | Arcane Explosion (close) | DoT spread (Bane + Corruption + UA) | Rain of Fire | Hellfire / Immolation Aura | Multi-Shot | Multi-Shot | Multi-Shot + Black Arrow | Mind Sear (close) | Hurricane / Starfall | Chain Lightning / Earthquake |
| Mobility | Blink + Ice Floes (talent) | Blink + Ice Floes | Blink + Ice Floes | Burning Rush (Cata?) | Burning Rush | Demonic Circle: Teleport | Disengage + Aspect of the Fox | Disengage + Aspect of the Fox | Disengage + Aspect of the Fox | Dispersion + Fade | Dash (cat form?) | Spiritwalker's Grace |
| CC | Polymorph | Polymorph | Polymorph | Banish (demon/elem) / Fear | Banish / Fear | Banish / Fear | Freezing Trap | Freezing Trap | Freezing Trap | Mind Control / Shackle Undead | Hibernate (beast/dragonkin) | Hex |
| Offensive purge | Spellsteal | Spellsteal | Spellsteal | Devour Magic (Felhunter) | Devour Magic (Felhunter) | Devour Magic (Felhunter) | Tranq Shot | Tranq Shot | Tranq Shot | Dispel Magic (offensive variant) | n/a | Purge (NOT Resto-spec) |
| Self-heal | Mage Ward + Mana Shield | Mage Ward | Mage Ward | Drain Life + Death Coil | Drain Life | Drain Life + Soul Link (pet) | Mend Pet (pet only); not self | Mend Pet (pet only) | Mend Pet | Vampiric Embrace + Vampiric Touch healing | Rejuvenation (resto-spec'd-with-talents) | Healing Surge (cross-spec) |

Per-class spec docs flesh out specific spell IDs, cooldowns, and exact cast times. This table is for quick role-binding lookup.

---

## How dungeon-level docs reference this doc

Per-dungeon ranged-DPS docs (`docs/dungeons/{dungeon}/ranged-dps.md`) should:

1. Reference this doc by file path in the preamble
2. Only document **deviations** from the generalist patterns (encounter-specific target priority, interrupt assignment, CC assignment)
3. Not duplicate range management, AoE thresholds, mobility patterns, or anti-patterns documented here

---

## Open hooks for `AltbotCombat.cpp`

The bot's ranged DPS evaluation order each tick:

```
1. resolve target priority (encounter override → defaults)
2. check interrupt-priority queue (pre-empt rotation if MUST_INTERRUPT in flight)
3. check AoE threshold (if cluster >= threshold, switch to AoE rotation)
4. check movement event (if movement required and currently casting hard-cast, cancel and switch to instant)
5. evaluate spec rotation tiers
6. cast on resolved target
```

Class-specific bindings come from `docs/specs/{active_spec}.md`.
Encounter-specific overrides come from `docs/dungeons/{active_dungeon}/ranged-dps.md`.

# Lost City of the Tol'vir — Encounter Facts

**Role-neutral mechanic data** for the bot's encounter handling. Per-role response logic lives in sibling files (`healer.md`, `ranged-dps.md`, `melee-dps.md`, `tank.md`). When a role doc references a mechanic, it cites the spell ID defined here.

Spell IDs verified at `wowhead.com/cata/spell=<id>` where consensus could be reached; UNVERIFIED items are flagged inline and aggregated in `docs/research/dbc-verification-checklist.md`. Strategy reconciled across 5+ sources — see `docs/research/lost-city-of-the-tolvir-guide-survey.md` for the audit trail.

---

## Overview

```yaml
dungeon: lost_city_of_the_tolvir
zone: lost_city_of_the_tolvir
continent: kalimdor_uldum
difficulty_modes: [normal, heroic]
level_range_normal: [83, 85]
level_range_heroic: 85
boss_count: 4
optional_bosses: []                     # none — Lockmaw and Barim both mandatory before Siamat
bosses_in_order:
  - general_husam                        # mandatory first
  - lockmaw                              # either order
  - high_prophet_barim                   # either order
  - siamat                               # final, gated by both Lockmaw and Barim deaths
notes: |
  Outdoor instance in southern Uldum. Path: outer courtyard (Husam) → second
  tier with two side rooms (Lockmaw and Barim, either order) → slipstreams
  (wind-vortex teleporters) up to high terrace platform (Siamat).

  No instance-wide buff/debuff aura. The defining instance-level mechanic is
  HEAVY POISON + DISEASE DISPEL PRESSURE — Lockmaw's Viscous Poison (Poison),
  Augh's Paralytic Blow Dart (Poison), and Barim's Plague of Ages (Disease).
  Lockmaw is the canonical MANA TEST of this dungeon for the bot healer
  (analogous to Karsh in Blackrock Caverns). Save Mana Tide / Innervate /
  Hymn of Hope / equivalent for Lockmaw.

  Bot-implementation hazards (flagged below):
  1. Barim Phase 2 may involve a "spirit realm" phase split (separate phased
     instance). Default assumption is same-room; if private-server scripts
     teleport players, bot's threat/target/position tracking must handle it.
  2. Augh's appearance trigger varies across sources (Lockmaw death vs
     Lockmaw 30% HP vs random jump-in). Bot detects via target.is_attackable.
  3. Siamat platform knockoff during Wailing Winds is wipe-risky on most
     servers. Bot must stay center.
```

---

## Trash — notable mechanics

Only the trash entries with mechanics worth automating. Standard packs (basic Tol'vir grunts, low-elite mobs) are pure threat-and-DPS and not enumerated.

```yaml
trash:
  # Outer courtyard — pre-Husam
  - mob: Tol'vir Stoneshaper
    abilities:
      - name: Earth_Shock_or_Lightning_Bolt   # name varies by source — UNVERIFIED specific spell
        spell_id: UNVERIFIED
        interruptible: true
        priority: MUST_INTERRUPT
        notes: caster hardcast Nature-flavor; hits hard if uninterrupted
    cc_valid: [Hex, Polymorph, Sap]            # Hex preferred — works regardless of humanoid/elemental
    creature_type: UNVERIFIED                   # humanoid vs elemental disputed across sources
    notes: caster trash; primary kick target

  - mob: Neferset Plaguebringer
    abilities:
      - name: disease_cast                      # name varies — UNVERIFIED
        spell_id: UNVERIFIED
        interruptible: true
        priority: MUST_INTERRUPT
    cc_valid: [Polymorph, Hex, Sap]
    notes: humanoid caster trash

  - mob: Neferset Theurgist
    abilities:
      - name: magic_cast
        spell_id: UNVERIFIED
        interruptible: true
        priority: MUST_INTERRUPT
    cc_valid: [Polymorph, Hex, Sap]

  # Second tier — between Husam and Lockmaw / Barim
  - mob: Tol'vir Oathblade
    abilities:
      - name: melee_cleave
        target: frontal_cone
        notes: tank face away from group
    notes: melee trash; tank-and-spank with face management

  - mob: Oathsworn Axemaster
    abilities:
      - name: heavy_melee
        target: tank
    notes: high-HP melee; possibly cleaves (UNVERIFIED)

  # Pre-Siamat — high terrace approach
  - mob: wind_elementals_or_tolvir_guards         # UNVERIFIED specific NPC types
    abilities:
      - name: standard_melee_or_caster
        spell_id: UNVERIFIED
    notes: composition varies by source; mostly tank-and-spank
```

### Trash skip notes

- **No major optional branch.** Both Lockmaw and Barim are mandatory before Siamat unlocks (slipstreams gated by both kills).
- A few patrols can be wall-hugged with line-of-sight or pulled cleanly. Speed-clears LoS the outer-courtyard pat to skip a Stoneshaper pack.

### CC priorities

- **Hex (Shaman)** — safest CC (works on humanoid AND elemental); preferred on Stoneshapers given creature-type ambiguity
- **Polymorph (Mage)** — fallback on humanoid trash
- **Sap (Rogue)** — opener-CC on humanoid casters
- **Repentance (Holy Pally)** — opener-CC on humanoid casters
- **Banish (Mage / Warlock)** — for elemental trash if Stoneshapers turn out to be elemental

### Pull markers

- **3-pull caster groups** (Stoneshapers / Plaguebringers / Theurgists): skull on lead caster (kill priority), cross/X on second (CC)
- **Mixed melee+caster packs**: skull on caster (interrupt + kill), melee tanked

---

## Boss 1 — General Husam

```yaml
boss: general_husam
npc_id: 44577
location: outer courtyard, just past entrance
hp_pct_phase_triggers: []
notes: |
  Single-phase, mechanic-overlap fight. Trap placement + Shockwave + charge +
  Hammer Fist all on independent timers. KITE PATTERN: tank moves Husam in a
  slow circle so trap zones accumulate behind the group.

phases:
  - id: 1
    name: regular
    trigger: pull
    end_condition: hp_pct == 0
    description: |
      Continuous tank-kite with periodic trap, shockwave, charge, and
      Hammer Fist combo on tank. Heroic adds Detonate Traps premature
      mass-detonation event.

mechanics:
  - name: Mystic Trap
    spell_id: 83171
    cast_by: general_husam
    cast_kind: instant
    target: ground_random_3_players                # 3 trap placements per cast
    arming_delay_ms: ~2000                          # UNVERIFIED — sources vary
    detonation_radius: 7
    school: Fire
    damage: 34124 normal / 48749 heroic
    knockback: yes (force=100)
    interruptible: false
    dispel_type: none
    avoidable: true
    avoidance: do not stand in trap zone after arming; visible ground marker
    priority: AVOID

  - name: Detonate Traps                            # heroic only
    spell_id: 91263
    cast_by: general_husam
    cast_kind: scripted
    target: all_armed_traps
    countdown_ms: 5000
    school: Fire
    interruptible: false
    dispel_type: none
    avoidable: true
    avoidance: clear all trap zones during the 5s countdown
    heroic_only: true
    priority: GROUP_RELOCATE

  - name: Bad Intentions
    spell_id: 83113
    cast_by: general_husam
    cast_kind: instant
    target: ally_random_non_tank
    school: Fire
    effect: charge + slam at impact point; ~43,875-46,125 damage + stun
    interruptible: false
    dispel_type: none
    avoidable: false
    notes: unavoidable; healer reactive on charge target

  - name: Hammer Fist
    spell_id: 83655
    cast_by: general_husam
    cast_kind: melee_combo
    target: tank
    duration_ms: 2000                                # 4 swings, one per 0.5s
    swings: 4
    swing_damage_pct_normal_melee: 75
    school: Physical
    interruptible: false
    dispel_type: none
    avoidable: false
    notes: tank predictable spike; pre-shield window

  - name: Shockwave
    spell_id: 83445
    cast_by: general_husam
    cast_kind: cast
    target: pbaoe_self_4_lines
    cast_time_ms: 2000
    school: Physical
    effect: 4 ground lines erupt outward in X-pattern; ~43,875 damage within 4y of each line
    interruptible: false                              # not kick-cast; visible-warning ground avoid
    dispel_type: none
    avoidable: true
    avoidance: move off the visible X-pattern lines
    priority: AVOID

  - name: melee
    cast_by: general_husam
    cast_kind: melee
    target: tank
    school: Physical

# Adds
adds: []                                              # none

positioning:
  default: |
    KITE PATTERN — tank moves Husam in a slow CIRCLE around the chamber.
    Group follows the tank's path, staying ahead of trap zones. Range stays
    inside the kite path so they don't lag behind.
  tank_facing: away from group; rotate facing as boss kites
  los_or_lookaway: none
  fight_specific: |
    Active environmental hazard avoidance (traps, Shockwave lines).
    Constant movement required for ALL roles.

interrupt_priority: []                                # no kick-required casts

dispel_priority: []                                   # no dispels

heroic_delta: |
  - Detonate Traps premature mass-detonation event added (5s countdown)
  - More traps spawned per cast (~5 vs ~3)
  - Damage scales (Mystic Trap 34,124 → 48,749)
  - Tank kite-pattern is mandatory (vs optional on normal)

bot_role_summary:
  tank: kite Husam in slow circle; rotate facing away from group on each direction change; pop short defensive on Hammer Fist combo
  healer: reactive on Bad Intentions impact (predictable spike); pre-HoT before Hammer Fist (predictable cooldown); group AoE if Detonate Traps catches multiple
  melee: stay behind boss; follow kite path; move off Shockwave lines; do NOT path through arming traps
  ranged: max range; follow kite path inside; avoid Shockwave lines; 100% mobility
```

---

## Boss 2 — Lockmaw (and Augh)

```yaml
boss: lockmaw
npc_id: 43614
augh_npc_id: UNVERIFIED                              # listed as separate companion NPC
location: side chamber, second tier (group choice — Lockmaw or Barim first)
hp_pct_phase_triggers: [30, 0]                       # 30% Venomous Rage on Lockmaw; 0% transitions to Augh
notes: |
  Two-phase encounter. Phase 1: Lockmaw alive, Augh present but immune /
  intermittent. Phase 2: Lockmaw dies → Augh becomes attackable. Heroic Augh
  is a full standalone fight; normal-mode Augh is briefer.

  CANONICAL MANA TEST OF THIS DUNGEON for the bot healer. Viscous Poison
  (Poison-school dispel, 12s DoT) and Paralytic Blow Dart (Poison-school
  dispel, 9s DoT) are continuous dispel pressure; healer pace mana
  accordingly. Save Mana Tide / Innervate / Hymn of Hope / equivalent for
  THIS fight.

phases:
  - id: 1
    name: lockmaw_alive
    trigger: pull
    end_condition: lockmaw.hp_pct == 0
    description: |
      Tank Lockmaw with face management (front = melee, BACK = Dust Flail
      tail cone, side = safe). Dispel Viscous Poison on rotation. AoE
      Frenzied Crocolisk adds from Scent of Blood. At 30% Lockmaw enters
      Venomous Rage (+25% damage). Augh is unattackable during this phase.

  - id: 2
    name: augh_phase
    trigger: lockmaw.hp_pct == 0
    end_condition: augh.hp_pct == 0
    description: |
      Augh becomes attackable. Tank kites or holds Augh; group avoids
      Whirlwind. Dispel Paralytic Blow Dart. Smoke Bomb stuns tank
      occasionally — DPS may need taunt-relay through the 3s window.

mechanics_lockmaw:
  - name: Viscous Poison
    spell_id: 81630
    cast_by: lockmaw
    cast_kind: instant
    target: ground_random_player
    school: Nature
    radius: 5
    immediate_damage: 8949 normal / 13424 heroic
    dot_damage: 4161/2s normal, 9249/2s heroic
    duration_ms: 12000
    movement_slow_pct: 30
    interruptible: false
    dispel_type: poison                              # PRIMARY DISPEL TARGET
    avoidable: yes (move out of ground patch)
    priority: DISPEL
    notes: HEAVY HEROIC HEALER MANA SINK — primary dispel rotation focus

  - name: Dust Flail
    spell_id: 81644
    cast_by: lockmaw
    cast_kind: channel
    target: rear_cone                                # behind boss
    duration_ms: 5000
    damage_per_sec: 11423-13276 base
    damage_ramp_per_sec: +50%/sec standing in
    school: Nature
    debuff_on_hit: -50% accuracy 10s
    interruptible: false
    dispel_type: none
    avoidable: true
    avoidance: NEVER stand behind Lockmaw
    priority: AVOID
    notes: tail attack; frontal+rear hazard means group SIDE-positions

  - name: Scent of Blood
    spell_id: 81690
    cast_by: lockmaw
    cast_kind: instant
    target: ally_random
    school: Physical
    bleed_dot: 1850-2150 / 2s for 30s
    summons: 2-4 frenzied_crocolisks (UNVERIFIED count)
    interruptible: false
    dispel_type: none                                # bleed not Poison/Disease/Magic
    avoidable: false
    priority: ADD_HANDLE

  - name: Venomous Rage
    spell_id: 81706
    cast_by: lockmaw
    cast_kind: passive
    target: self
    trigger: hp_pct == 30
    school: Physical
    effect: +25% damage on Lockmaw
    interruptible: false
    dispel_type: UNVERIFIED                          # some sources say enrage-strippable; default treats as non-removable
    avoidable: false
    priority: DEFENSIVE_CD

  - name: melee
    cast_by: lockmaw
    cast_kind: melee
    target: tank
    school: Physical

mechanics_augh:
  - name: Paralytic Blow Dart
    spell_id: 84799
    cast_by: augh
    cast_kind: instant
    target: ally_random
    school: Nature
    immediate_damage: 7861 normal / 10220 heroic
    dot_damage: 4300/2s for 9s
    attack_speed_slow_pct: 40
    interruptible: false
    dispel_type: poison                              # DISPEL ASAP
    avoidable: false
    priority: DISPEL
    notes: high priority on tank target; 40% attack speed slow neuters tank

  - name: Whirlwind
    spell_id: 1680
    cast_by: augh
    cast_kind: channel
    target: pbaoe_self
    school: Physical
    damage: high physical to anyone in melee range
    interruptible: false
    dispel_type: none
    avoidable: true
    avoidance: melee back off; tank kites or holds depending on positioning
    priority: KITE_OR_AVOID

  - name: Smoke Bomb
    spell_id: 7964
    cast_by: augh
    cast_kind: instant
    target: pbaoe_self
    school: Physical
    radius: 5
    stun_duration_ms: 3000
    interruptible: false
    dispel_type: none                                # stun not standard-dispellable
    avoidable: partial
    avoidance: ranged out of 5y range; tank likely affected
    priority: TANK_STUN_RELAY
    notes: 3s tank stun — DPS may need to take aggro briefly

  - name: melee
    cast_by: augh
    cast_kind: melee
    target: tank
    school: Physical

# Adds (Frenzied Crocolisks from Scent of Blood)
adds:
  - name: Frenzied Crocolisk
    npc_id: 44892
    abilities:
      - name: Vicious Bite
        spell_id: 81677
        cast_kind: melee
        target: closest_or_bleed_target
        effect: stacking bleed up to 3 stacks
        school: Physical
    notes: |
      Spawns from Scent of Blood. Fixates on the bleed target. Tank picks up
      via taunt OR group AoE-burns immediately. Low HP.

positioning:
  default: |
    Lockmaw: tank pulls boss to wall; faces boss INTO wall (so tank is between
    boss and wall, no rear-cone hits group). Group SIDE-stands (not front,
    not behind).
    Augh: tank-kite around chamber; melee back off during Whirlwind.
  tank_facing: |
    Lockmaw: face into wall (rear-cone safe direction)
    Augh: standard tank facing, kite-aware
  los_or_lookaway: none

interrupt_priority: []                                # no kick-required casts

dispel_priority:
  - { spell_id: 81630, name: Viscous Poison, type: poison, priority: HIGH, notes: heroic mana sink }
  - { spell_id: 84799, name: Paralytic Blow Dart, type: poison, priority: CRITICAL_IF_ON_TANK }

heroic_delta: |
  - Augh is full second-phase fight (normal mode Augh is briefer)
  - Viscous Poison damage ~2x (4,161/tick → 9,249/tick)
  - Venomous Rage at 30% hits harder
  - More crocolisk adds from Scent of Blood
  - Bot healer mana pacing CRITICAL — primary dispel-mana-test of dungeon

bot_role_summary:
  tank: |
    Lockmaw: face boss INTO wall; tail = rear-cone hazard. Pick up crocolisk adds via taunt or AoE threat.
    Augh: kite during Whirlwind; brace for Smoke Bomb stun (3s); DPS taunt-relay if available.
  healer: |
    PRIMARY DISPEL ROLE — Viscous Poison + Paralytic Blow Dart on rotation.
    Pop Mana Tide / Innervate / Hymn ~50% Lockmaw HP. SAVE major mana CD for THIS fight.
  melee: |
    NEVER stand behind Lockmaw (Dust Flail). Side-position. Step out of Augh Whirlwind.
    AoE-burn crocolisk adds when they spawn.
  ranged: |
    Spread for Scent of Blood add target. Cast at side angle to avoid Dust Flail rear-cone path.
    Step out of Smoke Bomb 5y range pre-cast if telegraphed.
```

---

## Boss 3 — High Prophet Barim

```yaml
boss: high_prophet_barim
npc_id: 43612
location: side chamber, second tier (group choice — Lockmaw or Barim first)
hp_pct_phase_triggers: [50]
notes: |
  Two-phase cycling encounter. Phase 1 → 50% HP triggers Repentance →
  Phase 2 (Harbinger of Darkness add fight) → Harbinger dies → Phase 1
  resumes with Barim attackable again. Cycle until Barim 0%.

  BOT-IMPLEMENTATION HAZARD: some sources describe Phase 2 as a "spirit
  realm" / separate phased instance. Default assumption is SAME ROOM (Barim
  becomes immune in place; Harbinger spawns; group fights Harbinger while
  Barim stands inert). If private-server scripts implement a phase teleport,
  bot's threat / target / position tracking must handle the transition
  gracefully (record tank's previous target, re-acquire on Phase 1 resume).

phases:
  - id: 1
    name: phase_1_light_begone_infidels
    trigger: pull
    end_condition: barim.hp_pct == 50
    description: |
      Tank Barim. Plague of Ages disease bounces among players (spread to limit).
      Heaven's Fury creates ground holy zones (move out). Fifty Lashings
      self-buff on Barim (tank/healer cooldown). Heroic: Blaze of the Heavens
      phoenix add — kite or burn.

  - id: 2
    name: phase_2_dark_kneel_and_repent
    trigger: barim.hp_pct == 50
    entry_event: Repentance (81947) — 6s group AoE stun + Hallowed Ground zone
    end_condition: harbinger_of_darkness.hp_pct == 0
    description: |
      Barim becomes IMMUNE. Harbinger of Darkness add spawns (NPC 43927).
      Tank picks up Harbinger; group fights Harbinger while killing Soul
      Fragments (spawned by Harbinger's Soul Sever — must die / be slowed
      before reaching Harbinger else Merged Souls heals + buffs).
      Wail of Darkness ticks group damage throughout.

  - id: 3
    name: phase_1_resume
    trigger: harbinger.hp_pct == 0
    end_condition: barim.hp_pct == 0
    description: |
      Barim re-engageable. Phase 1 mechanics resume. Bot tank re-acquires
      Barim. May cycle to Phase 2 again if Barim's HP drops below another
      threshold (UNVERIFIED — most sources say single P2 cycle, but some say
      repeating).

mechanics_barim_p1:
  - name: Plague of Ages
    spell_id: 82622
    cast_by: barim
    cast_kind: instant
    target: ally_random
    range: 45
    school: Nature
    immediate_damage: 6799 base / 8839 heroic
    dot_damage: 3805/2s base / 4947/2s heroic
    duration_ms: 9000
    jumps: yes (to nearby allies on dispel/expire)
    interruptible: false
    dispel_type: disease                             # PRIMARY DISPEL TARGET
    avoidable: false
    priority: DISPEL
    notes: spread to limit jump targets; dispel ASAP on heroic

  - name: Heaven's Fury
    spell_id: 81942
    cast_by: barim
    cast_kind: instant
    target: ground_random_player
    school: Holy
    radius: 12
    damage_per_tick: 18037-20962 holy / 1.5s
    duration_ms: 9000
    interruptible: false
    dispel_type: none
    avoidable: true
    avoidance: move out of zone; visible ground marker
    priority: AVOID

  - name: Fifty Lashings
    spell_id: 82506
    cast_by: barim
    cast_kind: instant
    target: self
    school: Physical
    effect: next 10 melee swings on tank deal +5000 bonus Physical damage
    duration_ms: 20000
    interruptible: false
    dispel_type: UNVERIFIED                           # may be dispel-stealable as buff
    avoidable: false
    priority: TANK_DEFENSIVE_CD

  - name: Blaze of the Heavens                        # heroic Phase 1 only per most sources
    spell_id: UNVERIFIED                              # NPC ID unknown
    cast_by: barim_summon
    cast_kind: scripted
    school: Fire
    effect: phoenix add; pulses Fire AoE; can be kited or burned
    interruptible: false
    dispel_type: none
    avoidable: partial
    heroic_likely_only: true
    priority: ADD_KITE_OR_BURN

  - name: melee
    cast_by: barim
    cast_kind: melee
    target: tank
    school: Physical

mechanics_barim_p2_transition:
  - name: Repentance
    spell_id: 81947
    cast_by: barim
    cast_kind: scripted
    target: pbaoe_self
    school: Holy
    trigger: hp_pct == 50
    stun_duration_ms: 6000
    interruptible: false
    dispel_type: none                                # script stun, not standard-dispellable
    avoidable: false
    priority: PHASE_TRANSITION
    notes: spawns Hallowed Ground zone; bot must move out post-stun

  - name: Hallowed Ground
    spell_id: UNVERIFIED                              # part of Repentance scripted effect
    cast_by: barim
    cast_kind: scripted
    target: ground_around_barim
    school: Holy
    radius: 13
    damage: 9620-11180 / 5s
    duration_ms: until phase 2 ends OR until next cycle
    interruptible: false
    dispel_type: none
    avoidable: true
    avoidance: move out of zone after Repentance stun breaks
    priority: AVOID

mechanics_harbinger_p2:
  - name: Wail of Darkness
    spell_id: 82533
    cast_by: harbinger_of_darkness                    # NPC 43927
    cast_kind: periodic
    target: pbaoe_all_players
    school: Shadow
    damage_per_tick: ~7000
    interruptible: false
    dispel_type: none
    avoidable: false
    priority: HEALER_PRESSURE

  - name: Soul Sever
    spell_id: 82255
    cast_by: harbinger_of_darkness
    cast_kind: instant
    target: ally_random
    school: Physical
    periodic_ms: 4000
    effect: creates Soul Fragment add that moves toward Harbinger
    interruptible: false
    dispel_type: none
    avoidable: false
    priority: ADD_INTERCEPT

  - name: Merged Souls                                 # avoid this trigger
    spell_id: 82263
    cast_by: harbinger_of_darkness
    cast_kind: passive_on_soul_fragment_arrival
    target: self
    school: Shadow
    effect: heals Harbinger 41,625-48,375 + 20% damage buff
    interruptible: false
    dispel_type: none
    avoidable: yes — kill / slow / stun Soul Fragments before they reach
    priority: PREVENT_VIA_FRAGMENT_KILL

# Adds
adds:
  - name: Blaze of the Heavens                         # heroic P1
    npc_id: UNVERIFIED
    spawn_phase: 1 (heroic)
    notes: phoenix-style; ranged DPS slow-burn or kite

  - name: Harbinger of Darkness                        # P2 primary
    npc_id: 43927
    spawn_phase: 2
    notes: tank picks up; primary kill target in P2

  - name: Soul Fragment                                # P2 secondary
    npc_id: UNVERIFIED
    spawn_phase: 2
    spawn_trigger: harbinger_soul_sever_cast
    movement: toward harbinger
    notes: kill / slow / stun before reaching Harbinger

positioning:
  default: |
    P1: spread (Plague of Ages bounce limit, ~8y between players); stay in
        healer range; move off Heaven's Fury zones.
    P2: tank holds Harbinger AWAY from Barim (avoid Hallowed Ground zone);
        ranged kills Soul Fragments fastest; melee on Harbinger.
    P1 resume: re-engage Barim from spread.
  tank_facing: away from group (standard)
  los_or_lookaway: none
  fight_specific: |
    Phase transition — when Repentance casts (50% HP), all players stunned
    for 6s. NO OUTPUT during stun. As stun breaks, MOVE OUT OF Hallowed
    Ground zone and re-engage on Harbinger.

interrupt_priority: []                                # no kick-required

dispel_priority:
  - { spell_id: 82622, name: Plague of Ages, type: disease, priority: HIGH, notes: spread + dispel; jumps if not removed }

heroic_delta: |
  - Damage scales (Plague of Ages 3,805/tick → 4,947/tick)
  - Blaze of the Heavens phoenix add in Phase 1 (heroic)
  - Soul Fragments more numerous / faster on heroic (UNVERIFIED specifics)
  - Plague dispel pressure higher (faster jump cycles)

bot_role_summary:
  tank: |
    P1: face Barim away from group; pop short defensive on Fifty Lashings active.
    P2: re-acquire NEW target (Harbinger of Darkness); position Harbinger AWAY
        from Hallowed Ground / Barim's location.
    P1 resume: re-acquire Barim.
  healer: |
    Dispel Plague of Ages priority (Disease).
    Pre-shield before Repentance stun (predictable at 50% HP).
    Post-stun: AoE heal cooldown to recover from stun + Hallowed Ground splash.
    P2: continuous AoE heal for Wail of Darkness ticks.
  melee: |
    P1: spread (limit Plague jump radius).
    P2: attack Harbinger; intercept Soul Fragments with stuns/slows if no ranged caster handles them.
    P1 resume: re-engage Barim.
  ranged: |
    P1: handle Blaze of the Heavens (heroic only) — slow-burn or kite.
    P2: PRIMARY Soul Fragment kill role (ranged DPS reaches them faster than melee).
    Spread always.

bot_implementation_hazard: |
  Phase 2 may involve a "spirit realm" / separate phased instance per some
  sources. Default assumption is SAME ROOM (no teleport). Bot encounter
  handler should be defensive:
  - Track previous tank target (Barim) before P2 transition
  - Detect Harbinger spawn AND/OR Barim immune flag as P2 trigger
  - On Harbinger death, re-acquire previous target (Barim)
  - If position tracking goes haywire after Repentance, recovery action is
    "reset target to active hostile mob" rather than "go back to Barim
    coordinates"
```

---

## Boss 4 — Siamat

```yaml
boss: siamat
npc_id: 44819
location: high terrace, reached via slipstreams (wind-vortex teleporters) after both Lockmaw and Barim die
hp_pct_phase_triggers: []
notes: |
  Two-phase final encounter on a circular wind-platform with KNOCKOFF HAZARD.
  Phase 1: Siamat invulnerable (Deflecting Winds 90% DR) until 3 Servants die.
  Phase 2: Wailing Winds entry → Siamat attackable; Gathered Storms scaling
  damage on group; continuous Minion spawns.

phases:
  - id: 1
    name: phase_1_winds_of_the_south_rise
    trigger: pull
    end_condition: 3rd Servant of Siamat dies
    description: |
      Siamat invulnerable (Deflecting Winds 90% DR). Three Servants spawn
      sequentially (one every ~45s OR on previous Servant's death). Killing
      all 3 ends P1. Throughout: Minions of Siamat spawn every ~30s and
      self-deplete (Depletion -10% HP / 2s); they generate Tempest Storm
      clouds at low HP. Cloud Burst targets random players. Storm Bolt
      continuous low-grade pressure.

  - id: 2
    name: phase_2_cower_before_the_storm
    trigger: 3rd Servant dies
    entry_event: Wailing Winds (83066) — 6s knockback + damage
    end_condition: siamat.hp_pct == 0
    description: |
      Siamat attackable. Wailing Winds opens phase (knockoff hazard — stay
      center). Gathered Storms: Siamat absorbs nearby Tempest Storms,
      gaining stacking 25s "blast 2 players" effect (heroic: hits ALL
      players). Continued Minion spawns and Storm Bolts (P2 damage 2x).

mechanics_siamat:
  - name: Storm Bolt
    spell_id: 73564
    cast_by: siamat
    cast_kind: instant
    target: ally_random
    school: Nature
    damage_p1: 7068-7931
    damage_p2: 14137-15862
    interruptible: false
    dispel_type: none
    avoidable: false
    priority: HEALER_REACTIVE

  - name: Cloud Burst
    spell_id: 83051
    cast_by: siamat
    cast_kind: instant
    target: ground_random_player
    school: Nature
    radius: 5
    warning_ms: 3000
    damage: 21645-25155 + knock-up
    duration_ticks: 3 (1/sec)
    interruptible: false
    dispel_type: none
    avoidable: true
    avoidance: move out of cloud ring before burst
    priority: AVOID

  - name: Deflecting Winds                              # P1 boss DR
    spell_id: UNVERIFIED
    cast_by: siamat
    cast_kind: passive
    target: self
    effect: 90% damage reduction until 3rd Servant dies
    duration: until_phase_2
    interruptible: false
    dispel_type: none
    avoidable: false
    priority: DPS_GATE
    notes: do NOT waste DPS on Siamat in P1; kill Servants first

  - name: Wailing Winds
    spell_id: 83066
    cast_by: siamat
    cast_kind: channel
    target: pbaoe_all_players
    trigger: phase_2_entry
    duration_ms: 6000
    school: Nature
    damage_per_sec: 3607-4192
    knockback: violent (knockoff hazard on platform edge)
    interruptible: false
    dispel_type: none
    avoidable: partial
    avoidance: STAY CENTER OF PLATFORM (knockoff = wipe risk)
    priority: STAY_CENTER

  - name: Gathered Storms
    spell_id: 84987
    cast_by: siamat
    cast_kind: scripted
    target: self_then_blast_players
    school: Nature
    effect_normal: blasts 2 random players for 2405-2795 / sec for 25s
    effect_heroic: blasts ALL players for same damage / duration
    trigger: siamat absorbs a Tempest Storm
    duration_ms: 25000
    interruptible: false
    dispel_type: none
    avoidable: yes — kite Tempest Storms AWAY from Siamat
    priority: PREVENT_VIA_TEMPEST_STORM_AVOIDANCE

  - name: melee
    cast_by: siamat
    cast_kind: melee
    target: tank
    school: Physical

mechanics_servant:
  - name: Lightning Nova
    spell_id: 65279
    cast_by: servant_of_siamat                          # NPC 45269
    cast_kind: channel
    target: pbaoe_self
    school: Nature
    radius: 10
    damage: 13828-16071
    interruptible: false
    dispel_type: none
    avoidable: true
    avoidance: stay outside 10y of Servant unless tanking
    priority: AVOID

  - name: Thunder Crash
    spell_id: 84522
    cast_by: servant_of_siamat
    cast_kind: scripted (UNVERIFIED — live cast OR death effect; see survey Disagreement #4)
    target: current_target_or_pbaoe_on_death
    school: Nature
    damage: 30062-34937
    interruptible: false
    dispel_type: none
    avoidable: partial
    avoidance: tank pops defensive on telegraph; group spreads from low-HP Servants
    priority: TANK_DEFENSIVE_OR_GROUP_SPREAD

  - name: Lightning Charge                              # heroic only, group buff on Servant kill
    spell_id: UNVERIFIED
    cast_by: servant_of_siamat (on death threshold)
    cast_kind: scripted
    target: group
    effect: stacking +10% damage / +10% haste / +33% damage taken
    heroic_only: likely
    interruptible: false
    dispel_type: none
    avoidable: false
    priority: HEALER_PRESSURE_SCALE
    notes: achievement-relevant (3 stacks at end); bot does NOT optimize for achievement

mechanics_minion:
  - name: Chain Lightning
    spell_id: 103637
    cast_by: minion_of_siamat                           # NPC 44704
    cast_kind: hardcast
    target: ally_random_then_arc
    arc_range: 10
    arc_count: 4
    school: Nature
    damage: 10175-11825 initial; 70% reduction per arc
    interruptible: true                                 # primary kick target on Siamat
    dispel_type: none
    avoidable: partial (spread to limit arcs)
    priority: MUST_INTERRUPT

  - name: Tempest Storm
    spell_id: 83446
    cast_by: minion_of_siamat
    cast_kind: self_at_low_hp
    trigger: minion.hp_pct == 5
    target: ground_at_minion
    school: Nature
    radius: 6
    damage_per_sec: 10237-13162
    duration: until cleared / absorbed
    interruptible: false
    dispel_type: none
    avoidable: true
    avoidance: kite cloud away from Siamat (P2 Gathered Storms trigger)
    priority: AVOID_AND_DO_NOT_LET_REACH_SIAMAT

  - name: Depletion
    spell_id: 84550
    cast_by: minion_of_siamat
    cast_kind: passive
    target: self
    effect: -10% HP / 2s
    duration: until_dead
    interruptible: false
    dispel_type: none
    avoidable: false
    priority: NOOP_SELF_DEPLETION
    notes: Minions die on their own — DO NOT WASTE DPS ON THEM

# Adds (Servants — 3 in P1; Minions — continuous)
adds:
  - name: Servant of Siamat
    npc_id: 45269
    count: 3 in P1
    spawn_cycle: every 45s OR on previous death
    abilities:
      - name: Lightning Nova (see mechanic)
      - name: Thunder Crash (see mechanic)
      - name: Lightning Charge (heroic-only) (see mechanic)
    kill_priority: ABSOLUTE in P1
    notes: tank picks up; group stays >10y unless tanking

  - name: Minion of Siamat
    npc_id: 44704
    count: continuous (every ~30s in BOTH phases)
    abilities:
      - name: Chain Lightning (interruptible) (see mechanic)
      - name: Tempest Storm (low-HP) (see mechanic)
      - name: Depletion (self) (see mechanic)
    kill_priority: DO NOT KILL — they self-destruct
    notes: |
      Minions are an interrupt target (Chain Lightning) and a positioning
      hazard (Tempest Storm at 5% HP). DPS does NOT waste damage on them.

positioning:
  default: |
    P1: tank pins Siamat near platform CENTER. Servants brought to tank.
        Group spread (Chain Lightning arc avoidance), >10y from active
        Servant, do not waste DPS on Minions.
    P2: SAME as P1 spread + clear Tempest Storms from Siamat path. Stay
        CENTER for Wailing Winds (knockoff hazard).
  tank_facing: away from group (standard)
  los_or_lookaway: none
  fight_specific: |
    PLATFORM KNOCKOFF HAZARD — Wailing Winds at P2 entry can launch players
    off the platform. Stay center.

interrupt_priority:
  - Minion Chain Lightning (103637)                      # primary kick rotation target on Siamat

dispel_priority: []                                      # no dispels

heroic_delta: |
  - Lightning Charge stacks on group from Servant deaths (achievement-relevant)
  - Gathered Storms hits ALL players (vs 2 players normal) — significantly
    higher healer pressure in P2
  - Tighter mechanic windows; possibly faster Servant cycle
  - Minion Chain Lightning interrupt becomes mandatory (vs optional normal)

bot_role_summary:
  tank: |
    Pin Siamat center. Servants brought to tank when they spawn. DO NOT
    taunt Minions. Pop defensive on Thunder Crash telegraph (live-cast
    interpretation) AND step back on low-HP Servant (death-effect interpretation).
  healer: |
    P1: steady healing through Storm Bolt and Lightning Nova spillover.
    P2 (heroic): GATHERED STORMS spike — pre-cooldown.
    Wailing Winds entry: AoE heal cooldown ready.
  melee: |
    P1: kill Servants in order (skull marker). DO NOT touch Minions.
    P2: switch to Siamat. Continue ignoring Minions.
    Stay >10y from active Servant unless that's your kill target.
  ranged: |
    Same kill priority as melee. INTERRUPT MINION CHAIN LIGHTNING on rotation
    (primary kick role on Siamat). Spread for own arc avoidance.
```

---

## Instance-wide notes

```yaml
instance_wide:
  curse_dispel_required: false
  disease_dispel_required: true                          # Plague of Ages on Barim
  poison_dispel_required: true                           # Viscous Poison + Paralytic Blow Dart on Lockmaw
  magic_dispel_required: false
  dispel_blacklist: []                                   # no debuffs to BLACKLIST

  mana_test_fight: lockmaw                                # canonical OOM-test of dungeon
  mana_cd_save_priority:
    husam:    [no_save]
    lockmaw:  [Mana Tide / Innervate / Hymn / equivalent at 50% boss HP]
    barim:    [secondary CD if not used on Lockmaw]
    siamat:   [secondary CD; Gathered Storms heroic spike requires saved CD]

  group_buff_anomaly: none
  required_consumables: standard (flask + food + pots per spec)

  knockoff_hazard:
    boss: siamat
    mechanic: Wailing Winds (83066) at P2 entry
    consequence: platform knockoff = wipe risk
    bot_default: stay center of platform during Wailing Winds

  bot_implementation_hazards:
    - id: BARIM_PHASE_2_SPIRIT_REALM
      description: |
        Phase 2 may involve a "spirit realm" / separate phased instance per
        some community guides. Default assumption is SAME ROOM (Barim
        immune-in-place; Harbinger spawns; group fights add).
      mitigation: |
        Bot encounter handler tracks tank's previous target (Barim) before
        P2 transition. On Harbinger death, re-acquires previous target. If
        position tracking goes haywire (player reported coordinates jump),
        recovery action is "reset target to active hostile mob" rather than
        "return to Barim coordinates."
      severity: medium
      verified_against: 4.3.4 private-server scripts (UNVERIFIED — confirm at integration time)

    - id: AUGH_TRIGGER_VARIES
      description: |
        Sources disagree on when Augh becomes attackable: Lockmaw death vs
        Lockmaw 30% HP vs random jump-in.
      mitigation: |
        Bot detects via target.is_attackable(Augh) flag rather than HP-threshold.
      severity: low

    - id: SIAMAT_THUNDER_CRASH_LIVE_OR_DEATH
      description: |
        Sources disagree whether Servant Thunder Crash is a live cast (tank
        takes hit) or death AoE (group must spread from low-HP Servants).
      mitigation: |
        Bot does both: tank pops defensive on cast warning AND group steps
        back when Servant HP drops below ~10%.
      severity: low

  cycle_summary: |
    Husam → (Lockmaw OR Barim) → (Lockmaw OR Barim) → Siamat. Lockmaw and
    Barim are interchangeable in clear order; group leader's call. Siamat
    gated by both kills (slipstreams unlock).
```

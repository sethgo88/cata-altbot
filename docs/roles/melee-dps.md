# Melee DPS — Role Generalist

**Class-agnostic melee DPS patterns** that apply across all content (dungeon, raid, world). Per-spec details (which spell, which CD name) live in `docs/specs/{spec}.md`. Per-encounter overrides (target priority, interrupt assignment) live in `docs/dungeons/{dungeon}/melee-dps.md`.

This doc is the central reference for *how a melee DPS thinks* — positioning, gap-closer pacing, interrupt as primary duty, mobility-CD allocation, anti-patterns. The bot's melee DPS logic in `AltbotCombat.cpp` should be structured around the patterns documented here, with class-specific bindings injected from the spec doc.

---

## Positioning — behind-target discipline

```yaml
positioning:
  default: behind target (rear arc)
  rationale: |
    1. Many melee abilities REQUIRE rear positioning (Backstab, Ambush, Shred,
       Mangle's bonus damage from behind in some specs)
    2. Front-arc bosses can parry/block — rear positioning eliminates these dodges
    3. Some bosses have frontal cones (Honor Guard Arc Slash, Erunak Magma Splash,
       Behemoth Blight Spray) that one-shot non-tanks if they're standing in front

  rear_arc_definition:
    angle: 180° behind target (target's rear half-circle)
    distance: 0-5y from target (in melee range)
    note: |
      Some specs benefit from "directly behind" (within 90° rear cone) for
      ability-specific bonuses. Per-spec doc declares tighter requirements.

  side_positioning:
    when: facing matters but rear isn't reachable due to terrain or stack pattern
    fallback: side (lateral) is acceptable for ability requirement avoidance
    avoid: front

position_response_to_target_facing:
  # When the target rotates / shifts facing
  target_pivots:
    response: re-position to maintain rear arc within 1 GCD
    cost: |
      Movement consumes 0.5-1.5s (depends on distance + speed). Bot should
      use mobility CDs sparingly here; positional re-acquisition is cheap.

  tank_swaps_facing:
    cause: tank repositioning a boss for a cone or kite mechanic
    response: follow the tank's repositioning; rear-arc auto-updates
    note: |
      Tanks should communicate facing changes via group target marks or
      vocal calls. Bot logic relies on `boss.heading` updates each tick.

front_arc_violation_consequences:
  # What happens if bot is in front of target
  - parry/dodge increase: ~6.5% reduction in effective damage from boss melee
  - frontal cone exposure: lethal damage on cone-equipped bosses
  - some abilities just don't work (Backstab requires rear)

  bot_rule: |
    Front-arc violation is a hard fail state. Bot pathing MUST enforce
    rear-arc when current target is hostile melee-engaged. Override is
    acceptable only when:
    - target is dead / mid-death-pose
    - bot is mid-cast of a non-positional ability (rare for melee)
    - explicit anti-positional ability is queued (e.g., Charge — but Charge
      ends in front; bot must reposition immediately after)
```

---

## Gap-closer pacing

```yaml
gap_closers:
  # Class-specific gap-closer abilities (Charge, Heroic Leap, Shadowstep,
  # Death Grip-self, Feral Charge, Burning Rush for warlock — but warlock isn't melee).

  use_cases:
    - opener: close from spawn / pull range to melee
    - target_switch: reach a new priority target faster
    - movement_event: catch up to tank during kite phases
    - escape: reposition out of damage zone (less common for melee — defensive CDs preferred)

  pacing_principle:
    rule: |
      Don't burn a gap-closer at the opener if the encounter has a planned
      target switch or movement event. Save 1 charge / 1 use for the predictable
      use case.

  examples:
    rogue:
      shadowstep: 24s CD baseline; behind-target teleport
      sprint: 1 min CD; pure speed
    warrior:
      charge: 15s CD; opener / re-engage; +rage gen
      heroic_leap: 45s CD; AoE + leap
      intercept: 30s CD; alternate charge for warriors with both
    paladin (ret):
      blade_of_justice: 15s CD; gap-closer with damage
    death_knight:
      death_grip: 35s CD; pulls enemy to bot (!), or chains-self with talent
    druid (feral cat):
      feral_charge_cat: 30s CD; range + bleed application
    shaman (enhancement):
      none baseline; positioning-heavy class
    paladin (ret) / warrior:
      interception_chain: combine multiple gap-closers for sustained mobility (rare)

cd_allocation_per_fight:
  # General template for melee gap-closer pacing
  opener: 1 use (charge to boss, etc.)
  predictable_target_switch: 1 use
  movement_event: 1 use (per movement event up to CD limit)
  reactive_repositioning: 0 (use defensives instead)
  total_per_full_clear: ~5-15 depending on encounter density
```

---

## Interrupt as primary duty

```yaml
interrupt_primary_duty:
  # Melee is closest to the target and has shortest-CD interrupts on most classes.
  # Convention across all content: melee handles interrupt rotation; ranged backs up.

  pre_pull_setup:
    - identify all interruptible casts in encounter
    - determine which are MUST_INTERRUPT (lethal, party-wide)
    - assign primary interrupter (typically the melee with shortest CD)
    - set up rotation if multiple interrupters available

  during_combat:
    rule: |
      If the bot's interrupt is OFF cooldown AND a MUST_INTERRUPT cast is in
      flight, drop everything and fire interrupt. Lost damage from one cast
      << prevented damage from a missed interrupt.

  cd_pacing:
    rule: |
      Don't use interrupt on minor/secondary casts if a MUST_INTERRUPT cast
      is coming up within 2 GCDs. Save it.

interrupt_class_tools:
  rogue: Kick (10s CD baseline; 4.3.4 — shortest in game)
  warrior: Pummel (10s CD baseline; off-GCD)
  paladin (ret/prot): Rebuke (15s; replaces older Avenger's Shield interrupt for ret)
  death_knight: Mind Freeze (10s; off-GCD)
  druid (feral cat / guardian): Skull Bash (15s; gap-closer + interrupt combo)
  shaman (enhancement): Wind Shear (6s — shortest baseline; ranged 25y)

interrupt_priority:
  # Generic priority; encounter docs override
  - tier_1: lethal-to-tank (every cast must land an interrupt)
  - tier_2: party-wide damage cast
  - tier_3: dispellable-or-interruptible offensive buff
  - tier_4: secondary casts (opportunistic)
  - tier_5: minor casts (idle GCD only)
```

---

## Cleave vs single-target

```yaml
cleave_vs_single_target:
  # Most melee classes have abilities with secondary cleave damage
  # (Whirlwind for warriors, Swipe for druids, Death and Decay for DKs, etc.)

  general_threshold:
    cleave_break_even: ~2 targets (cleaves are usually free secondary damage)
    full_aoe_break_even: 3+ targets

  spec_overrides:
    fury_warrior: Whirlwind for 2+ (cheap GCD dump)
    arms_warrior: Sweeping Strikes for 2-3 (buff that lasts X seconds — turns Mortal Strike etc. into cleaves)
    enhancement_shaman: Magma Totem for 3+; Fire Nova procs from Flame Shock
    feral_druid: Swipe for 2+; full AoE switch only at 4+
    death_knight (frost/unholy): Death and Decay for 3+; Howling Blast (Frost) for 2+
    rogue (combat): Blade Flurry for 2+ (cleave 100% damage); Killing Spree
    rogue (assassination/sub): poor AoE; stay single-target unless 4+
    retribution_paladin: Divine Storm for 3+; Crusader Strike cleaves naturally

  cc_protection:
    rule: cleave/AoE excludes CC'd targets — same as ranged DPS
    bot_implementation: maintain cc_protected flag; cleave abilities skip flagged targets
```

---

## Mobility & movement events

```yaml
mobility:
  # Movement during combat — when the boss/tank repositions or the bot must dodge.

  default_movement_state:
    in_melee_range: stay glued to target via auto-attack range
    rotation_uptime: optimize for "GCD active 95%+ of fight"

  movement_event_response:
    detection: encounter.md flags movement-required mechanics
    types:
      - ground_telegraph_dodge (Geyser, Earth Shards, Dark Fissure)
      - tank_kite_follow (Ulthok perimeter, Ozumat P2 Beast kite)
      - phase_transition_relocation (Mindbender Ghur'sha P1 → P2)

    bot_action:
      step_1: identify movement window in active encounter profile
      step_2: pre-position (move BEFORE the telegraph fully forms)
      step_3: use gap-closer to re-engage post-movement
      step_4: do NOT use defensive CD for movement (save for damage spikes)

  uptime_during_kite:
    rule: |
      During tank kite (Ulthok), maintain melee range by moving WITH the tank,
      not following AFTER. Anticipate tank's next position based on fissure
      spawn pattern.
    cost: ~10-20% DPS loss during kite phases is acceptable

  movement_cooldown_pacing:
    rule: |
      Mobility CDs (Sprint, Burning Rush, Stampeding Roar) are precious.
      Save for catastrophic positioning events (kite-mid-cast, phase transition
      with relocation across map).

mobility_class_tools:
  rogue: Sprint (60s / 90s talented; +70% speed)
  warrior: Heroic Leap (45s; gap-closer + leap)
  paladin: Speed of Light (long CD; talented)
  death_knight: Death's Advance (Cata? — not till MoP; in 4.3.4 limited)
  druid (feral): Dash / Stampeding Roar (raid-wide speed buff)
  shaman (enhancement): Ghost Wolf (out-of-combat); Spiritwalker's Grace (cast while moving — limited use for Enh)
```

---

## Stun / interrupt-class-CC chains

```yaml
stun_chains:
  # Some encounters benefit from chained stuns to reset bleeds (Snap Dragons in ToTT)
  # or to lock down unkillable mobs.

  use_cases:
    - reset_stacking_debuff: Razor Jaws on tank (ToTT trash) — stun resets the stack
    - lockdown_uninterruptible: stun a caster that can't be interrupted normally
    - emergency_cc: peel off the healer

  class_tools:
    rogue: Kidney Shot (1 min CD; combo-point cost), Cheap Shot (opener stun)
    warrior: Shockwave (Cata — talent); Concussion Blow (talent)
    paladin: Hammer of Justice (1 min CD baseline; 6s stun)
    death_knight: Strangulate (silence not stun; 2 min CD)
    druid (feral): Bash (1 min); Maim (combo points; stun)
    shaman (enhancement): Stormstrike (debuff not stun); none direct

  pacing:
    rule: |
      Stuns are limited resources. Reserve for designated mechanics. Don't
      blow Kidney Shot on auto-attack damage when a Snap Dragon stack reset
      is the mechanical use case.
```

---

## Anti-patterns (cross-content)

```yaml
anti_patterns:
  - id: front_arc_attack
    rule: never attack a target from the front arc unless mid-Charge or no rear-position option exists
    rationale: parry/dodge loss + cone-cleave death risk
    bot_implementation: |
      Position-acquisition layer enforces rear-arc on hostile melee targets.
      Override only with explicit `position_override: any` flag.

  - id: cleave_break_cc
    rule: do NOT cleave / AoE onto a CC'd target (same as ranged DPS rule)
    rationale: breaking CC frees an unmarked add
    bot_implementation: maintain cc_protected flag; cleave abilities skip

  - id: gap_closer_burn_no_followup
    rule: do not Charge / Shadowstep / Death Grip without an immediate damage queue
    rationale: gap-closer CD wasted if the follow-up damage isn't queued
    bot_implementation: |
      Gap-closer ability gated on `next_attack_queued AND target.in_attack_range_after_close`.
      Don't fire just to "close range" — fire to "close range and immediately attack."

  - id: tunnel_during_movement_event
    rule: do NOT continue auto-attacking through a movement-required mechanic
    rationale: bot death from a missed dodge > one auto-attack of damage
    bot_implementation: |
      Movement-event detection in encounter.md must trigger pre-emptive
      auto-attack pause and movement queue.

  - id: skip_interrupt_for_ability_burst
    rule: |
      Do NOT delay an interrupt to fit in a damage cooldown rotation. The
      damage CD will come back; the missed interrupt's consequence (lethal
      tank damage) won't.
    bot_implementation: |
      Interrupt-priority check runs BEFORE rotation evaluation each tick.
      MUST_INTERRUPT casts pre-empt damage abilities.

  - id: ignore_dot_on_low_target
    rule: do not refresh DoTs on near-dead targets (rogue Rupture, feral Rip, etc.)
    rationale: DoT scales with target lifespan; near-death = wasted application
    bot_implementation: |
      DoT-class spec doc declares clip_threshold rules (typical: don't refresh
      if target.hp_pct < 20 OR target.estimated_time_to_death < dot.duration / 2).

  - id: defensive_cd_for_movement
    rule: do not use defensive CD to enable movement (use mobility CD instead)
    rationale: defensive CDs save you from damage; mobility CDs fix positioning
    bot_implementation: |
      Defensive CD use gated on `damage_taken_recent > threshold OR scripted_damage_event_imminent`,
      NOT on `bot_repositioning == true`.

  - id: rage_focus_energy_cap
    rule: do not let your resource cap (Rage 100, Energy 100, Focus 100, Holy Power 3, Combo Points 5)
    rationale: capped resource = wasted generation
    bot_implementation: |
      Per-spec rotation prioritizes resource dump abilities at near-cap. Threshold
      varies (typically 80%+ of cap triggers a dump).
```

---

## Class-spec mapping table

Quick reference: how each Cata 4.3.4 melee DPS spec realizes the role-level patterns. Per-spec docs declare these bindings.

| Pattern | Combat Rogue | Assassination Rogue | Sub Rogue | Arms Warrior | Fury Warrior | Ret Paladin | Frost DK | Unholy DK | Enhancement Shaman | Feral Cat Druid |
|---|---|---|---|---|---|---|---|---|---|---|
| Resource | Energy + Combo Pts | Energy + Combo Pts | Energy + Combo Pts | Rage | Rage | Holy Power + Mana | Runes + Runic Power | Runes + Runic Power | Mana | Energy + Combo Pts |
| Filler | Sinister Strike | Mutilate | Hemorrhage | Mortal Strike + Slam | Bloodthirst + Raging Blow + Heroic Strike | Crusader Strike + Templar's Verdict | Obliterate + Frost Strike | Scourge Strike + Death Coil | Stormstrike + Lava Lash | Mangle + Shred |
| Burst CD | Adrenaline Rush + Killing Spree | Vendetta + Vanish | Shadow Dance + Find Weakness | Recklessness + Berserker Stance | Recklessness + Death Wish | Avenging Wrath + Zealotry | Pillar of Frost + Empower Rune Weapon | Dark Transformation + Summon Gargoyle | Feral Spirit + Bloodlust | Berserk + Tigers Fury |
| Interrupt | Kick (10s) | Kick (10s) | Kick (10s) | Pummel (10s) | Pummel (10s) | Rebuke (15s) | Mind Freeze (10s) | Mind Freeze (10s) | Wind Shear (6s) | Skull Bash (15s; gap-closer) |
| Gap-closer | Shadowstep (talent) | Shadowstep (talent) | Shadowstep (baseline) | Charge (15s) + Heroic Leap (45s) | Charge + Heroic Leap | Blade of Justice? (Cata: not yet — MoP) | Death Grip (35s, pulls them to you) | Death Grip | none baseline | Feral Charge: Cat (30s) |
| Cleave | Blade Flurry (2 targets, 100%) | poor — single-target focus | poor — single-target focus | Sweeping Strikes (5s, MS becomes cleave) + Bladestorm (CD) | Whirlwind (every GCD) + Bladestorm | Divine Storm + Hammer of Wrath cleave | Howling Blast (cone) + Death and Decay | Death and Decay + Pestilence | Magma Totem + Fire Nova | Swipe (Cat) + Thrash |
| Stun | Kidney Shot (1 min) + Cheap Shot | Kidney Shot + Cheap Shot | Cheap Shot + Kidney Shot | Concussion Blow (talent) | Pummel? no — that's interrupt | Hammer of Justice (1 min) | Strangulate (silence) | Strangulate | none direct | Bash + Maim |
| Self-heal | Recuperate (combo points) | Recuperate | Recuperate | Enraged Regen (CD) | Enraged Regen | Word of Glory (Holy Power) + Lay on Hands | Death Strike (rune cycle) | Death Strike | Healing Surge | Predator's Swiftness instant Healing Touch |
| Defensive | Cloak of Shadows + Evasion + Feint | same | same | Shield Wall (only with shield) + Last Stand | Enraged Regen | Divine Protection + Ardent Defender (prot only — ret limited) | Anti-Magic Shell + Icebound Fortitude | same | Shamanistic Rage + Stoneform racial | Survival Instincts + Barkskin |

Per-class spec docs flesh out exact spell IDs, CDs, and resource costs.

---

## How dungeon-level docs reference this doc

Per-dungeon melee-DPS docs (`docs/dungeons/{dungeon}/melee-dps.md`) should:

1. Reference this doc by file path in the preamble
2. Only document **deviations** from the generalist patterns (encounter-specific target priority, interrupt assignment, mobility-CD allocation)
3. Not duplicate positioning rules, gap-closer pacing, or anti-patterns documented here

---

## Open hooks for `AltbotCombat.cpp`

The bot's melee DPS evaluation order each tick:

```
1. resolve target priority (encounter override → defaults)
2. enforce rear-arc positioning (acquire if not satisfied)
3. check interrupt-priority queue (pre-empt rotation if MUST_INTERRUPT in flight)
4. check movement event (if scripted dodge required, pre-position)
5. check cleave threshold (if 2+ targets and cleave available, prefer cleave abilities)
6. evaluate spec rotation tiers (resource generation / spend / proc consumption)
7. cast on resolved target
```

Class-specific bindings come from `docs/specs/{active_spec}.md`.
Encounter-specific overrides come from `docs/dungeons/{active_dungeon}/melee-dps.md`.

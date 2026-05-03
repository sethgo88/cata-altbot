# End Time — Tank

**Tank-specific response logic** for the bot's encounter handling. Mechanics, spell IDs, phase triggers, and add abilities are defined in `encounter.md`; this doc only encodes how the tank reacts. Class-agnostic where possible.

**Layering**:
- Cross-content tank patterns (threat priority, defensive CD pacing, taunt rules) live in `docs/roles/tank.md` (deferred to tank phase).
- Per-spec rotation / CD / threat model lives in the corresponding `docs/specs/` tank doc (all tank specs deferred to tank phase).
- This doc layers End-Time-specific deviations on top of those.

---

## Dungeon-level overview

```yaml
dungeon_tank_overview:
  total_bosses: 5 (always)                      # 2 random echoes + Murozond
  random_pool: [baine, jaina, sylvanas, tyrande]
  estimated_full_clear_minutes: 15-25
  difficulty_modes: [heroic_only]
  threat_profile:
    echo_of_baine:    moderate (platform hop)
    echo_of_jaina:    moderate-high (Pyroblast kicks reduce tank pressure; Blink chase)
    echo_of_sylvanas: low (standard tank-and-spank)
    echo_of_tyrande:  moderate (Moonbolt single-target)
    echo_of_murozond: high (Infinite Breath cone + Tail Sweep + boss migration)
  defensive_cd_profile:
    echo_of_baine:    light
    echo_of_jaina:    moderate (Pyroblast spike if kick missed)
    echo_of_sylvanas: light
    echo_of_tyrande:  moderate (Moonbolt + Lunar Guidance ramp)
    echo_of_murozond: heavy (Infinite Breath cycles + Hourglass-CD pacing)
  key_responsibilities:
    - random_pool_profile_load
    - tyrande_pre_fight_moonbeam_routing       # PRIMARY tank-routing override
    - baine_platform_hop_with_group
    - jaina_blink_chase
    - murozond_boss_migration_around_bombs
    - face_away_from_group_always (Murozond Infinite Breath, Jaina trash, etc.)
    - dispel_blacklist_management (Murozond Temporal Blast)
```

---

## Trash — tank notes

```yaml
trash_tank:
  baine_shrine:
    pull_pattern: standard tank-and-spank; pull lead caster (Time-Twisted Seer) first; melee mobs (Breaker / Drake) on sides
    facing: away from group (Breaker frontal cleave)

  jaina_shrine:
    pull_pattern: |
      Risen Priest = primary kill (heal-cast). Tank holds threat while DPS
      kicks Priest's heal. Risen Sorceress secondary.
    facing: away from group

  sylvanas_shrine:
    pull_pattern: standard
    facing: away from group

  tyrande_shrine_moonbeam:
    notes: |
      ★ UNIQUE TANK MECHANIC ★
      Trash mobs have 90% damage reduction OUTSIDE moonbeams. Tank must:
      1. Pull mob to current moonbeam location (W → S → E → N rotation, ~45s cycle)
      2. Hold mob in beam until shield strips (visible aura)
      3. DPS opens damage when mob is in beam
      4. As beam rotates, MIGRATE mob to next beam location

      bot_implementation: |
        FIGHT-SPECIFIC TANK ROUTING. Bot tank's path-planner must:
        - Track active moonbeam location (4 fixed points; cycle every ~45s)
        - Position mob INSIDE beam radius
        - When beam rotates, move mob to new beam BEFORE shield re-applies
        - Suppress aggressive damage attempts until beam-strip confirmed

      Healer + DPS hold damage; tank takes more melee while shield-up because
      mob isn't being damage-aggro'd elsewhere. Pre-shield / pre-HoT helpful.

  murozond_shrine:
    pull_pattern: standard caster + melee dragonkin
    facing: away from group
```

---

## Boss 1 (random) — Echo of Baine

```yaml
boss_tank:
  boss: echo_of_baine
  difficulty_grade: medium (platform hop + lava avoidance)
  threat_intensity: moderate

  positioning:
    pull: starting platform; boss centered
    facing: away from group
    movement: |
      KEY RULE — STAY OUT OF LAVA. Boss touching lava grants Molten Mace
      buff (boss-side fire damage). Tank must drag boss along platforms,
      never through lava.
      On Pulverize cast → boss leaps to ranged platform → ranged platform
      sinks → tank swims/jumps to that platform → resumes threat.

  threat_management:
    standard: maintain threat through Pulverize leap (boss may briefly
      target ranged target for AoE damage; threat resumes post-leap)
    pulverize_response: |
      When Pulverize cast → boss leaves tank position → tank does NOT chase
      until boss has landed AND DPS-on-tank is resumed. Then re-engage at
      new platform.

  defensive_cd_use:
    standard: reactive on melee burst
    burst_window_post_totem: |
      If ranged DPS successfully throws totem back → boss stunned + 50%
      damage-taken-up debuff for 20s → boss may die fast in this window;
      defensive CDs less critical. Pop offensive CDs (Death Wish-style)
      if tank spec allows.

  interrupt_role: []                            # no kick-required casts

  heroic_delta: HEROIC ONLY — no delta

  bot_anti_patterns:
    - DO NOT drag boss through lava (Molten Mace boss buff is bad)
    - DO NOT stand in lava waiting for boss to return
    - DO NOT get separated from group (platform-stranding); plan migration path
    - DO NOT chase boss to ranged platform via lava-swim (let boss come back)
```

---

## Boss 2 (random) — Echo of Jaina

```yaml
boss_tank:
  boss: echo_of_jaina
  difficulty_grade: hard (Blink chase + Pyroblast tank spike)
  threat_intensity: moderate-high

  positioning:
    pull: standard pull location after fragment-collection
    facing: away from group (Frost Blades cone, even though boss faces tank)
    movement: |
      Boss Blinks every ~10-15s → tank chases ASAP to maintain threat.
      Use mobility CDs:
      - Prot Warrior: Charge / Heroic Leap / Intervene
      - Prot Pally: Long Arm of the Law / Speed of Light? — UNVERIFIED Cata
      - Blood DK: Death's Advance (talent)
      - Feral Bear: Stampeding Roar / Dash; Skull Bash gap-close

  threat_management:
    pyroblast_handling: |
      Pyroblast is heavy single-target on tank. Pre-emptive Shield Block /
      Barkskin / Pain Suppression / Icebound Fortitude IF kick is uncertain.
      DPS kick rotation should handle Pyroblast — but tank's defensive
      backup is required when kicks miss.
    blink_threat_drop: |
      Blink event may temporarily drop boss threat (mob target reset).
      On re-engage, threat-lead spell rotation: taunt / threat-cast.

  defensive_cd_use:
    pyroblast_uninterrupted: pop short defensive (Shield Block / Barkskin / Bone Shield) if kick rotation missed
    blink_chase: minimal — re-engage and threat-build
    rotational_majors: rotate Shield Wall / Survival Instincts / Icebound / Lay on Hands every ~3-4 Pyroblast cycles

  interrupt_role:
    pyroblast: PARTICIPATE — tank's kick counts in rotation
    frostbolt_volley: PARTICIPATE if Pyroblast on cd

  heroic_delta: HEROIC ONLY — no delta

  bot_anti_patterns:
    - DO NOT stand still during Blink (lose threat lead)
    - DO NOT detonate Flarecore (already heavy single-target pressure from Pyroblast)
    - DO NOT face boss into group during Blink chase (Frost Blades cone)
    - DO NOT skip Pyroblast kicks (tank spike compounds)
```

---

## Boss 3 (random) — Echo of Sylvanas

```yaml
boss_tank:
  boss: echo_of_sylvanas
  difficulty_grade: easy
  threat_intensity: low

  positioning:
    pull: standard pull
    facing: away from group
    movement: |
      Standard tank-and-spank. On Calling of the Highborne event, group
      is pulled to boss → tank focuses ghoul-kill alongside DPS.

  threat_management:
    standard: maintain threat through Calling event (boss threat resumes
      post-event; ghouls don't follow threat — march scripted to Sylvanas)
    calling_event: |
      Tank's threat is irrelevant to ghouls during Calling. Tank's role
      switches to "DPS the focused ghoul + tank Sylvanas's auto-attack
      damage." Ghouls march to Sylvanas regardless of threat.

  defensive_cd_use:
    standard: reactive only
    calling_event: pop short defensive if multiple ghoul-melees overlap on tank position

  interrupt_role: []                            # no confirmed kicks

  heroic_delta: HEROIC ONLY — no delta

  bot_anti_patterns:
    - DO NOT taunt ghouls (they march scripted; taunt has no effect)
    - DO NOT chase Sylvanas during Calling (group is pulled to her anyway)
    - DO NOT stand in shadow zones (Wracking Pain) during Calling
    - DO NOT continue threat-build on Sylvanas during Calling — DPS the ghoul
```

---

## Boss 4 (random) — Echo of Tyrande

```yaml
boss_tank:
  boss: echo_of_tyrande
  difficulty_grade: moderate
  threat_intensity: moderate

  positioning:
    pull: in melee on boss (in 15y Dark Moonlight aura — caster penalty
      doesn't apply to tank's auto-attack threat)
    facing: away from group (Moonbolt single-target so technically no cone
      but discipline)
    movement: minimal — boss is mostly stationary

  threat_management:
    moonbolt_single_target_pressure: |
      Moonbolt is heavy single-target on tank. Standard tank-and-spank
      pressure. Defensive CD pacing on rotation.
    lunar_guidance_phases: |
      At hp_pct == 80 and 55, boss gains cast-haste stacks. Indirect
      tank pressure: more frequent Moonbolt + missed Stardust kicks → more
      group damage → tank may need to taunt-back if mob-target-shifts
      (rare but possible).
    tears_of_elune_phase (hp_pct == 30): |
      BURN PHASE — tank survives via Bloodlust-amplified DPS killing boss
      fast. Pop major defensive CDs in this window if HP drops.

  defensive_cd_use:
    rotational: Shield Block / Holy Shield / Bone Shield / Frenzied Regen
    moonbolt_spike: short defensive CD if multiple Moonbolts back-to-back
    tears_phase: major defensive (Shield Wall / Survival Instincts / Icebound) for survivability
    lay_on_hands / last_stand: emergency only

  interrupt_role:
    stardust: PARTICIPATE — tank's kick is part of the top-priority rotation
    moonbolt: try if Stardust on cd

  heroic_delta: HEROIC ONLY — no delta

  bot_anti_patterns:
    - DO NOT face boss into group (Moonbolt single-target but discipline)
    - DO NOT chase Eyes of the Goddess silence-orb (it circles room; let it pass)
    - DO NOT skip Stardust kick (top-priority interrupt)
    - DO NOT pop major CDs pre-30% (save for Tears burn phase)
    - DO NOT kite Tears (burn phase, hold position)
```

---

## Boss 5 — Echo of Murozond

```yaml
boss_tank:
  boss: echo_of_murozond
  difficulty_grade: hard (Infinite Breath cone + boss migration + Hourglass-aware CDs)
  threat_intensity: high

  positioning:
    pull: pull at fixed Bronze Dragonshrine arena center
    facing: ★ AWAY FROM GROUP — MANDATORY ★ (Infinite Breath frontal cone, ~75k fire)
    movement: |
      ★ DYNAMIC MIGRATION ★ — as Distortion Bomb zones land, tank must
      migrate boss to clean ground. Never park boss on top of a bomb zone.
      Hourglass click clears all bombs → migration resets to wider arena.

  threat_management:
    standard: maintain threat through periodic Temporal Blast cycles
      (PBAoE → tank also takes the damage but threat persists)
    target_swap_NA: no target-swap mechanic (Infinite Breath / Tail Sweep
      are positional, not target-swap)
    boss_migration: |
      Reactively detect Distortion Bomb landing zones → plan migration
      path to clean ground that maintains group line-of-sight + range.
      bot_implementation: |
        encounter_state_distortion_bombs[] → pathfinder.compute_clean_arc(boss_pos, bombs)
        tank.move_boss_to(clean_arc)

  defensive_cd_use:
    rotational: Shield Block / Holy Shield / Bone Shield / Frenzied Regen
    infinite_breath_window: |
      Infinite Breath every ~12-15s; tank takes ~75k fire if facing wrong
      OR splash if cone-clipped. Pop short defensive (Shield Block /
      Barkskin / Bone Shield) on each cast.
    pre_hourglass_imminent: |
      ★ HOURGLASS-AWARE CD PACING ★
      Same rule as DPS — suppress new MAJOR defensive CD usage in ~5s
      window before planned Hourglass click. Click resets HP/mana to 100%
      and resets cooldowns.
      Short rotational CDs (Shield Block / Bone Shield / Holy Shield) are
      cheap enough to use anyway; only suppress Shield Wall / Survival
      Instincts / Icebound / Last Stand / Lay on Hands tier.
    post_hourglass_click: |
      All major CDs reset → free re-use window. Stack defensive CDs
      generously to survive next ramp cycle.

  interrupt_role: []                            # no kick-required casts

  heroic_delta: HEROIC ONLY — no delta

  bot_anti_patterns:
    - DO NOT face boss INTO group (Infinite Breath cone wipe-grade group damage)
    - DO NOT park boss on Distortion Bomb zone (group can't approach)
    - DO NOT pop Shield Wall / Survival Instincts / Icebound just before planned Hourglass click (wasted CD)
    - DO NOT auto-dispel Temporal Blast (DISPEL_BLACKLIST entry)
    - DO NOT click Hourglass yourself (ranged DPS handles; tank's role is to keep boss positioned)
    - DO NOT migrate boss directly through ranged DPS / healer position (cone catches them)
```

---

## Bot AI hooks

```yaml
ai_hooks:
  random_pool_detection:
    - encounter_zone_entry → poll_time_transit_device_portals OR detect_first_engaged_npc_id
    - encounter_npc_id_match → load_per_boss_tank_profile

  baine:
    - encounter_state_active → keep_boss_on_platform_out_of_lava
    - encounter_detect_pulverize_cast → wait_landing → swim_to_new_platform → re_engage
    - encounter_detect_throw_totem_event → no_action (ranged DPS handles)
    - encounter_state_post_totem_debuff_active → continue_threat_no_panic

  jaina:
    - encounter_state_active → behind_boss_threat_build
    - encounter_detect_pyroblast_cast → kick_rotation_participate AND pre_emptive_short_defensive_if_kick_uncertain
    - encounter_detect_frostbolt_volley_cast → kick_rotation_secondary
    - encounter_detect_blink → mobility_cd → re_engage
    - encounter_detect_frost_blades_cast → reposition_face_away

  sylvanas:
    - encounter_state_active → standard_tank_and_spank
    - encounter_detect_calling_of_the_highborne_cast → focus_ghoul_kill (threat irrelevant)
    - encounter_detect_blighted_arrows_on_self → step_clear

  tyrande:
    - encounter_state_active → in_melee_face_away_from_group
    - encounter_detect_stardust_cast → kick_rotation_TOP_PRIORITY
    - encounter_detect_moonbolt_spike → reactive_short_defensive_cd
    - encounter_state_lunar_guidance_stack_2 → tighten_kick_rotation
    - encounter_state_tears_of_elune_phase → POP_MAJOR_DEFENSIVE_CD_FOR_BURN_PHASE_SURVIVAL

  murozond:
    - encounter_state_active → face_boss_AWAY_from_group + standard_threat_build
    - encounter_detect_distortion_bomb_landing → plan_boss_migration_to_clean_ground
    - encounter_detect_infinite_breath_cast → short_defensive_cd
    - encounter_state_pre_hourglass_imminent → suppress_MAJOR_defensive_cd_use (rotational still ok)
    - encounter_state_post_hourglass_click → free_major_cd_use_window

dispel_blacklist:
  - { spell_id: UNVERIFIED, name: Temporal Blast (Murozond), scope: echo_of_murozond_fight, force: true }

defensive_cd_priority_per_fight:
  baine:    [reactive_only; rotational]
  jaina:    [pre_emptive_short_on_pyroblast_uncertain; rotational majors]
  sylvanas: [reactive_only]
  tyrande:  [rotational shorts; major CDs saved for tears_burn_phase]
  murozond: [rotational shorts on infinite_breath; HOURGLASS-AWARE major CD pacing]

threat_anomalies:
  baine:    [pulverize_leap_temporary_target_shift_to_ranged]
  jaina:    [blink_target_reset_on_each_blink]
  sylvanas: [calling_event_ghoul_threat_independent]
  tyrande:  [none — standard threat]
  murozond: [none — boss stationary if migrated by tank]

dungeon_specific_overrides:
  tyrande_trash_moonbeam:
    rule: drag mob INTO active moonbeam to strip 90% damage-reduction shield
    cycle: West → South → East → North, ~45s rotation
    bot_implementation: track beam_active_location; pull/migrate mob to beam radius
```

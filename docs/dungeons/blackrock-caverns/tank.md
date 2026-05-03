# Blackrock Caverns — Tank

**Tank-specific response logic** for the bot's encounter handling. Mechanics, spell IDs, phase triggers, and add abilities are defined in `encounter.md`; this doc only encodes how the tank reacts. Class-agnostic where possible.

**Layering**:
- Cross-content tank patterns (threat priority, defensive CD pacing, taunt rules) live in `docs/roles/tank.md` (deferred to tank phase).
- Per-spec rotation / CD / threat model lives in the corresponding `docs/specs/` tank doc (all tank specs deferred to tank phase).
- This doc layers BRC-specific deviations on top of those.

---

## Dungeon-level overview

```yaml
dungeon_tank_overview:
  total_bosses: 4 (+ 1 optional)
  estimated_full_clear_minutes: 25-45
  threat_profile:
    rom_ogg: low                          # standard tank-and-spank with stack-up
    corla: low                            # tank just pins boss; drakonids tanked-by-beam-intercepter
    karsh: high                           # tank kite-through-plume requires sustained threat through environmental damage
    obsidius: moderate                    # target-swap on clone-rotation
  defensive_cd_profile:
    rom_ogg: light                        # only Wounding Strike DoT is tank pressure
    corla: light                          # standard melee
    karsh: heavy                          # plume passes are predictable defensive-CD windows
    obsidius: moderate                    # Twilight Corruption stack ramp
  key_responsibilities:
    - threat_priority_per_fight
    - target_swap_acquisition (Obsidius)
    - environmental_self_damage_management (Karsh plume)
    - add_pickup (Rom'ogg adds; Beauty pups)
    - face_away_from_group (boss melee cleave on Drakonid trash, Quicksilver Tank trash)
```

---

## Trash — tank notes

```yaml
trash_tank:
  twilight_humanoids:
    pull_pattern: |
      Twilight Elementalists, Torturers, Mages, Sadists are CC-able. Default
      pull pattern: skull on lead caster (kill priority), CC on second
      caster (cross/X markers).
    facing: away from group (frontal cleaves on Drakonids and Quicksilver Tank)
  quicksilver_tank:
    notes: large mob; frontal_cleave; tank-and-spank from front; melee positions side/rear
  twilight_drakonid_packs:
    notes: melee_cleave; standard tank-pull-and-rotate-kicks
  fear_packs:
    notes: |
      Twilight Sadists fear-cast; tank stays in center while DPS interrupts.
      If fear lands, healer can't respond — tank does NOT chase strays.
```

---

## Boss 1 — Rom'ogg Bonecrusher

```yaml
boss_tank:
  boss: rom_ogg_bonecrusher
  difficulty_grade: easy
  threat_intensity: low

  positioning:
    pull: position centrally so all post-Chains-of-Woe stacks are consistent
    facing: irrelevant (no frontal cone)
    movement: minimal; do NOT kite
    chains_of_woe_response: |
      Boss casts Chains of Woe (75539) → all players pulled to fixed point
      near boss. Tank does not need to react; positioning is automatic.

  threat_management:
    chains_of_woe: |
      Adds and ranged players will end up stacked at boss after Chains of
      Woe. Maintain threat lead before mechanic resolves.
    add_pickup: |
      At ~50% HP, 4 Angered Earth elementals spawn from corners. Bot tank:
      taunt nearest add OR drop AoE threat (Thunder Clap / Swipe / Death and
      Decay / Consecration / Avenger's Shield) to grab all 4.
    skullcracker: stack on boss; group damage-share is positioning-driven

  defensive_cd_use:
    quake_unintercepted: pop short defensive (Shield Block / Barkskin / Pain Suppression equivalent) if Quake is NOT interrupted in time
    skullcracker: standard tanking — no CD needed if group stack is full
    add_wave: AoE threat tools take damage via reactive cleave; defensive CD if cleaved

  interrupt_role:
    quake: PRIMARY interrupt rotation participant — tank's interrupt counts in the rotation
    rotation_partners: [tank, melee_dps, ranged_dps_with_kick]

  heroic_delta: |
    - Quake interrupt mandatory; missed kick = healer pressure spike
    - Add wave possibly larger or in additional waves; AoE-threat tools
      reactive on add-spawn event, not HP-threshold

  bot_anti_patterns:
    - DO NOT kite during Chains of Woe (mechanic-intended pull)
    - DO NOT chase Skullcracker — stay stacked
    - DO NOT solo-pull adds away from group (they reach group anyway via pull mechanics)
```

---

## Boss 2 — Corla, Herald of Twilight

```yaml
boss_tank:
  boss: corla_herald_of_twilight
  difficulty_grade: easy
  threat_intensity: low

  positioning:
    pull: tank pins Corla in CENTER of room; do NOT chase drakonids
    facing: away from group (standard); central positioning
    movement: stay-still — Corla pinned

  threat_management:
    drakonid_handling: |
      Tank does NOT pick up drakonids. Drakonids are stationary on platforms;
      DPS intercepting beams melees their assigned drakonid. If a DPS dies,
      tank may need to grab the orphan beam — bot takes the leftover beam
      slot if 4-player composition (3 DPS) is reduced.
    leftover_beam: tank-as-fallback-intercepter — if a DPS bot is dead, tank picks up that beam

  defensive_cd_use:
    standard: no special CD needs; Corla is soft tank-and-spank
    leftover_beam_intercept: pop major CD (Survival Instincts / Shield Wall / Barkskin) if forced to intercept beam — beam damage is healer-pressure-grade

  interrupt_role:
    none: no kick-required casts on this fight

  heroic_delta: |
    - Two-DPS-per-beam rotation may free tank from intercept entirely
    - Corla's auto-attack damage marginally higher

  bot_anti_patterns:
    - DO NOT chase drakonids — tank stays pinning Corla
    - DO NOT taunt drakonids off DPS intercepters (they need to be tanked-by-beam-intercepter)
    - DO NOT step into beams unless forced (leftover-beam fallback)
```

---

## Boss 3 — Karsh Steelbender

```yaml
boss_tank:
  boss: karsh_steelbender
  difficulty_grade: medium
  threat_intensity: high

  positioning:
    pull: pull boss adjacent to central plume
    facing: away from group; rotate boss face during plume passes
    movement: |
      KITE PATTERN — drag boss THROUGH the central plume on cadence:
      - Normal: every ~10-12s
      - Heroic: every ~8-10s
      Pull boss BACK OUT after each pass so DPS/healer don't take collateral fire.

  fight_specific_override:
    plume_self_damage: |
      DELIBERATE STAND-IN behavior. Tank bot logic must distinguish Karsh's
      central plume from generic "fire on floor = avoid":
      - Generic fire-on-floor: AVOID (default rule)
      - Karsh central plume: STAND IN BRIEFLY (encounter-specific override)
      Stand in for ~3-5s exposure to apply Quicksilver effect to boss; pull
      out before DoT ramps too high.

  threat_management:
    standard: maintain threat through kite-pass
    cinderbreath: face boss so cone clips no party member (face boss away from any direction with 2+ players)

  defensive_cd_use:
    plume_pass: pop short defensive (Shield Block / Barkskin) on each pass to mitigate Searing Lava DoT
    long_fight: rotate major CDs (Shield Wall / Survival Instincts / Icebound Fortitude) every ~3-5 plume passes for mana relief on healer
    cinderbreath_on_self: minor — fire breath on tank is normal damage

  interrupt_role:
    none: Cinderbreath generally non-interruptible; one source disagrees (UNVERIFIED)

  heroic_delta: |
    - Faster plume cadence (~8-10s)
    - Cinderbreath hits harder; tank facing more important
    - Healer mana check fight — tank uses defensive CDs on rotation

  bot_anti_patterns:
    - DO NOT park Karsh INSIDE the plume (DPS/healer collateral)
    - DO NOT skip plume passes (Quicksilver buff stacks make boss un-killable)
    - DO NOT face Cinderbreath into group
    - DO NOT pull boss out of plume immediately (need ~3-5s exposure to apply Quicksilver)
```

---

## Boss 4 (optional) — Beauty

```yaml
boss_tank:
  boss: beauty
  optional: true
  default_skip: true                      # bot skips unless instructed to clear
  difficulty_grade: medium
  threat_intensity: low

  positioning:
    pull: tank pulls Beauty to SIDE of chamber so charges don't path through group
    facing: away from group (standard)

  threat_management:
    pup_clears: pull pups individually before Beauty (one at a time)
    pup_facing: face away from group (frontal cones on at least one named pup)

  defensive_cd_use:
    fear_landed: pop trinket / racial / fear-counter
    charge_stun_landed: post-stun defensive if HP drops

  interrupt_role:
    terrifying_roar: ATTEMPT — UNVERIFIED interruptibility; rotation tries
    pup_magma_spit: PROBABLY interruptible — try

  heroic_delta: |
    - Pups hit harder; charge stun longer; fear harder to break
    - Notable trinket loot (UNVERIFIED) — encounter worth clearing for that

  bot_anti_patterns:
    - DO NOT pull all 3 pups simultaneously on heroic (mechanics compound)
    - DO NOT pull Beauty before pups dead (pups + boss together is healer-OOM territory)
```

---

## Boss 5 — Ascendant Lord Obsidius

```yaml
boss_tank:
  boss: ascendant_lord_obsidius
  difficulty_grade: medium
  threat_intensity: moderate

  positioning:
    pull: tank in CENTER of room
    facing: away from group (standard)
    movement: minimal; clones come to tank or are picked up on swap

  threat_management:
    target_swap_awareness: |
      CRITICAL behavior. When the clone-rotation event fires, the previously-
      tanked Obsidius unit becomes a clone and a different Shadow of
      Obsidius becomes the "active" boss. Tank bot must:
      1. Detect clone-rotation event (scripted; UNVERIFIED detection method)
      2. Re-acquire threat on the NEW active unit (taunt OR closest-to-tank
         pickup)
      3. Position self between new active unit and group
    twilight_corruption: |
      Stacks on tank (76188 UNVERIFIED). Healer pressure ramps. Defensive CDs
      on rotation. Stack count for swap (3 vs 5) is UNVERIFIED — bot uses
      clone-rotation event as primary swap trigger, stack count secondary.

  defensive_cd_use:
    twilight_corruption_high: pop major defensive (Shield Wall / Survival Instincts) at ~3-5 stacks
    clone_pickup: short defensive on clone-pickup transition

  interrupt_role:
    none: no confirmed kick-required casts

  heroic_delta: |
    - Faster clone-pull cycle; tank target-swap reactivity tighter
    - Higher melee damage on tank

  bot_anti_patterns:
    - DO NOT chase clones (they come to tank)
    - DO NOT swap target preemptively (wait for clone-rotation event)
    - DO NOT attack Shadow of Obsidius adds outside the active-clone (they regen and don't need to die)
```

---

## Bot AI hooks

```yaml
ai_hooks:
  rom_ogg:
    - encounter_detect_chains_of_woe → no_action_change (mechanic-intended)
    - encounter_detect_skullcracker_channel → ensure stack-on-boss positioning
    - encounter_detect_add_spawn → AoE_threat_tools (Thunder Clap / Swipe / DnD / Consecration)
    - encounter_detect_quake_cast → kick_rotation_participate

  corla:
    - encounter_detect_drakonid_alive → pin_boss_central
    - encounter_detect_dps_dead AND beam_unmanned → tank_picks_up_beam_with_major_CD

  karsh:
    - encounter_state_kite_cadence_timer → drag_boss_through_plume (every 10s normal / 8s heroic)
    - encounter_detect_self_in_plume_dot → pop_defensive_cd_short
    - encounter_detect_cinderbreath_cast → face_boss_away_from_group

  obsidius:
    - encounter_detect_clone_rotation_event → re_acquire_threat_on_new_active_unit
    - encounter_detect_twilight_corruption_high_stack → pop_major_defensive_cd
    - encounter_detect_crepuscular_veil_on_self → no_action (tank does not get Veil per strat)

dispel_blacklist:
  - { spell_id: 75476, name: Crepuscular Veil, reason: Obsidius swap mechanic }

defensive_cd_priority_per_fight:
  rom_ogg:    [reactive_only]
  corla:      [reactive_only_if_intercepting_beam]
  karsh:      [rotation: short_CD per plume pass; major_CD every ~3-5 passes]
  obsidius:   [major_CD at high Twilight Corruption stacks]
```

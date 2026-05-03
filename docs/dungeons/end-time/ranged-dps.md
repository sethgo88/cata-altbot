# End Time — Ranged DPS

**Ranged-DPS-specific response logic** for the bot's encounter handling. Mechanics, spell IDs, phase triggers, and add abilities are defined in `encounter.md`; this doc only encodes how ranged DPS reacts. Class-agnostic where possible.

**Layering**:
- Cross-content ranged DPS patterns (positioning, spread/stack rules, threat ceiling) live in `docs/roles/ranged-dps.md`
- Per-spec rotation lives in the active spec's `docs/specs/` doc (frost-mage, affliction-warlock, mm-hunter, shadow-priest, balance-druid, elemental-shaman)
- This doc layers End-Time-specific deviations on top of those

---

## Dungeon-level overview

```yaml
dungeon_ranged_dps_overview:
  total_bosses: 5 (always)                      # 2 random echoes + Murozond
  random_pool: [baine, jaina, sylvanas, tyrande]
  estimated_full_clear_minutes: 15-25
  difficulty_modes: [heroic_only]
  positioning_profile:
    echo_of_baine:    opposite_platform_spread       # Pulverize jumps to ranged
    echo_of_jaina:    spread_max_range               # Frostbolt Volley AoE; Frost Blades cone
    echo_of_sylvanas: spread_8_10y                   # Black Arrow + Unholy Shot
    echo_of_tyrande:  edge_of_dark_moonlight_aura    # OUTSIDE 15y aura — POSITIONAL OVERRIDE
    echo_of_murozond: spread_far_dynamic             # Distortion Bombs target ranged; spread + reposition
  interrupt_load:
    echo_of_baine:    zero
    echo_of_jaina:    HIGH (Pyroblast + Frostbolt Volley)
    echo_of_sylvanas: zero
    echo_of_tyrande:  HIGH-CRITICAL (Stardust top-priority)
    echo_of_murozond: zero
  key_responsibilities:
    - random_pool_profile_load
    - throw_totem_handler (baine)               # PRIMARY ranged duty
    - flarecore_detonator (jaina)               # designated walker (if highest HP)
    - stardust_kick_rotation (tyrande)          # TOP priority kick
    - dark_moonlight_positional_override (tyrande)  # outside 15y aura
    - hourglass_click_handler (murozond)        # PRIMARY ranged duty (Mage/Warlock preferred)
    - ghoul_focus_coordination (sylvanas)       # explicit target-shift on Calling
```

---

## Trash — ranged DPS notes

```yaml
trash_ranged:
  baine_shrine:
    interrupt_role: |
      Time-Twisted Seer is caster — kick mandatory:
      - Frost Mage: Counterspell (24s)
      - Affliction Warlock: Spell Lock (Felhunter)
      - MM Hunter: Silencing Shot (24s)
      - Shadow Priest: Silence (45s)
      - Balance Druid: Solar Beam
      - Ele Shaman: Wind Shear (6s; off-GCD)
    cc_priorities: |
      Polymorph / Hex on Time-Twisted Seer if pull is risky.

  jaina_shrine_caster_priests:
    interrupt_role: |
      Risen Priest heal-cast — KICK MANDATORY (heal resets pull HP).
      Risen Sorceress Frostbolt-cast — interruptible.
    cc_priorities: Polymorph valid on both. Sap (Rogue) opener-CC for opener-pull.
    fragment_collection: |
      Bot picks up nearby Jaina Fragments while moving through trash. Pickup
      is an instant interaction — does not interrupt cast cycle. Aim for 16
      total before engaging boss.

  sylvanas_shrine:
    interrupt_role: Risen Banshee caster — kick rotation
    cc_priorities: Polymorph valid

  tyrande_shrine_moonbeam:
    notes: |
      Pre-fight: tank drags trash into rotating moonbeams to strip 90% damage
      reduction. Ranged DPS HOLDS damage until mob is in beam. This is
      counter-intuitive — the bot's "DPS on engage" reflex is overridden by
      "DPS only when mob has no shield."
    bot_implementation: |
      detect mob.has_aura(SHADOW_SHIELD) → suppress damage until aura.missing()
      or mob.in_moonbeam() == true.

  murozond_shrine:
    interrupt_role: caster trash — standard kick rotation
    cc_priorities: standard
```

---

## Boss 1 (random) — Echo of Baine

```yaml
boss_ranged:
  boss: echo_of_baine
  difficulty_grade: medium (positional + interaction)
  positioning:
    default: |
      Stand on OPPOSITE platform from boss. Pulverize jumps to ranged player
      and sinks the platform — being on the opposite platform spreads
      Pulverize risk and gives you room to migrate.
      Spread among ranged (8y minimum) so Pulverize doesn't multi-impact.
    facing: boss
    movement: |
      On Pulverize cast, target platform sinks ~3-5s post-landing. Migrate
      to adjacent platform via swim/jump (incurs lava DoT).
  interrupt_role: []                            # no kicks

  rotation_modifications:
    standard: |
      Standard ST rotation; pause briefly during platform-migration swim
      windows.
    throw_totem_handler: |
      ★ PRIMARY RANGED DUTY ★
      On Throw Totem cast event (ID 101614 UNVERIFIED):
      1. Detect totem gameobject on ground (ranged player target location)
      2. Click-interact totem to pick up (1 GCD)
      3. Re-throw at boss (1 GCD; range from totem-pickup-location to boss)
      Effect: stuns boss + 5% max HP damage + 50% damage-taken-up debuff for 20s.
      ASSIGNED to closest ranged DPS bot (lowest interrupt-cost rotation).

      bot_implementation: |
        encounter_detect_throw_totem_cast(target=ranged_player) →
          IF self == target OR closest_ranged(self, totem_landing_location):
            queue_interaction(totem_gameobject)
            queue_interaction(totem_in_hand → boss)
          ELSE:
            continue rotation

  cd_use:
    standard_offensive: rotational; bloodlust pre-pull or saved per group call
    burst_window_post_totem: |
      After successful Throw-Totem-back-to-boss → 50% damage-taken-up on boss
      for 20s → POP offensive CDs in this window (Combustion / Demon Soul /
      Trinket actives / Beserking racial / etc.).

  heroic_delta: HEROIC ONLY — no delta

  bot_anti_patterns:
    - DO NOT enter lava (you are not the tank; lava-stand has no benefit for ranged)
    - DO NOT cluster with other ranged on same platform (Pulverize multi-impact)
    - DO NOT delay Throw-Totem-back interaction (20s debuff is significant DPS aid)
    - DO NOT cast Knockback abilities (Thunderstorm Shaman, Typhoon Druid) — could push tank or clones into lava
```

---

## Boss 2 (random) — Echo of Jaina

```yaml
boss_ranged:
  boss: echo_of_jaina
  difficulty_grade: hard (kick rotation + Blink chase + Flarecore decision)
  positioning:
    default: |
      Max range; spread for Frostbolt Volley AoE; max range from boss for
      Frost Blades safety (cone is forward).
    facing: boss
    movement: |
      Boss Blinks every ~10-15s. On Blink, move to maintain max range from
      new boss position. Cast-while-moving classes preferred (see role doc).
  interrupt_role:
    pyroblast: PRIMARY KICK — every cast (kick if class has it)
    frostbolt_volley: SECONDARY KICK — if Pyroblast on cd

  rotation_modifications:
    standard: |
      Standard ST rotation. Frequent Blink interruptions; pause hardcasts
      during boss reposition. Cast-while-moving rotation:
      - Frost Mage: Frostbolt hardcast; Ice Lance instant on Fingers proc
      - Affliction Warlock: DoTs are instant; Drain Soul channel-mobile
      - MM Hunter: Steady Shot hardcast; Aimed Shot proc
      - Shadow Priest: Mind Flay channels mobility-friendly; DoTs instant
      - Balance Druid: Wrath/Starfire hardcast; Moonfire/Sunfire instant
      - Ele Shaman: Lightning Bolt hardcast; Lava Burst hardcast; Flame Shock instant
    flarecore_detonator: |
      DESIGNATED ROLE — highest-HP ranged DPS bot walks ember to detonate
      within 5s. Self-test on flarecore spawn:
      IF self.hp_pct >= 80 AND self == highest_hp_ranged():
        suspend rotation; walk to ember; detonate; resume
      ELSE:
        continue rotation; let other DPS detonate

      bot_implementation: |
        encounter_detect_flarecore_spawn(location) →
          IF role_assignment == flarecore_detonator AND self.hp_pct >= 80:
            move_to(ember.location) + detonate
          ELSE:
            stay_clear

  cd_use:
    standard_offensive: rotational
    save_for_burn_phase: NA — Jaina has no burn phase; rotational use ok

  heroic_delta: HEROIC ONLY — no delta

  bot_anti_patterns:
    - DO NOT stand still during Blink (you'll be out of range)
    - DO NOT detonate Flarecore at low HP (you'll die — let other player or natural detonate)
    - DO NOT delay Pyroblast kick (tank takes ~50k + 12s DoT)
    - DO NOT stand in Frost Blades cone (5s stun = lost rotation time)
    - DO NOT cluster with ranged (Frostbolt Volley AoE multi-hit)
```

---

## Boss 3 (random) — Echo of Sylvanas

```yaml
boss_ranged:
  boss: echo_of_sylvanas
  difficulty_grade: medium (target-shift on Calling)
  positioning:
    default: |
      SPREAD 8-10y minimum (Black Arrow AoE, Unholy Shot multi-target,
      Blighted Arrows ground-rune target).
    facing: boss
    movement: |
      Move out of Blighted Arrows ground rune within 5s. Reposition after
      Calling event ends.
  interrupt_role: []                            # no confirmed kicks

  rotation_modifications:
    standard: |
      Standard ST rotation on Sylvanas. Spread positioning constant.
    on_calling_of_the_highborne: |
      ★ TARGET-SHIFT EVENT ★
      Group is pulled to boss; 8 Risen Ghouls spawn in circle.
      ALL DPS bots shift target to LOWEST-HP GHOUL (ties broken by closest
      to bot's position) — see survey Disagreement #5.
      ★ COORDINATION REQUIRED ★ — all DPS focus the SAME ghoul. Bot logic:
        target = ghouls.filter(alive).order_by(hp_asc, distance_asc).first()
      Continue DPS until ghoul dies → shadow link breaks → escape through gap.
      Re-target Sylvanas after gap-escape.

      bot_implementation: |
        encounter_detect_calling_of_the_highborne_cast →
          state.dps_target = SYLVANAS_CALLING_GHOUL
          state.escape_gap_pending = true
        on_ghoul_death →
          IF gap_open:
            move_through_gap_then_resume_sylvanas_dps
            state.dps_target = SYLVANAS

  cd_use:
    standard_offensive: rotational
    save_for_calling: optional — pop offensive CD on ghoul-focus to break circle faster

  heroic_delta: HEROIC ONLY — no delta

  bot_anti_patterns:
    - DO NOT continue DPS on Sylvanas during Calling — kill ghoul instead
    - DO NOT cluster with other ranged (Black Arrow + Unholy Shot AoE)
    - DO NOT cross ghoul links during Calling (Wracking Pain ticks; can be lethal)
    - DO NOT stand in Blighted Arrows rune > 5s
    - DO NOT spread SO far that you're out of healer range (8-10y spread, not 30y+)
```

---

## Boss 4 (random) — Echo of Tyrande

```yaml
boss_ranged:
  boss: echo_of_tyrande
  difficulty_grade: hard (kick + positional override + burn phase)
  positioning:
    default: |
      ★ POSITIONAL OVERRIDE ★ — stand JUST OUTSIDE the 15y Dark Moonlight
      aura around boss. NOT max range — at-aura-edge. The -50% cast speed
      penalty is brutal for casters; staying outside the aura is a first-
      class concern.
      Spread for Eyes of the Goddess silence-orb path; sidestep Moonlance.
    facing: boss
    movement: |
      Tears of Elune at 30% HP — small puddles on feet. Reposition every
      few seconds; do NOT kite (burn phase, kill the boss).
  interrupt_role:
    stardust: ★ TOP PRIORITY ★ — every cast. Highest-priority kick in End Time.
    moonbolt: secondary — try if Stardust on cd (UNVERIFIED interruptibility)

  rotation_modifications:
    standard: |
      Standard ST rotation. Cast from outside Dark Moonlight aura (15y+).
      Sidestep Moonlance; avoid Eyes of the Goddess orb.
    lunar_guidance_phase_1 (hp_pct == 80): tighten kick cadence — Stardust 25% faster
    lunar_guidance_phase_2 (hp_pct == 55): tighter still — kick rotation must stagger
    tears_of_elune_phase (hp_pct == 30): |
      ★ BURN PHASE ★ — pop ALL offensive cooldowns:
      - Frost Mage: Combustion (UNVERIFIED Cata mage burst CD; Frost has Icy Veins/Mirror Image)
      - Affliction Warlock: Demon Soul + Bane of Havoc + Soul Swap
      - MM Hunter: Rapid Fire + Readiness + Trinkets
      - Shadow Priest: Power Infusion (self) + Shadowfiend
      - Balance Druid: Force of Nature + Starfall + Trinket
      - Ele Shaman: Bloodlust (group cast) + Elemental Mastery + Stormlash
      Bloodlust ideally goes here for the burn phase if not already used.

  cd_use:
    rotational: opener and per-CD-cycle
    burn_phase: |
      Save Bloodlust + class burst CDs for 30% Tears phase. Worth ~15-20%
      of total fight DPS to time correctly.

  heroic_delta: HEROIC ONLY — no delta

  bot_anti_patterns:
    - DO NOT stand within 15y of boss (Dark Moonlight aura — POSITIONAL OVERRIDE)
    - DO NOT skip Stardust kick (top-priority interrupt)
    - DO NOT cluster with healer on aura-edge (silence-orb multi-hit risk)
    - DO NOT kite Tears of Elune (burn phase; cast through)
    - DO NOT pre-pop Bloodlust (save for 30% burn phase)
```

---

## Boss 5 — Echo of Murozond

```yaml
boss_ranged:
  boss: echo_of_murozond
  difficulty_grade: hard (Hourglass click handler + dynamic positioning)
  positioning:
    default: |
      Spread far (12y+ between ranged) — Distortion Bombs target ranged;
      spread minimizes multi-bomb overlap of safe zones.
      Dynamic repositioning: as Distortion Bomb zones fill arena, "max
      range" becomes "most-distant safe spot from bombs and other ranged."
      Stay clear of Infinite Breath frontal cone (boss-facing).
    facing: boss
    movement: |
      Continuous reposition as new bombs land. Move BEFORE bomb lands if
      targeted; you have ~1-2s warning from cast.

  interrupt_role: []                            # no confirmed kicks

  rotation_modifications:
    standard: |
      Standard ST rotation. Ground-patch awareness: never cast from a
      Distortion Bomb zone.
    hourglass_click_handler: |
      ★ PRIMARY RANGED DUTY ★ (designated to highest-mobility ranged bot)
      Mage with Blink, Warlock with Demonic Circle, Hunter with Disengage
      = preferred clickers. Bot mobility ranking:
      1. Frost Mage (Blink, Iceblock)
      2. Affliction Warlock (Demonic Circle, Teleport)
      3. MM Hunter (Disengage, Posthaste)
      4. Other ranged (move on foot)

      CLICK TRIGGER CONDITIONS (any-of):
      - bomb_count_on_floor >= 4
      - party_lowest_hp_pct < 30
      - temporal_blast_stack_count_max >= 4
      - any_party_member_dead AND boss_hp_pct > 30
      - hp_pct(boss) <= 5 AND charges_remaining > 0  # final-burn use

      bot_implementation: |
        encounter_evaluate_hourglass_trigger(state) → bool
        IF self.role_assignment == hourglass_clicker AND trigger_met():
          interrupt_rotation
          move_to(hourglass_gameobject) — use mobility CD if needed
          interact(hourglass_gameobject)  # ~1.5s channel? UNVERIFIED
          resume_rotation_post_click

  cd_use:
    standard_offensive: rotational
    pre_hourglass_cd_pacing: |
      ★ HOURGLASS-AWARE CD PACING ★
      IF hourglass_click_imminent (next ~5s window):
        suppress new offensive CD usage
        reason: CDs reset on click; pop after for free re-use
      IF hourglass_click_just_happened (last ~5s):
        free CD usage window — pop offensive CDs
    post_click_burst_window: |
      Cooldowns reset post-click → IMMEDIATE burst opportunity.
      Pop Bloodlust (group) + class CDs in this window if available.
      Each Hourglass click = potential burst window.

  heroic_delta: HEROIC ONLY — no delta

  bot_anti_patterns:
    - DO NOT stand in Distortion Bomb zone (~25k arcane/sec = death)
    - DO NOT cluster with ranged or healer (bomb multi-target)
    - DO NOT pop offensive CD just before planned Hourglass click (wasted CD)
    - DO NOT stop casting in safe spot (cast-while-stationary preferred when bomb-clear)
    - DO NOT auto-dispel Temporal Blast (BLACKLIST entry)
    - DO NOT click Hourglass if you're not the designated clicker AND clicker is alive
    - DO NOT save all 5 Hourglass charges for emergencies (use ~1 per 25% boss HP)
```

---

## Bot AI hooks

```yaml
ai_hooks:
  random_pool_detection:
    - encounter_zone_entry → poll_time_transit_device_portals OR detect_first_engaged_npc_id
    - encounter_npc_id_match → load_per_boss_ranged_profile

  baine:
    - encounter_state_active → opposite_platform_spread
    - encounter_detect_pulverize_cast → migrate_platform_after_landing
    - encounter_detect_throw_totem_cast → designated_handler_pickup_and_throwback
    - encounter_state_post_totem_debuff_active → pop_offensive_cds_burst_window

  jaina:
    - encounter_state_active → max_range_spread
    - encounter_detect_pyroblast_cast → kick_rotation_top_priority
    - encounter_detect_frostbolt_volley_cast → kick_rotation_secondary
    - encounter_detect_flarecore_spawn → designated_detonator_walk_if_high_hp
    - encounter_detect_blink → reposition_max_range
    - encounter_detect_frost_blades_cast → sidestep_cone

  sylvanas:
    - encounter_state_active → spread_8_10y
    - encounter_detect_calling_of_the_highborne_cast → target_shift_to_lowest_hp_ghoul
    - encounter_detect_blighted_arrows_on_self → move_out_of_rune_within_5s
    - encounter_detect_shadow_link_proximity → step_away

  tyrande:
    - encounter_state_active → POSITIONAL_OVERRIDE_outside_15y_dark_moonlight
    - encounter_detect_stardust_cast → kick_rotation_TOP_PRIORITY
    - encounter_detect_moonbolt_cast → kick_rotation_secondary_attempt
    - encounter_detect_moonlance_cast → sidestep_lance_lane
    - encounter_detect_eyes_of_goddess_orb_path → avoid_orb
    - encounter_state_tears_of_elune_phase → POP_ALL_OFFENSIVE_CDS_BURN

  murozond:
    - encounter_state_active → spread_far_dynamic
    - encounter_detect_distortion_bomb_targeting_self → move_pre_landing
    - encounter_detect_hourglass_trigger_met AND self_is_clicker → click_hourglass
    - encounter_state_pre_hourglass_imminent → suppress_offensive_cd_use
    - encounter_state_post_hourglass_click → free_cd_burst_window

dispel_blacklist:
  - { spell_id: UNVERIFIED, name: Temporal Blast (Murozond), scope: echo_of_murozond_fight, force: true }
  # ranged bots typically only dispel via Mage Remove Curse / Druid Remove Corruption / etc.

mobility_cd_priority:
  baine:    [reactive_post_pulverize]
  jaina:    [reactive_on_blink]
  sylvanas: [reactive_calling_movement]
  tyrande:  [reactive_eyes_of_goddess]
  murozond: [PRIMARY_HOURGLASS_TRAVEL — Mage Blink, Warlock Demonic Circle, Hunter Disengage]

hourglass_click_handler:
  preferred_assignee_priority:
    1: frost_mage (Blink + Iceblock)
    2: affliction_warlock (Demonic Circle + Teleport)
    3: mm_hunter (Disengage + Posthaste)
    4: shadow_priest (Dispersion run-speed)
    5: balance_druid (Travel Form)
    6: ele_shaman (Ghost Wolf)
  click_trigger_conditions_any_of:
    - bomb_count_on_floor >= 4
    - party_lowest_hp_pct < 30
    - temporal_blast_stack_count_max >= 4
    - any_party_member_dead AND boss_hp_pct > 30
    - hp_pct(boss) <= 5 AND charges_remaining > 0
  charge_budget: 5 — aim to use all 5 across fight; never end fight with unused charges
```

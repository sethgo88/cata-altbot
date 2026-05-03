# End Time — Healer

**Healer-specific response logic** for the bot's encounter handling. Mechanics, spell IDs, phase triggers, and add abilities are defined in `encounter.md`; this doc only encodes how the healer reacts. Class-agnostic where possible.

**Layering**:
- Cross-content healer patterns (triage hierarchy, dispel taxonomy, mana pacing, anti-patterns) live in `docs/roles/healer.md`
- Per-spec rotation / CD / mana model lives in the active healer's `docs/specs/` doc (resto-shaman, holy-paladin, holy-priest, disc-priest, resto-druid)
- This doc layers End-Time-specific deviations on top of those

---

## Dungeon-level overview

```yaml
dungeon_healer_overview:
  total_bosses: 5 (always)                      # 2 random echoes + Murozond
  random_pool: [baine, jaina, sylvanas, tyrande]  # 2 of 4 spawn per run
  estimated_full_clear_minutes: 15-25
  difficulty_modes: [heroic_only]
  mana_profile:
    echo_of_baine:    moderate                 # group lava-swim damage on platform-rotation
    echo_of_jaina:    high                     # interrupt-failure on Pyroblast = tank spike
    echo_of_sylvanas: moderate                 # Calling event predictable; spread minimizes ambient damage
    echo_of_tyrande:  high                     # Stardust 60y AoE + Tears of Elune burn phase
    echo_of_murozond: high                     # Temporal Blast cycle + Distortion Bomb floor-fill; Hourglass resets mana
  drink_windows:
    - between echo 1 and echo 2 (back at Time Transit Device)
    - between echo 2 and Murozond (longest drink window — go in full mana)
    - NOT between phases of same boss (no break point)
  key_responsibilities:
    - random_echo_profile_load                 # detect which 2 echoes spawned; load profiles
    - jaina_pyroblast_pre_hot                  # tank-spike pre-empt
    - tyrande_stardust_pre_aoe_heal            # 60y AoE on uninterrupted Stardust
    - tyrande_dark_moonlight_positional_override  # stand OUTSIDE 15y aura
    - sylvanas_shriek_dispel_before_calling    # slow-dispel high-priority window
    - murozond_temporal_blast_DO_NOT_DISPEL    # CRITICAL — Hourglass-cleared
    - murozond_hourglass_aware_mana_pacing     # Hourglass click resets mana to 100%
```

---

## Trash — healer notes

```yaml
trash_healer:
  baine_shrine:
    notes: |
      Time-Twisted Breaker / Drake / Seer packs. Standard tank-and-spank.
      Seer is caster — DPS interrupts; healer reactive.

  jaina_shrine:
    notes: |
      Risen Priest healing-cast — healer NOT involved; DPS interrupts. If
      heal lands, mob HP resets — extended pull = mana drain. If DPS misses
      kick consistently, healer pre-HoTs tank for extended pull.
    fragment_collection: bot can pick up nearby fragments while moving (no combat overhead)

  sylvanas_shrine:
    notes: standard caster-trash; reactive heal flow

  tyrande_shrine_moonbeam_mechanic:
    notes: |
      UNIQUE TANK PRE-FIGHT MECHANIC. Tank drags mobs into rotating moonbeams
      to strip 90% damage-reduction shield. Healer:
      - Tank takes increased melee while waiting for moonbeam (mob isn't
        being damaged → not aggro-bound to attacker)
      - Pre-HoT tank during the routing
      - Group damage low until shield strips → conserve mana

  murozond_shrine:
    notes: standard caster + melee dragonkin trash; reactive heal
```

---

## Boss 1 (random) — Echo of Baine

```yaml
boss_healer:
  boss: echo_of_baine
  difficulty_grade: medium
  mana_intensity: moderate

  threshold_overrides:
    tier_2_tank_emergency: tank.hp_pct < 50    # standard
    tier_3_pulverize_target: ranged.hp_pct < 70  # whoever Pulverize lands on takes ~30k

  predictable_spike_events:
    - pulverize_landing:
        damage: ~30k physical on ranged target + sinks platform
        cadence: ~50s
        response: |
          Pre-shield ranged DPS rotation if Pulverize cast detected; reactive
          heal post-landing. Note: ranged target also has to swim/jump to
          new platform → lava-swim DoT compounds.
    - throw_totem_landing:
        damage: ~60k single-target on random player
        response: reactive single-target burst heal (Greater Healing Wave / Holy Light / Healing Touch)
    - lava_swim_group_damage:
        damage: stacking lava DoT on whoever swims between platforms
        response: HoT-roll group; cleanse stacks irrelevant (not dispellable)

  cooldown_use:
    standard: reactive HoT on tank; rotational class-specific maintenance
    aoe_window: optional — if multiple players in lava simultaneously
    no_unique_cd_save: |
      Save Mana Tide / Innervate / Hymn for Murozond fight if possible; this
      fight is moderate mana intensity but doesn't require burning major CDs.

  dispel_priority: []                           # no dispels required

  interrupt_role: []                            # no kick-required casts

  heroic_delta: HEROIC ONLY — no delta

  bot_anti_patterns:
    - DO NOT enter lava (you are NOT the tank — and even tank avoids it)
    - DO NOT attempt to dispel Molten Mace from boss (boss buff; Purge-only on offensive Shaman, irrelevant for healer)
    - DO NOT chase ranged target across platforms — heal from range
```

---

## Boss 2 (random) — Echo of Jaina

```yaml
boss_healer:
  boss: echo_of_jaina
  difficulty_grade: hard (interrupt-failure-sensitive)
  mana_intensity: high

  threshold_overrides:
    tier_2_tank_emergency: tank.hp_pct < 60    # raised — Pyroblast can deal ~50k + 12s DoT if uninterrupted

  predictable_spike_events:
    - pyroblast_uninterrupted:
        damage: ~50k fire + 12s DoT (~5k/3s = ~20k total)
        response: |
          Pre-HoT tank BEFORE Pyroblast cast (warning is the cast bar itself, ~3s).
          Reactive Greater Healing Wave / Holy Light / Healing Touch on landing.
          DoT component → HoT-roll for 12s.
    - frostbolt_volley:
        damage: ~18k arcane to all + 30% slow 4s
        response: |
          AoE heal cooldown if multiple players hit; slow rarely matters for
          healer positioning.
    - flarecore_detonator:
        damage: 5-50k fire on whoever walks the ember (designated DPS)
        response: |
          Reactive single-target burst heal on ember-walker. NOT pre-HoT —
          who detonates is dynamic per ember.
    - blink_chase_window:
        damage: brief no-damage window during Blink + tank chase
        response: top off ALL party members; recover mana

  cooldown_use:
    mana_tide_totem: deploy ~50% boss HP if interrupt-failure rate high
    innervate: self-cast ~30% mana (Resto Druid)
    spirit_link_totem: rare on this fight (group damage less spiky than Tyrande)
    avenging_wrath_holy_pally: standard rotational

  dispel_priority:
    - { name: Frostbolt Volley slow, school: magic, priority: LOW, notes: dispel-on-tank if mobility critical AND tank kicked the cast }

  interrupt_role:
    pyroblast: PARTICIPATE if class has kick (Resto Shaman Wind Shear, Holy Pally Rebuke)
    frostbolt_volley: PARTICIPATE if Pyroblast on cd

  heroic_delta: HEROIC ONLY — no delta

  bot_anti_patterns:
    - DO NOT detonate Flarecore yourself (healer in melee = mana drain + heal disruption)
    - DO NOT cast hardcasts during Blink chase (boss moves; line of sight breaks)
    - DO NOT auto-dispel Pyroblast DoT (12s, low priority — tank takes it as scheduled damage)
```

---

## Boss 3 (random) — Echo of Sylvanas

```yaml
boss_healer:
  boss: echo_of_sylvanas
  difficulty_grade: medium
  mana_intensity: moderate

  threshold_overrides:
    tier_2_tank_emergency: tank.hp_pct < 50    # standard

  predictable_spike_events:
    - calling_of_the_highborne:
        warning: cast detected (~3-5s channel)
        damage: |
          - Pull damage as group is yanked to Sylvanas (no damage from pull,
            but positioning disruption)
          - Wracking Pain ticks if any player crosses ghoul links / shadow zones
          - Group DPS-shift creates ambient damage from missing kicks elsewhere
        response: |
          Pre-cast group AoE heal (Wild Growth / Healing Rain / Circle of Healing
          / Holy Radiance / Prayer of Healing) right before Calling resolves.
          During the event, HoT-roll group; reactive heal anyone clipping links.
          On gap-escape: clean up.
    - black_arrow / unholy_shot:
        damage: AoE around target / multi-target missile
        response: spread minimizes; reactive single-target heal on hit player
    - shriek_of_the_highborne:
        damage: -50% MS on random + shadow tick
        response: DISPEL slow before next Calling event (slowed player can't escape)
        priority: HIGH_DISPEL
    - blighted_arrows:
        damage: ground rune lethal if standing >5s
        response: reactive heal target moving out; pre-HoT if player slow to react

  cooldown_use:
    mana_tide_totem: ~50% boss HP if multiple Callings missed
    spirit_link_totem: deploy IF group HP collapses during Wracking Pain ticks
    tranquility: emergency raid heal post-Calling failure (rare)

  dispel_priority:
    - { spell_id: UNVERIFIED, name: Shriek of the Highborne, school: magic, priority: HIGH_BEFORE_CALLING, notes: dispel slow on random target so they can escape ghoul circle }

  interrupt_role: []                            # no confirmed kicks

  heroic_delta: HEROIC ONLY — no delta

  bot_anti_patterns:
    - DO NOT dispel Wracking Pain (passive aura, not target debuff)
    - DO NOT stand in shadow zones or cross ghoul links during Calling
    - DO NOT chase strays during Calling — group is pulled to boss; stay with group
```

---

## Boss 4 (random) — Echo of Tyrande

```yaml
boss_healer:
  boss: echo_of_tyrande
  difficulty_grade: hard
  mana_intensity: high
  notes: |
    HARDEST kick-rotation fight in End Time. Stardust = top-priority kick.
    Healer also has POSITIONAL OVERRIDE: stand OUTSIDE 15y Dark Moonlight aura.
    30% HP burn phase (Tears of Elune) is the OOM-test.

  threshold_overrides:
    tier_2_tank_emergency: tank.hp_pct < 50
    tier_4_aoe_cluster_threshold: trigger_count: 3 → 2  # lower threshold during Tears of Elune phase

  predictable_spike_events:
    - stardust_uninterrupted:
        damage: ~50k arcane to ALL players in 60y
        response: |
          BIGGEST GROUP-DAMAGE event of the fight. Pre-cast AoE heal CD
          if kick is uncertain (e.g., kicker on cooldown):
          - Resto Shaman: Healing Stream Totem + Healing Rain pre-drop
          - Holy Paladin: Holy Radiance + Light of Dawn at full HP
          - Holy Priest: Circle of Healing + PoH
          - Disc Priest: PoM bounces + PW:Barrier
          - Resto Druid: Wild Growth + Tranquility on heroic
    - tears_of_elune_phase:
        trigger: hp_pct == 30
        damage: continuous stars on player feet (~30k arcane in 4y)
        response: |
          BURN PHASE — kill the boss. Healer burns through mana; pop ALL
          remaining CDs. Spirit Link / Tranquility / Aura Mastery / Divine
          Hymn here.
    - lunar_guidance_stacks:
        triggers: [hp_pct == 80, hp_pct == 55]
        damage: indirect — Stardust kicks become tighter, more interrupt failures
        response: pre-HoT before Stardust window; AoE CD pacing

  cooldown_use:
    mana_tide_totem: deploy at hp_pct == 55 (Lunar Guidance stack 2)
    spirit_link_totem: deploy at 30% (Tears burn phase) for group HP smooth
    innervate: self-cast at 30% mana
    tranquility: 30% boss HP burn phase
    avenging_wrath: at 30% boss HP (Holy Pally)
    divine_hymn: 30% burn phase

  dispel_priority: []                           # nothing confirmed

  interrupt_role:
    stardust: PARTICIPATE if class has kick — TOP PRIORITY
    moonbolt: try if Stardust on cd (UNVERIFIED interruptibility)

  heroic_delta: HEROIC ONLY — no delta

  bot_anti_patterns:
    - DO NOT stand within 15y of boss (Dark Moonlight -50% cast speed) — POSITIONAL OVERRIDE; not max-range, but at-aura-edge
    - DO NOT save AoE heal CD past Stardust if kick is uncertain
    - DO NOT kite Tears of Elune (burn phase; just heal through)
    - DO NOT cast hardcast during Eyes of the Goddess silence-orb path
```

---

## Boss 5 — Echo of Murozond

```yaml
boss_healer:
  boss: echo_of_murozond
  difficulty_grade: hard
  mana_intensity: high
  notes: |
    Final boss. DPS race with Hourglass-of-Time reset cushion. Healer's
    primary jobs: pace mana between Hourglass clicks, react to Distortion
    Bomb hits, manage Temporal Blast stack ramp WITHOUT dispelling.

    HOURGLASS RESET = HEALER FREE-MANA EVENT: HP/mana restored to 100% on
    each Hourglass click. Healer's strategy is to spend mana freely between
    clicks, knowing the click will refill.

  threshold_overrides:
    tier_2_tank_emergency: tank.hp_pct < 55    # raised — Infinite Breath splash + Distortion Bomb hits compound

  predictable_spike_events:
    - temporal_blast_cycle:
        cadence: ~15s
        damage: ~25k shadow PBAoE + stacking +10% arcane-taken debuff (20s)
        response: |
          AoE HoT-roll the group. Stacks ramp Distortion Bomb damage taken.
          DO NOT dispel — Hourglass clears on click.
    - distortion_bomb_landing:
        damage: ~25k arcane/sec ground patch (permanent until Hourglass)
        response: reactive heal if any player clipped (most should move out before landing)
    - infinite_breath_on_tank:
        damage: ~75k fire frontal cone (tank only if facing correctly)
        response: pre-HoT tank; Greater Heal post-cast
    - hourglass_click_event:
        damage: NONE (positive event — full HP/mana restore)
        response: |
          POST-CLICK: free mana window; spend freely; pop offensive support DPS
          if all triage tiers null (Atonement Disc, Lightning Bolt Resto Shaman).

  cooldown_use:
    mana_tide_totem: deploy 1 charge in mid-fight; second mana CD if Hourglass clicks slow
    spirit_link_totem: deploy if multiple players take simultaneous bomb damage + temporal blast stacks
    innervate: self-cast between Hourglass clicks
    hymn_of_hope: ~30% mana (Holy/Disc Priest)
    tranquility: emergency only — Hourglass click usually preferred

  dispel_priority: []                           # NONE — Temporal Blast BLACKLISTED
  DISPEL_BLACKLIST:
    - { spell_id: UNVERIFIED, name: Temporal Blast, reason: Hourglass-cleared more efficiently than per-stack dispels }

  interrupt_role: []                            # no confirmed kick targets

  hourglass_handling:
    primary_clicker: ranged DPS (typically Mage or Warlock)
    healer_role: NOT primary clicker; backup if all ranged DPS dead
    healer_post_click_behavior: |
      Click resets HP/mana to 100%. After click:
      - Spend mana freely on cluster heals
      - Reset HoT-roll on tank
      - Prepare for next ~15-30s window

  heroic_delta: HEROIC ONLY — no delta

  bot_anti_patterns:
    - DO NOT auto-dispel Temporal Blast (75476-equivalent on this fight; BLACKLISTED — Hourglass cleans up)
    - DO NOT click Hourglass yourself unless designated clicker is dead AND emergency
    - DO NOT pop Mana Tide / Hymn of Hope right BEFORE a planned Hourglass click (mana resets to 100% — wasted CD)
    - DO NOT stand in Distortion Bomb zones (you are NOT the tank; bomb DoT is unsurvivable)
    - DO NOT cluster with ranged DPS (Distortion Bombs target ranged; cluster = multi-bomb risk)
```

---

## Bot AI hooks

```yaml
ai_hooks:
  random_pool_detection:
    - encounter_zone_entry → poll_time_transit_device_portals OR detect_first_engaged_npc_id
    - encounter_npc_id_match → load_per_boss_healer_profile

  baine:
    - encounter_detect_pulverize_cast → pre_shield_ranged_target
    - encounter_detect_throw_totem_cast → reactive_burst_heal_target
    - encounter_state_lava_swim_active → group_aoe_hot_roll

  jaina:
    - encounter_detect_pyroblast_cast → pre_hot_tank_then_reactive_greater_heal
    - encounter_detect_frostbolt_volley_cast → pre_aoe_heal_or_dispel_slow_priority_low
    - encounter_detect_flarecore_walked → reactive_burst_heal_walker
    - encounter_detect_blink → recover_mana_top_off_party

  sylvanas:
    - encounter_detect_calling_of_the_highborne_cast → pre_aoe_heal_then_hot_roll
    - encounter_detect_shriek_on_random → DISPEL_slow_BEFORE_next_calling
    - encounter_detect_blighted_arrows_on_target → reactive_heal_pre_move

  tyrande:
    - encounter_state_active → POSITIONAL_OVERRIDE_outside_15y_dark_moonlight
    - encounter_detect_stardust_cast → pre_aoe_heal_cd_if_kick_uncertain
    - encounter_state_lunar_guidance_stack_2 → tighten_aoe_heal_cadence
    - encounter_state_tears_of_elune_phase → pop_all_remaining_cooldowns

  murozond:
    - encounter_detect_temporal_blast → group_aoe_hot_roll (DO NOT dispel)
    - encounter_detect_distortion_bomb_targeting → pre_hot_target
    - encounter_detect_hourglass_click → free_mana_window_spend_freely
    - encounter_state_pre_hourglass_click_imminent → suppress_mana_cd_use

dispel_blacklist:
  - { spell_id: UNVERIFIED, name: Temporal Blast (Murozond), scope: echo_of_murozond_fight, force: true }

dispel_priorities:
  - { spell_id: UNVERIFIED, name: Shriek of the Highborne (Sylvanas), scope: echo_of_sylvanas_fight, priority: HIGH_BEFORE_CALLING }
  - { spell_id: 101810, name: Frostbolt Volley slow (Jaina), scope: echo_of_jaina_fight, priority: LOW }

mana_cd_save_priority:
  baine:    [reactive_only]
  jaina:    [Mana Tide @ ~50% if interrupt failures, save other CDs for Tyrande/Murozond]
  sylvanas: [reactive_only_unless_calling_failure]
  tyrande:  [Mana Tide @ 55% boss HP, Spirit Link @ 30% Tears phase, Tranquility @ 30%]
  murozond: [Mana Tide between Hourglass clicks; Hymn of Hope @ 30% mana; Spirit Link reactive on bomb-cluster damage]

hourglass_aware_cd_pacing:
  rule: |
    On Murozond fight, suppress major mana / healing CDs in the ~5s window
    BEFORE a planned Hourglass click. Click resets HP/mana to 100%, so any
    CD popped just before is wasted. Pop CDs immediately AFTER click for
    best value.
```

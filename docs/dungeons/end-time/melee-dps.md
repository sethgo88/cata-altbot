# End Time — Melee DPS

**Melee-DPS-specific response logic** for the bot's encounter handling. Mechanics, spell IDs, phase triggers, and add abilities are defined in `encounter.md`; this doc only encodes how melee DPS reacts. Class-agnostic where possible.

**Layering**:
- Cross-content melee DPS patterns (positioning, interrupt rotation, threat ceiling) live in `docs/roles/melee-dps.md`
- Per-spec rotation lives in the active spec's `docs/specs/` doc (combat-rogue, fury-warrior, ret-paladin, frost-dk, enh-shaman, feral-cat)
- This doc layers End-Time-specific deviations on top of those

---

## Dungeon-level overview

```yaml
dungeon_melee_dps_overview:
  total_bosses: 5 (always)                      # 2 random echoes + Murozond
  random_pool: [baine, jaina, sylvanas, tyrande]
  estimated_full_clear_minutes: 15-25
  difficulty_modes: [heroic_only]
  positioning_profile:
    echo_of_baine:    in_melee_out_of_lava       # follow tank platform-rotation
    echo_of_jaina:    behind_chase_blink         # frequent reposition
    echo_of_sylvanas: behind_spread_partial      # spread for AoE; melee on boss
    echo_of_tyrande:  behind_in_dark_moonlight   # in 15y aura (cast speed irrelevant)
    echo_of_murozond: SIDE_of_boss               # avoid Infinite Breath front + Tail Sweep rear (Disagreement #3 default)
  interrupt_load:
    echo_of_baine:    zero
    echo_of_jaina:    HIGH (Pyroblast + Frostbolt Volley)
    echo_of_sylvanas: zero
    echo_of_tyrande:  HIGH-CRITICAL (Stardust top-priority)
    echo_of_murozond: zero
  key_responsibilities:
    - random_pool_profile_load
    - platform_hop_with_tank (baine)
    - blink_chase (jaina)
    - ghoul_focus_coordination (sylvanas)
    - stardust_kick_participation (tyrande)
    - side_position_avoid_tail_sweep (murozond)
```

---

## Trash — melee DPS notes

```yaml
trash_melee:
  baine_shrine:
    interrupt_role: |
      Time-Twisted Seer caster — kick mandatory. Melee bot kick on rotation:
      - Combat Rogue: Kick (10s)
      - Fury Warrior: Pummel (10s; off-GCD)
      - Ret Pally: Rebuke (15s; off-GCD)
      - Frost DK: Mind Freeze (10s; off-GCD)
      - Enh Shaman: Wind Shear (6s; off-GCD; 25y range)
      - Feral Cat: Skull Bash (60s; gap-close + interrupt)
    cc_priorities: standard CC only if tank requests

  jaina_shrine_priests:
    interrupt_role: |
      Risen Priest heal-cast — KICK MANDATORY. Risen Sorceress Frostbolt-cast.
      Melee in range = kick rotation primary participant.
    fragment_collection: |
      Bot picks up nearby Jaina Fragments while in melee. Pickup is instant
      interaction; doesn't break rotation.

  sylvanas_shrine:
    interrupt_role: Risen Banshee caster — kick rotation participant

  tyrande_shrine_moonbeam:
    notes: |
      Pre-fight TANK MECHANIC: drag mob into moonbeam to strip 90% damage
      reduction. Melee DPS HOLDS damage until shield strips. Counter-
      intuitive for melee whose default is "hit when in range." Override
      with "hit when mob.in_moonbeam == true."
    bot_implementation: |
      detect mob.has_aura(SHADOW_SHIELD) → suppress damage until aura.missing()
      or mob.in_moonbeam() == true.

  murozond_shrine:
    interrupt_role: caster trash — standard kick rotation
```

---

## Boss 1 (random) — Echo of Baine

```yaml
boss_melee:
  boss: echo_of_baine
  difficulty_grade: medium (positional + platform hop)
  positioning:
    default: |
      In melee on boss; out of lava (Molten Mace boss-buff is bad — DON'T
      drag boss into lava either).
    facing: behind boss (no confirmed cone but discipline)
    movement: |
      On Pulverize cast → boss leaps to ranged → ranged platform sinks →
      melee follows tank to next platform. Hop with tank.
  interrupt_role: []                            # no kicks

  rotation_modifications:
    standard: |
      Standard ST rotation. Pulverize-leap creates a ~3-5s no-melee window
      while boss is on ranged platform; use this window:
      - Combat Rogue: Sprint to re-engage; refresh SnD
      - Fury Warrior: Charge / Heroic Leap to re-engage (if available)
      - Ret Pally: Hammer of Wrath proc check; otherwise wait
      - Frost DK: Death Grip is RISKY — could pull boss off platform-rotation
        path; HOLD GRIP unless mid-platform-stuck scenario
      - Enh Shaman: ranged Maelstrom-Lightning Bolt during gap; Ghost Wolf
      - Feral Cat: Stampeding Roar (raid speed) for hop; Skull Bash gap-close
    burst_window_post_totem: |
      If ranged DPS successfully throws totem back at boss:
      → boss stunned + 50% damage-taken-up debuff for 20s
      → POP all offensive CDs in this 20s window:
      - Combat Rogue: Adrenaline Rush + Killing Spree + trinkets
      - Fury Warrior: Recklessness + Death Wish + trinkets
      - Ret Pally: Avenging Wrath + Zealotry + Guardian of Ancient Kings
      - Frost DK: Pillar of Frost + Empower Rune Weapon + trinkets
      - Enh Shaman: Feral Spirit + Stormstrike + trinkets
      - Feral Cat: Tiger's Fury + Berserk + trinkets

  cd_use:
    standard_offensive: rotational on engagement
    save_for_totem_debuff: |
      Burst window opens when totem-stun + 50%-debuff applied. 20s window
      → up to 1.5 standard burst-CD cycles.

  heroic_delta: HEROIC ONLY — no delta

  bot_anti_patterns:
    - DO NOT enter lava (no benefit; only damage)
    - DO NOT pull boss out of platform position (Death Grip / Charge-pull)
    - DO NOT skip platform hop (you'll be in lava-only space when boss lands)
    - DO NOT pre-pop CDs before totem-stun (save for the burst window)
```

---

## Boss 2 (random) — Echo of Jaina

```yaml
boss_melee:
  boss: echo_of_jaina
  difficulty_grade: hard (Blink chase + kicks + Frost Blades cone)
  positioning:
    default: behind boss; out of Frost Blades cone forward arc
    facing: behind boss
    movement: |
      Boss Blinks every ~10-15s → re-engage at new boss location ASAP.
      Use gap-closers / mobility CDs for fast re-engage:
      - Combat Rogue: Sprint, Step (talented)
      - Fury Warrior: Charge / Heroic Leap (or Intercept if stanced)
      - Ret Pally: Long Arm of the Law (movespeed talent — UNVERIFIED rotation)
      - Frost DK: Death Grip (CAREFUL — ensure tank threat lead first)
      - Enh Shaman: Spirit Wolf / Ghost Wolf
      - Feral Cat: Stampeding Roar / Dash; Skull Bash
  interrupt_role:
    pyroblast: PRIMARY KICK — every cast (any melee class with kick)
    frostbolt_volley: SECONDARY KICK — if Pyroblast on cd

  rotation_modifications:
    standard: |
      Standard ST rotation. Frost Blades cone → reposition between blade
      lanes. Blink chase causes ~3-5s no-melee window per cycle.
    flarecore_handling: |
      Melee in range — designated detonator only if no high-HP ranged
      available (typical ranged-priority chain). Default: do NOT walk
      ember; let ranged handle.

  cd_use:
    rotational: opener and per-CD-cycle
    no_burn_phase: Jaina has no burn phase; standard rotational CDs fine

  heroic_delta: HEROIC ONLY — no delta

  bot_anti_patterns:
    - DO NOT stand in Frost Blades cone (5s stun = lost rotation time)
    - DO NOT skip Pyroblast kick (primary tank pressure)
    - DO NOT chase Blink with abilities still on cd from previous chase (let cd recover)
    - DO NOT detonate Flarecore unless self.hp_pct >= 80 AND no ranged available
    - DO NOT pull boss off tank with Death Grip (verify threat lead first)
```

---

## Boss 3 (random) — Echo of Sylvanas

```yaml
boss_melee:
  boss: echo_of_sylvanas
  difficulty_grade: medium (target-shift on Calling)
  positioning:
    default: |
      Behind boss; spread partial — melee can't fully spread but stay 5y+
      from other melee for Black Arrow / Unholy Shot mitigation.
    facing: behind boss
    movement: |
      Blighted Arrows ground rune: move out within 5s.
      Calling event: pulled to boss → focus 1 ghoul → escape gap → re-engage.
  interrupt_role: []                            # no confirmed kicks

  rotation_modifications:
    standard: |
      Standard ST rotation on Sylvanas.
    on_calling_of_the_highborne: |
      ★ TARGET-SHIFT EVENT ★
      Same as ranged DPS — all DPS (incl melee) shift to LOWEST-HP GHOUL
      (ties broken by closest to bot's position).
      ★ COORDINATION ★ — same ghoul as other DPS bots.
      Buffs that are TARGET-BOUND TRANSFER:
      - Combat Rogue: CP attached to target → RESET on shift to ghoul.
        Build new CP on ghoul; finishers go on ghoul if 5 CP reached.
      - Frost DK: diseases on target → re-Outbreak on ghoul
      - Feral Cat: CP attached to target + bleeds → RESET. Apply Mangle/Rake/Rip
        on ghoul if extended kill.
      - Fury Warrior / Ret Pally / Enh Shaman: most buffs are self-buffs
        (Inquisition, Slice and Dice, Lightning Shield) → unaffected by shift.
      Ghoul kill expected fast; full DoT/CP rebuild on ghoul rarely worth.
      RESHIFT to Sylvanas after gap-escape; rebuild bleeds/diseases on her.

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
    save_for_calling: optional — pop offensive CD on ghoul-focus to break circle faster (CP/DoT classes lose value on shift)

  heroic_delta: HEROIC ONLY — no delta

  bot_anti_patterns:
    - DO NOT continue DPS on Sylvanas during Calling — kill ghoul instead
    - DO NOT cross ghoul links during Calling (Wracking Pain ticks)
    - DO NOT spend all 5 CP on ghoul if it'll die before finisher casts (timing)
    - DO NOT stay in melee of Sylvanas during Calling pull (group is yanked anyway)
```

---

## Boss 4 (random) — Echo of Tyrande

```yaml
boss_melee:
  boss: echo_of_tyrande
  difficulty_grade: hard (kick + Moonlance + burn phase)
  positioning:
    default: |
      Behind boss; in 15y Dark Moonlight aura (cast speed irrelevant for
      melee; not penalized like casters).
    facing: behind boss
    movement: |
      Sidestep Moonlance lance lanes; avoid Eyes of the Goddess silence-orb
      path; reposition for Tears of Elune at 30%.
  interrupt_role:
    stardust: ★ TOP PRIORITY ★ — every cast
    moonbolt: secondary — if Stardust on cd

  rotation_modifications:
    standard: |
      Standard ST rotation. Tears of Elune at 30% HP → BURN PHASE → pop ALL
      offensive CDs:
      - Combat Rogue: Adrenaline Rush + Killing Spree + Vendetta? (Assassin only)
        — for Combat: AR + Killing Spree + Trinket
      - Fury Warrior: Recklessness + Death Wish + Trinkets + Bloodlust
      - Ret Pally: Avenging Wrath + Zealotry + Guardian of Ancient Kings
      - Frost DK: Pillar of Frost + Empower Rune Weapon + Trinkets
      - Enh Shaman: Feral Spirit + Stormstrike + Bloodlust call
      - Feral Cat: Tiger's Fury + Berserk + Trinkets
      Bloodlust ideally goes here for the burn phase.

  cd_use:
    rotational: opener + per-CD-cycle
    burn_phase: |
      Save Bloodlust + class burst CDs for 30% Tears phase. Melee also
      benefits from increased boss HP-loss-rate during ranged-Stardust-kick
      pressure (less time-in-fight = less ranged-DPS pressure).

  heroic_delta: HEROIC ONLY — no delta

  bot_anti_patterns:
    - DO NOT stand in Moonlance lance lane (5s stun = lost rotation time)
    - DO NOT skip Stardust kick (top-priority)
    - DO NOT cluster with other melee on cone-side (Eyes silence multi-hit)
    - DO NOT pre-pop burn-phase CDs (save for 30% Tears)
    - DO NOT kite Tears (burn phase, melee through)
```

---

## Boss 5 — Echo of Murozond

```yaml
boss_melee:
  boss: echo_of_murozond
  difficulty_grade: hard (positional + Hourglass-aware CD pacing)
  positioning:
    default: |
      ★ POSITIONAL OVERRIDE ★ — SIDE of boss, NOT strict rear (Tail Sweep
      rear cone, Disagreement #3 conservative default).
      Out of Infinite Breath frontal cone.
      Avoid Distortion Bomb ground patches.
    facing: side of boss
    movement: |
      Continuous reposition as Distortion Bomb zones fill arena. Tank
      migrates boss; melee follows to maintain side-position.
  interrupt_role: []                            # no confirmed kicks

  rotation_modifications:
    standard: |
      Standard ST rotation. Distortion Bomb zone awareness — never melee
      from inside a bomb zone.
    hourglass_aware_cd_pacing: |
      ★ HOURGLASS-AWARE CD PACING ★
      Same rule as ranged. CDs reset on Hourglass click → suppress new
      offensive CD usage in ~5s window before planned click; pop after.

      Per class:
      - Combat Rogue: SnD self-buff persists through reset; CP attached to
        target persists. Adrenaline Rush + Killing Spree are major CDs;
        suppress before click.
      - Fury Warrior: Recklessness + Death Wish; suppress before click.
        Rage pool stays (resource, not cd).
      - Ret Pally: Avenging Wrath + Zealotry + Guardian; suppress before
        click. Holy Power resource stays.
      - Frost DK: Pillar of Frost + Empower Rune Weapon; suppress before
        click. Diseases on target stay; runes restored.
      - Enh Shaman: Feral Spirit + Stormstrike CD; suppress before click.
        Maelstrom Weapon stacks reset on click? UNVERIFIED.
      - Feral Cat: Tiger's Fury + Berserk; suppress before click. Bleeds
        on target stay. Energy refills.

      bot_implementation: |
        encounter_state_pre_hourglass_imminent (next ~5s) →
          suppress_offensive_cd_use
          continue_rotational_filler_only
        encounter_state_post_hourglass_click (next ~5s) →
          POP offensive CDs immediately

  cd_use:
    standard_offensive: rotational
    pre_click_suppress: see hourglass_aware_cd_pacing above
    post_click_burst_window: pop CDs immediately

  heroic_delta: HEROIC ONLY — no delta

  bot_anti_patterns:
    - DO NOT stand in Distortion Bomb zone (~25k arcane/sec = death)
    - DO NOT stand directly behind boss (Tail Sweep — Disagreement #3 conservative)
    - DO NOT stand in Infinite Breath frontal cone
    - DO NOT pop Recklessness / Avenging Wrath / Pillar of Frost / etc. just before Hourglass click
    - DO NOT chase tank if tank migrates over a bomb (cut your own path around)
    - DO NOT click Hourglass yourself (ranged DPS handles it; melee role is to keep DPS uptime)
```

---

## Bot AI hooks

```yaml
ai_hooks:
  random_pool_detection:
    - encounter_zone_entry → poll_time_transit_device_portals OR detect_first_engaged_npc_id
    - encounter_npc_id_match → load_per_boss_melee_profile

  baine:
    - encounter_state_active → in_melee_out_of_lava
    - encounter_detect_pulverize_cast → wait_for_landing → hop_to_next_platform
    - encounter_state_post_totem_stun_debuff_active → POP_OFFENSIVE_CDS_BURST_WINDOW

  jaina:
    - encounter_state_active → behind_boss_avoid_frost_blades_cone
    - encounter_detect_pyroblast_cast → kick_rotation_top_priority
    - encounter_detect_frostbolt_volley_cast → kick_rotation_secondary
    - encounter_detect_blink → re_engage_gap_close
    - encounter_detect_frost_blades_cast → sidestep_cone

  sylvanas:
    - encounter_state_active → behind_boss_partial_spread
    - encounter_detect_calling_of_the_highborne_cast → target_shift_to_lowest_hp_ghoul
    - encounter_detect_blighted_arrows_on_self → move_out_within_5s

  tyrande:
    - encounter_state_active → behind_boss_in_aura
    - encounter_detect_stardust_cast → kick_rotation_TOP_PRIORITY
    - encounter_detect_moonbolt_cast → kick_rotation_secondary_attempt
    - encounter_detect_moonlance_cast → sidestep_lance_lane
    - encounter_detect_eyes_of_goddess_orb → avoid_path
    - encounter_state_tears_of_elune_phase → POP_ALL_OFFENSIVE_CDS_BURN

  murozond:
    - encounter_state_active → side_of_boss_avoid_tail_sweep_and_breath
    - encounter_detect_distortion_bomb_landing → step_clear
    - encounter_state_pre_hourglass_imminent → suppress_offensive_cd_use
    - encounter_state_post_hourglass_click → POP_offensive_cds_burst

dispel_blacklist:
  - { spell_id: UNVERIFIED, name: Temporal Blast (Murozond), scope: echo_of_murozond_fight, force: true }
  # melee bots typically don't dispel; included for completeness on hybrid melee (Ret Pally Cleanse, Enh Shaman Cleanse Spirit)

cd_pacing:
  baine:    [save_burst_for_totem_stun_window]
  jaina:    [rotational]
  sylvanas: [rotational; mild save for ghoul-focus event]
  tyrande:  [save_burst_for_tears_burn_phase_at_30_percent]
  murozond: [HOURGLASS_AWARE — suppress pre-click, pop post-click]
```

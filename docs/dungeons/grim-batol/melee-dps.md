# Grim Batol — Melee DPS

**Melee-DPS-specific response logic** for the bot's encounter handling. Mechanics, spell IDs, phase triggers, and add abilities are defined in `encounter.md`; this doc only encodes how melee DPS reacts. Class-agnostic where possible.

**Layering**:
- Cross-content melee DPS patterns (positioning, interrupt rotation, threat ceiling) live in `docs/roles/melee-dps.md`
- Per-spec rotation lives in the active spec's `docs/specs/` doc (combat-rogue, fury-warrior, ret-paladin, frost-dk, enh-shaman, feral-cat)
- This doc layers Grim Batol-specific deviations on top of those

---

## Dungeon-level overview

```yaml
dungeon_melee_dps_overview:
  total_bosses: 4
  estimated_full_clear_minutes: 30-45
  positioning_profile:
    umbriss: behind_boss_ground_siege_aware     # behind boss; move out of Ground Siege cone; spread for Blitz path
    throngus: behind_boss_stance_aware          # behind boss; behind during Shield stance (99% reduction front); follow-tank-during-Mace (NOT into kite path)
    drahga: behind_boss_p1_then_behind_valiona  # P2 Devouring Flames cone behind override
    erudax: behind_boss_collapse_to_safe_zone   # SHADOW GALE STAND-IN OVERRIDE; intercept Corruptor on spawn
  interrupt_load:
    umbriss: zero                                # no kick-required casts
    throngus: low                                # Disorienting Roar attempt
    drahga: low                                  # Burning Shadowbolt attempt
    erudax: high                                 # MUST INTERRUPT Umbral Mending on Corruptor + gap-close to Corruptor on spawn
  key_responsibilities:
    - cave_in_patch_avoidance (throngus)
    - shield_stance_behind_positioning (throngus)
    - kill_invoked_flaming_spirit_priority (drahga)
    - corruptor_gap_close_and_interrupt (erudax)
    - shadow_gale_safe_zone_collapse (erudax)
```

---

## Trash — melee DPS notes

```yaml
trash_melee:
  twilight_humanoid_packs:
    cc_priorities: |
      Polymorph / Sap available targets first if multi-CC team. Melee bot
      contributes Sap (Rogue) or Hammer of Justice (Pally) to opener-CC;
      primary role is kill-ordered DPS on skull marker.
  caster_packs:
    notes: |
      Twilight Cultists / Beguilers / Faceless trash hardcasts. Melee in
      range MUST kick on rotation:
      - Combat Rogue: Kick (10s CD)
      - Fury Warrior: Pummel (10s CD; off-GCD)
      - Ret Pally: Rebuke (15s CD; off-GCD)
      - Frost DK: Mind Freeze (10s CD; off-GCD)
      - Enh Shaman: Wind Shear (6s CD; off-GCD; 25y range)
      - Feral Cat: Skull Bash (60s CD; gap-close + interrupt)
  trogg_packs:
    notes: AoE-cleave; standard rear-arc; bleeds NOT dispellable
  drakonid_packs:
    notes: tank face-aware; melee at side or rear (frontal cleave)
  twilight_demolisher:
    notes: large mob; frontal cleave; melee strictly side/rear
  mature_twilight_drake:
    notes: |
      BOTH frontal cleave AND tail swipe — melee positions at SIDE arc only.
      Behind-target rule violated by tail swipe; the standard "behind"
      heuristic must allow side-positioning here.
  bombing_run_sequence:
    notes: |
      If group uses bombing skip, melee is mounted on a drake. Skip
      automation is v2+ feature. Default: linear clear.
```

---

## Boss 1 — General Umbriss

```yaml
boss_melee:
  boss: general_umbriss
  difficulty_grade: medium
  positioning:
    default: behind boss; spread from other melee (Blitz path discipline)
    facing: boss back
    movement: |
      ON BLITZ TELEGRAPH (~3s) — if you are charge target OR in path,
      MOVE PERPENDICULAR.
      ON GROUND SIEGE TELEGRAPH (2s cast) — move out of cone arc.
      ON MALIGNANT TROGG SPAWN — DO NOT engage Malignant Trogg in melee
      near boss; ranged kites it. Continue boss DPS.
  interrupt_role:
    no_kick_required: true
  rotation_modifications:
    standard: |
      Standard ST rotation on Umbriss. AoE rotation on Trogg Dweller add
      cluster (Malignant Trogg is being kited by ranged):
      - Combat Rogue: Blade Flurry on; Fan of Knives if 3+ adds
      - Fury Warrior: Whirlwind / Bladestorm on cluster
      - Ret Pally: Divine Storm at 3 HP; Consecration on cluster
      - Frost DK: Howling Blast / Death and Decay
      - Enh Shaman: Magma Totem + Chain Lightning at 5 MW
      - Feral Cat: Swipe (Cat) + maintain primary target Rip
    blitz_response: |
      If self is Blitz target → INTERRUPT current rotation tick → MOVE
      perpendicular → RESUME ROTATION on return.
    ground_siege_response: |
      Move out of cone. 4s stun if caught — significant DPS loss.
  cd_use:
    standard_offensive: rotational; align with Tiger's Fury / Berserk / Avenging Wrath / Bloodlust
  heroic_delta: |
    - Blitz one-shots — pristine reaction time required
    - Trogg adds hit harder — bleed stacks more dangerous on melee
    - Frenzy at 30% — pop personal defensive (Cloak of Shadows, Shield Wall, etc.) if you're tanking adds
  bot_anti_patterns:
    - DO NOT engage Malignant Trogg in melee near boss (Modgud's Malice radius UNVERIFIED; conservative avoid)
    - DO NOT cluster melee together (Blitz multi-hit hazard)
    - DO NOT continue cast through Blitz telegraph if self is target
    - DO NOT use AoE-knockback (Heroic Throw stagger, Death Grip pull mechanics) on Trogg packs
```

---

## Boss 2 — Forgemaster Throngus

```yaml
boss_melee:
  boss: forgemaster_throngus
  difficulty_grade: medium (stance-aware positioning + Cave In avoidance)
  positioning:
    default: behind boss; rear arc
    stance_overrides:
      sword: |
        Standard behind-boss melee. Rotation pressure highest here on tank;
        steady DPS for melee.
      mace: |
        Tank kites; boss moves. Melee FOLLOWS at ~5-10 yards behind tank,
        STAYS OFF kite path (UNVERIFIED lava trail; conservative avoid).
        DPS gap-fills during kite-pass = ~3-5s no-melee window.
      shield: |
        Standard behind-boss positioning, but Phalanx provides 99% damage
        reduction from FRONT — make sure positioning is BEHIND, not flank.
        Out of Flaming Arrow patches.
    facing: boss back per stance
    movement: |
      ON MIGHTY STOMP CAST (2s) — pre-position to NOT cluster (Cave In
      patches drop random).
      ON CAVE IN PATCH ON SELF — move out (6-yard radius); resume rotation.
      ON IMPALING SLAM (Mace stance) — if self is jump target, brace; else
      reactive heal availability check.
  interrupt_role:
    disorienting_roar: ATTEMPT — kick on cooldown if available (highest value if interruptible)
    mighty_stomp: ATTEMPT — kick if available
  rotation_modifications:
    sword_stance: |
      Standard ST rotation; full uptime.
      DOT-class disease/poison maint:
      - Combat Rogue: Slice and Dice + Rupture maint; Eviscerate finisher
      - Fury Warrior: Bloodthirst CD + Whirlwind cleave + Heroic Strike rage dump
      - Ret Pally: Inquisition maint + Templar's Verdict 3 HP + Crusader Strike
      - Frost DK: disease maint via Outbreak + Obliterate spam
      - Enh Shaman: Lava Lash + Stormstrike + Maelstrom-LB
      - Feral Cat: Rake + Rip maint + Mangle + Shred + Ferocious Bite
    mace_stance_kite: |
      Boss moves; melee gap-fills:
      - Combat Rogue: Sprint to re-engage; Slice and Dice from 1 CP keep up
      - Fury Warrior: Charge / Heroic Leap to re-engage; rage high coming back
      - Ret Pally: Hammer of Wrath if execute window; otherwise wait for boss arrive
      - Frost DK: Death Grip — DO NOT USE on Throngus (interferes with tank kite path); save for execute
      - Enh Shaman: ranged Maelstrom-LB during gap; Ghost Wolf re-engage
      - Feral Cat: Stampeding Roar group speed; Skull Bash gap-close
    shield_stance: |
      BEHIND BOSS positioning enforced. Standard rear-arc rotation; full
      uptime. Shield stance is a recovery window for melee (boss rooted;
      group movement minimal).
  cd_use:
    standard_offensive: rotational; align with Tiger's Fury / Berserk / Avenging Wrath
    save_for_sword_stance: highest single-target DPS window; pop CDs early in Sword stance
  heroic_delta: |
    - Disorienting Roar haste-debuff harsher; interrupt high value
    - Mace stance lava trail more pronounced (UNVERIFIED) — kite-path avoidance precision matters
    - Cave In radius unchanged but damage higher
  bot_anti_patterns:
    - DO NOT stay in front of boss during Shield stance (99% damage reduction wastes DPS)
    - DO NOT step into Mace stance kite path (UNVERIFIED lava trail)
    - DO NOT chase tank through kite-path during Mace stance (you are NOT the tank; gap-fill instead)
    - DO NOT stand in Cave In patches
    - DO NOT use Death Grip on Throngus (disrupts tank Mace kite)
```

---

## Boss 3 — Drahga Shadowburner

```yaml
boss_melee:
  boss: drahga_shadowburner
  difficulty_grade: medium (kill-priority swap intensity)
  positioning:
    default_p1: behind Drahga; standard rear-arc melee
    default_p2: |
      Tank holds Valiona facing AWAY from group. Melee in Valiona's REAR
      ARC. ON DEVOURING FLAMES TELEGRAPH — collapse to BEHIND VALIONA
      if not already there. Spread from Seeping Twilight ground patches.
    default_p3: revert to P1 positioning
    facing: boss/valiona back
    movement: |
      Phase 1: stand-still at boss back; gap-close to Invoked Flaming
        Spirit on spawn
      Phase 2: move WITH group behind Valiona on breath cast; spread
        for Seeping Twilight
      Phase 3: stand-still execute
  interrupt_role:
    burning_shadowbolt: ATTEMPT — if class has kick (UNVERIFIED interruptibility)
    devouring_flames: ATTEMPT_BUT_POSITION_PRIMARY — UNVERIFIED interruptibility
  rotation_modifications:
    phase_1: |
      Standard ST rotation on Drahga. SWITCH OFF on Invoked Flaming Spirit
      spawn — top kill priority. Gap-close + slow + burst:
      - Combat Rogue: Sprint + Sap (won't work mid-combat; UNVERIFIED 5-man) + Kidney Shot stun + Eviscerate
      - Fury Warrior: Charge + Hamstring slow + Whirlwind cleave + Execute (if low)
      - Ret Pally: Hammer of Justice stun + Hand of Reckoning taunt (won't pull from tank but slows movement) + Templar's Verdict
      - Frost DK: Death Grip pull spirit AWAY from fixate-target + Chains of Ice slow + Obliterate burst
      - Enh Shaman: Frost Shock slow + Earthbind Totem (drop on spirit path) + Stormstrike + Lava Lash
      - Feral Cat: Skull Bash gap-close + Maim/Pounce stun + Shred burst
    phase_2: |
      Melee Valiona. Move WITH group behind on breath cast. Continue
      spirit kills as they spawn (gap-close priority).
    phase_3: |
      Drahga back on ground. Standard ST execute.
  cd_use:
    save_for_phase_2: |
      Phase 2 is the primary damage window — bloodlust often called.
      Pop offensive CDs at Phase 2 entry.
  heroic_delta: |
    - Spirits hit MUCH harder; gap-close + stun + burst critical
    - Devouring Flames may only exist on heroic (Disagreement #6) — bot conservatively assumes always-on
  bot_anti_patterns:
    - DO NOT continue boss DPS while Invoked Flaming Spirit is alive and approaching its target
    - DO NOT stand in Devouring Flames cone (move BEHIND Valiona)
    - DO NOT stand in Seeping Twilight ground patches
    - DO NOT use Death Grip on Drahga or Valiona (tank position discipline; only on Spirit)
    - DO NOT stand in Valiona's frontal arc (Shredding Swipe cone)
```

---

## Boss 4 — Erudax, the Duke of Below

```yaml
boss_melee:
  boss: erudax_duke_of_below
  difficulty_grade: hard (DPS check + safe-zone reversal + interrupt-heavy)
  positioning:
    default: behind Erudax; standard rear-arc melee
    shadow_gale_override: |
      ON SHADOW GALE TELEGRAPH (~5s) — COLLAPSE TO CENTER SAFE ZONE.
      EXPLICIT REVERSAL of normal "fire on floor = avoid" rule. The center
      circle IS the safe spot. Melee interrupts boss DPS to reposition;
      resumes rotation once stable in safe zone.
    corruptor_response: |
      ON FACELESS CORRUPTOR SPAWN — gap-close + slow + intercept. Melee
      contributes high-priority interrupt on Umbral Mending.
    feeble_body_tank: |
      When tank knocked back by Enfeebling Blow + Feeble Body, melee may
      lose target briefly (boss un-tanked). Continue attacks if still in
      range; otherwise wait for tank to re-engage.
    facing: active_target back (Erudax → Corruptor → Erudax cycle)
    movement: |
      Combine three movement triggers:
      1. Shadow Gale telegraph → MOVE TO CENTER (override default)
      2. Corruptor spawn → gap-close to Corruptor
      3. Erudax target swap (if any) → re-position behind
  interrupt_role:
    umbral_mending_75763: MUST_INTERRUPT — highest priority (Corruptor heal cast)
    twilight_corruption_75520: ATTEMPT — kill priority is canonical counter
    siphon_essence_75755: ATTEMPT — secondary
    binding_shadows_79466: ATTEMPT_IF_KICK_AVAILABLE — positioning is canonical counter
  rotation_modifications:
    erudax_dps: standard ST rotation; interrupt vigilance
    corruptor_burst: |
      ABSOLUTE KILL PRIORITY when Corruptor spawns. Gap-close + slow + interrupt + burst:
      - Combat Rogue: Sprint + Kick (10s) on Umbral Mending + Kidney Shot stun + Eviscerate burst
      - Fury Warrior: Charge + Pummel (10s, off-GCD) on Umbral Mending + Hamstring slow + Whirlwind/Execute
      - Ret Pally: Hammer of Justice stun + Rebuke (15s, off-GCD) on Umbral Mending + Templar's Verdict
      - Frost DK: Death Grip pull Corruptor (UNVERIFIED — may pull off intended path) + Mind Freeze (10s, off-GCD) + Obliterate
      - Enh Shaman: Wind Shear (6s, off-GCD, 25y range) on Umbral Mending — IDEAL CLASS for this fight + Stormstrike + Lava Lash
      - Feral Cat: Skull Bash (60s) on Umbral Mending + Maim stun + Shred burst
    target_swap_on_corruptor_dead: |
      Resume Erudax DPS. Combo-Point classes (Rogue, Feral Cat) RESET CP
      on target swap — re-build on Erudax. Frost DK disease re-application
      via Outbreak.
    heroic_2_corruptor_emergency: |
      On heroic with 2 Corruptors, single-target burst on closest:
      - Combat Rogue: Blade Flurry on (cleaves second Corruptor)
      - Fury Warrior: Whirlwind + Bladestorm if available
      - Ret Pally: Divine Storm at 3 HP on cluster
      - Frost DK: Howling Blast + Death and Decay
      - Enh Shaman: Magma Totem + Chain Lightning Maelstrom dump
      - Feral Cat: Swipe (Cat); but ST burst-down preferred
  cd_use:
    save_for_corruptor_burst: |
      Pop offensive CDs (Berserk, Avenging Wrath, Pillar of Frost,
      Recklessness, Tiger's Fury) on first Corruptor spawn — fastest
      kill prevents egg corruption.
  heroic_delta: |
    - 2 Corruptors per Shadow Gale = doubled DPS-check + doubled interrupt opportunities
    - Shadow Gale damage higher; late entry to safe zone fatal
    - Higher frequency Binding Shadows
  bot_anti_patterns:
    - DO NOT step OUT of Shadow Gale safe zone (REVERSAL of normal fire-avoid rule)
    - DO NOT continue Erudax DPS while Corruptor is alive and approaching egg
    - DO NOT spend Combo Points on Erudax just before Corruptor swap (CP-class CP-reset cost)
    - DO NOT stand in front of Erudax (cleave hazard if melee from front; UNVERIFIED Erudax cleave)
    - DO NOT use Death Grip on Erudax (tank position discipline; situational on Corruptor only)
```

---

## Bot AI hooks

```yaml
ai_hooks:
  umbriss:
    - encounter_state_active → behind_boss_spread_for_blitz
    - encounter_detect_blitz_telegraph_self → stop_attack_move_perpendicular
    - encounter_detect_blitz_telegraph_path → step_aside
    - encounter_detect_ground_siege_cast → move_out_of_cone
    - encounter_detect_trogg_dweller_cluster → swap_to_aoe_rotation
    - encounter_detect_malignant_trogg_alive → stay_in_melee_on_boss (do NOT engage Malignant in melee near boss)

  throngus:
    - encounter_state_stance_sword → standard_st_rotation_full_uptime
    - encounter_state_stance_mace → follow_tank_at_distance_off_kite_path
    - encounter_state_stance_shield → confirm_behind_boss_positioning
    - encounter_detect_cave_in_patch_dropped → move_out_of_radius_6
    - encounter_detect_disorienting_roar_cast → kick_rotation_participate
    - encounter_detect_mighty_stomp_cast → kick_rotation_attempt
    - encounter_detect_impaling_slam_self → reactive_brace

  drahga:
    - encounter_state_phase_1 → standard_rear_arc_melee
    - encounter_detect_invoked_flaming_spirit_spawn → switch_target_gap_close_slow_burst
    - encounter_state_phase_2 → behind_valiona_rear_arc
    - encounter_detect_devouring_flames_cast → move_behind_valiona
    - encounter_detect_seeping_twilight_on_self → move_out_of_patch
    - encounter_state_phase_3 → resume_drahga_st

  erudax:
    - encounter_detect_shadow_gale_telegraph → MOVE_TO_CENTER_SAFE_ZONE_OVERRIDE
    - encounter_state_in_safe_zone → continue_dps_rotation
    - encounter_detect_faceless_corruptor_spawn → gap_close_to_corruptor
    - encounter_detect_umbral_mending_cast → MUST_INTERRUPT_HIGH_PRIORITY
    - encounter_detect_twilight_corruption_channel → BURST_KILL_OR_INTERRUPT
    - encounter_detect_corruptor_dead → resume_erudax_st
    - encounter_detect_self_rooted_by_binding_shadows → continue_attacks_in_range_or_wait
    - encounter_detect_enfeebling_blow_landed_on_tank → confirm_tank_recovery_before_ranged_chase

dispel_blacklist:
  - { spell_id: 79466, name: Binding Shadows, scope: erudax_fight, force: false, notes: melee bots typically do not dispel; included for hybrid melee (Ret Pally Cleanse, Enh Shaman Cleanse Spirit) }

dispel_active:
  - { spell_id: 75809, name: Shield of Nightmares, scope: erudax_fight_corruptor, action: AUTO_DISPEL_IF_HYBRID_HAS_MAGIC_DISPEL, notes: Ret Pally Cleanse / Enh Shaman Cleanse Spirit (UNVERIFIED Magic dispel ability for Enh in 4.3.4) }

interrupt_priority_order:
  erudax:
    1: Umbral Mending (75763)
    2: Twilight Corruption (75520)
    3: Siphon Essence (75755)
    4: Binding Shadows (79466)
  drahga:
    1: Burning Shadowbolt (75245)             # ATTEMPT only
  throngus:
    1: Disorienting Roar (74976)              # ATTEMPT only
    2: Mighty Stomp (74984)                   # ATTEMPT only
  umbriss: []                                  # no kick-required casts
```

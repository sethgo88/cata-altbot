# Grim Batol — Tank

**Tank-specific response logic** for the bot's encounter handling. Mechanics, spell IDs, phase triggers, and add abilities are defined in `encounter.md`; this doc only encodes how the tank reacts. Class-agnostic where possible.

**Layering**:
- Cross-content tank patterns (threat priority, defensive CD pacing, taunt rules) live in `docs/roles/tank.md` (deferred to tank phase).
- Per-spec rotation / CD / threat model lives in the corresponding `docs/specs/` tank doc (all tank specs deferred to tank phase).
- This doc layers Grim Batol-specific deviations on top of those.

---

## Dungeon-level overview

```yaml
dungeon_tank_overview:
  total_bosses: 4
  estimated_full_clear_minutes: 30-45
  threat_profile:
    umbriss: moderate                          # boss + Trogg Dwellers; Malignant Trogg is RANGED-handled (kited)
    throngus: high                             # stance-aware; Mace stance is kite-tank (movement) + Shield stance is positional
    drahga: moderate                           # P1 standard; P2 pivot to Valiona; P3 standard execute
    erudax: moderate                           # boss central; Corruptor adds are DPS-handled; tank holds Erudax through knockback cycle
  defensive_cd_profile:
    umbriss: moderate                          # Bleeding Wound steady drain; Frenzy at 30% major CD
    throngus: heavy                            # Sword stance heavy melee; Mace stance Impaling Slam jumps; Shield stance phase recovery
    drahga: moderate                           # P2 Valiona melee + Shredding Swipe; tank-CD on Phase 2 entry
    erudax: heavy                              # Enfeebling Blow + Feeble Body 200% damage taken — major CD per cycle
  key_responsibilities:
    - threat_priority_per_fight
    - environmental_kite_management (Throngus Mace stance)
    - stance_aware_positioning (Throngus)
    - boss_pivot_on_phase_transition (Drahga P1→P2)
    - feeble_body_wait_out (Erudax — wait 5s before re-engaging)
    - face_away_from_group (boss melee cleave on Drakonid trash, Demolisher trash)
    - shadow_gale_safe_zone_collapse (Erudax — REVERSAL override)
```

---

## Trash — tank notes

```yaml
trash_tank:
  twilight_humanoid_packs:
    pull_pattern: |
      Twilight Cultists / Beguilers / Faceless trash are CC-able (humanoid).
      Default pull pattern: skull on lead caster (kill priority), CC on
      second caster (cross/X markers).
    facing: away from group (frontal cleaves on Drakonids, Demolishers, Drakes)
  trogg_packs:
    notes: AoE-cleave; Skardyn melee with bleeds. Tank picks all up, stand-tank.
  drakonid_packs:
    notes: melee cleave; standard tank-pull-and-rotate-kicks
  twilight_demolisher:
    notes: large mob; frontal cleave; tank face away from group
  mature_twilight_drake:
    notes: |
      BOTH frontal cleave AND tail swipe — tank positions WITH side
      arc clear of group. Group stays at side; melee at side too.
  bombing_run_sequence:
    notes: |
      Drake-rescue scripted event triggers; players can mount drakes and
      bomb large packs. Tank-bot DEFAULT: linear clear; bombing skip is
      v2+ feature. If skip is taken, tank also mounts drake.
  caster_packs_kick_role:
    notes: |
      Tank participates in kick rotation against caster trash:
      - Prot Warrior: Pummel (10s, off-GCD)
      - Prot Pally: Rebuke (15s, off-GCD)
      - Blood DK: Mind Freeze (10s, off-GCD)
      - Feral Tank Druid: Skull Bash (60s, gap-close + interrupt)
```

---

## Boss 1 — General Umbriss

```yaml
boss_tank:
  boss: general_umbriss
  difficulty_grade: medium
  threat_intensity: moderate

  positioning:
    pull: pull boss to chosen tank-spot; ranged spread around perimeter
    facing: away from group (Ground Siege cone discipline)
    movement: minimal; do NOT kite (boss does not require kiting)

  threat_management:
    boss: |
      Standard threat-on-pull (Heroic Throw / Avenger's Shield / Death
      Grip / Faerie Fire ranged-pull). Hold steady throughout.
    trogg_dweller_pickup: |
      On Summon Skardyn cast, 1 Malignant Trogg + 3 Trogg Dwellers spawn.
      Bot tank: Trogg Dwellers come to boss area — AoE threat tools
      (Thunder Clap / Avenger's Shield / Death and Decay / Swipe) to grab
      all Dwellers AT BOSS for cleave AoE.
    malignant_trogg: |
      Tank does NOT pick up Malignant Trogg in melee at boss. RANGED kites
      Malignant Trogg AWAY from boss (Modgud's Malice radius — must die
      far enough that buff doesn't reach Umbriss).
      If ranged kite fails AND Malignant Trogg approaches boss, tank can
      taunt + back-step away from boss to hold the Malignant Trogg in a
      kite spot. Last-resort behavior.

  defensive_cd_use:
    bleeding_wound_drain: standard active-mitigation rotation; rotate Shield Block / Holy Shield / Bone Shield / Savage Defense
    blitz_landed_on_self: tank is rarely the Blitz target (Blitz is non-tank random); if it lands on you in chaos, brace + reactive
    ground_siege_on_self: Ground Siege is tank-included in PBAoE — pop short defensive on cast detection
    frenzy_at_30: pop major defensive (Shield Wall / Survival Instincts / Icebound Fortitude / Last Stand) at 35% HP threshold
    summon_skardyn: short defensive on add-spawn for Trogg Dweller cleave damage

  interrupt_role: []                            # no kick-required casts on Umbriss

  heroic_delta: |
    - Bleeding Wound damage higher; active mitigation discipline matters
    - Blitz one-shots — if tank takes a chained Blitz, defensive CD essential
    - Frenzy at 30% more punishing — major CD on rotation
    - Trogg Dwellers hit harder; threat-tool cleave more important

  bot_anti_patterns:
    - DO NOT bring Malignant Trogg to boss (Modgud's Malice +100% damage stacking)
    - DO NOT chase Malignant Trogg if ranged is kiting it (stay on boss)
    - DO NOT face boss INTO group (Ground Siege cone splash)
    - DO NOT skip active mitigation rotation thinking the fight is easy (Bleeding Wound steady drain compounds)
```

---

## Boss 2 — Forgemaster Throngus

```yaml
boss_tank:
  boss: forgemaster_throngus
  difficulty_grade: hard (stance-aware tank rotation)
  threat_intensity: high

  positioning:
    pull: pull boss to arena center
    facing: away from group; rotate face per stance
    movement: |
      STANCE-AWARE behavior:
      - Sword: stand-tank in place; rotate face to keep tail/cleave away from group
      - Mace: KITE in circular arc around chamber; do NOT stand-tank
      - Shield: hold boss STILL (boss self-roots via Phalanx); face shield-side AWAY from DPS group

  fight_specific_override:
    mace_stance_kite_pattern: |
      DELIBERATE KITE behavior. Tank movement during Mace stance:
      - Boss is slowed -70% (Encumbered 75007 aura) but +200-300% damage
      - Tank cannot stand-tank — would die in seconds
      - Pre-plan a circular kite path along arena perimeter
      - Move at moderate pace; boss follows on lower speed
      - DPS gap-fills; healer follows at safe distance
      - DO NOT KITE INTO CAVE IN PATCHES (universal mechanic still active)
      - UNVERIFIED lava trail along boss path — conservative default avoid

    shield_stance_hold_still: |
      Boss self-roots and self-pacifies during Shield stance (Personal
      Phalanx 74908 aura). Tank simply HOLDS boss in place; does not need
      to actively tank-position. DPS positioning is the active concern
      (everyone behind shield). Tank face-side away from DPS group.

  threat_management:
    sword_stance: standard threat rotation; full uptime
    mace_stance: |
      Threat is NOT the bottleneck during Mace stance — boss does not
      attack tank directly during kite (per per-source descriptions);
      threat-tools used opportunistically (Shield of Righteousness, Death
      Strike, etc.). Maintain threat lead pre-stance.
    shield_stance: |
      Boss is rooted/pacified — threat moot during this stance. Recovery
      window for Sword stance threat lead.

  defensive_cd_use:
    sword_stance: |
      HEAVIEST damage stance. Active mitigation rotation high priority
      (Shield Block / Holy Shield / Bone Shield / Savage Defense).
      Major CD if Disorienting Roar lands AND damage spikes (Shield Wall
      / Survival Instincts / Icebound Fortitude / Last Stand).
    mace_stance: |
      Tank is moving, not stand-tanking — defensive CDs less critical.
      BUT: if Impaling Slam jumps to tank during kite, brace.
    shield_stance: |
      RECOVERY WINDOW. Boss is rooted; tank takes minimal damage. Reset
      defensive CD timers; regen mana/runic-power/rage as appropriate.

  interrupt_role:
    disorienting_roar: ATTEMPT — kick on cooldown if available
    mighty_stomp: ATTEMPT — kick if available

  heroic_delta: |
    - Mace stance damage especially severe; tank cannot stand-tank
    - Disorienting Roar may stack on heroic
    - Flaming Arrow patches hit harder on Shield stance
    - Lava trail UNVERIFIED but conservative kite-path planning matters

  bot_anti_patterns:
    - DO NOT stand-tank in Mace stance (you die)
    - DO NOT kite in Sword stance (DPS loss; positioning chaos)
    - DO NOT face boss INTO group during Sword stance (cleave / cone risk)
    - DO NOT face shield TOWARD group during Shield stance (99% reduction wastes group DPS)
    - DO NOT kite INTO Cave In patches (universal mechanic still active)
```

---

## Boss 3 — Drahga Shadowburner

```yaml
boss_tank:
  boss: drahga_shadowburner
  difficulty_grade: medium (phase-pivot tank behavior)
  threat_intensity: moderate

  positioning:
    pull_p1: pull Drahga to chosen tank-spot at his perch / ground area
    p2_pivot: |
      At Drahga HP == 25%, Drahga MOUNTS VALIONA. Tank PIVOTS to Valiona
      (now the threat target). Hold Valiona facing AWAY from group
      (Shredding Swipe cone) AND with open space behind for group to
      collapse into during Devouring Flames cast.
    p3_pivot: |
      At Valiona HP ~20%, Valiona departs; Drahga back on ground. Tank
      pivots BACK to Drahga; standard execute.
    facing: away from group (P1 + P3 Drahga; P2 Valiona)
    movement: minimal (Phase 1 + Phase 3); pivot-to-Valiona at Phase 2 entry

  threat_management:
    drahga_p1: standard threat on pull; ranged-DPS-friendly hold
    valiona_p2_pivot: |
      Valiona spawns / descends with own threat table. Tank applies threat
      via taunt + opener-rotation on Valiona at Phase 2 entry.
      DO NOT continue Drahga DPS — Drahga is mounted on Valiona and shares
      her threat table during Phase 2.
    invoked_flaming_spirit: |
      Spirit is RANGED/MELEE-DPS-handled (slow + burst). Tank does NOT
      pick up spirit. If spirit misroutes onto tank, tolerate — tank can
      survive Supernova on a defensive CD; spirits should still die fast.

  defensive_cd_use:
    p1: standard active-mitigation rotation; reactive on Burning Shadowbolt if tank-targeted
    p2_entry: pop major defensive (Shield Wall / Survival Instincts) on Phase 2 pivot — Valiona melee + Shredding Swipe is the spike window
    p2_devouring_flames: |
      If tank is in Valiona's frontal arc when Devouring Flames casts (rare
      — tank holds her facing away), pop major defensive. Otherwise tank
      stays in side/rear arc per anchor.
    p3: standard execute; reset CDs if available

  interrupt_role:
    burning_shadowbolt: ATTEMPT — if class has kick

  heroic_delta: |
    - Spirits hit much harder; tank-survivability gear matters more
    - Valiona melee + Shredding Swipe damage higher in Phase 2
    - Devouring Flames may only exist on heroic (Disagreement #6)

  bot_anti_patterns:
    - DO NOT pick up Invoked Flaming Spirit (it fixates a non-tank; intercepting wastes tank positioning)
    - DO NOT face Valiona INTO group during P2 (Shredding Swipe cone)
    - DO NOT stand in Seeping Twilight ground patches
    - DO NOT continue tanking Drahga at Phase 2 entry (he's now mounted on Valiona — pivot to Valiona)
    - DO NOT chase Valiona during Phase 3 transition (she departs; Drahga returns to ground)
```

---

## Boss 4 — Erudax, the Duke of Below

```yaml
boss_tank:
  boss: erudax_duke_of_below
  difficulty_grade: hard (knockback cycle + safe-zone reversal + add-management)
  threat_intensity: moderate

  positioning:
    pull: tank holds Erudax in CENTER OF ROOM
    facing: away from group (standard)
    movement: |
      Minimal during normal fight. Two scripted movements:
      1. Enfeebling Blow: tank knocked back ~400 distance; tank waits 5s
         (Feeble Body 200% damage taken) before re-engaging
      2. Shadow Gale: tank COLLAPSES TO CENTER SAFE ZONE with group
         (REVERSAL — center IS safe, outer zone is hostile)

  fight_specific_override:
    feeble_body_wait_out: |
      DELIBERATE WAIT behavior. After Enfeebling Blow knocks tank back:
      - Tank is now ~400 distance units from Erudax
      - Feeble Body debuff applies: +200% damage taken for 5s
      - Re-engaging during the debuff is suicidal — boss melee 3x normal
      - Bot tank: WAIT OUT 5s (or until Feeble Body falls off), THEN
        return to boss
      - Healer pre-shields if available (PW:S etc.)
      - DPS may pull threat during the wait — tank reclaims via taunt
        on return

    shadow_gale_safe_zone: |
      DELIBERATE STAND-IN behavior. Tank moves with group to center:
      - Default rule: "fire on floor = avoid" — REVERSED here
      - Center safe zone is the ONLY survival spot
      - Tank brings Erudax with him (he follows tank threat)
      - DO NOT attempt to "tank Erudax in outer zone while group is in
        center" — boss must be in safe zone with tank
      - Cast resolves; resume normal positioning post-cast

  threat_management:
    erudax: standard threat-on-pull; full uptime
    faceless_corruptor_spawn: |
      Tank does NOT pick up Faceless Corruptor in standard strat. DPS
      handles corruptor. Tank stays on Erudax.
      EXCEPTION: if Corruptor reaches melee range of tank's position (e.g.,
      tank dragged Erudax to room entrance during Shadow Gale collapse and
      Corruptor walked over), tank may incidentally hold corruptor; DPS
      still bursts corruptor as primary target.
    twilight_hatchlings: |
      Spawned from corrupted eggs (only if egg-corruption completes). Tank
      can taunt + AoE-grab if hatchlings are clustered near boss. Otherwise
      DPS-handled.

  defensive_cd_use:
    enfeebling_blow_recovery: |
      Major defensive on Feeble Body application (Shield Wall / Survival
      Instincts / Icebound Fortitude / Last Stand) — IF tank cannot wait
      out 5s due to incoming damage from other sources.
      OR: rely on the wait-out and only reactive-CD if surprise damage hits.
    shadow_gale_collapse: short defensive on Shadow Gale tick if AoE damage spikes during cast
    corruptor_cleave: minor — corruptors are DPS-handled; tank uses CDs reactive only if drawn off
    binding_shadows_root: |
      If tank is rooted by Binding Shadows (rare; usually targets non-tank
      ranged), tank waits out the 8s root; healer reactive heals.
      Tanks generally NOT primary Binding Shadows targets per most sources.

  interrupt_role:
    umbral_mending_75763: MUST_INTERRUPT — if in range (tanks usually NOT in range; ranged/melee primary kicker)
    twilight_corruption_75520: ATTEMPT — usually out of tank range (corruptor is at egg ring)
    binding_shadows_79466: ATTEMPT_IF_KICK_AVAILABLE

  heroic_delta: |
    - 2 Faceless Corruptors per Shadow Gale — DPS check tighter, but tank role unchanged
    - Shadow Gale damage higher; collapse-to-center timing tighter
    - Enfeebling Blow knockback farther / more frequent
    - More mana drain on healer overall — tank active mitigation discipline matters

  bot_anti_patterns:
    - DO NOT step OUT of Shadow Gale safe zone (REVERSAL of normal fire-avoid rule)
    - DO NOT re-engage Erudax immediately after Enfeebling Blow knockback (Feeble Body 200% damage taken — wait 5s)
    - DO NOT chase Faceless Corruptor (DPS-handled; tank stays on Erudax)
    - DO NOT face Erudax INTO group (cleave UNVERIFIED but conservative)
    - DO NOT pull Erudax to room entrance / outer ring (eggs are there; positions Corruptor cleave)
```

---

## Bot AI hooks

```yaml
ai_hooks:
  umbriss:
    - encounter_state_active → standard_threat_central_facing_away
    - encounter_detect_summon_skardyn → aoe_threat_pickup_trogg_dwellers
    - encounter_state_malignant_trogg_alive AND ranged_kiting → stay_on_boss
    - encounter_state_malignant_trogg_at_melee_unhandled → taunt_kite_away_last_resort
    - encounter_detect_ground_siege_cast → pop_defensive_cd_short
    - encounter_detect_frenzy_at_30 → pop_major_defensive_cd

  throngus:
    - encounter_state_stance_sword → stand_tank_active_mitigation_rotation
    - encounter_state_stance_mace → KITE_IN_CIRCULAR_ARC_AVOID_LAVA_TRAIL
    - encounter_state_stance_shield → hold_still_face_shield_away_from_group_recovery_window
    - encounter_detect_cave_in_patch_dropped → reposition_kite_path_around_patch
    - encounter_detect_disorienting_roar_cast → kick_rotation_attempt
    - encounter_detect_impaling_slam_self → pop_short_defensive_cd

  drahga:
    - encounter_state_phase_1 → stand_tank_drahga_central
    - encounter_detect_phase_2_trigger → PIVOT_TO_VALIONA_TAUNT_HOLD_FACING_AWAY
    - encounter_state_phase_2 → anchor_for_behind_valiona_collapse_zone
    - encounter_detect_devouring_flames_cast → pop_defensive_cd_if_in_arc
    - encounter_detect_phase_3_trigger → PIVOT_BACK_TO_DRAHGA_GROUND_EXECUTE
    - encounter_state_invoked_flaming_spirit_misrouted_onto_tank → pop_short_defensive_tolerate

  erudax:
    - encounter_state_active → hold_erudax_central_face_away
    - encounter_detect_enfeebling_blow_landed → WAIT_OUT_5S_FEEBLE_BODY_DEBUFF_BEFORE_RE_ENGAGE
    - encounter_detect_shadow_gale_telegraph → COLLAPSE_TO_CENTER_SAFE_ZONE_WITH_BOSS
    - encounter_state_in_safe_zone → maintain_threat_continue_rotation
    - encounter_detect_faceless_corruptor_spawn → STAY_ON_ERUDAX (DPS handles corruptor)
    - encounter_detect_twilight_hatchling_spawn → conditional_taunt_aoe_grab_if_clustered
    - encounter_detect_self_rooted_by_binding_shadows → wait_out_root_continue_attacks_if_in_range

dispel_blacklist:
  - { spell_id: 79466, name: Binding Shadows, scope: erudax_fight, force: false, notes: tanks typically do not dispel; included for hybrid tank (Prot Pally Cleanse) }

defensive_cd_priority_per_fight:
  umbriss:    [reactive_on_blitz_self; major_at_30_pct_frenzy]
  throngus:   [active_mitigation_rotation_sword; major_on_disorienting_roar_lands; recovery_during_shield]
  drahga:     [reactive_p1; major_on_p2_entry_pivot_to_valiona; reactive_p3_execute]
  erudax:     [major_on_enfeebling_blow_or_wait_out_5s; short_on_shadow_gale_tick_spike]

stance_detection_throngus:
  method: aura_on_boss
  stance_aura_ids:
    sword: UNVERIFIED                          # flag for DBC
    mace: 75007                                # Encumbered
    shield: 74908                              # Personal Phalanx
  cycle_duration_seconds: 30                   # UNVERIFIED — community-listed
  rotation_order: random
  bot_response_per_stance:
    sword: stand_tank_active_mitigation
    mace: kite_circular_avoid_lava
    shield: hold_still_recovery
```

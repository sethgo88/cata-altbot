# Grim Batol — Ranged DPS

**Ranged-DPS-specific response logic** for the bot's encounter handling. Mechanics, spell IDs, phase triggers, and add abilities are defined in `encounter.md`; this doc only encodes how ranged DPS reacts. Class-agnostic where possible.

**Layering**:
- Cross-content ranged DPS patterns (positioning, spread/stack rules, threat ceiling) live in `docs/roles/ranged-dps.md`
- Per-spec rotation lives in the active spec's `docs/specs/` doc (frost-mage, affliction-warlock, mm-hunter, shadow-priest, balance-druid, elemental-shaman)
- This doc layers Grim Batol-specific deviations on top of those

---

## Dungeon-level overview

```yaml
dungeon_ranged_dps_overview:
  total_bosses: 4
  estimated_full_clear_minutes: 30-45
  positioning_profile:
    umbriss: max_range_spread                    # standard ranged spread for Blitz multi-hit prevention
    throngus: max_range_stance_aware             # spread for Cave In; behind boss during Shield stance
    drahga: max_range_p1_then_behind_valiona_p2  # P2 Devouring Flames cone requires "behind Valiona" override
    erudax: max_range_spread_collapse_on_gale    # SHADOW GALE STAND-IN OVERRIDE — collapse to center safe zone
  interrupt_load:
    umbriss: zero                                # no kick-required casts
    throngus: low                                # Disorienting Roar attempt; UNVERIFIED interruptibility
    drahga: low                                  # Burning Shadowbolt attempt; UNVERIFIED
    erudax: high                                 # MUST INTERRUPT Umbral Mending on Faceless Corruptor
  key_responsibilities:
    - kite_malignant_trogg_away_from_boss (umbriss)
    - cave_in_patch_avoidance + behind_boss_shield_stance (throngus)
    - kill_invoked_flaming_spirit_priority + behind_valiona_cone (drahga)
    - shadow_gale_safe_zone_collapse + corruptor_burst_priority + umbral_mending_interrupt (erudax)
```

---

## Trash — ranged DPS notes

```yaml
trash_ranged:
  twilight_humanoid_casters:
    cc_priorities: |
      Ranged contributes opener-CC on Twilight Cultists / Beguilers / Faceless trash:
      - Frost Mage: Polymorph (Sheep) — primary multi-CC
      - Affliction Warlock: Banish for Drakonid (UNVERIFIED — likely not flagged Demon); Fear via Felhunter (limited 5-man use)
      - MM Hunter: Freezing Trap (Trap Launcher in Cata)
      - Shadow Priest: Shackle Undead (if undead-flagged); Mind Control (rare)
      - Balance Druid: Cyclone (8s) / Hibernate (Twilight Drakes)
      - Elemental Shaman: Hex (frog 60s, breaks on damage)
  caster_packs:
    interrupt_role: |
      - Frost Mage: Counterspell (24s) / Frost Nova root for kiting
      - Affliction Warlock: Spell Lock via Felhunter pet
      - MM Hunter: Silencing Shot (24s)
      - Shadow Priest: Silence (45s)
      - Balance Druid: Solar Beam (60s; 8s AoE silence)
      - Elemental Shaman: Wind Shear (6s; off-GCD; 25y range)

  trogg_packs:
    notes: AoE-burnable; no significant ranged-specific concerns
  drakonid_packs:
    notes: tank face-aware; standard ST cleave from rear
  mature_twilight_drake:
    notes: |
      Both frontal cleave AND tail swipe — ranged stays at MAX RANGE side
      arc, not directly behind (tail) or in front (cleave).
  bombing_run_sequence:
    notes: |
      If group uses bombing skip, ranged is mounted on a drake; skip
      automation is v2+ feature. Default: linear clear.
```

---

## Boss 1 — General Umbriss

```yaml
boss_ranged:
  boss: general_umbriss
  difficulty_grade: medium (kite-add responsibility)
  positioning:
    default: |
      Max range; SPREAD around Umbriss so Blitz doesn't multi-hit. Stay
      out of Ground Siege cone telegraph.
    facing: boss
    movement: |
      ON BLITZ TELEGRAPH (~3s warning) — if you are charge target OR in
      path between boss and target, MOVE PERPENDICULAR.
      ON GROUND SIEGE TELEGRAPH (2s cast) — move out of cone arc.
  interrupt_role:
    no_kick_required: true
  rotation_modifications:
    standard: |
      Standard ST rotation on Umbriss. Switch to Malignant Trogg on add
      spawn:
      - Frost Mage: Frost Nova root + Frostbolt slow on Malignant Trogg
      - Aff Warlock: Curse of Exhaustion (slow) + DoT spread; Banish if flagged Demon (UNVERIFIED)
      - MM Hunter: Concussive Shot slow + Freezing Trap (CC)
      - Shadow Priest: Mind Flay slow + reapply DoTs
      - Balance Druid: Entangling Roots; Faerie Fire for armor debuff
      - Ele Shaman: Earthbind Totem slow; Hex (frog 60s)
    add_handling: |
      MALIGNANT TROGG kill priority — ranged primary slows + bursts
      Malignant Trogg AWAY FROM BOSS. Trogg Dwellers are AoE'd by tank;
      ranged contributes cleave only if Malignant Trogg is dead or kited.
    blitz_response: |
      If self is Blitz target → STOP CAST → MOVE perpendicular →
      RESUME ROTATION.
  cd_use:
    standard_offensive: rotational; align with Bloodlust if pre-pull
  heroic_delta: |
    - Blitz one-shots — pristine reaction time required
    - Malignant Trogg hits harder; slow-kite more critical
  bot_anti_patterns:
    - DO NOT cast Malignant Trogg debuffs/DoTs near boss (Modgud's Malice radius UNVERIFIED but assume large)
    - DO NOT cluster ranged together (Blitz multi-hit hazard)
    - DO NOT continue cast through Blitz telegraph if self is target
    - DO NOT AoE the Trogg cluster if Malignant Trogg is in cluster (kill priority differentiation required)
```

---

## Boss 2 — Forgemaster Throngus

```yaml
boss_ranged:
  boss: forgemaster_throngus
  difficulty_grade: medium (stance-aware positioning)
  positioning:
    default: max range; SPREAD for Cave In patches
    facing: boss
    stance_overrides:
      sword: |
        Standard max range; spread; standard rotation. No special positioning.
      mace: |
        Stay OFF tank kite path (UNVERIFIED lava trail; conservative avoid).
        Continue ranged DPS from perimeter.
      shield: |
        BEHIND BOSS — Phalanx provides 99% damage reduction from front.
        Ranged must be in rear arc. Ranged is the easier role on Shield
        stance (movement-tolerant cast classes vs melee).
        Out of Flaming Arrow patches.
    movement: |
      ON MIGHTY STOMP CAST (2s) — pre-position to NOT be where Cave In
      patch will likely drop (random location, but spread reduces clustering).
      ON CAVE IN PATCH ON SELF — move out (6-yard radius).
  interrupt_role:
    disorienting_roar: ATTEMPT — kick on cooldown if available
    mighty_stomp: ATTEMPT — kick if available (UNVERIFIED interruptibility)
  rotation_modifications:
    sword_stance: standard ST rotation; cast-while-moving classes preferred for Cave In avoidance
    mace_stance: |
      Tank is kiting; boss moves. Ranged stays at distance:
      - Frost Mage: Frostbolt + Frostfire from max range; Ice Lance instant on Fingers proc
      - Aff Warlock: DoT-spam continues; Drain Soul channel during stable kite stretches
      - MM Hunter: Steady Shot / Aimed Shot; Hunter is best class for moving boss
      - Shadow Priest: Mind Flay channels mobility-friendly
      - Balance Druid: Wrath/Starfire hardcast; Moonfire/Sunfire instant on move
      - Ele Shaman: LB/LvB hardcast; Flame Shock instant on move
    shield_stance: |
      BEHIND BOSS positioning override. ALL ranged spells from rear arc
      regardless of normal kit positioning. Continue rotation on boss back.
  cd_use:
    standard_offensive: rotational; bloodlust pre-pull or saved per group call
  heroic_delta: |
    - Disorienting Roar haste-debuff harsher on heroic — interrupt is high value
    - Mace stance lava trail more pronounced (UNVERIFIED) — kite-path avoidance precision matters
  bot_anti_patterns:
    - DO NOT cast IN front of boss during Shield stance (99% damage reduction wastes DPS)
    - DO NOT stand IN Mace stance kite path (UNVERIFIED lava trail)
    - DO NOT stand IN Cave In patches
    - DO NOT cast knockback abilities (Thunderstorm Ele Shaman, Typhoon Boomkin) — disrupts tank kite on Mace
```

---

## Boss 3 — Drahga Shadowburner

```yaml
boss_ranged:
  boss: drahga_shadowburner
  difficulty_grade: medium (kill-priority-swap intensity)
  positioning:
    default_p1: max range; SPREAD for Invocation of Flame portal RNG
    default_p2: |
      Tank holds Valiona facing AWAY from group. Ranged at max range, in
      her rear arc or side. ON DEVOURING FLAMES TELEGRAPH — collapse to
      BEHIND VALIONA (override default ranged-spread). Stay out of Seeping
      Twilight ground patches.
    default_p3: revert to P1 positioning
    facing: boss/valiona
    movement: |
      Phase 1: stand-still or minor reposition on portal RNG
      Phase 2: move WITH group behind Valiona on breath cast; spread for
        Seeping Twilight patches
      Phase 3: stand-still execute
  interrupt_role:
    burning_shadowbolt: ATTEMPT — if class has kick (UNVERIFIED interruptibility)
    devouring_flames: ATTEMPT_BUT_POSITION_PRIMARY — UNVERIFIED interruptibility
  rotation_modifications:
    phase_1: |
      Standard ST rotation on Drahga. SWITCH OFF on Invoked Flaming Spirit
      spawn — top kill priority. Slows + burst:
      - Frost Mage: Frostbolt slow (innate) + Frost Nova root + Ice Lance burst on rooted spirit
      - Aff Warlock: Curse of Exhaustion + Shadowfury (talented stun) + DoT-spread
      - MM Hunter: Concussive Shot slow + Freezing Trap CC + Aimed Shot burst
      - Shadow Priest: Mind Flay slow + Mind Blast burst + Shadow Word: Death execute
      - Balance Druid: Entangling Roots + Starsurge nuke + Moonfire/Sunfire instant
      - Ele Shaman: Earthbind Totem + Frost Shock slow + Lava Burst nuke + Hex (60s CC)
    phase_2: |
      Switch to Valiona for damage; continue spirit kills as they spawn.
      MOVE BEHIND on Devouring Flames cast detection — stop-cast hardcast
      classes during reposition. Resume rotation once stable.
    phase_3: |
      Drahga back on ground; standard ST execute. Spirits may still spawn.
  cd_use:
    save_for_phase_2: |
      Phase 2 is the primary damage window — bloodlust often called here.
      Pop offensive CDs (Trinket procs, Combustion-Mage if leveling, Tide of
      Light) at Phase 2 entry.
  heroic_delta: |
    - Spirits hit MUCH harder; slow + burst critical to prevent fixate-target one-shot
    - Devouring Flames may only exist on heroic (Disagreement #6) — bot conservatively assumes always-on
    - More frequent spirit spawns — multiple in flight simultaneously possible
  bot_anti_patterns:
    - DO NOT continue boss DPS while Invoked Flaming Spirit is alive and approaching its target
    - DO NOT stand in Devouring Flames cone (move BEHIND Valiona)
    - DO NOT stand in Seeping Twilight ground patches
    - DO NOT cluster ranged together in P2 (Seeping Twilight target overlap)
```

---

## Boss 4 — Erudax, the Duke of Below

```yaml
boss_ranged:
  boss: erudax_duke_of_below
  difficulty_grade: hard (DPS check + kill-priority swap + safe-zone reversal)
  positioning:
    default: max range; SPREAD around Erudax (Binding Shadows targeting)
    shadow_gale_override: |
      ON SHADOW GALE TELEGRAPH (~5s) — COLLAPSE TO CENTER SAFE ZONE.
      EXPLICIT REVERSAL of normal "fire on floor = avoid" rule. The center
      circle IS the safe spot. STAY IN until cast resolves.
    corruptor_response: |
      ON FACELESS CORRUPTOR SPAWN — switch to Corruptor; intercept BEFORE
      it reaches Alexstrasza's Eggs at outer ring. Slow + burst.
    binding_shadows_response: |
      MOVE OUT of telegraphed Binding Shadows zone. If rooted, continue
      DPS from rooted position (root resolves on duration).
    facing: active_target (Erudax → Corruptor → Erudax cycle)
    movement: |
      Combine three movement triggers:
      1. Shadow Gale telegraph → MOVE TO CENTER
      2. Binding Shadows targeting → MOVE OUT
      3. Corruptor spawn → MOVE TOWARD Corruptor for slow application
  interrupt_role:
    umbral_mending_75763: MUST_INTERRUPT — highest priority (Corruptor heal cast)
    twilight_corruption_75520: ATTEMPT — kill priority is canonical counter
    siphon_essence_75755: ATTEMPT — secondary
    binding_shadows_79466: ATTEMPT_IF_KICK_AVAILABLE — positioning is canonical counter
  rotation_modifications:
    erudax_dps: standard ST rotation; respect Binding Shadows positioning
    corruptor_burst: |
      ABSOLUTE KILL PRIORITY when Corruptor spawns. Slow + burst:
      - Frost Mage: Frostbolt slow (innate) + Counterspell on Umbral Mending + Ice Lance burst
      - Aff Warlock: Curse of Exhaustion + Spell Lock (Felhunter) on Umbral Mending + Haunt + UA + Bane of Doom
      - MM Hunter: Concussive Shot slow + Silencing Shot on Umbral Mending + Aimed Shot burst
      - Shadow Priest: Mind Flay slow + Silence on Umbral Mending + Mind Blast + Shadow Word: Death
      - Balance Druid: Entangling Roots + Solar Beam silence on Umbral Mending + Starsurge nuke
      - Ele Shaman: Earthbind Totem + Wind Shear on Umbral Mending + Lava Burst nuke
    shield_of_nightmares_dispel: |
      If your spec has Magic dispel, contribute:
      - Balance Druid: Remove Corruption (Magic + Curse with talent) — UNVERIFIED Cata
      - Shadow Priest: Dispel Magic (offensive — removes 1 buff from enemy)
      - Aff Warlock: Felhunter Devour Magic (off-target buff steal)
      Healer is primary dispeller; ranged contributes if available.
    target_swap_on_corruptor_dead: |
      Resume Erudax DPS. Aff Warlock + Shadow Priest DoT-RESET on swap —
      re-apply DoT suite. Hunter Mark resets too.
  cd_use:
    save_for_corruptor_burst: |
      Pop offensive CDs (trinket procs, Bloodlust if first cycle, etc.) on
      first Corruptor spawn — fastest kill prevents egg corruption.
      Subsequent corruptor cycles may not have CDs available; positional
      slow + burst still works.
    heroic_2_corruptor_emergency: |
      On heroic with 2 Corruptors, AoE is appropriate:
      - Frost Mage: Blizzard cluster + Cone of Cold root
      - Aff Warlock: Seed of Corruption seed-spread
      - MM Hunter: Multi-Shot cleave + Trap Launcher Explosive Trap
      - Shadow Priest: Mind Sear (poor on 2 targets); single-target Mind Blast cycle preferred
      - Balance Druid: Hurricane channel + Starfall (toggle off for ST)
      - Ele Shaman: Chain Lightning + Magma Totem (limited for 2 targets)
  heroic_delta: |
    - 2 Corruptors per Shadow Gale = doubled DPS-check + doubled Umbral Mending interrupt opportunities
    - Shadow Gale damage higher — late entry to safe zone fatal
    - Higher frequency Binding Shadows
  bot_anti_patterns:
    - DO NOT step OUT of Shadow Gale safe zone (REVERSAL of normal fire-avoid rule)
    - DO NOT auto-dispel Binding Shadows (root resolves on duration; mana waste)
    - DO NOT continue Erudax DPS while Corruptor is alive and approaching egg
    - DO NOT cluster ranged (Binding Shadows multi-target hazard)
    - DO NOT cast knockback (Thunderstorm Ele, Typhoon Boomkin) — disrupts safe zone collapse
```

---

## Bot AI hooks

```yaml
ai_hooks:
  umbriss:
    - encounter_state_active → max_range_spread
    - encounter_detect_blitz_telegraph_self → stop_cast_move_perpendicular
    - encounter_detect_blitz_telegraph_path → step_aside
    - encounter_detect_ground_siege_cast → move_out_of_cone
    - encounter_detect_malignant_trogg_spawn → switch_to_trogg_apply_slow_burst
    - encounter_state_malignant_trogg_alive → confirm_distance_from_boss_before_cast

  throngus:
    - encounter_state_stance_sword → standard_st_rotation
    - encounter_state_stance_mace → stay_off_kite_path
    - encounter_state_stance_shield → reposition_behind_boss
    - encounter_detect_cave_in_patch_dropped → move_out_of_radius_6
    - encounter_detect_disorienting_roar_cast → kick_rotation_participate

  drahga:
    - encounter_state_phase_1 → max_range_spread
    - encounter_detect_invoked_flaming_spirit_spawn → switch_target_apply_slow_burst
    - encounter_state_phase_2 → max_range_behind_valiona
    - encounter_detect_devouring_flames_cast → move_behind_valiona_stop_cast
    - encounter_detect_seeping_twilight_on_self → move_out_of_patch
    - encounter_state_phase_3 → resume_drahga_st

  erudax:
    - encounter_detect_shadow_gale_telegraph → MOVE_TO_CENTER_SAFE_ZONE_OVERRIDE
    - encounter_state_in_safe_zone → continue_dps_rotation
    - encounter_detect_binding_shadows_targeting_self → move_out_of_telegraph
    - encounter_detect_faceless_corruptor_spawn → switch_to_corruptor_apply_slow
    - encounter_detect_umbral_mending_cast → MUST_INTERRUPT_HIGH_PRIORITY
    - encounter_detect_twilight_corruption_channel → ATTEMPT_INTERRUPT_OR_BURST_KILL
    - encounter_detect_shield_of_nightmares_aura → DISPEL_IF_HYBRID_HAS_MAGIC_DISPEL
    - encounter_state_corruptor_dead → resume_erudax_st
    - encounter_state_self_rooted_by_binding_shadows → continue_dps_from_rooted_position

dispel_blacklist:
  - { spell_id: 79466, name: Binding Shadows, scope: erudax_fight, force: false, notes: REACTIVE_ONLY for ranged hybrids }

dispel_active:
  - { spell_id: 75809, name: Shield of Nightmares, scope: erudax_fight_corruptor, action: AUTO_DISPEL_IF_HYBRID_HAS_MAGIC_DISPEL }

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

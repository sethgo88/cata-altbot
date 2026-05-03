# Halls of Origination — Melee DPS

**Melee-DPS-specific response logic** for the bot's encounter handling. Mechanics, spell IDs, phase triggers, and add abilities are defined in `encounter.md`; this doc only encodes how melee DPS reacts. Class-agnostic where possible.

**Layering**:
- Cross-content melee DPS patterns (positioning, interrupt rotation, threat ceiling) live in `docs/roles/melee-dps.md`
- Per-spec rotation lives in the active spec's `docs/specs/` doc (combat-rogue, fury-warrior, ret-paladin, frost-dk, enh-shaman, feral-cat)
- This doc layers HoO-specific deviations on top of those

---

## Dungeon-level overview

```yaml
dungeon_melee_dps_overview:
  total_bosses: 7
  estimated_full_clear_minutes: 35-55     # LONGEST Cata 5-man
  positioning_profile:
    anhuur: pit_runner_OR_topside_melee   # 1 of 2 designated pit-runners; OR melee Anhuur topside
    ptah: behind_outside_quicksand        # standard rear; out of cleave + Quicksand
    anraphet: behind_dynamic              # standard rear; heroic void zones force re-position
    setesh: portal_priority_mobile        # active-target rear; target-swap heavy
    ammunae: behind_outside_clouds        # standard rear; out of Spore clouds
    isiset: behind_face_away_supernova    # standard rear, FACE AWAY on Supernova
    rajh: behind_kite_winds               # standard rear; kite Solar Winds
  interrupt_load:
    anhuur: zero (Hymn interrupt-immune; Reckoning instant)
    ptah: medium (Flame Bolt 5s channel — opportunistic kick)
    anraphet: zero (no kick targets on boss; Stone Trogg gauntlet kicks rotation)
    setesh: HIGH (Anti-Magic Prison ABSOLUTE)
    ammunae: HIGH (Consume Life Energy ABSOLUTE; Wither secondary)
    isiset: low (Arcane Barrage rotation kick)
    rajh: HIGH (Sun Orb + Inferno Leap rotation kicks)
  key_responsibilities:
    - anhuur_pit_runner_assignment (1 of 2 designated)
    - target_swap_on_portals (setesh)
    - kick_rotation_dense (rajh, ammunae)
    - face_away_on_supernova (isiset)
    - cooldown_alignment_with_blessing (rajh burn window)
    - melee_kicks_at_close_range (Wind Shear, Pummel, Skull Bash, Mind Freeze, Rebuke, Kick)
```

---

## Trash — melee DPS notes

```yaml
trash_melee:
  pre_anhuur_anubisath:
    cc_priorities: |
      Melee bot contributes opener-CC where class allows:
      - Combat Rogue: Sap (humanoid CC, opener)
      - Ret Paladin: Hammer of Justice (6s stun)
      - Frost DK: Hungering Cold (10s freeze; talent)
      - Feral Cat: Maim (CC in form; CP-cost)
    aoe_role: standard ST + cleave on humanoid packs

  vault_gauntlet_stone_troggs:
    aoe_priority: |
      AoE on trogg cluster near tank:
      - Combat Rogue: Blade Flurry on; Fan of Knives if 3+ adds
      - Fury Warrior: Whirlwind / Bladestorm on cluster
      - Ret Paladin: Divine Storm at 3 HP; Consecration on stack
      - Frost DK: Howling Blast spam on disease-applied pack
      - Enh Shaman: Magma Totem + Chain Lightning at 5 MW
      - Feral Cat: Swipe (Cat) + maintain primary target Rip
    interrupt_role: |
      Stone Trogg caster types — kick rotation:
      - Combat Rogue: Kick (10s; off-GCD)
      - Fury Warrior: Pummel (10s; off-GCD)
      - Ret Paladin: Rebuke (15s; off-GCD)
      - Frost DK: Mind Freeze (10s; off-GCD)
      - Enh Shaman: Wind Shear (6s; off-GCD; 25y range)
      - Feral Cat: Skull Bash (60s; gap-close + interrupt)

  setesh_halls_shadowlancers:
    cc_priorities: |
      Shadowlancers (humanoid) — Sap by Combat Rogue is primary CC opener.
    interrupt_role: |
      Caster trash; kick rotation. PRIMARY KICK on trash version of Anti-Magic
      Prison (Void Seeker mini-add).

  construct_hall_caster_trash:
    interrupt_role: kick rotation; positioning side/rear
```

---

## Boss 1 — Temple Guardian Anhuur

```yaml
boss_melee:
  boss: temple_guardian_anhuur
  difficulty_grade: medium                # door event coordination
  positioning:
    role: |
      1 of 2 DESIGNATED PIT-RUNNERS (default — bot composition picks 2 DPS).
      If assigned: drop into east OR west pit on Shield of Light cast,
      kill Pit Vipers, channel Beacon of Light (8s normal / 10s heroic).
      If NOT assigned: stay topside, melee Anhuur, continue boss DPS.
    facing: any (no boss frontal cone)
    movement: |
      - Active phase: melee Anhuur on platform; out of Burning Light blue ground
      - Shield phase: drop to assigned pit; kill snakes; channel beacon; return
  interrupt_role:
    none: no kick-required casts on Anhuur (Hymn interrupt-immune; Reckoning instant)
  rotation_modifications:
    pit_runner_assigned: |
      Melee Pit Vipers in pit, then channel Beacon. Most damage in pit is
      melee-AoE-friendly:
      - Combat Rogue: Fan of Knives + Blade Flurry on snake cluster
      - Fury Warrior: Whirlwind + Bladestorm (40s) on cluster
      - Ret Paladin: Consecration + Divine Storm at 3 HP
      - Frost DK: Howling Blast + Death and Decay
      - Enh Shaman: Fire Nova (level 80+) + Magma Totem
      - Feral Cat: Swipe + Thrash (CP-cost) on cluster
    pit_runner_NOT_assigned: continue melee on Anhuur
    shield_phase: stop boss DPS (boss damage-immune); no DPS opportunity
  cd_use:
    pit_burst: pop instant CDs on snake cluster (Combustion-equivalent / Trinket procs); save sustained CDs for boss
  heroic_delta: |
    - Beacon cast 8s → 10s (longer pit channel)
    - More Pit Vipers per side
  bot_anti_patterns:
    - DO NOT cast on boss during Shield of Light (immune)
    - DO NOT try to interrupt Reverberating Hymn
    - DO NOT skip Beacon channel if assigned
    - DO NOT drop into pit if NOT assigned
  bot_implementation_flag: |
    Pit-runner assignment — same as ranged-dps.md:
    1. At pull, designate 2 DPS (or accept group-leader assignment)
    2. On Shield of Light cast, assigned bots path to nearest pit edge
    3. Drop off platform
    4. Melee/AoE Pit Vipers
    5. Channel Beacon of Light
    6. Return path topside after channel completes
```

---

## Boss 2 — Earthrager Ptah

```yaml
boss_melee:
  boss: earthrager_ptah
  difficulty_grade: easy
  positioning:
    default: BEHIND boss; OUT of Quicksand pools; OUT of cleave arc (Raging Smash)
    facing: boss back
    movement: |
      Side-step Quicksand pools (visible). Dodge telegraphed Earth Spikes.
      On heroic burrow phase, kite-out sand vortex tornado while AoE-burning
      adds.
  interrupt_role:
    flame_bolt: PARTICIPATE in kick rotation (5s channel; easy kick)
  rotation_modifications:
    standard_phase: standard ST rotation on boss rear
    burrow_phase_50: |
      AoE rotation on add cluster:
      - Combat Rogue: Blade Flurry + Fan of Knives on scarab swarm
      - Fury Warrior: Whirlwind + Bladestorm + Cleave on cluster
      - Ret Paladin: Consecration + Divine Storm + Hammer of Wrath if HP-execute
      - Frost DK: Howling Blast on disease-applied pack
      - Enh Shaman: Fire Nova + Magma Totem + Chain Lightning at 5 MW
      - Feral Cat: Swipe + Thrash; maintain Rip on Dustbone Horror
  cd_use:
    standard: rotational; no special hold
  heroic_delta: |
    - Flame Bolt targets 3
    - Sand vortex kite during burrow
    - More Quicksand pools
  bot_anti_patterns:
    - DO NOT stand in Quicksand
    - DO NOT stand in front of boss (Raging Smash 150% cleave)
    - DO NOT chase tank if boss is repositioned (let tank move boss; you reposition behind)
```

---

## Boss 3 — Vault of Lights gauntlet + Anraphet

```yaml
boss_melee:
  boss: vault_of_lights_anraphet
  difficulty_grade: hard

  # ============================================================
  # PHASE A — Warden gauntlet
  # ============================================================
  warden_gauntlet:
    positioning: stack with tank for AoE cleave; spread for Earth/Fire Warden novas
    rotation: AoE-priority on Stone Trogg cluster + focus-DPS on active warden
    cd_use: align trinket procs / Berserk / Blade Flurry with focus-warden bursts
    interrupt_role: kick rotation on Stone Trogg caster types

  # ============================================================
  # PHASE B — Anraphet
  # ============================================================
  anraphet:
    positioning: |
      BEHIND boss. On heroic, dynamically re-position as void zones
      accumulate; tank may move boss to clean ground — melee follows.
    facing: boss back
    movement: out of Alpha Beam paths; spread for multi-beam scenarios

    interrupt_role:
      none: no kick-required casts on Anraphet himself
      stone_trogg_during_gauntlet: kick rotation

    rotation_modifications:
      standard: standard ST rotation behind boss
      omega_stance_channel: continue DPS (channel doesn't break melee; pulses AoE)
      alpha_beam_self_target: |
        If self is target, MOVE OUT of beam path:
        - Combat Rogue: full mobility; Sprint to relocate; rotation continues
        - Fury Warrior: Charge / Heroic Leap to relocate; rage stays
        - Ret Paladin: Hand of Freedom (snare immune); standard rotation
        - Frost DK: Death's Advance (talent — UNVERIFIED) for movement
        - Enh Shaman: Ghost Wolf brief; Spirit Walk (talent) for snare immune
        - Feral Cat: Stampeding Roar / Dash; rotation continues
      crumbling_ruin_late: |
        BURST through stacks before max-HP reduction makes execute thresholds
        impossible. Pop offensive CDs on late stacks.

    cd_use:
      awakening_burst: hold offensive CDs through awakening; resume on engagement
      crumbling_ruin_late: BURST through high stacks
      gauntlet_focus_warden: align CDs with active-warden burst window

    heroic_delta: |
      - Alpha Beam persistent void zones — mobility burden
      - Crumbling Ruin stacks faster
      - Stone Trogg gauntlet faster spawn

    bot_anti_patterns:
      - DO NOT stand in Alpha Beam path
      - DO NOT chase tank into void zones (re-position behind boss after tank moves)
      - DO NOT skip Stone Trogg AoE during gauntlet
      - DO NOT continue cast hardcast (most melee is melee — N/A)
```

---

## Boss 4 — Setesh, Construct of Destruction

```yaml
boss_melee:
  boss: setesh
  difficulty_grade: hard                  # canonical mana-test; portal priority + target swap
  positioning:
    default: |
      Mobile positioning. Setesh drifts (untankable). Melee bot follows
      active-target priority:
      1. Active Wurm pack (AoE-burn)
      2. Active Seeker (kick + nuke)
      3. Active Sentinel (low priority — Void Barrier 90% reduction)
      4. Active Portal (heroic — destroy)
      5. Boss (random target; lowest priority)
    facing: active target rear
    movement: |
      Out of Reign of Chaos bubbles + Chaos Blast ground patches; INTO
      Seed of Chaos for damage buff.
  interrupt_role:
    anti_magic_prison: |
      ABSOLUTE PRIORITY — Void Seeker cast. Kick rotation:
      - Combat Rogue: Kick (10s)
      - Fury Warrior: Pummel (10s)
      - Ret Paladin: Rebuke (15s)
      - Frost DK: Mind Freeze (10s)
      - Enh Shaman: Wind Shear (6s) — best uptime; 25y range = melee can kick from drift distance
      - Feral Cat: Skull Bash (60s; gap-close + interrupt)
  rotation_modifications:
    portal_kill_heroic: |
      On heroic, target priority above. Lots of target swaps:
      - Combat Rogue: CP attached to target — RESET on swap; Slice and
        Dice via 1 CP from new target; rebuild for Eviscerate
      - Frost DK: diseases on target — Outbreak on new target
      - Feral Cat: CP attached to target — RESET; Mangle/Rake/Rip on new
      - Fury Warrior / Ret Pally / Enh Shaman: most buffs self (Inquisition,
        Lightning Shield) — unaffected by target swap
    portal_kill_normal: |
      On normal, portals auto-close. Target priority:
      1. Anti-Magic Prison kick
      2. Active Wurms (AoE-burn)
      3. Boss (let portals auto-close)
    seed_of_chaos: walk into seed for +100% damage buff for 15s
    mobile_positioning: |
      Setesh drifts; melee follows active target. Movement-friendly classes
      preferred:
      - Combat Rogue: full mobility
      - Fury Warrior: Charge / Heroic Leap for re-engage
      - Frost DK: Death's Advance / Death Grip (don't grip Setesh — UNVERIFIED if works on untankable boss)
      - Enh Shaman: Ghost Wolf for movement
      - Feral Cat: Dash / Stampeding Roar
  cd_use:
    blessing_of_the_sun_NOT_HERE: save Bloodlust for Rajh
    portal_burn_window: pop offensive CDs (Berserk / Avenging Wrath / Pillar of Frost / etc.) on portal-spawn for burst on Wurms
    seed_of_chaos_buff_active: BURST during +100% damage buff
  heroic_delta: |
    - Portals don't auto-close — DPS must destroy
    - Anti-Magic Prison casts more frequent
    - More target-swap pressure
  bot_anti_patterns:
    - DO NOT focus-DPS Sentinels (Void Barrier 90% reduction)
    - DO NOT stand in Reign of Chaos bubbles
    - DO NOT skip Anti-Magic Prison kick
    - DO NOT skip Seed of Chaos walk-into
    - DO NOT continue ST DPS on Setesh during heroic when portals up
    - DO NOT spend CP on dying targets (Rogue/Druid CP-management)
```

---

## Boss 5 — Ammunae, Construct of Life

```yaml
boss_melee:
  boss: ammunae
  difficulty_grade: medium
  positioning:
    default: BEHIND boss; OUT of Noxious Spore clouds (6y, 30s)
    facing: boss back
    movement: |
      Side-step Spore clouds. Switch to Bloodpetal Sprouts on Rampant
      Growth conversion. Melee can help Spore kills if close.
  interrupt_role:
    consume_life_energy: |
      ABSOLUTE PRIORITY — interrupt every cast. Kicks:
      - Combat Rogue: Kick (10s)
      - Fury Warrior: Pummel (10s)
      - Ret Paladin: Rebuke (15s)
      - Frost DK: Mind Freeze (10s)
      - Enh Shaman: Wind Shear (6s; off-GCD; 25y range)
      - Feral Cat: Skull Bash (60s; gap-close + interrupt)
    wither: SECONDARY interrupt; can also be dispelled by healer
  rotation_modifications:
    standard: standard ST on boss
    sprout_burst: |
      AoE on Bloodpetal Sprouts after Rampant Growth:
      - Combat Rogue: Blade Flurry + Fan of Knives
      - Fury Warrior: Whirlwind + Bladestorm
      - Ret Paladin: Divine Storm + Consecration
      - Frost DK: Howling Blast spam
      - Enh Shaman: Fire Nova + Magma Totem
      - Feral Cat: Swipe / Thrash
    spore_kill_assist: instant hits on Spore if in melee range; otherwise ranged handles
  cd_use:
    standard: rotational; align with Bloodlust if not on Setesh / Rajh
    sprout_burst: instant CDs (Berserk / Trinket procs) on sprout cluster
  heroic_delta: |
    - Pods spawn faster
    - Bloodpetal Sprouts hit harder
  bot_anti_patterns:
    - DO NOT skip kick on Consume Life Energy
    - DO NOT stand in Noxious Spore clouds
    - DO NOT chase pods into ranged-DPS responsibility (let ranged kill pods unless adjacent)
```

---

## Boss 6 — Isiset, Construct of Magic

```yaml
boss_melee:
  boss: isiset
  difficulty_grade: medium                # face-away mid-combat unusual for melee
  positioning:
    default: BEHIND boss; FACE AWAY on Supernova cast
    facing: |
      DEFAULT: behind boss (rear position).
      OVERRIDE: FACE AWAY from Isiset on Supernova cast.
      For melee, "behind boss" already means the bot's BACK is to Isiset's
      front — which is correct facing-away (the bot is looking outward).
      bot_implementation: melee behind-position naturally satisfies face-away;
      verify camera/facing direction matches.
    movement: out of Astral Rain channel ground
  interrupt_role:
    arcane_barrage_familiar: PARTICIPATE in kick rotation on Celestial Familiar adds
  rotation_modifications:
    standard_phase: standard ST behind Isiset
    supernova_face_away: |
      Melee behind boss = facing rear-direction = facing AWAY from Isiset's
      front. Standard rear-positioning satisfies the mechanic.
      EDGE CASE: if melee is forced to side-position (e.g., by Astral Rain
      ground), bot must verify facing on Supernova cast.
    split_phase_familiar_focus: |
      Single-target on assigned familiar (kill priority Astral Rain →
      Celestial Call → Veil of Sky). Move to chosen familiar's position.
      Veil of Sky has reflect chance — melee classes don't typically have
      offensive dispel; Mage Spellsteal / Shaman Purge / Priest Mass Dispel
      handle it. **Enh Shaman has Purge (offensive dispel)** — Enh melee
      bot should Purge Veil of Sky if encountered.
  cd_use:
    standard: rotational; align with Bloodlust if not used elsewhere
    split_phase_burst: pop offensive CDs on assigned familiar
  heroic_delta: |
    - All damage values higher
    - Familiar HP higher (split phase longer)
  bot_anti_patterns:
    - DO NOT face Isiset on Supernova cast (4s disorient = wasted GCDs)
    - DO NOT stand in Astral Rain channel ground
    - DO NOT skip familiar kill priority
    - DO NOT melee Veil of Sky-shielded familiar (reflect chance)
```

---

## Boss 7 — Rajh, Construct of the Sun

```yaml
boss_melee:
  boss: rajh
  difficulty_grade: hard                  # final boss; dense kick rotation
  positioning:
    default: BEHIND boss; OUT of Solar Winds tornado path
    facing: boss back
    movement: |
      Kite-out Solar Winds tornado. On heroic, avoid Solar Fire patches
      dropped along Solar Winds movement path (89131).
  interrupt_role:
    sun_orb: PRIMARY KICK — 3s channel; rotation:
      - Combat Rogue: Kick (10s)
      - Fury Warrior: Pummel (10s)
      - Ret Paladin: Rebuke (15s)
      - Frost DK: Mind Freeze (10s)
      - Enh Shaman: Wind Shear (6s; 25y range)
      - Feral Cat: Skull Bash (60s; gap-close + interrupt)
    inferno_leap: PRIMARY KICK — 3s telegraph; saves random player from leap-kill
  rotation_modifications:
    standard_phase: standard ST behind boss; kick rotation on Sun Orb / Inferno Leap
    blessing_of_the_sun_burn_window: |
      8s of +100% damage to ALL PLAYERS. SAVE OFFENSIVE CDS:
      - Combat Rogue: Adrenaline Rush + Killing Spree + Vendetta (Assn — N/A) + Trinket procs
      - Fury Warrior: Recklessness + Death Wish + Berserker Stance + Bloodlust
      - Ret Paladin: Avenging Wrath + Zealotry + Guardian of Ancient Kings + Inquisition
      - Frost DK: Pillar of Frost + Empower Rune Weapon + Bloodlust
      - Enh Shaman: Bloodlust (if not used) + Feral Spirit + Stormstrike on CD + Lava Lash
      - Feral Cat: Tiger's Fury + Berserk + Trinket procs
    solar_winds_kite: |
      Tornado(s) roam. Movement-friendly classes preferred:
      - Combat Rogue: full mobility; Sprint
      - Fury Warrior: Charge / Heroic Leap to re-engage
      - Ret Paladin: Hand of Freedom + Speed of Light (talent — UNVERIFIED 4.3.4)
      - Frost DK: Death's Advance (talent)
      - Enh Shaman: Ghost Wolf brief / Spirit Walk
      - Feral Cat: Dash / Stampeding Roar
  cd_use:
    blessing_of_the_sun: |
      ABSOLUTE COOLDOWN ALIGNMENT — Bloodlust + offensive CDs in this window.
      bot_implementation: BURN_WINDOW state tracks Blessing of the Sun
      channel; all offensive CDs unlocked during channel.
  heroic_delta: |
    - Solar Fire (89131) persistent fire patches
    - Floor fills with hazards over time
    - Achievement window tightens
  bot_anti_patterns:
    - DO NOT stand in Solar Winds tornado path
    - DO NOT skip kicks on Sun Orb / Inferno Leap
    - DO NOT save offensive CDs past Blessing of the Sun window
    - DO NOT continue melee on boss while Solar Wind is overlapping rear position (relocate)
```

---

## Bot AI hooks

```yaml
ai_hooks:
  anhuur:
    - encounter_state_pull → designate_pit_runners (1 of 2 melee DPS bots)
    - encounter_detect_shield_of_light_cast AND self.is_pit_runner → drop_to_pit + clear_vipers + channel_beacon
    - encounter_detect_shield_of_light_cast AND NOT self.is_pit_runner → maintain_topside_melee + stop_dps_on_boss
    - encounter_detect_burning_light_self → move_out
    - encounter_state_active → melee_topside_OR_pit_clear

  ptah:
    - encounter_state_active → melee_rear_outside_quicksand
    - encounter_detect_flame_bolt_cast → kick_if_self_has_interrupt
    - encounter_state_burrow_phase → swap_to_aoe_rotation
    - encounter_detect_quicksand_self → move_out
    - encounter_detect_earth_spike_self → move_out
    - encounter_detect_raging_smash_about_to_fire → verify_self_NOT_in_front_arc

  vault_of_lights_anraphet:
    - encounter_state_warden_gauntlet → aoe_priority_troggs + focus_dps_active_warden
    - encounter_detect_warden_nova → spread_or_break_bubble per warden type
    - encounter_state_anraphet_active → melee_rear_dynamic
    - encounter_detect_alpha_beam_self → move_out
    - encounter_state_crumbling_ruin_high_stack → burn_offensive_cds

  setesh:
    - encounter_state_active → mobile_target_priority
    - encounter_detect_anti_magic_prison_cast → kick_absolute_priority
    - encounter_detect_chaos_portal_open AND difficulty.heroic → swap_target_to_portal_addn_priority
    - encounter_detect_seed_of_chaos_nearby → walk_through_for_buff
    - encounter_detect_void_seeker_cast_active → kick_absolute_priority
    - encounter_state_void_sentinel_alive AND no_other_targets → low_priority_dps
    - encounter_detect_chaos_blast_ground_telegraph → move_out
    - encounter_detect_reign_of_chaos_bubble_self → move_out
    - encounter_target_died (CP-class) → DO_NOT_spend_CP_on_dying_target

  ammunae:
    - encounter_detect_consume_life_energy_cast → kick_absolute_priority
    - encounter_detect_seedling_pod_spawn_in_melee_range → swap_target_kill_pod (else let ranged handle)
    - encounter_detect_spore_in_melee_range → kill_spore + move_from_cloud_spawn
    - encounter_detect_wither_cast → kick_secondary_priority
    - encounter_detect_rampant_growth_cast → swap_to_aoe_on_sprouts

  isiset:
    - encounter_detect_supernova_cast → verify_facing_AWAY_from_boss (rear position naturally satisfies)
    - encounter_state_split_phase → focus_fire_kill_priority_familiar
    - encounter_detect_veil_of_sky_familiar AND self.has_purge → enh_shaman_purge_only
    - encounter_detect_celestial_familiar_cast → kick_arcane_barrage
    - encounter_detect_astral_rain_ground → move_out

  rajh:
    - encounter_state_active → melee_rear + kite_solar_winds
    - encounter_detect_sun_orb_cast → kick_primary_priority
    - encounter_detect_inferno_leap_cast → kick_primary_priority
    - encounter_detect_blessing_of_the_sun_channel → POP_ALL_OFFENSIVE_CDS (BURN_WINDOW)
    - encounter_detect_solar_winds_path_self → relocate_rear
    - encounter_detect_solar_fire_patch_self_heroic → move_out

dispel_blacklist: []                      # NONE — Halls of Origination has no "do not dispel" entries
# Melee bots: Ret Paladin Cleanse / Enh Shaman Cleanse Spirit / Feral Druid Remove Corruption
# all unaffected by absent blacklist; standard dispel rules apply.

cooldown_save_priority:
  anhuur:    [no_save — minor rotational only]
  ptah:      [no_save — minor rotational only]
  anraphet:  [save Bloodlust if not used; pop on Crumbling Ruin late]
  setesh:    [pop offensive CDs on portal kills + Seed of Chaos windows]
  ammunae:   [save Bloodlust for Rajh; instant CDs on sprout windows]
  isiset:    [save Bloodlust for Rajh; pop on familiar kill phases]
  rajh:      [BURN — Bloodlust + all offensive CDs in Blessing of the Sun window]
```

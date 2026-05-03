# Halls of Origination — Ranged DPS

**Ranged-DPS-specific response logic** for the bot's encounter handling. Mechanics, spell IDs, phase triggers, and add abilities are defined in `encounter.md`; this doc only encodes how ranged DPS reacts. Class-agnostic where possible.

**Layering**:
- Cross-content ranged DPS patterns (positioning, spread/stack rules, threat ceiling) live in `docs/roles/ranged-dps.md`
- Per-spec rotation lives in the active spec's `docs/specs/` doc (frost-mage, affliction-warlock, mm-hunter, shadow-priest, balance-druid, elemental-shaman)
- This doc layers HoO-specific deviations on top of those

---

## Dungeon-level overview

```yaml
dungeon_ranged_dps_overview:
  total_bosses: 7
  estimated_full_clear_minutes: 35-55     # LONGEST Cata 5-man
  positioning_profile:
    anhuur: pit_runner_OR_topside         # 1 of 2 designated pit-runners; OR topside ranged
    ptah: max_range_spread                 # standard
    anraphet: max_range_dynamic            # heroic void zones force re-position
    setesh: portal_priority_mobile         # target-swap heavy; mobile
    ammunae: max_range_kill_pods           # ranged DPS focus
    isiset: max_range_face_away_supernova  # face-away override
    rajh: max_range_kite_winds             # dense kick rotation + Solar Winds kite
  interrupt_load:
    anhuur: zero (Hymn interrupt-immune; Burning Light / Reckoning have no kick)
    ptah: medium (Flame Bolt 5s channel — opportunistic kick)
    anraphet: zero (no kick targets)
    setesh: HIGH (Anti-Magic Prison ABSOLUTE priority)
    ammunae: HIGH (Consume Life Energy ABSOLUTE; Wither secondary)
    isiset: low (Arcane Barrage rotation kick)
    rajh: HIGH (Sun Orb + Inferno Leap rotation kicks)
  key_responsibilities:
    - anhuur_pit_runner_assignment (1 of 2 designated)
    - kill_pods_priority (ammunae)
    - portal_kills_heroic (setesh)
    - kick_rotation_dense (rajh)
    - face_away_on_supernova (isiset)
    - cooldown_alignment_with_blessing (rajh burn window)
```

---

## Trash — ranged DPS notes

```yaml
trash_ranged:
  pre_anhuur_anubisath:
    cc_priorities: |
      Ranged contributes opener-CC on humanoid Anubisath casters:
      - Frost Mage: Polymorph (Sheep) — primary multi-CC
      - Affliction Warlock: Banish on demonic; Fear if PvE-allowed
      - MM Hunter: Freezing Trap on caster
      - Shadow Priest: Shackle Undead (if undead-flagged); Mind Control rare
      - Balance Druid: Cyclone (8s)
      - Elemental Shaman: Hex (frog 60s, breaks on damage)
  vault_gauntlet_stone_troggs:
    aoe_priority: |
      AoE-burn troggs near tank cleave:
      - Frost Mage: Blizzard / Frostfire Bolt cleave
      - Aff Warlock: Seed of Corruption + Hellfire (high-AoE-burn)
      - MM Hunter: Multi-Shot + Trap Launcher Explosive Trap
      - Shadow Priest: Mind Sear if 4+ targets
      - Balance Druid: Hurricane (channel) / Wild Mushroom detonation
      - Ele Shaman: Magma Totem + Chain Lightning at 5 LS stacks
    interrupt_role: |
      Stone Trogg caster types — Counterspell / Spell Lock / Silencing Shot
      / Silence / Solar Beam / Wind Shear; rotate kicks across pulls.
  setesh_halls_shadowlancers:
    cc_priorities: |
      Shadowlancers are humanoid CC targets — Polymorph / Banish (if demonic
      class) / Hex / Cyclone work. Frost Mage Sheep is primary CC role.
    interrupt_role: |
      Shadowlancer caster trash — kick rotation. PRIMARY KICK for Anti-Magic
      Prison (in trash form, on smaller add).
    notes: |
      MANA-DRAIN GAUNTLET — keep DPS on the rails; conserve mana-cost CDs
      (Mage Mana Gem, Shaman Mana Spring Totem, Shadow Priest Dispersion).

  construct_hall_caster_trash:
    interrupt_role: kick rotation on caster channels; turn-away if frontal
```

---

## Boss 1 — Temple Guardian Anhuur

```yaml
boss_ranged:
  boss: temple_guardian_anhuur
  difficulty_grade: medium                # door event coordination
  positioning:
    role: |
      1 of 2 DESIGNATED PIT-RUNNERS (default — bot composition picks 2 DPS).
      If assigned: drop into east OR west pit on Shield of Light cast,
      kill Pit Vipers, channel Beacon of Light (8s normal / 10s heroic).
      If NOT assigned: stay topside, max range, continue boss DPS.
    facing: any
    movement: |
      - Active phase: max range from boss; out of Burning Light blue ground
      - Shield phase: drop to assigned pit; kill snakes; channel beacon; return topside
  interrupt_role:
    none: no kick-required casts on this fight (Hymn interrupt-immune; Reckoning instant)
  rotation_modifications:
    pit_runner_assigned: |
      AoE-burn Pit Vipers, then channel Beacon. Most damage in pit is
      ranged-AoE-friendly:
      - Frost Mage: Blizzard for snake cluster
      - Aff Warlock: Seed of Corruption on lead snake; AoE-burn
      - MM Hunter: Multi-Shot + Trap Launcher Explosive Trap
      - Shadow Priest: Mind Sear at center of snake stack
      - Balance Druid: Hurricane for snake cluster
      - Ele Shaman: Earthquake (level 85) or Chain Lightning
    pit_runner_NOT_assigned: continue boss DPS topside
    shield_phase: stop boss DPS (boss damage-immune); no DPS opportunity
  cd_use:
    save_for_real_bosses: minimal CD use; rotational only on Anhuur
  heroic_delta: |
    - Beacon cast 8s → 10s (longer pit channel)
    - More Pit Vipers per side
  bot_anti_patterns:
    - DO NOT cast on boss during Shield of Light (immune to damage)
    - DO NOT try to interrupt Reverberating Hymn (interrupt-immune during shield)
    - DO NOT skip Beacon channel if assigned (group depends on it)
    - DO NOT drop into pit if NOT assigned (only 2 designated runners)
  bot_implementation_flag: |
    Pit-runner assignment is the most non-standard navigation requirement.
    Bot must:
    1. At pull, designate 2 DPS bots (or accept group-leader assignment)
    2. On Shield of Light cast, assigned bots path to nearest pit edge
    3. Drop off platform (verify path-finding doesn't get stuck)
    4. AoE Pit Vipers
    5. Channel Beacon of Light (interruptible by mob damage? UNVERIFIED)
    6. Return path to topside after channel completes
```

---

## Boss 2 — Earthrager Ptah

```yaml
boss_ranged:
  boss: earthrager_ptah
  difficulty_grade: easy
  positioning:
    default: max range; spread to avoid Earth Spike multi-target
    facing: boss
    movement: |
      Out of Quicksand pools (visible on floor); out of telegraphed Earth
      Spikes; on heroic during burrow phase, kite the sand vortex tornado.
  interrupt_role:
    flame_bolt: PARTICIPATE in kick rotation if available (5s channel = easy kick)
  rotation_modifications:
    standard_phase: standard ST rotation on boss
    burrow_phase_50: |
      AoE rotation on add cluster:
      - Frost Mage: Blizzard / Frost Nova for root + Frostfire Bolt cleave
      - Aff Warlock: Seed of Corruption on horror; AoE-burn scarabs
      - MM Hunter: Multi-Shot + Trap Launcher Explosive Trap on scarab pile
      - Shadow Priest: Mind Sear at scarab center
      - Balance Druid: Hurricane (channel) on scarab cluster
      - Ele Shaman: Magma Totem + Chain Lightning + Earthquake (lvl 85)
    sand_vortex_heroic: kite-out tornado; mount camel for movement-buff if convenient
  cd_use:
    standard: rotational; no special hold
  heroic_delta: |
    - Flame Bolt targets 3 (vs 2)
    - Sand vortex kite during burrow
    - More Quicksand pools
  bot_anti_patterns:
    - DO NOT stand in Quicksand
    - DO NOT cluster ranged (Earth Spike multi-hit)
    - DO NOT cast in front of boss (Raging Smash cleave — though ranged shouldn't be in front anyway)
```

---

## Boss 3 — Vault of Lights gauntlet + Anraphet

```yaml
boss_ranged:
  boss: vault_of_lights_anraphet
  difficulty_grade: hard

  # ============================================================
  # PHASE A — Warden gauntlet
  # ============================================================
  warden_gauntlet:
    positioning: stay near tank for AoE cleave; spread for Earth/Fire Warden novas
    rotation: AoE-priority on Stone Trogg cluster + focus-DPS on active warden
    cd_use: |
      Hold offensive CDs for warden burst windows. Trinket procs / Combustion
      / Berserk align well with focus-warden DPS.
    notes: continuous trogg spawn; mana-conservation favored

  # ============================================================
  # PHASE B — Anraphet
  # ============================================================
  anraphet:
    positioning: |
      Max range from boss. Spread to avoid Alpha Beam multi-target. On
      heroic, dynamically re-position as void zones accumulate (path-recompute
      every Alpha Beam cycle).
    facing: boss
    movement: out of Alpha Beam paths; spread for multi-beam scenarios

    interrupt_role:
      none: no kick-required casts on Anraphet himself

    rotation_modifications:
      standard: standard ST rotation
      omega_stance_channel: continue DPS (channel is ground-AoE; doesn't break casts)
      alpha_beam_self_target: |
        If self is target, MOVE OUT of beam path:
        - Frost Mage: Frostbolt hardcast — stop-cast, move, resume
        - Aff Warlock: DoTs are instant; movement-friendly
        - MM Hunter: Steady Shot hardcast; Aimed Shot proc-only
        - Shadow Priest: Mind Flay channel breaks on movement; instant DoTs OK
        - Balance Druid: Wrath/Starfire hardcast; instant Moonfire/Sunfire OK
        - Ele Shaman: Lightning Bolt hardcast; instant Flame Shock + Lava Burst (proc) OK

    cd_use:
      awakening_burst: hold offensive CDs through awakening (boss isn't targetable yet); resume on engagement
      crumbling_ruin_late: BURST through Crumbling Ruin stacks before max-HP reduction makes target threshold drop too low

    heroic_delta: |
      - Alpha Beam persistent void zones — mobility burden on ranged
      - Crumbling Ruin stacks faster — DPS pressure
      - Stone Trogg gauntlet faster spawn

    bot_anti_patterns:
      - DO NOT stand in Alpha Beam path (random target; movement is constant)
      - DO NOT cast hardcast spells while moving (Frostbolt, Lightning Bolt break)
      - DO NOT cluster ranged (Alpha Beam multi-target hazard)
      - DO NOT skip Stone Trogg AoE during gauntlet (tank will lose threat)
```

---

## Boss 4 — Setesh, Construct of Destruction

```yaml
boss_ranged:
  boss: setesh
  difficulty_grade: hard                  # canonical mana-test; portal priority
  positioning:
    default: |
      Mobile positioning. Setesh drifts (no tank pin). Stay max range from
      boss; portal-priority means lots of target swaps. Walk into Seed of
      Chaos for +100% damage buff (15s).
    facing: active target (boss / Wurm / Seeker / Sentinel)
    movement: |
      Out of Reign of Chaos bubbles + Chaos Blast ground patches; INTO
      Seed of Chaos for damage buff.
  interrupt_role:
    anti_magic_prison: |
      ABSOLUTE PRIORITY — Void Seeker cast. Block magical healing on whoever
      it lands on (often healer or tank). Bot ranged must rotate kicks
      across pulls.
      - Frost Mage: Counterspell (24s; off-GCD)
      - Aff Warlock: Spell Lock via Felhunter pet (24s)
      - MM Hunter: Silencing Shot (24s)
      - Shadow Priest: Silence (45s)
      - Balance Druid: Solar Beam (60s; 8s AoE silence — good for Seekers)
      - Ele Shaman: Wind Shear (6s; off-GCD; 25y range — best uptime)
  rotation_modifications:
    portal_kill_heroic: |
      On heroic, portals don't auto-close. DPS priority:
      1. Anti-Magic Prison cast (kick)
      2. Active Wurms (low HP; AoE-burn)
      3. Active Seekers (kick + nuke if cast active)
      4. Active Portal (if no Wurms/Seeker active, burn portal)
      5. Active Sentinel (only if no other targets — Void Barrier 90% reduction)
      6. Boss (random target; not the priority target)
    portal_kill_normal: |
      On normal, portals auto-close. DPS priority:
      1. Anti-Magic Prison cast (kick)
      2. Boss (best DPS uptime; let portals auto-close)
      3. Wurms / Seekers if up
    seed_of_chaos: walk into seed for +100% damage buff for 15s (significant DPS gain)
  cd_use:
    blessing_of_the_sun_NOT_HERE: this is Setesh, not Rajh — save Bloodlust for Rajh if not used elsewhere
    portal_burn_window: pop offensive CDs on portal-spawn (Combustion / Berserk / Avenging Wrath)
  heroic_delta: |
    - Portals don't auto-close — DPS must destroy
    - Anti-Magic Prison casts more frequent
    - Significantly more target-swap pressure
  bot_anti_patterns:
    - DO NOT stand in Reign of Chaos bubbles
    - DO NOT skip Anti-Magic Prison kick (UNVERIFIED dispellability — if landed, healing is gone)
    - DO NOT focus-DPS Sentinels (90% damage reduction — lowest priority)
    - DO NOT skip Seed of Chaos walk-into (free +100% damage buff)
    - DO NOT continue ST DPS on Setesh during heroic when portals are up (portals auto-close on normal only)
```

---

## Boss 5 — Ammunae, Construct of Life

```yaml
boss_ranged:
  boss: ammunae
  difficulty_grade: medium
  positioning:
    default: max range from boss; mobility for Spore cloud avoidance + pod kills
    facing: boss / pod / spore (whichever is current target)
    movement: out of Noxious Spore clouds (6y); reposition for pods (random spawn)
  interrupt_role:
    consume_life_energy: |
      ABSOLUTE PRIORITY — interrupt every cast. Kicks rotation across DPS:
      - Frost Mage: Counterspell (24s)
      - Aff Warlock: Spell Lock via Felhunter
      - MM Hunter: Silencing Shot (24s)
      - Shadow Priest: Silence (45s)
      - Balance Druid: Solar Beam (60s)
      - Ele Shaman: Wind Shear (6s — best uptime)
    wither: SECONDARY interrupt; can also be dispelled by healer post-cast
  rotation_modifications:
    standard: standard ST on boss BETWEEN add priorities
    pod_kill_priority: |
      KILL SEEDLING PODS ON SPAWN (every ~14s, 14y from boss):
      - Frost Mage: Frostfire Bolt to one-shot small pods
      - Aff Warlock: instant DoT spread (Seed of Corruption + Bane of Doom)
      - MM Hunter: Aimed Shot / Steady Shot
      - Shadow Priest: instant Mind Blast / SW:Death execute
      - Balance Druid: Starfire / Wrath; Sunfire instant
      - Ele Shaman: Lava Burst (instant on FS proc) / Lightning Bolt
    spore_kill: kill Spore add on spawn, then move away from cloud spawn point
    rampant_growth_burst: |
      AoE rotation on Bloodpetal Sprouts (converted pods). Hopefully pod
      count was 0 before Rampant Growth (no sprouts spawn). If sprouts up,
      AoE down.
  cd_use:
    standard: rotational; align with Bloodlust if not on Setesh
    pod_burn: instant-cast CDs (Mage Combustion, Druid Force of Nature) align with pod-cluster windows
  heroic_delta: |
    - Pods spawn faster
    - Pods may heal boss (UNVERIFIED) — pod kills more important
    - Bloodpetal Sprouts hit harder
  bot_anti_patterns:
    - DO NOT skip kick on Consume Life Energy (boss energy gain accelerates Rampant Growth)
    - DO NOT skip pod kills (passive boss buff stacks fast)
    - DO NOT stand in Noxious Spore clouds (6y radius, 30s duration)
```

---

## Boss 6 — Isiset, Construct of Magic

```yaml
boss_ranged:
  boss: isiset
  difficulty_grade: medium                # face-away mid-combat is unusual
  positioning:
    default: max range; FACE AWAY on Supernova cast
    facing: |
      DEFAULT: face boss (cast direction).
      OVERRIDE: FACE AWAY from Isiset on Supernova cast (4s disorient if facing).
      Bot must temporarily reverse facing mid-combat.
    movement: out of Astral Rain channel ground patches
  interrupt_role:
    arcane_barrage_familiar: PARTICIPATE in kick rotation on Celestial Familiar adds
  rotation_modifications:
    standard_phase: standard ST on Isiset
    supernova_face_away: |
      On Supernova cast detection, IMMEDIATELY FACE AWAY:
      - Frost Mage: stop-cast, turn 180°, resume Frostbolt facing-away (?check spell facing rules — UNVERIFIED for hardcasts)
      - Aff Warlock: instant DoTs OK while facing away (no facing requirement)
      - MM Hunter: Steady Shot / Aimed Shot facing requirements (UNVERIFIED — believed face-direction-flexible)
      - Shadow Priest: Mind Flay channel; instant DoTs OK
      - Balance Druid: instant Moonfire/Sunfire OK; Wrath/Starfire facing OK away
      - Ele Shaman: hardcasts have facing; pause if needed
    split_phase_familiar_focus: |
      Single-target focus-fire on assigned familiar (kill priority Astral
      Rain → Celestial Call → Veil of Sky):
      - Frost Mage: Frostbolt + Frostfire Bolt + Ice Lance proc; SPELLSTEAL Veil of Sky
      - Aff Warlock: full DoT setup on familiar; UA + Corruption + Bane of Agony
      - MM Hunter: Aimed Shot / Steady Shot rotation
      - Shadow Priest: VT + SWP + Mind Blast + Mind Flay; MASS DISPEL Veil of Sky
      - Balance Druid: Eclipse-driven; instant DoTs preserved
      - Ele Shaman: Flame Shock + Lava Burst + Lightning Bolt; PURGE Veil of Sky
    veil_of_sky_dispel: |
      Mage Spellsteal / Shaman Purge / Priest Mass Dispel — strip Veil of
      Sky shield. Significant DPS gain.
  cd_use:
    standard: rotational; align with Bloodlust if not on Setesh / Rajh
    split_phase_burst: pop offensive CDs on assigned familiar
  heroic_delta: |
    - All damage values higher
    - Familiar HP higher (split phase longer)
  bot_anti_patterns:
    - DO NOT face Isiset on Supernova cast (4s disorient = wasted GCDs)
    - DO NOT stand in Astral Rain channel ground
    - DO NOT skip familiar kill priority (empower scaling is steep)
    - DO NOT continue casting Veil of Sky-shielded familiar (reflect chance)
```

---

## Boss 7 — Rajh, Construct of the Sun

```yaml
boss_ranged:
  boss: rajh
  difficulty_grade: hard                  # final boss; dense kick rotation
  positioning:
    default: max range; SPREAD for Inferno Leap (avoid multi-hit)
    facing: boss
    movement: kite-out Solar Winds; avoid Solar Fire patches (heroic)
  interrupt_role:
    sun_orb: PRIMARY KICK — 3s channel; rotation:
      - Frost Mage: Counterspell (24s)
      - Aff Warlock: Spell Lock via Felhunter
      - MM Hunter: Silencing Shot (24s)
      - Shadow Priest: Silence (45s)
      - Balance Druid: Solar Beam (60s; AoE silence)
      - Ele Shaman: Wind Shear (6s; best uptime)
    inferno_leap: PRIMARY KICK — 3s telegraph; saves a random player from leap-kill
  rotation_modifications:
    standard_phase: standard ST rotation on boss; kick rotation on Sun Orb / Inferno Leap
    blessing_of_the_sun_burn_window: |
      8s of +100% damage to ALL PLAYERS. SAVE OFFENSIVE CDS for this window:
      - Frost Mage: Combustion (per source spec — UNVERIFIED) + Icy Veins + Mirror Image + Mana Gem
      - Aff Warlock: Demon Soul + Soulburn Soul Fire + Curse of Elements
      - MM Hunter: Rapid Fire + Readiness + Bestial Wrath
      - Shadow Priest: Shadowfiend + Mind Blast + Devouring Plague
      - Balance Druid: Starfall + Force of Nature + Eclipse-aligned cycle
      - Ele Shaman: Bloodlust (if not used) + Elemental Mastery + Fire Elemental Totem
    solar_winds_kite: |
      Tornado(s) roam. Movement-friendly classes preferred:
      - Frost Mage: Frostbolt hardcast — pause on movement
      - Aff Warlock: instant DoTs OK; Drain Soul channel breaks
      - MM Hunter: Steady Shot hardcast — pause; instants OK
      - Shadow Priest: Mind Flay channel — break on move; DoTs OK
      - Balance Druid: instant DoTs preserved; hardcasts pause
      - Ele Shaman: Lava Burst + Flame Shock + Lightning Bolt; Spiritwalker's Grace (90s CD) for mobility
  cd_use:
    blessing_of_the_sun: |
      ABSOLUTE COOLDOWN ALIGNMENT — Bloodlust + offensive CDs in this window.
      bot_implementation: BURN_WINDOW state tracks Blessing of the Sun
      channel; all offensive CDs unlocked during channel.
  heroic_delta: |
    - Solar Fire (89131) persistent fire patches dropped along Solar Winds
    - Floor fills with hazards over time
    - Achievement window (Sun-of-a...) tightens kill timer
  bot_anti_patterns:
    - DO NOT cluster ranged (Inferno Leap multi-hit hazard)
    - DO NOT stand in Solar Winds tornado path
    - DO NOT skip kicks on Sun Orb / Inferno Leap
    - DO NOT save offensive CDs past Blessing of the Sun window (use them; another Blessing comes if not killed)
    - DO NOT continue casting hardcasts while kiting Solar Winds
```

---

## Bot AI hooks

```yaml
ai_hooks:
  anhuur:
    - encounter_state_pull → designate_pit_runners (1 of 2 ranged DPS bots)
    - encounter_detect_shield_of_light_cast AND self.is_pit_runner → drop_to_pit + clear_vipers + channel_beacon
    - encounter_detect_shield_of_light_cast AND NOT self.is_pit_runner → maintain_topside_position + stop_dps_on_boss
    - encounter_detect_burning_light_self → move_out
    - encounter_state_active → max_range_topside_OR_pit_clear

  ptah:
    - encounter_state_active → max_range_spread
    - encounter_detect_flame_bolt_cast → kick_if_self_has_interrupt
    - encounter_state_burrow_phase → swap_to_aoe_rotation
    - encounter_detect_quicksand_self → move_out
    - encounter_detect_earth_spike_self → move_out

  vault_of_lights_anraphet:
    - encounter_state_warden_gauntlet → aoe_priority_troggs + focus_dps_active_warden
    - encounter_detect_warden_nova → spread_or_break_bubble per warden type
    - encounter_state_anraphet_active → max_range_spread + dynamic_void_zone_movement
    - encounter_detect_alpha_beam_self → move_out + pause_hardcast
    - encounter_state_crumbling_ruin_high_stack → burn_offensive_cds

  setesh:
    - encounter_state_active → mobile_max_range
    - encounter_detect_anti_magic_prison_cast → kick_absolute_priority
    - encounter_detect_chaos_portal_open AND difficulty.heroic → swap_target_to_portal
    - encounter_detect_seed_of_chaos_nearby → walk_through_for_buff
    - encounter_detect_void_seeker_cast_active → kick_absolute_priority
    - encounter_state_void_sentinel_alive AND no_other_targets → low_priority_dps_sentinel
    - encounter_detect_chaos_blast_ground_telegraph → move_out
    - encounter_detect_reign_of_chaos_bubble_self → move_out

  ammunae:
    - encounter_detect_consume_life_energy_cast → kick_absolute_priority
    - encounter_detect_seedling_pod_spawn → swap_target_kill_pod
    - encounter_detect_spore_spawn → kill_spore + move_from_cloud_spawn
    - encounter_detect_wither_cast → kick_secondary_priority
    - encounter_detect_rampant_growth_cast → swap_to_aoe_on_sprouts

  isiset:
    - encounter_detect_supernova_cast → FACE_AWAY (override default facing)
    - encounter_state_split_phase → focus_fire_kill_priority_familiar (Astral Rain → Celestial Call → Veil of Sky)
    - encounter_detect_veil_of_sky_familiar AND self.has_offensive_dispel → spellsteal_purge_mass_dispel
    - encounter_detect_celestial_familiar_cast → kick_arcane_barrage
    - encounter_detect_astral_rain_ground → move_out

  rajh:
    - encounter_state_active → max_range_spread + kite_solar_winds
    - encounter_detect_sun_orb_cast → kick_primary_priority
    - encounter_detect_inferno_leap_cast → kick_primary_priority
    - encounter_detect_blessing_of_the_sun_channel → POP_ALL_OFFENSIVE_CDS (BURN_WINDOW)
    - encounter_detect_solar_winds_path_self → move_out
    - encounter_detect_solar_fire_patch_self_heroic → move_out

dispel_blacklist: []                      # NONE — Halls of Origination has no "do not dispel" entries

cooldown_save_priority:
  anhuur:    [no_save — minor rotational only]
  ptah:      [no_save — minor rotational only]
  anraphet:  [save Bloodlust if not used; pop on Crumbling Ruin late]
  setesh:    [pop offensive CDs on portal kills + Seed of Chaos windows]
  ammunae:   [save Bloodlust for Rajh; pop instant CDs on pod windows]
  isiset:    [save Bloodlust for Rajh; pop on familiar kill phases]
  rajh:      [BURN — Bloodlust + all offensive CDs in Blessing of the Sun window]
```

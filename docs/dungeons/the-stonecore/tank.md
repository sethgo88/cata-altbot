# The Stonecore — Tank

**Tank-specific response logic** for the bot's encounter handling. Mechanics, spell IDs, phase triggers, and add abilities are defined in `encounter.md`; this doc only encodes how the tank reacts. Class-agnostic where possible.

**Layering**:
- Cross-content tank patterns (threat priority, defensive CD pacing, taunt rules) live in `docs/roles/tank.md` (deferred to tank phase).
- Per-spec rotation / CD / threat model lives in the corresponding `docs/specs/` tank doc (all tank specs deferred to tank phase).
- This doc layers Stonecore-specific deviations on top of those.

---

## Dungeon-level overview

```yaml
dungeon_tank_overview:
  total_bosses: 4 (none optional)
  estimated_full_clear_minutes: 25-45
  threat_profile:
    millhouse_event: high                   # 3-5 mob trash; multi-target threat + add management
    corborus: moderate                      # tank holds boss + Rock Borer pickup in Burrow
    slabhide: low                           # tank-and-spank with cone facing
    ozruk: high                             # tank-against-wall + run through Ground Slam + Spike Shield bleed soak
    azil: moderate                          # tank-on-boss + Devout Follower add cleave; Force Grip lift interrupts threat briefly
  defensive_cd_profile:
    millhouse_event: moderate               # Spinning Slash damage, Fireball direct hits if missed kicks
    corborus: moderate                      # Rock Bore stack ramp; Dampening Wave timing
    slabhide: light                         # Sand Blast tank cone is normal
    ozruk: heavy                            # Spike Shield bleed peak + Shatter resolution + Rupture knockback
    azil: heavy                             # Force Grip slam if kick missed; Curse of Blood damage amp
  key_responsibilities:
    - threat_priority_per_fight
    - run_through_ground_slam (ozruk)
    - environmental_self_damage_management (ozruk Spike Shield bleed)
    - rock_borer_pickup (corborus)
    - devout_follower_add_pickup (azil)
    - face_boss_away_from_group (slabhide cone, trash cleaves)
    - millhouse_event_multi_target_threat (trash)
    - stage_2_target_swap_devout_followers (azil)
```

---

## Trash — tank notes

```yaml
trash_tank:
  millhouse_event:
    pull_pattern: |
      Tank pulls Millhouse and the 3-5 surrounding mobs into the
      corridor. Skull on Millhouse (ranged-DPS kill priority); CC on
      Earthshaper (sheep / hex / sap); CC on second Berserker if
      multi-CC available.
      DEFAULT: tank at corridor center; melee on tank-target; CC kept
      out of cleave range.
    facing: away from group (frontal cleaves on Berserker / Earthshaper Ground Shock)
    threat_management: |
      Multi-target threat: Thunder Clap / Swipe / Death and Decay /
      Avenger's Shield / Hammer of the Righteous on pull. Hold boss-
      group together; let CC trickle in.
    defensive_cd_use: |
      Pop short defensive (Shield Block / Barkskin / etc.) on opening
      threat-grab; rotate longer CD if Earthshaper Force of Earth
      fires (transformation = group damage spike).

  earthshaper_packs:
    pull_pattern: |
      Skull on Earthshaper (priority interrupt OR kill); CC on second
      Earthshaper if multiple. Force of Earth (5s cast) is the kick
      target for melee/ranged.
    notes: |
      If group fails to interrupt and Earthshaper transforms, tank pop
      defensive + healer pop cooldown + group AoE-burns the elemental
      form. The transformation is a group-wide damage spike.

  berserker_spinning_slash:
    notes: |
      Charge target prediction: Berserker Charge (81574) gap-closes to
      a player. Tank should be the threat target so Charge lands on
      tank (not a fragile DPS). Tank takes Charge → standard pickup.
      Spinning Slash 4s channel: tank stays in melee for threat (5y
      bleed is tank-tankable; healer reactive heal). DPS kites out.

  magmalord_packs:
    notes: |
      Standard tank-and-spank. Magma Eruption ground patches —
      reposition if standing in fire.

  flayer_packs:
    notes: |
      Flay AoE; tank takes the brunt as primary threat target. Defensive
      CD on multi-Flayer pull.

  crystalspawn_giants:
    notes: |
      Large physical mobs; tank-and-spank from front; melee positions
      side/rear. Potential immune to humanoid CC — handle as a pure
      tank target.
```

---

## Boss 1 — Corborus

```yaml
boss_tank:
  boss: corborus
  difficulty_grade: medium
  threat_intensity: moderate

  positioning:
    pull: tank Corborus in CENTER of room (so Burrow exit points are reachable)
    facing: away from group (no significant frontal cone, but standard discipline)
    movement: |
      Ground phase: minimal. Burrow phase: mobile to pick up Rock
      Borers at random spawn locations + dodge Thrashing Charges.

  threat_management:
    standard: |
      Tank-and-spank threat in ground phase. Be aware of Rock Borer
      threat in Burrow phase (small adds; tank picks up via AoE).
    rock_borer_pickup: |
      Rock Borers spawn at random player locations during Burrow.
      Tank: AoE-threat tools (Thunder Clap / Swipe / Death and Decay /
      Consecration / Avenger's Shield) to grab. Bot kicks AoE-threat
      reactively on Rock Borer add-spawn detection.
    crystal_shard_HEROIC: |
      Heroic adds spawn during Crystal Barrage. Ranged DPS handles AoE;
      tank doesn't need to grab them (they're not high-threat).

  defensive_cd_use:
    rock_bore_stack_ramp: |
      Rock Bore bleed (4k/3s heroic) stacks on tank. At 3-5 stacks,
      pop short defensive (Shield Block / Barkskin / Pain Suppression
      equivalent). At 8-10 stacks, pop major defensive (Shield Wall /
      Survival Instincts / Icebound Fortitude).
    dampening_wave_window: |
      Pop defensive ~1s before Dampening Wave hits (if cast bar
      visible) — heal-absorb prevents healer top-up, defensive reduces
      incoming damage to bridge to dispel.
    thrashing_charge_direct_hit: |
      If a Thrashing Charge clips tank during Burrow (rare; tank
      should be moving), pop major defensive + healer emergency heal.

  interrupt_role:
    none: no kick-required casts on boss

  heroic_delta: |
    - Rock Bore bleed deals 4k/3s; stack ramp is real
    - Dampening Wave heal-absorb 15k — defensive timing tighter
    - Crystal Shards heroic adds — ranged AoE handles; tank doesn't pick up

  bot_anti_patterns:
    - DO NOT chase Rock Borers OUT of group (they reach group via melee anyway)
    - DO NOT stand in Crystal Barrage splash zone (5y radius)
    - DO NOT chase Crystal Barrage target through 5y splash (their problem to dodge)
    - DO NOT stand in Thrashing Charge dust telegraph
```

---

## Boss 2 — Slabhide

```yaml
boss_tank:
  boss: slabhide
  difficulty_grade: easy (normal); medium (heroic)
  threat_intensity: low

  positioning:
    pull: pull boss to ONE SIDE of chamber (leave space for stalactite-pillar LoS mechanic later)
    facing: away from group (60° forward cone is Sand Blast)
    movement: |
      Ground phase: minimal. Air phase: maintain threat from below;
      reposition if stalactite drops nearby. Heroic Crystal Storm:
      LoS-break behind a stalactite pillar.

  threat_management:
    standard: tank-and-spank; cone facing
    sand_blast_cone: |
      60° forward cone. Tank face boss away from group. Rotate boss
      face during Lava Fissure under tank to keep cone safe (don't let
      cone clip group while moving off Lava Fissure).
    air_phase: |
      Slabhide is in the air during air phase; tank threat continues
      via reactive damage (taunt range works). Tank is NOT meleeing
      during air phase; reposition for upcoming Crystal Storm if heroic.

  defensive_cd_use:
    standard: rotational on Sand Blast tick + auto-attack
    crystal_storm_HEROIC_los_break: |
      During the 6s LoS-break, tank takes auto-attack damage via
      proximity (UNVERIFIED whether boss melees during channel — boss
      is channeling Crystal Storm, may not auto-attack). Default: pop
      short defensive on channel start; LoS-break full pillar.
    stalactite_direct_hit: |
      If a stalactite drops on tank, immediate damage spike — pop
      defensive + healer reactive heal.

  interrupt_role:
    none: no kick-required (Sand Blast / Lava Fissure instant; Crystal Storm mitigated by LoS)

  heroic_delta: |
    - Crystal Storm 6s LoS-break — tank participates in the LoS
      break, reposition
    - Lava Pool 30s persistence — ground hazard accumulates near
      tank-boss position
    - Stalactite damage harder; direct hit spike

  bot_anti_patterns:
    - DO NOT face boss into group (Sand Blast cone)
    - DO NOT stand in Lava Fissure crack (move boss off if fissure spawns under tank)
    - DO NOT skip Crystal Storm LoS-break on heroic (tank dies in 6s of unmitigated channel)
    - DO NOT chase Slabhide into the air (you can't melee flying boss)
```

---

## Boss 3 — Ozruk

```yaml
boss_tank:
  boss: ozruk
  difficulty_grade: hard (heroic); medium (normal)
  threat_intensity: high
  notes: |
    THE Cata-launch heroic skill check. Tank's primary roles:
    (1) Position Ozruk against a wall in the corridor (minimize knockback)
    (2) RUN THROUGH BOSS during Ground Slam (do NOT stay in front)
    (3) Manage Spike Shield bleed self-damage
    (4) Step to ≥ 5y from boss when Spike Shield expires (Shatter)
    (5) On heroic, ensure pre-applied DoT or bleed is on boss for
        Paralyze-break (or rely on Spike Shield bleed stacks self-applied
        from melee hits — see encounter.md Disagreement #5)

  positioning:
    pull: pull boss into the CORRIDOR; tank against a wall
    facing: away from group (standard); 4y forward arc is Ground Slam zone
    movement: |
      Continuous awareness of Ground Slam + Spike Shield + Bulwark cycle:
      - Ground Slam 3s cast → RUN THROUGH boss to behind
      - Spike Shield expires (≈10s) → step to ≥ 5y
      - Bulwark — tank doesn't deviate (Bulwark is spell-reflect; melee
        unaffected)
      - Default: stay in melee against wall

  threat_management:
    standard: maintain threat through ability cycle; defensive CDs on cooldown
    ground_slam_run_through: |
      3s cast = 3s window to move from wall through boss to other side.
      DO NOT just turn boss; the 4y forward arc is wherever boss is
      facing. Tank must physically move to behind.
      - Prot Warrior: Charge / Heroic Leap to other side
      - Prot Pally: walk through (no gap-close); 3s is plenty
      - Blood DK: Death Strike anchor; walk through
      - Feral Tank Druid (Bear): Charge / walk
    spike_shield_self_application_HEROIC: |
      Tank's melee hits during Spike Shield apply bleed stacks on self.
      On heroic, this is the Paralyze-break mechanism for tank (1-2
      stacks ticking during the 8s Paralyze break the stun).
      - Default: continue tank melee for 1-2 stacks during Spike Shield;
        then PAUSE melee for the rest of the 10s window.
      - Alternative: rely on existing class DoTs/bleeds (Vampiric
        Touch — N/A tank specs; Censure — Ret Pally only; Holy
        Vengeance — Ret only). Tank specs typically don't have a
        passive boss-DoT.
    spike_shield_self_application_NORMAL: |
      STOP MELEE for the 10s window. The bleed (300/sec) compounds
      pointlessly; pause melee. Maintain threat via auto-attack only;
      defensive abilities (Shield Block / Word of Glory / Death Strike /
      Frenzied Regeneration) usable.

  defensive_cd_use:
    spike_shield_peak: |
      At Spike Shield 7-8s mark, pop short defensive (Shield Block /
      Barkskin / Bone Shield / Holy Shield).
    shatter_resolution: |
      Step to ≥ 5y at 9s mark; defensive CD if step-out is partial.
    paralyze_HEROIC: |
      8s stun. Tank cannot defensive-CD during stun (locked). Pre-pop
      defensive BEFORE Spike Shield expires:
      - Shield Wall / Survival Instincts / Icebound Fortitude / Aura
        Mastery (Holy buff) — major CDs for the Paralyze duration
      - Pain Suppression external (Disc Priest, if available)
    rupture_knockback_HEROIC: |
      Post-Ground Slam, 3 spike lines erupt forward + knockback up.
      Tank already-moved-through avoids the lines. Knockback on tank
      from frontal positioning is rare (tank should be behind by Ground
      Slam resolution).
    bulwark_phase: |
      No tank deviation. Spell reflect doesn't affect melee. Continue
      threat rotation.

  interrupt_role:
    none: Ground Slam non-kickable (per multiple sources; one disputes — see Disagreement #4)

  heroic_delta: |
    - Paralyze added — tank pre-pops major defensive before stun
    - Spike Shield bleed 600/sec (vs 300/sec) — tank pause-melee window
      is more critical OR 1-2 stack acceptance for Paralyze break
    - Rupture knockback up — tank positioning pre-Ground-Slam matters
    - Bulwark 100% reflect — caster damage drops; tank's relative
      threat contribution rises (DPS pause hardcasts → threat ceiling
      relaxes)

  bot_anti_patterns:
    - DO NOT stay in 4y front arc during Ground Slam (run through)
    - DO NOT continue full melee during Spike Shield NORMAL (pointless bleed pile-up)
    - DO NOT stop melee fully during Spike Shield HEROIC (need 1-2 stacks for Paralyze break, OR tank specs may have no class DoT)
    - DO NOT stand within 5y of boss when Spike Shield expires (Shatter AoE — tank IS the boss anchor; usually farthest forward; step out matters)
    - DO NOT pull Ozruk into open room (knockback hazard); corridor + wall positioning is canonical
    - DO NOT pop defensive CD during Bulwark (no melee threat to tank from spell reflect)
```

---

## Boss 4 — High Priestess Azil

```yaml
boss_tank:
  boss: high_priestess_azil
  difficulty_grade: hard
  threat_intensity: moderate

  positioning:
    pull: tank Azil at center of platform (within kick range of melee)
    facing: away from group (standard)
    movement: |
      Stage 1: minimal. Stage 2: re-target Devout Followers (boss
      75% mitigated, in air; tank can't melee her). Avoid Gravity Well
      pull. Avoid Seismic Shard tombstones in Stage 2.

  threat_management:
    stage_1: |
      Standard tank-and-spank on Azil. Kick rotation participation:
      tank kick on Force Grip when off CD (Pummel / Avenger's Shield
      [silence] / Death Grip [interrupt? UNVERIFIED] / Skull Bash
      [Bear]).
    devout_follower_add_pickup: |
      Add waves spawn 1-3 times per Stage 1; 9-12 cultists per wave.
      Tank picks up via AoE threat (Thunder Clap / Swipe / Death and
      Decay / Avenger's Shield / Hammer of the Righteous). Hold add
      cluster near boss for cleave DPS.
    stage_2_transition: |
      Boss flies to altar; gains Energy Shield. Tank cannot melee her
      during Stage 2 (out of range). Tank PIVOTS to pick up Stage 2
      Devout Follower spawns and hold them away from group.

  defensive_cd_use:
    force_grip_unkicked: |
      If kick rotation misses Force Grip cast, tank takes 70k+ slam per
      tick for 5s (~280k+). Pop major defensive (Shield Wall / Survival
      Instincts / Icebound Fortitude / Aura Mastery) immediately on
      detection of unkicked Force Grip channel.
    curse_of_blood_on_self: |
      +5000 phys damage taken. Pop short defensive if Curse + auto-
      attacks compounding (~3-4 melee in Curse window = significant
      damage spike).
    stage_2_transition_knockback: |
      Energy Shield creation knockback hits 5y radius; tank usually
      within 5y. Pre-pop defensive on Stage 2 transition detection.
    seismic_shard_direct_hit: |
      Stage 2 tombstones hit ~942k (heroic) — wipe-tier on direct.
      Tank should be sidestepping; if predicted impact, pop major
      defensive.

  interrupt_role:
    force_grip: PARTICIPATE in kick rotation if class has interrupt
    rotation_partners: [tank_with_kick, melee_dps_PRIMARY, ranged_dps_with_kick, healer_with_kick]
    notes: |
      Tank kick options:
      - Prot Warrior: Pummel (10s CD; off-GCD; Berserker Stance? Cata
        warriors are stance-locked — UNVERIFIED Cata Pummel stance
        requirement)
      - Prot Pally: Avenger's Shield (silence); Rebuke (15s; Ret-spec
        only? UNVERIFIED Prot access)
      - Blood DK: Mind Freeze (10s; off-GCD)
      - Feral Tank Druid (Bear): Skull Bash (60s; long CD)

  heroic_delta: |
    - More Devout Followers per wave — AoE threat tools more important
    - Force Grip more punishing if kick missed — defensive CD timing
    - Gravity Wells require kills to shrink — tank holds adds NEAR
      wells for kill-funnel
    - Seismic Shard direct hit wipe-tier — tank sidestep tighter

  bot_anti_patterns:
    - DO NOT chase Azil during Force Grip lift (you're being held; just take the slam)
    - DO NOT skip kick rotation participation (every class kick counts)
    - DO NOT stand in Gravity Well (10y pull radius; ramp damage)
    - DO NOT stand at altar during Stage 2 transition (Energy Shield 5y knockback + 47k arcane)
    - DO NOT continue Azil melee in Stage 2 (boss out of range; focus Devout Followers)
```

---

## Bot AI hooks

```yaml
ai_hooks:
  corborus:
    - encounter_state_active → maintain_threat_in_center
    - encounter_detect_burrow_phase → AoE_threat_tools_on_rock_borer_spawn
    - encounter_detect_dampening_wave_imminent → pop_short_defensive
    - encounter_detect_rock_bore_stacks(self) >= 5 → pop_short_defensive
    - encounter_detect_thrashing_charge_dust_telegraph → move_perpendicular

  slabhide:
    - encounter_state_active → face_boss_away_from_group
    - encounter_detect_lava_fissure_under_tank → reposition_boss_off_crack_keep_cone_safe
    - encounter_detect_air_phase → maintain_threat_via_taunt_range
    - encounter_detect_crystal_storm_cast_HEROIC → move_to_pillar_los_break

  ozruk:
    - encounter_state_pull → tank_against_wall_in_corridor
    - encounter_detect_ground_slam_cast → run_through_boss
    - encounter_detect_spike_shield_aura_NORMAL → stop_melee
    - encounter_detect_spike_shield_aura_HEROIC → 1_2_melee_then_stop (deliberate bleed stacks for Paralyze break)
    - encounter_detect_spike_shield_8s → pop_short_defensive
    - encounter_detect_spike_shield_9s → step_to_5y
    - encounter_detect_paralyze_imminent_HEROIC → pop_major_defensive
    - encounter_detect_bulwark → no_action (melee unaffected)

  azil:
    - encounter_state_active_stage_1 → tank_boss_within_kick_range
    - encounter_detect_force_grip_cast → kick_if_class_has_kick
    - encounter_detect_force_grip_unkicked → pop_major_defensive
    - encounter_detect_devout_follower_spawn → AoE_threat_tools
    - encounter_detect_curse_of_blood_self_active → pop_short_defensive
    - encounter_detect_stage_2_transition → swap_target_to_devout_followers + pop_short_defensive
    - encounter_detect_seismic_shard_cast → sidestep_telegraph
    - encounter_detect_gravity_well_target → no_position_self_in_well

dispel_blacklist:
  - { spell_id: 92426, name: Paralyze (Ozruk heroic), reason: prefer DoT-damage-break over dispel; tank specs may rely on Spike-Shield-self-bleed for break, scope: ozruk_fight }

defensive_cd_priority_per_fight:
  millhouse_event: [reactive_on_earthshaper_force_of_earth_uninterrupted]
  corborus:        [reactive_on_rock_bore_stack_ramp; align_with_dampening_wave]
  slabhide:        [reactive_on_stalactite_direct_hit; pop_on_crystal_storm_HEROIC]
  ozruk:           [pre_shatter_short; pre_paralyze_major_HEROIC; rotation_through_spike_shield]
  azil:            [reactive_on_force_grip_unkicked; pre_stage_2_transition; rotation_on_curse_of_blood]

interrupt_priority_per_fight:
  millhouse_event: [{ spell_id: 81459, name: Force of Earth (Earthshaper), urgency: HIGHEST }]
  corborus:        []
  slabhide:        []
  ozruk:           []                           # Ground Slam non-kickable
  azil:            [{ spell_id: 79351, name: Force Grip, urgency: HIGHEST }]
```

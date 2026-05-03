# Lost City of the Tol'vir — Ranged DPS

**Ranged-DPS-specific response logic** for the bot's encounter handling. Mechanics, spell IDs, phase triggers, and add abilities are defined in `encounter.md`; this doc only encodes how ranged DPS reacts. Class-agnostic where possible.

**Layering**:
- Cross-content ranged DPS patterns (positioning, spread/stack rules, threat ceiling) live in `docs/roles/ranged-dps.md`
- Per-spec rotation lives in the active spec's `docs/specs/` doc (frost-mage, affliction-warlock, mm-hunter, shadow-priest, balance-druid, elemental-shaman)
- This doc layers Lost City of the Tol'vir-specific deviations on top of those

---

## Dungeon-level overview

```yaml
dungeon_ranged_dps_overview:
  total_bosses: 4
  estimated_full_clear_minutes: 30-45
  positioning_profile:
    husam:    follow_kite_path_max_range   # constant movement
    lockmaw:  side_position_spread          # never behind boss; spread for adds
    barim:    spread                        # Plague of Ages bounce limit; Soul Fragment intercept
    siamat:   spread_stay_center            # Chain Lightning arcs + Wailing Winds knockoff
  interrupt_load:
    husam:   none                           # no kicks
    lockmaw: none                           # no kicks
    barim:   none                           # no kicks
    siamat:  HIGH                           # Minion Chain Lightning primary kick target
  key_responsibilities:
    - kite_path_movement (husam)
    - side_position_spread (lockmaw — never behind)
    - soul_fragment_intercept (barim P2 — ranged kills fastest)
    - blaze_of_the_heavens_handle (barim heroic P1 — ranged kites/burns)
    - minion_chain_lightning_interrupt (siamat — primary kick rotation)
```

---

## Trash — ranged DPS notes

```yaml
trash_ranged:
  tolvir_stoneshapers:
    cc_priorities: |
      Ranged contributes opener-CC and interrupts:
      - Frost Mage: Polymorph (Sheep) — primary CC; Counterspell (24s) interrupt
      - Affliction Warlock: Banish if elemental (UNVERIFIED creature_type); Spell Lock via Felhunter pet
      - MM Hunter: Freezing Trap on caster; Silencing Shot (24s) interrupt
      - Shadow Priest: Shackle Undead if undead-flavor (UNVERIFIED); Silence (45s) interrupt
      - Balance Druid: Cyclone (8s); Solar Beam (60s; 8s AoE silence)
      - Elemental Shaman: Hex (60s, breaks on damage) — SAFEST CC given humanoid/elemental ambiguity; Wind Shear (6s; off-GCD; 25y) interrupt
    interrupt_role: |
      Stoneshaper Earth Shock-flavor casts are MUST_INTERRUPT — primary kick
      target on trash. Every ranged spec contributes a kick.

  neferset_plaguebringers:
    cc_priorities: |
      Same humanoid CC list as Stoneshapers (probably humanoid).
      Polymorph / Hex / Banish (if elemental) / Sap.
    interrupt_role: |
      Disease-flavor casts on trash — interrupt to limit dispel pressure on
      group during clear.

  oathblade_axemaster_packs:
    notes: melee+cleave packs; ranged stays at range; standard ST

  pre_siamat_trash:
    notes: |
      Wind-flavor adds; mostly tank-and-spank. Spread for any AoE casts.
```

---

## Boss 1 — General Husam

```yaml
boss_ranged:
  boss: general_husam
  difficulty_grade: medium (movement-heavy)
  positioning:
    default: |
      Max range from boss; FOLLOW the kite path. Tank kites Husam in a slow
      circle around the chamber — ranged stays just inside the kite radius
      so they don't fall behind or out of healer range.
    facing: boss (cast direction)
    movement: |
      CONSTANT MOVEMENT. Ranged bot must:
      - Move off Shockwave X-pattern lines (visible ground markers)
      - Move off arming Mystic Trap zones
      - Heroic: clear away from existing trap zones during Detonate Traps countdown
      Cast-while-moving classes preferred:
      - Frost Mage: Frostbolt hardcast (stop-cast on movement); Ice Lance instant on Fingers proc
      - Affliction Warlock: DoTs all instant; Drain Soul channel breaks on movement (resume when stable)
      - MM Hunter: Steady Shot hardcast; Aspect of the Fox enables hardcasting on the move (UNVERIFIED Cata talent name)
      - Shadow Priest: DoTs instant; Mind Flay channel mobile-friendly (interruptible by movement break)
      - Balance Druid: Wrath/Starfire hardcast; Moonfire/Sunfire instant; Lunar Shower talent boosts mobile DPS
      - Elemental Shaman: Lightning Bolt hardcast (stop-cast on movement); Flame Shock instant; Lava Burst hardcast
  interrupt_role:
    none: no kick-required casts
  rotation_modifications:
    standard: |
      Standard ST rotation modulated by movement breaks. Use procs and
      instants between movements:
      - Frost Mage: Brain Freeze + Fingers of Frost procs are mobile windows
      - Affliction Warlock: Drain Soul on stationary windows; DoTs maintained instant
      - MM Hunter: Aimed Shot procs; Steady Shot during stationary windows
      - Shadow Priest: Mind Flay during stationary; DoT maintenance instant
      - Balance Druid: Eclipse-driven; Moonfire/Sunfire instant during movement
      - Elemental Shaman: instant Lava Burst on Lava Surge proc; FS maintenance
    detonate_traps_heroic: |
      During 5s countdown: STOP DPS, MOVE to clear ground; resume DPS when
      countdown ends.
  cd_use:
    save_for_real_bosses: standard offensive CDs rotational; no special hold
  heroic_delta: |
    - More movement (more traps, Detonate Traps event)
    - Mystic Trap damage scales — collateral if caught
  bot_anti_patterns:
    - DO NOT stand still to hardcast (trap zones spawn at random positions; mobility = survival)
    - DO NOT lag behind kite path (out of healer range)
    - DO NOT path through arming Mystic Traps (visible ground markers)
    - DO NOT cast Knockback abilities (Thunderstorm, Typhoon) — disrupts tank kite path
```

---

## Boss 2 — Lockmaw (and Augh)

```yaml
boss_ranged:
  boss: lockmaw
  difficulty_grade: medium (positional + add-cleave)
  positioning:
    default: |
      Stand at SIDE of boss — NEVER behind (Dust Flail rear cone), NEVER
      directly in front (close-range hazard if pulled in).
      For Lockmaw: tank usually pulls boss to wall facing in; ranged spreads
      at side angles outside 5y of melee.
      For Augh: stay at max range; back off Whirlwind 5y radius.
    facing: boss
    movement: |
      Move out of Viscous Poison ground patches (5y radius).
      Spread for Scent of Blood adds (don't cluster — adds path to bleed target).
  interrupt_role:
    none: no kick-required casts
  rotation_modifications:
    add_handling_scent_of_blood: |
      AoE rotation when Frenzied Crocolisks spawn (2-4 adds typical):
      - Frost Mage: Blizzard / Frostfire Bolt cleave; Frost Nova for root
      - Affliction Warlock: Seed of Corruption + Hellfire (UNVERIFIED Hellfire as Affliction tool); DoT spread via Soul Swap
      - MM Hunter: Multi-Shot / Trap Launcher Explosive Trap
      - Shadow Priest: Vampiric Touch + Shadow Word: Pain spread; Mind Sear poor at 4 targets
      - Balance Druid: Hurricane (channel) / Wild Mushroom detonation
      - Elemental Shaman: Magma Totem at boss feet; Chain Lightning at 5+ MW (UNVERIFIED — Ele MW model differs from Enh)
    augh_phase: |
      Standard ST rotation on Augh. Step out of Whirlwind 5y radius.
      Smoke Bomb stuns nearby — ranged at 25+ y is safe.
    venomous_rage_30_pct: |
      Lockmaw +25% damage; group takes more damage. Ranged continues ST DPS;
      no rotation change but burst CDs may pop here.
  cd_use:
    bloodlust_or_heroism: |
      If group leader calls for Lust on Lockmaw 30% (Venomous Rage): use it.
      Otherwise hold for execute window or save for Siamat P2.
    survival_cooldowns: |
      Pop major defensive if Viscous Poison lands on self AND HP drops below 60%:
      - Frost Mage: Iceblock (5min CD)
      - Aff Warlock: Healthstone; Soulstone self pre-fight
      - MM Hunter: Disengage; Master's Call (UNVERIFIED)
      - Shadow Priest: Dispersion
      - Balance Druid: Barkskin
      - Ele Shaman: Stoneclaw Totem (talented)
  heroic_delta: |
    - Augh full second-phase fight — sustained DPS demand
    - More crocolisk adds; AoE rotation more important
    - Viscous Poison damage 2x — defensive CDs on rotation if caught
  bot_anti_patterns:
    - DO NOT stand behind Lockmaw (Dust Flail = severe damage + 50% accuracy)
    - DO NOT stand in Viscous Poison ground patches
    - DO NOT cluster ranged together (Scent of Blood adds path to bleed target — cluster catches collateral)
    - DO NOT continue casting hardcasts during Smoke Bomb stun (wasted GCD)
```

---

## Boss 3 — High Prophet Barim

```yaml
boss_ranged:
  boss: high_prophet_barim
  difficulty_grade: medium (phase transition + Soul Fragment role)
  positioning:
    default: |
      P1: max range from boss; SPREAD ~8y between players (Plague of Ages
      bounce limit). Move off Heaven's Fury zones.
      P2: max range from Harbinger; PRIORITIZE Soul Fragment kill positioning
      (between Soul Fragment spawn point and Harbinger).
      P1 resume: re-engage Barim from spread position.
    facing: boss / Harbinger / Soul Fragment depending on phase
    movement: |
      P1: move off Heaven's Fury (12y zones); maintain spread.
      Repentance transition: BE READY to move (group stunned 6s, then must
      step out of Hallowed Ground zone).
      P2: position to intercept Soul Fragments.
  interrupt_role:
    none: no kick-required casts
  rotation_modifications:
    phase_1_standard: |
      Standard ST rotation on Barim from max range with movement for Heaven's
      Fury avoidance. DoT-spec classes (Aff Warlock, Shadow Priest, Balance
      Druid) maintain DoTs through movement.
    phase_1_blaze_of_the_heavens_heroic: |
      Phoenix add appears on heroic P1. Two strategies (group leader call):
      A) Burn — ranged DPS focuses Blaze; melee continues Barim
      B) Kite — slow-burn over the fight; ranged ignores until Barim 50%
      Default for bot: A) BURN if ranged group has 2+ casters with consistent ranged DPS
      - Frost Mage: Frostbolt + Frostfire on Blaze
      - Aff Warlock: DoT-swap to Blaze + Drain Soul if execute
      - MM Hunter: full ST rotation on Blaze
      - Shadow Priest: DoT swap to Blaze
      - Balance Druid: Moonfire/Sunfire on Blaze + Eclipse rotation
      - Ele Shaman: Flame Shock + LB on Blaze
    repentance_transition: |
      6s group stun at 50% HP. NO OUTPUT during stun. As stun breaks:
      - Move out of Hallowed Ground zone (13y radius around Barim)
      - Re-target Harbinger of Darkness (new active threat)
      - DoT classes: re-apply DoTs on Harbinger (most DoTs reset on target swap)
    phase_2_soul_fragment_priority: |
      RANGED IS PRIMARY SOUL FRAGMENT KILL ROLE. Soul Fragments spawn from
      Soul Sever (every ~4s) and move toward Harbinger. Ranged DPS:
      - Switch target to Soul Fragment immediately on spawn
      - Use slow / stun / burst to kill before Soul Fragment reaches Harbinger
      - Frost Mage: Frostbolt slow built-in; Frost Nova root if Fragment in melee range
      - Aff Warlock: Curse of Exhaustion (slow); Death Coil (fear-stun)
      - MM Hunter: Concussive Shot (slow); Wing Clip (melee)
      - Shadow Priest: Mind Flay slow built-in
      - Balance Druid: Solar Beam silence/slow; Cyclone disorient
      - Ele Shaman: Earthbind Totem (slow); Chain Lightning burst
      Once Fragment dead, return to Harbinger DPS.
    phase_1_resume: |
      Harbinger dies → Barim re-engageable. Re-target Barim; resume P1
      rotation. DoT classes re-apply DoTs on Barim (target swap reset).
  cd_use:
    bloodlust_or_heroism: |
      Save for Siamat P2 — Barim is medium-pace; not the burn moment.
    survival_cooldowns: |
      Pop defensive if Plague of Ages lands AND HP drops below 60%:
      - Frost Mage: Iceblock
      - Aff Warlock: Healthstone + Felhunter Spell Lock if dispel ally fails
      - Shadow Priest: Dispersion
      - Others: standard
  heroic_delta: |
    - Damage scales (Plague of Ages 2x)
    - Blaze of the Heavens add (heroic P1) — burn or kite decision
    - Soul Fragments more numerous / faster — ranged Soul Fragment role tighter
  bot_anti_patterns:
    - DO NOT cluster ranged together (Plague of Ages bounces; cluster = group infection)
    - DO NOT stand in Heaven's Fury zones to maintain DPS (move first, DPS second)
    - DO NOT attempt to dispel Plague of Ages via ranged-class dispels (priority is healer; only specific specs can Disease-cleanse)
    - DO NOT continue hardcasting on Barim during Repentance stun (wasted GCD)
    - DO NOT keep DPS on Barim after he becomes immune in P2 (switch to Harbinger / Fragments)
```

---

## Boss 4 — Siamat

```yaml
boss_ranged:
  boss: siamat
  difficulty_grade: medium (interrupt-heavy + positional)
  positioning:
    default: |
      P1: max range from Siamat; SPREAD (Chain Lightning arcs jump 10y).
      Stay >10y from active Servant (Lightning Nova).
      P2: SAME as P1 + STAY CENTER OF PLATFORM (Wailing Winds knockoff
      hazard). Avoid Tempest Storm clouds (do not let reach Siamat).
    facing: active target (Servant in P1; Siamat in P2)
    movement: |
      P1: minimal — spread maintained, occasional Cloud Burst dodge
      P2 entry (Wailing Winds 6s): STAY CENTER, brace for knockback
      P2: move to clear Tempest Storms from Siamat path
  interrupt_role:
    minion_chain_lightning: |
      PRIMARY KICK TARGET on this fight. Minions of Siamat continuously
      hardcast Chain Lightning (103637) — interrupt rotates among ranged DPS.
      - Frost Mage: Counterspell (24s; off-GCD; 30y)
      - Aff Warlock: Spell Lock via Felhunter pet (24s; pet ability)
      - MM Hunter: Silencing Shot (24s; off-GCD; 35y)
      - Shadow Priest: Silence (45s; off-GCD; 30y)
      - Balance Druid: Solar Beam (60s; ground-place AoE silence)
      - Ele Shaman: Wind Shear (6s; off-GCD; 25y) — SHORTEST CD; primary kick rotation
      Coordinate with other ranged: Wind Shear primary, others on rotation when needed.
  rotation_modifications:
    phase_1_servant_priority: |
      KILL ORDER: Servants of Siamat in spawn order (3 total). Tank picks
      up; ranged contributes ranged DPS from outside 10y radius.
      DPS is ABSOLUTELY GATED on Servant kills — Siamat takes 90% reduced
      damage during P1 (Deflecting Winds).
      DO NOT WASTE DPS ON SIAMAT IN P1.
    minion_handling: |
      Minions self-deplete via Depletion (-10% HP / 2s). DO NOT KILL.
      DPS does NOT target Minions. Interrupt Chain Lightning as a free
      action when off CD; otherwise let them die.
      EXCEPTION: if Minion is at 5% HP and about to form Tempest Storm in
      a bad spot (path to Siamat in P2), kite or stun briefly. Does NOT
      mean burst-DPS; just terrain control.
    phase_2_siamat_burn: |
      Wailing Winds 6s entry → Siamat attackable. Standard ST rotation:
      - Frost Mage: Frostbolt + Ice Lance + procs
      - Aff Warlock: full DoT ramp + Haunt + UA + BoA + Corr; Drain Soul execute
      - MM Hunter: Steady Shot + Aimed Shot rotation; Rapid Fire / Readiness CDs
      - Shadow Priest: VT + SW:P + DP + Mind Blast Shadow Orb consumer
      - Balance Druid: Eclipse-bar driven rotation
      - Ele Shaman: 6-tier rotation (Lava Surge → LvB → ES Fulmination → FS maint → ES Fulmination spend → LB filler)
      Watch for Cloud Burst spawn under self → move out.
      Watch for Tempest Storm clouds → kite away from Siamat.
    gathered_storms_p2_heroic: |
      Stacks on group; heroic = all players; +33% damage taken risk if
      Lightning Charge stacks layered. Continue rotation; rely on healer
      AoE heal.
  cd_use:
    bloodlust_or_heroism: |
      P2 ENTRY is the canonical Lust moment. After Wailing Winds resolves
      and Siamat is attackable, group leader calls Lust → align offensive
      CDs:
      - Frost Mage: Mirror Image + Icy Veins
      - Aff Warlock: Demon Soul (UNVERIFIED Cata) + Bloodlust
      - MM Hunter: Rapid Fire + Bestial Wrath (talent — UNVERIFIED MM access)
      - Shadow Priest: Shadowfiend / Mindbender + Power Infusion (talent)
      - Balance Druid: Force of Nature treants + Starfall
      - Ele Shaman: Elemental Mastery + Spiritwalker's Grace
    survival_cooldowns: |
      Pop defensive on heroic Gathered Storms if all-target spike + low HP:
      - Frost Mage: Iceblock
      - Aff Warlock: Healthstone
      - MM Hunter: Aspect of the Hawk + Deterrence (UNVERIFIED Cata)
      - Shadow Priest: Dispersion
      - Balance Druid: Barkskin
      - Ele Shaman: Stoneclaw Totem (talented)
  heroic_delta: |
    - Lightning Charge stacks on group (achievement-relevant; ignore for survival)
    - Gathered Storms hits ALL players — major P2 spike
    - Minion Chain Lightning interrupt MANDATORY (group cleave damage if missed)
    - Tighter mechanic windows
  bot_anti_patterns:
    - DO NOT DPS Siamat in P1 (Deflecting Winds = 90% DR; wasted GCDs)
    - DO NOT DPS Minions of Siamat (self-deplete; wastes damage that should hit Servants)
    - DO NOT cluster ranged together (Chain Lightning arcs)
    - DO NOT step off platform during Wailing Winds (knockoff = wipe)
    - DO NOT stand within 10y of active Servant (Lightning Nova)
    - DO NOT stand in Cloud Burst clouds
    - DO NOT let Tempest Storms reach Siamat in P2 (Gathered Storms trigger)
```

---

## Bot AI hooks

```yaml
ai_hooks:
  husam:
    - encounter_state_active → follow_kite_path_max_range
    - encounter_detect_shockwave_telegraph → move_off_x_pattern_lines
    - encounter_detect_mystic_trap_landed_near_self → move_clear_radius
    - encounter_detect_detonate_traps_countdown_heroic → move_to_clean_ground

  lockmaw:
    - encounter_state_active → side_position_outside_5y_melee
    - encounter_detect_viscous_poison_under_self → move_out_of_patch
    - encounter_detect_scent_of_blood_adds_spawned → swap_to_aoe_rotation
    - encounter_detect_smoke_bomb_telegraph → move_outside_5y
    - encounter_state_augh_phase_active → max_range_dps_augh

  barim:
    - encounter_state_phase_1_active AND heroic AND blaze_of_the_heavens_alive → dps_blaze_priority
    - encounter_state_barim_hp_pct < 55 → brace_for_repentance_stun
    - encounter_detect_repentance_stun_active → no_action
    - encounter_state_phase_2_active → swap_target_to_soul_fragment_on_spawn
    - encounter_detect_soul_fragment_spawn → priority_kill_with_slow_or_burst
    - encounter_detect_soul_fragment_dead → return_target_to_harbinger
    - encounter_detect_harbinger_dead → swap_target_back_to_barim

  siamat:
    - encounter_state_phase_1_active → swap_target_to_active_servant
    - encounter_state_servant_active → maintain_distance_10y
    - encounter_detect_minion_chain_lightning_cast → kick_rotation_participate  PRIMARY
    - encounter_state_minion_active → ignore_unless_interruption_window
    - encounter_state_third_servant_dead → brace_for_wailing_winds_stay_center
    - encounter_detect_wailing_winds_active → continue_dps_or_minimal_movement_stay_center
    - encounter_state_phase_2_active → swap_target_to_siamat
    - encounter_detect_cloud_burst_under_self → move_out_of_radius
    - encounter_detect_tempest_storm_path_to_siamat → kite_or_avoid

dispel_blacklist: []                                # no debuffs to BLACKLIST in this dungeon
                                                    # (ranged hybrid healers' Plague-of-Ages dispel is HEALER role)

interrupt_priority_per_fight:
  husam:    [no_kicks]
  lockmaw:  [no_kicks]
  barim:    [no_kicks]
  siamat:   [Minion Chain Lightning (103637) — PRIMARY]
```

---

## Bot-implementation notes

- **Siamat Minion-vs-Servant target discrimination**: Minions and Servants are both Siamat-summoned adds, but they have OPPOSITE DPS rules — Servants are kill-priority absolute (P1 gate); Minions are DO-NOT-KILL (self-deplete). Bot ranged DPS targeting logic must distinguish them by NPC ID (Servant 45269 vs Minion 44704), not by visual appearance or proximity.
- **Barim phase-transition target swap**: ranged DPS bot must NOT continue DPS on Barim after Repentance fires — Barim is immune in P2. Bot detects via `target.is_immune` or via Harbinger-spawn event, swaps to Soul Fragment priority then Harbinger.
- **DoT-class target swaps (Affliction, Shadow Priest, Balance Druid)**: Plague of Ages bounces, Barim P2 transition, and Soul Fragment intercept all force target swaps. Bot DoT-class rotation must accept that DoTs reset on each target change; no time to ramp full DoT load on Soul Fragments (kill them with burst, not DoTs).
- **Siamat platform knockoff defensive**: bot ranged DPS bot's positioning logic must include a "stay-within-platform-center-radius" rule active during Wailing Winds — ranged's default "max range from boss" rule would push them to platform edge, which is the knockoff zone. Override: cap range at platform-center radius during Wailing Winds.

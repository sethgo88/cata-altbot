# Lost City of the Tol'vir — Tank

**Tank-specific response logic** for the bot's encounter handling. Mechanics, spell IDs, phase triggers, and add abilities are defined in `encounter.md`; this doc only encodes how the tank reacts. Class-agnostic where possible.

**Layering**:
- Cross-content tank patterns (threat priority, defensive CD pacing, taunt rules) live in `docs/roles/tank.md` (deferred to tank phase).
- Per-spec rotation / CD / threat model lives in the corresponding `docs/specs/` tank doc (all tank specs deferred to tank phase).
- This doc layers Lost City of the Tol'vir-specific deviations on top of those.

---

## Dungeon-level overview

```yaml
dungeon_tank_overview:
  total_bosses: 4
  estimated_full_clear_minutes: 30-45
  threat_profile:
    husam:    moderate                    # standard tank-and-spank with kite path
    lockmaw:  high                        # face management critical (front + back hazards); add pickup
    barim:    moderate                    # phase-2 target swap (Barim → Harbinger → Barim)
    siamat:   moderate                    # Servant pickup; Minion ignore; P2 boss tank
  defensive_cd_profile:
    husam:    moderate                    # Hammer Fist combo on cadence
    lockmaw:  high                        # Lockmaw 30% Venomous Rage; Augh Smoke Bomb stun
    barim:    moderate                    # Fifty Lashings active windows
    siamat:   moderate                    # Servant Thunder Crash incoming damage
  key_responsibilities:
    - kite_pattern (husam)
    - lockmaw_face_management (UNIQUE — face into wall to bury rear cone)
    - smoke_bomb_stun_relay (augh — DPS taunt during 3s window)
    - phase_2_target_swap (barim — Barim → Harbinger → Barim)
    - servant_pickup (siamat — 3 sequential adds)
    - stay_center_wailing_winds (siamat P2 entry — knockoff hazard)
```

---

## Trash — tank notes

```yaml
trash_tank:
  caster_packs:
    pull_pattern: |
      Tol'vir Stoneshapers, Neferset Plaguebringers, Neferset Theurgists are
      caster trash. Default pull pattern: skull on lead caster (kill priority),
      cross/X on second (CC). Tank pulls all melee mobs and rounds them up
      while CC takes the second caster.
    facing: away from group (frontal cleaves on Tol'vir Oathblade and
            Oathsworn Axemaster trash)
  oathblade_axemaster_packs:
    notes: large melee mobs; cleave from front; tank-and-spank with face
           management
  fear_packs:
    notes: |
      Tol'vir / Neferset trash with fear-flavor casts. Tank stays in center
      while DPS interrupts. If fear lands, healer can't respond — tank does
      NOT chase strays.
  pre_siamat_trash:
    notes: |
      Wind-flavor adds; tank-and-spank with standard pull patterns.
```

---

## Boss 1 — General Husam

```yaml
boss_tank:
  boss: general_husam
  difficulty_grade: medium (kite-pattern)
  threat_intensity: moderate

  positioning:
    pull: pull boss to the OUTER EDGE of the chamber, then begin slow circle
    facing: away from group; rotate facing as boss moves
    movement: |
      KITE PATTERN — slow circle around the chamber so trap zones accumulate
      BEHIND the group, not where the group is moving toward. Cadence:
      - Don't park; constant slow movement
      - Heroic: tighter cadence due to more traps
      - Adjust circle direction if a Mystic Trap lands directly in path

  threat_management:
    standard: maintain threat through the kite; group follows tank's path
    bad_intentions_charge: |
      Boss charges a random non-tank player and slams them. Tank does NOT
      need to react with taunt — the charged player IS targeted and slammed,
      then boss returns to tank. Continue threat rotation.

  defensive_cd_use:
    hammer_fist_combo:
      timing: predictable cadence (every ~12-15s UNVERIFIED)
      response: pop short defensive (Shield Block / Barkskin / Bone Shield refresh / Holy Shield equivalent) on combo cast
    bad_intentions_landed_on_tank: |
      RARE — Bad Intentions targets non-tank typically. If tank charged
      (rare), pop major defensive and accept the slam.
    detonate_traps_heroic: |
      During 5s countdown: move boss to clean ground (tank is leading the
      kite path) — don't park during the countdown.

  interrupt_role:
    none: no kick-required casts on boss

  fight_specific_override:
    kite_pattern: |
      ACTIVE ENVIRONMENTAL HAZARD AVOIDANCE. Tank bot must:
      - Distinguish "trap zone = avoid path" from generic "fire on floor = avoid"
      - Lead the group's movement (group follows tank)
      - Constant slow movement; do NOT park boss
      Bot tank logic: rather than holding position, maintain a "kite-path"
      target waypoint that updates every ~3-5s based on trap-zone positions.

  heroic_delta: |
    - Detonate Traps premature mass-detonation event (5s countdown) requires
      tank-led group relocation during the countdown
    - More traps spawned per cast — less clean ground
    - Mystic Trap damage scales (34,124 → 48,749) — accidental trap-step is lethal
    - Tank kite-pattern is mandatory (vs optional on normal where stationary
      tank can survive with sloppy positioning)

  bot_anti_patterns:
    - DO NOT park Husam in one spot (traps accumulate around boss)
    - DO NOT path through arming Mystic Traps (visible markers)
    - DO NOT face boss into the group when rotating (Hammer Fist is melee
      anyway, but don't cleave-spillover any frontal abilities — UNVERIFIED
      whether Husam has a frontal cone)
    - DO NOT skip Hammer Fist defensive (predictable spike)
```

---

## Boss 2 — Lockmaw (and Augh)

```yaml
boss_tank:
  boss: lockmaw
  difficulty_grade: HARD (face management + 2-phase + add pickup)
  threat_intensity: high

  positioning:
    pull: |
      Pull Lockmaw to a WALL. Face boss INTO the wall — tank stands between
      boss and wall.
      Why: Lockmaw's TAIL is the rear cone (Dust Flail). Facing into wall
      means the tail points back at the wall (no group in line of fire).
      Group SIDE-stands at 90 degrees from boss facing.
    facing: |
      Lockmaw: into the wall (tank is behind boss, facing wall, boss faces wall)
      WAIT — re-reading: tank IS the front of the boss (boss attacks tank).
      Correct positioning:
        - Tank stands at the wall facing AWAY from wall (toward room)
        - Boss faces tank (toward wall)
        - Boss's tail points INTO the wall — rear cone hits wall, not group
      This is the only safe configuration; tail = rear cone, head = front
      (where tank is).
    movement: minimal during Lockmaw phase; tank holds position at wall
    augh_phase: |
      Augh phase: tank kites Augh in a loop or holds in a clear area.
      Whirlwind hazard means melee back off; tank holds aggro through
      Whirlwind via standard threat tools.

  threat_management:
    lockmaw: standard tanking; face boss away from group is wrong here —
             face boss INTO wall; ensure group is at SIDE
    crocolisk_adds: |
      Frenzied Crocolisks spawn from Scent of Blood and fixate on bleed
      target (random player). Tank picks up via:
      - Taunt nearest add OR
      - AoE threat tools (Thunder Clap / Swipe / Death and Decay /
        Consecration / Avenger's Shield)
      Adds are low-HP; group AoE-burns them while tank holds aggro.
    augh_pickup: |
      Augh becomes attackable when Lockmaw dies. Tank must:
      - Detect Augh-attackable transition (target.is_attackable(Augh))
      - Pull aggro on Augh immediately (taunt or initial threat strike)
      - Position Augh in a clear area (Whirlwind kite-around)

  defensive_cd_use:
    venomous_rage_30_pct: |
      At Lockmaw 30% HP: +25% damage. Pop major defensive (Shield Wall /
      Survival Instincts / Icebound Fortitude / Divine Protection / Last
      Stand) for the burn-down window.
    augh_smoke_bomb: |
      3s tank stun on Augh's Smoke Bomb. Tank cannot use active mitigation
      during stun. Pre-shield BEFORE Smoke Bomb cast warning if telegraphed;
      DPS taunt-relay if available (Hand of Reckoning / Dark Command /
      Growl Bear-form / etc.) to off-tank during stun.
    augh_whirlwind: |
      Whirlwind ticks high physical damage on melee. Tank either kites or
      pops major defensive to absorb. Healer pre-HoT before Whirlwind
      window.

  interrupt_role:
    none: no kick-required casts on Lockmaw or Augh

  fight_specific_override:
    lockmaw_face_management: |
      EXPLICIT OVERRIDE of standard "face boss away from group" rule. Lockmaw
      has a REAR cone (Dust Flail), not a front cone. Tank must face boss
      INTO a wall so the tail (rear cone) is buried in the wall.
      Group SIDE-stands. This is the ONLY fight in the dungeon (and one of
      few in Cata) with rear-cone-positioning logic.

  heroic_delta: |
    - Augh full second-phase fight; sustained tank-on-Augh time
    - Viscous Poison damage 2x (healer pressure but not directly tank-side)
    - Venomous Rage 30% spike higher
    - More crocolisk adds; tank AoE-threat tools more important

  bot_anti_patterns:
    - DO NOT face Lockmaw away from a wall (rear cone hits group)
    - DO NOT chase Augh through Whirlwind (kite or back off)
    - DO NOT skip Venomous Rage 30% defensive (tank takes extra 25% damage)
    - DO NOT engage Augh while Lockmaw alive (Augh is immune)
    - DO NOT taunt boss off the wall mid-fight (positioning resets)
```

---

## Boss 3 — High Prophet Barim

```yaml
boss_tank:
  boss: high_prophet_barim
  difficulty_grade: medium (phase-2 target swap)
  threat_intensity: moderate

  positioning:
    pull: tank Barim near room center; facing away from group
    facing: away from group (standard)
    movement: minimal in P1; in P2, position Harbinger AWAY from Barim's
              location (Hallowed Ground zone center)

  threat_management:
    phase_1: standard tanking; face boss away
    repentance_transition: |
      6s group AoE stun at 50% HP. Tank cannot do anything during stun.
      Threat resets are unlikely (Repentance is positional, not aggro
      reset), but ensure threat lead before 50% HP.
    phase_2_harbinger_pickup: |
      Harbinger of Darkness spawns at start of P2. Tank must:
      - Detect Harbinger spawn (NPC ID 43927)
      - Re-target Harbinger immediately (Barim is now immune)
      - Pull aggro on Harbinger via taunt or threat strike
      - Position Harbinger AWAY from Hallowed Ground zone (move tank +
        Harbinger together to clear ground)
    phase_1_resume: |
      Harbinger dies → Barim re-engageable. Tank re-acquires Barim.
      Re-target Barim and re-establish threat lead.

  defensive_cd_use:
    fifty_lashings_active: |
      Self-buff on Barim: next 10 melee swings deal +5,000 bonus Physical
      damage. When buff visible on Barim, tank pops short-CD defensive
      (Shield Block / Barkskin / Bone Shield refresh) for the 10-swing
      window.
    repentance_stun: |
      Cannot use active mitigation during 6s stun. Pre-shield with
      reactive cooldown immediately before 50% HP threshold.
    phase_2_harbinger_burn: |
      Standard tanking on Harbinger; defensive on rotation per Wail of
      Darkness ticks.

  interrupt_role:
    none: no kick-required casts

  fight_specific_override:
    target_swap: |
      Tank target must change on phase transitions:
      - P1: target = Barim
      - P2: target = Harbinger of Darkness (NPC 43927)
      - P1 resume: target = Barim
      Bot tank logic must detect target-immune state on Barim and active
      Harbinger spawn as the swap signal.

  heroic_delta: |
    - Damage scales (Plague of Ages 2x; Heaven's Fury implied scaling)
    - Blaze of the Heavens phoenix add in Phase 1 — RANGED DPS handles;
      tank does NOT taunt (kept at range)
    - Soul Fragments more numerous / faster — ranged handles; tank
      continues Harbinger DPS

  bot_anti_patterns:
    - DO NOT continue DPS / threat actions on Barim during P2 (immune)
    - DO NOT taunt Blaze of the Heavens (kited by ranged)
    - DO NOT stand in Hallowed Ground zone with Harbinger
    - DO NOT skip Fifty Lashings defensive (predictable spike)
    - DO NOT chase Soul Fragments (ranged DPS kill priority)
```

---

## Boss 4 — Siamat

```yaml
boss_tank:
  boss: siamat
  difficulty_grade: medium (sequential adds + P2 target swap + knockoff awareness)
  threat_intensity: moderate

  positioning:
    pull: pull Siamat near the CENTER of the platform
    facing: away from group (standard)
    movement: |
      P1: hold Siamat center; bring active Servant to tank position
      P2 entry (Wailing Winds): STAY CENTER (knockoff hazard); brace for
        knockback
      P2: hold Siamat center; clear path of Tempest Storms

  threat_management:
    siamat_p1: hold aggro (won't be hit much due to Deflecting Winds 90% DR
              on incoming damage)
    servant_pickup: |
      Three Servants of Siamat spawn sequentially in P1 (one per ~45s OR on
      previous Servant's death). Tank must:
      - Pull Servant to tank position (taunt or close-range threat strike)
      - Hold Servant at center / near Siamat for group cleave-friendly DPS
      - Maintain threat through Servant's Lightning Nova (channeled PBAoE
        damage; tank is in 10y, so tank takes the hit)
    minion_handling: |
      DO NOT TAUNT Minions of Siamat. Minions self-deplete via Depletion
      and die on their own. Tank ignores Minions; threat stays on
      Siamat / active Servant.
    p2_siamat_burn: |
      After 3rd Servant dies → Wailing Winds → Siamat attackable. Tank
      target was already Siamat; threat continues. Standard tanking.

  defensive_cd_use:
    servant_thunder_crash: |
      Servant casts Thunder Crash on tank (live-cast interpretation). Tank
      pops short-CD defensive on cast warning.
      ALTERNATE interpretation (death-effect): tank steps back when Servant
      drops below ~10% HP. Bot tank does BOTH (defensive on cast + step back
      on low HP).
    wailing_winds_p2_entry: |
      6s channel + violent knockback. Tank must STAY CENTER (knockoff
      hazard). Pop short defensive to mitigate the channel damage.
    gathered_storms_p2_heroic: |
      Heroic only — Gathered Storms blasts ALL players (not just 2). Tank
      takes share of damage; rotational defensive use.
    cloud_burst_landing_under_self: |
      If Cloud Burst targets tank position, MOVE OUT (boss can be moved
      briefly — Siamat doesn't have a fixated tank-only mechanic).

  interrupt_role:
    minion_chain_lightning: |
      OPPORTUNISTIC kick — if Minion is in melee range of tank AND ranged
      DPS hasn't kicked yet:
      - Prot Warrior: Pummel (10s; off-GCD) — actually requires Berserker
        Stance (UNVERIFIED Cata Prot stance access)
      - Prot Pally: Rebuke (15s)
      - Blood DK: Mind Freeze (10s)
      - Feral Tank Druid (Bear): Skull Bash (60s)
      Default: tank stays on Siamat / Servant; ranged handles Minion kicks.

  fight_specific_override:
    stay_center_wailing_winds: |
      EXPLICIT OVERRIDE of any "kite to safe ground" tank rule. Wailing
      Winds at P2 entry knocks ALL players violently — staying at platform
      edge = knockoff = wipe. Tank holds position center even as boss
      mechanics fire.

  heroic_delta: |
    - Lightning Charge stacks on group from Servant deaths (achievement-
      relevant; tank takes some scaling damage)
    - Gathered Storms hits all players in P2
    - Tighter mechanic windows; sustained tank threat in P2 critical
    - Minion Chain Lightning interrupt mandatory (group cleave damage)

  bot_anti_patterns:
    - DO NOT taunt Minions of Siamat (they self-destruct; threat-noop)
    - DO NOT step off platform during Wailing Winds (knockoff = wipe)
    - DO NOT DPS Siamat in P1 with Vengeance-driven offensive output (90%
      DR — wasted threat generation isn't wasted, but offensive output is)
    - DO NOT skip Thunder Crash defensive (large hit on tank)
    - DO NOT let Servants pile on top of Siamat in a bad position (move
      them slightly off-center if needed for group AoE-clean ground)
```

---

## Bot AI hooks

```yaml
ai_hooks:
  husam:
    - encounter_state_active → kite_path_slow_circle
    - encounter_detect_hammer_fist_cast → pop_defensive_cd_short
    - encounter_detect_shockwave_telegraph → move_off_x_pattern_lines
    - encounter_detect_mystic_trap_in_path → adjust_kite_direction
    - encounter_detect_detonate_traps_countdown_heroic → lead_group_to_clean_ground

  lockmaw:
    - encounter_state_active → face_boss_into_wall
    - encounter_detect_scent_of_blood_adds_spawned → aoe_threat_tools_pickup
    - encounter_state_lockmaw_hp_pct < 32 → pop_major_defensive_for_venomous_rage
    - encounter_detect_lockmaw_dead → swap_target_to_augh_acquire_threat
    - encounter_detect_augh_smoke_bomb_telegraph → pre_shield_OR_dps_taunt_relay
    - encounter_detect_augh_whirlwind_active → maintain_threat_through_damage

  barim:
    - encounter_state_phase_1_active → standard_tank_on_barim
    - encounter_detect_fifty_lashings_active → pop_short_defensive
    - encounter_state_barim_hp_pct < 53 → pre_shield_for_repentance
    - encounter_detect_repentance_stun_active → no_action
    - encounter_detect_repentance_stun_ended → move_clear_of_hallowed_ground
    - encounter_state_phase_2_active → swap_target_to_harbinger_of_darkness
    - encounter_detect_harbinger_dead → swap_target_back_to_barim

  siamat:
    - encounter_state_phase_1_active → hold_siamat_center
    - encounter_detect_servant_spawn → taunt_or_threat_strike_pickup
    - encounter_detect_servant_thunder_crash_cast → pop_defensive_cd_short
    - encounter_state_servant_hp_pct < 12 → step_back_brace_for_death_aoe
    - encounter_state_minion_active → ignore_threat_noop
    - encounter_state_third_servant_dead → brace_for_wailing_winds_stay_center
    - encounter_detect_wailing_winds_active → stay_center_pop_defensive
    - encounter_state_phase_2_active → standard_tank_siamat
    - encounter_detect_cloud_burst_under_self → move_boss_short_distance

dispel_blacklist: []                                # no debuffs to BLACKLIST in this dungeon

defensive_cd_priority_per_fight:
  husam:    [SHORT_CD on Hammer Fist combo cast]
  lockmaw:  [MAJOR_CD on Venomous Rage 30%; SHORT_CD on Augh Smoke Bomb (pre-stun)]
  barim:    [SHORT_CD on Fifty Lashings active windows; reactive on Wail of Darkness P2]
  siamat:   [SHORT_CD on Thunder Crash cast; MAJOR_CD on Wailing Winds entry; rotational on Gathered Storms heroic]

target_swap_signals:
  barim:
    - barim.is_immune AND harbinger.is_alive → swap_to_harbinger
    - harbinger.is_dead AND barim.is_attackable → swap_to_barim
  siamat:
    - servant_active.is_alive → maintain_servant_threat
    - third_servant.is_dead → swap_to_siamat
  lockmaw:
    - lockmaw.is_dead AND augh.is_attackable → swap_to_augh

fight_specific_overrides:
  husam:
    - movement_pattern: kite_slow_circle (vs default stationary tank)
  lockmaw:
    - boss_facing: into_wall (vs default away_from_group)
    - rationale: Dust Flail is REAR cone, not front; group side-stands
  siamat:
    - movement_pattern: stay_center_during_wailing_winds (vs default kite
      to safe ground)
    - rationale: knockoff hazard
```

---

## Bot-implementation notes

- **Lockmaw face-into-wall logic**: this is the only fight in the dungeon (and one of few in Cata) where the tank's standard "face boss away from group" rule is flipped. Bot tank positioning logic must accept fight-specific facing overrides — Lockmaw flags `tank_facing: into_wall` (boss's tail = rear cone is buried). On all other fights, default `tank_facing: away_from_group` applies.
- **Husam kite-pattern target waypoint**: rather than tracking a fixed boss-position, bot tank maintains a moving "kite-path waypoint" updated every ~3-5s based on trap-zone positions. Group's "follow tank" logic also keys off this waypoint.
- **Barim P2 target-swap detection**: bot tank's swap signal is `barim.is_immune AND harbinger.is_alive` — NOT HP-threshold (since Barim's 50% trigger is already past once Harbinger is alive; HP-based swap risks double-fire). On Phase 1 resume, signal is `harbinger.is_dead AND barim.is_attackable`.
- **Siamat platform-knockoff during Wailing Winds**: bot tank's positioning logic must include a "stay-within-platform-center-radius" rule active during Wailing Winds (overriding default tank "kite to safe ground" rule). Siamat has fixated mechanics that don't require kiting; tank stays put.
- **Augh Smoke Bomb stun-relay**: 3s tank stun. If the bot's group composition includes a DPS with a taunt ability (Ret Pally Hand of Reckoning, DK Dark Command, Feral Bear Growl in Bear, etc.), DPS taunts during stun window. Bot logic: `tank.stunned AND boss.aggro_target != tank → DPS_with_taunt.taunt(boss)`.
- **Siamat Thunder Crash dual-interpretation**: sources disagree whether Thunder Crash is a live cast (tank takes hit) or death effect (group spreads from low-HP Servant). Bot tank does BOTH — pops defensive on cast warning AND steps back when Servant HP < 12%.
- **Bot-impl hazard cross-reference**: see encounter.md `bot_implementation_hazards` block for the Barim P2 spirit-realm scenario, Augh trigger ambiguity, and Siamat platform knockoff. Tank-specific recovery action for Barim P2 spirit-realm: if private-server scripts teleport players, bot tank's recovery is "re-acquire active hostile mob in current map" rather than "return to Barim coordinates."

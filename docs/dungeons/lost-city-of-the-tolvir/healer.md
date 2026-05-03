# Lost City of the Tol'vir — Healer

**Healer-specific response logic** for the bot's encounter handling. Mechanics, spell IDs, phase triggers, and add abilities are defined in `encounter.md`; this doc only encodes how the healer reacts. Class-agnostic where possible.

**Layering**:
- Cross-content healer patterns (triage hierarchy, dispel taxonomy, mana pacing, anti-patterns) live in `docs/roles/healer.md`
- Per-spec rotation / CD / mana model lives in the active healer's `docs/specs/` doc (resto-shaman, holy-paladin, holy-priest, disc-priest, resto-druid)
- This doc layers Lost City of the Tol'vir-specific deviations on top of those

---

## Dungeon-level overview

```yaml
dungeon_healer_overview:
  total_bosses: 4
  estimated_full_clear_minutes: 30-45
  mana_profile:
    husam:    moderate                    # constant movement; reactive heals on Bad Intentions + Hammer Fist
    lockmaw:  HIGH                        # CANONICAL MANA TEST — heavy poison dispel pressure
    barim:    moderate                    # disease dispel + AoE recovery from Repentance stun
    siamat:   moderate (heroic: high)     # P2 Gathered Storms spike on heroic
  drink_windows:
    - after Husam
    - after Lockmaw                       # critical — Barim and Siamat still ahead
    - after Barim                         # critical — go into Siamat full mana
  key_responsibilities:
    - dispel_rotation_lockmaw             # CRITICAL — primary mana sink
    - dispel_rotation_barim               # CRITICAL — Plague of Ages bounce limits
    - mana_pacing_for_lockmaw_oom_test
    - cooldown_pacing                     # Mana Tide / Spirit Link / Hymn of Hope / Innervate placement
    - phase_2_repentance_stun_recovery    # Barim P2 transition — pre-shield + post-stun AoE heal
```

**Mana-test designation**: Lockmaw is the canonical mana-test of this dungeon (analogous to Karsh in Blackrock Caverns). Save Mana Tide / Innervate / Hymn of Hope / equivalent for Lockmaw. Bot healer must NOT burn major mana CD on Husam.

---

## Trash — healer notes

```yaml
trash_healer:
  tolvir_stoneshapers:
    dispels: []                            # no significant dispel calls on standard trash
    cooldown_use: none required; reactive only
    notes: |
      Stoneshaper Nature-flavor casts are interrupt-targets for DPS; if a
      cast lands due to missed kick, healer reactive on hit player.

  neferset_plaguebringers:
    notes: |
      Disease-flavor casts on trash. UNVERIFIED whether these apply
      Disease-school debuffs (likely yes for Plaguebringer-themed mob).
      If healer has Cleanse Disease: dispel reactively. If not: heal-through.

  oathblade_axemaster_packs:
    notes: melee+cleave packs; standard tank-heal flow

  pre_siamat_trash:
    notes: |
      Wind-flavor adds on slipstream/high-terrace approach. Composition
      varies by source. Standard tank-and-spank healing.
```

---

## Boss 1 — General Husam

```yaml
boss_healer:
  boss: general_husam
  difficulty_grade: medium (movement-heavy)
  mana_intensity: moderate

  threshold_overrides:
    tier_2_tank_emergency: tank.hp_pct < 50    # raised — Hammer Fist combo can spike fast
    tier_3_charge_target_emergency: charge_target.hp_pct < 60   # Bad Intentions spike

  predictable_spike_events:
    - hammer_fist_combo:
        warning: 4 swings over 2s; periodic on tank
        damage: 4x 75%-melee-damage hits in 2s
        response: |
          Pre-HoT tank before combo lands; reactive heal during.
          - Resto Shaman: Riptide + Earth Shield refresh
          - Holy Paladin: Beacon of Light up, Holy Light spam
          - Holy Priest: Renew + PW:S; PoM bounces from tank
          - Disc Priest: PW:S preemptive shield
          - Resto Druid: Lifebloom rolling + Rejuv pre-cast
    - bad_intentions_charge:
        warning: instant — random non-tank charged + slammed
        damage: ~43,875-46,125 + stun on impact
        response: |
          REACTIVE FAST HEAL on charged player post-impact.
          Cannot pre-shield specific target (random).
          Default: have Riptide / PoM / Lifebloom rolling on multiple
          targets so something's already up when charge lands.
    - detonate_traps_heroic:
        warning: 5s countdown
        damage: mass detonation if anyone caught in trap zone
        response: |
          GROUP AOE HEAL pre-staged on countdown end.
          - Resto Shaman: Healing Stream Totem + Healing Rain pre-drop
          - Holy Paladin: Holy Radiance pre-cast
          - Holy Priest: Circle of Healing primed
          - Disc Priest: Power Word: Barrier ground-place if multiple at risk
          - Resto Druid: Wild Growth pre-stack + Tranquility heroic emergency

  cooldown_use:
    standard: rolling reactive HoT/shield maintenance (Riptide / Beacon / Renew / PW:S / Lifebloom)
    no_unique_cd_save: |
      Save Mana Tide / Innervate / Hymn / Spirit Link for LOCKMAW.
      Husam should NOT burn major mana CD.

  dispel_priority: []                       # no dispels required

  interrupt_role:
    none: no kick-required casts on Husam

  movement_demand: |
    HIGH — healer must move with the kite path. Cast-on-the-move spells
    preferred during transitions:
    - Resto Shaman: Riptide instant; Healing Wave during stationary windows
    - Holy Paladin: Beacon-bounce + instant Word of Glory; Holy Shock instant
    - Holy Priest: Renew instant; PoM instant; Heal during stationary
    - Disc Priest: PW:S instant; Penance during stationary
    - Resto Druid: ALL HoTs instant — best mobility-healer for Husam

  heroic_delta: |
    - Detonate Traps premature mass-detonation event (5s countdown)
    - Mystic Trap damage 34,124 → 48,749 (anyone caught)
    - Higher movement demand; cast-on-move preferred

  bot_anti_patterns:
    - DO NOT stand still to hardcast (movement is mandatory; trap zones spawn under stationary players)
    - DO NOT burn Mana Tide / equivalent on Husam (save for Lockmaw)
    - DO NOT panic-flash-heal on Bad Intentions impact (one strong heal, then resume rotation)
```

---

## Boss 2 — Lockmaw (and Augh)

```yaml
boss_healer:
  boss: lockmaw
  difficulty_grade: HARD (canonical mana test)
  mana_intensity: HIGH
  notes: |
    THE canonical Cata leveling OOM-test for new healers in this dungeon
    (analogous to Karsh in Blackrock Caverns). Continuous Poison-school
    dispel pressure: Viscous Poison (12s DoT) on random players + Paralytic
    Blow Dart (9s DoT + 40% attack speed slow) when Augh phase arrives.
    Mana pacing IS the skill check on this fight.

  threshold_overrides:
    tier_2_tank_emergency: tank.hp_pct < 60   # raised — Lockmaw 30% Venomous Rage spike
    tier_3_dot_target_emergency: dot_target.hp_pct < 50   # Viscous Poison-affected

  predictable_spike_events:
    - viscous_poison_target:
        damage: 4,161-9,249 / 2s for 12s + 30% slow
        response: |
          DISPEL FIRST; heal target second.
          - Resto Shaman: Cleanse Spirit (5778 if specced) — UNVERIFIED Cata version; Poison Cleansing Totem (8166) drop alternative
          - Holy Paladin: Cleanse (4987)
          - Holy Priest: Cure Disease (528) DOES NOT cover Poison; Cata change required — check
          - Disc Priest: SAME — Cure Disease only
          - Resto Druid: Remove Corruption (2782) — covers Poison + Curse
          NOTE: Priest healers rely on Shaman/Pally/Druid teammate for Poison;
          if soloing, heal-through-the-DoT.
    - paralytic_blow_dart_target:
        damage: 4,300 / 2s for 9s + 40% attack speed slow
        response: |
          DISPEL FIRST; heal target second.
          CRITICAL if landed on tank — 40% attack-speed slow degrades tank
          threat AND mitigation procs. Top dispel priority during Augh phase.
    - venomous_rage_30_pct:
        damage: +25% damage on Lockmaw — tank takes harder hits
        response: |
          Pop major tank-CD save if available (Pain Suppression / Guardian
          Spirit / equivalent). Mana Tide / Innervate fired here if not
          already.
    - dust_flail_anyone_caught_behind:
        damage: 11,423-13,276 / sec ramping +50% per second
        response: |
          DO NOT heal-through if it's a recoverable mistake — yell at the
          melee bot to step out (anti-pattern). If unrecoverable: fast heal +
          they'll learn next pull.
    - augh_smoke_bomb_tank_stun:
        damage: 3s stun on tank
        response: |
          Pre-HoT tank before stun; bot tank may not be able to use active
          mitigation during stun. Heal-through with stable HoTs.

  cooldown_use:
    mana_tide_totem:
      timing: deploy ~50% Lockmaw HP (roughly mid-fight)
      notes: PRIMARY MANA RECOVERY — bot healer must use this fight
    innervate:
      timing: self-cast ~30% mana
      notes: Resto Druid; Cata pre-fix had cross-target Innervate
    hymn_of_hope:
      timing: ~40% mana
      notes: Holy Priest combat-mana-tide
    spirit_link_totem:
      timing: HP-distribution if group HP varies wildly
      notes: Resto Shaman; useful if multiple players take Viscous Poison + Augh DoTs
    avenging_wrath:
      timing: rotational on Holy Pally
    tranquility:
      timing: emergency raid heal if group HP collapses
      notes: rare on Lockmaw unless multiple Viscous Poisons compound

  dispel_priority:
    - { spell_id: 81630, name: Viscous Poison, type: poison, priority: HIGH, target_strategy: rolling_per_application }
    - { spell_id: 84799, name: Paralytic Blow Dart, type: poison, priority: CRITICAL_IF_ON_TANK }
    notes: |
      Poison-school dispels — Cleanse Spirit (Shaman), Cleanse (Pally),
      Remove Corruption (Druid). Priests do NOT have Poison cleanse natively
      in Cata; if Priest is solo healer, heal-through Poisons.

  interrupt_role:
    none: no kick-required casts

  heroic_delta: |
    - Viscous Poison damage ~2x (4,161/tick → 9,249/tick)
    - Augh full second-phase fight; Paralytic Blow Dart sustained pressure
    - Venomous Rage 30% spike higher
    - More crocolisk adds from Scent of Blood; cleave damage on melee

  bot_anti_patterns:
    - DO NOT skip Mana Tide / equivalent — Lockmaw is the moment
    - DO NOT chase off-target overheals while a Viscous Poison sits undispelled
    - DO NOT stand in Viscous Poison ground patches yourself
    - DO NOT stand behind Lockmaw (Dust Flail rear cone)
    - DO NOT attempt to Cleanse Disease on Viscous Poison (wrong school) — bot dispel logic must dispatch to Poison cleanse spell, not Disease
    - If Priest solo-healer: DO NOT panic about Poison — heal-through is the only option; pace mana
```

---

## Boss 3 — High Prophet Barim

```yaml
boss_healer:
  boss: high_prophet_barim
  difficulty_grade: medium
  mana_intensity: moderate

  threshold_overrides:
    tier_2_tank_emergency: tank.hp_pct < 50   # Fifty Lashings active = +5000 per swing on tank
    tier_4_aoe_cluster: party.injured_count(radius=10, hp_pct_lt=70) >= 3  # Wail of Darkness ticks in P2

  predictable_spike_events:
    - plague_of_ages_application:
        damage: 6,799-11,960 immediate + 3,805-5,751 / 2s for 9s
        response: |
          DISPEL FIRST (Disease-school); heal-through if dispel on CD.
          - Resto Shaman: Cleanse Spirit (5778) — Cata version covers Disease per most sources, UNVERIFIED
          - Holy Paladin: Cleanse (4987) — covers Disease + Poison + Magic
          - Holy Priest: Cure Disease (528) / Abolish Disease — Cata version Cure Disease
          - Disc Priest: SAME — Cure Disease (528)
          - Resto Druid: Cyclone (UNVERIFIED Cata Disease coverage); Druid Disease cleanse may not exist — check
          NOTE: Druid may NOT have Disease cleanse; if Druid solo-healer,
          heal-through Plague.
    - heaven_s_fury_zone_landing:
        damage: 18,037-20,962 / 1.5s for 9s in 12y radius
        response: |
          NO HEAL — anyone caught moves out. If movement stuck: fast heal
          + accept the spike. Pre-HoT before zone resolves if telegraphed.
    - fifty_lashings_active:
        damage: tank takes +5000 bonus per swing for next 10 swings
        response: |
          Top tank to 100% before debuff lands; constant HoT-rolling during.
          Pop tank-CD save (Pain Suppression / Guardian Spirit) if HP drops.
    - repentance_phase_transition:
        warning: 50% HP on Barim (predictable trigger)
        damage: 6s group AoE stun + Hallowed Ground ground zone (9,620-11,180 / 5s)
        response: |
          PRE-SHIELD ALL PLAYERS at 55% Barim HP:
          - Resto Shaman: Earth Shield refresh on tank; Riptide rolling
          - Holy Paladin: Beacon up + Sacred Shield (UNVERIFIED Cata)
          - Holy Priest: PW:S all 5 players; PoM bouncing
          - Disc Priest: PW:S all 5 + Power Word: Barrier ground-place
          - Resto Druid: Lifebloom on tank + Rejuv on all + Wild Growth
          DURING STUN: cannot cast (healer also stunned).
          POST STUN (6s): AoE heal cooldown to recover; group moves out of
          Hallowed Ground.
    - phase_2_wail_of_darkness:
        damage: ~7,000 Shadow / tick to ALL players
        response: |
          Continuous AoE-heal cycling.
          Spirit Link Totem if HP variance high.

  cooldown_use:
    mana_tide_totem: deploy ~50% boss HP if not used on Lockmaw
    pain_suppression / guardian_spirit:
      timing: tank Fifty Lashings active OR Repentance stun
    spirit_link_totem:
      timing: P2 Wail of Darkness cycle if HP variance
    avenging_wrath:
      timing: rotational on Holy Pally; align with P2 if mana stable
    tranquility:
      timing: emergency raid heal during P2 if group HP collapses

  dispel_priority:
    - { spell_id: 82622, name: Plague of Ages, type: disease, priority: HIGH, target_strategy: dispel_first_application_then_jump_carriers }
    notes: |
      Disease-school dispel — Cleanse Spirit (Shaman; UNVERIFIED Cata),
      Cleanse (Pally), Cure Disease (Priest), Druid Disease cleanse
      (UNVERIFIED).

  interrupt_role:
    none: no kick-required casts

  heroic_delta: |
    - Damage scales (Plague of Ages 3,805/tick → 4,947/tick)
    - Blaze of the Heavens phoenix add adds Fire DoT pressure on group
    - Plague jump cycles faster; dispel rotation tighter
    - Soul Fragments faster / more numerous in P2 → if any reach Harbinger,
      Merged Souls makes the fight drag, increasing total healer pressure

  bot_anti_patterns:
    - DO NOT cast spells during Repentance stun (wasted GCD)
    - DO NOT stand in Heaven's Fury zones to heal a player (move them or accept the spike)
    - DO NOT chase Plague-of-Ages through dispels if multiple bounces happening (dispel rotation per application)
    - DO NOT forget Phase 1 resumes — Barim re-engageable after Harbinger dies
    - DO NOT auto-dispel non-Plague debuffs (Heaven's Fury and Wail of Darkness are NOT dispellable)
```

---

## Boss 4 — Siamat

```yaml
boss_healer:
  boss: siamat
  difficulty_grade: medium (P2 heroic spike)
  mana_intensity: moderate (heroic: high)

  threshold_overrides:
    tier_2_tank_emergency: tank.hp_pct < 50   # Servant tanking can spike from Lightning Nova spillover
    tier_4_aoe_cluster: party.injured_count(radius=10, hp_pct_lt=70) >= 3  # P2 Gathered Storms

  predictable_spike_events:
    - wailing_winds_phase_2_entry:
        warning: 3rd Servant dies → Wailing Winds 6s channel
        damage: 3,607-4,192 / sec to ALL players for 6s + violent knockback
        response: |
          PRE-SHIELD ALL PLAYERS at 3rd Servant low HP:
          - Resto Shaman: Earth Shield + Riptide rolling
          - Holy Paladin: Beacon + Holy Radiance pre-stack
          - Holy Priest: PW:S all + PoM bouncing + Hymn of Hope if mana low
          - Disc Priest: PW:S all + Power Word: Barrier ground-place
          - Resto Druid: Wild Growth + Lifebloom + Rejuv all
          DURING WAILING WINDS: continuous AoE heal; players knocked but
          alive.
          POST WAILING WINDS: AoE heal cooldown if group HP < 50%.
    - lightning_nova_servant:
        damage: 13,828-16,071 to anyone within 10y of Servant
        response: |
          Heal-through; if multiple players caught, AoE heal cooldown.
          PRE-CONDITION: group should be >10y from Servant.
    - gathered_storms_p2_heroic:
        damage_normal: 2,405-2,795 / sec for 25s (2 random players)
        damage_heroic: 2,405-2,795 / sec for 25s on ALL players
        response: |
          HEROIC: continuous AoE heal cycling; major mana drain. Pop Mana
          Tide / Hymn / Innervate if not used earlier.
          Cancel by killing Tempest Storms before they reach Siamat.
    - cloud_burst_landing:
        damage: 21,645-25,155 + knock-up if standing in
        response: |
          Player should move out; if caught: fast heal + reactive HoT.
    - storm_bolt_continuous:
        damage_p1: 7,068-7,931 random target
        damage_p2: 14,137-15,862 random target (2x P1)
        response: |
          Continuous reactive heal. Standard ranged damage pattern.
    - thunder_crash_servant:
        damage: 30,062-34,937 (large hit)
        response: |
          Pre-HoT tank; reactive heal post-Thunder-Crash.
          UNVERIFIED whether live cast or death effect (see survey
          Disagreement #4); bot heals tank either way.

  cooldown_use:
    mana_tide_totem:
      timing: P2 entry OR earlier if mana below 40%
      notes: critical heroic for Gathered Storms recovery
    spirit_link_totem:
      timing: P2 Gathered Storms heroic — equalize HP
    pain_suppression / guardian_spirit:
      timing: tank Thunder Crash incoming (live-cast scenario)
    avenging_wrath:
      timing: align with P2 if not used on Lockmaw
    tranquility:
      timing: emergency P2 heroic if Gathered Storms cascades + Wailing Winds catches multiple low

  dispel_priority: []                       # no dispels

  interrupt_role:
    minion_chain_lightning: |
      PARTICIPATE if class has off-GCD ranged kick (Wind Shear, Counterspell,
      Holy Pally Rebuke at melee). Healer's interrupt is a bonus, not
      primary; ranged DPS lead.

  heroic_delta: |
    - Lightning Charge stacks on group from Servant deaths (+33% damage taken
      per stack — significant healer pressure scaling)
    - Gathered Storms hits ALL players (vs 2) — major P2 spike
    - Tighter mechanic windows; healer movement higher
    - Minion Chain Lightning interrupt becomes mandatory (raid-wide arc damage if missed)

  bot_anti_patterns:
    - DO NOT stand in Cloud Burst (you are part of the group — move out)
    - DO NOT stand within 10y of active Servant (Lightning Nova clip)
    - DO NOT step off platform during Wailing Winds (knockoff = wipe)
    - DO NOT attempt to dispel Servants' debuffs (none are dispellable)
    - DO NOT waste DPS on Minions of Siamat (they self-destruct; you're a healer anyway, but support-DPS during downtime should NOT target Minions)
    - DO NOT stand in Tempest Storm path
```

---

## Bot AI hooks

```yaml
ai_hooks:
  husam:
    - encounter_detect_hammer_fist_combo → pre_hot_tank
    - encounter_detect_bad_intentions_charge_landed(target) → reactive_fast_heal target
    - encounter_state_detonate_traps_countdown → pre_stage_aoe_heal_cd
    - encounter_state_active → maintain_movement_with_group

  lockmaw:
    - encounter_detect_viscous_poison_application(target) → dispel_poison target
    - encounter_detect_paralytic_blow_dart_application(target) → dispel_poison target  PRIORITY=CRITICAL_IF_TANK
    - encounter_state_lockmaw_hp_pct < 60 → deploy_mana_tide_or_innervate
    - encounter_state_venomous_rage_active → tank_cd_save
    - encounter_state_augh_phase_active → ready_for_paralytic_blow_dart_dispels

  barim:
    - encounter_detect_plague_of_ages_application(target) → dispel_disease target
    - encounter_state_barim_hp_pct < 55 → pre_shield_all_players  # Repentance imminent
    - encounter_detect_repentance_cast → group_aoe_heal_cd_pre_stage
    - encounter_state_phase_2_active → continuous_aoe_heal_cycle
    - encounter_detect_harbinger_dead → re_target_tank_for_focus

  siamat:
    - encounter_state_third_servant_low_hp → pre_shield_all_players  # Wailing Winds imminent
    - encounter_detect_wailing_winds_cast → continuous_aoe_heal
    - encounter_state_phase_2_active AND heroic → continuous_aoe_heal_cycle  # Gathered Storms
    - encounter_detect_cloud_burst_target(target) → maintain_distance_self  # don't be the target
    - encounter_state_self_mana_below_40 → mana_tide_or_hymn_or_innervate

dispel_blacklist: []                          # no debuffs to BLACKLIST in this dungeon

dispel_whitelist:
  - { spell_id: 81630, name: Viscous Poison, school: poison, fight: lockmaw, priority: HIGH }
  - { spell_id: 84799, name: Paralytic Blow Dart, school: poison, fight: lockmaw_augh, priority: CRITICAL_IF_TANK }
  - { spell_id: 82622, name: Plague of Ages, school: disease, fight: barim, priority: HIGH }

mana_cd_save_priority:
  husam:    [no_save]
  lockmaw:  [PRIMARY: Mana Tide / Innervate / Hymn of Hope at 50% boss HP]
  barim:    [SECONDARY: if not used on Lockmaw]
  siamat:   [HEROIC P2 SECONDARY: Gathered Storms recovery]
```

---

## Bot-implementation notes

- **Barim P2 spirit-realm hazard** (see encounter.md `bot_implementation_hazards`): if private-server scripts teleport players to a separate phased instance during Barim P2, the bot's healing-target tracking may break (party members no longer in the same map). Bot healer's recovery action: `if all party members report null position, fall back to healing self until detection re-syncs`.
- **Augh trigger detection**: bot uses `target.is_attackable(Augh) AND aura.active(target.lockmaw, IS_DEAD)` — does NOT rely on Lockmaw HP-threshold (sources disagree on exact trigger).
- **Priest healers and Poison cleanse**: Priests do NOT have native Poison cleanse in Cata 4.3.4. If Priest is solo healer on Lockmaw, the bot's dispel logic must NOT attempt to cast Cure Disease on Poison-school debuffs — that's an anti-pattern. Heal-through is the only option; bot accepts the DoT damage and paces mana.
- **Druid Disease cleanse** (UNVERIFIED): if Druid does NOT have Disease cleanse on Plague of Ages, same heal-through logic applies on Barim. Verify against DBC at integration time.

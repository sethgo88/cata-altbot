# Zul'Aman — Tank

**Tank-specific response logic** for the bot's encounter handling. Mechanics, spell IDs, phase triggers, and add abilities are defined in `encounter.md`; this doc only encodes how the tank reacts. Class-agnostic where possible.

**Layering**:
- Cross-content tank patterns (threat priority, defensive CD pacing, taunt rules) live in `docs/roles/tank.md` (deferred to tank phase).
- Per-spec rotation / CD / threat model lives in the corresponding `docs/specs/` tank doc (all tank specs deferred to tank phase).
- This doc layers ZA-specific deviations on top of those.

---

## Dungeon-level overview

```yaml
dungeon_tank_overview:
  total_bosses: 6
  estimated_full_clear_minutes: 40-60
  difficulty_modes: [heroic]                # heroic-only
  threat_profile:
    akil_zon: low                            # standard pin + add-pickup
    nalorakk: moderate                       # form swaps + bear-form Mangle stack ramp
    jan_alai: HIGH                           # AoE threat on hatchlings + boss + Hatchers
    halazzi: moderate                        # Saber Lash partner positioning + split adds
    hex_lord: moderate                       # standard pin; class-set may add adds (UNVERIFIED)
    daakara: HIGH                            # form-state machine; per-form positioning + defensive CDs
  defensive_cd_profile:
    akil_zon: light                          # standard tank-melee
    nalorakk: moderate                       # bear-form Mangle stacks; CD on form transition
    jan_alai: moderate                       # AoE pressure during 35% phase + hatchling cleave
    halazzi: light                           # Saber Lash split mitigates tank damage
    hex_lord: moderate                       # Soul Drain damage if not interrupted; Spirit Bolts at 20%
    daakara: HEAVY                           # Lynx-form Claw Rage burst is the highest tank spike of ZA
  key_responsibilities:
    - threat_priority_per_fight
    - face_management                        # Brutal Swipe (Nalorakk), Flame Breath (Jan'alai), Bear Claw (Daakara bear form)
    - add_pickup                             # Akil'zon Soaring Eagles, Jan'alai hatchlings + hatchers, Halazzi Spirit + Lightning Totem
    - form_transition_response               # Nalorakk troll/bear, Daakara troll/bear/eagle/lynx/dragonhawk
    - lynx_form_burst_defensive              # Daakara Lynx Claw Rage major CD
    - hex_lord_class_set_response            # depends on roll
```

---

## Trash — tank notes

```yaml
trash_tank:
  amani_humanoids:
    pull_pattern: |
      Tribesman, Warbringer, Tempest, Beast Tamer, Berserker — CC-able
      humanoids. Default pull pattern: skull on lead caster (kill priority),
      CC on second caster (cross/X markers).
    facing: away from group (frontal cleaves on Amani Berserker, Dragonhawk)
  warbringer_battle_shout:
    notes: HIGH-PRIORITY KICK target — denies self-buff. Tank participates in kick rotation.
  beast_packs:
    notes: |
      Bears, Lynxes, Dragonhawks: standard CC unavailable. Brute-force kill;
      tank pulls + face-away (Dragonhawk cone breath).
  amani_bear_pet_handling:
    notes: |
      Beast Tamer's Bear pet is non-CC-able by most classes. Tank picks
      up; if Hibernate/Trap available from class on group, CC the pet
      and ST burn the Tamer.
  amani_savage:
    notes: |
      Burning Brand stacks bleed on tank. Tank STAYS STILL — moving
      doesn't help; healer HoTs through it. Standard tank-and-spank.
```

---

## Boss 1 — Akil'zon (Eagle Aspect)

```yaml
boss_tank:
  boss: akil_zon
  difficulty_grade: easy
  threat_intensity: low

  positioning:
    pull: pin Akil'zon centrally on platform
    facing: away from group (no major cone; standard)
    movement: minimal
    eagle_storm_telegraph: |
      Tank moves to STACK with group on Eagle Storm telegraph (~3-5s
      window). Position central so designated lift player is reachable
      by all.

  threat_management:
    standard_pin: maintain threat on Akil'zon
    add_pickup: |
      Soaring Eagles spawn periodically. Tank picks up via AoE threat:
      - Prot Warrior: Thunder Clap + Cleave + Devastate threat
      - Prot Pally: Avenger's Shield + Hammer of the Righteous + Consecration
      - Blood DK: Death and Decay + Howling Blast + Pestilence-spread
      - Feral Tank Druid: Swipe (bear) + Thrash + Maul (off-hand cleave)
    static_disruption_targeted: |
      If you become Static Disruption target, MOVE OUT of group while
      keeping Akil'zon on you. Brief side-step is sufficient.

  defensive_cd_use:
    standard: reactive — Akil'zon's auto-attack is moderate
    eagle_storm: pop short defensive CD if soloing the lift (Shield Block / Barkskin / Bone Shield)
    add_cleave: short CD if multiple Soaring Eagles + Akil'zon melee compound

  interrupt_role:
    call_lightning: |
      PARTICIPATE in interrupt rotation if class has off-GCD kick:
      - Prot Warrior: no ranged kick (Pummel is melee + Heroic Throw is silence-talented)
      - Prot Pally: Rebuke (15s; off-GCD; melee range — useful only if Akil'zon is in melee)
      - Blood DK: Mind Freeze (10s; off-GCD; melee)
      - Feral Tank Druid: Skull Bash (60s; gap-close + interrupt; bear form)

  heroic_delta: |
    Heroic-only. Static Disruption tick higher; Eagle Storm lethal if
    soloed; Call Lightning unintercepted is healer pressure.

  bot_anti_patterns:
    - DO NOT chase Soaring Eagles (they come to group; AoE-pickup as they arrive)
    - DO NOT skip Eagle Storm stack participation (you are part of the lift group)
    - DO NOT face Akil'zon into the group during stack (no cone, but melee positioning)
```

---

## Boss 2 — Nalorakk (Bear Aspect)

```yaml
boss_tank:
  boss: nalorakk
  difficulty_grade: medium
  threat_intensity: moderate

  positioning:
    pull: pin Nalorakk in center
    facing: |
      AWAY FROM GROUP — Brutal Swipe cone in bear form. Maintain face-
      away through form transitions (cone applies in bear form only,
      but consistent face-away simplifies bot logic).
    movement: minimal; ride through form swaps in place

  threat_management:
    troll_form: standard threat-rotation; Mojo Volley doesn't break threat
    bear_form: |
      Mangle bleed stacks on tank. Stacks RESET on troll-swap. Single
      tank rides through stacks (no tank-swap mechanic in 5-man heroic).
    surge_charge: |
      Random target — tank does not need to react. Healer heals charged
      player. Tank maintains threat lead.

  defensive_cd_use:
    bear_form_transition: |
      Pop short defensive CD on bear-form transition (predictable from
      form-change visual telegraph):
      - Prot Warrior: Shield Block (rotational) + Last Stand (3min) on high stacks
      - Prot Pally: Holy Shield / Word of Glory (heal-self) + Divine Protection
      - Blood DK: Vampiric Blood (-15% damage 10s) + Bone Shield rotational
      - Feral Tank Druid: Survival Instincts (-50% damage 12s)
    high_mangle_stack: |
      At ~5-6 stacks (UNVERIFIED), pop major defensive CD:
      - Prot Warrior: Shield Wall (5min CD)
      - Prot Pally: Ardent Defender (3min CD; -20% damage)
      - Blood DK: Icebound Fortitude (3min CD; -20% damage)
      - Feral Tank Druid: Barkskin (1min) + Survival Instincts overlap

  interrupt_role:
    mojo_volley: |
      PRIMARY interrupt rotation participant in troll form:
      - Prot Pally: Rebuke (15s; off-GCD)
      - Blood DK: Mind Freeze (10s; off-GCD)
      - Feral Tank Druid: Skull Bash (60s — save for emergencies)
      - Prot Warrior: Pummel (10s; melee; off-GCD) IF in melee range
        with Nalorakk (you are — tank-melee)

  heroic_delta: |
    Heroic-only. Mojo Volley unintercepted = group damage spike; Mangle
    stacks higher in bear form.

  bot_anti_patterns:
    - DO NOT face Nalorakk into group during bear form (Brutal Swipe cone)
    - DO NOT skip kicks on Mojo Volley (top kick priority on this fight)
    - DO NOT tank-swap (no swap mechanic in 5-man)
    - DO NOT pop major defensive CD in troll form (waste — stacks reset)
```

---

## Boss 3 — Jan'alai (Dragonhawk Aspect)

```yaml
boss_tank:
  boss: jan_alai
  difficulty_grade: hard (AoE add management)
  threat_intensity: HIGH

  positioning:
    pull: pin Jan'alai in CENTER of chamber
    facing: |
      AWAY FROM GROUP — Flame Breath cone is wide. Critical face-management
      for the entire fight.
    movement: |
      Move to AoE-position when hatchlings spawn (typically pull boss +
      hatchlings to a clear spot away from Fire Bomb patches).

  threat_management:
    standard_st: maintain threat on Jan'alai through fight
    hatcher_spawn: |
      Hatchers spawn at sides; tank does NOT need to pick up immediately.
      DPS kills Hatchers on sight. Tank stays on Jan'alai unless Hatcher
      is missed and reaches an egg pile.
    hatchling_pickup: |
      AT 35% Hatch All Eggs trigger (or earlier if Hatchers reach eggs):
      AoE THREAT TOOLS CRITICAL. Hatchlings will go for closest non-tank
      otherwise:
      - Prot Warrior: Thunder Clap + Cleave + Shockwave (talent)
      - Prot Pally: Avenger's Shield + Consecration + Hammer of the
        Righteous + Holy Wrath
      - Blood DK: Death and Decay (PRIMARY add-AoE; ground-place) +
        Howling Blast + Blood Boil + Pestilence-spread
      - Feral Tank Druid: Swipe (bear) + Thrash (bear AoE bleed) +
        Berserk-Mangle for threat
    boss_threat_drift: |
      During 35% AoE phase, ranged DPS may overaggro Jan'alai while
      AoE-burning hatchlings. Tank Taunt-cycle: use Taunt to recover
      threat if boss drifts.

  defensive_cd_use:
    pre_35_phase: standard tank-melee mitigation
    35_phase_aoe: |
      Pop defensive CDs to mitigate hatchling cleave + Jan'alai melee:
      - Prot Warrior: Shield Block rotational + Last Stand
      - Prot Pally: Divine Protection + Word of Glory self-heal
      - Blood DK: Vampiric Blood + Bone Shield + Rune Tap
      - Feral Tank Druid: Survival Instincts + Barkskin
    fire_bomb_on_self: |
      If a Fire Bomb lands on tank position, MOVE the boss + tank to a
      clear area. Pop short defensive if fire patch DoT ramps.

  interrupt_role:
    hatcher_channel: |
      Tank can taunt the Hatcher and break the channel, but the
      preferred response is DPS kill. Tank-taunt is fallback only.

  heroic_delta: |
    Heroic-only. Hatchers tougher; Fire Bomb damage higher; 35% Hatch
    All Eggs more hatchlings.

  bot_anti_patterns:
    - DO NOT face Jan'alai into group (Flame Breath cone)
    - DO NOT skip AoE threat tools during hatchling phase (hatchlings reach healer otherwise)
    - DO NOT chase Hatchers (DPS kills them; tank stays on Jan'alai)
    - DO NOT stand in Fire Bomb patches (tank suffers same DoT as DPS)
```

---

## Boss 4 — Halazzi (Lion Aspect)

```yaml
boss_tank:
  boss: halazzi
  difficulty_grade: medium
  threat_intensity: moderate

  positioning:
    pull: pin Halazzi in center
    facing: away from group (standard)
    movement: minimal; reposition slightly for splits

  threat_management:
    standard_st: maintain threat on Halazzi
    saber_lash_partner: |
      Halazzi's Saber Lash splits damage between tank and ONE nearest
      melee. Tank does NOT need to actively manage this — positioning
      is melee DPS responsibility. Tank maintains threat as normal.
    split_at_66: |
      Spirit of the Lynx spawns. Tank can pick up via taunt OR let DPS
      handle (Spirit goes to closest target). DEFAULT: tank picks up
      Spirit if it threatens healer; otherwise let DPS cleave.
    split_at_33: same as 66%
    lightning_totem: |
      Tank does NOT need to taunt Lightning Totem (it's stationary AoE
      Nature damage, not threat-targeted). DPS kills it on sight.

  defensive_cd_use:
    standard: reactive
    saber_lash_split: pop short CD if Saber Lash damage spikes (rare — split mitigates)
    no_unique_cd_save: standard rotational

  interrupt_role:
    none: no kick-required hardcasts

  dispel_role:
    frenzy_dispel: |
      Tank classes generally don't have ENRAGE-targeted dispels. Tranquilizing
      Shot (Hunter) and Soothe (Druid) are non-tank-spec abilities. Tank
      defers to ranged DPS / healer.
    flame_shock_dispel: |
      Prot Pally has Cleanse (Magic + Disease + Poison); Prot Warrior /
      Blood DK / Feral Tank Druid don't. Default: healer handles.

  heroic_delta: |
    Heroic-only. Lightning Totem hits faster (kill priority); Spirit HP
    higher; Frenzy dispel timing more critical (but tank rarely has the dispel).

  bot_anti_patterns:
    - DO NOT taunt Lightning Totem (stationary; not threat-targeted)
    - DO NOT pull multiple melee into Saber Lash share zone (only ONE)
    - DO NOT waste defensive CDs (Saber Lash split mitigates damage already)
```

---

## Boss 5 — Hex Lord Malacrass

```yaml
boss_tank:
  boss: hex_lord_malacrass
  difficulty_grade: hard (non-deterministic)
  threat_intensity: moderate

  positioning:
    pull: pin Hex Lord central
    facing: away from group (standard)
    movement: |
      Move out of class-set ground effects (Flamestrike / Starfall /
      Shadowfury) — tank suffers same damage as DPS. Otherwise minimal.

  threat_management:
    standard_st: maintain threat on Hex Lord
    class_set_adds: |
      Some class-set rolls summon adds (Mage Mirror Image, Unholy DK
      Gargoyle, Hunter pet, Shadow Priest Shadowfiend? UNVERIFIED).
      Tank picks up via taunt + AoE threat. Default response: AoE pickup
      rotation as Jan'alai.

  defensive_cd_use:
    soul_drain_uninterrupted: |
      Soul Drain on tank target = tank can be drained too. If interrupt
      fails AND Soul Drain hits tank, pop short defensive (Shield Block /
      Bone Shield / Survival Instincts).
    spirit_bolts_at_20: |
      Pop major defensive CD at 22% boss HP for Spirit Bolts AoE:
      - Prot Warrior: Shield Wall (5min CD) — mitigates AoE
      - Prot Pally: Ardent Defender (3min) + Aura Mastery (devotion -20% AoE — actually a Holy Pally CD; UNVERIFIED for Prot)
      - Blood DK: Icebound Fortitude (3min)
      - Feral Tank Druid: Survival Instincts + Barkskin overlap

  interrupt_role:
    soul_drain: |
      PARTICIPATE — top kick priority on this fight:
      - Prot Pally: Rebuke (15s; off-GCD)
      - Blood DK: Mind Freeze (10s; off-GCD)
      - Feral Tank Druid: Skull Bash (60s — save for healer-class roll)
      - Prot Warrior: Pummel (10s; melee; off-GCD)
    class_set_hardcasts: |
      Reactive — tank participates in kick rotation for healer-class
      rolls (Resto Druid Tranquility, Holy Priest Heal, Holy Pally
      Holy Light, Resto Shaman Healing Wave). Missed kick = boss
      heals himself.

  heroic_delta: |
    Heroic-only. Soul Drain damage higher; Spirit Bolts at sub-20% lethal.

  bot_anti_patterns:
    - DO NOT pre-commit to a fixed kick rotation (depends on class roll)
    - DO NOT skip Soul Drain kicks
    - DO NOT skip healer-class hardcast kicks (boss heals himself)
    - DO NOT stand in class-set ground effects

  bot_implementation_required: |
    HEX LORD ABILITY-SET DETECTION HOOK — tank's interrupt + add-pickup
    priority queries encounter_state.hex_lord_class_roll. Default
    fallback: interrupt all Hex Lord hardcasts; pick up any spawned adds.
```

---

## Boss 6 — Daakara

```yaml
boss_tank:
  boss: daakara
  difficulty_grade: hard (form-state machine; Lynx burst)
  threat_intensity: HIGH

  positioning:
    pull: pin Daakara central
    facing: |
      AWAY FROM GROUP — critical for bear form (Bear Claw cone).
      Maintain face-away through all form transitions.
    movement: per-form rules (see below)
    per_form:
      troll: standard pin
      bear: face-away CRITICAL
      eagle: minimal movement; ranged-form so no melee rules
      lynx: stand-still; absorb Claw Rage burst
      dragonhawk: minimal movement; spread for fire patches

  threat_management:
    standard_st: maintain threat across all forms
    form_transition: |
      Threat persists through form changes. Tank does NOT need to taunt
      on form swap.

  defensive_cd_use:
    troll_form: standard rotational
    bear_form: |
      Pop short CD if Bear Claw cone clips group (positioning failure):
      - Prot Warrior: Shield Block
      - Prot Pally: Holy Shield rotational
      - Blood DK: Bone Shield rotational
      - Feral Tank Druid: Frenzied Regeneration (talent — UNVERIFIED Cata)
    eagle_form: |
      Tank is mostly idle on threat (no melee from boss in eagle form?
      UNVERIFIED). Move out of Cyclone / Static Charge ground patches.
    lynx_form: |
      MAJOR DEFENSIVE CD on Lynx-form transition (Disagreement #9):
      - Prot Warrior: Shield Wall (5min CD) — PRIMARY for Lynx form
      - Prot Pally: Ardent Defender (3min) — PRIMARY for Lynx form
      - Blood DK: Vampiric Blood (1min CD) + Icebound Fortitude (3min CD;
        rotate per Lynx-form occurrence)
      - Feral Tank Druid: Survival Instincts (3min CD; -50% damage 12s)
        — PRIMARY for Lynx form
      Lynx Claw Rage is the highest tank damage spike of the fight.
    dragonhawk_form: |
      Reactive defensive on fire patch DoT if positioning forces tank
      into a patch.

  interrupt_role:
    grievous_throw: |
      TROLL FORM ONLY — PARTICIPATE in interrupt rotation:
      - Prot Pally: Rebuke (15s; off-GCD)
      - Blood DK: Mind Freeze (10s; off-GCD)
      - Feral Tank Druid: Skull Bash (60s)
      - Prot Warrior: Pummel (10s; off-GCD; melee)

  heroic_delta: |
    Heroic-only. Form damage scales up; Lynx Claw Rage burst higher;
    dragonhawk fire patches harder to dodge.

  bot_anti_patterns:
    - DO NOT face boss into group during bear form (Bear Claw cone)
    - DO NOT stand in eagle Cyclone / Static Charge patches
    - DO NOT stand in dragonhawk Flame Whirl / Pillar of Fire patches
    - DO NOT skip Lynx-form major defensive CD (highest tank spike)
    - DO NOT continue swinging during form-transition (boss may move briefly)

  bot_implementation_required: |
    DAAKARA FORM-DETECTION HOOK — tank's positioning + defensive CD
    priorities query encounter_state.daakara_active_form. Default
    fallback on form-change: face-away + reactive move-out + short
    defensive CD until form aura confirms specific form.

    Lynx-form major CD is HARD-CODED — bot tank pops Shield Wall /
    Ardent Defender / Survival Instincts on Lynx-form aura detection.
    Long-CD overflow: if major CD is on cooldown, layer short CDs
    (Shield Block + Last Stand / Bone Shield / Barkskin) instead.
```

---

## Bot AI hooks

```yaml
ai_hooks:
  akil_zon:
    - encounter_detect_static_disruption_targeted_self → side_step_with_boss
    - encounter_detect_eagle_storm_telegraph → stack_with_group_centrally
    - encounter_detect_call_lightning → kick_rotation_participate (if class has off-GCD kick)
    - encounter_detect_soaring_eagles_spawn → AoE_threat_pickup

  nalorakk:
    - encounter_detect_form_change_to_bear → pop_short_defensive_cd
    - encounter_detect_form_change_to_troll → no_action (stacks reset)
    - encounter_state_bear_form AND mangle_stack_high → pop_major_defensive_cd
    - encounter_detect_mojo_volley_cast → kick_rotation_participate
    - encounter_state_form_bear → face_away_from_group_critical

  jan_alai:
    - encounter_detect_hatcher_spawn → no_action (DPS kills it)
    - encounter_detect_hatch_all_eggs → AoE_threat_tools_active
    - encounter_detect_boss_threat_dropping → taunt_recover
    - encounter_detect_self_in_fire_bomb → move_boss_to_clear_area
    - encounter_state_active → face_away_from_group

  halazzi:
    - encounter_detect_split_at_66 → maintain_boss_threat (DPS handles spirit)
    - encounter_detect_split_at_33 → maintain_boss_threat
    - encounter_detect_spirit_threatens_healer → taunt_spirit
    - encounter_detect_lightning_totem → no_action (DPS kills)

  hex_lord:
    - encounter_state_class_roll_detected → load_kick_priority_profile + load_add_pickup_profile
    - encounter_detect_soul_drain_cast → fire_kick (top priority)
    - encounter_detect_class_set_hardcast → kick_per_profile
    - encounter_detect_class_set_summon → AoE_threat_pickup
    - encounter_detect_boss_hp_lt_22 → pop_major_defensive_cd_for_spirit_bolts
    - encounter_detect_class_set_ground_effect → move_boss_out

  daakara:
    - encounter_detect_form_change → load_form_profile
    - encounter_state_active_form == troll → kick_rotation_for_grievous_throw
    - encounter_state_active_form == bear → face_away_critical + reactive_short_cd
    - encounter_state_active_form == eagle → standard_pin + move_out_check
    - encounter_state_active_form == lynx → POP_MAJOR_DEFENSIVE_CD (Shield Wall / Ardent Defender / Survival Instincts)
    - encounter_state_active_form == dragonhawk → standard_pin + move_out_check

dispel_blacklist: []                         # NO instance-wide blacklist; tank dispel limited to Prot Pally Cleanse

defensive_cd_priority_per_fight:
  akil_zon:    [reactive_only]
  nalorakk:    [bear_form_short_CD, major_CD_at_high_mangle_stack]
  jan_alai:    [reactive_short_CD, major_CD_during_35_phase]
  halazzi:     [reactive_only]
  hex_lord:    [major_CD_at_22_for_spirit_bolts]
  daakara:     [MAJOR_CD_on_lynx_form_transition (HARD-CODED), reactive_short_CD_per_form]
```

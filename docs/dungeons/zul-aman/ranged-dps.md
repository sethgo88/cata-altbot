# Zul'Aman — Ranged DPS

**Ranged-DPS-specific response logic** for the bot's encounter handling. Mechanics, spell IDs, phase triggers, and add abilities are defined in `encounter.md`; this doc only encodes how ranged DPS reacts. Class-agnostic where possible.

**Layering**:
- Cross-content ranged DPS patterns (positioning, spread/stack rules, threat ceiling) live in `docs/roles/ranged-dps.md`
- Per-spec rotation lives in the active spec's `docs/specs/` doc (frost-mage, affliction-warlock, mm-hunter, shadow-priest, balance-druid, elemental-shaman)
- This doc layers ZA-specific deviations on top of those

---

## Dungeon-level overview

```yaml
dungeon_ranged_dps_overview:
  total_bosses: 6
  estimated_full_clear_minutes: 40-60
  difficulty_modes: [heroic]                # heroic-only
  positioning_profile:
    akil_zon: TOGGLE_SPREAD_OR_STACK         # spread default; STACK on Eagle Storm telegraph
    nalorakk: max_range_spread               # spread for Surge charge
    jan_alai: max_range_spread               # spread for Fire Bomb; AoE rotation in 35% phase
    halazzi: max_range_spread                # ready to swap target on splits
    hex_lord: max_range_spread               # spread for class-set ground effects
    daakara: per_form                        # form-dependent positioning
  interrupt_load:
    akil_zon: high                           # Call Lightning on cooldown
    nalorakk: HIGH                           # Mojo Volley every cast in troll form
    jan_alai: low                            # only Hatcher channel (kill is primary)
    halazzi: zero                            # no kicks
    hex_lord: HIGHEST                        # Soul Drain + class-set hardcasts
    daakara: medium                          # Grievous Throw on troll form
  key_responsibilities:
    - akil_zon_stack_override                # STACK on Eagle Storm — opposite of default spread
    - kick_rotation_high_density             # Nalorakk + Hex Lord
    - target_swap_on_halazzi_splits
    - aoe_burst_on_jan_alai_35_phase
    - hex_lord_class_set_detection
    - daakara_form_transition_response
```

---

## Trash — ranged DPS notes

```yaml
trash_ranged:
  amani_humanoids:
    cc_priorities: |
      Ranged contributes opener-CC:
      - Frost Mage: Polymorph (Sheep) — primary multi-CC
      - Affliction Warlock: Banish-no (Demons only); Fear can scatter — generally avoid in 5-man
      - MM Hunter: Freezing Trap (icetrap) on caster
      - Shadow Priest: Shackle Undead (only undead — limited use); Mind Control (rare 5-man use)
      - Balance Druid: Cyclone (8s) or Hibernate (beasts only)
      - Elemental Shaman: Hex (frog 60s, breaks on damage)
  caster_packs:
    interrupt_role: |
      - Frost Mage: Counterspell (24s) / Frost Nova root for kiting
      - Affliction Warlock: Spell Lock via Felhunter pet
      - MM Hunter: Silencing Shot (24s)
      - Shadow Priest: Silence (45s)
      - Balance Druid: Solar Beam (60s; 8s AoE silence)
      - Elemental Shaman: Wind Shear (6s; off-GCD; 25y range)
  warbringer_battle_shout:
    notes: PRIORITY KICK — denies self-buff; rotate kicks across DPS
  beast_packs:
    notes: |
      Bears, Lynxes, Dragonhawks: standard CC doesn't apply. Hibernate
      (Druid only) or Freezing Trap (Hunter only) are the only options.
      Brute-force kill the rest.
```

---

## Boss 1 — Akil'zon (Eagle Aspect)

```yaml
boss_ranged:
  boss: akil_zon
  difficulty_grade: medium (positional toggle)
  positioning:
    DEFAULT: max range, SPREAD (3y+ between players) for Static Disruption
    EAGLE_STORM_TELEGRAPH: |
      STACK tight (within ~3y) under designated lift player. ~3-5s
      telegraph window. Resume spread when channel ends.
    facing: boss
    movement: |
      Move out if YOU are the Static Disruption target. Otherwise minimal.
  interrupt_role:
    call_lightning: PARTICIPATE — Counterspell / Wind Shear / Silencing Shot / Solar Beam / Spell Lock / Silence rotation
  rotation_modifications:
    static_disruption_targeted: |
      If you are the target, MOVE OUT of group while continuing DPS.
      Cast-while-moving instants only:
      - Frost Mage: Ice Lance / Mage Bomb / Frost Bomb
      - Affliction Warlock: DoTs maintained instant
      - MM Hunter: Steady Shot interrupted on movement; Aimed Shot proc only
      - Shadow Priest: SW:P / VT instant; Mind Flay channels mobility-friendly
      - Balance Druid: Moonfire / Sunfire / Starsurge instants
      - Elemental Shaman: Flame Shock instant; Lightning Bolt interrupted
    eagle_storm_stack: |
      Continue DPS while in storm cloud (you are alive in there). Cast-
      while-moving only if storm-cloud allows (UNVERIFIED whether storm
      lifts breaks casting):
      - All ranged: instant abilities only during storm channel
    add_phase: |
      AoE rotation on Soaring Eagles spawn:
      - Frost Mage: Blizzard / Frostfire Bolt cleave; Frost Nova for root
      - Affliction Warlock: Seed of Corruption + Hellfire if no DoT spread
      - MM Hunter: Multi-Shot / Trap Launcher Explosive Trap
      - Shadow Priest: Mind Sear if 4+ targets; SW:P/VT spread
      - Balance Druid: Hurricane / Wild Mushroom detonation
      - Elemental Shaman: Magma Totem + Chain Lightning + Earthquake
        — POSITIONAL HAZARD: do NOT use Thunderstorm during stack
  cd_use:
    standard_offensive: rotational on engagement
  heroic_delta: |
    Heroic-only. Static Disruption tick higher; Eagle Storm lethal if
    soloed.
  bot_anti_patterns:
    - DO NOT stay max range during Eagle Storm telegraph (must stack)
    - DO NOT stay stacked outside of Eagle Storm (Static Disruption splash)
    - DO NOT cast Thunderstorm / Typhoon during stack (knockback during storm = wipe risk)
    - DO NOT use Iceblock unless dropping group damage from Eagle Storm (you blockee out of the lift)
```

---

## Boss 2 — Nalorakk (Bear Aspect)

```yaml
boss_ranged:
  boss: nalorakk
  difficulty_grade: medium (high kick density on troll form)
  positioning:
    default: max range, SPREAD (3y+) for Surge charge
    facing: boss
    movement: minimal — Surge charge is the only forced movement
  interrupt_role:
    mojo_volley: |
      MUST INTERRUPT every troll-form cast. Rotation:
      - Counterspell (Mage; 24s; 40y; locks Nature school 8s)
      - Spell Lock (Felhunter; 24s; 30y; off-GCD)
      - Silencing Shot (MM Hunter; 24s; 35y; instant)
      - Silence (Shadow Priest; 45s; 30y; locks for 5s)
      - Solar Beam (Balance Druid; 60s; ground 30y; AoE silence)
      - Wind Shear (Ele Shaman; 6s; 25y; off-GCD; SHORTEST CD — primary)
  rotation_modifications:
    troll_form: |
      Standard ST rotation; rotate kicks on Mojo Volley.
    bear_form: |
      No kicks needed (no casts in bear form). Continue ST DPS.
    surge_charge: |
      If targeted, brace for knockback. Continue DPS — most ranged
      classes recover quickly:
      - Frost Mage: Iceblock if HP-critical (immune)
      - Aff Warlock: Demonic Circle (talent) for re-position
      - MM Hunter: Disengage post-charge for distance
      - Shadow Priest: Dispersion (-90% damage 6s) if HP-critical
      - Balance Druid: Barkskin (-20% damage 12s)
      - Ele Shaman: Stoneclaw Totem (talented self-shield)
  cd_use:
    standard_offensive: rotational; align with troll-form windows (boss is more vulnerable in troll form)
  heroic_delta: |
    Heroic-only. Mojo Volley unintercepted = group damage spike;
    Mangle stacks higher.
  bot_anti_patterns:
    - DO NOT skip Mojo Volley kicks (top kick priority on this fight)
    - DO NOT cluster ranged (Surge multi-hit)
    - DO NOT cast hardcast spells if Surge is imminent (predictable from cooldown timer)
```

---

## Boss 3 — Jan'alai (Dragonhawk Aspect)

```yaml
boss_ranged:
  boss: jan_alai
  difficulty_grade: hard (AoE burst phase)
  positioning:
    default: max range, SPREAD (3y+) for Fire Bomb avoidance
    facing: boss
    movement: |
      Move out of Fire Bomb ground patches; otherwise minimal.
      During 35% Hatch All Eggs phase, AoE-position around hatchling
      cluster (typically center where tank pulls them).
  interrupt_role:
    hatcher_channel: PARTICIPATE if Hatcher reaches eggs before kill (fallback only)
  rotation_modifications:
    pre_35_phase: |
      Standard ST rotation on Jan'alai; SWAP to Hatcher when it spawns.
      Each class's Hatcher kill-priority:
      - Frost Mage: Polymorph the Hatcher (humanoid, Sheep works) as
        fallback if multiple spawn; otherwise burn with ST rotation
      - Aff Warlock: DoT-spread to Hatcher; Drain Soul if execute
      - MM Hunter: target Hatcher with single-target; Aimed Shot opener
      - Shadow Priest: VT + SW:P + Mind Blast; Silence the channel as backup
      - Balance Druid: Cyclone the Hatcher if multiple (8s; humanoid); ST otherwise
      - Ele Shaman: Hex the Hatcher (frog 60s) as multi-Hatcher CC;
        otherwise ST burn
    35_phase_aoe: |
      SWAP TO AOE ROTATION. Hatchlings spawn en masse:
      - Frost Mage: Blizzard / Frostfire Bolt / Frost Nova (root the wave)
      - Aff Warlock: Seed of Corruption (PRIMARY AoE) + Hellfire if cluster
      - MM Hunter: Multi-Shot / Trap Launcher Explosive Trap / Volley (UNVERIFIED Cata)
      - Shadow Priest: Mind Sear (CHANNEL — AoE; primary on 4+ targets)
      - Balance Druid: Hurricane (CHANNEL) / Wild Mushroom / Starfall
      - Ele Shaman: Magma Totem + Chain Lightning + Earthquake +
        Thunderstorm if knockback acceptable (NOT IN GROUP STACK)
    after_hatchlings: resume ST on Jan'alai
  cd_use:
    save_offensive_for_35: |
      Pop AoE-burst CDs at 35% trigger:
      - Frost Mage: Mirror Image (UNVERIFIED Cata talent)
      - Aff Warlock: Soulburn-Seed of Corruption (talented multi-target opener)
      - MM Hunter: Rapid Fire (3min CD)
      - Shadow Priest: Shadowfiend (5min CD)
      - Balance Druid: Force of Nature (3min CD; treants AoE)
      - Ele Shaman: Bloodlust / Heroism if not used on pull
  heroic_delta: |
    Heroic-only. Hatchers tougher; Fire Bomb damage higher; 35% spawns
    more hatchlings.
  bot_anti_patterns:
    - DO NOT stand in Fire Bomb patches
    - DO NOT cleave Jan'alai during 35% hatchling phase (focus AoE on hatchlings)
    - DO NOT skip Hatcher kills (channel = double the hatchlings)
    - DO NOT use Earthquake / Thunderstorm in group stack (knockback)
```

---

## Boss 4 — Halazzi (Lion Aspect)

```yaml
boss_ranged:
  boss: halazzi
  difficulty_grade: medium (target-priority swaps)
  positioning:
    default: max range from boss; ready to swap to Spirit of the Lynx on splits
    facing: boss / spirit / totem
    movement: minimal
  interrupt_role:
    none: no kick-required hardcasts
  rotation_modifications:
    phase_1_pre_66: |
      Standard ST rotation on Halazzi.
    split_at_66: |
      SWAP to Spirit of the Lynx (or cleave both per Disagreement #8):
      - AoE-class (Mage / Druid / Ele Shaman / Aff Warlock with Bane of Havoc): cleave both
      - Pure ST (MM Hunter / Shadow Priest with single-target focus): kill Spirit first, return to Halazzi
      AOE-CLASS RULE: if class.has_aoe_or_cleave_tools(): cleave both; else: kill Spirit first.
    lightning_totem_kill: |
      KILL ON SIGHT — Lightning Totem AoE Nature damages group.
      Off-GCD targeted casts (any single instant) work; Hunters
      auto-cleave via pet possible.
    phase_2_pre_33: standard ST on Halazzi
    split_at_33: same as 66% — repeat AoE/ST swap
    execute_phase: full burn on Halazzi
  dispel_role:
    frenzy_dispel: |
      If class has Magic dispel:
      - MM Hunter: Tranquilizing Shot (off-GCD; instant; ENRAGE-targeted)
      - Frost Mage: Spell Steal (also gains haste buff for Mage)
      - Resto/Balance Druid: Soothe (off-GCD; instant)
      Frenzy dispel is HIGHER PRIORITY than rotational DPS for the class
      that has the dispel.
    flame_shock_dispel: |
      Mage Spell Steal can target the buffed Halazzi (if Halazzi self-
      buffed via Flame Shock) — UNVERIFIED whether mechanic supports this.
      Otherwise let healer handle Flame Shock dispels.
  cd_use:
    align_with_splits: pop offensive CDs on split transitions for Spirit burn
  heroic_delta: |
    Heroic-only. Lightning Totem hits faster; Spirit HP higher; Frenzy
    dispel timing more critical.
  bot_anti_patterns:
    - DO NOT ignore Spirit of the Lynx spawns (target-priority swap)
    - DO NOT skip Lightning Totem (low HP; AoE Nature damage on group)
    - DO NOT skip Frenzy dispel if class has it (priority over rotation)
```

---

## Boss 5 — Hex Lord Malacrass

```yaml
boss_ranged:
  boss: hex_lord_malacrass
  difficulty_grade: hard (non-deterministic; high kick density)
  positioning:
    default: max range, SPREAD (5y+) for class-set ground effects
    facing: boss
    movement: |
      Move out of class-set ground effects (Flamestrike / Starfall /
      Shadowfury). Otherwise minimal.
  interrupt_role:
    soul_drain: |
      MUST INTERRUPT — top kick priority on this fight. Rotation:
      - Counterspell (Mage)
      - Spell Lock (Felhunter)
      - Silencing Shot (MM Hunter)
      - Silence (Shadow Priest)
      - Solar Beam (Balance Druid)
      - Wind Shear (Ele Shaman; SHORTEST CD)
    class_set_hardcasts: |
      Reactive interrupts based on detected class set:
      - Healer-class rolls (Resto Druid Tranquility, Holy Priest Heal,
        Holy Pally Holy Light, Resto Shaman Healing Wave) — HIGHEST
        priority kicks; missed kick = boss heals himself
      - DPS-class hardcasts (Mage Frostbolt, Warlock Drain Life,
        Shaman Lightning Bolt) — secondary
  rotation_modifications:
    standard_st: |
      Standard ST rotation on Hex Lord. Adjust positioning for class-set
      ground effects:
      - Mage Flamestrike: spread + move-out
      - Druid Starfall: spread (Starfall hits randoms)
      - Warlock Shadowfury: spread + move-out (stun)
      - Hunter Trap Launcher Snake Trap (UNVERIFIED): ignore
    spirit_bolts_at_20: |
      Continue DPS during channel; instant abilities only if class is
      cast-on-move impaired:
      - Frost Mage: Ice Lance / Frost Bomb / Mage Bomb instants
      - Aff Warlock: DoTs maintained
      - MM Hunter: Steady Shot interrupted on movement; instant Aimed Shot proc only
      - Shadow Priest: Mind Flay channel; SW:P/VT instant
      - Balance Druid: instants (Moonfire/Sunfire/Starsurge)
      - Ele Shaman: Flame Shock instant
  cd_use:
    pop_offensive_at_pull: standard rotational
    save_burst_for_sub_20: pop final-burn CDs on Spirit Bolts trigger
  heroic_delta: |
    Heroic-only. Soul Drain damage higher; Spirit Bolts at sub-20%
    lethal if group not topped.
  bot_anti_patterns:
    - DO NOT pre-commit to a fixed kick rotation (depends on class roll)
    - DO NOT cluster ranged during class-set ground effects
    - DO NOT skip Soul Drain kicks (top kick priority always)
    - DO NOT skip healer-class hardcast kicks (boss heals himself = wipe risk)
  bot_implementation_required: |
    HEX LORD ABILITY-SET DETECTION HOOK — ranged DPS bot's interrupt
    priority queries encounter_state.hex_lord_class_roll and adjusts
    kick targets accordingly. Default fallback: interrupt all visible
    Hex Lord hardcasts.
```

---

## Boss 6 — Daakara

```yaml
boss_ranged:
  boss: daakara
  difficulty_grade: hard (form-state machine)
  positioning:
    default: max range; spread (3y+)
    facing: boss
    movement: |
      Per-form movement:
      - troll: minimal
      - bear: spread for Charge (knockback target prediction)
      - eagle: spread + move out of Cyclone / Static Charge ground patches
      - lynx: spread for Lynx Rush chain
      - dragonhawk: spread + move out of fire patches
  interrupt_role:
    grievous_throw: |
      TROLL FORM ONLY — MUST INTERRUPT. Rotation as Nalorakk Mojo Volley
      kick rotation.
    other_forms: no kicks required
  rotation_modifications:
    troll_form: |
      Standard ST + Grievous Throw kicks.
    bear_form: |
      Standard ST. No kicks. Stay spread for Charge.
    eagle_form: |
      Standard ST. Move out if you are Static Charge target. Cast-on-
      move instants if mobile.
    lynx_form: |
      Standard ST. No kicks. Stay spread for Lynx Rush.
    dragonhawk_form: |
      Standard ST. Spread + move out of Flame Whirl / Pillar of Fire
      ground patches.
  cd_use:
    standard_offensive: rotational; align with form windows where forms favor casts (troll = stable)
    final_burn: any held CD pops in last 20%
  heroic_delta: |
    Heroic-only. Form damage scales up; eagle/dragonhawk ground patches
    harder to dodge.
  bot_anti_patterns:
    - DO NOT continue casting during form-transition (boss may move/become invulnerable briefly)
    - DO NOT stand in eagle Cyclone / Static Charge patches
    - DO NOT stand in dragonhawk Flame Whirl / Pillar of Fire patches
    - DO NOT cluster (multiple form mechanics target spread)
  bot_implementation_required: |
    DAAKARA FORM-DETECTION HOOK — ranged DPS bot's positioning + kick
    priorities query encounter_state.daakara_active_form and adjust
    accordingly. Default fallback on form-change: spread + reactive
    move-out + standard ST.
```

---

## Bot AI hooks

```yaml
ai_hooks:
  akil_zon:
    - encounter_detect_static_disruption_telegraph → spread_check (default already)
    - encounter_detect_self_static_disruption_target → move_out_of_group + cast_on_move_instants
    - encounter_detect_eagle_storm_telegraph → STACK_WITH_GROUP (override default spread)
    - encounter_detect_call_lightning → kick_rotation_participate
    - encounter_detect_soaring_eagles_spawn → swap_to_aoe_rotation

  nalorakk:
    - encounter_state_form_troll → kick_rotation_for_mojo_volley
    - encounter_detect_mojo_volley_cast → fire_kick_if_off_cd
    - encounter_state_form_bear → standard_st_no_kicks
    - encounter_detect_surge_imminent → defensive_cd_if_self_target

  jan_alai:
    - encounter_detect_hatcher_spawn → swap_target_priority_to_hatcher
    - encounter_detect_boss_hp_lt_38 → prepare_aoe_rotation
    - encounter_detect_hatch_all_eggs → swap_to_aoe_rotation
    - encounter_detect_hatchlings_dead → resume_st_on_jan_alai
    - encounter_detect_self_in_fire_bomb → move_out

  halazzi:
    - encounter_detect_split_at_66 → swap_target_priority_per_aoe_capability
    - encounter_detect_split_at_33 → swap_target_priority_per_aoe_capability
    - encounter_detect_lightning_totem → swap_target_kill_totem
    - encounter_detect_frenzy_buff AND class_has_magic_dispel → dispel_target_boss
    - encounter_state_active → standard_st_otherwise

  hex_lord:
    - encounter_state_class_roll_detected → load_kick_priority_profile
    - encounter_detect_soul_drain_cast → fire_kick (top priority)
    - encounter_detect_class_set_hardcast → kick_per_profile
    - encounter_detect_class_set_ground_effect → spread_or_move_out
    - encounter_detect_boss_hp_lt_22 → final_burn_cd_window

  daakara:
    - encounter_detect_form_change → load_form_profile (spread/movement/kicks)
    - encounter_state_active_form == troll → kick_rotation_for_grievous_throw
    - encounter_state_active_form == eagle → spread + move_out_check
    - encounter_state_active_form == dragonhawk → spread + move_out_check
    - encounter_state_active_form == lynx → spread (Lynx Rush)
    - encounter_state_active_form == bear → spread (Charge)

dispel_blacklist: []                         # NO instance-wide blacklist

reactive_interrupt_state:
  hex_lord: {kick_profile: detected_at_pull_start; default_fallback: interrupt_all_hardcasts}
  daakara: {kick_profile: per_active_form; troll_form_only_kicks}
```

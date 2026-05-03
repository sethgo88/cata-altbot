# Zul'Aman — Melee DPS

**Melee-DPS-specific response logic** for the bot's encounter handling. Mechanics, spell IDs, phase triggers, and add abilities are defined in `encounter.md`; this doc only encodes how melee DPS reacts. Class-agnostic where possible.

**Layering**:
- Cross-content melee DPS patterns (positioning, interrupt rotation, threat ceiling) live in `docs/roles/melee-dps.md`
- Per-spec rotation lives in the active spec's `docs/specs/` doc (combat-rogue, fury-warrior, ret-paladin, frost-dk, enh-shaman, feral-cat)
- This doc layers ZA-specific deviations on top of those

---

## Dungeon-level overview

```yaml
dungeon_melee_dps_overview:
  total_bosses: 6
  estimated_full_clear_minutes: 40-60
  difficulty_modes: [heroic]                # heroic-only
  positioning_profile:
    akil_zon: TOGGLE_SPREAD_OR_STACK         # spread default; STACK on Eagle Storm telegraph
    nalorakk: behind_boss                    # behind in bear (cone); flexible in troll
    jan_alai: behind_boss                    # cone-aware; AoE on hatchlings during 35%
    halazzi: tank_share_or_split             # ONE melee shares Saber Lash with tank
    hex_lord: behind_boss                    # high kick density
    daakara: behind_boss                     # form-aware (cone in bear)
  interrupt_load:
    akil_zon: high                           # Call Lightning on cooldown
    nalorakk: HIGHEST                        # Mojo Volley every cast in troll form
    jan_alai: low                            # Hatcher channel only
    halazzi: zero                            # no kicks
    hex_lord: HIGHEST                        # Soul Drain + class-set
    daakara: medium                          # Grievous Throw on troll form
  key_responsibilities:
    - akil_zon_stack_override                # melee already in stack zone; just don't move on telegraph
    - kick_rotation_high_density             # Nalorakk + Hex Lord
    - saber_lash_partner_rule (halazzi)      # exactly ONE melee shares
    - aoe_burst_on_jan_alai_35_phase
    - hex_lord_class_set_detection
    - daakara_form_transition_response
```

---

## Trash — melee DPS notes

```yaml
trash_melee:
  amani_humanoids:
    cc_priorities: |
      Melee bot contributes Sap (Rogue) or Hammer of Justice (Pally) /
      Repentance (Pally) to opener-CC; primary role is kill-ordered DPS
      on skull marker. Combat Rogue can Sap before pull; Ret Pally can
      Repentance the second caster.
  fear_packs:
    notes: |
      No fear-cast trash specific to ZA in standard rotation. Standard
      kick rotation handles caster trash:
      - Combat Rogue: Kick (10s CD; off-GCD; STRONGEST melee kick)
      - Fury Warrior: Pummel (10s CD; off-GCD)
      - Ret Pally: Rebuke (15s CD; off-GCD)
      - Frost DK: Mind Freeze (10s CD; off-GCD)
      - Enh Shaman: Wind Shear (6s CD; off-GCD; 25y range)
      - Feral Cat: Skull Bash (60s CD; gap-close + interrupt)
  warbringer_battle_shout:
    notes: PRIORITY KICK — denies self-buff; rotate kicks across melee
  beast_packs:
    notes: |
      Bears, Lynxes, Dragonhawks: melee-position normally; no CC available
      for melee class on beasts. Brute-force kill.
  amani_savage:
    notes: tank stays still during Burning Brand stack; melee continues DPS
```

---

## Boss 1 — Akil'zon (Eagle Aspect)

```yaml
boss_melee:
  boss: akil_zon
  difficulty_grade: easy (positional toggle)
  positioning:
    default: stacked at melee under boss (default for melee anyway)
    eagle_storm_telegraph: |
      ALREADY STACKED — just don't run away. Static Disruption splash
      requires the TARGET to move out, not melee proactively spreading.
    facing: boss (no frontal cone)
    movement: minimal — Static Disruption-targeted melee runs out then back
  interrupt_role:
    call_lightning: PARTICIPATE — top kick priority for melee
  rotation_modifications:
    static_disruption_targeted: |
      If you are the target, MOVE OUT briefly then return to melee:
      - Combat Rogue: Sprint to recover position
      - Fury Warrior: Charge / Heroic Leap to re-engage
      - Ret Pally: standard movement
      - Frost DK: Death's Advance (talent — UNVERIFIED)
      - Enh Shaman: Ghost Wolf if needed (rare)
      - Feral Cat: Stampeding Roar self-speed if mid-mechanic
    eagle_storm_stack: |
      Stay stacked; continue melee on boss while in storm cloud.
      All melee rotations continue normally (storm-cloud doesn't break
      melee swings — UNVERIFIED for special abilities).
    add_phase: |
      AoE rotation on Soaring Eagles spawn:
      - Combat Rogue: Blade Flurry on; Fan of Knives if 3+ adds
      - Fury Warrior: Bladestorm or Whirlwind on add cluster
      - Ret Pally: Divine Storm at 3 HP
      - Frost DK: Howling Blast spam
      - Enh Shaman: Magma Totem + Chain Lightning at 5 MW + Earthquake (UNVERIFIED Cata melee Earthquake)
      - Feral Cat: Swipe (Cat) + maintain primary target Rip
  cd_use:
    standard_offensive: rotational on engagement
  heroic_delta: |
    Heroic-only. Static Disruption tick higher; Eagle Storm lethal if
    soloed.
  bot_anti_patterns:
    - DO NOT chase ranged players (they may be moving for Static Disruption)
    - DO NOT cast knockbacks (none of standard melee specs have these in ZA boss-fight contexts)
    - DO NOT delay Call Lightning kicks (top kick priority)
```

---

## Boss 2 — Nalorakk (Bear Aspect)

```yaml
boss_melee:
  boss: nalorakk
  difficulty_grade: medium (high kick density)
  positioning:
    default: BEHIND boss (avoid Brutal Swipe cone in bear form)
    facing: boss back
    movement: |
      Stay behind through form transitions. Surge charge target
      knockbacks; recover via gap-close (Charge / Death Grip — but
      Death Grip pulls boss off tank — DO NOT use).
  interrupt_role:
    mojo_volley: |
      MUST INTERRUPT every troll-form cast. Rotation:
      - Combat Rogue: Kick (10s; off-GCD; PRIMARY)
      - Fury Warrior: Pummel (10s; off-GCD)
      - Ret Pally: Rebuke (15s; off-GCD)
      - Frost DK: Mind Freeze (10s; off-GCD)
      - Enh Shaman: Wind Shear (6s; off-GCD; SHORTEST CD)
      - Feral Cat: Skull Bash (60s; gap-close + interrupt — long CD,
        save for emergency)
  rotation_modifications:
    troll_form: |
      Standard ST + kicks. Boss is more vulnerable in troll form
      (lower armor? UNVERIFIED) — pop offensive CDs in troll windows:
      - Combat Rogue: Adrenaline Rush + Killing Spree
      - Fury Warrior: Recklessness
      - Ret Pally: Avenging Wrath + Zealotry
      - Frost DK: Pillar of Frost + Raise Dead
      - Enh Shaman: Feral Spirit + Stormstrike spam
      - Feral Cat: Tiger's Fury + Berserk (cat)
    bear_form: |
      Standard ST. NO KICKS NEEDED. Continue rotation. Spread for Surge
      charge if cooldown is up.
    surge_charge: |
      If targeted, brace for knockback. Recover via:
      - Combat Rogue: Sprint
      - Fury Warrior: Charge / Heroic Leap
      - Ret Pally: Hand of Freedom (self) or just walk back
      - Frost DK: Path of Frost / standard movement
      - Enh Shaman: Ghost Wolf
      - Feral Cat: Stampeding Roar (self speed)
  cd_use:
    align_with_troll_form: pop offensive CDs in troll-form windows
  heroic_delta: |
    Heroic-only. Mojo Volley kicks mandatory; Mangle stacks higher in
    bear form.
  bot_anti_patterns:
    - DO NOT skip Mojo Volley kicks (top kick priority on this fight)
    - DO NOT stand in Brutal Swipe cone (bear form)
    - DO NOT use Death Grip (Frost DK) on Nalorakk (pulls off tank)
    - DO NOT cluster melee unnecessarily (Surge charge multi-hit)
```

---

## Boss 3 — Jan'alai (Dragonhawk Aspect)

```yaml
boss_melee:
  boss: jan_alai
  difficulty_grade: hard (AoE phase)
  positioning:
    default: BEHIND boss (Flame Breath cone)
    facing: boss back
    movement: |
      Move out of Fire Bomb ground patches. During 35% Hatch All Eggs
      phase, position with hatchling cluster (typically center where
      tank pulls them).
  interrupt_role:
    hatcher_channel: PARTICIPATE if Hatcher reaches eggs (fallback only)
  rotation_modifications:
    pre_35_phase: |
      Standard ST on Jan'alai; SWAP to Hatcher when it spawns:
      - Combat Rogue: Sap the Hatcher pre-engage if possible (humanoid);
        otherwise Garrote-Silence + ST burn
      - Fury Warrior: Spell Reflection on Hatcher channel cast
        (UNVERIFIED whether Spell Reflection works on channels);
        otherwise interrupt rotation
      - Ret Pally: Hammer of Justice stuns Hatcher channel; standard ST
      - Frost DK: Mind Freeze the channel; ST burn
      - Enh Shaman: Wind Shear the channel; ST burn
      - Feral Cat: Skull Bash the channel; ST burn
    35_phase_aoe: |
      SWAP TO AOE ROTATION on hatchlings:
      - Combat Rogue: Blade Flurry + Fan of Knives spam
      - Fury Warrior: Bladestorm (channel; immune to interrupts) +
        Whirlwind spam
      - Ret Pally: Divine Storm at 3 HP + Consecration
      - Frost DK: Howling Blast (Frost) + Pestilence spread + Death and
        Decay
      - Enh Shaman: Magma Totem + Earthquake (UNVERIFIED Cata) +
        Chain Lightning at 5 MW
      - Feral Cat: Swipe (Cat form AoE; talent buff) + Thrash (Bear-
        form-only; not in Cat) — Cat AoE is weaker; maintain Rip on
        primary boss target while Swiping
    after_hatchlings: resume ST on Jan'alai
  cd_use:
    save_offensive_for_35: |
      Pop AoE-burst CDs at 35% trigger:
      - Combat Rogue: Adrenaline Rush + Blade Flurry + Killing Spree
      - Fury Warrior: Bladestorm + Recklessness
      - Ret Pally: Avenging Wrath + Zealotry + Holy Wrath spam
      - Frost DK: Pillar of Frost + Army of the Dead (4min CD)
      - Enh Shaman: Feral Spirit + Bloodlust if not used elsewhere
      - Feral Cat: Berserk (cat) + Tiger's Fury
  heroic_delta: |
    Heroic-only. Hatchers tougher; Fire Bomb damage higher; 35% spawns
    more hatchlings.
  bot_anti_patterns:
    - DO NOT stand in Flame Breath cone arc (positioning)
    - DO NOT stand in Fire Bomb patches
    - DO NOT cleave Jan'alai during 35% (focus AoE on hatchlings)
    - DO NOT skip Hatcher kills (channel = double the hatchlings)
```

---

## Boss 4 — Halazzi (Lion Aspect)

```yaml
boss_melee:
  boss: halazzi
  difficulty_grade: medium (Saber Lash partner rule)
  positioning:
    SABER_LASH_PARTNER: |
      Exactly ONE melee bot is the "Saber Lash partner" — stands in
      tank's melee range to share Saber Lash damage with tank.
      OTHER melee bots position SIDE/BACK of tank to AVOID sharing.
      Bot election: lowest-HP-melee or designated melee bot is partner;
      others stay back.
    facing: boss back (standard melee)
    movement: minimal; swap to Spirit / Lightning Totem on splits
  interrupt_role:
    none: no kick-required hardcasts
  rotation_modifications:
    phase_1_pre_66: |
      Standard ST rotation. Saber Lash partner shares damage tick.
    split_at_66: |
      SWAP target to Spirit of the Lynx (or cleave both per Disagreement #8):
      - AoE-class (Combat Rogue Blade Flurry, Fury Warrior Bladestorm,
        Ret Pally Divine Storm, Frost DK Howling Blast, Enh Shaman
        Magma Totem, Feral Cat Swipe): cleave both
      - Pure ST: kill Spirit first, return to Halazzi
      AOE-CLASS RULE: melee bots default to CLEAVE BOTH (most melee
      specs have AoE tools).
    lightning_totem_kill: |
      Cleave priority — most melee AoE auto-hits the Totem if positioned
      correctly. Combat Rogue Blade Flurry; Fury Warrior Whirlwind;
      Frost DK Death and Decay; etc.
    phase_2_pre_33: standard ST on Halazzi
    split_at_33: same as 66% — repeat AoE/ST swap
    execute_phase: full burn on Halazzi
  dispel_role:
    flame_shock_dispel: |
      If class has Magic dispel:
      - Ret Pally: Cleanse (Magic + Disease + Poison; on-GCD; instant)
      - Enh Shaman: Cleanse Spirit (Curse + Magic [talented]; on-GCD)
      Otherwise let healer handle.
    frenzy_dispel: |
      Most melee classes don't have ENRAGE-targeted dispel. Tranquilizing
      Shot (Hunter) and Soothe (Druid) are ranged-class abilities. Melee
      bots: skip Frenzy dispel.
  cd_use:
    align_with_splits: pop offensive CDs on split transitions for Spirit burn
  heroic_delta: |
    Heroic-only. Lightning Totem hits faster; Spirit HP higher.
  bot_anti_patterns:
    - DO NOT have multiple melee in Saber Lash share zone (only ONE)
    - DO NOT skip Spirit of the Lynx spawns (target-priority swap)
    - DO NOT skip Lightning Totem (low HP; AoE Nature on group)
```

---

## Boss 5 — Hex Lord Malacrass

```yaml
boss_melee:
  boss: hex_lord_malacrass
  difficulty_grade: hard (non-deterministic; high kick density)
  positioning:
    default: BEHIND boss
    facing: boss back
    movement: |
      Move out of class-set ground effects (Flamestrike / Starfall /
      Shadowfury). Otherwise minimal.
  interrupt_role:
    soul_drain: |
      MUST INTERRUPT — top kick priority on this fight. Rotation:
      - Combat Rogue: Kick (10s; off-GCD; PRIMARY)
      - Fury Warrior: Pummel (10s; off-GCD)
      - Ret Pally: Rebuke (15s; off-GCD)
      - Frost DK: Mind Freeze (10s; off-GCD)
      - Enh Shaman: Wind Shear (6s; off-GCD; SHORTEST CD)
      - Feral Cat: Skull Bash (60s; gap-close + interrupt — save for healer-class roll)
    class_set_hardcasts: |
      Reactive interrupts based on detected class set:
      - Healer-class rolls (Resto Druid Tranquility, Holy Priest Heal,
        Holy Pally Holy Light, Resto Shaman Healing Wave) — HIGHEST
        priority kicks; missed kick = boss heals himself
      - DPS-class hardcasts — secondary
  rotation_modifications:
    standard_st: |
      Standard ST rotation. Adjust positioning for class-set ground effects:
      - Mage Flamestrike: move out
      - Druid Starfall: spread
      - Warlock Shadowfury: move out (stun)
    spirit_bolts_at_20: |
      Continue melee through channel (melee swings unaffected by Spirit
      Bolts cast). Pop defensive CD if HP-critical:
      - Combat Rogue: Cloak of Shadows (90s; magic resist)
      - Fury Warrior: Spell Reflection (UNVERIFIED whether AoE-reflectable)
      - Ret Pally: Divine Protection (-20% magic 10s)
      - Frost DK: Anti-Magic Shell (75% magic absorb 5s) — STRONG
      - Enh Shaman: Shamanistic Rage + Stoneclaw Totem
      - Feral Cat: Survival Instincts (-50% damage 12s)
  cd_use:
    pop_offensive_at_pull: standard rotational
    save_burst_for_sub_20: pop final-burn CDs on Spirit Bolts trigger
  heroic_delta: |
    Heroic-only. Soul Drain damage higher; Spirit Bolts at sub-20% lethal.
  bot_anti_patterns:
    - DO NOT pre-commit to a fixed kick rotation (depends on class roll)
    - DO NOT skip Soul Drain kicks (always top kick priority)
    - DO NOT skip healer-class hardcast kicks (boss heals himself)
    - DO NOT stand in class-set ground effects
  bot_implementation_required: |
    HEX LORD ABILITY-SET DETECTION HOOK — melee DPS bot's interrupt
    priority queries encounter_state.hex_lord_class_roll. Default
    fallback: interrupt all visible Hex Lord hardcasts.
```

---

## Boss 6 — Daakara

```yaml
boss_melee:
  boss: daakara
  difficulty_grade: hard (form-state machine)
  positioning:
    default: BEHIND boss
    facing: boss back
    movement: per-form rules (see below)
    per_form:
      troll: standard behind
      bear: BEHIND CRITICAL — Bear Claw cone is wider; tank face-management
      eagle: stay behind; move out of Cyclone / Static Charge ground patches
      lynx: stay behind; spread for Lynx Rush
      dragonhawk: stay behind; move out of Flame Whirl / Pillar of Fire
  interrupt_role:
    grievous_throw: |
      TROLL FORM ONLY — MUST INTERRUPT. Rotation as Nalorakk Mojo Volley
      kick rotation.
    other_forms: no kicks required
  rotation_modifications:
    troll_form: |
      Standard ST + Grievous Throw kicks. Pop offensive CDs in troll
      windows (boss is more vulnerable; UNVERIFIED).
    bear_form: |
      Standard ST. Behind position critical. No kicks.
    eagle_form: |
      Standard ST. Move out of Cyclone / Static Charge patches.
      Cast-on-move classes are advantaged.
    lynx_form: |
      Standard ST. Spread for Lynx Rush. Pop defensive CD if you become
      Lynx Rush chain target.
    dragonhawk_form: |
      Standard ST. Move out of Flame Whirl / Pillar of Fire. Spread.
  cd_use:
    standard_offensive: rotational; align with troll-form windows
    final_burn: any held CD pops in last 20%
  heroic_delta: |
    Heroic-only. Form damage scales; eagle/dragonhawk patches harder to dodge.
  bot_anti_patterns:
    - DO NOT continue swinging during form-transition (boss may move briefly)
    - DO NOT stand in eagle Cyclone / Static Charge patches
    - DO NOT stand in dragonhawk Flame Whirl / Pillar of Fire patches
    - DO NOT cluster melee (Lynx Rush chain hits multiple targets)
    - DO NOT use Death Grip (Frost DK) on Daakara (pulls off tank, also disrupts form positioning)
  bot_implementation_required: |
    DAAKARA FORM-DETECTION HOOK — melee DPS bot's positioning + kick
    priorities query encounter_state.daakara_active_form. Default
    fallback on form-change: behind boss + reactive move-out + ST.
```

---

## Bot AI hooks

```yaml
ai_hooks:
  akil_zon:
    - encounter_detect_self_static_disruption_target → move_out_of_group + cast_on_move
    - encounter_detect_eagle_storm_telegraph → STAY_STACKED (already in melee = already stacked)
    - encounter_detect_call_lightning → kick_rotation_participate
    - encounter_detect_soaring_eagles_spawn → swap_to_aoe_rotation

  nalorakk:
    - encounter_state_form_troll → kick_rotation_for_mojo_volley
    - encounter_detect_mojo_volley_cast → fire_kick_if_off_cd
    - encounter_state_form_bear → standard_st_no_kicks
    - encounter_detect_self_surge_target → defensive_cd + recover_position

  jan_alai:
    - encounter_detect_hatcher_spawn → swap_target_priority_to_hatcher
    - encounter_detect_boss_hp_lt_38 → prepare_aoe_rotation
    - encounter_detect_hatch_all_eggs → swap_to_aoe_rotation
    - encounter_detect_hatchlings_dead → resume_st_on_jan_alai
    - encounter_detect_self_in_fire_bomb → move_out
    - encounter_detect_self_in_flame_breath_cone → move_to_back

  halazzi:
    - encounter_state_active AND self_is_saber_lash_partner → stand_in_share_zone
    - encounter_state_active AND self_is_NOT_saber_lash_partner → stand_back_or_side
    - encounter_detect_split_at_66 → swap_target_per_aoe_capability
    - encounter_detect_split_at_33 → swap_target_per_aoe_capability
    - encounter_detect_lightning_totem → cleave_or_swap_kill_totem

  hex_lord:
    - encounter_state_class_roll_detected → load_kick_priority_profile
    - encounter_detect_soul_drain_cast → fire_kick (top priority)
    - encounter_detect_class_set_hardcast → kick_per_profile
    - encounter_detect_class_set_ground_effect → move_out
    - encounter_detect_boss_hp_lt_22 → final_burn_cd_window
    - encounter_detect_spirit_bolts_cast → defensive_cd_if_hp_critical

  daakara:
    - encounter_detect_form_change → load_form_profile
    - encounter_state_active_form == troll → kick_rotation_for_grievous_throw
    - encounter_state_active_form == bear → behind_position_critical
    - encounter_state_active_form == eagle → standard_behind + move_out_check
    - encounter_state_active_form == lynx → standard_behind + spread_check
    - encounter_state_active_form == dragonhawk → standard_behind + move_out_check

dispel_blacklist: []                         # NO instance-wide blacklist; melee dispel limited to Ret Pally Cleanse / Enh Shaman Cleanse Spirit

reactive_interrupt_state:
  hex_lord: {kick_profile: detected_at_pull_start; default_fallback: interrupt_all_hardcasts}
  daakara: {kick_profile: per_active_form; troll_form_only_kicks}
```

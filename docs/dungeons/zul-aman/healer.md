# Zul'Aman — Healer

**Healer-specific response logic** for the bot's encounter handling. Mechanics, spell IDs, phase triggers, and add abilities are defined in `encounter.md`; this doc only encodes how the healer reacts. Class-agnostic where possible.

**Layering**:
- Cross-content healer patterns (triage hierarchy, dispel taxonomy, mana pacing, anti-patterns) live in `docs/roles/healer.md`
- Per-spec rotation / CD / mana model lives in the active healer's `docs/specs/` doc (resto-shaman, holy-paladin, holy-priest, disc-priest, resto-druid)
- This doc layers ZA-specific deviations on top of those

---

## Dungeon-level overview

```yaml
dungeon_healer_overview:
  total_bosses: 6
  estimated_full_clear_minutes: 40-60
  difficulty_modes: [heroic]                # heroic-only
  mana_profile:
    akil_zon: moderate                       # Static Disruption splash + Eagle Storm shared damage
    nalorakk: moderate                       # Mangle stack ramp in bear form; troll form light
    jan_alai: HIGH                           # MANA-CHECK fight — 35% Hatch All Eggs phase
    halazzi: moderate                        # split transitions + Flame Shock dispels
    hex_lord: VARIABLE                       # depends on rolled class set; Soul Drain damage steady
    daakara: moderate to high                # form-transition damage spikes; Lynx-form tank burst
  drink_windows:
    - after Akil'zon
    - after Nalorakk                         # full mana for Jan'alai (mana check)
    - after Jan'alai                         # recover from Hatch All Eggs phase
    - after Halazzi
    - after Hex Lord                         # full mana for Daakara
  key_responsibilities:
    - reactive_dispel_on_halazzi             # Frenzy (boss buff) + Flame Shock (debuff)
    - reactive_dispel_on_hex_lord            # depends on rolled class set
    - mana_pacing_for_jan_alai_hatch_phase
    - cooldown_pacing                        # Mana Tide / Spirit Link / Hymn / Innervate / Tranquility placement
    - form_transition_anticipation_on_daakara
    - eagle_storm_pre_stack_heal_on_akil_zon
    - tank_burst_response_on_daakara_lynx_form
```

---

## Trash — healer notes

```yaml
trash_healer:
  amani_humanoids:
    dispels: []                              # no significant dispel calls
    cooldown_use: none required; reactive only
    notes: |
      Watch for Battle Shout cast (Warbringer); if landed, trash hits
      harder for ~30s — no dispel; just heal through.

  amani_savage_bleed_packs:
    notes: Burning Brand bleed on tank; standard tank-heal flow

  caster_packs:
    notes: |
      Tempest / Hatcher casters; if interrupts fail, multi-target Lightning
      Bolt damage spikes group. Reactive AoE heal.

  hatcher_caster_packs:
    notes: pre-Jan'alai trash; pre-mana for the boss fight

  amani_dragonhawk_packs:
    notes: cone breath on tank; pre-HoT before pull
```

---

## Boss 1 — Akil'zon (Eagle Aspect)

```yaml
boss_healer:
  boss: akil_zon
  difficulty_grade: medium
  mana_intensity: moderate

  threshold_overrides:
    tier_2_tank_emergency: tank.hp_pct < 50  # standard
    tier_3_static_disruption_targeted: |
      If targeted player doesn't move out, splash damage hits group.
      Reactive AoE heal threshold lowered to party.injured_count(radius=8y, hp_pct_lt=85) >= 3

  predictable_spike_events:
    - eagle_storm_telegraph:
        warning: ~3-5s telegraph before lift
        damage: AoE Nature damage shared among lifted players
        response: |
          Pre-cast AoE heal cooldown to land during channel:
          - Resto Shaman: Healing Stream Totem + Healing Rain at stack point
          - Holy Paladin: Holy Radiance + Light of Dawn pre-stack
          - Holy Priest: Circle of Healing + Prayer of Healing on stack
          - Disc Priest: Power Word: Barrier ground-place at stack point
          - Resto Druid: Wild Growth pre-stack
        positioning_note: HEALER STACKS WITH GROUP during Eagle Storm
    - static_disruption_targeted_splash:
        damage: Nature damage on target + group splash if target doesn't move
        response: reactive single-target heal on target; AoE heal on splash
    - call_lightning_targeted:
        damage: single-target Nature on random
        response: |
          Reactive heal IF DPS misses interrupt. Should be rare on heroic
          if DPS rotation is on point.
    - soaring_eagles_spawn:
        damage: light cleave on group while adds reach players
        response: AoE heal during burn

  cooldown_use:
    standard: reactive HoT on tank (Riptide / Beacon / Renew / PW:S / Lifebloom)
    aoe_window: align AoE heal cooldown to Eagle Storm channel
    no_unique_cd_save: |
      Save Mana Tide / Innervate / Hymn / Spirit Link / Tranquility for
      Jan'alai 35% phase. Akil'zon shouldn't burn major mana CD.

  dispel_priority: []                        # no confirmed dispels

  interrupt_role:
    call_lightning: PARTICIPATE in interrupt rotation if class has kick (Resto Shaman Wind Shear, Holy Pally Rebuke, Holy Priest doesn't have one — skip; Disc Priest doesn't have one — skip; Resto Druid Skull Bash if traveling? UNVERIFIED — likely skip)

  heroic_delta: |
    Heroic-only; consistency. Eagle Storm lethal if soloed; pre-stack heal
    CD timing is critical.

  bot_anti_patterns:
    - DO NOT chase strays during Eagle Storm telegraph (be in the stack yourself)
    - DO NOT cast healing rain / wild growth / circle of healing while spread (mis-positioning)
    - DO NOT burn Mana Tide / Innervate on Akil'zon (save for Jan'alai)
```

---

## Boss 2 — Nalorakk (Bear Aspect)

```yaml
boss_healer:
  boss: nalorakk
  difficulty_grade: medium
  mana_intensity: moderate

  threshold_overrides:
    tier_2_tank_emergency: tank.hp_pct < 60  # raised — Mangle bleed stacks ramp tank damage

  predictable_spike_events:
    - bear_form_transition:
        warning: form-change visual telegraph
        damage: Mangle bleed begins stacking on tank; Brutal Swipe cone hits
        response: |
          Pre-HoT tank BEFORE form-swap to bear (predictable from telegraph).
          Mangle stacks reset on next troll-swap, so manage stack-window
          rather than panic-heal.
    - troll_form_transition:
        warning: form-change visual telegraph
        damage: |
          Mojo Volley if uninterrupted (group damage); Mangle stacks reset
          (tank pressure relief).
        response: |
          Group-AoE heal IF Mojo Volley lands. If kicked reliably, troll
          form is healer-relief window.
    - surge_charge:
        damage: charge impact + knockback on random player
        response: pre-shield random target if pattern is predictable; reactive heal post-impact

  cooldown_use:
    standard: HoT-roll tank during bear form
    bear_form_high_stack: pop short defensive cooldown buff for tank if class supports (Pain Suppression / Guardian Spirit / Beacon-stacked-heal)
    save_for_jan_alai: still hold Mana Tide / Innervate / Hymn / Spirit Link

  dispel_priority: []                        # bleeds are Physical; no Magic dispels

  interrupt_role:
    mojo_volley: PARTICIPATE if class has kick — Resto Shaman Wind Shear is best fit (off-GCD, 25y range)

  heroic_delta: |
    Heroic-only. Mojo Volley unintercepted = group damage spike; Mangle
    stacks higher in bear form.

  bot_anti_patterns:
    - DO NOT keep Mangle stacks in mind during troll form (they reset)
    - DO NOT skip pre-HoT before bear-form transition (predictable)
    - DO NOT burn major mana CD on Nalorakk (save for Jan'alai)
```

---

## Boss 3 — Jan'alai (Dragonhawk Aspect)

```yaml
boss_healer:
  boss: jan_alai
  difficulty_grade: hard
  mana_intensity: HIGH
  notes: |
    THE MANA-CHECK FIGHT of ZA heroic. The 35% Hatch All Eggs phase is the
    canonical AoE-burn check for healer mana. Save all major mana CDs for
    here. Pre-position group for AoE heals before the trigger.

  threshold_overrides:
    tier_2_tank_emergency: tank.hp_pct < 60  # AoE pressure during 35% phase
    tier_3_aoe_threshold: party.injured_count(radius=12y, hp_pct_lt=80) >= 3 → AoE heal

  predictable_spike_events:
    - hatcher_spawn:
        damage: hatchlings reach group if Hatcher channels uninterrupted
        response: |
          Reactive AoE heal during hatchling cleave; tank picks up via
          AoE threat. If DPS kills Hatcher fast, no significant damage.
    - flame_breath_cone:
        damage: cone hit on whoever is in arc
        response: reactive heal — arc-clipping should be rare
    - fire_bomb_ground_patch:
        damage: DoT on slow-movers
        response: reactive HoT + remove-from-fire dispel-flavor (stand out — not actual dispel)
    - hatch_all_eggs_at_35:
        warning: PREDICTABLE HP TRIGGER
        damage: massive AoE from many hatchling melees; group-wide pressure
        response: |
          PRE-CAST major AoE heal CDs at boss HP 38-40%:
          - Resto Shaman: Mana Tide Totem + Spirit Link Totem + Healing Rain + Healing Stream Totem
          - Holy Paladin: Avenging Wrath + Holy Radiance + Light of Dawn spam + Aura Mastery (Devotion Aura -20% AoE)
          - Holy Priest: Hymn of Hope + Divine Hymn (channel) + Circle of Healing + Prayer of Healing
          - Disc Priest: Power Word: Barrier + Pain Suppression on tank + Penance + Power Word: Shield rotation
          - Resto Druid: Tranquility (channel) + Wild Growth + Lifebloom-roll on tank + Tree of Life form

  cooldown_use:
    mana_tide_totem: deploy at boss HP 38% (pre-position for hatch trigger)
    innervate: self-cast at ~30% mana during 35% phase
    hymn_of_hope: cast at boss HP 38% (group mana relief)
    spirit_link_totem: deploy during 35% hatch phase (HP equalization)
    tranquility: deploy during 35% hatch phase (channel through; Tree of Life double-effect)
    avenging_wrath: cast at 35% trigger (Holy Pally CD)
    aura_mastery: cast at 35% trigger (Holy Pally; -20% AoE for group)
    power_word_barrier: ground-place at group stack-point during 35% phase

  dispel_priority: []                        # no dispels

  interrupt_role:
    hatcher_channel: PARTICIPATE if Hatcher kill is delayed — Resto Shaman Wind Shear; otherwise DPS kills the Hatcher

  heroic_delta: |
    Heroic-only. Hatchers tougher; Fire Bomb damage higher; 35% Hatch All
    Eggs more hatchlings. Mana check intensifies.

  bot_anti_patterns:
    - DO NOT panic-flash-heal during Hatch All Eggs (mana-blow)
    - DO NOT skip Mana Tide / Hymn / equivalent — Jan'alai 35% IS the moment
    - DO NOT stand in Fire Bomb ground patches (you're the healer, not the tank)
    - DO NOT save Tranquility for Daakara (Jan'alai 35% is the higher-priority window)
```

---

## Boss 4 — Halazzi (Lion Aspect)

```yaml
boss_healer:
  boss: halazzi
  difficulty_grade: medium
  mana_intensity: moderate

  threshold_overrides:
    tier_2_tank_emergency: tank.hp_pct < 60  # Saber Lash splash + Frenzy ramp

  predictable_spike_events:
    - saber_lash_split:
        damage: split between tank + one melee
        response: HoT both targets; reactive heal on damage tick
    - split_at_66:
        damage: transition damage + Spirit of the Lynx melee on aggro
        response: |
          Pre-HoT tank before 66% trigger; group AoE heal during split
          transition. Spirit goes to closest target — usually a DPS.
    - split_at_33:
        damage: same as 66%; second Spirit
        response: same — pre-HoT + AoE heal
    - frenzy_undispelled:
        damage: enraged Halazzi auto-attacks heavier on tank
        response: dispel ASAP (Tranquilizing Shot / Soothe / Spell Steal); reactive tank heal until dispelled
    - flame_shock_undispelled:
        damage: Fire DoT on random player
        response: dispel as Magic; reactive HoT on target

  cooldown_use:
    mana_tide_totem: deploy on first split (66%) if not used elsewhere
    innervate: self-cast at ~30% mana
    avenging_wrath_holy_pally: align with split transition
    standard_reactive: HoT-roll tank + Spirit of the Lynx target

  dispel_priority:
    - Frenzy (boss buff)                    # SHOULD_DISPEL via Magic — Tranquilizing Shot / Soothe / Spell Steal
    - Flame Shock (debuff on player)        # SHOULD_DISPEL via Magic — Dispel Magic / Cleanse / Cleanse Spirit / Remove Corruption
    notes: |
      DISPEL IS THE PRIMARY HEALER ACTION on this fight. Frenzy untreated
      = tank dies faster. Flame Shock untreated = mana burn.

  interrupt_role:
    none: no kick-required hardcasts

  heroic_delta: |
    Heroic-only. Lightning Totem hits faster (kill priority); Spirit HP
    higher; Frenzy dispel timing more critical.

  bot_anti_patterns:
    - DO NOT skip Frenzy dispel (Magic-class healers must include it in dispel rotation)
    - DO NOT skip Flame Shock dispel
    - DO NOT cleave-heal during splits (focus on tank + Spirit's target)
```

---

## Boss 5 — Hex Lord Malacrass

```yaml
boss_healer:
  boss: hex_lord_malacrass
  difficulty_grade: hard
  mana_intensity: variable
  notes: |
    NON-DETERMINISTIC FIGHT. Healer must REACTIVELY DETECT Hex Lord's
    rolled ability set in the first 30s and adjust dispel + healing
    profile accordingly.

  threshold_overrides:
    tier_2_tank_emergency: tank.hp_pct < 60  # Soul Drain + class-set damage
    tier_2_soul_drain_target: |
      Soul Drain target HP drops fast; reactive heal threshold lowered to
      target.hp_pct < 70 if Soul Drain channel detected.

  predictable_spike_events:
    - soul_drain_channel:
        damage: HP + mana drain on target
        response: |
          PRIMARY: rely on DPS interrupt. SECONDARY: reactive heal on
          target if interrupt fails.
    - spirit_bolts_at_20:
        warning: PREDICTABLE HP TRIGGER (sub-20% threshold)
        damage: AoE Shadow nuke on group
        response: |
          PRE-CAST AoE heal CDs at boss HP 22%:
          - Resto Shaman: Spirit Link Totem + Healing Rain + Healing Stream
          - Holy Paladin: Aura Mastery + Holy Radiance + Light of Dawn
          - Holy Priest: Divine Hymn (channel) + Power Word: Barrier
          - Disc Priest: Power Word: Barrier + Pain Suppression rotation
          - Resto Druid: Tranquility (channel) + Wild Growth
    - class_set_damage:
        damage: VARIABLE — depends on rolled class
        response: REACTIVE based on detected ability set

  reactive_dispel_profile:
    # Detected at runtime based on Hex Lord's class roll
    shadow_priest_rolled:
      dispel_targets:
        - Shadow Word: Pain (DoT, dispel Magic)
        - Vampiric Touch (DoT, dispel Magic)
      priority: HIGH — DoT dispels are mana-positive for group
    affliction_warlock_rolled:
      dispel_targets:
        - Curse of Agony (Curse — dispel via Druid Remove Corruption / Mage Remove Curse)
        - Corruption / Bane of Doom (DoT — dispel Magic if class can)
      priority: HIGH — multiple debuffs to clear
    disc_priest_rolled:
      dispel_targets:
        - Power Word: Shield (dispel Magic to remove Hex Lord's self-shield) — Mage Spell Steal preferred
      priority: MEDIUM
    fire_mage_rolled:
      dispel_targets:
        - (Hex Lord may apply Ignite-flavor DoT — dispel Magic)
      priority: MEDIUM
    holy_priest / resto_druid / resto_shaman / holy_paladin_rolled:
      dispel_targets: []                     # healing-class rolls; DPS interrupts are primary
      priority: KICK_PRIORITY                # focus DPS interrupts on Hex Lord's heal casts
    other_rolls:
      dispel_targets: []                     # depends on specific ability subset
      priority: REACTIVE

  cooldown_use:
    mana_tide_totem: deploy at boss HP 22% (pre-position for Spirit Bolts) if not used elsewhere
    innervate: self-cast at ~30% mana
    spirit_link_totem: deploy at 22% (HP equalization for Spirit Bolts)
    tranquility: deploy at 22% (Spirit Bolts mitigation)
    aura_mastery: deploy at 22% (Holy Pally; -20% AoE)
    power_word_barrier: ground-place at 22% group stack point
    save_some_for_daakara: keep at least one mana-recovery CD for final boss

  dispel_priority:
    REACTIVE_PER_CLASS_SET: see reactive_dispel_profile above
    DISPEL_BLACKLIST: []                     # no fight-level blacklist (unlike BRC Crepuscular Veil)

  interrupt_role:
    soul_drain: PRIMARY kick if class has it — Resto Shaman Wind Shear; otherwise DPS rotation
    class_set_hardcasts: |
      Healer participates in interrupt rotation only on healer-class rolls
      (Resto Druid Tranquility, Holy Priest Heal, Holy Paladin Holy Light,
      Resto Shaman Healing Wave) — these are the highest-priority kicks
      because Hex Lord healing himself is a wipe risk.

  heroic_delta: |
    Heroic-only. Soul Drain damage higher; Spirit Bolts at sub-20% lethal
    if group not topped.

  bot_anti_patterns:
    - DO NOT pre-commit to a fixed dispel rotation (depends on class roll)
    - DO NOT skip Spirit Bolts pre-shielding (predictable trigger)
    - DO NOT auto-dispel Hex Lord's offensive shields if Mage on group (Spell Steal converts the shield to your buff)

  bot_implementation_required: |
    HEX LORD ABILITY-SET DETECTION HOOK — healer's dispel logic queries
    encounter_state.hex_lord_class_roll at start of pull. Profile:
      hex_lord_class_roll = {none, frost_mage, fire_mage, ..., resto_druid, ...}
    Healer's dispel + interrupt + cooldown rules use the rolled class to
    select the active sub-profile. Default fallback if unrecognized:
    "dispel Magic on group; interrupt all Hex Lord hardcasts; pre-shield
    for Spirit Bolts."
```

---

## Boss 6 — Daakara

```yaml
boss_healer:
  boss: daakara
  difficulty_grade: hard
  mana_intensity: moderate to high

  threshold_overrides:
    tier_2_tank_emergency: tank.hp_pct < 60  # standard except Lynx form
    tier_2_lynx_form_tank_emergency: tank.hp_pct < 75   # raised — Claw Rage burst

  predictable_spike_events:
    - troll_form_grievous_throw_uninterrupted:
        damage: heavy bleed DoT on target
        response: HoT-roll target + reactive heal; rely on DPS kick primarily
    - bear_form_charge:
        damage: charge impact + knockback
        response: reactive heal post-impact
    - eagle_form_static_charge_targeted:
        damage: Nature damage + ground patch on target
        response: reactive heal target; pre-position spread
    - eagle_form_cyclone_ground_patch:
        damage: DoT on slow-movers
        response: reactive HoT on target if standing in patch
    - lynx_form_claw_rage:
        damage: BURST tank damage — highest spike in the fight
        response: |
          PRE-HOT tank on Lynx form transition; align tank-defensive CD
          (Shield Wall / Survival Instincts / Vampiric Blood) — but those
          are tank's responsibility. Healer pre-shields tank (PW:S / Earth
          Shield ramp).
    - lynx_form_lynx_rush:
        damage: chain charge — party-wide
        response: AoE heal during Lynx form
    - dragonhawk_form_flame_whirl:
        damage: AoE fire patches; spread + move-out check
        response: reactive HoT on patch-clippers
    - form_transition:
        damage: depends on incoming form; usually a damage lull
        response: |
          PREDICT incoming form via boss aura; pre-position heal style
          (eagle = AoE-pre-spread, lynx = burst-tank pre-HoT, dragonhawk =
          AoE-fire pre-HoT).

  cooldown_use:
    mana_tide_totem: deploy on Lynx form transition (highest pressure)
    innervate: self-cast at ~30% mana
    spirit_link_totem: deploy if multi-target damage (Lynx Rush chain or dragonhawk fire patches)
    avenging_wrath: align with Lynx form transition
    final_burn: any held CD pops in final 20%

  dispel_priority: []                        # no Magic dispels confirmed; bleeds are Physical

  interrupt_role:
    grievous_throw: PARTICIPATE on troll form if class has kick (Resto Shaman Wind Shear)

  heroic_delta: |
    Heroic-only. Form damage scales; Lynx Claw Rage burst higher; dragonhawk
    fire patches harder to dodge.

  bot_anti_patterns:
    - DO NOT cast big-mana heals during damage lulls (form transitions can have 2-3s pause)
    - DO NOT ignore boss aura — form change is your primary state signal
    - DO NOT save all CDs for Daakara — Jan'alai 35% and Hex Lord 20% have higher windows

  bot_implementation_required: |
    DAAKARA FORM-DETECTION HOOK — healer's response profile queries
    encounter_state.daakara_active_form at every tick. Profile:
      daakara_active_form = {troll, bear, eagle, lynx, dragonhawk, transition}
    Per-form sub-profile applies to threshold overrides + cooldown timing.
```

---

## Bot AI hooks

```yaml
ai_hooks:
  akil_zon:
    - encounter_detect_static_disruption_telegraph → spread_positioning_check
    - encounter_detect_eagle_storm_telegraph → pre_cast_aoe_heal_cd AND stack_with_group
    - encounter_detect_call_lightning → kick_rotation_participate (if class has kick)
    - encounter_detect_soaring_eagles_spawn → reactive_aoe_heal

  nalorakk:
    - encounter_detect_form_change_to_bear → pre_hot_tank
    - encounter_detect_mojo_volley_cast → kick_rotation_participate
    - encounter_detect_surge_charge → reactive_heal_target
    - encounter_state_bear_form_high_mangle_stack → pop_short_tank_cd

  jan_alai:
    - encounter_detect_hatcher_spawn → no_action_change (DPS kills it)
    - encounter_detect_boss_hp_lt_38 → pre_position_aoe_heal_cds
    - encounter_detect_hatch_all_eggs → pop_major_aoe_cd (Mana Tide / Spirit Link / Tranquility / Hymn)
    - encounter_detect_fire_bomb_ground_patch → reactive_hot_on_patch_targets

  halazzi:
    - encounter_detect_frenzy_buff → dispel_magic_target_boss
    - encounter_detect_flame_shock_debuff → dispel_magic_target_player
    - encounter_detect_split_at_66 → pre_hot_tank AND aoe_heal_window
    - encounter_detect_split_at_33 → pre_hot_tank AND aoe_heal_window

  hex_lord:
    - encounter_state_class_roll_detected → load_dispel_profile
    - encounter_detect_soul_drain_channel → reactive_heal_target (kick is primary)
    - encounter_detect_boss_hp_lt_22 → pre_position_aoe_heal_cds
    - encounter_detect_spirit_bolts → pop_major_aoe_cd
    - encounter_detect_class_set_dispellable_debuff → dispel_per_profile
    - encounter_detect_hex_lord_self_shield AND mage_on_group → spell_steal_target

  daakara:
    - encounter_detect_form_change → load_form_profile
    - encounter_state_active_form == lynx → pre_hot_tank AND pre_shield_tank
    - encounter_state_active_form == dragonhawk → pre_position_aoe_heal AND watch_fire_patches
    - encounter_state_active_form == eagle → pre_position_spread_heal
    - encounter_detect_grievous_throw_landed → reactive_hot_on_target

dispel_blacklist: []                         # NO instance-wide blacklist; reactive dispels per fight

mana_cd_save_priority:
  akil_zon:    [no_save]
  nalorakk:    [no_save]
  jan_alai:    [HIGH PRIORITY — Mana Tide / Innervate / Hymn / Spirit Link / Tranquility at 38% boss HP]
  halazzi:     [Mana Tide if not used on Jan'alai; secondary CD]
  hex_lord:    [Spirit Link / Tranquility / Power Word: Barrier at 22% boss HP]
  daakara:     [secondary CDs on Lynx form; final burn CDs in last 20%]

reactive_dispel_state:
  halazzi: {dispel_magic_targets: [Frenzy_buff_on_boss, Flame_Shock_debuff_on_player]}
  hex_lord: {dispel_profile: detected_at_pull_start}
```

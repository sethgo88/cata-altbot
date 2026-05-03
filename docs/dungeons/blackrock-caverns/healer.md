# Blackrock Caverns — Healer

**Healer-specific response logic** for the bot's encounter handling. Mechanics, spell IDs, phase triggers, and add abilities are defined in `encounter.md`; this doc only encodes how the healer reacts. Class-agnostic where possible.

**Layering**:
- Cross-content healer patterns (triage hierarchy, dispel taxonomy, mana pacing, anti-patterns) live in `docs/roles/healer.md`
- Per-spec rotation / CD / mana model lives in the active healer's `docs/specs/` doc (resto-shaman, holy-paladin, holy-priest, disc-priest, resto-druid)
- This doc layers BRC-specific deviations on top of those

---

## Dungeon-level overview

```yaml
dungeon_healer_overview:
  total_bosses: 4 (+ 1 optional)
  estimated_full_clear_minutes: 25-45
  mana_profile:
    rom_ogg: low                          # short fight; tank pressure only
    corla: low_to_moderate                # beam-intercepter is steady damage source
    karsh: high                           # canonical Cata-leveling OOM-test for new healers
    obsidius: moderate                    # tank Twilight Corruption stacks ramp
  drink_windows:
    - after Rom'ogg
    - after Corla
    - after Karsh                         # MOST IMPORTANT — go into Obsidius full mana
  key_responsibilities:
    - dispel_blacklist_per_fight          # CRITICAL on Obsidius
    - mana_pacing_for_karsh_oom_test
    - cooldown_pacing                     # Mana Tide / Spirit Link / Hymn of Hope / Innervate placement
    - anti_pattern_awareness              # NEVER auto-dispel Crepuscular Veil
```

---

## Trash — healer notes

```yaml
trash_healer:
  twilight_humanoids:
    dispels: []                             # no significant dispel calls on standard trash
    cooldown_use: none required; reactive only
    notes: |
      Watch for fear-cast Twilight Sadists (interrupt role for DPS); if fear
      lands, healer waits for DR to drop then continues triage.

  conflagrations:
    notes: environmental fire; not a healer concern, but pre-shield melee who must walk through

  quicksilver_tank_packs:
    notes: tank takes frontal_cleave damage; standard tank-heal flow
```

---

## Boss 1 — Rom'ogg Bonecrusher

```yaml
boss_healer:
  boss: rom_ogg_bonecrusher
  difficulty_grade: easy
  mana_intensity: low

  threshold_overrides:
    tier_2_tank_emergency: tank.hp_pct < 50    # raised — Skullcracker stack damage adds party-wide pressure

  predictable_spike_events:
    - skullcracker_channel:
        warning: Chains of Woe (75539) precedes by ~6s
        damage: AoE shared among stacked group
        response: |
          Pre-cast AoE heal cooldown to land during channel ticks:
          - Resto Shaman: Healing Stream Totem + Healing Rain pre-drop
          - Holy Paladin: Holy Radiance + Light of Dawn pre-stack
          - Holy Priest: Circle of Healing + PoH on stack
          - Disc Priest: PoM bounces + Power Word Barrier ground-place
          - Resto Druid: Wild Growth pre-stack + Tranquility on heroic
    - add_spawn_at_50:
        damage: light cleave on stack
        response: continue tank focus; AoE heal supports cleave damage

  cooldown_use:
    standard: reactive HoT on tank (Riptide / Beacon / Renew / PW:S / Lifebloom)
    aoe_window: align cooldown to Skullcracker channel
    no_unique_cd_save: |
      Save Mana Tide / Innervate / Hymn / Spirit Link for Karsh fight; Rom'ogg
      shouldn't burn major mana CD.

  dispel_priority: []                       # no dispels required

  interrupt_role:
    quake: PARTICIPATE in interrupt rotation if class has kick (Resto Shaman Wind Shear, Holy Pally Rebuke, etc.)

  heroic_delta: |
    - Skullcracker stack damage higher; AoE heal cooldown more important
    - Quake unintercepted = healer pressure on whole group

  bot_anti_patterns:
    - DO NOT chase strays during Skullcracker (stacked damage-share is the strat)
    - DO NOT burn Mana Tide / equivalent on Rom'ogg (save for Karsh / Obsidius)
```

---

## Boss 2 — Corla, Herald of Twilight

```yaml
boss_healer:
  boss: corla_herald_of_twilight
  difficulty_grade: easy
  mana_intensity: low_to_moderate

  threshold_overrides:
    tier_3_beam_intercepter: |
      Whichever bot is currently intercepting a beam takes steady damage.
      Treat them as a soft tank for the duration of intercept (HoT-rolling
      target).

  predictable_spike_events:
    - beam_intercept:
        damage: continuous Shadow damage while in beam
        duration: stacks 1→4 on normal, 1→3 on heroic before swap
        response: |
          Apply HoT to current intercepter (Riptide on Resto Shaman, Renew on
          Priest, Lifebloom + Rejuv on Druid, Beacon + Light of Dawn for Holy
          Pally).

  cooldown_use:
    none_required: standard reactive healing
    save_for_karsh: still hold Mana Tide / Innervate

  dispel_priority: []                       # no dispels — Twilight Evolution is non-dispellable

  interrupt_role:
    none: no kick-required casts

  heroic_delta: |
    - Beam intercept damage higher
    - Two-DPS-per-beam rotation means more hp swings as players swap in/out
    - Healer pressure on whoever holds beam longest

  bot_anti_patterns:
    - DO NOT attempt to dispel Twilight Evolution stacks (75571) — not dispellable
    - DO NOT stop healing the beam-intercepter to top off off-target players
```

---

## Boss 3 — Karsh Steelbender

```yaml
boss_healer:
  boss: karsh_steelbender
  difficulty_grade: hard
  mana_intensity: high
  notes: |
    THE canonical Cata leveling OOM-test for new healers. Mana pacing is the
    primary skill check. Tank takes predictable fire damage on each plume
    pass; group takes occasional Cinderbreath splash.

  threshold_overrides:
    tier_2_tank_emergency: tank.hp_pct < 60   # raised — Searing Lava DoT can spike tank during plume pass

  predictable_spike_events:
    - plume_pass_tank_damage:
        cadence_normal: ~10-12s
        cadence_heroic: ~8-10s
        damage: Searing Lava DoT on tank for ~3-5s exposure
        response: |
          Pre-HoT BEFORE plume pass; reactive heal during; one big heal after
          (Greater Healing Wave / Holy Light / Healing Touch / etc.).
          Pacing rule: don't flash-heal; use mana-efficient heals between
          passes.

  cooldown_use:
    mana_tide_totem: deploy ~50% boss HP (mana check window)
    innervate: self-cast ~30% mana (Resto Druid; pre-Cata fixed Innervate target rules)
    hymn_of_hope: combat-mana-tide (Holy Priest); ~40% mana
    spirit_link_totem: HP-distribution if group HP varies wildly (Resto Shaman; rare on Karsh)
    avenging_wrath_holy_pally: standard rotational on Holy Pally
    tranquility: emergency raid heal if group HP collapses (rare on Karsh)

  dispel_priority: []                       # no dispels

  interrupt_role:
    none: Cinderbreath generally non-interruptible

  heroic_delta: |
    - Faster plume cadence (~8-10s) = more frequent damage spikes
    - Cinderbreath hits harder if range positioning sloppy
    - Mana check intensifies — bot must pace heals carefully

  bot_anti_patterns:
    - DO NOT panic-flash-heal on every plume pass (mana-blow)
    - DO NOT stand in plume yourself (you are not the tank)
    - DO NOT skip Mana Tide / equivalent — Karsh is the moment
```

---

## Boss 4 (optional) — Beauty

```yaml
boss_healer:
  boss: beauty
  optional: true
  default_skip: true
  difficulty_grade: medium
  mana_intensity: moderate

  predictable_spike_events:
    - berserker_charge_stun_target:
        damage: charge impact + immediate melee follow-up
        response: pre-shield (PW:S / Earth Shield) random target; reactive heal post-stun
    - terrifying_roar_fear:
        duration: 4-6s of unhealable group movement
        counter: Tremor Totem (if Resto Shaman); Fear Ward (if Holy/Disc Priest)
        response: pre-emptive HoT before Roar telegraph; recover after

  cooldown_use:
    save_for_real_bosses: Beauty is optional; don't burn major CDs

  dispel_priority: []                       # fear is not a dispel

  interrupt_role:
    terrifying_roar: ATTEMPT — UNVERIFIED interruptibility

  heroic_delta: |
    - Charge stun longer; landed players take more damage post-stun
    - Roar fear longer; counter abilities more valuable

  bot_anti_patterns:
    - DO NOT auto-cleanse on this fight (no relevant dispels)
    - If on Resto Shaman: drop Tremor Totem (8143) PRE-pull, not after Roar lands
```

---

## Boss 5 — Ascendant Lord Obsidius

```yaml
boss_healer:
  boss: ascendant_lord_obsidius
  difficulty_grade: medium
  mana_intensity: moderate

  threshold_overrides:
    tier_2_tank_emergency: tank.hp_pct < 50   # Twilight Corruption stack ramp

  predictable_spike_events:
    - twilight_corruption_stack_ramp:
        damage: increasing tank melee damage as stacks accumulate
        response: HoT-roll tank; pop Mana Tide ~50% boss HP if available
    - crepuscular_veil_on_random:
        damage: shadow flavor on debuffed player
        response: |
          REACTIVE HEAL the affected player. DO NOT DISPEL the debuff.

  cooldown_use:
    mana_tide_totem: ~50% boss HP if not used on Karsh
    innervate: self-cast at ~30% mana
    spirit_link_totem: deploy if multiple players take Veil + cleave damage simultaneously

  dispel_priority:
    DISPEL_BLACKLIST:
      - { spell_id: 75476, name: Crepuscular Veil, reason: Obsidius swap mechanic }
    notes: |
      CRITICAL — bot must NOT auto-dispel Magic during this encounter.
      Add fight-level dispel-blacklist for Crepuscular Veil. Do NOT remove
      this entry on heroic (Disagreement #9 unresolved; default conservative
      = never dispel).

  interrupt_role:
    none

  heroic_delta: |
    - More frequent Crepuscular Veil applications
    - More tank pressure from Twilight Corruption
    - Some sources claim heroic dispel-window — bot still does NOT dispel
      (conservative default; see survey Disagreement #9)

  bot_anti_patterns:
    - DO NOT auto-dispel Crepuscular Veil (75476) — breaks the swap mechanic
    - DO NOT chase Veil-affected player with overheals (they're moving out of group; let them position first)
```

---

## Bot AI hooks

```yaml
ai_hooks:
  rom_ogg:
    - encounter_detect_chains_of_woe → pre_cast_aoe_heal_cd
    - encounter_detect_skullcracker_channel → reactive_aoe_heal

  corla:
    - encounter_detect_active_beam_intercepter(target) → apply_hot_tier_3
    - encounter_state_drakonids_alive → conserve_mana

  karsh:
    - encounter_state_kite_pass_imminent → pre_hot_tank
    - encounter_state_kite_pass_active → reactive_heal_tank
    - encounter_detect_self_mana_below_50 → mana_tide / innervate
    - encounter_detect_cinderbreath_landed → reactive_aoe_heal

  obsidius:
    - encounter_detect_crepuscular_veil_applied(target) → reactive_heal target NOT dispel
    - encounter_detect_twilight_corruption_high_stack → pop_party_cd
    - encounter_detect_clone_rotation_event → re-target tank for healing focus

dispel_blacklist:
  - { spell_id: 75476, name: Crepuscular Veil, scope: obsidius_fight, force: true }

mana_cd_save_priority:
  rom_ogg:    [no_save]
  corla:      [no_save]
  karsh:      [Mana Tide / Innervate / Hymn of Hope at 50% boss HP]
  obsidius:   [secondary CD if not used on Karsh]
```

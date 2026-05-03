# Well of Eternity — Healer

**Healer-specific response logic** for the bot's encounter handling. Mechanics, spell IDs, phase triggers, and add abilities are defined in `encounter.md`; this doc only encodes how the healer reacts. Class-agnostic where possible.

**Layering**:
- Cross-content healer patterns (triage hierarchy, dispel taxonomy, mana pacing, anti-patterns) live in `docs/roles/healer.md`
- Per-spec rotation / CD / mana model lives in the active healer's `docs/specs/` doc (resto-shaman, holy-paladin, holy-priest, disc-priest, resto-druid)
- This doc layers WoE-specific deviations on top of those

---

## Dungeon-level overview

```yaml
dungeon_healer_overview:
  total_bosses: 3 (heroic-only)
  estimated_full_clear_minutes: 25-40
  mana_profile:
    perotharn: moderate                       # Fel Decay heal-decision + Corrupting Touch tank ramp
    queen_azshara: high                       # long fight (~3 min), sustained AoE, MC events
    mannoroth: high                           # very long fight, multiple phases, healer-pressure-heavy phase 2
  drink_windows:
    - after Peroth'arn
    - after Queen Azshara                     # MOST IMPORTANT — go into Mannoroth full mana
  key_responsibilities:
    - dispel_blacklist_per_fight              # CRITICAL: never dispel Fel Decay
    - heal_or_not_heal_decision               # Fel Decay punishes healing
    - phase_2_hide_state_perotharn            # do NOT cast during stealth phase
    - total_obedience_kick_assist             # interrupt rotation participation if class has kick
    - mana_pacing_for_long_fights             # Azshara + Mannoroth are 3+ min each
    - moonbeam_stack_awareness                # Tyrande Moonbeam = free top-off + buff
    - npc_ally_blacklist                      # do NOT heal allied NPCs (Illidan/Tyrande/Malfurion)
```

---

## Trash — healer notes

```yaml
trash_healer:
  illidan_stealth_section:
    notes: |
      DO NOT CAST during Shadowcloak escort. Casting breaks party stealth
      and pulls trash. Bot healer holds GCDs while following Illidan.

  crystal_packs:
    dispels: []                                # no dispels needed
    notes: standard tank-heal flow on small packs (satyrs / shivarras / felguards)

  highborne_packs:
    notes: |
      Astromancers Frostbolt-cast tank/random; reactive heal flow.
      Watch for fear-cast Highborne (interrupt role for DPS); standard
      tank-heal flow.

  shores_demon_trash:
    notes: |
      Felhound / Felguard / Infernal preview of final-boss waves; tank takes
      cleave damage; standard flow.

  ally_npc_healing:
    BLACKLIST: |
      Bot healer logic must NOT heal Illidan / Tyrande / Malfurion (allied
      NPCs). They are scripted to not require heals; healer mana wasted.
      Exception: Tyrande in Mannoroth phase 2 — she is incapacitated by
      Dreadlord Debilitators; she's UNHEALABLE during stun (Moonbeam
      auto-resolves). Still no heal allocation.
```

---

## Boss 1 — Peroth'arn

```yaml
boss_healer:
  boss: perotharn
  difficulty_grade: medium
  mana_intensity: moderate

  threshold_overrides:
    tier_2_tank_emergency: tank.hp_pct < 60   # raised — Corrupting Touch stack ramp + Fel Flames splash
    tier_3_fel_decay_heal_decision: |
      Fel Decay'd target: heal ONLY if target.hp_pct < 50.
      Healing the target damages the healer (caster). Trade-off: small ticks
      pass on HoT auto-rolls (acceptable); large drops require heal (accept
      caster-damage cost).

  predictable_spike_events:
    - corrupting_touch_tank_ramp:
        cadence: continuous melee on tank
        damage: stacking debuff +10% damage per stack (20s duration)
        response: |
          HoT-roll tank from pull. Pre-cast tank's primary HoT every 15s to
          stay ahead of stack ramp. Reactive heals on stack 5+.
    - fel_flames_random_target:
        damage: ~33-37k initial fire hit on random
        response: reactive heal hit target; pre-shield if predictable
    - fel_decay_random_target:
        damage: ~38-42k initial Shadow + ~8-9k/s for 10s
        response: |
          HEAL DECISION: if target.hp_pct < 50 → heal (accept caster damage).
          If target.hp_pct >= 50 → let HoTs handle the tick; do NOT direct-heal.
          NEVER auto-cleanse — Shadow dispel = no.

  phase_2_stealth_phase:
    behavior: |
      HIDE BEHIND PILLAR. DO NOT CAST. Casting breaks Shadowcloak and may
      attract Eyes (UNVERIFIED whether casting visibility = detection;
      conservative default = no casts).
      Pre-HoT tank BEFORE Drain Essence cast (when boss reaches ~70% HP) so
      tank survives phase transition.
    cooldown_use: |
      Save major CDs for phase 3 (Endless Frenzy = +25% damage taken if
      detection occurred; Enfeebled = +25% damage to boss if successful).

  cooldown_use:
    standard_phase_1: HoT-roll tank; reactive heals on Fel Flames + Fel Decay
    phase_3_endless_frenzy: |
      If party FAILED phase 2 (detected): boss is enraged; pop major CD
      (Spirit Link Totem / Power Word: Barrier / Hand of Sacrifice / Tranquility).
    phase_3_enfeebled: |
      If party SUCCEEDED phase 2: boss is weakened; standard healing — fight
      ends fast. No major CD needed.

  dispel_priority:
    DISPEL_BLACKLIST:
      - { spell_id: 105544, name: Fel Decay, reason: heal-vs-decay IS the mechanic }
    notes: |
      Bot must NOT auto-cleanse Fel Decay even though it's Shadow-school
      (Priest Dispel Magic / Druid Cleanse / Pally Cleanse can technically
      target it). The HEAL-OR-NOT-HEAL decision IS the encounter design.

  interrupt_role:
    drain_essence: NO — Illidan handles. Do NOT waste kick CD.
    fel_flames: NO — instant cast, can't kick.
    fel_decay: NO — instant cast, can't kick.

  heroic_delta: |
    Heroic-only encounter; numbers above ARE heroic. No normal-mode delta.

  bot_anti_patterns:
    - DO NOT auto-cleanse Fel Decay (DISPEL_BLACKLIST)
    - DO NOT cast during phase 2 stealth (breaks hide)
    - DO NOT heal allied NPCs (Illidan)
    - DO NOT use kick on Drain Essence (Illidan's job)
    - DO NOT panic-overheal Fel Decay'd target (caster takes damage)
```

---

## Boss 2 — Queen Azshara

```yaml
boss_healer:
  boss: queen_azshara
  difficulty_grade: hard
  mana_intensity: high
  notes: |
    LONG fight (~3 min). Sustained AoE damage from magus rotation. Total
    Obedience interrupt is the survival check — if class has a kick, bot
    healer participates in rotation. Mana pacing is the secondary skill check.

  threshold_overrides:
    tier_2_tank_emergency: tank.hp_pct < 50
    tier_3_mc_target: |
      Hand-of-the-Queen MC'd player can't be healed effectively. Skip heal
      slot for MC'd target until puppet strings die.

  predictable_spike_events:
    - total_obedience_pre_cast:
        warning: 8s cast bar from Azshara
        damage: 0 if interrupted; WIPE if not
        response: |
          PARTICIPATE in interrupt rotation if class has kick:
          - Resto Shaman: Wind Shear (6s CD; off-GCD) — best in the dungeon
          - Holy Paladin: Rebuke (15s CD; off-GCD)
          - Holy Priest: Silence-via-Disc (off-spec only); usually no
          - Disc Priest: Silence (45s CD)
          - Resto Druid: Solar Beam (60s CD; talented from Balance — usually no)
          NEVER let Total Obedience finish without the rotation having tried.
    - magi_aoe_rotation:
        damage: continuous AoE from active magi (Coldflame line / Firebomb / Arcane Bomb)
        response: |
          Random AoE pulses on positioning failures. Reactive AoE heal cooldowns:
          - Resto Shaman: Healing Stream + Healing Rain on stack
          - Holy Paladin: Holy Radiance on stack
          - Holy Priest: Circle of Healing + PoH
          - Disc Priest: PoM bouncing + Atonement-Smite if mana stable
          - Resto Druid: Wild Growth on stack
    - hand_of_queen_mc:
        damage: MC'd player drops out of rotation
        response: |
          Switch focus to ALIVE players; do NOT heal MC'd target. Healer
          DPS-pressures puppet strings if class has DPS abilities (Resto
          Shaman LB; Holy Pally Holy Shock; etc.).

  cooldown_use:
    mana_tide_totem: |
      Deploy ~1:30 into fight (mid-fight mana check). Resto Shaman.
    innervate: self-cast at ~50% mana. Resto Druid.
    hymn_of_hope: |
      Holy Priest combat-mana-tide; ~50% mana. Cast outside MC events.
    spirit_link_totem: |
      If multiple players take Magus AoE simultaneously, deploy. Conservative.
    avenging_wrath_holy_pally: |
      Standard rotational on Holy Pally; align with high-pressure window.
    tranquility: emergency raid heal if AoE pulses compound; rare on Azshara

  dispel_priority: []                          # no dispels

  interrupt_role:
    total_obedience: |
      HIGHEST PRIORITY in dungeon. Bot interrupt rotation reserves at least
      one kick for it. Resto Shaman Wind Shear is best (6s CD = guaranteed
      coverage every cast). Holy Pally Rebuke is also short-CD.
    fireball_fire_magus: |
      Secondary interrupt (healer mana mitigation); rotate kicks if available.

  heroic_delta: |
    Heroic-only encounter; numbers above ARE heroic.

  bot_anti_patterns:
    - DO NOT skip Total Obedience interrupt (wipe if missed)
    - DO NOT heal Azshara's MC'd target (waste; switch focus)
    - DO NOT stand in magus AoEs (Coldflame line / Firebomb / Arcane Bomb)
    - DO NOT burn Mana Tide / Innervate too early (long fight)
    - DO NOT target Azshara herself (Shroud of Luminosity = invuln)
```

---

## Boss 3 — Mannoroth and Varo'then

```yaml
boss_healer:
  boss: mannoroth_and_varothen
  difficulty_grade: hard
  mana_intensity: high
  notes: |
    LONG multi-phase fight. Phase 1: standard tank-heal on Varo'then engagement
    + Magistrike arc cleave + Fel Firestorm patches. Phase 2: peak healer
    pressure — demon waves + Magistrike Arcs proc damage + Tyrande rescue.
    Phase 3: Gift of Sargeras AUTO-HEALS — healer pressure DROPS; switch to
    DPS-assist mode.

  threshold_overrides:
    tier_2_tank_emergency: tank.hp_pct < 50    # demon-wave cleave on tank
    tier_3_dreadlord_event: |
      During Dreadlord Debilitator rescue, Tyrande is stunned and the
      Moonbeam doesn't appear yet. Demon waves continue. Pre-CD this window.

  predictable_spike_events:
    - magistrike_arc_phase_1:
        damage: ~28-32k Fire to target + arc to nearby (3 total)
        response: pre-shield random tank-adjacent target; reactive heal post-arc
    - fel_firestorm_phase_all:
        damage: ~19-20k Fire/sec on patch contact
        response: |
          CONTINUOUS movement. Players hit by patches = bad positioning.
          Reactive heal on hit; encourage spread + relocation.
    - demon_wave_aoe_phase_2:
        cadence: 75% / 60% / 45% / 30% Mannoroth HP
        damage: |
          Felhounds: tank cleave damage
          Felguards: tank + group cleave
          Doomguards: group ranged damage
          Infernals: heavy AoE damage on whole group
        response: |
          AoE heal cooldowns staggered with wave triggers.
          Worst wave: Infernals (30%) — pop Spirit Link / PWB / Tranquility.
    - dreadlord_debilitator_event:
        warning: Tyrande HP drops to 0 / stun visual
        damage: continuous demon-wave damage during rescue
        response: pre-CD before bot DPS rescues Tyrande
    - tyrande_moonbeam:
        effect: ground area buff + LIGHT HP regen
        response: STACK ON MOONBEAM — free top-off + party damage buff
    - phase_3_gift_of_sargeras:
        effect: 20% HP regen / 2s + 30k AoE Fire dmg / 2s
        response: |
          Healer pressure DROPS. Switch to DPS-assist mode (Atonement-Smite,
          Lightning Bolt, Holy Shock-on-Crusader-Wrath, Wrath, etc.).
          Standard tank-heal still flows; HP-recovery is auto.

  cooldown_use:
    phase_1_standard: tank-heal flow; rotational heals
    phase_2_demon_waves: |
      Major CDs scheduled with wave triggers:
        Felhounds (75%): no major CD (manageable)
        Felguards (60%): rotational AoE CD (Healing Rain / Holy Radiance / etc.)
        Doomguards (45%): rotational AoE CD; Mana Tide / Innervate window
        Infernals (30%): MAJOR CD — Spirit Link / Power Word: Barrier / Tranquility / Divine Hymn
    phase_2_dreadlord_rescue: pre-CD before bot DPS rescues Tyrande
    phase_3_gift_of_sargeras: ENABLE DPS-MODE; conserve mana for finish
    bloodlust_alignment: heroism/bloodlust per group call (typically pre-pull)

  dispel_priority: []                          # no dispels needed

  interrupt_role:
    doomguard_shadow_bolt_phase_2: PARTICIPATE if class has kick
    other: opportunistic only

  heroic_delta: |
    Heroic-only encounter; numbers above ARE heroic.

  bot_anti_patterns:
    - DO NOT heal Illidan / Tyrande / Malfurion (allied NPC blacklist)
    - DO NOT stand in Fel Firestorm patches (continuous movement required)
    - DO NOT skip Moonbeam stack (free heal + damage buff)
    - DO NOT panic-CD on phase 1 (save for phase 2 demon waves)
    - DO NOT burn mana on phase 3 (Gift of Sargeras auto-heals)
    - DO NOT taunt or attempt to tank Mannoroth (Illidan's job)
```

---

## Bot AI hooks

```yaml
ai_hooks:
  perotharn:
    - encounter_detect_corrupting_touch_stacks(target=tank, stacks>=5) → pre_cd_tank_defensive_heal
    - encounter_detect_fel_flames_target(self) → reactive_heal_self_or_target
    - encounter_detect_fel_decay_target(target) → conditional_heal: if target.hp_pct < 50 then heal else skip
    - encounter_detect_drain_essence_cast → phase_2_pre_hot_tank
    - encounter_state_phase_2_active → enter_hide_state + suspend_casts
    - encounter_detect_phase_3_endless_frenzy → pop_major_party_cd
    - encounter_detect_phase_3_enfeebled → standard_heal_finish

  queen_azshara:
    - encounter_detect_total_obedience_cast → kick_rotation_participate (PRIORITY: HIGHEST)
    - encounter_detect_fireball_fire_magus → kick_rotation_participate (PRIORITY: high)
    - encounter_detect_hand_of_queen_mc(target=other) → suspend_heal_for_target until_strings_die
    - encounter_detect_magus_aoe_pulse → reactive_aoe_heal_cd
    - encounter_state_self_mana_below_60 → mana_tide / innervate / hymn_of_hope

  mannoroth:
    - encounter_detect_magistrike_charge(varothen, phase=1) → pre_shield_arc_targets
    - encounter_detect_fel_firestorm_patch_landed → reactive_heal_hit_targets
    - encounter_detect_demon_wave(wave=infernal) → pop_major_cd (Spirit Link / PWB / Tranquility)
    - encounter_detect_dreadlord_debilitator_event → pre_cd_phase_2_continued
    - encounter_detect_tyrande_moonbeam → stack_self_on_moonbeam
    - encounter_detect_gift_of_sargeras_buff → enter_dps_assist_mode

dispel_blacklist:
  - { spell_id: 105544, name: Fel Decay (Peroth'arn), scope: perotharn_fight, force: true }

ally_npc_heal_blacklist:
  - illidan_stormrage
  - tyrande_whisperwind
  - malfurion_stormrage

mana_cd_save_priority:
  perotharn:        [tank_corrupting_touch_ramp, no_major_cd_unless_endless_frenzy]
  queen_azshara:    [Mana Tide / Innervate / Hymn at ~1:30 mark]
  mannoroth:        [phase 2 Infernal wave; phase 2 Dreadlord rescue]

phase_specific_states:
  perotharn_phase_2_hide:
    cast_inhibit: true
    movement: pathfind_to_pillar_los_clear_of_eyes
    duration: 40s OR phase_3_signal
  azshara_mc_event:
    target_switch: from_tank_heal_to_alive_party
    skip_target: mc_d_player
  mannoroth_phase_3_gift_of_sargeras:
    mode: dps_assist
    healer_pressure: dropped
```

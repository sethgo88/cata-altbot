# Zul'Gurub — Healer

**Healer-specific response logic** for the bot's encounter handling. Mechanics, spell IDs, phase triggers, and add abilities are defined in `encounter.md`; this doc only encodes how the healer reacts. Class-agnostic where possible.

**Layering**:
- Cross-content healer patterns (triage hierarchy, dispel taxonomy, mana pacing, anti-patterns) live in `docs/roles/healer.md`
- Per-spec rotation / CD / mana model lives in the active healer's `docs/specs/` doc (resto-shaman, holy-paladin, holy-priest, disc-priest, resto-druid)
- This doc layers ZG-specific deviations on top of those

---

## Dungeon-level overview

```yaml
dungeon_healer_overview:
  total_bosses: 5 (+ 1 optional Cache of Madness, default skip)
  estimated_full_clear_minutes: 45-60 (launch-tier) / 25-35 (geared)
  difficulty_tier: HEROIC_ONLY                # no normal mode
  mana_profile:
    venoxis: high                              # 3 phases of sustained pressure
    mandokir: moderate                         # spike pressure (Decapitate revives, Bloodletting recoveries)
    kilnara: high                              # panther multi-pull + Phase 1 AoE if Tears unkicked
    zanzil: moderate                           # cauldron-timing-driven; Graveyard Gas spikes
    jindo: very_high                           # both phases sustained; Twisted Spirit enrage in Phase 2

  drink_windows:
    - after Venoxis                            # critical; long fight
    - after Mandokir (if not bypassed via skip rule)
    - after Kilnara (if not bypassed via skip rule)
    - after Zanzil (if not bypassed via skip rule)
    - after final mid-boss before Jin'do       # MOST IMPORTANT — Jin'do is mana-intense

  key_responsibilities:
    - interrupt_kick_rotation_participation (Whispers of Hethiss, Tears of Blood)
    - bloodletting_heal_suppress (Mandokir)
    - panther_clear_aoe_pacing (Kilnara)
    - cauldron_timing (Zanzil — drink Toxic Torment on Green)
    - deadzone_in_out_positioning (Jin'do Phase 1)
    - parallel_zone_phase_2 (Jin'do — implementation-flagged)
```

---

## Trash — healer notes

```yaml
trash_healer:
  voodoo_acolytes:
    dispels: []                                # no significant dispel calls
    notes: ranged Shadow Bolt damage; reactive triage on hit target

  razzashi_raptors:
    notes: melee_cleave on tank; standard tank-heal flow

  bethekk_acolytes:
    notes: Shadow caster damage on random; reactive triage

  zanzili_pre_packs:
    notes: |
      Tank uses Burning Blood cauldron for AoE clear (first pack) and
      Frostburn for Rabid Gurubashi (second pack). Healer is reactive only;
      cauldron-driven encounter is tank-led.

  gurubashi_spirit_warriors_pre_jindo:
    notes: pre-fight kill; brief tank-heal flow
```

---

## Boss 1 — High Priest Venoxis

```yaml
boss_healer:
  boss: high_priest_venoxis
  difficulty_grade: hard
  mana_intensity: high

  threshold_overrides:
    tier_2_tank_emergency: tank.hp_pct < 50    # raised — Phase 2 +50% physical damage on snake form
    tier_3_whispers_target: |
      Whoever takes Whispers of Hethiss (96466) is a soft tank for the
      channel duration (8s of 1850-2150 nature dmg every 0.3s if unkicked).
      Treat as HoT-rolling target.

  predictable_spike_events:
    - whispers_of_hethiss_on_random:
        warning: 2s windup before channel begins
        damage: 1850-2150 nature dmg every 0.3s for 8s if not kicked
        response: |
          PRIORITY KICK PARTICIPATION first — if class has interrupt
          (Wind Shear, Rebuke, etc.), kick. If kick on CD, reactive heal
          + HoT on target.
    - toxic_link_break:
        warning: scripted aura on 2 players paired
        damage: explosion if linked players break in cluster (~10000 dmg in 8y)
        response: pre-shield linked players; verify they walk apart safely
    - phase_2_acid_pool_at_furthest:
        warning: pool spawns at furthest player
        damage: 4712-5287 nature dmg per 0.5s if standing in
        response: pre-HoT the targeted ranged; reactive on splash-damaged
    - phase_2_breath_cone:
        damage: 15000 nature dmg every 0.5s for 3s in cone
        response: AoE heal if cone clipped multiple players (positional fail)
    - phase_3_bloodvenom_tendrils:
        damage: tendrils chase fixated players; AoE on contact
        response: HoT-roll fixated players; AoE-heal on tendril impacts
    - phase_3_venom_withdrawal:
        notes: BURN WINDOW — boss takes +100% damage; healer can support DPS briefly

  cooldown_use:
    standard: reactive HoT on tank + Whispers target
    aoe_window: Tears of Blood radius hit (Phase 1 if cone clipped)
    save_for_phase_3: |
      Hold major raid CD (Tranquility / Spirit Link / PW: Barrier / Divine
      Hymn) for Phase 3 — Venom Withdrawal burst window doubles as healer
      pressure spike (boss melee + tendrils + altar floor poison).

  dispel_priority: []                          # no dispels required

  interrupt_role:
    whispers_of_hethiss: |
      PARTICIPATE in interrupt rotation if class has kick:
      - Resto Shaman: Wind Shear (6s CD; 25y)
      - Holy Paladin: Rebuke (15s CD; melee — usually too far)
      - Holy/Disc Priest: Silence (45s CD; 30y; Disc only via Reflective Shield? UNVERIFIED)
      - Resto Druid: Skull Bash (60s CD; bear/cat — not in healer form)

  heroic_delta: |
    Heroic-only fight.

  bot_anti_patterns:
    - DO NOT panic-heal Toxic Linked players (let them spread first)
    - DO NOT stand in poison maze (Phase 1) or acid pool (Phase 2)
    - DO NOT burn major CD before Phase 3 — Venom Withdrawal is the spike
    - DO NOT stand at extreme range in Phase 2 (you become acid-pool target)
```

---

## Boss 2 — Bloodlord Mandokir

```yaml
boss_healer:
  boss: bloodlord_mandokir
  difficulty_grade: medium-hard
  mana_intensity: moderate

  threshold_overrides:
    tier_2_tank_emergency: tank.hp_pct < 50    # raised — Frenzy phase melee
    tier_3_revived_player: |
      Revived player has Spirit's Vengeance buff (+25% dmg/healing/-10% dmg
      taken). NOT a healer-pressure target by default; standard triage.

  predictable_spike_events:
    - decapitate_on_random:
        damage: instant death on target unless they pop immunity
        response: |
          DO NOT pre-emptively heal target — the kill is instant and
          immune-or-die. Healer focus is on the post-revive support:
          - Spirit's Vengeance buff means revived player is healthy on
            spawn; minimal heal needed
          - Watch for stacking deaths (rare, but possible) → AoE heal at
            the revival cluster
    - bloodletting_dot:
        damage: 50% current HP per tick for 5 ticks (10s)
        response: |
          HEAL-SUPPRESS this debuff. DO NOT spam-heal during.
          - Overhealing during Bloodletting is wasted (boss heals 50% of
            damage dealt)
          - Healing target ONLY if HP < 20% (life-saving exception)
          - After expiry: ONE big heal to restore (Greater Healing Wave /
            Holy Light / Healing Touch / etc.)
    - devastating_slam_furrow:
        damage: 190-200k physical in 45° cone
        response: AoE heal if multiple players clipped (position fail)
    - frenzy_phase_melee:
        damage: doubled tank attack speed at 20% HP
        response: |
          HoT-roll tank; pop tank-defensive helper (Pain Suppression /
          Lay on Hands / Guardian Spirit / Power Word: Barrier).

  cooldown_use:
    pain_suppression / hand_of_protection: at Frenzy phase if tank HP drops
    mana_tide / hymn_of_hope / innervate: ~50% boss HP if mana low
    spirit_link_totem: optional — Spirit's Vengeance HP-equalize is a unique use case (UNVERIFIED interaction)

  dispel_priority: []                          # Bloodletting NOT dispelled

  interrupt_role:
    none: Devastating Slam non-interruptible (UNVERIFIED)

  heroic_delta: |
    Heroic-only fight.

  bot_anti_patterns:
    - DO NOT spam-heal Bloodletting target (trains the boss heal)
    - DO NOT panic on Decapitate — it's instant; immunity or revive
    - DO NOT dispel Bloodletting (some sources say Magic; standard strat = leave)
    - DO NOT heal Ohgan (raptor pet); DPS killing it = group success signal
```

---

## Boss 3 — High Priestess Kilnara

```yaml
boss_healer:
  boss: high_priestess_kilnara
  difficulty_grade: hard
  mana_intensity: high

  threshold_overrides:
    tier_2_tank_emergency: tank.hp_pct < 50    # panther multi-pull + Phase 2 leap bleeds
    tier_4_panther_aoe_cluster: |
      During panther pre-clear, group cluster takes leap-bleed damage.
      Tier 4 AoE cluster threshold: party.injured_count(radius=10, hp_pct_lt=80) >= 3

  predictable_spike_events:
    - tears_of_blood_unkicked:
        damage: 8500-11500 shadow dmg every tick within 12y
        response: |
          PRIORITY KICK PARTICIPATION first; if missed, AoE heal in 12y
          radius — only melee + tank are typically clipped (ranged > 12y).
    - lash_of_anguish_dot:
        damage: 18500-21500/sec for 10s
        response: |
          DISPEL if class has Magic dispel AND target hp_pct < 50.
          Otherwise reactive heal + HoT-roll target.
          DISPEL-DISPOSITION CONSERVATIVE — see Disagreement #4.
    - wail_of_sorrow_on_random:
        damage: 18500-21500 shadow dmg ranged hit
        response: reactive heal target
    - phase_2_panther_leap_chains:
        damage: Gaping Wound bleeds (9250-10750/2s) on leap targets
        response: HoT-roll leap targets; AoE-heal if 2+ players bleeding
    - phase_2_avatar_haste_melee:
        damage: increased tank melee
        response: HoT-roll tank; pop defensive

  cooldown_use:
    panther_clear_aoe: AoE heal CD on largest cluster (Tranquility / Divine Hymn / Spirit Link)
    phase_2_transition: hold one CD for transition damage
    mana_tide / innervate / hymn_of_hope: ~50% Kilnara HP

  dispel_priority:
    - { spell_id: 96958, name: Lash of Anguish, condition: target_hp_pct < 50 AND class_has_magic_dispel }

  interrupt_role:
    tears_of_blood: |
      PRIMARY — participate in kick rotation. Resto Shaman Wind Shear is
      the canonical healer-kick. Holy/Disc Priest Silence works.
    shadow_bolt: SECONDARY — kick if rotation has slack

  heroic_delta: |
    Heroic-only fight.

  bot_anti_patterns:
    - DO NOT auto-cleanse Lash of Anguish unless class actually has Magic dispel (HoLight Pally, Disc Priest, Holy Priest, Resto Druid via Nature's Cure-talent UNVERIFIED, Shaman via Cleansing Waters? UNVERIFIED)
    - DO NOT push Kilnara to 50% with panthers asleep (un-healable Phase 2)
    - DO NOT stand within 12y of Kilnara during Tears of Blood radius
    - DO NOT heal Pride of Bethekk panthers (sleeping or awake — they are hostile)
```

---

## Boss 4 — Zanzil

```yaml
boss_healer:
  boss: zanzil
  difficulty_grade: medium
  mana_intensity: moderate

  threshold_overrides:
    tier_4_graveyard_gas_aoe: |
      Green elixir → arena-wide nature DoT (5-10% max HP/sec). With Toxic
      Torment buff (-90% nature damage) the tick is manageable; without it
      everyone dies fast.

  predictable_spike_events:
    - voodoo_bolt_on_random:
        damage: 33250-36750 shadow dmg
        response: |
          PRIORITY KICK PARTICIPATION; reactive heal target if missed.
    - terrible_tonic_on_random:
        damage: 30000 nature dmg
        response: reactive heal
    - zanzili_fire_line:
        damage: 66000 + 50000/s for 5s in line
        response: heal anyone clipped (positional fail)
    - blue_elixir_berserker_fixate:
        damage: Berserker melee + Thunderclap (12750-17250) on fixated
        response: HoT-roll fixated player; reactive
    - red_elixir_zombies:
        damage: cluster melee on tank/group
        response: AoE heal during burn
    - green_elixir_graveyard_gas:
        damage: arena-wide nature DoT
        response: |
          DRINK TOXIC TORMENT CAULDRON for self -90% nature dmg.
          Then sustain group heals with Tier 4 AoE rotation.
          bot_implementation FLAG — cauldron interaction.

  cooldown_use:
    standard: reactive only; cauldron-driven encounter
    save_for_jindo: hold major CDs for final boss

  dispel_priority: []                          # no dispels

  interrupt_role:
    voodoo_bolt: PRIMARY — participate in kick rotation

  heroic_delta: |
    Heroic-only fight.

  bot_anti_patterns:
    - DO NOT skip Toxic Torment cauldron drink on Green elixir (group OOM/dies)
    - DO NOT drink Frostburn (melee buff; healer doesn't auto-attack the right target)
    - DO NOT drink Burning Blood (melee/tank cluster anchor; healer is at range)
    - DO NOT panic-heal during Graveyard Gas if everyone has Toxic Torment (mitigation makes it survivable)

  bot_implementation_flag: |
    Cauldron is a clickable GameObject. Healer bot must be able to:
      - Pathfind to Toxic Torment cauldron on Green elixir cast
      - Use the cauldron object
      - Resume healing after acquiring buff
    Without this, Green elixir is a wipe.
```

---

## Boss 5 — Jin'do the Godbreaker

```yaml
boss_healer:
  boss: jin_do_the_godbreaker
  difficulty_grade: very_hard
  mana_intensity: very_high

  threshold_overrides:
    tier_2_tank_emergency: tank.hp_pct < 55
    tier_4_aoe_cluster: |
      Phase 2 Twisted Spirit waves AND Body Slam impacts cause group-wide
      pressure. Tier 4 threshold tightened: party.injured_count(radius=12,
      hp_pct_lt=85) >= 3

  predictable_spike_events:
    - phase_1_shadows_of_hakkar_cast:
        warning: 3s cast time
        damage: 121-128k shadow per hit (un-mitigated; chain on up to 10 targets)
        response: |
          MOVE INTO active Deadzone before cast resolves (-90% magic dmg).
          Pre-shield group; reactive AoE heal on tick.
          ENCOUNTER-SPECIFIC OVERRIDE — normally avoid ground AoE; here
          you stand in.
    - phase_1_outside_deadzone_during_cast:
        damage: full Shadows of Hakkar dmg on whoever is outside
        response: emergency tier-1 heal; this is a positioning fail

    - phase_2_transition:
        warning: hp_pct == 70
        notes: |
          ENTIRE PARTY teleported to Spirit Realm (parallel zone).
          bot_implementation FLAG — bot session must handle zone-state
          change. Pre-cast HoTs do NOT carry over (UNVERIFIED — buff
          persistence across phase change).

    - phase_2_body_slam_on_player:
        damage: leap damage + knockback + Sunder Rift debuff (+100% dmg taken)
        response: |
          Reactive heal on hit player; HoT-roll if Sunder Rift on player
          for the 20s duration (vulnerable to all damage).
    - phase_2_shadow_spike_random:
        damage: 63-77k shadow dmg in 4y radius on impact
        response: AoE heal cluster if multiple clipped
    - phase_2_twisted_spirit_waves:
        damage: stacking AoE pressure on whole group
        response: |
          Sustained AoE healing. Twisted Spirits are SOFT ENRAGE — if DPS
          can't keep up with waves, healer mana drains fast.

  cooldown_use:
    phase_1: |
      Drink phase pre-pull. Reactive heals during; ONE major CD on first
      Shadows of Hakkar cast (in case Deadzone positioning is sloppy).
    phase_2_entry: |
      Major CD on Spirit Realm entry — initial Twisted Spirit wave + first
      Body Slam often comes immediately.
    phase_2_chain_breaks: rotate AoE CDs on each Body Slam impact / chain break sequence
    final_phase: hold one CD for return-to-physical-realm finish (rare; usually fight ends quickly post-chains)

  dispel_priority: []                          # no dispels

  interrupt_role:
    none: Shadows of Hakkar non-interruptible (mitigated, not stopped)

  heroic_delta: |
    Heroic-only fight.

  bot_anti_patterns:
    - DO NOT stand outside Deadzone during Shadows of Hakkar (~120k death)
    - DO NOT stand inside Deadzone outside Shadows of Hakkar (90% cast speed reduction = no heals)
    - DO NOT skip cooldown rotation on Phase 2 entry (twisted spirits ramp fast)
    - DO NOT lose track of group in parallel zone (Phase 2 teleport may dissociate group references)

  bot_implementation_flag_phase2: |
    Phase 2 parallel-zone teleport. Healer bot needs:
      - Detection of zone/phase ID change at 70% Jin'do HP
      - Group reference re-resolution (party leader, tank, etc.)
      - Continuation of healing logic on the Spirit Realm side
      - Detection of return to physical realm
    Without this, Phase 2 is un-completable.

  bot_implementation_flag_phase1: |
    Deadzone in/out timing. Encounter-specific override:
      - Cast START of Shadows of Hakkar (3s) → MOVE INTO Deadzone
      - Cast END → MOVE OUT
    Normal "ground AoE = avoid" rule does NOT apply here.
```

---

## Optional Boss — Cache of Madness

```yaml
boss_healer:
  boss: cache_of_madness
  optional: true
  default_skip: true                           # Archaeology dependency
  difficulty_grade: variable (1 of 4 random spawns)
  mana_intensity: moderate

  default: SKIP unless group leader explicit + Archaeology unlocked

  generic_response:
    - kite_from_fixate: HoT-roll fixated player
    - dispel_roots (Gri'lek Entangling Roots): if class has Nature dispel
    - aoe_cluster: standard Tier 4 thresholds

  bot_anti_patterns:
    - DO NOT auto-engage if Archaeology not unlocked (group will fail key check)
```

---

## Bot AI hooks

```yaml
ai_hooks:
  venoxis:
    - encounter_detect_whispers_cast → kick_rotation_participate
    - encounter_detect_toxic_link_paired → pre_shield_paired_players
    - encounter_state_phase_3_active → reserve_major_cd_for_venom_withdrawal
    - encounter_detect_phase_3_tendril_spawn → hot_roll_fixated_targets

  mandokir:
    - encounter_detect_decapitate_telegraph → no_action (immunity is target's job)
    - encounter_detect_bloodletting_applied(target) → SUPPRESS_HEALING(target) UNTIL hp_pct < 20 OR debuff_expired
    - encounter_detect_bloodletting_expired(target) → big_heal(target)
    - encounter_state_frenzy_phase → tank_hot_roll + defensive_helper_cd

  kilnara:
    - encounter_detect_tears_of_blood_cast → kick_rotation_participate
    - encounter_detect_lash_of_anguish_applied(target) → IF class_has_magic_dispel AND target.hp_pct < 50 → dispel
    - encounter_state_panther_clear → aoe_heal_cd_pacing
    - encounter_state_phase_2_avatar → tank_hot_roll + reactive_aoe_heal

  zanzil:
    - encounter_detect_voodoo_bolt_cast → kick_rotation_participate
    - encounter_detect_green_elixir_active → DRINK_TOXIC_TORMENT_CAULDRON (bot_implementation_flag)
    - encounter_detect_zanzili_fire_line → reactive_heal_clipped_players

  jindo:
    - encounter_detect_shadows_of_hakkar_cast → MOVE_INTO_DEADZONE (bot_implementation_flag — encounter override)
    - encounter_detect_shadows_of_hakkar_resolved → MOVE_OUT_OF_DEADZONE
    - encounter_detect_phase_2_transition_70 → handle_zone_change (bot_implementation_flag — parallel zone)
    - encounter_detect_body_slam_on_player(target) → reactive_heal AND hot_roll_for_sunder_rift_duration
    - encounter_detect_twisted_spirit_wave → aoe_heal_pacing

dispel_blacklist: []                           # no fight-level blacklists in ZG

dispel_whitelist:
  - { spell_id: 96958, name: Lash of Anguish, scope: kilnara, condition: target.hp_pct < 50 AND class_has_magic_dispel }

mana_cd_save_priority:
  venoxis:    [hold major CD for Phase 3 burst window]
  mandokir:   [Pain Suppression / Lay on Hands at Frenzy phase]
  kilnara:    [AoE CD on largest panther cluster + Phase 2 transition]
  zanzil:     [reactive only; cauldron-driven]
  jindo:      [Phase 2 entry (Twisted Spirit ramp) + per chain-break cycle]
```

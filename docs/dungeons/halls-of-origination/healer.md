# Halls of Origination — Healer

**Healer-specific response logic** for the bot's encounter handling. Mechanics, spell IDs, phase triggers, and add abilities are defined in `encounter.md`; this doc only encodes how the healer reacts. Class-agnostic where possible.

**Layering**:
- Cross-content healer patterns (triage hierarchy, dispel taxonomy, mana pacing, anti-patterns) live in `docs/roles/healer.md`
- Per-spec rotation / CD / mana model lives in the active healer's `docs/specs/` doc (resto-shaman, holy-paladin, holy-priest, disc-priest, resto-druid)
- This doc layers HoO-specific deviations on top of those

---

## Dungeon-level overview

```yaml
dungeon_healer_overview:
  total_bosses: 7
  estimated_full_clear_minutes: 35-55     # LONGEST Cata 5-man
  mana_profile:
    anhuur: moderate                      # Reverberating Hymn raid-AoE during shield phase
    ptah: low_to_moderate                 # Flame Bolt + add phase
    anraphet: high                        # Vault gauntlet drains, then awakening burst, then Omega Stance
    setesh: VERY_HIGH                     # CANONICAL DUNGEON MANA-TEST — pop Mana Tide here
    ammunae: moderate                     # Wither dispels + CLE channel target
    isiset: moderate                      # Astral Rain channels
    rajh: high                            # Sun Strike raid-AoE every cycle + Blessing channel
  drink_windows:
    - top of stair landing BEFORE Vault of Lights gauntlet (mana check before Anraphet)
    - top of stair landing AFTER Anraphet, BEFORE Setesh halls (CRITICAL — Setesh is the mana-test)
    - between each Construct fight if pull cadence allows
    - before Rajh (final boss; full mana ideal)
  key_responsibilities:
    - magic_dispel_pressure                # Divine Reckoning (anhuur) + Nemesis Strike (anraphet) + Wither (ammunae)
    - mana_pacing_for_setesh_canonical_oom_test
    - cooldown_pacing_across_seven_bosses
    - face_away_on_isiset_supernova
    - raid_heal_through_unavoidable_aoe   # Anraphet awakening, Rajh Sun Strike, Reverberating Hymn
```

---

## Trash — healer notes

```yaml
trash_healer:
  pre_anhuur_corridor:
    dispels: []                             # no significant calls
    notes: standard tank healing; conservation focus

  vault_of_lights_gauntlet:
    dispels: []                             # gauntlet trash has no dispel calls
    cooldown_use: |
      Pop short healing CD on warden nova events (Flame Warden / Earth Warden).
      Stone Trogg constant-spawn drains mana over the gauntlet — pace
      efficient heals; avoid flash-heal spam.
    drink_window: TOP OF STAIR LANDING BEFORE entering chamber. NO drink time during gauntlet.
    notes: |
      4 wardens + continuous Stone Trogg trash. Tank takes cleave damage
      throughout. Healer mana-pace through to Anraphet awakening burst.

  setesh_halls_gauntlet:
    dispels: []                             # Anti-Magic Prison interrupted, not dispelled
    cooldown_use: |
      Pop major mana CD if pulled in succession (Mana Tide / Hymn of Hope /
      Innervate). This is the precursor mana drain to Setesh fight.
    drink_window: TOP OF STAIR LANDING AT START OF SETESH HALLS
    notes: |
      CANONICAL MANA-TEST PRECURSOR. Multiple small pulls drain mana before
      reaching the boss. Drink at every stair landing if possible.
      bot_implementation: SETESH_MANA_TEST flag triggers conservative
      mana-pacing mode for the entire halls + boss fight.

  construct_hall_trash:
    notes: standard tank healing between Constructs; minor cleave
```

---

## Boss 1 — Temple Guardian Anhuur

```yaml
boss_healer:
  boss: temple_guardian_anhuur
  difficulty_grade: medium                # door event coordination
  mana_intensity: moderate

  positioning:
    role: STAY TOPSIDE during shield phase (NEVER drop into pit per Disagreement #8)
    facing: free
    movement: avoid Burning Light blue ground

  predictable_spike_events:
    - reverberating_hymn_channel:
        trigger: Shield of Light cast (66% / 33%)
        warning: ~1s after shield cast
        damage: stacking AoE while shield is up; escalates over channel
        response: |
          Pop AoE heal cooldown topside on shield cast:
          - Resto Shaman: Healing Stream Totem + Healing Rain pre-drop
          - Holy Paladin: Holy Radiance + Light of Dawn on stack
          - Holy Priest: Circle of Healing + PoH spam; consider Hymn of Hope
          - Disc Priest: PoM bouncing + PW:Barrier ground-place under group
          - Resto Druid: Wild Growth + maintain Lifebloom on tank
        critical: |
          Topside group has 3 players (tank + healer + 1 DPS) plus stragglers
          — keep all alive until both beacons activate.

    - divine_reckoning_application:
        damage: 8s explode timer; 7y AoE if undispelled; HEALS BOSS 5x damage
        response: |
          PRIORITY MAGIC DISPEL on debuffed player. If dispel unavailable,
          debuffed player MOVES OUT of group; healer reactive-heals after
          detonation.

    - burning_light_ground:
        damage: 24-26k Holy/sec to anyone in 4y patch
        response: stay out; reactive heal if someone gets caught

  cooldown_use:
    standard: reactive on tank + topside group
    aoe_window: |
      Align AoE heal CD to Reverberating Hymn channel start (shield cast).
      Two predictable shield phases (66%, 33%) — split CDs accordingly.
    no_unique_cd_save: |
      Save Mana Tide / Innervate / Hymn of Hope for SETESH (mana-test fight).
      Anhuur is moderate-mana; do not burn major CDs here.

  dispel_priority:
    - { spell_id: 75592, name: Divine Reckoning, dispel_type: magic, urgency: HIGH (8s detonation timer) }
    - { spell_id: 74538, name: Pit Viper Poison-tipped Fangs, dispel_type: poison, urgency: LOW (only on lever-runner) }

  interrupt_role:
    none: |
      Healer DOES NOT interrupt anything on Anhuur. Reverberating Hymn is
      interrupt-IMMUNE during shield. Burning Light has no cast. Divine
      Reckoning has no interrupt window (instant application).

  heroic_delta: |
    - Beacon cast 8s → 10s (longer pit exposure for runners; longer Hymn
      channel for healer to survive)
    - More Pit Vipers per side (more poison DoT pressure on lever-runners)
    - Reverberating Hymn damage scales harder

  bot_anti_patterns:
    - DO NOT drop into pit on shield (healer stays topside)
    - DO NOT chase pit-runners with cross-pit heals (out of LoS / range)
    - DO NOT skip dispel on Divine Reckoning (boss heals 5x damage = wipe pressure)
    - DO NOT try to interrupt Reverberating Hymn (interrupt-immune)
```

---

## Boss 2 — Earthrager Ptah

```yaml
boss_healer:
  boss: earthrager_ptah
  difficulty_grade: easy
  mana_intensity: low_to_moderate

  positioning:
    role: free-positioning at max range
    facing: free
    movement: avoid Quicksand pools + telegraphed Earth Spikes

  predictable_spike_events:
    - flame_bolt_channel:
        trigger: 5s channel on 2 random players (3 on heroic)
        damage: continuous fire damage
        response: |
          Reactive heal Flame Bolt targets (HoT-roll preferred):
          - Resto Shaman: Riptide on each target
          - Holy Paladin: Beacon of Light on tank; reactive Flash of Light
          - Holy Priest: Renew on targets; Prayer of Mending bounce
          - Disc Priest: PW:S each target preemptively
          - Resto Druid: Rejuvenation on each target

    - tumultuous_earthstorm_phase_50:
        damage: cleave from Dustbone Horrors + scarab swarm
        response: AoE heal during burrow phase; HoT tank through cleave

  cooldown_use:
    standard: reactive
    save_for_real_bosses: hold Mana Tide / Innervate / Hymn for Setesh

  dispel_priority: []                      # no dispel calls

  interrupt_role:
    flame_bolt: PARTICIPATE in kick rotation if class has interrupt and rotation has space

  heroic_delta: |
    - Flame Bolt targets 3 (vs 2)
    - Sand vortex tornado kite during burrow phase

  bot_anti_patterns:
    - DO NOT stand in Quicksand
    - DO NOT cluster ranged (Earth Spike multi-hit)
    - DO NOT burn Mana Tide here
```

---

## Boss 3 — Vault of Lights gauntlet + Anraphet

```yaml
boss_healer:
  boss: vault_of_lights_anraphet
  difficulty_grade: hard                  # gauntlet drain + awakening burst + dispel pressure
  mana_intensity: high

  # ============================================================
  # PHASE A — Warden gauntlet
  # ============================================================
  warden_gauntlet:
    positioning: stay near tank (cleave-heal range); spread for Earth/Fire Warden novas
    cooldown_use: |
      Pop short CD on each warden's nova event:
      - Flame Warden Raging Inferno: spread + AoE heal
      - Earth Warden knockback: post-knockback spread heal
    mana_pacing: |
      Conservative healing through gauntlet. Constant Stone Trogg cleave
      damage; pace efficient heals (avoid flash-heal spam).

  # ============================================================
  # PHASE B — Anraphet (post-awakening)
  # ============================================================
  anraphet:
    positioning: max range; spread; rotate around chamber as void zones accumulate (heroic)

    predictable_spike_events:
      - awakening_unavoidable_aoe:
          trigger: 4th warden dies → Anraphet emerges
          damage: ~30-50% party HP burst
          response: |
            PRE-SHIELD entire group BEFORE 4th warden dies (or immediately
            after). Reactive AoE heal post-burst.
            - Resto Shaman: Healing Tide Totem if available
            - Holy Paladin: Aura Mastery + Holy Radiance
            - Holy Priest: Power Word: Barrier + Hymn of Hope
            - Disc Priest: PW:S group + PW:Barrier on stack
            - Resto Druid: Tranquility (raid-recovery)

      - nemesis_strike_tank:
          damage: shadow DoT on tank
          response: |
            DISPEL MAGIC on tank (CANONICAL DISPEL CALL of HoO).
            Reactive heal between dispels.
          critical: |
            If dispel missed, tank shadow DoT compounds with melee damage —
            tank HP can drop fast. Bot dispel logic must prioritize this.

      - alpha_beams_random_target:
          damage: shadow channel on random player
          response: reactive heal target; movement is target's responsibility

      - omega_stance_channel:
          damage: 8s pulsing AoE; -50% movement for 4s
          response: |
            Pop AoE heal CD:
            - Resto Shaman: Healing Rain + Chain Heal
            - Holy Paladin: Holy Radiance + Light of Dawn
            - Holy Priest: Sanctuary Chakra Circle of Healing + PoH
            - Disc Priest: Atonement-Smite for healing; PW:Barrier under stack
            - Resto Druid: Wild Growth + maintain Lifebloom

      - crumbling_ruin_stack:
          damage: max-HP reduction (10% per stack, max 10)
          response: |
            HEAL THROUGH (no dispel). At high stacks, party max-HP drops
            significantly — keep everyone topped to absolute HP, not %.
            Soft-enrage clock; DPS check tightens.

  cooldown_use:
    awakening: pre-shield all + AoE heal CD
    omega_stance: AoE heal CD per channel
    mana_relief: |
      Mana Tide Totem / Hymn of Hope / Innervate if hitting <50% mana — do
      not save these for Setesh if Anraphet drains hard. Setesh has its own
      mana relief windows.

  dispel_priority:
    - { spell_id: 75604, name: Nemesis Strike, dispel_type: magic, urgency: HIGH (continuous tank pressure) }

  interrupt_role:
    none: no kick-required casts on Anraphet himself

  heroic_delta: |
    - Persistent Alpha Beam void zones — group movement increases
    - Omega Stance hits harder
    - Crumbling Ruin stacks faster (DPS pressure → healer can't out-heal forever)
    - Stone Trogg gauntlet faster spawn rate

  bot_anti_patterns:
    - DO NOT skip Nemesis Strike dispel (tank shadow DoT compounds dangerously)
    - DO NOT stand in Alpha Beam path (ranged-and-healer movement)
    - DO NOT burn ALL major CDs on awakening (save 1 for Omega Stance later)
```

---

## Boss 4 — Setesh, Construct of Destruction

```yaml
boss_healer:
  boss: setesh
  difficulty_grade: hard
  mana_intensity: VERY_HIGH                # CANONICAL DUNGEON MANA-TEST
  notes: |
    CANONICAL MANA-TEST OF HALLS OF ORIGINATION. Constant pressure from:
    - Anti-Magic Prison casts (must be kicked, but if not the player can't be magic-healed)
    - Random Chaos Bolt damage (10-13k unresistable on random players)
    - Add cleave damage from portals
    - Boss is untankable — tank kites adds, healer raid-heals
    bot_implementation: SETESH_OOM_RISK flag — pop Mana Tide / Innervate /
    Hymn of Hope here regardless of fight progress.

  positioning:
    role: max range; mobility-friendly positioning
    facing: free
    movement: out of Reign of Chaos bubbles + Chaos Blast ground patches; INTO Seed of Chaos for damage buff (low priority for healer)

  threshold_overrides:
    tier_2_tank_emergency: tank.hp_pct < 60   # raised — tank takes melee from kited adds + occasional Chaos Bolt
    tier_3_aoe_threshold: party.injured_count(radius=10y, hp_pct_lt=70) >= 2   # lowered — multiple players take damage simultaneously

  predictable_spike_events:
    - chaos_bolt_random:
        damage: 10-13k unresistable shadow on random target
        response: reactive heal; pre-shield random rotation if DPS available

    - reign_of_chaos_bubbles:
        damage: 7.8k/sec to anyone inside
        response: reactive heal; expect target to move out

    - chaos_blast_ground:
        damage: 5s telegraph; persistent void zone after landing
        response: pre-position; reactive if caught

    - anti_magic_prison_landed:
        damage: target loses MAGIC HEALING for duration
        response: |
          Switch affected player to PHYSICAL/HOT-only healing if possible
          (Holy Paladin has no physical heal; Resto Shaman has Riptide HoT
          ticks but Magic-school; Druid HoTs are Magic).
          Most healer kits cannot effectively heal Anti-Magic Prison target.
          Conclusion: target survives via personal CDs / heal-stones / etc.
          PRIMARY DEFENSE = kick the cast (DPS responsibility, not healer).

    - portal_add_cleave:
        damage: tank takes melee from Sentinels + Wurms + Seekers
        response: continuous tank-heal; HoT-roll tank

  cooldown_use:
    mana_tide_totem: deploy at ~50% boss HP (Resto Shaman) — CRITICAL window
    innervate: self-cast at ~30% mana (Resto Druid)
    hymn_of_hope: combat-mana CD (Holy Priest); deploy at ~40% mana
    spirit_link_totem: deploy if multiple players take Chaos Bolt + cleave simultaneously (HP redistribute)
    healing_tide_totem: AoE heal CD for portal-spawn windows (Resto Shaman level 85)
    avenging_wrath: standard rotational on Holy Pally + offensive contribution
    tranquility: emergency raid recovery if HP collapses (rare on Setesh; but possible)

  dispel_priority: []                       # Anti-Magic Prison dispellability UNVERIFIED — bot does NOT attempt magic dispel

  interrupt_role:
    anti_magic_prison: |
      ABSOLUTE PRIORITY — if healer has interrupt (Resto Shaman Wind Shear,
      Holy Pally Rebuke), kick Anti-Magic Prison. The single most important
      kick in the dungeon for healer survival.

  heroic_delta: |
    - Portals don't auto-close — DPS slower on portal kills = more adds
    - Anti-Magic Prison casts more frequent
    - Mana drain pressure significantly higher
    - Bot healer must be VERY conservative on mana between casts

  bot_anti_patterns:
    - DO NOT save Mana Tide for "later" — Setesh IS the moment
    - DO NOT skip kicks on Anti-Magic Prison (if healer has kick, this is the priority)
    - DO NOT panic-flash-heal (Setesh fight is long; pace heals)
    - DO NOT try to magic-dispel Anti-Magic Prison if landed (UNVERIFIED dispellability)
    - DO NOT auto-walk into Seed of Chaos (healer DPS gain is low; mobility cost is high)
```

---

## Boss 5 — Ammunae, Construct of Life

```yaml
boss_healer:
  boss: ammunae
  difficulty_grade: medium
  mana_intensity: moderate

  positioning:
    role: max range; mobility for Spore cloud avoidance
    facing: free
    movement: out of Noxious Spore clouds (6y radius, 30s duration)

  predictable_spike_events:
    - wither_dispel_call:
        damage: -60% attack/cast/movement for 10s on random player
        response: |
          PRIORITY MAGIC DISPEL on affected player. The 60% slow effectively
          incapacitates the target; dispel ASAP.

    - consume_life_energy_channel:
        damage: 8k Nature/sec + stun on target
        response: |
          PRIMARY KICK by DPS. If kick missed:
          - HoT-roll target through channel
          - Healer cannot end the channel; only an interrupt can
          - Reactive heal stunned target

    - rampant_growth_burst:
        damage: 29-34k Nature AoE + converts pods to Bloodpetal Sprouts
        response: |
          AoE heal CD on cast:
          - Resto Shaman: Healing Rain + Chain Heal
          - Holy Paladin: Holy Radiance + Light of Dawn
          - Holy Priest: PoH on stack
          - Disc Priest: PW:Barrier ground-place
          - Resto Druid: Wild Growth + Tranquility on heroic
          Pre-empt by pre-casting pods: keep pod count low to minimize burst.

    - noxious_spores_cloud:
        damage: 10-12k Nature / 1.5s to anyone in 6y cloud
        response: target moves out; reactive heal if caught

  cooldown_use:
    standard_reactive: tank heal continuous
    rampant_growth_burst: AoE heal CD per occurrence
    save_major_cd: hold Spirit Link / Tranquility for high-pod-count Rampant Growth

  dispel_priority:
    - { spell_id: 76043, name: Wither, dispel_type: magic, urgency: HIGH (10s incapacitation) }

  interrupt_role:
    consume_life_energy: |
      ABSOLUTE PRIORITY — if healer has interrupt (Wind Shear / Rebuke /
      Skull Bash for Resto Druid), kick Consume Life Energy. Boss energy
      cap depends on this; missed kicks accelerate Rampant Growth.
    wither: SECONDARY — can interrupt OR dispel after-cast

  heroic_delta: |
    - Pods spawn faster (more pod kills required from DPS)
    - Pods may actively heal boss (UNVERIFIED) — DPS check tightens
    - Bloodpetal Sprouts hit harder

  bot_anti_patterns:
    - DO NOT skip Wither dispel (10s incapacitation can be lethal if target is tank)
    - DO NOT stand in Noxious Spore clouds
    - DO NOT skip kick on Consume Life Energy (boss energy gain is the soft-enrage timer)
```

---

## Boss 6 — Isiset, Construct of Magic

```yaml
boss_healer:
  boss: isiset
  difficulty_grade: medium
  mana_intensity: moderate

  positioning:
    role: max range; FACE AWAY on Supernova cast
    facing: AWAY from Isiset on Supernova (4s disorient if facing)
    movement: avoid Astral Rain channel ground

  predictable_spike_events:
    - supernova_face_away:
        damage: 24-26k Arcane AoE + 4s disorient on facing players
        response: |
          FACE AWAY mid-combat. Pre-cast HoT before Supernova so disoriented
          players still have rolling heals.

    - astral_rain_channel:
        damage: arcane every 3s for 8s; scales 5.4k → 6.6k → 9.0k per tick by familiar count
        response: AoE heal cooldown if Astral Rain is empowered; reactive otherwise

    - split_phase_familiar_focus:
        damage: focus-fire reduces remaining empower time
        response: HoT-roll group during split; reactive heal as familiars die

    - celestial_familiar_arcane_barrage:
        damage: 9-10k arcane per cast on random target
        response: reactive heal target; HoT preferred (multiple familiars at high empower)

  cooldown_use:
    standard_reactive: tank-heal continuous
    astral_rain_empowered: AoE heal CD if Astral Rain familiar wasn't killed first
    save_for_rajh: hold Mana Tide / Hymn for Rajh if Setesh was cleared cheap

  dispel_priority:
    - { spell_id: 74133, name: Veil of Sky, dispel_type: magic_spellsteal, urgency: MEDIUM (DPS gain) }
    notes: |
      Veil of Sky is removed by Spellsteal (Mage), Purge (Shaman), or Mass
      Dispel (Priest). Healer-priest with Mass Dispel can clear Veil of Sky
      familiar shield. Resto Shaman cannot Purge (Enhancement-only — UNVERIFIED).
      Resto Druid has no offensive dispel.

  interrupt_role:
    arcane_barrage_familiar: PARTICIPATE in kick rotation if available

  heroic_delta: |
    - All damage values higher
    - Familiar HP higher (split phase longer = more empowered ability casts)

  bot_anti_patterns:
    - DO NOT face Isiset on Supernova cast (4s disorient = no healing)
    - DO NOT stand in Astral Rain channel ground
    - DO NOT skip Spellsteal/Purge/Mass Dispel on Veil of Sky if class has it
```

---

## Boss 7 — Rajh, Construct of the Sun

```yaml
boss_healer:
  boss: rajh
  difficulty_grade: hard                  # final boss; dense kick rotation; long fight
  mana_intensity: high

  positioning:
    role: max range; mobility for Solar Winds tornado kite
    facing: free (no face-away mechanic)
    movement: kite-out Solar Winds; avoid Solar Fire patches (heroic); spread for Inferno Leap

  predictable_spike_events:
    - sun_strike_raid_aoe:
        cadence: ~every cycle (20-energy spend)
        damage: 18-21k Fire to ALL players within 50y + applies UNDISPELLABLE tank DoT
        response: |
          AoE heal cooldown per Sun Strike:
          - Resto Shaman: Healing Rain + Chain Heal
          - Holy Paladin: Holy Radiance + Light of Dawn
          - Holy Priest: PoH + Circle of Healing
          - Disc Priest: PW:S sweep + Atonement
          - Resto Druid: Wild Growth maintenance

    - sun_strike_tank_dot:
        damage: continuous undispellable fire DoT on tank
        response: HoT-roll tank; refresh frequent; pre-cast tank heals

    - inferno_leap_random:
        damage: 3s telegraph; fatal fire damage on landing if not interrupted
        response: |
          PRIMARY KICK by DPS. If lands, target is LIKELY DEAD — pre-shield
          if class allows (PW:S, Hand of Sacrifice, Earth Shield).

    - sun_orb_summon:
        damage: 13y radius AoE on landing
        response: target moves out of orb landing; reactive heal if caught

    - blessing_of_the_sun_channel:
        damage: 8s AoE channel + +100% damage buff for ALL PLAYERS
        response: |
          AoE heal CD during Blessing of the Sun:
          - All AoE heals up
          - Bot offensive-CD alignment: ALL PLAYERS (including healer for
            support DPS) gain +100% damage during this window
          - Healer can contribute support-DPS during Blessing if all alive

    - solar_winds_tornado:
        damage: 4y radius rolling fire AoE
        response: kite-out; reactive heal anyone caught

  cooldown_use:
    sun_strike_aoe: AoE heal CD per Sun Strike
    blessing_of_the_sun: AoE heal CD during Blessing channel (8s of damage)
    tank_dot_continuous: HoT-roll tank; Beacon (HPally), Riptide (Shaman), Renew (Priest), Lifebloom (Druid)
    save_emergency: Tranquility / Hymn of Hope if HP collapses across multiple Sun Strikes

  dispel_priority: []                       # Sun Strike DoT is UNDISPELLABLE

  interrupt_role:
    sun_orb: PARTICIPATE in kick rotation
    inferno_leap: PARTICIPATE — saving a random player from leap-kill is high-priority

  heroic_delta: |
    - Solar Fire (89131) persistent fire patches dropped along Solar Winds path
    - Floor fills with hazards over time
    - All damage values higher
    - Achievement window (Sun-of-a...) tightens kill timer

  bot_anti_patterns:
    - DO NOT stand in Solar Winds tornado path
    - DO NOT skip kicks on Sun Orb / Inferno Leap (both interruptible; DPS rotation may have space)
    - DO NOT try to dispel tank Sun Strike DoT (UNDISPELLABLE)
    - DO NOT save AoE heal CDs for "later" — Sun Strike is every cycle
```

---

## Bot AI hooks

```yaml
ai_hooks:
  anhuur:
    - encounter_detect_shield_of_light → STAY_TOPSIDE + AoE_heal_cd_on_reverberating_hymn
    - encounter_detect_divine_reckoning_target → magic_dispel target
    - encounter_detect_burning_light_ground → reactive_heal anyone caught
    - encounter_state_pit_runner_active → no_action (pit runners self-recover after exit)

  ptah:
    - encounter_detect_flame_bolt_channel → reactive_heal_targets + hot_roll
    - encounter_state_burrow_phase → aoe_heal_through_cleave
    - encounter_detect_quicksand_self → move_out

  vault_of_lights_anraphet:
    - encounter_state_warden_gauntlet_active → conservative_mana_pacing
    - encounter_detect_warden_nova → aoe_heal_cd
    - encounter_detect_4th_warden_dies → PRE_SHIELD_GROUP + brace for awakening burst
    - encounter_detect_anraphet_awakening_aoe → AoE_heal_cd_major
    - encounter_detect_nemesis_strike_tank → magic_dispel_high_priority
    - encounter_detect_omega_stance_channel → aoe_heal_cd
    - encounter_state_alpha_beam_target → reactive_heal_target

  setesh:
    - encounter_state_active → set_flag_SETESH_OOM_RISK
    - encounter_state_self_mana_below_50 → pop_mana_tide / innervate / hymn_of_hope
    - encounter_detect_anti_magic_prison_cast → kick_if_self_has_interrupt
    - encounter_detect_anti_magic_prison_landed_self → no_self_dispel (UNVERIFIED dispellability)
    - encounter_detect_chaos_bolt_target → reactive_heal target
    - encounter_state_portal_active → AoE_heal_cd_for_cleave_window

  ammunae:
    - encounter_detect_wither_target → magic_dispel_high_priority
    - encounter_detect_consume_life_energy_channel → kick_if_self_has_interrupt + hot_target
    - encounter_detect_rampant_growth_cast → aoe_heal_cd
    - encounter_detect_noxious_spores_self → move_out

  isiset:
    - encounter_detect_supernova_cast → FACE_AWAY (override default facing)
    - encounter_detect_astral_rain_empowered → aoe_heal_cd
    - encounter_state_split_phase → hot_roll_group
    - encounter_detect_veil_of_sky_familiar → spellsteal_purge_mass_dispel_if_class

  rajh:
    - encounter_detect_sun_strike_raid_aoe → aoe_heal_cd
    - encounter_state_active → continuous_tank_dot_hot_roll
    - encounter_detect_inferno_leap_target → preshield_target_if_class + reactive_heal
    - encounter_detect_blessing_of_the_sun_channel → AoE_heal_cd_major + support_dps_optional
    - encounter_state_solar_winds_active → mobile_positioning

dispel_blacklist: []                      # NONE — Halls of Origination has no "do not dispel" entries

mana_cd_save_priority:
  anhuur:    [no_save]
  ptah:      [no_save]
  anraphet:  [reactive_if_drained]        # Vault gauntlet may force CD use
  setesh:    [PRIMARY_BURN — Mana Tide / Innervate / Hymn of Hope ABSOLUTE]
  ammunae:   [secondary_if_setesh_handled]
  isiset:    [secondary_if_setesh_handled]
  rajh:      [secondary_if_setesh_handled — final boss; full mana ideal]
```

# Zul'Gurub — Tank

**Tank-specific response logic** for the bot's encounter handling. Mechanics, spell IDs, phase triggers, and add abilities are defined in `encounter.md`; this doc only encodes how the tank reacts. Class-agnostic where possible.

**Layering**:
- Cross-content tank patterns (threat priority, defensive CD pacing, taunt rules) live in `docs/roles/tank.md` (deferred to tank phase).
- Per-spec rotation / CD / threat model lives in the corresponding `docs/specs/` tank doc (all tank specs deferred to tank phase).
- This doc layers ZG-specific deviations on top of those.

---

## Dungeon-level overview

```yaml
dungeon_tank_overview:
  total_bosses: 5 (+ 1 optional Cache of Madness, default skip)
  estimated_full_clear_minutes: 45-60 (launch-tier) / 25-35 (geared)
  difficulty_tier: HEROIC_ONLY
  threat_profile:
    venoxis: moderate                           # multi-phase repositioning
    mandokir: moderate                          # Ohgan target swap
    kilnara: high                               # 16 panthers + boss = multi-target threat
    zanzil: moderate                            # boss reposition out of Zanzili Fire line
    jindo: high                                 # Phase 1 Deadzone management + Phase 2 Body Slam positioning
  defensive_cd_profile:
    venoxis: heavy                              # Phase 2 +50% physical damage taken from melee removed (snake form loses dodge/parry — wait, that's Venoxis taking +50% physical from us; tank receives standard melee)
    mandokir: heavy                             # Frenzy phase doubled attack speed
    kilnara: moderate                           # Phase 2 Avatar haste; bleed accumulation
    zanzil: light                               # mostly reposition + interrupt on boss
    jindo: heavy                                # Phase 1 Shadows of Hakkar (mitigated via Deadzone); Phase 2 sustained spirit damage
  key_responsibilities:
    - whispers_kick_rotation (venoxis)
    - reposition_for_phase_2_cone (venoxis)
    - lead_tendril_kite (venoxis phase 3)
    - hold_threat_through_ohgan_swap (mandokir; tank stays on boss)
    - cluster_panthers_for_aoe (kilnara pre-engage)
    - reposition_zanzil_off_zanzili_fire (zanzil)
    - cauldron_drink_burning_blood_or_toxic_torment (zanzil)
    - position_jindo_near_deadzone_zones (jindo phase 1)
    - position_gurubashi_spirit_for_body_slam_on_chain (jindo phase 2)
```

---

## Trash — tank notes

```yaml
trash_tank:
  voodoo_acolytes:
    pull_pattern: |
      Skull on lead caster (kill-priority); CC on second caster (Polymorph,
      Hex, Sap). Tank pulls melee adds + grabs casters via charge or
      Avenger's Shield/Rebuke pull.
    facing: away from group

  razzashi_raptors:
    notes: pack-pulls; frontal cleave on raptors; tank face-away

  bethekk_acolytes:
    pull_pattern: same as Voodoo Acolytes

  zanzili_pre_packs:
    first_pack_burning_blood: |
      TANK STRAT — drink Burning Blood cauldron pre-pull, AoE pulls cluster.
      Burning Blood deals AoE fire dmg around drinker — tank cluster anchor.
    second_pack_rabid_gurubashi: |
      TANK STRAT — drink Frostburn cauldron, strike a Rabid Gurubashi for
      stun-burst. Rabid Gurubashi have NO threat table (fixate); tank
      controls one, group bursts.

  gurubashi_spirit_warriors_pre_jindo:
    notes: pre-fight kill on stairs; standard tank-and-spank on 2 mobs
```

---

## Boss 1 — High Priest Venoxis

```yaml
boss_tank:
  boss: high_priest_venoxis
  difficulty_grade: hard (multi-phase repositioning)
  threat_intensity: moderate

  positioning:
    phase_1: |
      Pull boss centrally on platform; navigate around poison maze patches
      as they spawn. Keep boss in a clean zone.
    phase_2: |
      Reposition boss away from Pool of Acrid Tears (which spawns at
      furthest player). Rotate boss face during Breath of Hethiss cone
      cast — point cone away from group.
    phase_3: |
      Drag boss to platform edge (off altar floor poison). Lead Bloodvenom
      tendrils away from group; tendrils chase fixated targets, so tank
      doesn't kite tendrils — tank just holds boss while group manages.
    facing: |
      Phase 1: away from group (standard).
      Phase 2: rotate 90° on Breath of Hethiss cast telegraph.
      Phase 3: away from group.
    movement: high — reposition every phase

  threat_management:
    standard: maintain threat through phase transitions
    phase_2: |
      Snake form has +50% physical damage taken (loses dodge/parry).
      Tank's threat scales with damage dealt — threat is easier this phase.
    phase_3: |
      Tendrils don't fixate the tank; threat focus stays on Venoxis.

  defensive_cd_use:
    phase_2: |
      Snake form melee hits harder on tank (boss is Blessed of the Snake
      God = +50% physical dmg dealt). Pop short defensive on phase 2 entry:
      - Prot Warrior: Shield Block
      - Prot Pally: Holy Shield (eaten? UNVERIFIED Cata) / Divine Protection
      - Blood DK: Vampiric Blood + Death Strike heals
      - Feral Bear: Survival Instincts + Frenzied Regeneration
    phase_3: |
      Major defensive on Phase 3 entry — tendril splash + boss melee +
      altar floor potential = peak healer pressure window.

  interrupt_role:
    whispers_of_hethiss: |
      PARTICIPATE — primary kick rotation. Tank kicks:
      - Prot Warrior: Pummel (10s CD; off-GCD)
      - Prot Pally: Rebuke (15s CD; off-GCD)
      - Blood DK: Mind Freeze (10s CD; off-GCD)
      - Feral Bear: Skull Bash (60s CD; gap-close)

  heroic_delta: |
    Heroic-only fight.

  bot_anti_patterns:
    - DO NOT keep boss in poison maze patches (Phase 1)
    - DO NOT face boss into group during Breath of Hethiss (cone clip group)
    - DO NOT stand on altar in Phase 3 (poison floor)
    - DO NOT chase tendrils (Phase 3) — group kites them; tank holds boss
    - DO NOT skip Phase 2 defensive CD use — boss melee hits hard
```

---

## Boss 2 — Bloodlord Mandokir

```yaml
boss_tank:
  boss: bloodlord_mandokir
  difficulty_grade: hard (Frenzy phase + Ohgan threat hand-off)
  threat_intensity: moderate

  positioning:
    pull: pull Mandokir near center of arena
    facing: away from group (standard)
    movement: minimal; lateral dodges on Devastating Slam if cone clips tank

  threat_management:
    standard: hold threat on Mandokir
    ohgan_handling: |
      Tank does NOT need to tank Ohgan. Ohgan attacks Chained Spirits
      (passive friendlies); group melee/ranged kill Ohgan from his patrol
      path. Tank stays on Mandokir.
    decapitate_target_protection: |
      Tank cannot prevent Decapitate (random non-tank target). If Decapitate
      pulls tank-target threat after revive, taunt to re-establish.

  defensive_cd_use:
    standard: rotational short defensives
    bloodletting_on_self: |
      If Bloodletting (96776) lands on tank, healer SUPPRESSES heal. Tank
      should:
      - NOT pop major CD (defeats the heal-trap mechanic)
      - Pop SHORT defensive only if HP < 30% (life-saving)
      - Accept the HP drop; emergency-heal post-expiry
    frenzy_phase: |
      Major defensive (Shield Wall / Survival Instincts / Icebound
      Fortitude) at 20% HP entry — boss attack speed doubles.

  interrupt_role:
    none: Devastating Slam non-interruptible (UNVERIFIED)

  heroic_delta: |
    Heroic-only fight.

  bot_anti_patterns:
    - DO NOT taunt Ohgan off DPS (Ohgan is DPS-target, not tank-target)
    - DO NOT chase Ohgan around the arena (he patrols Chained Spirits;
      group kills him when he comes close)
    - DO NOT panic on Bloodletting on self (heal-suppress is intended)
    - DO NOT skip Frenzy-phase major defensive
    - DO NOT stand in Devastating Slam furrow (lateral dodge if it points at tank)
```

---

## Boss 3 — High Priestess Kilnara

```yaml
boss_tank:
  boss: high_priestess_kilnara
  difficulty_grade: hard (16 panther multi-pull + boss)
  threat_intensity: high

  positioning:
    panther_clear: |
      Pull 2-4 sleeping panthers at a time; cluster them for AoE. Heroic-
      geared groups can pull all 16 with cooldowns; default = 2-4.
    boss_phase_1: |
      Pull Kilnara to a CLEARED zone (panther-free). Stand in melee range
      of Kilnara; group ranged > 12y to dodge Tears of Blood radius.
    boss_phase_2: |
      Avatar of Bethekk (panther form). Maintain threat through leap
      attacks; tank stays put — Kilnara comes to tank.
    facing: away from group (standard)
    movement: minimal in boss phases; cluster-pull movement during panther clear

  threat_management:
    panther_aoe: |
      Cluster panthers; rotate AoE threat tools (Thunder Clap / Swipe /
      Death and Decay / Consecration / Avenger's Shield). Maintain threat
      lead during AoE damage.
    boss_only_phases: standard ST threat
    panther_aggro_during_kilnara_phase_2: |
      If panthers awaken during Phase 2 (push-fail), tank picks them up
      before they aggro group. Rare and recoverable but stressful.

  defensive_cd_use:
    panther_clear: |
      Rotate short defensives during multi-pull (cluster bleeds add up).
      Pop major defensive on largest cluster.
    phase_2_avatar_haste: |
      Kilnara hastes in Phase 2; tank takes rapid melee. Major defensive
      on Phase 2 entry; rotate short CDs through Phase 2.

  interrupt_role:
    tears_of_blood: PRIMARY — participate in kick rotation
    shadow_bolt: SECONDARY — kick if rotation has slack

  heroic_delta: |
    Heroic-only fight.

  bot_anti_patterns:
    - DO NOT pull all 16 panthers at once unless group has burst CDs ready
    - DO NOT push Kilnara to 50% with panthers asleep (un-healable Phase 2)
    - DO NOT face Kilnara into group (Wave of Agony cone)
    - DO NOT walk Kilnara through sleeping panthers (wakes them via aura/proc — UNVERIFIED but safer assumption)
```

---

## Boss 4 — Zanzil

```yaml
boss_tank:
  boss: zanzil
  difficulty_grade: medium (cauldron mechanic + reposition)
  threat_intensity: moderate

  positioning:
    pull: pull Zanzil near 2 cauldrons (Burning Blood + Toxic Torment for tank's typical drinks)
    facing: away from group
    movement: |
      REPOSITION on Zanzili Fire ground line cast. Drag Zanzil 90° off the
      line; group steps off line.

  threat_management:
    standard: hold threat on Zanzil
    zombie_pickup: |
      On Red elixir, multiple Zanzili Zombies spawn. Tank-cluster zombies in
      melee range; AoE threat (Thunder Clap / Swipe / DnD / Consecration);
      drink Burning Blood cauldron for AoE fire pulse around tank.
    berserker_no_threat: |
      Blue elixir Berserker has NO threat table (fixates random player).
      Tank does NOT engage Berserker via taunt — group bursts with
      Frostburn cauldron buff.

  defensive_cd_use:
    standard: short defensive on rotation
    red_elixir_zombies: short defensive when zombies hit (cluster damage)
    graveyard_gas_on_tank: tank drinks Toxic Torment with group on Green elixir
    no_special_save: standard tank-fight defensive cadence

  interrupt_role:
    voodoo_bolt: PRIMARY — participate in kick rotation

  heroic_delta: |
    Heroic-only fight.

  bot_anti_patterns:
    - DO NOT keep Zanzil in Zanzili Fire line (move boss out)
    - DO NOT drink Frostburn (tank's next strike on Zanzil would waste / unsafe)
    - DO NOT taunt Berserker (off-threat fixate)
    - DO NOT skip Toxic Torment on Green elixir (gas damage is unmitigable without)
    - DO NOT skip Burning Blood drink on Red elixir (zombies will overwhelm without AoE)

  bot_implementation_flag: |
    Cauldron is a clickable GameObject. Tank bot must:
      - Pathfind to Burning Blood cauldron on Red elixir (cluster-anchor)
      - Pathfind to Toxic Torment on Green
      - NOT drink Frostburn (boss-target unsafe; never use as tank)
      - Use the cauldron object (interact action)
      - Resume tanking after acquiring buff (with Zanzil still in melee range)
```

---

## Boss 5 — Jin'do the Godbreaker

```yaml
boss_tank:
  boss: jin_do_the_godbreaker
  difficulty_grade: very_hard (multi-phase positioning + parallel zone)
  threat_intensity: high

  positioning:
    phase_1_default: |
      Pull Jin'do NEAR (but not on) where Deadzones spawn. On Shadows of
      Hakkar cast START (3s warning), drag Jin'do INTO active Deadzone
      (or position group to step in). On cast END, drag boss back OUT.
    phase_2_spirit_realm: |
      Pick up Gurubashi Spirit; position spirit so its Body Slam jump-target
      lands ON A CHAIN LOCATION. Tank holds spirit near chain; group
      stands on chain.
    phase_3_return: tank-and-spank Jin'do to 0 in physical realm
    facing: away from group (standard)
    movement: high — Phase 1 in/out of Deadzone, Phase 2 chain positioning

  threat_management:
    phase_1: standard threat on Jin'do
    phase_2_spirit_pickup: |
      Pickup Gurubashi Spirit on spawn; position via taunt/pull for Body
      Slam landing on chain.
    phase_2_twisted_spirit_adds: |
      AoE threat on adds; off-target adds are group's responsibility but
      tank takes initial threat ping.

  defensive_cd_use:
    phase_1_shadows_of_hakkar: |
      Major defensive timing IF positioned outside Deadzone (positioning
      fail) — otherwise standard rotational.
    phase_2_entry: short defensive on Spirit Realm entry (initial Twisted
      Spirit + spirit body slam pressure)
    phase_2_per_chain: rotate short CDs per chain-break cycle
    phase_3_return: standard rotational

  interrupt_role:
    none: Shadows of Hakkar non-interruptible

  heroic_delta: |
    Heroic-only fight.

  bot_anti_patterns:
    - DO NOT stand outside Deadzone during Shadows of Hakkar (~120k death)
    - DO NOT keep Jin'do inside Deadzone outside Shadows of Hakkar (90% cast speed reduction = no threat from spells; melee strike-only)
    - DO NOT position Gurubashi Spirit away from chains in Phase 2 (Body Slam misses chain location)
    - DO NOT lose track of group in parallel zone (Phase 2)

  bot_implementation_flag_phase2: |
    Phase 2 parallel-zone teleport. Tank bot must:
      - Detect zone/phase ID change at 70% Jin'do HP
      - Re-acquire group references on Spirit Realm side
      - Pick up Gurubashi Spirit immediately on spawn
      - Position spirit for Body Slam on chain
      - Re-acquire Jin'do target on return-to-physical-realm transition
    Without this, Phase 2 is un-completable.

  bot_implementation_flag_phase1: |
    Deadzone in/out timing override. Tank bot must:
      - Detect Shadows of Hakkar cast start (3s) → DRAG BOSS INTO Deadzone
      - Detect cast end → DRAG BOSS OUT
    The "drag boss" action is movement-while-tanking, not just self-move.
```

---

## Optional Boss — Cache of Madness

```yaml
boss_tank:
  boss: cache_of_madness
  optional: true
  default_skip: true                            # Archaeology dependency

  positioning_per_variant:
    hazza_rah: tank illusion adds at 66/33%; ST on boss between
    renataki: tank-and-spank; pickup post-Vanish (re-establish threat)
    wushoolay: SPREAD positioning around room; avoid Lightning Cloud
    gri_lek: tank-and-spank; dodge Rupture Line ground

  bot_anti_patterns:
    - DO NOT engage if Archaeology not unlocked
```

---

## Bot AI hooks

```yaml
ai_hooks:
  venoxis:
    - encounter_detect_whispers_cast → kick_rotation_participate
    - encounter_state_phase_2_active → defensive_cd_short_on_entry
    - encounter_detect_breath_of_hethiss_cast → ROTATE_BOSS_FACING_AWAY_FROM_GROUP
    - encounter_state_phase_3_active → defensive_cd_major_on_entry
    - encounter_detect_acid_pool_spawn(target) → reposition_boss_off_pool

  mandokir:
    - encounter_detect_ohgan_active → no_action (group kills Ohgan; tank holds boss)
    - encounter_detect_decapitate_landed_on_dps → taunt_if_threat_lost_post_revive
    - encounter_detect_bloodletting_on_self → defensive_only_if_hp_lt_30
    - encounter_state_frenzy_phase → MAJOR_DEFENSIVE_ON_ENTRY
    - encounter_detect_devastating_slam_telegraph → lateral_dodge_if_arc_includes_tank

  kilnara:
    - encounter_state_panther_clear → cluster_panthers_AND_aoe_threat_tools
    - encounter_detect_tears_of_blood_cast → kick_rotation_participate
    - encounter_state_kilnara_phase_2 → MAJOR_DEFENSIVE_ON_ENTRY + sustained_threat
    - encounter_detect_panther_wake_during_phase_2 → emergency_pickup_on_strays

  zanzil:
    - encounter_detect_voodoo_bolt_cast → kick_rotation_participate
    - encounter_detect_zanzili_fire_line → REPOSITION_BOSS_OFF_LINE
    - encounter_detect_red_elixir → DRINK_BURNING_BLOOD AND cluster_zombies (bot_implementation_flag)
    - encounter_detect_green_elixir → DRINK_TOXIC_TORMENT
    - encounter_detect_blue_elixir → no_taunt (Berserker is off-threat fixate)

  jindo:
    - encounter_detect_shadows_of_hakkar_cast → DRAG_BOSS_INTO_DEADZONE (encounter override)
    - encounter_detect_shadows_of_hakkar_resolved → DRAG_BOSS_OUT_OF_DEADZONE
    - encounter_detect_phase_2_transition → handle_zone_change (bot_implementation_flag)
    - encounter_detect_gurubashi_spirit_spawn → PICKUP_AND_POSITION_NEAR_CHAIN
    - encounter_detect_brittle_barrier_removed(chain) → no_action (DPS bursts; tank holds spirit)
    - encounter_detect_phase_3_return_to_physical → re_acquire_jindo_threat

dispel_blacklist: []                            # no fight-level blacklists

defensive_cd_priority_per_fight:
  venoxis:    [phase_2_entry: short; phase_3_entry: major; rotational otherwise]
  mandokir:   [reactive_only_outside_frenzy; frenzy_phase: major]
  kilnara:    [panther_clear: rotate short CDs; phase_2_entry: major]
  zanzil:     [reactive_only; cauldron-driven]
  jindo:      [phase_1_short_on_shadows_of_hakkar_if_outside_deadzone (positioning fail); phase_2_entry: short; per_chain_break: rotate short]
```

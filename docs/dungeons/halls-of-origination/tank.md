# Halls of Origination — Tank

**Tank-specific response logic** for the bot's encounter handling. Mechanics, spell IDs, phase triggers, and add abilities are defined in `encounter.md`; this doc only encodes how the tank reacts. Class-agnostic where possible.

**Layering**:
- Cross-content tank patterns (threat priority, defensive CD pacing, taunt rules) live in `docs/roles/tank.md` (deferred to tank phase).
- Per-spec rotation / CD / threat model lives in the corresponding `docs/specs/` tank doc (all tank specs deferred to tank phase).
- This doc layers HoO-specific deviations on top of those.

---

## Dungeon-level overview

```yaml
dungeon_tank_overview:
  total_bosses: 7
  estimated_full_clear_minutes: 35-55     # LONGEST Cata 5-man
  threat_profile:
    anhuur: low                           # tank-and-spank topside; pit-runners aren't tank's job
    ptah: moderate                        # add pickup at 50%; cleave-aware positioning
    anraphet: HIGH                        # warden gauntlet + boss + Stone Trogg constant-spawn
    setesh: NON_STANDARD                  # boss UNTANKABLE; tank role = add-tank for portals
    ammunae: moderate                     # Bloodpetal Sprout pickup on Rampant Growth conversion
    isiset: low                           # tank holds Isiset central; familiars handled by DPS
    rajh: moderate                        # standard tanking + Sun Strike DoT
  defensive_cd_profile:
    anhuur: light                         # rotational on Burning Light splash
    ptah: light                           # cleave + add melee
    anraphet: heavy                       # Vault gauntlet pressure + awakening burst + Omega Stance
    setesh: moderate                      # add cleave from kited adds
    ammunae: light                        # standard
    isiset: light                         # standard
    rajh: heavy                           # Sun Strike DoT continuous
  key_responsibilities:
    - threat_priority_per_fight
    - non_standard_setesh_add_tank        # canonical bot_implementation flag
    - add_pickup (anhuur N/A, ptah dustbones, ammunae sprouts, anraphet trogg gauntlet)
    - face_away_from_group                # standard for Ptah Raging Smash, Anraphet, Rajh
    - face_away_isiset_supernova          # natural by default tank facing
    - awakening_burst_defensive (Anraphet auto-AoE)
    - blessing_of_the_sun_burn_window (Rajh — tank survives 8s AoE channel)
```

---

## Trash — tank notes

```yaml
trash_tank:
  pre_anhuur_anubisath:
    pull_pattern: |
      Anubisath patrols + caster mixes. CC humanoid casters (DPS Polymorph
      / Sap); skull on lead caster. Tank pulls melee + non-CC'd casters.
    facing: away from group (Anubisath cleave)

  vault_of_lights_gauntlet:
    pull_pattern: |
      Tank engages first elemental warden; Stone Troggs spawn continuously
      on warden engagement. Tank holds trogg threat via AoE-threat tools.
    aoe_threat: |
      Bot tank logic — rotate AoE-threat tools on continuous trogg spawn:
      - Prot Warrior: Thunder Clap on CD; Shockwave; Cleave on rage
      - Prot Paladin: Consecration ground-pull; Avenger's Shield; Hammer of the Righteous
      - Blood DK: Death and Decay; Heart Strike (cleaves); Blood Boil
      - Feral Tank Druid: Swipe; Thrash; Magic dispel via Tree if needed
    facing: |
      Active warden faces away from group (cleave); tank rotates between
      wardens 1 at a time. Group focus-fires while tank holds trogg AoE
      threat.
    notes: |
      Most threat-intensive trash gauntlet in Cata 5-mans. Bot tank must
      hold simultaneous threat on:
      - 1 active elemental warden (focus target)
      - 3 idle elemental wardens (won't pull aggro until DPS targets them)
      - Continuous Stone Trogg trash (AoE-threat sweep)

  setesh_halls_gauntlet:
    pull_pattern: |
      Multiple smaller pulls; CC Shadowlancers (humanoid). Tank pulls melee
      + non-CC'd; positions to pull mob casters off LoS.
    facing: away from group
    notes: precursor mana-drain to Setesh fight; healer drinks at stair landings

  construct_hall_trash:
    pull_pattern: standard tank-and-spank + caster CC priority
    facing: away from group (frontal cleaves on Anubisath)
```

---

## Boss 1 — Temple Guardian Anhuur

```yaml
boss_tank:
  boss: temple_guardian_anhuur
  difficulty_grade: medium                # threat low but coordination required
  threat_intensity: low

  positioning:
    pull: tank engages Anhuur at center of upper platform
    facing: |
      AWAY from group (no boss frontal cone, but standard tank-and-spank
      face-away for cleave-style splash damage if any)
    movement: |
      Stay on upper platform. Do NOT drop into pit (per Disagreement #8 — tank
      stays topside). Move out of Burning Light blue ground.

  threat_management:
    standard: hold threat on Anhuur; minimal AoE pressure (no adds for tank)
    pit_runner_threat: |
      Pit Vipers attack pit-runners (not tank). Tank does NOT need to taunt
      vipers. If a pit-runner dies and viper aggros to platform, tank picks
      up viper.

  defensive_cd_use:
    burning_light_splash: minor — only if positioning catches tank in patch
    reverberating_hymn: rotational defensive on shield phases (channel hits tank too)
    divine_reckoning_target: |
      If tank is debuffed, MOVE OUT of group; healer dispels Magic. Do not
      sit on debuff.

  interrupt_role:
    none: |
      Reverberating Hymn is INTERRUPT-IMMUNE during shield. Burning Light /
      Divine Reckoning have no interrupt window. Tank does not interrupt
      anything on Anhuur.

  heroic_delta: |
    - Beacon cast 8s → 10s (longer shield phase = tank survives more Hymn)
    - Reverberating Hymn damage scales harder; defensive CD on each shield phase

  bot_anti_patterns:
    - DO NOT drop into pit (tank stays topside; pit-runners are 2 DPS by default)
    - DO NOT chase pit-runners through chamber
    - DO NOT try to interrupt Reverberating Hymn (interrupt-immune during shield)
    - DO NOT stand in Burning Light blue ground

  bot_implementation_flag: |
    Tank's role on Anhuur is unusual ONLY in that it's PASSIVE during shield
    phases (boss is immune; pit-runners are doing the work). Bot tank must
    NOT engage other behavior (no AoE-threat to pull, no taunt cycle). Just
    survive Reverberating Hymn channel + heal between phases.
```

---

## Boss 2 — Earthrager Ptah

```yaml
boss_tank:
  boss: earthrager_ptah
  difficulty_grade: easy
  threat_intensity: moderate

  positioning:
    pull: tank engages Ptah at center of sand chamber
    facing: |
      AWAY from group — Raging Smash 150% cleave on tank + 2 nearest
      enemies. Face boss away so cleave doesn't catch melee/healer.
    movement: |
      Rotate boss when Quicksand pools spawn near current position. Pull
      boss to clean ground; minimize melee re-positioning.

  threat_management:
    standard: hold threat on Ptah throughout phase 1 + phase 2
    burrow_phase_50: |
      Boss disperses; Dustbone Horror (2) + Jeweled Scarab (8) adds spawn.
      Bot tank: AoE-threat tools immediately on add-spawn event:
      - Prot Warrior: Thunder Clap + Shockwave + Cleave
      - Prot Paladin: Consecration + Avenger's Shield + Hammer of the Righteous
      - Blood DK: Death and Decay + Blood Boil + Heart Strike (cleaves)
      - Feral Tank Druid: Swipe + Thrash
      Tank picks up Dustbone Horrors (high HP) + holds scarab cluster threat.

  defensive_cd_use:
    raging_smash_high_damage: rotational defensive on stack (Shield Block / Barkskin / Blood Shield)
    flame_bolt_self_target: minor — only if tank is one of the 2 (3 heroic) channeled targets
    earth_spike_landed: rotational defensive if tank is caught
    burrow_phase: defensive CD on add cleave

  interrupt_role:
    flame_bolt: PARTICIPATE in kick rotation if tank has interrupt (most tanks do — Pummel / Rebuke / Mind Freeze / Skull Bash)

  heroic_delta: |
    - Flame Bolt targets 3 (vs 2)
    - Sand vortex tornado spawns during burrow phase — kite-out
    - More Quicksand pools — more frequent boss-relocation

  bot_anti_patterns:
    - DO NOT face boss into group (Raging Smash cleave)
    - DO NOT stand in Quicksand
    - DO NOT abandon adds during burrow phase (bot must hold AoE threat throughout)
    - DO NOT chase sand vortex tornado (kite-out, don't chase)
```

---

## Boss 3 — Vault of Lights gauntlet + Anraphet

```yaml
boss_tank:
  boss: vault_of_lights_anraphet
  difficulty_grade: HARD                  # most threat-intensive boss in dungeon
  threat_intensity: HIGH

  # ============================================================
  # PHASE A — Warden gauntlet
  # ============================================================
  warden_gauntlet:
    positioning: |
      Tank engages first elemental warden. Stone Troggs continuously spawn
      around chamber. Tank holds simultaneous threat on:
      - 1 focus warden
      - 3 idle wardens (low priority — don't engage until focus dies)
      - Continuous trogg trash

    threat_management: |
      AoE-threat sweep on trogg cluster + maintain threat on focus warden:
      - Prot Warrior: Thunder Clap (every CD) + Shockwave + Cleave on rage
      - Prot Paladin: Consecration ground-pull + Avenger's Shield + Hammer of the Righteous
      - Blood DK: Death and Decay (every CD) + Blood Boil + Heart Strike (cleaves)
      - Feral Tank Druid: Swipe + Thrash + Mangle bear

    facing: |
      Active warden faces away from group; tank rotates as wardens die.

    defensive_cd_use:
      warden_nova: rotational defensive on each warden's nova event (Flame, Earth, Air, Water)
      trogg_cleave: rotational on dense trogg pulls

    add_pickup: |
      Stone Trogg spawn cadence ~ every 6-10s per spawn pad. Tank must
      maintain threat sweep continuously; if a trogg pulls onto a DPS or
      healer, that's a tank failure.

  # ============================================================
  # PHASE B — Anraphet
  # ============================================================
  anraphet:
    positioning: |
      AFTER 4th warden dies + awakening burst: tank engages Anraphet
      central. On heroic, dynamically re-position as void zones accumulate
      — pull boss to clean ground.

    facing: away from group (standard)
    movement: rotate boss away from new void zones (heroic)

    threat_management:
      standard: maintain threat on Anraphet throughout
      crumbling_ruin: stacks reduce party max-HP; tank must survive boss melee + Nemesis Strike DoT

    defensive_cd_use:
      awakening_burst: |
        UNAVOIDABLE room AoE on 4th warden death. Bot tank: pop major
        defensive CD (Shield Wall / Survival Instincts / Icebound Fortitude /
        Ardent Defender / Last Stand) just BEFORE 4th warden dies.
        bot_implementation_flag: predict-and-pre-CD on warden HP near 0.
      omega_stance_channel: rotational defensive on each Omega Stance cast
      nemesis_strike_high_stack: |
        If healer can't dispel fast enough, tank pops short defensive on
        DoT renewal.
      crumbling_ruin_late: major defensive at high stacks (max-HP reduced; absolute HP precarious)

    interrupt_role:
      none: no kick-required casts on Anraphet
      stone_trogg_during_gauntlet: kick rotation on trogg casters

    heroic_delta: |
      - Persistent Alpha Beam void zones — tank dynamic re-positioning
      - Crumbling Ruin stacks faster — defensive CD pacing tighter
      - Stone Trogg gauntlet faster spawn — AoE-threat density increased
      - Awakening burst harder — bigger pre-CD window

    bot_anti_patterns:
      - DO NOT engage Anraphet before awakening AoE resolves (boss is targetable but in motion; threat lost)
      - DO NOT chase wardens 1-by-1 if troggs pulling group (AoE-threat first)
      - DO NOT skip pre-CD on 4th warden death (awakening burst is significant on tank)
      - DO NOT skip dispel-on-tank Nemesis Strike calls (healer responsibility, but tank can self-cleanse on Pala)
      - DO NOT stand in Alpha Beam path (rare — boss usually targets random ally, but possible)
```

---

## Boss 4 — Setesh, Construct of Destruction

```yaml
boss_tank:
  boss: setesh
  difficulty_grade: HARD                  # most non-standard tank role in dungeon
  threat_intensity: NON_STANDARD          # boss UNTANKABLE; tank role is add-tank

  positioning:
    pull: |
      Tank does NOT engage boss directly (untankable). Tank positions to
      intercept first portal spawn; group spreads.
    facing: away from active adds (whatever tank is currently kiting)
    movement: |
      MOBILE — kite Sentinels around the room (rooted/slowed); intercept
      Wurms; pick up Seekers if loose.

  threat_management:
    boss_untankable: |
      CRITICAL — boss has NO threat table. Setesh attacks random players
      with Chaos Bolt. Tank does NOT attempt to threat boss. Bot tank
      logic flag: SETESH_UNTANKABLE_MODE.
    add_priority: |
      Tank holds threat on portal adds:
      1. Void Wurms (on closest player; AoE-burn target — tank picks up to keep them in cleave)
      2. Void Seekers (if loose — kick + threat hold)
      3. Void Sentinels (90% damage reduction — KITE with roots/slows; do NOT attempt high-DPS)
    dk_tank_kite: |
      Blood DK tank: Chains of Ice spam keeps adds slowed — best class for
      this fight per Dexerto. Constant snare uptime makes Sentinel-kiting
      trivial.
    other_tank_classes: |
      Prot Warrior: Hamstring (5s slow) on rage; Piercing Howl (10y AoE slow)
      Prot Paladin: Hand of Reckoning (taunt); no native slow — relies on Earth Bind Totem from Shaman or Frost Trap from Hunter
      Feral Tank Druid: Infected Wounds (passive 50% slow on Mangle)
    bot_implementation_flag: |
      Tank's behavior on Setesh is FUNDAMENTALLY DIFFERENT from every other
      Cata 5-man boss. Bot tank state machine flag: SETESH_ADD_TANK_MODE.
      Skip boss-threat logic; engage add-tank logic exclusively.

  defensive_cd_use:
    chaos_bolt_target_self: rotational on if Setesh targets tank (random)
    add_cleave: rotational on dense portal-spawn cleave
    save_for_heroic: heroic portals don't auto-close → adds compound; major CDs on simultaneous Wurm + Seeker waves

  interrupt_role:
    anti_magic_prison: |
      ABSOLUTE PRIORITY — if tank has interrupt, kick Anti-Magic Prison cast.
      All tanks have interrupts (Pummel / Rebuke / Mind Freeze / Skull Bash).
      Tank kick is a primary part of the rotation here.

  heroic_delta: |
    - Portals don't auto-close — tank holds adds for longer
    - More adds per portal cycle
    - Mana drain on healer extreme — tank defensive CDs help relieve healer mana

  bot_anti_patterns:
    - DO NOT attempt to threat Setesh (untankable; wastes tank rage/runes/CDs)
    - DO NOT focus DPS on Sentinels (Void Barrier 90% reduction)
    - DO NOT skip Anti-Magic Prison kick (priority kick; tank participates in rotation)
    - DO NOT attempt to taunt Sentinels off DPS (Sentinels target closest player; tank stays close)
    - DO NOT skip slow/root abilities (Sentinel kite requires snare uptime)
```

---

## Boss 5 — Ammunae, Construct of Life

```yaml
boss_tank:
  boss: ammunae
  difficulty_grade: medium
  threat_intensity: moderate

  positioning:
    pull: tank engages Ammunae central
    facing: away from group (standard)
    movement: |
      Stay central. Pre-position to pick up Bloodpetal Sprouts on Rampant
      Growth conversion — Sprouts spawn where pods were.

  threat_management:
    standard: hold threat on Ammunae throughout
    sprout_pickup: |
      On Rampant Growth cast (boss energy 100), all Seedling Pods convert
      to Bloodpetal Sprouts. Tank uses AoE-threat tool to pick up sprout
      cluster. DPS swaps to AoE-burn.

  defensive_cd_use:
    rampant_growth_burst: rotational defensive on each Rampant Growth (29-34k AoE)
    sprout_cleave: rotational on sprout pickup cluster
    consume_life_energy_target: minor — channel is on random player, rarely tank

  interrupt_role:
    consume_life_energy: |
      ABSOLUTE PRIORITY — if tank has interrupt, kick Consume Life Energy.
      Tank participates in kick rotation.
    wither: SECONDARY — interrupt OR healer dispel after-cast

  heroic_delta: |
    - Pods spawn faster — more Sprouts on Rampant Growth conversion
    - Bloodpetal Sprouts hit harder — tank defensive CDs more important on sprout pickup

  bot_anti_patterns:
    - DO NOT skip Sprout pickup (sprouts will cleave healer if loose)
    - DO NOT chase pods (ranged DPS handles pod kills; tank stays on boss)
    - DO NOT skip Consume Life Energy kick if tank has interrupt
```

---

## Boss 6 — Isiset, Construct of Magic

```yaml
boss_tank:
  boss: isiset
  difficulty_grade: easy
  threat_intensity: low

  positioning:
    pull: tank engages Isiset central
    facing: |
      AWAY from group — Supernova face-away mechanic. Tank's natural
      "face boss away" already satisfies the mechanic (tank is between
      Isiset and group; tank's BACK is to Isiset's front; tank's FACE is
      to group, which is the safe direction).
      EDGE: tank facing depends on which direction you call "facing the
      boss" vs "facing toward group" — for HoO Isiset, the canonical
      tank position is BEHIND the boss (away from group). Bot tank logic:
      verify position is "between group and Isiset's BACK" not "between
      group and Isiset's FACE."
    movement: stay central; rotate boss minimally

  threat_management:
    standard: hold threat on Isiset
    split_phase: |
      During split phase, tank may need to pick up Astral Familiar (mirror
      image) cluster. Default: tank stays on focus-fire familiar (closest)
      while DPS focuses. On heroic, tank cleaves all 3 / 2 mirror images
      for AoE threat.
    celestial_familiar_adds: |
      Small adds summoned by Celestial Call. AoE-threat on summon; let
      DPS burn down.

  defensive_cd_use:
    astral_rain_self_target: rotational if tank caught in Astral Rain ground
    supernova_facing_disorient: |
      If tank facing is wrong on Supernova, disorient lasts 4s — tank
      loses threat but boss positioned at center. Use defensive CD if
      caught.
    veil_of_sky_reflect: rotational if tank attacks Veil of Sky-shielded familiar (reflect chance)

  interrupt_role:
    arcane_barrage_familiar: PARTICIPATE in kick rotation on Celestial Familiar adds

  heroic_delta: |
    - All damage values higher
    - Familiar HP higher (split phase longer; more time facing-away)
    - No novel mechanics

  bot_anti_patterns:
    - DO NOT face Isiset on Supernova cast (4s disorient)
    - DO NOT melee Veil of Sky-shielded familiar (reflect chance on tank)
    - DO NOT skip Astral Familiar pickup on split (cluster threat for AoE-cleave)
```

---

## Boss 7 — Rajh, Construct of the Sun

```yaml
boss_tank:
  boss: rajh
  difficulty_grade: medium                # final boss; sustained tank pressure
  threat_intensity: moderate

  positioning:
    pull: tank engages Rajh central; group spreads
    facing: away from group (standard)
    movement: |
      Kite-out Solar Winds tornado (4y radius). On heroic, avoid Solar Fire
      patches (89131) dropped along Solar Winds path.

  threat_management:
    standard: hold threat on Rajh throughout
    blessing_of_the_sun_channel: |
      8s recharge phase. Tank survives AoE; no threat issue (boss channels
      in center). No active threat management during channel.

  defensive_cd_use:
    sun_strike_continuous_dot: |
      UNDISPELLABLE tank DoT applied on each Sun Strike cycle. Continuous
      damage on tank. Bot tank: rotational defensive CDs (Shield Block /
      Barkskin / Blood Shield) on each cycle.
    blessing_of_the_sun_aoe: |
      Pop major defensive CD on Blessing of the Sun channel start (tank
      takes group AoE damage).
    inferno_leap_self_target: |
      Random target leap. If tank is target (rare), pop short defensive
      AND step away from group (10y radius).

  interrupt_role:
    sun_orb: PRIMARY KICK if tank has interrupt — 3s channel; rotation
    inferno_leap: PRIMARY KICK — 3s telegraph; saves random player from leap-kill

  heroic_delta: |
    - Solar Fire (89131) persistent fire patches — tank kite path narrows
    - Sun Strike DoT hits harder — defensive CD pacing tighter
    - Achievement window (Sun-of-a...) tightens kill timer — tank survives only one Blessing channel

  bot_anti_patterns:
    - DO NOT stand in Solar Winds tornado path
    - DO NOT skip kicks on Sun Orb / Inferno Leap (tank participates in rotation)
    - DO NOT attempt to dispel Sun Strike DoT (UNDISPELLABLE)
    - DO NOT save defensive CDs through entire fight (Sun Strike DoT is continuous; rotate)
    - DO NOT chase Solar Winds tornado (kite-out, don't pursue)

  burn_window:
    blessing_of_the_sun: |
      During Blessing channel: ALL PLAYERS gain +100% damage. Tank
      contribution to burn window:
      - Prot Warrior: Avatar / Bloodbath / threat-cleaves
      - Prot Paladin: Avenging Wrath (offensive); Holy Shield rotation
      - Blood DK: Pillar of Frost + AMS for self-mitigation; threat continues
      - Feral Tank Druid: Berserk (Bear AoE) + Mangle on CD
      bot_implementation: tank offensive-CD heuristic aligns with Blessing window
      (in addition to standard tank-mitigation heuristic).
```

---

## Bot AI hooks

```yaml
ai_hooks:
  anhuur:
    - encounter_state_active → tank_topside_central + face_away
    - encounter_detect_burning_light_self → move_out
    - encounter_detect_shield_of_light_cast → no_engage_change (boss immune)
    - encounter_detect_divine_reckoning_self → move_out_of_group + signal_dispel
    - encounter_state_pit_runners_active → no_action_change (pit-runners self-manage)

  ptah:
    - encounter_state_active → face_away_from_group + threat_hold
    - encounter_detect_quicksand_self → move_to_clean_ground
    - encounter_detect_earth_spike_self → move_out
    - encounter_detect_flame_bolt_cast → kick_if_self_has_interrupt
    - encounter_state_burrow_phase → AoE_threat_tools (Thunder Clap / Consecration / DnD / Swipe)
    - encounter_detect_dustbone_horror_spawn → taunt_or_threat_pickup
    - encounter_detect_sand_vortex_heroic → kite_out_path

  vault_of_lights_anraphet:
    - encounter_state_warden_gauntlet → AoE_threat_continuous (Stone Troggs) + threat_focus_warden
    - encounter_detect_warden_nova → defensive_cd_short
    - encounter_state_4th_warden_HP_below_5 → PRE_CD_MAJOR (Shield Wall / Survival Instincts) for awakening burst
    - encounter_detect_anraphet_awakening_aoe → defensive_cd_active
    - encounter_state_anraphet_active → threat_hold + face_away
    - encounter_detect_omega_stance_cast → defensive_cd_short
    - encounter_detect_nemesis_strike_high_stack → defensive_cd_short OR self_cleanse_if_paladin
    - encounter_detect_alpha_beam_self → move_out (rare; usually random ally)
    - encounter_state_crumbling_ruin_high_stack → defensive_cd_major

  setesh:
    - encounter_state_active → set_flag_SETESH_UNTANKABLE_MODE + skip_boss_threat_logic
    - encounter_detect_chaos_portal_open → intercept_position_for_adds
    - encounter_detect_void_wurm_spawn → AoE_threat_pickup
    - encounter_detect_void_sentinel_spawn → kite_with_snare (Chains of Ice / Hamstring / Infected Wounds)
    - encounter_detect_void_seeker_spawn → threat_pickup + intercept
    - encounter_detect_anti_magic_prison_cast → kick_absolute_priority
    - encounter_detect_chaos_bolt_self_target → defensive_cd_short
    - encounter_state_void_sentinel_alive_long → no_focus_dps (90% reduction)

  ammunae:
    - encounter_state_active → threat_hold + face_away
    - encounter_detect_consume_life_energy_cast → kick_absolute_priority
    - encounter_detect_rampant_growth_cast → defensive_cd_short + AoE_threat_for_sprout_pickup
    - encounter_detect_bloodpetal_sprout_spawn → AoE_threat_pickup
    - encounter_detect_wither_cast → kick_secondary_priority

  isiset:
    - encounter_detect_supernova_cast → verify_facing_AWAY_from_boss (default tank position satisfies)
    - encounter_state_split_phase → threat_pickup_familiar_cluster (AoE-threat)
    - encounter_detect_celestial_familiar_spawn → AoE_threat
    - encounter_state_active → face_away_from_group + threat_hold

  rajh:
    - encounter_state_active → face_away_from_group + threat_hold
    - encounter_detect_sun_strike_dot_self → defensive_cd_rotational
    - encounter_detect_sun_orb_cast → kick_primary_priority
    - encounter_detect_inferno_leap_cast → kick_primary_priority
    - encounter_detect_blessing_of_the_sun_channel → defensive_cd_major + offensive_cd_unlock
    - encounter_detect_solar_winds_path_self → kite_out
    - encounter_detect_solar_fire_patch_self_heroic → move_out

dispel_blacklist: []                      # NONE — Halls of Origination has no "do not dispel" entries

defensive_cd_priority_per_fight:
  anhuur:    [reactive_only_minor]
  ptah:      [rotation: short_CD on Raging Smash stack; no major]
  anraphet:  [rotation: short_CD on Omega Stance + warden novas; major_PRE_CD on 4th warden death (awakening)]
  setesh:    [rotation: short_CD on add cleave; major on simultaneous Wurm + Seeker portal waves]
  ammunae:   [rotation: short_CD on Rampant Growth burst]
  isiset:    [rotation: short_CD on split phase familiar pickup]
  rajh:      [rotation: short_CD on each Sun Strike cycle; major on Blessing of the Sun channel]

setesh_special_mode:
  flag: SETESH_UNTANKABLE_MODE
  description: |
    Skip boss-threat logic entirely. Bot tank state machine switches to
    add-tank-only mode. Threat priority: Wurms > Seekers > Sentinels (kite).
    Re-enables boss-threat logic on encounter exit.
  bot_implementation: NEW state machine entry — most non-standard tank fight in Cata 5-mans.
```

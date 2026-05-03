# Hour of Twilight — Tank

**Tank-specific response logic** for the bot's encounter handling. Mechanics, spell IDs, phase triggers, and add abilities are defined in `encounter.md`; this doc only encodes how the tank reacts. Class-agnostic where possible.

**Layering**:
- Cross-content tank patterns (threat priority, defensive CD pacing, taunt rules) live in `docs/roles/tank.md` (deferred to tank phase).
- Per-spec rotation / CD / threat model lives in the corresponding `docs/specs/` tank doc (all tank specs deferred to tank phase).
- This doc layers Hour of Twilight–specific deviations on top of those.

---

## Dungeon-level overview

```yaml
dungeon_tank_overview:
  total_bosses: 3
  estimated_full_clear_minutes: 15-25
  difficulty_mode: heroic_only
  threat_profile:
    arcurion: low                              # standard tank-and-spank with target-switch on tomb
    asira: low                                 # tank drag-out-of-bomb; minimal threat pressure
    benedictus: moderate                       # interrupt-heavy; Smite/Twilight Blast is tank damage
  defensive_cd_profile:
    arcurion: moderate                         # Hand of Frost spike + Torrent execute group AoE
    asira: light                                # standard melee
    benedictus: heavy                          # P2 Wave of Twilight (no Water Shell), Smite/Twilight Blast
  key_responsibilities:
    - threat_priority_per_fight
    - target_handoff_for_icy_tomb (arcurion)   # tank holds boss while DPS switches to tomb
    - drag_boss_out_of_smoke_bomb (asira)      # NEW non-default tank movement
    - kick_rotation_participation (arcurion + benedictus)
    - phase_2_transform_detection (benedictus)
    - thrall_proximity_awareness                # don't drag boss through Thrall's totems / Water Shell

```

---

## Trash — tank notes

```yaml
trash_tank:
  thrall_escort:
    pace: |
      Thrall walks/runs at scripted NPC speed — bot tank should NOT
      sprint ahead. Default tank movement may outrun Thrall; tank-bot
      logic should pace with Thrall when out of combat.
    pause_points: |
      Thrall stops periodically to monologue. Tank-bot waits at his
      position rather than continuing to next pull.
    bot_implementation_gap: |
      AltbotFollow.cpp does NOT have NPC-pace-matching mode. Default tank
      movement assumes player-driven travel; bot tank may overshoot
      Thrall's pace. WORKAROUND: rely on master/group leader to gate tank
      movement, or blacklist Hour of Twilight from bot LFD until follow
      logic gains FollowNPCTarget pace mode.

  faceless_voidstalker_packs:
    pull_pattern: |
      Caster trash with Shadow Bolt hardcasts. Tank stack mobs at melee
      range so DPS interrupts hit; rotate kicks (tank's kick counts in
      rotation).
    facing: away from group (frontal cleave on Bruisers, melee mobs)

  twilight_humanoid_packs:
    pull_pattern: |
      Skull on lead caster (kill priority); CC on second caster (Sap /
      Polymorph / Hex valid).

  twilight_assassin_engagements:
    notes: |
      Stealth-engage humanoids appear and may target ranged/healer
      first. Tank: TAUNT immediately on visual appear (Hand of
      Reckoning, Dark Command, Growl, Death Grip). Standard threat
      reset on engagement.

  corrupted_slime_packs:
    notes: |
      Slimes leave acid patches; tank kites slowly to keep group out of
      patches. Don't stack slimes on top of each other (compounding
      ground patches).
```

---

## Boss 1 — Arcurion

```yaml
boss_tank:
  boss: arcurion
  difficulty_grade: medium
  threat_intensity: low

  positioning:
    pull: |
      Position Arcurion roughly central in the canyon. AVOID parking
      under overhead ledges (Frozen Servitor Boulder reticles overlap
      there).
    facing: away from group (standard tank facing)
    movement: |
      Minimal — boss stays roughly central. Move only if Boulder
      reticle lands on tank position.

  threat_management:
    standard: maintain threat lead through Hand of Frost interrupts
    icy_tomb_target_switch: |
      When Icy Tomb (103252) spawns on Thrall, DPS bots switch target
      to the tomb. Tank continues holding Arcurion (do NOT switch tank
      target — boss is still tankable while tomb is up). Tank's threat
      lead on Arcurion should be high enough that DPS-on-tomb doesn't
      pull aggro (tomb is a destructible object, not a hostile target).

  defensive_cd_use:
    hand_of_frost_unkicked: |
      Pop short defensive (Shield Block / Barkskin / Bone Shield / Holy
      Shield equivalent) if Hand of Frost (102593) is NOT interrupted
      and you're at <60% HP.
    chains_of_frost: |
      You're rooted for 10s. Continue rotation; standard threat. Pop
      Anti-Magic Shell (DK) if HP critical (Frost school).
    torrent_execute_30pct: |
      Persistent group AoE. Tank takes Torrent damage too. Rotate
      defensives:
      - Prot Warrior: Shield Wall + Last Stand on rotation through execute
      - Prot Pally: Divine Protection + Ardent Defender + Guardian of Ancient Kings
      - Blood DK: Anti-Magic Shell (75% Magic absorb 5s) — STRONG on Frost;
        Vampiric Blood; Icebound Fortitude
      - Feral Bear / Guardian: Survival Instincts + Frenzied Regeneration

  interrupt_role:
    hand_of_frost: PRIMARY interrupt rotation participant — tank's interrupt counts
    rotation_partners: [tank, melee_dps, ranged_dps_with_kick]

  heroic_delta: |
    N/A — heroic-only fight.

  bot_anti_patterns:
    - DO NOT switch tank target to Icy Tomb (DPS handles tomb; tank holds boss)
    - DO NOT stand under canyon ledges (Boulder reticle overlap)
    - DO NOT chase strays during Boulder telegraphs — boss stays central
    - DO NOT skip Hand of Frost interrupts (heavy single-target nuke = tank death)
```

---

## Boss 2 — Asira Dawnslayer

```yaml
boss_tank:
  boss: asira_dawnslayer
  difficulty_grade: medium (movement-tank)
  threat_intensity: low

  positioning:
    pull: |
      Position Asira centrally near Thrall's Rising Fire Totem location.
      Keep her in melee range of the totem buff zone for the group.
    facing: away from group (standard)
    movement: |
      KEY RULE — DRAG ASIRA OUT OF CHOKING SMOKE BOMB. When Asira drops
      a Smoke Bomb (103790) at her feet, tank IMMEDIATELY moves Asira
      ~10y away from the cloud so she's out of it AND the cloud is left
      behind.
      The cloud blocks targeting in/out — if Asira stays inside, DPS
      cannot target her (un-targetable from outside the cloud). Tank
      drag-out is the load-bearing positional behavior of this fight.

  threat_management:
    standard: maintain threat lead; mostly auto-attack DPS
    body_block_pivot: |
      If a CASTER ALLY has Mark of Silence (102726) on them and is in
      cast range of Asira, tank may need to position so the marked ally
      has line-of-sight to a body-blocker (tank or melee). Standard play:
      melee handles body-block; tank rarely needs to body-block.

  defensive_cd_use:
    standard: |
      Light defensive use. Asira's auto-attack is moderate; no scripted
      tank-spike outside Choking Smoke Bomb DoT (if tank stays in cloud).
    blade_barrier_execute_30pct: |
      Long fight tail if shield isn't broken quickly. Tank conservative
      defensive use; rotate short CDs through extended fight.

  interrupt_role:
    none: no kick-required casts on this fight

  heroic_delta: |
    N/A — heroic-only fight.

  bot_anti_patterns:
    - DO NOT stand in Smoke Bomb cloud (5k Nature / sec)
    - DO NOT remain in cloud while Asira drops bombs at her feet (move out OR move her out)
    - DO NOT taunt-pull Asira through ranged casters (knife-line hazard)
    - DO NOT pull Asira out of Thrall's totem range during execute (DPS loses the buff)
```

---

## Boss 3 — Archbishop Benedictus

```yaml
boss_tank:
  boss: archbishop_benedictus
  difficulty_grade: HARD (mechanically dense)
  threat_intensity: moderate

  positioning:
    pull: |
      Tank holds Benedictus CENTRAL on the Wyrmrest platform. Maintain
      ≥10y from clustered ranged/healer (Shear AoE radius is 10y; tank
      doesn't take Shear cleave but tank's position relative to allies
      affects who else does).
    facing: away from group (standard)
    movement: |
      Phase 1: minimal; rotate boss only to face away from incoming
      Wave-of-Virtue path so tank doesn't get clipped between boss and
      wave.
      Phase 2: more movement-active — gap-run Wave of Twilight (no
      Water Shell), dodge orbs that Thrall isn't destroying.

  threat_management:
    standard: maintain threat through both phases; standard tank rotation
    p2_transform_target_handling: |
      At 60% HP, Twilight Epiphany transforms boss. Likely SAME NPC ID
      with model swap — tank threat reference probably preserves but
      UNVERIFIED. Conservative default: re-taunt immediately on transform
      to ensure threat lock. Bot tank logic must detect the transform
      event and re-validate target.

  defensive_cd_use:
    smite_unkicked_p1: |
      Pop short defensive if Smite isn't interrupted and you're at <60% HP.
      Holy school nuke.
    twilight_blast_unkicked_p2: |
      Same — pop short defensive on un-kicked Twilight Blast. Shadow
      school.
    wave_of_virtue_p1: |
      Stand in Thrall's Water Shell bubble — damage immunity. No CD use
      needed.
    wave_of_twilight_p2: |
      NO WATER SHELL. Tank must:
      - Prot Warrior: Shield Wall (-40% damage 12s) — strong save
      - Prot Pally: Divine Shield (immune all damage 8s, 5min CD) —
        DROPS THREAT but survives wave
      - Blood DK: Icebound Fortitude (-20% damage 12s); AMS (75% magic
        absorb 5s) — strong on Shadow
      - Feral Bear / Guardian: Survival Instincts (-50% damage 12s);
        Barkskin (-20% damage 12s)
      OR gap-run the wave (tank still has to find the gap; boss may
      need to be dropped temporarily — DPS may pick up briefly).
    long_fight_pacing: |
      Rotate major defensives across both phases. Save one major CD
      (Shield Wall / Divine Shield / Icebound Fortitude) for P2 wave.

  interrupt_role:
    smite_p1: PRIMARY interrupt rotation participant
    twilight_blast_p2: PRIMARY interrupt rotation participant
    rotation_partners: [tank, melee_dps, ranged_dps_with_kick]

  heroic_delta: |
    N/A — heroic-only fight.

  bot_anti_patterns:
    - DO NOT skip Smite / Twilight Blast interrupts (heavy school nukes)
    - DO NOT face boss into ranged group (no frontal cleave but Wave of
      Virtue/Twilight directionality may interact — UNVERIFIED)
    - DO NOT continue rotation through Wave of Twilight in P2 — gap-run
      OR pop major defensive
    - DO NOT skip phase 2 transform detection (target reference may stale;
      threat may need re-establish)
    - DO NOT stand outside Water Shell during Wave of Virtue (P1)
    - DO NOT pull boss away from Thrall's positioning during P1 (Thrall's
      Chain Lightning needs LoS to orbs near boss)
```

---

## Bot AI hooks

```yaml
ai_hooks:
  instance_wide:
    - encounter_state_thrall_walking            → pace_with_thrall_speed (bot tank match-pace)
    - encounter_detect_thrall_pause_point       → wait_at_thrall_position
    - encounter_detect_thrall_walk_resume       → resume_movement
    - encounter_detect_thrall_hp_low            → fail-condition alert (no direct tank action)

  arcurion:
    - encounter_detect_hand_of_frost_cast       → kick_rotation_participate (PRIMARY)
    - encounter_detect_hand_of_frost_unkicked   → pop_short_defensive
    - encounter_detect_icy_tomb_spawn(thrall)   → continue_holding_boss (do NOT switch target)
    - encounter_detect_icy_boulder_telegraph_self → move_out_of_reticle
    - encounter_state_boss_hp_pct <= 30         → torrent_execute_phase
    - encounter_state_torrent_active            → defensive_rotation
    - encounter_detect_thrall_bloodlust         → align_offensive_cd

  asira_dawnslayer:
    - encounter_detect_smoke_bomb_ground_place  → DRAG_BOSS_OUT (~10y away from cloud)
    - encounter_state_boss_in_cloud             → DRAG_BOSS_OUT
    - encounter_detect_mark_of_silence(ally)    → no_tank_action_required (melee body-blocks)
    - encounter_state_boss_hp_pct <= 30         → blade_barrier_phase
    - encounter_detect_thrall_rising_fire_totem → keep_boss_in_totem_range

  archbishop_benedictus:
    - encounter_detect_smite_cast(P1)           → kick_rotation_participate (PRIMARY)
    - encounter_detect_smite_unkicked           → pop_short_defensive
    - encounter_detect_twilight_blast_cast(P2)  → kick_rotation_participate (PRIMARY)
    - encounter_detect_twilight_blast_unkicked  → pop_short_defensive
    - encounter_detect_purifying_light_orb_p1   → no_tank_action (Thrall destroys 2/3, party dodges 1)
    - encounter_detect_corrupting_twilight_orb_p2 → dodge_orbs_in_path
    - encounter_detect_wave_of_virtue_p1        → stack_into_water_shell
    - encounter_detect_wave_of_twilight_p2      → pop_major_defensive_cd OR gap_run
    - encounter_state_boss_hp_pct == 60         → twilight_epiphany_imminent
    - encounter_detect_twilight_epiphany_cast   → phase_2_transition_detect; re_taunt_to_validate_threat
    - encounter_detect_thrall_twilight_prison   → discontinue_thrall_water_shell_expectations

dispel_blacklist: []                            # no dispel-blacklist entries

defensive_cd_priority_per_fight:
  arcurion:    [reactive_short_cd_on_hand_of_frost; major_cd_rotation_during_torrent_30pct]
  asira:       [reactive_only; no_scripted_spike]
  benedictus:  [reactive_short_on_smite/twilight_blast; major_save_for_p2_wave_of_twilight]

target_validation_triggers:
  arcurion:    boss_npc_id == 54590 throughout (Icy Tomb is separate spawn, not target swap for tank)
  asira:       boss_npc_id == 54968 throughout
  benedictus:  boss_npc_id == 54938 across both phases (UNVERIFIED whether transform changes ID)
```

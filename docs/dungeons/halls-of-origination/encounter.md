# Halls of Origination — Encounter Facts

**Role-neutral mechanic data** for the bot's encounter handling. Per-role response logic lives in sibling files (`healer.md`, `ranged-dps.md`, `melee-dps.md`, `tank.md`). When a role doc references a mechanic, it cites the spell ID defined here.

Spell IDs verified at `wowhead.com/cata/spell=<id>` and `wowhead.com/cata/npc=<id>` where consensus could be reached; many are flagged **UNVERIFIED** in this doc and aggregated in `docs/research/dbc-verification-checklist.md`. Strategy reconciled across 5+ sources — see `docs/research/halls-of-origination-guide-survey.md` for the audit trail.

---

## Overview

```yaml
dungeon: halls_of_origination
zone: halls_of_origination
continent: kalimdor_uldum
difficulty_modes: [normal, heroic]
level_range_normal: [84, 85]
level_range_heroic: 85
boss_count: 7
optional_bosses: []                       # all 7 required for LFD completion (original 4.0 flagged 4 optional; superseded by LFD)
bosses_in_order:
  - temple_guardian_anhuur
  - earthrager_ptah
  - anraphet                              # gated behind Vault of Lights warden gauntlet
  - setesh                                # construct order is non-canonical; bot default puts Setesh first (mana-test placement)
  - ammunae
  - isiset
  - rajh                                  # canonical "final boss" at top of pyramid
notes: |
  Non-linear hub-and-spoke layout. Central chamber (Vault of Lights) gates
  Anraphet behind a 4-elemental-warden gauntlet with continuous Stone Trogg
  trash spawn. After Anraphet, the four Constructs (Setesh, Ammunae, Isiset,
  Rajh) live in side-halls and can be cleared in any order; Rajh is the
  de-facto final boss at the pyramid summit.

  LONGEST Cata 5-man (7 bosses, 35-55 min). No instance-wide buff/debuff.
  Magic-dispel pressure on Anraphet (Nemesis Strike) and Ammunae (Wither);
  interrupt-heavy on Ammunae (Consume Life Energy), Setesh halls (Anti-Magic
  Prison), and Rajh (Sun Orb + Inferno Leap).

  Two canonical drink windows: BEFORE Vault gauntlet, BEFORE Setesh halls.
  Setesh fight is the dungeon's mana-test.
```

---

## Trash — notable mechanics

Only the trash entries with mechanics worth automating. Standard packs (basic Anubisath grunts, low-elite mobs) are pure threat-and-DPS and not enumerated.

```yaml
trash:
  # Pre-Anhuur (entry corridor)
  - mob: Anubisath Sentinel
    abilities:
      - name: melee_cleave
        target: frontal_cone
        notes: tank face away
    notes: high-HP humanoid; tank-and-spank

  - mob: Earthen Guardian (patrol)
    abilities:
      - name: standard_melee
    notes: humanoid; CC-able (Polymorph / Sap / Hex)

  # Pre-Ptah (sand corridor)
  - mob: Anubisath patrol
    notes: pull individually; standard tank-and-spank

  - mob: Quicksand                          # environmental, not killable
    abilities:
      - name: ground_hazard
        avoidable: true
        notes: visible pool on floor — stand out

  - mob: Scarab swarm
    notes: small AoE-able cluster

  # Vault of Lights gauntlet (BEFORE Anraphet) — see boss block 3
  - mob: Stone Trogg
    abilities:
      - name: melee
      - name: caster_cast (variable per pack)
        interruptible: true
        priority: MUST_INTERRUPT
    notes: |
      CONTINUOUS SPAWN while any Elemental Warden is alive. Humanoid; CC-able
      but not standard practice (AoE-burn instead).

  # Setesh halls (BEFORE Setesh) — CANONICAL MANA-DRAIN GAUNTLET
  - mob: Shadowlancer
    abilities:
      - name: shadow_cast
        interruptible: true
    notes: humanoid; primary CC target (Polymorph / Sap)

  - mob: Void Sentinel (trash form)
    abilities:
      - name: Void Barrier
        spell_id: 63710                    # UNVERIFIED — same as boss-add ID
        effect: 90% damage reduction
    notes: AoE-burn or skip-tank; trash version of boss adds

  # Construct hall trash (Isiset / Ammunae / Rajh approaches)
  - mob: Anubisath / caster trash
    abilities:
      - name: frontal_channel (caster)
        interruptible: true
    notes: turn away from group; CC-able humanoid
```

### Trash skip notes

- **No canonical skip** of any boss; all 7 are required for LFD completion.
- Some Setesh-halls packs can be wall-hugged or stealth-skipped; not standard for LFD.
- **Drink windows**:
  - Top of Vault of Lights stair landing — before warden gauntlet
  - Top of Setesh halls stair landing — after Anraphet, before Setesh
  - Between Constructs (any order, depending on pull cadence)

### CC priorities

- **Polymorph / Hex / Sap** valid on Anubisath / Earthen Guardian / Stone Trogg / Shadowlancer humanoids
- **Banish** valid on Void-themed adds if applicable (UNVERIFIED Demon vs Aberration classification)
- **Earthen Guardian** — Bind Elemental works on the elemental subset
- Void Sentinels (trash form) — generally NOT CC'd; AoE-burn

---

## Boss 1 — Temple Guardian Anhuur

```yaml
boss: temple_guardian_anhuur
npc_id: 39425
location: Chamber of Prophecy (entry hall, two-tier platform with side pits)
hp_pct_phase_triggers: [66, 33]
notes: |
  Three-active-phase boss with two scripted shield interrupts at 66% and 33%
  HP. Each shield phase forces 2 players (default: 2 DPS) to drop into the
  side pits, kill Pit Vipers, then activate Beacons of Light to drop the
  shield. Non-trivial bot navigation; flag for bot_implementation.

phases:
  - id: 1
    name: phase_1
    trigger: pull
    end_condition: hp_pct == 66
    description: tank-and-spank with Burning Light AoEs + Divine Reckoning Magic dispels

  - id: 2
    name: shield_phase_1
    trigger: hp_pct == 66
    end_condition: both Beacons of Light activated (shield drops)
    description: |
      Boss casts Shield of Light (immune to damage AND interrupts) and
      Reverberating Hymn (escalating raid AoE). Two players drop into side
      pits, clear Pit Vipers, channel Beacons of Light (8s normal / 10s
      heroic). Both beacons must complete to drop shield. Healer + remaining
      players topside survive Hymn channel.

  - id: 3
    name: phase_2
    trigger: shield drops
    end_condition: hp_pct == 33
    description: same as phase 1

  - id: 4
    name: shield_phase_2
    trigger: hp_pct == 33
    end_condition: both Beacons of Light activated
    description: same as shield_phase_1

  - id: 5
    name: phase_3
    trigger: shield_phase_2 ends
    end_condition: hp_pct == 0
    description: final burn; same mechanics as phase_1

mechanics:
  - name: Burning Light
    spell_id: 75117
    cast_by: temple_guardian_anhuur
    cast_kind: ground_zone
    target: ground_random
    duration_ms: 10000
    radius: 4
    effect: 24-26k Holy/sec for 10s (4y radius)
    interruptible: false
    dispel_type: none
    avoidable: true
    avoidance: move out of blue ground patch
    priority: MOVE_OUT

  - name: Divine Reckoning
    spell_id: 75592
    cast_by: temple_guardian_anhuur
    cast_kind: debuff_application
    target: ally_random
    delay_ms: 8000                          # explodes after 8s
    radius: 7
    effect: |
      Magic debuff on random player; explodes after 8s for 42-47k Holy AoE
      to debuffed player + nearby allies (7y); HEALS BOSS for 5x damage dealt.
    interruptible: false
    dispel_type: magic                      # CRITICAL — dispel before detonation
    avoidable: true
    avoidance: |
      Either dispel Magic OR debuffed player MOVES OUT of group before
      detonation. Failure = boss heal + group damage.
    priority: MAGIC_DISPEL_HIGH

  - name: Shield of Light
    spell_id: 80747
    cast_by: temple_guardian_anhuur
    cast_kind: scripted_phase
    target: self
    trigger: hp_pct == 66 OR hp_pct == 33
    effect: boss IMMUNE to damage and interrupts; triggers Reverberating Hymn
    interruptible: false
    dispel_type: none
    bot_signal: PHASE_TRANSITION_DOOR_EVENT

  - name: Reverberating Hymn
    spell_id: 75322
    cast_by: temple_guardian_anhuur
    cast_kind: channel
    target: pbaoe_self
    duration_ms: until_shield_drops
    effect: 400 Holy / 4s baseline, ESCALATES over channel
    interruptible: false                    # interrupt-immune during shield
    dispel_type: none
    avoidable: false
    priority: AOE_HEAL_PRESSURE
    notes: |
      Damage stacks/escalates over time. Drives the urgency of beacon
      activation. Healer pops AoE heal cooldown topside; pit-runners take
      damage but heal-recover after.

  - name: Pit Viper Poison-tipped Fangs
    spell_id: 74538
    cast_by: pit_viper (NPC 39444)
    cast_kind: melee
    target: closest_player
    effect: 3.7-4.3k Nature / 3s for 15s (Poison DoT)
    interruptible: false
    dispel_type: poison                     # if hit; usually just ignore on lever-runner

  - name: Beacon of Light (interactable)
    spell_id: UNVERIFIED                    # interactable object cast
    cast_by: player (lever-runner)
    cast_kind: interaction_channel
    target: ground_object_lever
    cast_time_ms_normal: 8000
    cast_time_ms_heroic: 10000
    effect: drops Shield of Light when BOTH beacons (east + west) are activated
    bot_signal: COMPLETION_REQUIRED_TO_RESUME_DPS

# Adds (Pit Vipers — spawn during shield phase)
adds:
  - name: Pit Viper
    npc_id: 39444
    count: 6+ per pit per shield phase
    locked_to: side pits (east + west)
    abilities:
      - name: Poison-tipped Fangs (74538)
    kill_priority: HIGH for pit-runners (block beacon channel)
    notes: lever-runners must clear before channeling Beacon

positioning:
  default: |
    Phase 1 / 2 / 3 (active): tank centrally on upper platform, group at melee
    range OR ranged spread per spec. Move OUT of Burning Light blue ground.
    Divine Reckoning target moves OUT of group if undispelled.

    Shield phases: 2 DPS drop into pits (one east, one west); kill Pit Vipers;
    channel Beacon of Light. Healer + tank + 1 DPS remain topside.
  tank_facing: away from group
  los_or_lookaway: none
  pit_assignment: |
    Bot logic: 2 designated DPS bots assigned "pit_runner" role at pull;
    each takes east or west pit on shield trigger. Healer NEVER drops
    (per Disagreement #8 default). Tank stays topside.
  bot_implementation_flag: |
    Drop-into-pit navigation is non-standard. Bot pit-runners must:
    1. Detect Shield of Light cast
    2. Path to nearest pit edge and drop
    3. Engage Pit Vipers (kill all in path to lever)
    4. Channel Beacon of Light (8s normal / 10s heroic) — UNINTERRUPTIBLE BY MOB DAMAGE? UNVERIFIED
    5. Path back up to platform after both beacons activated

interrupt_priority:
  - Pit_Viper_casts (if any) — lever-runners
  # NOTE: Reverberating Hymn is INTERRUPT-IMMUNE during shield. Bot must NOT waste kicks on it.

dispel_priority:
  - { spell_id: 75592, name: Divine Reckoning, dispel_type: magic, priority: HIGH }

heroic_delta: |
  - Beacon cast time 8s → 10s (longer pit exposure for lever-runners)
  - More Pit Vipers per side (multiple swarms; possibly respawn during channel — UNVERIFIED)
  - Reverberating Hymn damage escalates harder; shield-down urgency tighter

bot_role_summary:
  tank: hold boss centrally on platform; face away; do NOT drop into pit (default)
  healer: stay TOPSIDE; raid-heal Reverberating Hymn channel; dispel Divine Reckoning Magic on affected player
  melee: 1 of 2 designated pit-runners — drop into pit on shield, kill vipers, channel beacon
  ranged: 1 of 2 designated pit-runners — same as melee runner; or stay topside if not assigned
```

---

## Boss 2 — Earthrager Ptah

```yaml
boss: earthrager_ptah
npc_id: 39428
location: sand corridor chamber, after Anhuur. Wide sandy room with idle camels around perimeter.
hp_pct_phase_triggers: [50]
notes: |
  Two-phase: tank-and-spank with ground hazards → 50% Tumultuous Earthstorm
  burrow phase with adds → boss reforms and burns down. Camels around the
  room are mountable for 50% movement speed (canonical "camel race"); bot
  default ignores camels.

phases:
  - id: 1
    name: phase_1
    trigger: pull
    end_condition: hp_pct == 50
    description: tank-and-spank with Earth Spike + Quicksand + Flame Bolt + Raging Smash cleave

  - id: 2
    name: tumultuous_earthstorm
    trigger: hp_pct == 50
    end_condition: all adds dead
    description: |
      Boss disperses (untargetable). Dustbone Horror (2) + Jeweled Scarab (8)
      adds spawn. Heroic-only sand vortex tornado roams the room. AoE adds
      while moving away from tornado.

  - id: 3
    name: phase_2
    trigger: end of phase 2
    end_condition: hp_pct == 0
    description: boss reforms; resume phase_1 mechanics; final burn

mechanics:
  - name: Flame Bolt
    spell_id: 75540
    cast_by: earthrager_ptah
    cast_kind: channel
    target: ally_random
    duration_ms: 5000
    target_count_normal: 2
    target_count_heroic: 3
    effect: fire damage channel on 2 (or 3 heroic) random players
    interruptible: true                     # 5s channel; long enough to kick
    dispel_type: none
    priority: KICK_AVAILABLE

  - name: Earth Spike
    spell_id: 75339
    cast_by: earthrager_ptah
    cast_kind: ground_delay
    target: ground_random
    delay_ms: 4000
    radius: 4
    effect: nature damage + knockup (4y radius) after 4s
    interruptible: false
    dispel_type: none
    avoidable: true
    avoidance: move out of telegraphed ground spike

  - name: Raging Smash
    spell_id: 83650
    cast_by: earthrager_ptah
    cast_kind: melee
    target: tank_plus_2_nearest
    effect: 150% melee damage to current target + 2 additional enemies (cleave)
    interruptible: false
    dispel_type: none
    avoidable: partial
    avoidance: melee positions side/rear (out of cleave arc)

  - name: Quicksand
    spell_id: 75547
    cast_by: environmental
    cast_kind: passive_pool
    target: ground_random_near_players
    radius: 7
    effect: nature DoT every 2s + 50% movement slow
    interruptible: false
    dispel_type: none
    avoidable: true
    avoidance: move out of pool

  - name: Tumultuous Earthstorm
    spell_id: UNVERIFIED                    # scripted phase event
    cast_by: earthrager_ptah
    cast_kind: scripted_phase
    target: self
    trigger: hp_pct == 50
    effect: boss disperses; adds spawn; sand vortex (heroic)
    bot_signal: BURROW_PHASE_AOE_MODE

  - name: Smash (Dustbone Horror add)
    spell_id: 75453
    cast_by: dustbone_horror (NPC 40808)
    cast_kind: melee
    target: closest_player
    effect: 125% melee damage on target
    interruptible: false

# Adds (50% HP burrow phase)
adds:
  - name: Dustbone Horror
    npc_id: 40808
    count: 2
    spawn_trigger: hp_pct == 50
    abilities:
      - name: Smash (75453)
    notes: tank picks up; high-HP add

  - name: Jeweled Scarab
    npc_id: 40458
    count: 8
    spawn_trigger: hp_pct == 50
    abilities:
      - name: melee
    notes: low-HP cluster; AoE-burn priority

positioning:
  default: |
    Tank central, facing away. Melee side/rear (cleave). Ranged max range,
    spread for Earth Spike multi-target. All players move out of Quicksand
    pools.
  tank_facing: away from group (Raging Smash cleave)
  los_or_lookaway: none
  burrow_phase: |
    During Tumultuous Earthstorm: stack near Dustbone Horror cluster for AoE
    cleave; ranged kites scarabs to tank; on heroic, KITE the sand vortex
    tornado around the room.

interrupt_priority:
  - Flame_Bolt (75540) — kick on rotation if available

dispel_priority: []                       # no dispels

heroic_delta: |
  - Flame Bolt targets 3 (vs 2)
  - Sand vortex tornado spawns during Tumultuous Earthstorm; kite-out
  - More Quicksand pools
  - Adds hit harder

bot_role_summary:
  tank: face boss away; rotate boss during Quicksand; pick up Dustbone Horrors on burrow-phase spawn
  healer: pre-shield Flame Bolt targets if predictable; standard tank healing; AoE burrow phase
  melee: side/rear positioning; switch to AoE on burrow phase
  ranged: max range; spread; AoE Jeweled Scarabs on burrow phase; kite sand vortex on heroic
```

---

## Boss 3 — Vault of Lights warden gauntlet + Anraphet

```yaml
boss_complex: vault_of_lights_anraphet
location: Vault of Lights chamber (central pyramid hall, after sand corridor)
notes: |
  TWO COMPOSED ENCOUNTERS in one chamber. Cannot leave / drink between them.
  Phase A: 4 elemental wardens (mini-bosses) on side platforms, with
  continuous Stone Trogg trash spawn. Wardens killed in any order. After
  4th warden dies, Anraphet awakens automatically.
  Phase B: Anraphet engages with unavoidable room-AoE awakening.

# ============================================================
# PHASE A — Elemental Warden gauntlet
# ============================================================
warden_gauntlet:
  duration: until all 4 wardens dead
  trash_spawn: continuous Stone Trogg waves while ANY warden alive

  wardens:
    - name: Flame Warden
      npc_id: UNVERIFIED
      key_ability: Raging Inferno (fire nova; spread out)
      strategy: stand at range; spread for nova

    - name: Water Warden
      npc_id: UNVERIFIED
      key_ability: Bubble (traps player; group breaks bubble)
      strategy: focus-break trapped-player bubble

    - name: Air Warden
      npc_id: UNVERIFIED
      key_ability: Whirlwind tornadoes (kite-out)
      strategy: kite-out tornado paths

    - name: Earth Warden
      npc_id: UNVERIFIED
      key_ability: Knockback nova (collective throwback)
      strategy: spread to avoid group throw

  trash:
    - name: Stone Trogg
      type: humanoid
      count: continuous
      cc_valid: yes (Polymorph / Sap / Hex)
      ai_priority: AoE-burn near tank
      notes: |
        Tank holds AoE threat on troggs while focus-DPS on active warden.
        Healer raid-heals through cleave; mana drains over the gauntlet.

  drink_window: TOP OF STAIR LANDING — BEFORE entering chamber. NO drink during gauntlet.

  warden_kill_order: |
    Any order. Bot default: closest-to-tank for AoE-cleave efficiency.
    See survey Disagreement #7 — guides do not agree on optimal order.

  trigger_anraphet: 4th warden death → Anraphet awakens (UNAVOIDABLE)

# ============================================================
# PHASE B — Anraphet
# ============================================================
boss: anraphet
npc_id: 39788
hp_pct_phase_triggers: []
notes: |
  Single phase with recurring Nemesis Strike + Alpha Beams + Crumbling Ruin
  cycle. Soft-enrage via Crumbling Ruin (max-HP reduction stacks). Heroic-
  only persistent void zones from Alpha Beams force movement-puzzle as fight
  progresses.

phases:
  - id: 1
    name: awakening
    trigger: 4th warden dies
    end_condition: awakening AoE resolves (~3s)
    description: |
      UNAVOIDABLE room-wide AoE. Kills remaining troggs. Damages party for
      ~30-50% group HP. Healer pre-shields; tank uses defensive CD.

  - id: 2
    name: regular
    trigger: end of awakening
    end_condition: hp_pct == 0
    description: |
      Tank-and-spank with Nemesis Strike (tank DoT — Magic dispel) +
      Alpha Beams (random-target shadow AoE; heroic-only persistent void
      zones) + Crumbling Ruin (max-HP stack soft-enrage) + occasional
      Omega Stance (8s pulsing AoE channel).

mechanics:
  - name: Nemesis Strike
    spell_id: 75604
    cast_by: anraphet
    cast_kind: melee
    target: tank
    effect: 75% melee + recurring shadow DoT on tank
    interruptible: false
    dispel_type: magic                       # CRITICAL — canonical dispel call
    priority: MAGIC_DISPEL_HIGH

  - name: Alpha Beams
    spell_id: 76184
    cast_by: anraphet
    cast_kind: channel
    target: ally_random
    duration_ms: 3000
    radius: 5
    tick_ms: 500
    effect: shadow damage every 0.5s for 3s in 5y radius
    interruptible: false
    dispel_type: none
    avoidable: true
    avoidance: move out of beam path immediately
    heroic_modifier: persistent void zone left on ground for remainder of fight

  - name: Omega Stance
    spell_id: 75623
    cast_by: anraphet
    cast_kind: channel
    target: pbaoe_self
    duration_ms: 8000
    tick_ms: 1000
    effect: shadow AoE damage every 1s for 8s; reduces movement speed 50% for 4s
    interruptible: false
    dispel_type: none
    avoidable: false
    priority: AOE_HEAL_COOLDOWN

  - name: Crumbling Ruin
    spell_id: 75609
    cast_by: anraphet
    cast_kind: scripted_application
    target: party_all
    trigger: post-Alpha-Beams AND post-Omega-Stance
    effect: |
      Reduces max HP by 10% per stack, up to 10 stacks. Soft-enrage —
      drives DPS check.
    interruptible: false
    dispel_type: none

  - name: Awakening AoE
    spell_id: UNVERIFIED                    # scripted unavoidable burst
    cast_by: anraphet
    cast_kind: scripted_phase
    target: pbaoe_room
    trigger: 4th warden dies
    effect: ~30-50% party HP burst (unavoidable)

# Adds (none on Anraphet itself)
adds: []

positioning:
  default: |
    Tank central; ranged + healer max range, spread to avoid Alpha Beam
    multi-hit. Heroic: rotate group around chamber as void zones accumulate.
  tank_facing: away from group (standard)
  los_or_lookaway: none
  heroic_movement: void zones from Alpha Beams persist; floor fills over time

interrupt_priority: []                    # no kick-required casts on Anraphet

dispel_priority:
  - { spell_id: 75604, name: Nemesis Strike, dispel_type: magic, priority: HIGH }

heroic_delta: |
  - Alpha Beams persistent void zones (floor-fill movement-puzzle)
  - Omega Stance hits harder
  - Crumbling Ruin stacks faster (DPS check tightens)
  - Stone Trogg packs spawn faster during gauntlet (more trash pressure)

bot_role_summary:
  tank: AoE threat through warden gauntlet; standard tanking on Anraphet; defensive CD on awakening
  healer: dispel Nemesis Strike Magic on tank; AoE heal cooldown on Omega Stance; pre-shield awakening burst
  melee: AoE on troggs during gauntlet; standard rear positioning on Anraphet; move out of Alpha Beams
  ranged: AoE / cleave on troggs; max range on Anraphet; movement-priority for void zones (heroic)
```

---

## Boss 4 — Setesh, Construct of Destruction

```yaml
boss: setesh
npc_id: 39732
location: Seat of Destruction (chaos-themed Construct hall)
hp_pct_phase_triggers: []
notes: |
  CANONICAL DUNGEON MANA-TEST. Boss is UNTANKABLE — no threat table; attacks
  random players. Tank role is portal-add management. Anti-Magic Prison from
  Void Seekers is the primary kick target (blocks magical healing on a
  player). Heroic: portals do NOT auto-close — DPS must destroy them.

phases:
  - id: 1
    name: regular
    trigger: pull
    end_condition: hp_pct == 0
    description: |
      Setesh drifts and attacks random players (untankable). Periodic Chaos
      Portals open and spawn Void Sentinel + Void Wurms + Void Seeker. Reign
      of Chaos / Chaos Blast ground-zones force movement. Seed of Chaos
      grants +100% damage buff on walk-through. Continuous fight; constant
      mana drain.

mechanics:
  - name: Chaos Bolt
    spell_id: 50796
    cast_by: setesh
    cast_kind: cast
    target: ally_random
    cast_time_ms: 2000
    effect: 10-13k UNRESISTABLE shadow damage on random target
    interruptible: false                     # not interruptible per most reports
    dispel_type: none

  - name: Reign of Chaos
    spell_id: 77030
    cast_by: setesh
    cast_kind: scripted
    target: ground_zones
    effect: spawns void bubbles dealing 7.8k/sec to anyone inside
    interruptible: false
    dispel_type: none
    avoidable: true
    avoidance: move out of bubbles

  - name: Chaos Blast
    spell_id: 76681
    cast_by: setesh
    cast_kind: ground_delay
    target: ground_random
    delay_ms: 5000
    effect: lands after 5s; persistent void zone; move-out
    interruptible: false
    dispel_type: none
    avoidable: true

  - name: Seed of Chaos
    spell_id: 76870
    cast_by: setesh
    cast_kind: passive_ground
    target: ground_random
    effect: detonates on player approach; +100% damage buff for 15s
    interruptible: false
    dispel_type: none
    bot_signal: WALK_INTO_DELIBERATELY        # only positive ground-effect in dungeon
    notes: per Disagreement #4 — bot walks into Seed when DPS phase + Seed exists

  - name: Summon Chaos Portal
    spell_id: UNVERIFIED                    # scripted summon event
    cast_by: setesh
    cast_kind: scripted
    target: ground_random
    cadence_normal: ~30s (auto-close after add wave)
    cadence_heroic: ~30s (PERSISTENT — must be destroyed)
    effect: opens portal; Sentinel + Wurms (pair) + Seeker spawn over ~6s
    bot_signal: ADD_PORTAL_OPEN

# Adds (per portal)
adds:
  - name: Void Sentinel
    npc_id: 41208
    count: 1 per portal
    abilities:
      - name: Void Barrier (63710) — 90% damage reduction
      - name: Charged Fists (77238) — physical damage + attack speed buff
    ai_priority: |
      DPS-SKIP unless no other targets. Tank kites with roots/slows
      (Chains of Ice, Frost Trap-style abilities).

  - name: Void Seeker
    npc_id: 41371
    count: 1 per portal
    abilities:
      - name: Anti-Magic Prison (76903) — traps target, prevents magical healing
    ai_priority: |
      KICK PRIORITY ABSOLUTE. Anti-Magic Prison cast must be interrupted.
      If lands on healer or tank, that player loses healing for the duration.

  - name: Void Wurm
    npc_id: 41374
    count: 2 per portal
    abilities:
      - name: melee
    ai_priority: AoE-burn target (low HP)

positioning:
  default: |
    Mobile fight. Setesh drifts. Tank kites adds around the room. DPS
    target-swaps between portals (heroic) and adds.
  tank_facing: away from active adds (whatever the tank is currently kiting)
  los_or_lookaway: none
  heroic_routing: portal positions force re-routing as fight progresses

interrupt_priority:
  - Anti_Magic_Prison (Void Seeker, 76903) — MUST_INTERRUPT every cast (rotation)

dispel_priority: []                       # Anti-Magic Prison dispellability UNVERIFIED — bot does NOT attempt magic dispel

heroic_delta: |
  - PORTALS DO NOT AUTO-CLOSE — DPS must destroy them (single biggest heroic change)
  - Faster portal cycle / more adds per portal
  - Anti-Magic Prison casts more frequent
  - Mana drain pressure increases significantly

bot_role_summary:
  tank: |
    UNTANKABLE BOSS — switch to add-tank mode. Kite Sentinels (root/slow);
    threat-hold Wurms; intercept Seekers. DK tank: Chains of Ice spam keeps
    adds slowed.
    bot_implementation_flag: most non-standard tank fight in Cata 5-mans.
  healer: |
    CANONICAL DUNGEON MANA-TEST. Pop Mana Tide / Innervate / Hymn of Hope
    here. Constant pressure from Anti-Magic Prison + add cleave + Chaos
    Bolt random hits.
    bot_implementation_flag: dungeon mana-stress check.
  melee: portal priority (heroic); AoE Wurms; interrupt Seekers; walk-into Seed of Chaos
  ranged: same priority; portal kills on heroic; Seed of Chaos walk-into for damage buff
```

---

## Boss 5 — Ammunae, Construct of Life

```yaml
boss: ammunae
npc_id: 39731
location: Seat of Life (plant/nature-themed Construct hall)
hp_pct_phase_triggers: []
notes: |
  Single-phase with continuous add management. Seedling Pods spawn passively
  and buff the boss; convert to Bloodpetal Sprouts on Rampant Growth (boss
  energy at 100). Wither debuff dispels (Magic). Consume Life Energy must
  be interrupted (boss energy gain + target damage).

phases:
  - id: 1
    name: regular
    trigger: pull
    end_condition: hp_pct == 0
    description: |
      Ammunae periodically casts Wither (Magic dispel on player) and Consume
      Life Energy (channel — interrupt to deny boss energy). Seedling Pods
      spawn passively in random locations; pods buff boss damage/haste while
      alive. At 100 boss energy, Rampant Growth converts all pods to
      Bloodpetal Sprouts (active threat adds). Spores spawn periodically;
      kill, then move out of poison cloud.

mechanics:
  - name: Wither
    spell_id: 76043
    cast_by: ammunae
    cast_kind: cast
    target: ally_random
    cast_time_ms: 2000
    duration_ms: 10000
    effect: -60% attack/cast/movement speed for 10s
    interruptible: true                     # interruptible per most guides
    dispel_type: magic
    priority: MAGIC_DISPEL_HIGH OR INTERRUPT

  - name: Consume Life Energy
    spell_id: 75665
    cast_by: ammunae
    cast_kind: channel
    target: ally_random
    duration_ms: 4000
    effect: |
      8k Nature/sec + stun on target; boss gains 12 energy/sec while
      channeling. MUST INTERRUPT.
    interruptible: true
    dispel_type: none
    priority: MUST_INTERRUPT

  - name: Rampant Growth
    spell_id: 75790
    cast_by: ammunae
    cast_kind: scripted
    target: pbaoe_room + pods
    trigger: boss energy == 100
    effect: |
      Converts ALL Seedling Pods to Bloodpetal Sprouts (active threat adds);
      29-34k Nature AoE; soft-enrage if uncontrolled.
    interruptible: false
    dispel_type: none

  - name: Noxious Spores (cloud)
    spell_id: 75702
    cast_by: spore (NPC 16286) on death
    cast_kind: passive_ground
    target: ground_at_spore_death
    radius: 6
    duration_ms: 30000
    effect: 10-12k Nature / 1.5s
    interruptible: false
    dispel_type: none
    avoidable: true
    avoidance: move out of cloud

  - name: Pod Energize (passive boss buff)
    spell_id: UNVERIFIED                    # passive aura from each living pod
    cast_by: seedling_pod (NPC 51329)
    cast_kind: passive_aura
    target: ammunae
    effect: +5% damage, +30% melee haste per living pod
    interruptible: false
    dispel_type: none
    bot_signal: KILL_PODS_PRIORITY_HIGH

# Adds
adds:
  - name: Seedling Pod
    npc_id: 51329
    count: continuous spawn (every ~14s, random location 14y from boss)
    abilities:
      - name: passive Pod Energize buff on Ammunae
    ai_priority: KILL FAST (ranged-DPS focus)

  - name: Bloodpetal Blossom
    npc_id: 40622
    count: spawned by Rampant Growth (one per surviving pod)
    abilities:
      - name: 150% melee + bleed (3.6-4.7k/2s)
    ai_priority: tank picks up; AoE-burn

  - name: Spore
    npc_id: 16286
    count: periodic
    abilities:
      - name: passive movement (slow drift)
      - name: Noxious Spores (75702) on death — 30s ground cloud
    ai_priority: kill, then move out of cloud

positioning:
  default: |
    Tank central; melee on boss rear; ranged max range with focus on pods +
    spores; healer free-positioning. All players move out of Noxious Spore
    clouds.
  tank_facing: away from group
  los_or_lookaway: none

interrupt_priority:
  - Consume_Life_Energy (75665) — MUST_INTERRUPT every cast
  - Wither (76043) — interrupt OR dispel Magic afterward

dispel_priority:
  - { spell_id: 76043, name: Wither, dispel_type: magic, priority: HIGH }

heroic_delta: |
  - Pods spawn more frequently
  - Pods may actively heal boss (UNVERIFIED — see Disagreement #3)
  - Bloodpetal Sprouts hit harder

bot_role_summary:
  tank: standard tanking + Bloodpetal Sprout pickup on Rampant Growth conversion
  healer: dispel Wither Magic on affected player; tank-heal CLE channel target; raid-heal Rampant Growth burst
  melee: standard rear positioning on boss; switch to Sprouts on conversion; help with Spore kills
  ranged: KILL SEEDLING PODS on spawn (priority HIGH); kill Spores; move out of clouds
```

---

## Boss 6 — Isiset, Construct of Magic

```yaml
boss: isiset
npc_id: 39587
location: Seat of Magic (arcane-themed Construct hall)
hp_pct_phase_triggers: [66, 33]
notes: |
  Three-phase split mechanic: at 66% Isiset splits into 3 Astral Familiars
  (mirror images), at 33% she splits into 2 (the survivors). Killing one
  familiar disables that ability but EMPOWERS the others. Supernova is the
  signature face-away mechanic.

phases:
  - id: 1
    name: phase_1
    trigger: pull
    end_condition: hp_pct == 66
    description: |
      Isiset solo. Supernova face-away + Astral Rain channel + Celestial Call
      (1 familiar) + Veil of Sky shield.

  - id: 2
    name: split_phase_1
    trigger: hp_pct == 66
    end_condition: 1 familiar killed; Isiset reforms
    description: |
      Isiset splits into 3 Astral Familiars (Astral Rain / Celestial Call /
      Veil of Sky variants). Kill ONE — Isiset reforms with that ability
      permanently disabled; remaining 2 abilities EMPOWERED.
      Recommended kill order: Astral Rain first.

  - id: 3
    name: phase_2
    trigger: split_phase_1 ends
    end_condition: hp_pct == 33
    description: same as phase_1, with 1 ability disabled and 1 ability empowered

  - id: 4
    name: split_phase_2
    trigger: hp_pct == 33
    end_condition: 1 familiar killed; Isiset reforms
    description: |
      Isiset splits into 2 Astral Familiars (the survivors). Kill ONE.
      Recommended kill order: Celestial Call (if alive) over Veil of Sky.

  - id: 5
    name: phase_3
    trigger: split_phase_2 ends
    end_condition: hp_pct == 0
    description: final burn with one ability remaining

mechanics:
  - name: Supernova
    spell_id: 74136
    cast_by: isiset
    cast_kind: cast
    target: pbaoe_self
    cast_time_ms: 2500
    radius: 50                              # estimate; UNVERIFIED
    effect: 24-26k Arcane AoE; FACING players disoriented 4s
    interruptible: false                     # UNVERIFIED — likely not
    dispel_type: none
    avoidable: partial
    avoidance: FACE AWAY from boss when cast resolves
    priority: FACE_AWAY_OVERRIDE

  - name: Astral Rain
    spell_id: 74135
    cast_by: isiset OR astral_rain_familiar
    cast_kind: channel
    target: ground_random
    duration_ms: 8000
    tick_ms: 3000
    effect: |
      Star-shower; arcane every 3s for 8s. Damage scales:
      base / 2-survivor / 3-survivor: 5.4-6.3k → 6.6-7.7k → 9.0-10.5k

  - name: Celestial Call
    spell_id: 74364
    cast_by: isiset OR celestial_call_familiar
    cast_kind: scripted
    target: ground
    effect: |
      Summons Celestial Familiar adds (small). Count scales by surviving
      familiars: 1 → 2 → 3.

  - name: Veil of Sky
    spell_id: 74133
    cast_by: isiset OR veil_of_sky_familiar
    cast_kind: scripted_aura
    target: self
    effect: |
      Damage shield + spell reflect. Absorb scales 130k → 260k → 390k;
      reflect chance 20% → 40% → 60%.
    dispel_type: magic                      # SPELLSTEAL / PURGE / MASS DISPEL
    priority: SPELLSTEAL_HIGH for Mage / Shaman / Priest

  - name: Arcane Barrage (Celestial Familiar)
    spell_id: 74374
    cast_by: celestial_familiar (small add)
    cast_kind: cast
    target: ally_random
    effect: 9.5-10.5k arcane
    interruptible: true

# Adds (mirror images on split phases)
mirror_image_adds:
  - name: Astral Familiar — Astral Rain
    npc_id: 39720
    empowers_when_others_die: doubles/triples Astral Rain damage
    kill_priority_recommendation: FIRST (default)

  - name: Astral Familiar — Celestial Call
    npc_id: 39721
    empowers_when_others_die: more Celestial Familiars per cast
    kill_priority_recommendation: SECOND

  - name: Astral Familiar — Veil of Sky
    npc_id: 39722
    empowers_when_others_die: stronger absorb + reflect chance
    kill_priority_recommendation: THIRD (Spellsteal/Purge/Mass Dispel target if encountered)

celestial_familiar_adds:
  - name: Celestial Familiar
    npc_id: UNVERIFIED
    abilities:
      - name: Arcane Barrage (74374)
    notes: small interruptible-cast adds; AoE-burn or focus down

positioning:
  default: |
    Tank holds Isiset central, FACING AWAY from group (Supernova face-away).
    All players turn AWAY from Isiset on Supernova cast. Ranged + healer max
    range, spread for Astral Rain.
  tank_facing: away from group
  los_or_lookaway: |
    LOOKAWAY MECHANIC — all players face AWAY from boss on Supernova cast.
    bot_implementation_flag: bot must temporarily reverse facing mid-combat.
  split_phase_focus: |
    Single-target focus-fire on chosen Astral Familiar (kill priority
    Astral Rain → Celestial Call → Veil of Sky).

interrupt_priority:
  - Arcane_Barrage (Celestial Familiar, 74374) — kick if available

dispel_priority:
  - { spell_id: 74133, name: Veil of Sky, dispel_type: magic_spellsteal, priority: HIGH for Mage/Shaman/Priest }

heroic_delta: |
  - All damage values higher
  - Familiar HP higher (split phase takes longer)
  - No novel mechanics

bot_role_summary:
  tank: face boss away; tank-and-spank; hold familiars on split (closest-to-tank)
  healer: cooldown for Astral Rain channel; standard tank healing; turn-away on Supernova
  melee: turn-away on Supernova cast; positional behind boss; focus-fire familiar in split
  ranged: face-away on Supernova; max range; focus-fire familiar; Spellsteal/Purge Veil if Mage/Shaman/Priest
```

---

## Boss 7 — Rajh, Construct of the Sun

```yaml
boss: rajh
npc_id: 39378
location: Seat of Radiance (top of pyramid; de-facto final boss)
hp_pct_phase_triggers: []
notes: |
  Energy-resource-driven encounter. Boss starts at 100 Solar Energy, spends
  20/cast on abilities. At <10 energy, triggers Blessing of the Sun
  (recharge phase) — boss takes increased damage AND players gain +100%
  damage buff for 8s. Canonical "burn window" of the fight.

phases:
  - id: 1
    name: active_phase
    trigger: pull
    end_condition: boss_energy < 10
    description: |
      Rajh casts Sun Strike / Sun Orb / Inferno Leap / Solar Winds at
      20-energy each. Tank-and-spank with kick rotation on Sun Orb +
      Inferno Leap; ranged + melee kite-out Solar Winds tornado.

  - id: 2
    name: blessing_of_the_sun
    trigger: boss_energy < 10
    end_condition: 8s channel completes
    description: |
      Recharge phase. Boss channels in center; takes increased damage; AoE
      damage on group; +100% damage buff for ALL PLAYERS for 8s. Burn-down
      window — save offensive CDs (Bloodlust if not used; Berserk; Avenging
      Wrath; Combustion).

  - id: 3
    name: post_recharge
    trigger: blessing_of_the_sun ends
    end_condition: hp_pct == 0 OR boss_energy < 10 again
    description: resume active_phase mechanics; another Blessing if not killed

mechanics:
  - name: Sun Strike
    spell_id: 73872
    cast_by: rajh
    cast_kind: scripted
    target: pbaoe_room (50y)
    effect: |
      18-21k Fire AoE to ALL players within 50y. Applies UNDISPELLABLE tank
      DoT.
    interruptible: false
    dispel_type: none                        # UNDISPELLABLE
    avoidable: false

  - name: Summon Sun Orb
    spell_id: 80352
    cast_by: rajh
    cast_kind: channel
    target: ground_random
    cast_time_ms: 3000
    radius: 13
    effect: summons orb; AoE burst on landing within 13y
    interruptible: true                      # PRIMARY KICK
    priority: MUST_INTERRUPT

  - name: Inferno Leap
    spell_id: 87653
    cast_by: rajh
    cast_kind: telegraph
    target: ally_random
    cast_time_ms: 3000
    radius: 10
    effect: |
      Boss leaps to random player; fatal fire damage in 10y on landing.
      Target gains +50% movement speed to escape.
    interruptible: true                      # PRIMARY KICK
    priority: MUST_INTERRUPT

  - name: Solar Winds
    spell_id: 74108
    cast_by: rajh
    cast_kind: scripted
    target: ground_movement_pattern
    radius: 4
    effect: roving fire tornado(s); 4y radius; kite-out
    interruptible: false
    dispel_type: none
    avoidable: true
    avoidance: kite-out tornado paths

  - name: Blessing of the Sun
    spell_id: 76355
    cast_by: rajh
    cast_kind: channel
    target: pbaoe_self
    duration_ms: 8000
    trigger: boss_energy < 10
    effect: |
      AoE damage on group + boss takes increased damage + +100% damage buff
      for ALL PLAYERS for 8s.
    interruptible: false
    dispel_type: none
    bot_signal: OFFENSIVE_CD_BURN_WINDOW

  - name: Solar Fire (heroic only)
    spell_id: 89131
    cast_by: rajh / solar_winds
    cast_kind: passive_ground
    target: ground_along_solar_wind_path
    effect: persistent flame patches; fire damage
    interruptible: false
    dispel_type: none
    avoidable: true
    avoidance: move out

# Adds
adds:
  - name: Sun Orb
    npc_id: UNVERIFIED
    spawn_trigger: Summon Sun Orb cast lands
    abilities:
      - name: AoE burst (13y radius)
    notes: avoid landing zone; orb is temporary

  - name: Solar Winds tornado
    npc_id: UNVERIFIED
    abilities:
      - name: roving fire AoE (4y)
    notes: environmental hazard; not killable

positioning:
  default: |
    Tank central, facing away. Players SPREAD for Inferno Leap (no multi-hit).
    Kite-out Solar Winds. During Blessing of the Sun: stack-OR-spread per
    healer preference; group bursts boss with damage buff.
  tank_facing: away from group
  los_or_lookaway: none

interrupt_priority:
  - Summon_Sun_Orb (80352) — MUST_INTERRUPT
  - Inferno_Leap (87653) — MUST_INTERRUPT
  # Sun Strike is NOT interruptible

dispel_priority: []                       # Sun Strike DoT is UNDISPELLABLE

heroic_delta: |
  - Solar Fire (89131) persistent fire patches dropped along Solar Winds path
  - All damage values higher
  - Achievement window: kill in one Blessing of the Sun (Sun-of-a... achievement)

bot_role_summary:
  tank: standard tanking with defensive CDs through Sun Strike DoT
  healer: predictable Sun Strike raid-AoE; Blessing of the Sun heavy throughput; Solar Winds movement healing
  melee: kite-out Solar Winds; interrupt Sun Orb / Inferno Leap; max-burst during Blessing
  ranged: max range; kick rotation on Sun Orb / Inferno Leap; max-burst during Blessing
  cooldown_alignment: |
    Save offensive CDs for Blessing of the Sun window (the +100% damage
    buff doubles the value of Bloodlust / Berserk / Avenging Wrath /
    Combustion). bot_implementation_flag: cooldown alignment with
    encounter-specific damage buff window.
```

---

## Instance-wide notes

```yaml
instance_wide:
  curse_dispel_required: false
  disease_dispel_required: false
  poison_dispel_required: optional        # Pit Vipers on Anhuur (situational)
  magic_dispel_required: true             # Anraphet Nemesis Strike, Ammunae Wither, Anhuur Divine Reckoning, Isiset Veil of Sky (Spellsteal)
  dispel_blacklist: []                    # NONE — no "do not dispel" entries (unlike BRC Obsidius)
  group_buff_anomaly: none                # no instance-wide aura
  required_consumables: standard (flask + food + pots per spec)

  notable_dispel_calls:
    - { spell_id: 75592, name: Divine Reckoning, fight: anhuur, dispel_type: magic, urgency: HIGH (8s timer) }
    - { spell_id: 75604, name: Nemesis Strike, fight: anraphet, dispel_type: magic, urgency: HIGH (continuous tank DoT) }
    - { spell_id: 76043, name: Wither, fight: ammunae, dispel_type: magic, urgency: HIGH (10s incapacitation) }
    - { spell_id: 74133, name: Veil of Sky, fight: isiset, dispel_type: magic_spellsteal, urgency: MEDIUM (DPS gain on shield removal) }

  notable_kick_calls:
    - { spell_id: 75665, name: Consume Life Energy, fight: ammunae, urgency: ABSOLUTE (boss energy + target damage) }
    - { spell_id: 76903, name: Anti-Magic Prison, fight: setesh_void_seeker, urgency: ABSOLUTE (blocks magical healing) }
    - { spell_id: 80352, name: Summon Sun Orb, fight: rajh, urgency: HIGH (rotation kick) }
    - { spell_id: 87653, name: Inferno Leap, fight: rajh, urgency: HIGH (saves random player from leap-kill) }
    - { spell_id: 75540, name: Flame Bolt, fight: ptah, urgency: MEDIUM (5s channel; situational kick) }
    - { spell_id: 76043, name: Wither, fight: ammunae, urgency: MEDIUM (interrupt OR dispel) }
    - { spell_id: 74374, name: Arcane Barrage (Celestial Familiar), fight: isiset_familiar, urgency: LOW (rotation kick) }

  bot_implementation_flags:
    - {
        flag: anhuur_door_event,
        description: |
          2 designated DPS bots drop into side pits on Shield of Light cast.
          Bot must handle: drop-off-platform navigation, viper kill, Beacon
          channel, return path. NON-STANDARD movement.
      }
    - {
        flag: setesh_untankable,
        description: |
          Tank does NOT threat boss. Switch to add-tank mode (kite Sentinels,
          threat Wurms, intercept Seekers). NON-STANDARD tank behavior.
      }
    - {
        flag: setesh_mana_test,
        description: |
          Canonical dungeon mana-test. Healer pops Mana Tide / Innervate /
          Hymn here. Bot healer state-machine flag: SETESH_OOM_RISK.
      }
    - {
        flag: ptah_burrow_phase_aoe,
        description: |
          AoE management during 50% burrow phase. Bot AoE-rotation toggle
          on add-spawn event; back to ST when boss reforms.
      }
    - {
        flag: isiset_face_away,
        description: |
          Supernova requires all players FACE AWAY mid-combat. Bot must
          temporarily reverse facing on Supernova cast detection.
      }
    - {
        flag: rajh_blessing_burn_window,
        description: |
          Blessing of the Sun (76355) gives +100% damage to ALL PLAYERS for
          8s. Save offensive CDs (Bloodlust / Berserk / etc.) for this
          window. Bot offensive-CD heuristic must align with encounter
          burn windows.
      }
    - {
        flag: setesh_seed_walk_into,
        description: |
          Seed of Chaos (76870) is the ONLY positive ground-effect in the
          dungeon. Walk into for +100% damage buff. Bot ground-hazard
          avoidance logic must whitelist this spell ID.
      }
    - {
        flag: anraphet_heroic_void_zone_movement,
        description: |
          Heroic Alpha Beams leave persistent void zones. Floor fills over
          time. Bot must dynamically re-position (full path-recompute) as
          zones accumulate.
      }
```

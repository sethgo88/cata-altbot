# The Stonecore — Encounter Facts

**Role-neutral mechanic data** for the bot's encounter handling. Per-role response logic lives in sibling files (`healer.md`, `ranged-dps.md`, `melee-dps.md`, `tank.md`). When a role doc references a mechanic, it cites the spell ID defined here.

Spell IDs verified at `wowhead.com/cata/spell=<id>` where consensus could be reached; remaining UNVERIFIED items are flagged inline and aggregated in `docs/research/dbc-verification-checklist.md`. Strategy reconciled across 5+ sources — see `docs/research/the-stonecore-guide-survey.md` for the audit trail.

---

## Overview

```yaml
dungeon: the_stonecore
zone: the_stonecore
continent: deepholm                        # flying-mount entrance from Temple of Earth west side
difficulty_modes: [normal, heroic]
level_range_normal: [82, 84]
level_range_heroic: 85
boss_count: 4
optional_bosses: []                        # NO optional bosses — entire spine required
bosses_in_order:
  - corborus
  - slabhide
  - ozruk
  - high_priestess_azil
notes: |
  Entrance corridor → Millhouse Manastorm trash event → first chamber
  (Corborus, in The Winding Halls) → switchback ramp w/ Magmalord +
  Crystalspawn Giant + Flayer packs → Slabhide chamber (The Overlook) →
  Ozruk corridor (Chamber of Fanatics) → Azil's chamber (Heart of
  Destruction).

  No instance-wide buff/debuff. The dungeon is the launch-Cata heroic-gate
  skill check, primarily because of Ozruk's Paralyze → Shatter combo on
  heroic. Two dispel-required encounters: Corborus (Magic) and Azil (Curse).
  One major interrupt-rotation encounter: Azil (Force Grip on tank).
```

---

## Trash — notable mechanics

Only the trash entries with mechanics worth automating. Standard Twilight cultists and low-elite mobs that are pure threat-and-DPS are not enumerated.

```yaml
trash:
  # Pre-Corborus — the Millhouse Manastorm event (HARDEST PULL OF THE DUNGEON)
  - mob: Millhouse Manastorm
    npc_id: 43391
    abilities:
      - name: Pyroblast
        interruptible: true
        priority: MUST_INTERRUPT
        notes: hardcast caster; flees at low HP to next pull
      - name: Frostbolt
        interruptible: true
        priority: MUST_INTERRUPT
    notes: |
      Pulls with 3-5 Stonecore mobs. KILL FIRST (skull marker). Fragile;
      flees when ~30% HP. Returns repeatedly through the pre-Corborus
      corridor.

  - mob: Stonecore Berserker
    abilities:
      - name: Charge
        spell_id: 81574
        target: ally_random
        notes: charges target; gap-close
      - name: Spinning Slash
        spell_id: 81568
        cast_kind: channel
        duration_ms: 4000
        radius: 5
        school: physical
        priority: KITE_AWAY
        notes: |
          Channeled bleed AoE; ticks every 0.5s; stacks up to 10. Kite
          OUT of 5y radius during channel; do NOT melee while channeled.
    notes: |
      CC valid (Sap, Hex, Repentance, Polymorph). UNVERIFIED claim that
      Berserker has fixed-target / threat-ignore mechanic — see survey
      Disagreement #9.

  - mob: Stonecore Earthshaper
    abilities:
      - name: Force of Earth
        spell_id: 81459
        cast_time_ms: 5000
        school: nature
        interruptible: true
        priority: MUST_INTERRUPT_HIGHEST       # transformation = group wipe if completed
        notes: |
          5-SECOND CAST. Failure to interrupt = transforms Earthshaper
          into earth elemental ("Force of Earth") which deals heavy
          physical damage. CRITICAL INTERRUPT.
      - name: Ground Shock
        spell_id: 81530
        cast_time_ms: 600
        school: nature
        target: frontal_cone
        notes: 5y radius cone + knockback; tank face-away
      - name: Lava Burst
        spell_id: 81576
        cast_time_ms: 2000
        school: fire
        interruptible: true
        priority: MUST_INTERRUPT
        notes: 30y ranged hardcast; interruptible
    notes: |
      HIGHEST CC PRIORITY on multi-mob pulls. Polymorph / Hex / Sheep
      this mob first. If left uncontrolled, the 5s Force of Earth cast
      WILL fire and the group wipes.

  - mob: Stonecore Flayer
    abilities:
      - name: Flay (rapid AoE)
        notes: |
          Fast multi-strike AoE around the mob. Melee positions side/rear
          to minimize stacks. UNVERIFIED spell ID.
    notes: tank-and-spank; positioning concern only

  - mob: Crystalspawn Giant
    abilities:
      - name: melee_cleave
        target: frontal_cone
        notes: large mob; tank-and-spank from front; melee positions side/rear
    notes: high HP; not CC-able (likely elemental immunity)

  # Pre-Slabhide
  - mob: Stonecore Magmalord
    npc_id: 42789
    abilities:
      - name: Ignite
        spell_id: UNVERIFIED
        cast_kind: cast
        target: ally_random
        school: fire
        interruptible: UNVERIFIED              # likely; cast bar implied
        notes: applies fire DoT (4-5k normal / 14-15k heroic + 1k/sec for 10s)
      - name: Magma Eruption
        spell_id: UNVERIFIED
        cast_kind: cast
        target: ground_random
        school: fire
        notes: AoE around target locations; 14-15k damage in 2y radius
    notes: |
      Fire elemental; humanoid CC likely doesn't work. Standard tank-and-
      spank with interrupt rotation. UNVERIFIED humanoid-CC eligibility.

  # Pre-Slabhide elementals (drop from ceiling pre-pulled by entry to chamber)
  - mob: Twilight elementalist (pre-Slabhide)
    abilities:
      - name: hardcast
        interruptible: true
        notes: standard caster trash; CC valid
    notes: |
      Mob set drops in before Slabhide descends. Clear them; THEN Slabhide
      flies in for the boss pull (see survey Disagreement #10).

  # Pre-Ozruk corridor
  - mob: Twilight cultists (Ozruk corridor)
    abilities:
      - name: hardcast
        interruptible: true
    notes: humanoid CC valid (Polymorph, Hex, Sap, Fear)

  - mob: Earth elementals (Ozruk corridor)
    abilities:
      - name: hardcast (UNVERIFIED specific abilities)
        interruptible: UNVERIFIED
    notes: not humanoid-CC eligible

  # Pre-Azil
  - mob: Twilight followers (Azil hall)
    abilities:
      - name: hardcast
        interruptible: true
      - name: Mind Control (UNVERIFIED)
        priority: BREAK_VIA_DISPEL
        notes: |
          UNVERIFIED whether MC is in this trash set. Some sources mention
          it. If present: dispel/cleanse the affected player.
    notes: humanoid CC valid
```

### Trash skip notes

- **No optional boss room** to skip (unlike BRC's Beauty).
- A few mob clusters between Slabhide and Ozruk can be wall-hugged / LoS-skipped on speed-clears with movement-speed groups.
- Millhouse pull is **mandatory** and is the dungeon's hardest trash event.

### CC priorities

- **Polymorph / Hex / Sheep highest priority on Stonecore Earthshaper** (5s Force of Earth cast = group wipe if not interrupted or CC'd)
- Sap / Repentance / Polymorph valid on Berserkers
- Standard CC priority: Earthshaper > Berserker > Flayer > Magmalord (fire elemental, may resist)
- Crystalspawn Giants and Earth Elementals: not CC-able; tank-and-spank

---

## Boss 1 — Corborus

```yaml
boss: corborus
npc_id: 43438
location: first encounter, The Winding Halls (after Millhouse trash event)
hp_pct_phase_triggers: []                  # phases are time-triggered, not HP-based
notes: |
  Two-phase cycling encounter. Ground phase (Crystal Barrage + Dampening
  Wave) → Burrow phase (Thrashing Charges + Rock Borer adds) → repeats
  until kill. Burrow trigger is scripted on time (UNVERIFIED ~30-45s
  ground phase between burrows — see survey Disagreement #1).

phases:
  - id: 1
    name: ground_phase
    trigger: pull
    end_condition: scripted_burrow_event_fires
    description: |
      Tank-and-spank with periodic Crystal Barrage (group spreads) +
      Dampening Wave (Magic dispel off tank). Cycles back from phase 2.

  - id: 2
    name: burrow_phase
    trigger: scripted_burrow_event
    end_condition: 4 thrashing charges complete + boss resurfaces
    description: |
      Boss unattackable underground. 4 Thrashing Charges erupt from
      random player locations. Rock Borer adds spawn at random player
      positions. AoE-burn the borers; dodge the charges.

  - id: 3
    name: ground_resume
    trigger: end of phase 2
    end_condition: scripted_burrow_event OR hp_pct == 0
    description: cycle resumes (back to phase 1)

mechanics:
  - name: Crystal Barrage
    spell_id: 86881
    cast_by: corborus
    cast_kind: channel
    target: ground_random_player
    duration_ms: 4000
    radius: 5
    school: physical
    effect: |
      Ticks every 500ms in 5y radius around the targeted ground spot.
      Heroic spawns Crystal Shard adds every 500ms within the area.
    interruptible: false                   # channeled and not kick-style
    dispel_type: none
    avoidable: true
    avoidance: GROUP SPREADS (≥ 5 yards apart); targeted player moves out
    priority: GROUP_SPREAD

  - name: Dampening Wave
    spell_id: 82415
    cast_by: corborus
    cast_kind: instant
    target: pbaoe_self                     # 60y radius from boss
    school: shadow
    effect: |
      Shadow damage (~10k) + heal-absorb buff. 4000 absorb on normal,
      15000 on heroic. Affects all enemies within 60y.
    interruptible: false
    dispel_type: magic
    DISPEL_PRIORITY: HIGHEST                # tank first, then group
    notes: |
      Top dispel priority of the dungeon. Healer dispels off the TANK
      first (heal-absorb blocks tank healing); then off the rest of the
      group as bandwidth allows.

  - name: Burrow
    spell_id: 26381                        # UNVERIFIED — scripted entry
    cast_by: corborus
    cast_kind: scripted
    target: self
    effect: boss becomes unattackable; Rock Borers spawn
    interruptible: false
    notes: phase-state trigger for bot

  - name: Thrashing Charge
    spell_id: 81828
    cast_by: corborus
    cast_kind: cast
    cast_time_ms: 2900
    target: pbaoe_path                     # 40y radius effect from emerge point
    school: physical
    effect: |
      Erupts from underground; deals ~141k damage on heroic to players
      in path; knockback 150. Direct hit is lethal.
    interruptible: false
    avoidable: true
    avoidance: |
      Watch for dust-cloud telegraph on ground; move PERPENDICULAR to
      the charge axis. 4 charges per Burrow phase.
    priority: AVOID_TELEGRAPH

  - name: Rock Bore (Rock Borer add ability)
    spell_id: 80028
    cast_by: rock_borer (npc 43917)
    cast_kind: instant
    target: tank
    school: physical
    effect: |
      Bleed; 1k/3s normal, 4k/3s heroic; 15s duration; up to 10 stacks.
    interruptible: false
    dispel_type: none                      # Bleed = not dispellable in standard system
    notes: AoE-burn Rock Borers fast to prevent stack ramp on tank

  - name: Crystal Shard (heroic only)
    spell_id: UNVERIFIED                   # heroic add summoned by Crystal Barrage
    cast_by: crystal_shard_add
    target: closest_player
    notes: |
      Heroic-only mob; ~7.5k HP; spawns every 500ms during Crystal
      Barrage; explodes if reaching a player. Ranged DPS AoE-burns.

# Adds
adds:
  - name: Rock Borer
    npc_id: 43917
    count: UNVERIFIED (~3 per Burrow spawn)
    abilities:
      - { name: Rock Bore, spell_id: 80028 }
    notes: |
      Spawn at random player locations during Burrow. Low HP. AoE-burn.
      Leftover bleed stacks ramp tank damage in next ground phase.

  - name: Crystal Shard
    count: continuous spawn during Crystal Barrage on heroic
    abilities:
      - name: explosion_on_contact
    notes: |
      HEROIC ONLY. Spawns every 500ms during Crystal Barrage. Ranged AoE.

positioning:
  default: |
    GROUP SPREAD ≥ 5 yards. Crystal Barrage ticks in 5y radius around
    targeted ground; spread prevents multi-player hits. THIS IS OPPOSITE
    OF ROM'OGG (BRC) STACK RULE.
  tank_facing: away from group (standard); no frontal cone
  los_or_lookaway: none
  burrow_response: |
    During Burrow, watch for dust-cloud telegraphs and move perpendicular
    to the charge axis. AoE Rock Borers in the gaps.

interrupt_priority: []                     # no kick-required casts on boss

dispel_priority:
  - { spell_id: 82415, name: Dampening Wave, type: magic, target: tank_then_group, urgency: HIGHEST }

heroic_delta: |
  - Crystal Barrage spawns Crystal Shard adds every 500ms (ranged AoE)
  - Dampening Wave heal-absorb is 15k (vs 4k normal); dispel mandatory
  - Rock Bore bleed deals 4k/3s (vs 1k/3s); fast Rock Borer kills critical
  - Thrashing Charge damage scales hard (~141k); direct hit one-shot

bot_role_summary:
  tank: standard tank-and-spank ground phase; pick up Rock Borers in Burrow; re-acquire boss on resurface; defensive CDs on Rock Bore stack ramp
  healer: DISPEL DAMPENING WAVE FIRST OFF TANK (highest dispel priority); reactive heal Crystal Barrage; tank Rock Bore stack ramp
  melee: in melee on boss in ground phase; AoE Rock Borers in Burrow; dodge Thrashing Charge dust-cloud telegraphs
  ranged: SPREAD ≥ 5y; AoE Crystal Shards on heroic; predict Thrashing Charge erupt point; Crystal Barrage ground-target avoid
```

---

## Boss 2 — Slabhide

```yaml
boss: slabhide
npc_id: 43214
location: second encounter, The Overlook chamber (after pre-boss elemental drops)
hp_pct_phase_triggers: []                  # time-triggered, not HP-based
notes: |
  Two-phase cycling encounter. Ground phase (Sand Blast cone + Lava
  Fissure) → Air phase (lifts off, drops Stalactites, channels Crystal
  Storm on heroic, lands) → repeats. Air phase trigger is scripted on
  time (UNVERIFIED ~60s — see survey Disagreement #2).

phases:
  - id: 1
    name: ground_phase
    trigger: pull
    end_condition: scripted_air_phase_event
    description: |
      Tank-and-spank with Sand Blast (frontal cone, tank face-away) and
      Lava Fissure (random ground-target; 5s/3s telegraph → fire eruption +
      lava pool).

  - id: 2
    name: air_phase
    trigger: scripted_air_phase_event
    end_condition: stalactites_dropped AND boss_landed AND crystal_storm_ended
    description: |
      Boss lifts off. Stalactites drop on visible ground textures
      (avoid). Boss lands. On HEROIC: channels Crystal Storm for 6s —
      LoS-break behind a stalactite pillar to avoid damage. Stalactite
      pillars persist ~30-35s, blocking LoS and movement.

  - id: 3
    name: ground_resume
    trigger: end of phase 2
    end_condition: scripted_air_phase_event OR hp_pct == 0
    description: cycle resumes

mechanics:
  - name: Sand Blast
    spell_id: 80807                        # UNVERIFIED
    cast_by: slabhide
    cast_kind: instant
    target: frontal_cone
    school: nature
    cone_angle: 60_degrees
    range: 15
    effect: ~47k Nature damage on heroic to all enemies in cone
    interruptible: false                   # instant
    dispel_type: none
    avoidable: true
    avoidance: TANK FACES AWAY from group; ranged stays out of arc
    priority: TANK_FACING_RULE

  - name: Lava Fissure (target placement)
    spell_id: 80803
    cast_by: slabhide
    cast_kind: instant
    target: ground_random_player
    school: fire
    effect: |
      Spawns crack on player position. After 5s normal / 3s heroic →
      Eruption fires.
    interruptible: false
    avoidable: true
    avoidance: MOVE OUT of crack before eruption
    priority: GROUND_AVOID

  - name: Eruption (the actual damage from Lava Fissure)
    spell_id: 80800
    cast_by: lava_fissure (env)
    cast_kind: instant
    target: ground_self
    school: fire
    radius: 5
    effect: |
      Fire damage in 5y radius; spawns persistent Lava Pool (10s normal /
      30s heroic; 24k fire/sec).
    interruptible: false
    notes: triggered by Lava Fissure ground-target after delay

  - name: Lava Pool
    spell_id: UNVERIFIED                   # fire DoT zone
    cast_by: env
    cast_kind: passive
    target: ground_self
    school: fire
    duration_ms_normal: 10000
    duration_ms_heroic: 30000
    effect: ~24k fire damage per second to players inside
    interruptible: false
    avoidable: true
    avoidance: STAND OUT of lava pool; reposition off ground patches

  - name: Stalactite (drop)
    spell_id: 80643
    cast_by: slabhide (during air phase)
    cast_kind: instant
    target: ground_random
    school: physical
    effect: |
      Heavy physical damage + knockback to player at impact point.
      Drops a persistent pillar that blocks LoS and movement for ~30-35s.
    interruptible: false
    avoidable: true
    avoidance: MOVE OFF visible ground texture before stalactite drops
    priority: GROUND_AVOID
    secondary_use: stalactite pillars usable as LoS shields for heroic Crystal Storm

  - name: Crystal Storm (HEROIC ONLY)
    spell_id: 92265
    cast_by: slabhide (after landing from air phase)
    cast_kind: cast
    cast_time_ms: 2500
    duration_ms: 6000
    target: pbaoe_self
    school: physical                       # spell page says Physical; community says Nature — see survey Disagreement #8
    radius: 8
    effect: ticks every 100ms; heavy damage to players in LoS
    interruptible: false                   # not kick-style; LoS is the mitigation
    avoidable: true
    avoidance: |
      LOS-BREAK BEHIND STALACTITE PILLAR for the 6s channel. The
      stalactites dropped during air phase are the cover. Plan a pillar
      pair before air phase ends.
    priority: LOS_BREAK
    notes: HEROIC ONLY mechanic; not present on normal

  - name: melee
    cast_by: slabhide
    cast_kind: melee
    target: tank
    effect: standard tank melee

# No adds during the boss encounter

positioning:
  default: |
    Tank: pull boss to one side of the chamber (not center) to leave
    space for stalactite-pillar LoS mechanic later. Face boss away from
    group.
    Melee: behind boss; respond to Lava Fissure under feet by moving out.
    Ranged: max range, OUT of cone arc; respond to Lava Fissure.
    Healer: free positioning; maintain LoS through stalactites
    (reposition proactively if a stalactite drops between you and group).
  tank_facing: away from group (frontal cone is 60° forward)
  los_or_lookaway: |
    HEROIC ONLY: LoS-break behind stalactite pillar during 6s Crystal
    Storm channel. Plan pillar pair during air phase.
  air_phase_response: |
    Move continuously to avoid Stalactite ground textures. Pre-position
    near a stalactite pillar that will become LoS cover for Crystal
    Storm.

interrupt_priority: []                     # no kick-required casts

dispel_priority: []                        # no dispels required

heroic_delta: |
  - Lava Fissure delay 5s → 3s; less reaction time
  - Lava Pool persists 10s → 30s; ground becomes increasingly hazardous
  - Crystal Storm ADDS as a heroic-only mechanic after each air phase
  - Stalactite damage hits harder; direct hit is one-shot territory

bot_role_summary:
  tank: standard tank-and-spank with cone facing; rotate boss face during Lava Fissure under tank to keep cone away from group
  healer: maintain LoS through stalactites (movement-aware); on heroic, pre-shield/pre-HoT before Crystal Storm channel (6s of LoS-broken healing)
  melee: dodge Lava Fissure (main melee skill check); behind-boss positioning; on heroic, position near stalactite pillar for Crystal Storm cover
  ranged: max range; out of cone arc; LoS-break Crystal Storm on heroic — encounter-specific positioning override
```

---

## Boss 3 — Ozruk

```yaml
boss: ozruk
npc_id: 42188
location: third encounter, Chamber of Fanatics corridor (signature heroic skill check)
hp_pct_phase_triggers: []                  # single-phase with cycling abilities
notes: |
  Single phase; cyclically uses Spike Shield + Bulwark + Ground Slam +
  Shatter (+ Paralyze + Rupture on heroic). The fight rules are:
    1. Run THROUGH boss when Ground Slam casts (everyone, tank included)
    2. Stop melee when Spike Shield is up; stop hardcasts when Bulwark is up
    3. Be ≥ 5y from boss when Shatter fires
    4. (HEROIC) Have a DoT/HoT-via-melee applied so Paralyze breaks on
       damage tick before its 8s expire-damage resolves.

  This is the launch-Cata HEROIC skill check. The Paralyze → Shatter
  combo on heroic is the wipe-tier mechanic that gates progression.

phases:
  - id: 1
    name: regular
    trigger: pull
    end_condition: hp_pct == 0
    description: |
      Single phase, cycling abilities on triggers (aura-state changes
      and 3s casts).

mechanics:
  - name: Ground Slam
    spell_id: 78903
    cast_by: ozruk
    cast_kind: cast
    cast_time_ms: 3000
    target: pbaoe_self_forward             # 4y radius IN FRONT of boss
    school: physical
    effect: ~94k physical damage on heroic; 4y radius forward; triggers Rupture on heroic
    interruptible: false                   # NOT kick-able per multiple sources (see survey Disagreement #4)
    avoidable: true
    avoidance: |
      RUN THROUGH BOSS — everyone (tank included) runs to behind the
      boss before the 3s cast resolves. Default melee "stay behind boss"
      doesn't suffice during the cast — tank that's stationary in front
      takes the hit.
    priority: RUN_THROUGH
    cast_telegraph_ms: 3000                # 3s cast = 3s movement window

  - name: Rupture (HEROIC ONLY)
    spell_id: 1943
    cast_by: ozruk (post-Ground-Slam)
    cast_kind: scripted
    target: line_forward                   # 3 lines of rock spikes in facing direction
    school: physical
    effect: ~141k damage + knockback up to players struck
    interruptible: false
    avoidable: true
    avoidance: stay BEHIND boss after Ground Slam resolves; do NOT re-enter front arc
    notes: HEROIC ONLY; chained off Ground Slam

  - name: Elementium Spike Shield
    spell_id: 78835
    cast_by: ozruk
    cast_kind: instant
    target: self
    school: arcane
    duration_ms: 10000
    effect: |
      10s self-buff. Melee/ranged hits cause stacking bleed (300/sec
      normal, 600/sec heroic; 5 stacks max; 3s per stack). On heroic,
      this buff TRIGGERS Paralyze on its 10s expiration alongside
      Shatter.
    interruptible: false
    dispel_type: none                      # boss buff; not dispellable
    counter_NORMAL: STOP melee/ranged attacks for the 10s duration
    counter_HEROIC: |
      Accept 1-2 bleed stacks DELIBERATELY so the bleed DoT ticks break
      Paralyze when it's applied at 10s. (Or rely on existing DoTs:
      Warlock Corruption, Hunter Serpent Sting, Shadow Priest VT/SW:P,
      Paladin Censure, etc.)

  - name: Elementium Bulwark
    spell_id: 78939
    cast_by: ozruk
    cast_kind: instant
    target: self
    school: shadow
    duration_ms: 10000
    effect: 100% spell reflect on heroic (20% normal); 10s
    interruptible: false
    dispel_type: none                      # boss buff
    counter: STOP HARDCASTS — caster bots wait out the 10s on instant abilities only (DoTs already ticking are safe)
    notes: existing DoTs ticking on the boss are NOT reflected; only new spell casts
    priority: STOP_HARDCAST

  - name: Shatter
    spell_id: 77715
    cast_by: ozruk
    cast_kind: instant                     # fires at Spike Shield expiration
    target: pbaoe_self
    school: frost                          # per Wowhead spell page
    radius: 5
    effect: ~84k+ heroic damage; 5y radius AoE
    interruptible: false
    avoidable: true
    avoidance: |
      MOVE TO ≥ 5 YARDS at the 9-second mark of Spike Shield (1s before
      Shatter resolves). Tank, melee, healer, ranged — everyone steps
      out.
    priority: STEP_OUT
    chain: triggered by Elementium Spike Shield expiring

  - name: Paralyze (HEROIC ONLY)
    spell_id: 92426
    cast_by: ozruk (alongside Shatter)
    cast_kind: instant
    target: pbaoe_self                     # all players (50000y "anywhere")
    school: arcane
    duration_ms: 8000
    effect: |
      8s stun. Damage taken removes the stun (DoT/HoT ticks count). At
      expiration, deals ~47k arcane damage to players still stunned.
    interruptible: false
    dispel_type: magic
    counter_PRIMARY: |
      DAMAGE-BREAK via active DoTs/HoTs. Pre-applied Warlock Corruption,
      Hunter Serpent Sting, Shadow Priest VT/SW:P, Paladin Censure,
      Druid Insect Swarm, etc., tick during the stun and break it on
      first tick.
    counter_BACKUP: |
      Healer Magic dispel (Cleanse / Dispel Magic / Purify) is the
      fallback if no DoT is active and damage is too low to break
      naturally. Bot prefers DAMAGE-BREAK (passive) over DISPEL (active
      GCD cost).
    priority: DAMAGE_BREAK_VIA_PRE_DOT

  - name: melee
    cast_by: ozruk
    cast_kind: melee
    target: tank
    effect: standard tank melee

positioning:
  default: |
    Tank: pull Ozruk into the corridor and tank against a wall. Minimizes
    knockback room (Rupture on heroic knocks up).
    Melee: behind boss; runs through on Ground Slam.
    Ranged: max range; runs through to behind during Ground Slam (or
    just out of 4y front-arc); steps to ≥ 5y for Shatter.
    Healer: max range; ≥ 5y from boss for Shatter.
  tank_facing: away from group; tank against wall
  los_or_lookaway: none
  ground_slam_response: ALL PLAYERS RUN THROUGH BOSS
  shatter_response: ALL PLAYERS ≥ 5y from boss at 9s of Spike Shield
  spell_reflect_response: ALL CASTERS STOP HARDCASTS while Bulwark is active

interrupt_priority: []                     # NOT KICKABLE; see Disagreement #4

dispel_priority:
  - { spell_id: 92426, name: Paralyze, type: magic, target: self_or_group, urgency: BACKUP_ONLY, notes: PREFER DOT-DAMAGE-BREAK over dispel }

heroic_delta: |
  - Paralyze added to Shatter cycle — 8s stun
  - Rupture (post-Ground-Slam) — spike-line damage + knockback up
  - Spike Shield bleed deals 600/sec instead of 300/sec
  - HEROIC SKILL CHECK: pre-applied DoT must tick during Paralyze to
    break the stun before it expires for 47k arcane damage AND before
    Shatter resolves at 5y
  - Bulwark spell-reflect 100% (vs 20% normal); caster bots wait out

bot_role_summary:
  tank: tank-against-wall; RUN THROUGH BOSS on Ground Slam (deliberate into-AoE-then-out movement); pop defensive on Spike Shield expiration; ≥ 5y for Shatter
  healer: do NOT auto-dispel Paralyze on heroic by default — let DoTs handle break; pre-HoT before Shatter resolves so HoT ticks pre-break Paralyze; defensive dispel is fallback
  melee: STOP MELEE on Spike Shield (normal) OR allow 1-2 bleed stacks for Paralyze break (heroic); RUN THROUGH on Ground Slam; ≥ 5y for Shatter
  ranged: STOP HARDCASTS on Bulwark; CONTINUE DOTS already on boss; ensure DoT ticks during Paralyze on heroic; ≥ 5y for Shatter
```

---

## Boss 4 — High Priestess Azil

```yaml
boss: high_priestess_azil
npc_id: 42333
location: final encounter, Heart of Destruction
hp_pct_phase_triggers: []                  # cycles by time, not HP
notes: |
  Two-stage cycling encounter. Stage 1 (ground): Force Grip channel +
  Gravity Wells + Curse of Blood + Devout Follower add waves. Stage 2
  (levitation at altar): Energy Shield (75% damage reduction) + 3
  Seismic Shard tombstones cast in sequence; more adds. Cycles. Stage
  trigger is scripted on time (UNVERIFIED ~60-90s — see survey
  Disagreement #6).

phases:
  - id: 1
    name: stage_1_ground
    trigger: pull
    end_condition: scripted_levitation_event
    description: |
      Boss attackable on the ground. Periodically casts Force Grip on
      tank (CHANNELED — INTERRUPTIBLE; HIGH PRIORITY KICK), Gravity
      Wells spawn at random player positions, Curse of Blood applies on
      random target (CURSE DISPEL), Devout Followers spawn in waves.

  - id: 2
    name: stage_2_levitation
    trigger: scripted_levitation_event
    end_condition: 3 seismic shards complete + scripted descend event
    description: |
      Boss flies to altar; gains Energy Shield (75% mitigation; deals
      knockback + arcane damage on creation to players within 5y).
      Casts Seismic Shard 3 times in sequence (massive ground-target
      tombstones; sidestep). More Devout Followers spawn during this
      stage.

  - id: 3
    name: stage_1_resume
    trigger: end of stage 2
    end_condition: scripted_levitation_event OR hp_pct == 0
    description: cycle resumes

mechanics:
  - name: Force Grip
    spell_id: 79351
    cast_by: high_priestess_azil
    cast_kind: cast_then_channel
    cast_time_ms: 1500
    duration_ms: 5000                      # channel
    target: tank
    school: physical
    effect: |
      Channels for 5s; lifts target and slams them; ~70-79k physical per
      slam tick. CHANNELED — interruptible by kick (Wind Shear,
      Counterspell, Pummel, Kick, Rebuke, Mind Freeze, Skull Bash,
      Silencing Shot, Solar Beam, Spell Lock).
    interruptible: true
    dispel_type: none
    priority: HIGHEST_KICK_PRIORITY
    notes: |
      THE primary interrupt-rotation challenge of the dungeon. Bot kick
      rotation must be tight; missed kick = 5s of slam damage on tank
      (~280k+ damage).

  - name: Gravity Well
    spell_id: 79249
    cast_by: high_priestess_azil
    cast_kind: instant
    target: ground_random_player
    school: shadow
    radius: 10
    duration_ms_normal: 20000
    duration_heroic: until 4 unit kills (shrinks 25% per kill)
    effect: |
      Pull effect dragging players within 10y toward center. Ramping
      shadow damage as you approach center (closer = more damage).
    interruptible: false
    dispel_type: none
    avoidable: true
    avoidance: MOVE OUT of well's pull radius (10y); push outward against pull
    priority: GROUND_AVOID
    secondary_use_HEROIC: |
      Funnel Devout Followers through wells to shrink them; each
      death reduces well by 25%.

  - name: Curse of Blood
    spell_id: 16098
    cast_by: high_priestess_azil
    cast_kind: cast
    cast_time_ms: 2000
    target: ally_random
    school: shadow
    duration_ms: 15000
    effect: +5000 physical damage taken (heroic); +1000 (normal — older value)
    interruptible: UNVERIFIED               # likely interruptible (2s cast); not commonly listed in interrupt rotations
    dispel_type: curse                     # CURSE — Mage Remove Curse / Druid Remove Corruption / Shaman Cleanse Spirit (talented)
    DISPEL_PRIORITY: HIGH                  # always dispel on application
    avoidable: false
    notes: |
      Random target; if it lands on the tank-target during a cleave
      moment, damage compounds heavily. Dispel ASAP.

  - name: Energy Shield (Stage 2)
    spell_id: 79050
    cast_by: high_priestess_azil
    cast_kind: cast
    cast_time_ms: 2000
    target: self
    school: arcane
    duration: until end of stage 2
    effect: |
      75% damage reduction self-buff. ON CREATION: deals ~47k arcane
      damage to enemies within 5y + knockback 150.
    interruptible: false
    dispel_type: none                      # boss buff
    avoidable_creation_damage: true
    avoidance: STAY ≥ 5y from boss when she begins lifting to altar
    priority: STAGE_TRANSITION_AWARENESS

  - name: Seismic Shard
    spell_id: 79002
    cast_by: high_priestess_azil (during stage 2)
    cast_kind: scripted                    # spell page is buff form; actual missile
    target: ground_random_player
    school: physical
    radius: 3.5
    effect: ~942k-1057k physical damage on direct hit (heroic) — wipe-tier
    interruptible: false
    avoidable: true
    avoidance: SIDESTEP visible ground-target telegraph; 3.5y radius is small
    priority: GROUND_AVOID_HIGHEST
    cast_count_per_stage_2: 3

  - name: melee (stage 1 only)
    cast_by: high_priestess_azil
    cast_kind: melee
    target: tank

# Adds
adds:
  - name: Devout Follower
    npc_id: 42428                          # UNVERIFIED — verify against TC NPC table
    count_per_wave: 9-12 (UNVERIFIED — see survey Disagreement #6)
    waves_per_stage_1: 1-3
    abilities:
      - name: melee_attack
        target: closest_player
    notes: |
      Low-HP humanoid cultists. AoE-burn. On HEROIC, funnel through
      Gravity Wells to shrink wells while killing.
    kill_priority: HIGH

  - name: Disciple of Twilight (UNVERIFIED separate type)
    notes: |
      Achievement "Rotten to the Core" requires 60 Disciples in 10s
      during the Azil heroic encounter — implies a separate (or
      alternate-name) add type from Devout Follower, possibly heroic-
      only or stage-2 specific. Flag for DBC verification.

positioning:
  default: |
    Tank: holds Azil at center of platform (within kick range of melee);
    don't chase Force Grip lift.
    Stage 1 group: SPREAD; respond to Gravity Well by moving OUTWARD
    against pull.
    Stage 2 group: SPREAD widely; sidestep Seismic Shard tombstones
    individually; avoid the altar area for Energy Shield knockback.
    Healer: free positioning; maintain LoS; stay ≥ 5y from Azil during
    stage transitions.
  tank_facing: away from group (standard)
  los_or_lookaway: none
  gravity_well_response: MOVE OUTWARD against pull; do not center
  stage_2_response: SPREAD; sidestep Seismic Shard individually

interrupt_priority:
  - { spell_id: 79351, name: Force Grip, urgency: HIGHEST, target: high_priestess_azil }

dispel_priority:
  - { spell_id: 16098, name: Curse of Blood, type: curse, target: random_ally, urgency: HIGH }

heroic_delta: |
  - Gravity Wells shrink only by killing units (no time decay) —
    strategic add-funnel through wells
  - More Devout Followers per wave (9-12+)
  - Seismic Shard direct hit is wipe-tier
  - Curse of Blood applies more frequently — dispel-cooldown management
  - Force Grip is more punishing if kick missed
  - "Rotten to the Core" achievement requires 60 Disciple kills in 10s

bot_role_summary:
  tank: hold Azil within kick range of melee; pop defensive on Force-Grip-not-kicked event; in Stage 2, AoE on Devout Followers (boss is 75% mitigated)
  healer: DISPEL CURSE OF BLOOD on detection (HIGH priority); reactive heal Force Grip channel (kick will end it but not before some damage); pre-HoT before Stage 2 transition for knockback
  melee: KICK FORCE GRIP every cast (primary interrupt duty); switch to AoE on Devout Followers; sidestep Seismic Shard in Stage 2
  ranged: kick Force Grip if class has kick; AoE Devout Followers; SPREAD in Stage 2 (avoid Seismic Shard cross-hit); sidestep individual tombstones
```

---

## Instance-wide notes

```yaml
instance_wide:
  curse_dispel_required: true                # Azil Curse of Blood (16098) — HIGH priority
  disease_dispel_required: false
  poison_dispel_required: false
  magic_dispel_required: true                # Corborus Dampening Wave (82415) — HIGHEST priority
                                             # Ozruk Paralyze (92426) — BACKUP only (prefer DoT-break)
  dispel_blacklist: []                       # NO blacklisted dispels (unlike BRC's Crepuscular Veil)
  group_buff_anomaly: none
  required_consumables: standard (flask + food + pots per spec)
  dispel_summary: |
    Stonecore is a HIGH-DISPEL dungeon — two required dispel-types
    (Magic on Corborus, Curse on Azil) and a conditional dispel
    (Magic-Paralyze on Ozruk heroic, prefer DoT-break).
  interrupt_summary: |
    Stonecore is a HIGH-INTERRUPT dungeon for tank+melee:
    - Stonecore Earthshaper Force of Earth (81459, 5s cast) — wipe if missed
    - Azil Force Grip (79351, channeled) — every cast
    - Twilight casters and Magmalord Lava Burst — standard rotation
    Ozruk: NO interrupt-required casts (Ground Slam is non-kickable).
  heroic_skill_check: ozruk_paralyze_shatter_combo
```

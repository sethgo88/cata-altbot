# The Vortex Pinnacle — Encounter Facts

**Role-neutral mechanic data** for the bot's encounter handling. Per-role response logic lives in sibling files (`healer.md`, `ranged-dps.md`, `melee-dps.md`, `tank.md`). When a role doc references a mechanic, it cites the spell ID defined here.

Spell IDs verified at `wowhead.com/cata/spell=<id>` where consensus could be reached; some are flagged **UNVERIFIED — DBC pass** in this doc and aggregated in the survey doc. Strategy reconciled across 10 sources — see `docs/research/the-vortex-pinnacle-guide-survey.md` for the audit trail.

---

## Overview

```yaml
dungeon: the_vortex_pinnacle
zone: the_vortex_pinnacle
continent: skywall_floating_above_uldum
difficulty_modes: [normal, heroic]
level_range_normal: [82, 84]
level_range_heroic: 85
boss_count: 3
optional_bosses: []                       # none — all 3 required
bosses_in_order:
  - grand_vizier_ertan
  - altairus
  - asaad_caliph_of_zephyrs
notes: |
  Three floating-platform subzones connected by Slipstream tornado-vehicles:
  Cyclone Summit (Ertan) → bridge with Howling Gale hazards → Nimbus Rise
  (Altairus) → Slipstream → Temple of Asaad (Asaad).

  No instance-wide buff/debuff. No instance-wide curse/disease/poison dispel
  requirement. Magic dispel is REQUIRED on Asaad heroic (Static Cling).

  Heroic-only mechanics are unusually concentrated:
  - Ertan: Lurking Tempest add spawns
  - Altairus: Twisting Winds patrolling tornadoes
  - Asaad: Static Cling magic-dispel mechanic

  Falling off any platform = teleport to dungeon entrance (NOT death, but
  significant time loss + run-back).
```

---

## Trash — notable mechanics

Only the trash entries with mechanics worth automating. Standard packs (basic flying wisps, low-elite mobs) are pure threat-and-DPS and not enumerated.

```yaml
trash:
  # Cyclone Summit (pre-Ertan)
  - mob: Wild Vortex
    abilities:
      - name: wind_cast
        interruptible: true
        priority: MUST_INTERRUPT
        notes: caster trash; spawns small adds if not interrupted
    notes: caster; CC valid

  - mob: Gust Soldier
    abilities:
      - name: damage_over_time_debuff
        dispel_type: magic                 # UNVERIFIED — guide says "dispellable"
        target: tank_or_melee
    notes: melee; standard pull

  - mob: Armored Mistral
    abilities:
      - name: Cloud Guard
        target: self
        effect: ranged-attack resistance buff while active
    notes: forces melee engagement; ranged DPS less effective during Cloud Guard window

  - mob: Cloud Prince
    abilities:
      - name: Turbulence
        cast_kind: cast
        target: pbaoe_self
        effect: AoE damage on cast
        defensive_response: pop short defensive on Turbulence cast
      - name: spawn_adds
        cast_kind: scripted
        target: ground_around_cloud_prince
        effect: spawns small elementals during fight
    notes: |
      MINI-BOSS-STYLE. CANNOT be CC'd (immune to Polymorph/Hex/Sap/Banish).
      AoE-burn priority before Turbulence resolves.

  - mob: Lurking Tempest (trash version)
    abilities:
      - name: Lightning Bolt
        cast_kind: cast
        target: facing_player
        effect: only attacks players FACING it
    notes: |
      Same mechanic as boss adds — does not engage if no one faces it.
      Bot rule: target-suppression list — do NOT acquire as target unless
      already engaged.

  # Bridge between Ertan and Slipstream-to-Altairus
  - mob: Howling Gale (×2 on bridge)
    abilities:
      - name: gale_aoe
        cast_kind: passive
        target: ground_around_self
        effect: |
          Standing AoE that can knock players off the bridge into open air.
          OFF-PLATFORM KNOCKOFF = teleport to dungeon entrance (bot-implementation
          critical — see instance_wide notes).
    notes: |
      DPS-down to disable temporarily; cross while disabled. UNVERIFIED whether
      they "die" or just disable — mechanically equivalent for the strat.
    bot_critical: true

  # Nimbus Rise (pre-Altairus)
  - mob: Turbulent Squall
    abilities:
      - name: caster_cast
        interruptible: true
        priority: MUST_INTERRUPT
      - name: self_buff
        dispel_type: magic
        priority: MUST_PURGE                 # purge / spellsteal / Mass Dispel valid
    notes: caster; CC + interrupt + dispel target

  - mob: Empyrean Assassin
    abilities:
      - name: Vapor Form
        cast_kind: cast
        target: self
        effect: stealth / heal-into-stealth combo
        interruptible: true
        priority: MUST_INTERRUPT
        notes: interrupt or the assassin re-stealths and resets/heals
      - name: Lethargic Poison
        cast_kind: melee_application
        target: tank_or_melee
        effect: stacking poison debuff
        dispel_type: poison
        priority: HIGH                      # Cleanse / Cleanse Spirit / Abolish Poison
    notes: stealth-style melee; high pressure on tank

  - mob: Young Storm Dragon
    abilities:
      - name: aoe_damage
        target: pbaoe_self
      - name: frontal_breath
        target: frontal_cone
    notes: |
      MOST DANGEROUS trash mob in the dungeon. Tank with care; healer mana
      pressure. Group can use platform Slipstream as panic-escape if HP
      collapses (= teleport to entrance, full reset).

  # Temple of Asaad (pre-Asaad)
  - mob: Servant of Asaad
    abilities:
      - name: Sure Strike
        cast_kind: instant
        target: tank
        effect: physical charge
    notes: easiest mob in the area; tank-and-spank

  - mob: Temple Adept
    abilities:
      - name: Greater Heal
        cast_kind: cast
        target: ally_lowest_hp
        interruptible: true
        priority: MUST_INTERRUPT             # MANDATORY — heals other mobs to full
      - name: Holy Smite
        cast_kind: cast
        target: enemy_random
        interruptible: true
        priority: SHOULD_INTERRUPT
    notes: |
      HEALER TRASH — HIGHEST CC PRIORITY in the dungeon's trash. Mark for
      Polymorph / Hex / Sap. Mind Control is canonically excellent here:
      they heal YOUR group instead.

  - mob: Executor of the Caliph
    abilities:
      - name: aoe_damage
        target: pbaoe_self
      - name: defensive_buff
        cast_kind: cast
        interruptible: false                # uninterruptible; wait it out
    notes: large melee; AoE pressure

  - mob: Minister of Air
    abilities:
      - name: nature_caster_attacks
        cast_kind: cast
        target: enemy_random
        interruptible: true
    notes: |
      Nearby environmental "Grounding Field" object grants spell immunity if
      stood in. Player exploits this against the Minister. Not bot-relevant
      unless bot AI can use environmental objects.

  - mob: Skyfall Star (trash version)
    abilities:
      - name: Arcane Barrage
        cast_kind: cast
        target: enemy_random
        school: arcane
    notes: |
      Trash version of the boss-summoned Skyfall Star. Low HP, ranged DPS
      priority. First trash pack of these can be SKIPPED by hugging right
      wall + dropping past staircase.
```

### Trash skip notes

- **Several Cyclone Summit packs** can be Slipstream-skipped (ride the tornado past them); not the standard play, but valid for speed-clears.
- **First Skyfall Star pack near Asaad** can be skipped by hugging the right wall and dropping next to the staircase.
- **Bridge Howling Gales must be disabled** — cannot be skipped, the bridge is the only path.
- **Default bot behavior**: clear trash the tank engages; do not auto-skip unless group leader instructs.

### CC priorities

- **Polymorph / Hex / Sap / Banish** valid on Tol'vir humanoids and most elementals
- **Cloud Prince** = **immune to CC** (AoE-burn instead)
- **Lurking Tempest** = ignore via target-suppression (don't engage)
- **Empyrean Assassin** = Sap on initial pull, otherwise interrupt-cycle
- **Mind Control on Temple Adept** = top-tier play if Priest in group

### Pull markers for CC

- Multi-pull with Temple Adept: skull on Adept (kill priority OR Mind Control), cross/X on second caster
- Caster trash: skull on lead caster, CC second
- Cloud Prince: skull on Prince (no CC); spread for Turbulence

---

## Boss 1 — Grand Vizier Ertan

```yaml
boss: grand_vizier_ertan
npc_id: 43878
location: first encounter, Cyclone Summit platform after entrance + first slipstream
hp_pct_phase_triggers: []
notes: |
  Single-phase tank-and-spank with periodic Cyclone Shield retract events
  (~every 30s) and (heroic only) periodic Lurking Tempest add spawns at
  platform edges.

  Standout mechanic: 8 cyclones rotate around boss at ~30y radius. Group
  STAYS INSIDE THE RING — Storm's Edge DoT punishes anyone outside. This
  is an EXPLICIT OVERRIDE of default ranged-spread positioning.

phases:
  - id: 1
    name: regular
    trigger: pull
    end_condition: hp_pct == 0
    description: |
      Continuous: tank holds Ertan, group stacks inside cyclone ring,
      Lightning Bolts on tank. Periodic cyclone retract events; on heroic,
      periodic Lurking Tempest add spawns at platform edges.

mechanics:
  - name: Cyclone Shield
    spell_id: 86292
    cast_by: grand_vizier_ertan
    cast_kind: passive
    target: self
    effect: |
      8 cyclones orbit Ertan at ~30y radius. Contact damage 23,750-26,250
      Nature + 60% slow/attack/cast debuff for 12s. Cyclones periodically
      retract toward boss (~every 30s) — players dodge OR position in dead
      zones between cyclones.
    interruptible: false
    dispel_type: none
    avoidable: partial
    avoidance: |
      Stay INSIDE the ring (between Ertan and the cyclone orbit path). Dead
      zones between cyclones are stable for stationary players; retract
      events are dodged through gaps.
    priority: GROUP_INSIDE_RING

  - name: Storm's Edge
    spell_id: 86309
    cast_by: grand_vizier_ertan
    cast_kind: passive
    target: pbaoe_aura_outside_cyclones
    effect: 9,599-11,732 Nature DoT/sec on players OUTSIDE the cyclone ring
    interruptible: false
    dispel_type: none
    avoidable: true
    avoidance: stay INSIDE the cyclone ring
    priority: STAY_INSIDE_RING

  - name: Lightning Bolt
    spell_id: UNVERIFIED                   # likely 86331 or adjacent — DBC pass
    cast_by: grand_vizier_ertan
    cast_kind: cast
    target: tank (current_target)
    cast_time_ms: 2000-2500
    effect: 8,500-11,500 Nature damage on current target
    interruptible: true
    dispel_type: none
    priority: MEDIUM_INTERRUPT             # ease healer; not mandatory

  - name: Summon Tempest
    spell_id: 86340
    cast_by: grand_vizier_ertan
    cast_kind: scripted
    target: platform_edges
    cast_time_ms: 0
    cooldown_ms: 30000-45000
    effect: spawns Lurking Tempest add at platform edge
    interruptible: false
    heroic_only: true                       # UNVERIFIED whether normal-mode has any tempests
    priority: ADD_SPAWN_TRIGGER

  - name: (Lurking Tempest) Lightning Bolt
    spell_id: UNVERIFIED
    cast_by: lurking_tempest
    cast_kind: cast
    target: facing_player
    cast_time_ms: 2000
    effect: 6,562-8,437 Nature damage on whoever is FACING the tempest
    interruptible: true
    dispel_type: none
    priority: TARGET_SUPPRESSION_PREFERRED   # don't engage = don't take damage

  - name: melee
    cast_by: grand_vizier_ertan
    cast_kind: melee
    target: tank
    effect: standard tank melee

# Adds (heroic only)
adds:
  - name: Lurking Tempest
    count: continuous_spawn (heroic only)
    locked_to: platform_edges
    abilities:
      - name: Lightning Bolt (only when faced)
    notes: |
      DEFAULT BEHAVIOR: TARGET-SUPPRESSION. Tempest only attacks when a
      player has it as their target / is facing it. Bot rule: do NOT auto-
      acquire as target. If a tempest somehow becomes hostile (mis-faced),
      kill it as a secondary fallback.
    kill_priority: SUPPRESS                  # don't engage by default

positioning:
  default: |
    All players STACK INSIDE the cyclone ring at melee range. This is an
    EXPLICIT OVERRIDE of default ranged-spread behavior.
  tank_facing: any (no frontal cone) — face center of platform on heroic for tempest mgmt
  los_or_lookaway: |
    HEROIC: face toward Ertan (boss is at platform center). Do NOT face
    platform edges (where tempests spawn). Bot AI: target-suppress tempest
    instead of camera-management.
  ranged_override: ranged STACKS WITH MELEE inside cyclone ring

interrupt_priority:
  - Ertan Lightning Bolt                    # MEDIUM_INTERRUPT — ease healer
  # No other kicks required

dispel_priority: []                         # no dispels required

heroic_delta: |
  - Lurking Tempest add spawns at platform edges (~30-45s cadence)
  - Cyclone retract damage higher; gap-dodge tighter
  - Lightning Bolt hits harder; interrupt rotation more meaningful

bot_role_summary:
  tank: hold threat through cyclone retracts; face platform center on heroic
  healer: pre-HoT before each cyclone retract event (predictable ~30s timer); light pressure overall
  melee: stack inside ring; rotate Lightning Bolt interrupts
  ranged: STACK WITH MELEE inside cyclone ring — explicit override of default ranged-spread; SUPPRESS Lurking Tempest targeting on heroic
```

---

## Boss 2 — Altairus

```yaml
boss: altairus
npc_id: 43873
location: second encounter, Nimbus Rise platform (post-bridge / post-slipstream from Ertan)
hp_pct_phase_triggers: []
notes: |
  Single-phase Storm Drake fight with continuous Call the Wind direction
  shifts (~every 30s) and periodic random-target Chilling Breath cone casts.

  Defining mechanic: wind direction. Standing UPWIND of Altairus = haste
  buff; standing DOWNWIND = haste/cast/move debuff. Group repositions on
  every direction shift.

phases:
  - id: 1
    name: regular
    trigger: pull
    end_condition: hp_pct == 0
    description: |
      Tank holds Altairus center; group positions UPWIND of boss; reposition
      on every Call the Wind direction shift. Heroic: dodge Twisting Winds
      patrolling tornadoes throughout.

mechanics:
  - name: Call the Wind
    spell_id: UNVERIFIED                    # scripted, drives directional aura
    cast_by: altairus
    cast_kind: scripted
    target: platform
    cast_time_ms: 0
    cooldown_ms: 30000                      # direction shifts every ~30s
    effect: |
      Establishes wind direction (one of 4 cardinal). Players upwind of
      Altairus gain Upwind buff; players downwind gain Downwind debuff.
    interruptible: false
    dispel_type: none
    priority: REPOSITION_TRIGGER

  - name: Upwind of Altairus
    spell_id: UNVERIFIED
    cast_by: aura_emitter (call_the_wind)
    cast_kind: aura
    target: ally_upwind_of_boss
    effect: |
      +30-100% haste/cast speed (magnitude UNVERIFIED — see survey
      Disagreement #2). +30% movement speed. WANTED buff.
    interruptible: false
    dispel_type: none                        # do not dispel — mechanic-positive

  - name: Downwind of Altairus
    spell_id: UNVERIFIED
    cast_by: aura_emitter (call_the_wind)
    cast_kind: aura
    target: ally_downwind_of_boss
    effect: |
      -30-100% haste/cast speed. -30% movement. PUNISHMENT debuff.
    interruptible: false
    dispel_type: none                        # NOT dispellable — positional
    notes: |
      Bot dispel logic must NOT attempt to dispel Downwind. Wasted GCD;
      removed only by repositioning to Upwind side.
    DISPEL_BLACKLIST: true

  - name: Chilling Breath
    spell_id: 88308
    cast_by: altairus
    cast_kind: cast
    target: frontal_cone (random target — boss aims at chosen target, stays aimed through cast)
    cast_time_ms: 2000                      # UNVERIFIED — likely 2s
    cooldown_ms: 15000-25000                # UNVERIFIED — periodic
    effect: |
      Frontal 120° Frost cone.
      Normal: 25,500-34,500 Frost damage
      Heroic: 47,500-52,500 Frost damage
    interruptible: UNVERIFIED                # most sources say no; one says yes
    dispel_type: none
    avoidable: true
    avoidance: |
      Targeted player MOVES to point cone away from group, OR group stays
      spread enough that only the targeted player is in cone arc.
    priority: CONE_AVOIDANCE

  - name: Twisting Winds
    spell_id: 88314
    cast_by: altairus (heroic)
    cast_kind: passive_environmental
    target: ground_patrolling
    effect: |
      Heroic-only patrolling mini-tornadoes. Contact: 11,700-12,300 Nature
      damage + KNOCKUP (carries fall-off-platform risk).
    interruptible: false
    dispel_type: none
    avoidable: true
    avoidance: continuous evasion; do not stand still
    heroic_only: true

  - name: Lightning Blast
    spell_id: UNVERIFIED
    cast_by: altairus
    cast_kind: reactive
    target: player_leaving_platform
    effect: 55,500-64,500 Nature damage (anti-leash)
    interruptible: false
    dispel_type: none
    notes: not relevant to in-platform combat

  - name: melee
    cast_by: altairus
    cast_kind: melee
    target: tank
    effect: standard Storm Drake melee

# Adds
adds: []                                    # Altairus has no adds

positioning:
  default: |
    Tank holds Altairus near platform CENTER, facing AWAY from group.
    All players (tank included) want UPWIND of Altairus buff — position
    so the boss is downwind of you (i.e., you're UPSTREAM of the wind).
  tank_facing: away from group (standard); reposition on every wind shift
  los_or_lookaway: none
  wind_shift_response: |
    On direction-shift event (every ~30s), all players REPOSITION to new
    upwind side. This is a continuous spatial tracking requirement.
  chilling_breath_response: |
    Targeted player MOVES to point cone away from group. Other players
    spread to limit clipping.
  twisting_winds_response: |
    HEROIC ONLY. Continuous evasion; do not stand still. Center-of-platform
    is safer than edges (knockup carries fall risk).

interrupt_priority: []                      # Chilling Breath UNVERIFIED interruptibility; not in rotation by default

dispel_priority: []                         # Downwind is NOT dispellable — explicit blacklist

heroic_delta: |
  - Twisting Winds patrolling tornadoes (NEW MECHANIC)
  - Chilling Breath damage roughly doubles
  - Wind-shift reposition window tighter

bot_role_summary:
  tank: standard tank-positioning + reposition on wind shifts; want Upwind buff like everyone else
  healer: pre-HoT Chilling Breath target if predictable; reposition with group; on heroic, dodge Twisting Winds
  melee: in melee; reposition with group on wind shift; dodge Twisting Winds
  ranged: spread for Chilling Breath, then bunch UPWIND for buff; reposition on wind shift

dispel_blacklist:
  - { spell_id: UNVERIFIED, name: Downwind of Altairus, reason: positional debuff — not dispellable }
```

---

## Boss 3 — Asaad, Caliph of Zephyrs

```yaml
boss: asaad_caliph_of_zephyrs
npc_id: 43875
location: third (final) encounter, Temple of Asaad platform after second slipstream
hp_pct_phase_triggers: []
notes: |
  Single-phase Djinn boss with three concurrent event types:
  - Periodic Skyfall Star summons (continuous throughout fight)
  - Periodic Chain Lightning on random target (group spread 15y+)
  - HEROIC ONLY: Static Cling root (jump-avoid OR Magic-dispel)
  - Major event: Unstable Grounding Field channel → Supremacy of the Storm
    (group MUST be inside triangle when storm fires)

phases:
  - id: 1
    name: regular
    trigger: pull
    end_condition: hp_pct == 0
    description: |
      Tank holds Asaad center; ranged DPS focuses Skyfall Stars on spawn;
      group spreads 15y+ for Chain Lightning; on heroic, healer dispels
      Static Cling. Every ~50-70s, Unstable Grounding Field spawns a triangle
      and Asaad channels Supremacy of the Storm — group stacks inside
      triangle to survive.

mechanics:
  - name: Skyfall Star (summon)
    spell_id: UNVERIFIED                    # summon scripted
    cast_by: asaad
    cast_kind: scripted
    target: ground_random
    cast_time_ms: 0
    cooldown_ms: 30000-40000                # continuous spawn
    effect: spawns Skyfall Star add
    interruptible: false
    priority: ADD_SPAWN_TRIGGER

  - name: (Skyfall Star) Arcane Barrage
    spell_id: UNVERIFIED                    # NPC-ability variant of player Arcane Barrage
    cast_by: skyfall_star
    cast_kind: cast
    target: enemy_random
    cast_time_ms: 0                         # continuous casts back-to-back
    cooldown_ms: 0                          # spam-cast
    effect: |
      Normal: 5,400-6,600 Arcane damage (UNVERIFIED — see survey D#4)
      Heroic: ~10,000-11,000 Arcane damage
    school: arcane
    interruptible: UNVERIFIED                # likely yes, but kill is faster than kick
    dispel_type: none
    priority: KILL_PRIORITY_ABSOLUTE         # ranged DPS swap

  - name: Chain Lightning
    spell_id: 87622
    cast_by: asaad
    cast_kind: cast
    target: enemy_random
    cast_time_ms: 2000                      # UNVERIFIED
    cooldown_ms: 15000-25000                # UNVERIFIED
    effect: |
      25,437-29,562 Nature damage initial. Bounces 40y between targets, 50%
      reduced damage per bounce. Group spread 15y+ breaks the chain.
    interruptible: UNVERIFIED                # standard strat does not kick
    dispel_type: none
    avoidable: partial
    avoidance: maintain 15y+ spread between all players
    priority: GROUP_SPREAD

  - name: Static Cling
    spell_id: 87618
    cast_by: asaad
    cast_kind: cast
    target: enemy_random_typically_ranged    # 2 players preferred per warcraft.wiki.gg
    cast_time_ms: 2000                      # UNVERIFIED
    cooldown_ms: 30000                       # casts twice between Supremacy events per Glow's guide
    effect: |
      Players touching ground when cast END resolves are ROOTED for 18s.
      AVOIDABLE by JUMP at cast end (sub-second timing — bot does NOT rely
      on this).
    school: nature
    dispel_type: magic                       # MAGIC dispel — REQUIRED on heroic
    interruptible: false
    heroic_only: true                        # NORMAL mode does not have Static Cling
    priority: MAGIC_DISPEL_HIGH
    notes: |
      Bot dispel logic: dispel EVERY application (do NOT optimize for "only
      second cast" — see survey Disagreement #5; safer-default).
    bot_jump_timing: NOT_RECOMMENDED         # sub-second timing unreliable for bot AI

  - name: Unstable Grounding Field
    spell_id: 86911
    cast_by: asaad
    cast_kind: channel
    target: ground_random_triangle
    cast_time_ms: 5000                       # UNVERIFIED ~5s channel
    cooldown_ms: 50000-70000                 # major event cadence
    effect: |
      Spawns a TRIANGULAR safe zone on the ground at a random location.
      Spell-targeting redirected away from units inside the field. Channel
      ends → Supremacy of the Storm fires.
    interruptible: false
    dispel_type: none
    avoidable: false
    priority: TRIANGLE_STACK_TRIGGER

  - name: Supremacy of the Storm
    spell_id: 86930
    cast_by: asaad
    cast_kind: channel
    target: pbaoe_platform_excluding_triangle
    duration_ms: 6000
    effect: |
      MASSIVE Nature damage outside the triangle (one-shots).
      19,475-21,525 Nature damage per 0.5s tick (per warcraft.wiki.gg)
      INSIDE the triangle, damage is suppressed to survivable levels.
    interruptible: false
    dispel_type: none
    avoidable: true
    avoidance: STAND INSIDE the Unstable Grounding Field triangle
    priority: TRIANGLE_STACK_MANDATORY

  - name: Lightning Strike (auto-attack)
    spell_id: UNVERIFIED
    cast_by: asaad
    cast_kind: melee_or_ranged_auto
    target: tank (current_target)
    effect: standard Asaad auto-attack on tank (Nature damage)

# Adds
adds:
  - name: Skyfall Star
    count: continuous_spawn
    locked_to: ground_random_spawn
    abilities:
      - name: Arcane Barrage (continuous cast on random player)
    kill_priority: ABSOLUTE                  # ranged DPS swap on every spawn

positioning:
  default: |
    Tank holds Asaad in platform CENTER. All other players SPREAD 15y+
    for Chain Lightning. Ranged DPS handles Skyfall Stars on spawn.

    On Unstable Grounding Field spawn (every ~50-70s):
    - All players IMMEDIATELY move INTO the triangle (random location each cast)
    - Stack inside until Supremacy of the Storm channel ends (~6s)
    - Resume spread positioning after
  tank_facing: away from group (standard)
  los_or_lookaway: none
  triangle_stack_response: |
    On Grounding Field spawn → ALL move into triangle.
    Triangle position is RANDOM per cast (not memorized).
    Bot AI: spatial query "point in triangle" + path to nearest interior point.
  static_cling_response: |
    HEROIC ONLY.
    Primary defense: MAGIC DISPEL the rooted player.
    Secondary defenses (class-specific):
    - Hand of Freedom (Paladin)
    - Cleanse (Paladin self / friendly)
    - Lichborne (DK; talented)
    - Druid Powershift
    - Anti-Magic Shell (DK; every other cast)

interrupt_priority: []                       # no kick-mandatory casts

dispel_priority:
  - { spell_id: 87618, name: Static Cling, dispel_type: magic, heroic_only: true, priority: HIGH }

heroic_delta: |
  - Static Cling (NEW MECHANIC) — Magic dispel role required
  - Skyfall Star damage higher; spawn cadence may be slightly faster
  - Chain Lightning damage higher
  - Supremacy of the Storm tick damage may be higher (UNVERIFIED — universally
    described as "one-shot regardless")

bot_role_summary:
  tank: tank-and-spank in center; one of the simpler tank fights in the dungeon
  healer: |
    Magic-dispel Static Cling on heroic (high priority before Grounding
    Field channel completes); reactive heal Chain Lightning + Skyfall
    Arcane Barrage damage; ride out Supremacy of the Storm in triangle
  melee: maintain 15y+ spread (yes, melee too — Chain Lightning bounces); move into triangle on Grounding Field spawn
  ranged: |
    PRIMARY = Skyfall Star kill priority (continuous swap); spread 15y+;
    on heroic, target of choice for Static Cling (jump-avoid is NOT
    bot-reliable — eat root + dispel)
```

---

## Instance-wide notes

```yaml
instance_wide:
  curse_dispel_required: false
  disease_dispel_required: false
  poison_dispel_required: true              # Empyrean Assassin trash (Lethargic Poison)
  magic_dispel_required: true                # Asaad heroic (Static Cling) + Turbulent Squall trash (purge)
  dispel_blacklist:
    - { spell_id: UNVERIFIED, name: Downwind of Altairus, reason: positional debuff — not dispellable }
    # No fight-specific dispel blacklists like BRC's Crepuscular Veil
  group_buff_anomaly:
    - { name: Upwind of Altairus, scope: altairus_fight, type: positional_buff, behavior: maintain }
  required_consumables: standard (flask + food + pots per spec)

# BOT-IMPLEMENTATION FLAGS — CRITICAL FOLLOWUPS
bot_implementation_flags:
  slipstream_traversal:
    severity: BLOCKER
    description: |
      The dungeon REQUIRES vehicle-traversal between platforms via Slipstream
      tornadoes. Players click a tornado object → mounted as vehicle → ride
      across open sky → dismount on next platform.

      cata-altbot does NOT currently handle vehicle entry / exit reliably
      (similar gap to Oculus drake fight per leveling.md Open Issues block,
      though lighter-weight here — no vehicle-combat phase, just one-way
      traversal).
    workaround_until_fixed: |
      Bot must be MASTER-INSTRUCTED to use the slipstream — bot follow-the-
      tank logic should pause at slipstream-pickup points; master clicks the
      tornado to mount the bot. Alternatively, master pulls bot through the
      slipstream by leading with their own slipstream traversal.

      Worst case: bot stays at the entrance / current platform; rest of
      group clears boss without that bot.
    module_work_needed: |
      Extend AltbotFollow.cpp with an interaction-handler:
      - Detect slipstream gameobject under tank's path
      - Issue right-click on slipstream
      - Wait for vehicle-mount confirmation
      - Re-acquire tank position post-dismount
    test_dungeons: [the-vortex-pinnacle]
    related_issues: [oculus-vehicle, escort-NPC-follow]

  fall_off_platform_recovery:
    severity: HIGH
    description: |
      Falling off any platform = teleport to dungeon entrance. NOT death
      (no soulstone / corpse run), but the player is now far from the
      group and needs to slipstream back.

      Hazards: Howling Gale bridge knock-off, Altairus Twisting Winds
      knockup, Asaad knockback (some sources mention; UNVERIFIED).
    workaround_until_fixed: |
      Bot path-finding must keep agents away from platform edges.
      AltbotFollow.cpp follow-distance should err CENTRAL when on Vortex
      Pinnacle platforms.

      If a bot does fall, it must auto-route back via slipstream (depends
      on slipstream_traversal fix).
    module_work_needed: |
      AltbotFollow.cpp edge-avoidance per-zone override; slipstream re-entry
      logic.

  triangle_position_query:
    severity: MEDIUM
    description: |
      Asaad's Unstable Grounding Field spawns a 3-vertex polygon (triangle)
      at a random location. All players must enter triangle before Supremacy
      of the Storm channel ends.

      Bot AI needs a `point_in_triangle(self.pos, triangle_vertices)`
      predicate AND a "move to nearest interior point" pathing override
      that fires on Grounding Field spawn.
    workaround_until_fixed: |
      Bot can heuristic by following the master/tank into the triangle (if
      master moves first). Risk: master is dead / disconnected → bots fail
      mechanic.
    module_work_needed: |
      Encounter event handler for spell ID 86911 → spatial-query helper +
      pathing override.

  static_cling_dispel_priority:
    severity: LOW                            # already covered by general dispel logic
    description: |
      Asaad heroic Static Cling (87618) is Magic dispel. Bot healer must
      include this in dispel-priority list when in this dungeon on heroic.
    workaround_until_fixed: |
      Generic Magic-dispel rule applies; no special handling needed beyond
      adding spell ID to dungeon-level dispel priority list.
    module_work_needed: none beyond doc-driven spell-ID inclusion in dispel logic

  lurking_tempest_target_suppression:
    severity: LOW
    description: |
      Heroic Ertan: Lurking Tempest adds harmlessly idle if no player faces
      / targets them. Bot AI must NOT auto-acquire tempest as target.
    workaround_until_fixed: |
      Encounter-level target-suppression list referencing Lurking Tempest
      NPC ID. If a tempest IS engaged (somehow targeted by a player), bots
      can secondary-fall-back to "kill the active tempest."
    module_work_needed: |
      Per-encounter target-acquisition exclusion list.
```

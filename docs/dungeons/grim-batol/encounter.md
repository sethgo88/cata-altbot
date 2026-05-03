# Grim Batol — Encounter Facts

**Role-neutral mechanic data** for the bot's encounter handling. Per-role response logic lives in sibling files (`healer.md`, `ranged-dps.md`, `melee-dps.md`, `tank.md`). When a role doc references a mechanic, it cites the spell ID defined here.

Spell IDs verified at `wowhead.com/cata/spell=<id>` and `wowhead.com/cata/npc=<id>` where consensus could be reached; UNVERIFIED items are aggregated in `docs/research/dbc-verification-checklist.md`. Strategy reconciled across 6 sources — see `docs/research/grim-batol-guide-survey.md` for the audit trail.

---

## Overview

```yaml
dungeon: grim_batol
zone: grim_batol
continent: eastern_kingdoms_twilight_highlands
difficulty_modes: [normal, heroic]
level_range_normal: [84, 85]
level_range_heroic: 85
boss_count: 4
optional_bosses: []                          # no skippable bosses (unlike BRC's Beauty)
bosses_in_order:
  - general_umbriss
  - forgemaster_throngus
  - drahga_shadowburner
  - erudax_duke_of_below
notes: |
  Linear path with a mid-instance bombing-run / drake-rescue trash-skip
  sequence (player-coordinated; not bot-automated by default). Trash density
  is high. Heroic difficulty was widely flagged as one of the hardest 5-mans
  at Cata launch — Erudax in particular has a hard DPS check on Faceless
  Corruptor adds.

  No instance-wide buff/debuff. No instance-wide curse/disease/poison dispel
  requirement. Magic dispels matter on Faceless Corruptor (Shield of
  Nightmares) and possibly Throngus (Disorienting Roar — UNVERIFIED).
```

---

## Trash — notable mechanics

Only the trash entries with mechanics worth automating. Standard packs (basic Twilight grunts, low-elite mobs) are pure threat-and-DPS and not enumerated.

```yaml
trash:
  # Pre–Umbriss (entrance hallway + first chamber)
  - mob: Twilight Cultist
    abilities:
      - name: Shadow Bolt cast
        interruptible: true
        priority: MUST_INTERRUPT
    notes: humanoid caster; Polymorph / Hex / Sap valid; CC primary

  - mob: Skardyn (Trogg)
    abilities:
      - name: melee_with_bleed
        target: tank
        notes: standard physical bleed; not dispellable
    notes: same model as Umbriss adds; AoE-burnable

  - mob: Twilight Drakonid
    abilities:
      - name: melee_cleave
        target: frontal_cone
        notes: tank face away
    notes: courtyard pack mob; CC-immune in standard strat

  # Post-Umbriss bombing-run sequence (drake-rescue)
  - mob: Captured / Freed Red Drake
    notes: |
      Friendly NPC after rescue scripted event. Players can mount and bomb
      large trash packs from above (skip optional). Bot DEFAULT: linear
      clear, do NOT assume bombing skip — skip is a player-coordinated
      optimization, not bot-automatable in v1.

  # Pre–Throngus
  - mob: Twilight Cultist + Drakonid pairs
    notes: caster CC valid on Cultist; tank face Drakonid away

  - mob: Twilight Demolisher                # large mob
    abilities:
      - name: frontal_cleave
        target: frontal_cone
        notes: side/rear positioning required for melee
    notes: tank-and-spank; high HP

  # Pre–Drahga
  - mob: Twilight Beguiler
    abilities:
      - name: caster_hardcasts
        interruptible: true
        priority: MUST_INTERRUPT
    notes: humanoid caster; CC valid

  - mob: Drakonid (approach hallway)
    abilities:
      - name: melee_cleave
        target: frontal_cone

  # Pre–Erudax (rookery approach)
  - mob: Faceless humanoid (rookery)
    abilities:
      - name: dark_magic_hardcasts
        interruptible: true
        priority: MUST_INTERRUPT
    notes: |
      SAME mob family as Erudax-fight Faceless Corruptors but lower-HP trash
      version. Treat hardcasts as kick-targets.

  - mob: Mature Twilight Drake
    abilities:
      - name: melee_cleave
        target: frontal_cone
      - name: tail_swipe
        target: rear_arc
    notes: large flying NPC; tank face-aware on BOTH front and rear

  - mob: Ambient Twilight Hatchling          # rookery floor
    abilities:
      - name: melee_low_damage
    notes: low-HP eggs of the same family that spawn during Erudax encounter; AoE-burnable
```

### Trash skip notes

- **Bombing-run sequence (post-Umbriss courtyard)**: skippable via drake bombing; player-coordinated. **Default bot behavior: linear clear**.
- A few mob clusters along the rookery approach can be wall-hugged on speed-clears.
- Rookery ambient eggs / hatchlings are NOT the same as encounter-spawned eggs (Alexstrasza's Eggs are encounter-specific).

### CC priorities

- **Polymorph / Hex / Sap / Banish** valid on Twilight humanoid casters (Cultists, Beguilers, Faceless trash)
- **Hibernate** (Druid) valid on Twilight Drakes
- **Freezing Trap** (Hunter) valid on most non-elementals
- Skardyn / Trogg / Drakonid melee adds — generally not standard CC targets

---

## Boss 1 — General Umbriss

```yaml
boss: general_umbriss
npc_id: 39625
location: first chamber, after entrance ramp
hp_pct_phase_triggers: [30]                  # Frenzy enrage
notes: |
  Single-phase tank-and-spank with periodic charge avoidance, cone stun, and
  add waves. Critical mechanic is Modgud's Malice add-positioning: Malignant
  Trogg MUST die away from boss or Umbriss gets +100% damage (stacking).

phases:
  - id: 1
    name: regular
    trigger: pull
    end_condition: hp_pct == 30
    description: tank-and-spank with Blitz / Ground Siege / add-wave cycle

  - id: 2
    name: frenzy_execute
    trigger: hp_pct == 30
    end_condition: hp_pct == 0
    description: Frenzy enrage active (+50% attack speed, +50% damage); finish off

mechanics:
  - name: Bleeding Wound
    spell_id: 74846
    cast_by: general_umbriss
    cast_kind: melee_application
    target: tank
    duration_ms: 10000
    school: physical (bleed)
    effect: |
      Tank-targeted bleed DoT, 10s. Community guides report it falls off when
      target heals above 90% HP (UNVERIFIED via Wowhead — flag for DBC).
    interruptible: false
    dispel_type: none                        # not dispellable; bleed
    avoidable: false
    priority: TANK_HEAL_PRIORITY

  - name: Blitz
    spell_id: 74670
    cast_by: general_umbriss
    cast_kind: cast
    target: ally_random_non_tank
    cast_time_ms: 3000                        # ~3s telegraph
    effect: charges to target's location; massive physical damage + knockback within 6 yards
    interruptible: false                     # mechanic-intended
    dispel_type: none
    avoidable: true
    avoidance: |
      TARGETED player and anyone IN PATH between Umbriss and target moves
      perpendicular at telegraph.

  - name: Ground Siege
    spell_id: 74634
    cast_by: general_umbriss
    cast_kind: cast
    target: pbaoe_self                        # Wowhead lists 20-yard radius PBAoE
    cast_time_ms: 2000
    effect: AoE damage + 4s stun
    interruptible: UNVERIFIED                 # not flagged on Wowhead; community guides imply non-interruptible
    dispel_type: none
    avoidable: true
    avoidance: move out of telegraphed zone (community guides describe it as cone-style — see survey)

  - name: Frenzy
    spell_id: 96800
    cast_by: general_umbriss
    cast_kind: scripted
    target: self
    trigger: hp_pct <= 30
    effect: +50% attack speed, +50% damage
    interruptible: false
    dispel_type: UNVERIFIED                   # possibly soothable (Tranq Shot / Soothe / Shiv) — flag for DBC
    avoidable: false
    priority: TANK_DEFENSIVE_CD_TRIGGER

  - name: Summon Skardyn
    spell_id: 74859
    cast_by: general_umbriss
    cast_kind: scripted
    effect: spawns 1 Malignant Trogg + 3 Trogg Dwellers
    interruptible: false
    cadence: periodic during fight (UNVERIFIED interval)

# Adds
adds:
  - name: Malignant Trogg
    npc_id: 39984
    count: 1 per Summon Skardyn
    abilities:
      - name: Modgud's Malice
        spell_id: 74699
        cast_kind: death_trigger              # cast when add dies
        target: pbaoe_self_on_death
        effect: |
          ON DEATH, applies a buff. If Umbriss is in radius, Umbriss gains
          +100% damage (STACKING). CRITICAL POSITIONING — Malignant Trogg
          MUST die away from boss.
        priority: KILL_AWAY_FROM_BOSS
      - name: Modgud's Malady
        spell_id: 74837
        cast_kind: melee_application
        target: melee_target
        effect: stacking bleed
      - name: Claw Puncture
        spell_id: 76507
        cast_kind: melee
    handling: |
      Tank picks up but does NOT bring to boss (or off-tank kites). Ranged
      DPS slow + kite + burst kill far from Umbriss. CC valid (UNVERIFIED
      mob-flagging — Hex/Polymorph if humanoid; Banish if Demon).

  - name: Trogg Dweller
    npc_id: 45467
    count: 3 per Summon Skardyn
    abilities:
      - name: Claw Puncture
        spell_id: 76507
    handling: |
      AoE-burnable safely (no Modgud's Malice on death). Tank can pick all
      up at boss; standard cleave DPS.

positioning:
  default: |
    Tank in CENTER, facing AWAY from group (Ground Siege cone discipline).
    Ranged SPREAD (Blitz multi-hit prevention).
    Malignant Trogg KITED to a corner away from boss.
  tank_facing: away from group
  los_or_lookaway: none
  kill_zone: |
    Malignant Trogg kill location MUST be > Modgud's-Malice-radius from
    boss (UNVERIFIED radius — assume 20 yards; verify via DBC).

interrupt_priority: []                        # no kick-required casts

dispel_priority:
  - { spell: Frenzy, dispel_type: UNVERIFIED, action: SOOTHE_IF_AVAILABLE }

heroic_delta: |
  - Blitz reportedly one-shots undergeared players
  - Trogg adds hit harder; Modgud's Malady stacks more dangerous on melee
  - Frenzy at 30% more punishing — defensive CD on rotation
  - GuiasWow describes "Infested Trogg explodes on death" variant (UNVERIFIED;
    not on Wowhead encounter journal — see survey Disagreement #4)

bot_role_summary:
  tank: hold boss central; pick up Trogg Dwellers; do NOT bring Malignant Trogg to boss
  healer: pre-shield Blitz target if predictable; sustained tank healing on Bleeding Wound; pop CD at 30% Frenzy
  melee: stay behind boss; move out of Ground Siege telegraph; AoE Trogg Dwellers
  ranged: spread for Blitz; slow + burst kill Malignant Trogg AWAY from boss
```

---

## Boss 2 — Forgemaster Throngus

```yaml
boss: forgemaster_throngus
npc_id: 40177
location: second encounter, arena chamber
hp_pct_phase_triggers: []
notes: |
  Three-stance random rotation (Pick Weapon ability cycles ~30s):
  - SWORD: highest tank damage; Disorienting Roar AoE haste reduction
  - MACE: kite-phase; -70% movement on boss but +200-300% damage; tank does
    NOT stand-tank
  - SHIELD: Personal Phalanx (boss roots + pacifies self); group must DPS
    from BEHIND boss; Flaming Arrows from chamber walls
  Universal: Mighty Stomp → Cave In ground patches drop on random players.

phases:
  - id: 1
    name: random_stance_cycle
    trigger: pull
    end_condition: hp_pct == 0
    description: |
      Continuous fight; weapon-stance rotates ~every 30s in random order. No
      HP-gated phases. Bot detects current stance via aura/buff on boss
      (UNVERIFIED stance-marker spell ID).

mechanics:
  # Universal mechanics (all stances)
  - name: Mighty Stomp
    spell_id: 74984
    cast_by: forgemaster_throngus
    cast_kind: cast
    target: pbaoe_telegraph
    cast_time_ms: 2000
    effect: triggers Cave In ground patches at random player locations
    interruptible: UNVERIFIED                 # 2s cast; possibly kickable
    dispel_type: none
    avoidable: false                          # cast itself; ground patches avoidable

  - name: Cave In
    spell_id: 74987
    cast_by: forgemaster_throngus
    cast_kind: ground_persistent_aura
    target: random_player_locations
    radius_yards: 6
    effect: persistent ground patch; periodic ticks; physical damage
    interruptible: false
    dispel_type: none
    avoidable: true
    avoidance: move out of patch

  # Sword stance
  - name: Disorienting Roar
    spell_id: 74976
    cast_by: forgemaster_throngus
    cast_kind: cast
    cast_time_ms: 1500
    target: pbaoe_self
    effect: AoE -50% haste (melee, ranged, spell)
    interruptible: UNVERIFIED                 # 1.5s cast; possibly kickable
    dispel_type: UNVERIFIED                   # Wowhead "n/a" but Glowberry claims Magic dispel — see survey Disagreement #2
    DEFAULT_DISPEL_BEHAVIOR: NO_AUTO_DISPEL   # bot defaults to non-dispellable; flag for DBC
    stance: sword

  - name: Dual Blades
    spell_id: 74981
    cast_by: forgemaster_throngus
    cast_kind: passive_buff
    target: self
    effect: rapid melee swings on tank
    stance: sword

  - name: Thrash
    spell_id: 47480
    cast_by: forgemaster_throngus
    cast_kind: passive_buff
    target: self
    effect: bonus melee swings on tank
    stance: sword

  # Shield stance
  - name: Personal Phalanx
    spell_id: 74908
    cast_by: forgemaster_throngus
    cast_kind: applied_on_shield_pick
    target: self
    duration_ms: 30000
    effect: |
      Boss SELF-ROOTS and SELF-PACIFIES; triggers Glancing Blows periodic.
      Provides 99% damage reduction from frontal direction.
      Group must DPS from BEHIND boss during this stance.
    stance: shield

  - name: Flaming Arrow
    spell_id: 45101
    cast_by: chamber_archers
    cast_kind: ranged_ground_deploy
    target: random_floor_patch
    effect: fire ground patch
    interruptible: false
    avoidable: true
    avoidance: move out
    stance: shield

  # Mace stance
  - name: Encumbered
    spell_id: 75007
    cast_by: forgemaster_throngus
    cast_kind: passive_buff
    target: self
    effect: -70% movement speed; +200-300% damage
    stance: mace

  - name: Impaling Slam
    spell_id: 75057
    cast_by: forgemaster_throngus
    cast_kind: instant
    target: random_ally
    effect: jumps to random target; deals damage + 5s DoT
    interruptible: false
    dispel_type: UNVERIFIED                   # DoT may be Magic
    stance: mace

  # UNVERIFIED universal/mace mechanic
  - name: Lava Patch (Mace stance fire trail)
    spell_id: UNVERIFIED                      # not on Wowhead encounter journal
    cast_by: forgemaster_throngus
    cast_kind: passive_along_kite_path
    target: ground_behind_boss
    effect: fire ground patch left along boss kite path
    interruptible: false
    avoidable: true
    avoidance: |
      Tank kites in pre-planned arc; melee follows BEHIND tank; ranged stays
      OFF kite path. Multiple sources mention this; Wowhead omits — flag for
      DBC (see survey Disagreement #5).
    stance: mace_only_per_some_sources_or_universal

# Stance detection
stance_detection:
  method: aura_on_boss
  stance_aura_ids:
    sword: UNVERIFIED                         # stance buff ID; flag for DBC
    mace: 75007                               # Encumbered is the mace-stance signature
    shield: 74908                             # Personal Phalanx is the shield-stance signature
  cycle_duration_seconds: 30                  # UNVERIFIED — community-listed
  rotation_order: random

positioning:
  default_per_stance:
    sword:
      tank: face away from group; standard rear-arc melee
      melee: behind boss
      ranged: max range; spread
      facing: away from group
    mace:
      tank: KITE in circular arc around chamber; do NOT stand-tank
      melee: follow at ~5-10 yards behind tank; do NOT step into kite-path lava
      ranged: max range from kite path
      facing: tank rotates boss face per kite arc
    shield:
      tank: hold boss still (boss self-roots); face boss SHIELD-side AWAY from DPS group
      melee: BEHIND boss (Phalanx 99% reduction from front)
      ranged: max range; behind boss for arc; out of Flaming Arrow patches
      facing: shield_side_away_from_group

  universal:
    move_out_of: cave_in_patches
    no_overlap: cave_in_persistent_patches_stack_with_movement

interrupt_priority:
  - { spell: Disorienting Roar, action: ATTEMPT, notes: UNVERIFIED interruptibility — kick if available }
  - { spell: Mighty Stomp, action: ATTEMPT, notes: UNVERIFIED interruptibility — kick if available }

dispel_priority:
  - { spell: Disorienting Roar, dispel_type: UNVERIFIED, default: NO_AUTO_DISPEL }

heroic_delta: |
  - Mace stance damage especially severe; tank cannot stand-tank
  - Disorienting Roar may stack on heroic (per GuiasWow heroic delta)
  - Flaming Arrows hit harder; precise avoidance matters
  - Lava trail / fire patches more pronounced (UNVERIFIED — Disagreement #5)

bot_role_summary:
  tank: detect stance via aura; switch behavior per stance (Sword=stand-tank, Mace=kite, Shield=hold-still-shield-away)
  healer: heaviest pressure during Sword stance; recover during Shield (mobility-low); reactive on Impaling Slam jump-target
  melee: stance-conditional positioning (behind-always, BEHIND-shield-side during Shield, follow-tank-during-Mace)
  ranged: max range; spread for Cave In; behind during Shield stance
```

---

## Boss 3 — Drahga Shadowburner

```yaml
boss: drahga_shadowburner
npc_id: 40319
location: third encounter, raised platform overlooking chamber
hp_pct_phase_triggers: [25]                  # Phase 2 trigger (Cata Classic) — see survey Disagreement #3
notes: |
  Two-to-three phase fight. Phase 1: Drahga ground-cast Burning Shadowbolts +
  Invocation of Flame summons. Phase 2 (at ~25% HP): Drahga MOUNTS VALIONA;
  dragon descends and engages directly with Shredding Swipe + Seeping
  Twilight + Devouring Flames cone breath. Phase 3 (Valiona departs at low
  HP): Drahga back on ground for execute.

phases:
  - id: 1
    name: drahga_ground
    trigger: pull
    end_condition: drahga.hp_pct == 25
    description: |
      Drahga casts Burning Shadowbolts at tank/random; Invocation of Flame
      summons Invoked Flaming Spirit adds that fixate random players.
      Group kills spirits before they reach target.

  - id: 2
    name: valiona_descent
    trigger: drahga.hp_pct == 25
    end_condition: valiona.hp_pct == 20         # UNVERIFIED — Valiona departs at low HP
    description: |
      Drahga MOUNTS Valiona. Dragon descends and is tanked directly. Valiona
      cleaves with Shredding Swipe (frontal cone), drops Seeping Twilight
      ground patches on random players, and casts Devouring Flames cone
      breath in a random direction (group MOVES BEHIND). Drahga continues
      to summon spirits via Invocation of Flame.

  - id: 3
    name: execute
    trigger: end of phase 2 (Valiona departs)
    end_condition: drahga.hp_pct == 0
    description: |
      Drahga back on ground; resume Phase 1 mechanics for execute. Tank
      stand-tanks Drahga; group bursts through final HP.

mechanics:
  - name: Burning Shadowbolt
    spell_id: 75245
    cast_by: drahga_shadowburner
    cast_kind: cast
    cast_time_ms: 1500
    school: fire+shadow
    target: ally_random_or_tank
    effect: ranged single-target damage
    interruptible: UNVERIFIED                 # 1.5s cast; community guides imply kickable
    dispel_type: none
    avoidable: false

  - name: Invocation of Flame
    spell_id: 75218
    cast_by: drahga_shadowburner
    cast_kind: instant
    target: ground_marker_then_summon
    duration_ms: 10000                        # 10s buff before spirit spawns
    effect: summons Invoked Flaming Spirit at portal location
    interruptible: false                      # instant
    dispel_type: none
    cadence: periodic (UNVERIFIED interval)

  - name: Supernova
    spell_id: 75238
    cast_by: invoked_flaming_spirit
    cast_kind: scripted_on_impact
    target: pbaoe_self
    school: fire
    effect: massive AoE damage; potential one-shot
    interruptible: false
    avoidable: true
    avoidance: |
      KILL the spirit before it reaches its fixated target. Slow / stun /
      burst DPS as priority.

  # Phase 2 — Valiona
  - name: Shredding Swipe
    spell_id: 75271
    cast_by: valiona
    cast_kind: melee_cleave
    target: frontal_cone
    school: physical
    effect: frontal cone melee on tank + anyone in cone
    interruptible: false
    avoidable: true
    avoidance: face Valiona AWAY from group (tank discipline)

  - name: Seeping Twilight
    spell_id: 75317
    cast_by: valiona
    cast_kind: ground_deploy
    target: random_player_location
    radius_yards: 10
    duration_pulse_ms: 1250
    school: shadow
    effect: shadow damage (~6336 normal / 9261 heroic) + 50% slow
    interruptible: false
    dispel_type: UNVERIFIED                   # slow may be Magic-dispellable
    avoidable: true
    avoidance: move out of patch

  - name: Devouring Flames
    spell_id: UNVERIFIED                      # not on Wowhead Drahga NPC page; community guides confirm — see survey Disagreement #6
    cast_by: valiona
    cast_kind: cast
    cast_time_ms: 2000-4000                   # UNVERIFIED
    target: frontal_cone_random_direction
    school: fire
    effect: heavy fire cone in random direction; can one-shot
    interruptible: UNVERIFIED
    avoidable: true
    avoidance: |
      ON CAST DETECTION, all DPS/healer MOVE BEHIND Valiona. Tank holds
      Valiona still; group rotates to her rear arc. Heroic-only per some
      sources; baseline per others — bot conservatively treats as always-on.

  - name: Valiona melee
    cast_by: valiona
    cast_kind: melee
    target: tank
    effect: standard tank melee in P2

# Adds
adds:
  - name: Invoked Flaming Spirit
    npc_id: UNVERIFIED                        # spawned by Invocation of Flame
    count_per_invocation: 1
    behavior: |
      Spawns at Invocation of Flame portal location. FIXATES on a random
      player. Runs at fixated target at high speed. On reaching target,
      casts Supernova (75238) for massive AoE damage.
    abilities:
      - name: Supernova (75238)
    kill_priority: ABSOLUTE_HIGHEST            # drop boss DPS, switch all DPS to spirit
    counter:
      - slow (Frostbolt slow, Curse of Exhaustion, Concussive Shot)
      - stun (Hammer of Justice, Stomp pet, Bash, etc.)
      - burst DPS

  - name: Valiona (Phase 2 only)
    npc_id: UNVERIFIED
    spawn_trigger: drahga.hp_pct == 25
    despawn_trigger: valiona.hp_pct == 20      # UNVERIFIED
    notes: |
      Tanked directly during Phase 2. Drahga rides Valiona; both share fight
      space. Valiona departs at low HP, returning fight to Phase 3 execute.

positioning:
  default:
    phase_1: |
      Tank holds Drahga at his perch / chosen spot. Ranged SPREAD for
      Invocation of Flame portal RNG. Melee in standard rear-arc.
    phase_2: |
      Tank pivots to Valiona; holds her facing AWAY from group (Shredding
      Swipe cone). Group repositions BEHIND Valiona on Devouring Flames
      cast detection. SPREAD from Seeping Twilight ground patches.
    phase_3: |
      Revert to Phase 1 positioning; standard tank-and-spank execute.
  tank_facing: away from group (P1 + P3); face Valiona away from group (P2)
  los_or_lookaway: none
  movement_pattern_p2: |
    Tank pre-plans a "behind-Valiona" anchor point — ideally, hold Valiona
    at one end of the chamber with open space behind for group to collapse
    into during Devouring Flames cast.

interrupt_priority:
  - { spell: Burning Shadowbolt (75245), action: ATTEMPT, notes: UNVERIFIED interruptibility }
  - { spell: Devouring Flames, action: ATTEMPT_BUT_POSITION_PRIMARY, notes: UNVERIFIED interruptibility — positioning is the canonical counter }

dispel_priority: []                            # no confirmed dispels

heroic_delta: |
  - Invoked Flaming Spirit hits MUCH harder — guaranteed one-shot if reaches target
  - Devouring Flames may only exist on heroic (some sources call it baseline; Disagreement #6)
  - More frequent Invocation of Flame in Phase 2 — multiple spirits at once possible
  - Seeping Twilight damage scales (6336 → 9261)

bot_role_summary:
  tank: P1 standard; P2 pivot to Valiona, hold facing AWAY from group, anchor for behind-Valiona repositioning during Devouring Flames
  healer: predictable damage spike on Devouring Flames if positioning off; heavy reactive on spirit-impact; tank damage spike on Valiona melee in P2
  melee: switch off boss to spirit on Invoked Flaming Spirit spawn; in P2, move behind Valiona on Devouring Flames cast detection
  ranged: spread for portal RNG; slow + burst spirit; collapse behind Valiona on breath cast
```

---

## Boss 4 — Erudax, the Duke of Below

```yaml
boss: erudax_duke_of_below
npc_id: 40484
location: rookery chamber, final encounter
hp_pct_phase_triggers: []
notes: |
  Single-phase but with a recurring Shadow Gale → Faceless Corruptor add-wave
  cycle every ~25-40s. Most complex 5-man encounter at Cata launch. Heroic
  has a hard DPS check on the Corruptor add wave (2 corruptors per cycle).

  CRITICAL safe-zone REVERSAL: Shadow Gale safe-zone is the EYE / center
  circle (stand IN, not OUT). Inverts the normal "fire on floor = avoid"
  rule. Bot must explicitly recognize Shadow Gale as a stand-in mechanic.

phases:
  - id: 1
    name: regular_cycle
    trigger: pull
    end_condition: hp_pct == 0
    description: |
      Tank-and-spank with Enfeebling Blow + Binding Shadows + periodic
      Shadow Gale → Faceless Corruptor add-wave cycle. No HP-gated
      phase transitions; mechanics rotate on cooldown.

mechanics:
  - name: Binding Shadows
    spell_id: 79466
    cast_by: erudax
    cast_kind: cast
    cast_time_ms: 1500
    target: ally_random_ranged
    school: shadow (Magic dispel per Wowhead spell page)
    effect: |
      Shadow damage + ROOTS target ~8s. Heals Erudax for 3x damage dealt
      (per community guides — UNVERIFIED for exact multiplier).
    interruptible: UNVERIFIED                 # 1.5s cast; possibly kickable
    dispel_type: magic
    DEFAULT_DISPEL_BEHAVIOR: NO_AUTO_DISPEL   # standard strat is to move out of telegraph; root resolves on duration
    avoidable: true
    avoidance: move OUT of telegraph at cast start

  - name: Enfeebling Blow
    spell_id: 75789
    cast_by: erudax
    cast_kind: instant
    target: tank
    school: shadow
    effect: |
      Tank-targeted; deals 75% weapon damage; KNOCKS tank back ~400 distance
      units; applies Feeble Body debuff.
    interruptible: false
    dispel_type: none

  - name: Feeble Body
    spell_id: 75792
    cast_by: erudax_via_enfeebling_blow
    cast_kind: applied_aura
    target: tank
    duration_ms: 5000
    school: shadow
    effect: +200% damage taken for 5s
    interruptible: false
    dispel_type: UNVERIFIED                   # likely non-dispellable
    avoidable: false
    priority: TANK_WAIT_OUT_BEFORE_RE_ENGAGE

  - name: Shadow Gale
    spell_id: 75694
    cast_by: erudax
    cast_kind: cast
    cast_time_ms: 5000                        # ~5s telegraph per community guides; Wowhead spell-page lists 1.25s
    target: room_wide
    school: shadow
    effect: |
      Whole-room AoE; SAFE ZONE is the EYE / center circle. Damage in OUTER
      zone, not inner zone. REVERSAL of normal "fire on floor = avoid"
      rule. Damage in safe-zone is doubled toward boss (per Dexerto).
    interruptible: false
    dispel_type: none
    avoidable: true
    avoidance: |
      ALL PLAYERS COLLAPSE TO CENTER SAFE ZONE on cast detection.
      EXPLICIT STAND-IN MECHANIC.
    priority: GROUP_COLLAPSE_TO_SAFE_ZONE

  - name: melee
    cast_by: erudax
    cast_kind: melee
    target: tank
    effect: standard boss melee

# Adds — Faceless Corruptor (NPC 48844)
adds:
  - name: Faceless Corruptor
    npc_id: 48844
    spawn_trigger: post_shadow_gale
    count_normal: 1
    count_heroic: 2
    spawn_location: room_entrance
    behavior: |
      Walks toward Alexstrasza's Eggs at outer ring. Casts Twilight
      Corruption on egg. If channel completes, egg hatches 2 Twilight
      Hatchlings.
    abilities:
      - name: Twilight Corruption
        spell_id: 75520
        cast_kind: channel
        cast_time_ms: 6000                    # UNVERIFIED — community-listed
        target: alexstraszas_egg
        effect: corrupts egg → hatches 2 Twilight Hatchlings on completion
        interruptible: UNVERIFIED              # channel; may not be kickable — kill priority is the canonical counter
        priority: PREVENT_CAST_VIA_KILL_OR_SLOW
      - name: Shield of Nightmares
        spell_id: 75809
        cast_kind: self_buff
        target: self
        duration_ms: 4000
        school: shadow
        effect: |
          Self-buff; reflects Manifested Nightmare AoE damage to attackers
          when Corruptor is hit. Standard strat: DISPEL (Magic) before
          continuing burn.
        dispel_type: magic
        DEFAULT_DISPEL_BEHAVIOR: AUTO_DISPEL   # active dispel target on Corruptor
      - name: Umbral Mending
        spell_id: 75763
        cast_kind: cast
        cast_time_ms: 2500
        target: ally_radius_100yd               # heals Corruptor + nearby allies (incl. Erudax) for 20% HP
        school: shadow
        effect: heals Corruptor + Erudax for 20% HP
        interruptible: true
        priority: MUST_INTERRUPT
      - name: Siphon Essence
        spell_id: 75755
        cast_kind: scripted
        target: alexstraszas_egg_radius
        school: shadow
        effect: drains essence from eggs (secondary mechanic; UNVERIFIED in 4.3.4 strat)
        interruptible: UNVERIFIED
        priority: ATTEMPT_INTERRUPT
      - name: melee
        target: closest_player
    kill_priority: ABSOLUTE_HIGHEST_ON_SPAWN
    handling: |
      Drop Erudax DPS on Corruptor spawn. Slow + burst kill before reaching
      egg. Dispel Shield of Nightmares (Magic). Interrupt Umbral Mending
      (highest cast-priority). Tank can stay on Erudax; ranged + melee
      handle Corruptor.

  - name: Twilight Hatchling
    npc_id: 39388
    spawn_trigger: twilight_corruption_completes_on_egg
    count_per_egg: 2
    abilities:
      - name: Twilight Blast
        spell_id: 99393
        cast_kind: cast
        target: ranged_or_aoe                  # UNVERIFIED — likely ranged single-target with splash
        effect: AoE shadow damage to group
        interruptible: UNVERIFIED
    handling: AoE-burnable; secondary kill priority after Corruptor

  - name: Alexstrasza's Egg
    npc_id: 40486
    type: ENVIRONMENTAL_OBJECT
    location: outer_ring_of_room
    notes: |
      Eggs are encounter-spawned objects, not the same as ambient rookery
      eggs. Faceless Corruptors target these. If corrupted, eggs spawn
      Twilight Hatchlings. Eggs themselves are not killable by players in
      standard strat.

positioning:
  default: |
    Tank holds Erudax in CENTER OF ROOM. Ranged SPREAD around perimeter
    (Binding Shadows targeting). Melee behind boss.
  tank_facing: away from group (standard)
  los_or_lookaway: none
  shadow_gale_response: |
    ALL PLAYERS COLLAPSE TO CENTER SAFE ZONE on cast detection. The center
    is the ONLY safe spot. Late entry is FATAL.
  corruptor_spawn_response: |
    Tank stays on Erudax. DPS (priority: ranged for slow + burst) intercepts
    Corruptor at room entrance ring; prevents Corruptor from reaching egg.
  feeble_body_response: |
    Tank, after being knocked back by Enfeebling Blow + Feeble Body
    application, WAITS OUT 5s before re-engaging Erudax (200% damage taken
    means stand-tanking during debuff is suicide).

interrupt_priority:
  - { spell: Umbral Mending (75763), priority: 1, action: MUST_INTERRUPT }     # highest priority
  - { spell: Twilight Corruption (75520), priority: 2, action: ATTEMPT_OR_KILL, notes: kill priority is canonical }
  - { spell: Siphon Essence (75755), priority: 3, action: ATTEMPT_INTERRUPT }
  - { spell: Binding Shadows (79466), priority: 4, action: ATTEMPT_IF_KICK_AVAILABLE, notes: positioning is canonical counter }

dispel_priority:
  - { spell_id: 75809, name: Shield of Nightmares, target: faceless_corruptor, dispel_type: magic, priority: 1, action: AUTO_DISPEL }
  - { spell_id: 79466, name: Binding Shadows, target: ally_player, dispel_type: magic, priority: 2, action: REACTIVE_ONLY, notes: do NOT auto-dispel; let root resolve unless player crippled }

heroic_delta: |
  - 2 Faceless Corruptors per Shadow Gale cycle (vs 1 normal) — DPS-check tightens
  - Shadow Gale damage higher; late entry to safe zone fatal
  - Enfeebling Blow knockback farther / more frequent
  - Heroic-only achievement: "Don't Need to Break Eggs to Make an Omelet"
    (defeat without any Corruptor reaching Twilight Corruption cast)

bot_role_summary:
  tank: hold Erudax central; absorb Enfeebling Blow knockback; WAIT OUT Feeble Body 5s before re-engaging; do NOT chase Corruptor
  healer: pre-AoE-heal CD on Shadow Gale telegraph; DISPEL Shield of Nightmares on Corruptor; do NOT auto-dispel Binding Shadows
  melee: gap-close + slow + interrupt on Corruptor spawn; Umbral Mending = MUST INTERRUPT; standard rear-arc on Erudax
  ranged: spread for Binding Shadows; collapse to center on Shadow Gale; switch to Corruptor on spawn; Magic dispel Shield of Nightmares (if hybrid)
```

---

## Instance-wide notes

```yaml
instance_wide:
  curse_dispel_required: false
  disease_dispel_required: false
  poison_dispel_required: false
  magic_dispel_required: true                   # on Erudax (Shield of Nightmares); UNVERIFIED on Throngus (Disorienting Roar)
  dispel_blacklist:
    - { spell_id: 79466, name: Binding Shadows, scope: erudax_fight, force: false, notes: REACTIVE_ONLY — let root resolve unless cripples positioning }
  dispel_priority:
    - { spell_id: 75809, name: Shield of Nightmares, scope: erudax_fight_corruptor_target, action: AUTO_DISPEL_HIGH_PRIORITY }
  group_buff_anomaly: none
  required_consumables: standard (flask + food + pots per spec)
  bombing_run_skip: |
    Available in 4.3.4. Player-coordinated. Bot DEFAULT: linear clear; do NOT
    automate bombing skip. Skip is a v2+ feature gate.
```

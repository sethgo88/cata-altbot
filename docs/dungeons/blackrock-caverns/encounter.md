# Blackrock Caverns — Encounter Facts

**Role-neutral mechanic data** for the bot's encounter handling. Per-role response logic lives in sibling files (`healer.md`, `ranged-dps.md`, `melee-dps.md`, `tank.md`). When a role doc references a mechanic, it cites the spell ID defined here.

Spell IDs verified at `wowhead.com/cata/spell=<id>` where consensus could be reached; many are flagged **UNVERIFIED** in this doc and aggregated in `docs/research/dbc-verification-checklist.md`. Strategy reconciled across 4-5 sources — see `docs/research/blackrock-caverns-guide-survey.md` for the audit trail.

---

## Overview

```yaml
dungeon: blackrock_caverns
zone: blackrock_caverns
continent: eastern_kingdoms_blackrock_mountain
difficulty_modes: [normal, heroic]
level_range_normal: [80, 82]
level_range_heroic: 85
boss_count: 4
optional_bosses: [beauty]                # skippable side branch
bosses_in_order:
  - rom_ogg_bonecrusher
  - corla_herald_of_twilight
  - karsh_steelbender
  - ascendant_lord_obsidius
notes: |
  Linear path: elevator → Rom'ogg chamber → ramp tunnel (Beauty branch + Corla
  room) → forge (Karsh) → throne room (Obsidius). Beauty's chamber forks off
  the main spine and is skippable without consequence.

  No instance-wide buff/debuff. No instance-wide curse/disease/poison dispel
  requirement. Fights are mostly tank-and-spank with one or two unique
  mechanic each (Skullcracker stack, Corla beam-block, Karsh kite-through-
  lava, Obsidius clone-swap).
```

---

## Trash — notable mechanics

Only the trash entries with mechanics worth automating. Standard packs (basic Twilight grunts, low-elite mobs) are pure threat-and-DPS and not enumerated.

```yaml
trash:
  # Pre–Rom'ogg
  - mob: Twilight Drakonid
    abilities:
      - name: melee_cleave
        target: frontal_cone
        notes: tank face away
    notes: standard pack mob; pull individually or in pairs

  - mob: Twilight Elementalist
    abilities:
      - name: Lava Spit
        interruptible: true
        priority: MUST_INTERRUPT
        notes: frontal_cone hardcast — turn away from group
    notes: caster trash; CC valid (Polymorph, Hex, Sap)

  - mob: Twilight Torturer
    abilities:
      - name: Red_Hot_Poker
        interruptible: true
        priority: MUST_INTERRUPT
        notes: tank-targeted DoT/stun on cast
    notes: melee-cast hybrid; CC valid

  # Pre–Corla / Beauty branch
  - mob: Crazed Mage
    abilities:
      - name: Fireball
        interruptible: true
        priority: MUST_INTERRUPT
    notes: Polymorph-CC primary target; AoE-burn secondary

  - mob: Conflagration                  # environmental, not killable
    abilities:
      - name: ground_fire_aura
        avoidable: true
        notes: visible patch on floor — stand out

  # Pre–Karsh (forge approach)
  - mob: Quicksilver Tank                 # large mob
    abilities:
      - name: frontal_cleave
        target: frontal_cone
        notes: side/rear positioning required for melee
    notes: tank-and-spank; high HP

  - mob: Twilight Sadist
    abilities:
      - name: Fear_cast
        interruptible: true
        priority: MUST_INTERRUPT
        notes: 3-target fear cast; interrupt on rotation
    notes: humanoid; Polymorph/Hex/Fear-CC valid

  # Pre–Obsidius
  - mob: Twilight Drakonid (final hallway)
    abilities:
      - name: melee_cleave
        target: frontal_cone
    notes: pulls into corner of throne room — line-of-sight if needed
```

### Trash skip notes

- **Beauty's chamber (entire room)**: skippable; LoS / wall-hug viable. Default bot behavior is **skip Beauty unless group leader instructs to clear**.
- A few mob clusters between Rom'ogg and Corla can be wall-hugged on speed-clears.

### CC priorities

- Polymorph / Hex / Fear / Sap valid on Twilight humanoids (Elementalists, Torturers, Mages, Sadists)
- Bonecrusher / Drakonid types are not standard CC targets

---

## Boss 1 — Rom'ogg Bonecrusher

```yaml
boss: rom_ogg_bonecrusher
npc_id: 39665
location: first encounter, ground-floor chamber after the elevator
hp_pct_phase_triggers: [50]
notes: single-phase tank-and-spank with periodic add wave at ~50% HP. Recurring Chains-of-Woe → Skullcracker stack-up cycle every ~30s.

phases:
  - id: 1
    name: regular
    trigger: pull
    end_condition: hp_pct == 50
    description: tank-and-spank with Quake interrupts + Chains-of-Woe / Skullcracker stack cycle

  - id: 2
    name: add_wave
    trigger: hp_pct == 50
    end_condition: all adds dead
    description: 4 Angered Earth elementals spawn; AoE-burn while continuing boss DPS

  - id: 3
    name: execute
    trigger: end of phase 2
    end_condition: hp_pct == 0
    description: resume regular cycle; final burn

mechanics:
  - name: Quake
    spell_id: 75272                      # UNVERIFIED — DBC pass
    cast_by: rom_ogg_bonecrusher
    cast_kind: cast
    target: pbaoe_self
    cast_time_ms: 2000
    cooldown_ms: 10000-15000
    effect: AoE damage; ticks if not interrupted
    interruptible: true
    dispel_type: none
    avoidable: false
    priority: MUST_INTERRUPT
    notes: bot interrupt priority HIGH; rotate kicks across DPS/tank

  - name: Chains of Woe
    spell_id: 75539                      # UNVERIFIED
    cast_by: rom_ogg_bonecrusher
    cast_kind: cast
    target: pbaoe_pull
    cast_time_ms: 1500
    effect: pulls all players to fixed point near boss
    interruptible: false                 # too short to reliably kick; mechanic-intended
    dispel_type: none
    avoidable: false
    notes: precedes Skullcracker; stack-up signal

  - name: Skullcracker
    spell_id: 75543                      # UNVERIFIED
    cast_by: rom_ogg_bonecrusher
    cast_kind: channel
    target: pbaoe_self
    duration_ms: 6000
    effect: AoE damage SHARED among all players in melee range
    interruptible: false
    dispel_type: none
    avoidable: partial
    avoidance: STACK at melee range — damage divides among players in stack
    priority: GROUP_STACK
    notes: solo'd Skullcracker is lethal; full-group stack is survivable

  - name: Wounding Strike
    spell_id: UNVERIFIED
    cast_by: rom_ogg_bonecrusher
    cast_kind: melee
    target: tank
    effect: tank-only DoT; healer pressure on tank

  - name: Call Bonecrushers
    spell_id: SKIPPED-AWAITING-INGAME    # 2026-05-10: was 75319 = "Omen Event Credit" per DBC; not bound in TC BRC scripts. Confirm in-game
    cast_by: rom_ogg_bonecrusher
    cast_kind: scripted
    target: ground_random_corners
    effect: summons 4 Angered Earth elementals at ~50% HP
    interruptible: false

# Adds
adds:
  - name: Angered Earth
    count: 4 (normal); additional waves possible on heroic — see survey Disagreement #1
    abilities:
      - name: melee
        target: closest_player
    notes: tank picks up; AoE-burn before reaching group

positioning:
  default: STACK at melee range under boss (Skullcracker damage-share)
  tank_facing: irrelevant (no frontal cone)
  los_or_lookaway: none
  ranged_override: ranged STACKS WITH MELEE — overrides default "ranged spread" behavior

interrupt_priority:
  - Quake (boss)                         # MUST_INTERRUPT every cast on heroic

dispel_priority: []                       # no dispels required

heroic_delta: |
  - Additional add waves possible (see Disagreement #1) — bot uses add-spawn-event trigger, not HP-threshold
  - Quake hits harder; missed interrupt = significant healer pressure
  - Skullcracker damage high enough that missing the stack is likely lethal

bot_role_summary:
  tank: hold threat through Chains of Woe; position centrally so post-pull stack is consistent
  healer: pre-stack-heal before Skullcracker (predictable ~6s warning); AoE heal aligned with channel tick
  melee: in range already; rotate Quake interrupts
  ranged: STACK with melee for Skullcracker — explicit override of default ranged-spread
```

---

## Boss 2 — Corla, Herald of Twilight

```yaml
boss: corla_herald_of_twilight
npc_id: 39679
location: second encounter, circular chamber with 3 raised drakonid platforms
hp_pct_phase_triggers: []
notes: |
  Two-phase: beam phase (drakonids alive) → Corla burn (drakonids dead).
  Some guides call this single-phase; mechanically the bot must kill drakonids
  before Corla in either model — see survey Disagreement #2.

phases:
  - id: 1
    name: beam_phase
    trigger: pull
    end_condition: all 3 drakonids dead
    description: |
      Three drakonids on raised platforms channel beams at Corla. DPS/healer
      stand in beams to intercept (taking damage, gaining Twilight Evolution
      stacks). Drakonids killed individually while one player intercepts
      that drakonid's beam. Tank holds Corla central.

  - id: 2
    name: corla_burn
    trigger: end of phase 1
    end_condition: corla.hp_pct == 0
    description: tank-and-spank Corla; no significant mechanics

mechanics:
  - name: Evolution (drakonid → Corla beam)
    spell_id: 75610                      # UNVERIFIED
    cast_by: twilight_drakonid (3 instances)
    cast_kind: channel
    target: corla (line-of-sight from drakonid to boss)
    duration_ms: continuous_until_drakonid_dies
    effect: empowers Corla while uninterrupted; intercept by body-blocking beam
    interruptible: false                 # not kick-style; intercepted by positioning
    dispel_type: none

  - name: Twilight Evolution (debuff on intercepter)
    spell_id: 75732                      # verified 2026-05-10 via DBC (was 75571 = "Wounding Strike")
    cast_by: evolution_beam_passive
    cast_kind: stacking_aura
    target: ally_in_beam
    stack_max: 5
    effect: at 5 stacks, player TRANSFORMS into hostile drakonid (effectively dies)
    decay: stacks fall off ~10s after exiting beam
    interruptible: false
    dispel_type: none                    # not dispellable; managed by stepping out of beam
    avoidable: yes — step out at stack 3-4 (heroic) or 4-5 (normal)
    priority: SELF_MANAGED_POSITIONING

  - name: Dark Command (Corla auto-attack)
    spell_id: 75823                      # UNVERIFIED
    cast_by: corla
    cast_kind: melee
    target: tank
    effect: standard tank melee with shadow flavor

  - name: drakonid_melee
    cast_by: twilight_drakonid
    cast_kind: melee
    target: closest_player
    notes: drakonids melee whoever is intercepting their beam

# Adds (drakonids — pre-existing, not summoned)
adds:
  - name: Twilight Drakonid (Corla)
    count: 3
    locked_to: raised_platforms (stationary while channeling)
    abilities:
      - name: Evolution channel (see mechanic above)
      - name: drakonid_melee
    kill_priority: ABSOLUTE (must die before Corla burn-down)

positioning:
  default: |
    Tank pins Corla in CENTER. 3 DPS each assigned a drakonid platform.
    Each DPS stands BETWEEN their assigned drakonid and Corla, intercepting
    the beam.
  tank_facing: away from group (standard tank)
  los_or_lookaway: none
  beam_assignment: explicit per-bot state — "I am responsible for beam X"
  swap_threshold:
    normal: stack 4 (out for 10s, back in)
    heroic: stack 3 (mandatory two-player rotation per beam)

interrupt_priority: []                    # no kick-required casts

dispel_priority: []                       # no dispels

heroic_delta: |
  - Two-player rotation per beam mandatory
  - Beam-intercept damage higher; healer pressure on whoever is intercepting
  - Drakonid HP higher; if not killed before mechanics compound, healer drains OOM

bot_role_summary:
  tank: pin Corla central; melee one drakonid in range if soloing the leftover beam
  healer: rotate cooldowns onto current beam-intercepter (predictable damage source)
  melee: take closest beam; intercept while meleeing assigned drakonid
  ranged: take assigned beam; cast-while-moving classes preferred (Hunter, Frost Mage, Shaman with Lava Surge); stop-cast when stepping in/out of beam
```

---

## Boss 3 — Karsh Steelbender

```yaml
boss: karsh_steelbender
npc_id: 39698
location: third encounter, central forge platform with central magma plume
hp_pct_phase_triggers: []
notes: single-phase tank-kite mechanic. Tank repeatedly drags Karsh through central lava plume to manage Quicksilver Armor stacks (damage-reduction debuff/buff swap). DPS stays out of lava; only the tank passes through.

phases:
  - id: 1
    name: regular
    trigger: pull
    end_condition: hp_pct == 0
    description: continuous tank-kite-through-plume; DPS from outside; healer at range

mechanics:
  - name: Quicksilver Armor
    spell_id: 75842                      # UNVERIFIED
    cast_by: karsh_steelbender
    cast_kind: passive
    target: self
    effect: |
      Damage-reduction stacking buff on Karsh (some sources call it a debuff
      swap — see survey Disagreement #4). Lava plume strips/replaces stacks
      so DPS damage normalizes. Mechanical outcome: tank must drag boss
      through plume periodically.
    interruptible: false
    dispel_type: none
    avoidable: false
    priority: TANK_KITE_TRIGGER
    cadence_normal: ~10-12s              # tank passes through every cycle
    cadence_heroic: ~8-10s

  - name: Cinderbreath
    spell_id: SKIPPED-AWAITING-INGAME    # 2026-05-10: was 75763 = "Umbral Mending" per DBC; "Cinderbreath" not present in Spell.dbc. Karsh's known spells are SPELL_HEAT_WAVE 75851 / SPELL_BOUND_FLAMES 93499 (TC source). Confirm in-game with .lookup spell
    cast_by: karsh_steelbender
    cast_kind: cast
    target: frontal_cone
    cast_time_ms: 2000
    cooldown_ms: 12000
    effect: fire AoE breath
    interruptible: false                 # most reports say no; one source disagrees — see survey
    dispel_type: none
    avoidable: true
    avoidance: stand BEHIND or to the SIDE of Karsh

  - name: Searing Lava (central plume)
    spell_id: SKIPPED-AWAITING-INGAME    # 2026-05-10: was 75852 = "[DND] Summon" per DBC. Karsh fire kit in TC: SPELL_HEAT_WAVE 75851, SPELL_FORCECAST_LAVA_SPOUT 88538, SPELL_BOUND_FLAMES 93499. Confirm in-game
    cast_by: environmental
    cast_kind: passive
    target: ground_center_plume
    effect: fire DoT applied while standing in plume (~3-5s exposure required to apply Quicksilver effect to boss)
    interruptible: false
    dispel_type: none
    avoidable: true                      # everyone EXCEPT tank avoids
    tank_exception: TANK MUST STAND IN BRIEFLY to apply Quicksilver

  - name: melee
    cast_by: karsh_steelbender
    cast_kind: melee
    target: tank
    effect: standard tank melee

positioning:
  default: |
    Tank: drag boss through center plume periodically; pull boss BACK OUT
    after each pass so DPS/healer don't take collateral fire.
    Melee: behind boss, OUTSIDE the plume.
    Ranged: at range, OUT of cone arc, OFF the plume.
    Healer: at range, free-positioning, cone-aware.
  tank_facing: away from group (standard); face into plume during kite-pass
  los_or_lookaway: none
  fight_specific_override: |
    Tank "stand in environmental hazard" override for the central plume.
    All other roles use default "fire on floor = avoid" rule.

interrupt_priority: []                    # no kick-required

dispel_priority: []                       # no dispels

heroic_delta: |
  - Quicksilver stacks faster; tank kite cadence ~8-10s
  - Cinderbreath hits harder
  - Healer mana check: tank takes ~6-8 lava-passes; canonical OOM-test for
    new Cata healers

bot_role_summary:
  tank: drag boss through center plume on cadence (~10s normal / ~8s heroic); pull back out after pass
  healer: predictable tank fire-damage spikes on each plume-pass; pre-HoT before pass
  melee: behind boss outside plume; do NOT chase tank through plume on kite
  ranged: maximize range; standard ranged behavior correct
```

---

## Boss 4 (optional) — Beauty

```yaml
boss: beauty
npc_id: 39700
optional: true
location: side branch off main spine; chamber with 3 named pup adds + Beauty
notes: |
  OPTIONAL. Default bot behavior: SKIP unless group leader instructs to clear.
  Pups (Lucky, Buster, Spot) precede Beauty in the same chamber; pull pups
  individually first, then Beauty.

phases:
  - id: 0
    name: pup_lucky
    trigger: pull pup individually
    end_condition: pup dies
    description: tank-and-spank one pup at a time

  - id: 0b
    name: pup_buster
    trigger: pull pup individually

  - id: 0c
    name: pup_spot
    trigger: pull pup individually

  - id: 1
    name: beauty
    trigger: all pups dead AND beauty pulled
    end_condition: hp_pct == 0
    description: tank-and-spank Beauty with Berserker Charge + Terrifying Roar fear

mechanics_beauty:
  - name: Berserker Charge
    spell_id: 76030                      # UNVERIFIED
    cast_by: beauty
    cast_kind: instant
    target: ally_random
    effect: charge + STUN on impact
    interruptible: false
    dispel_type: none                    # stun broken by trinket / Iceblock / Bubble / Ice Block / Lichborne / Bestial Wrath etc.
    avoidable: partial
    avoidance: spread to prevent multi-hit charge

  - name: Terrifying Roar
    spell_id: 76028                      # UNVERIFIED
    cast_by: beauty
    cast_kind: cast
    target: pbaoe_self
    cast_time_ms: 2000
    cooldown_ms: 30000
    effect: AoE FEAR on group
    interruptible: UNVERIFIED              # guides disagree — see survey Disagreement #6
    counter:
      - Tremor Totem (Shaman)
      - Fear Ward (Priest, Dwarf)
      - Berserker Rage (Warrior)
      - Will of the Forsaken (Undead)
      - PvP trinket
    dispel_type: none                    # fear is not dispellable in classic sense
    avoidable: false

  - name: Flamebreak
    spell_id: 76032                      # UNVERIFIED
    cast_by: beauty
    cast_kind: instant
    target: pbaoe_self
    effect: fire AoE around boss
    interruptible: false
    avoidable: true
    avoidance: stand at range

mechanics_pups:
  - name: Magma Spit (Lucky? UNVERIFIED)
    spell_id: UNVERIFIED
    cast_by: pup
    cast_kind: cast
    target: ally_random
    effect: ranged fire bolt
    interruptible: true                   # likely; UNVERIFIED

  - name: Little Big Flame Breath (Buster? UNVERIFIED)
    spell_id: UNVERIFIED
    cast_by: pup
    cast_kind: cast
    target: frontal_cone

  - name: Magma Trail (Spot? UNVERIFIED)
    spell_id: UNVERIFIED
    cast_by: pup
    cast_kind: passive
    target: behind_pup_movement
    effect: fire trail; kite-pup-around-room

positioning:
  default: tank pulls Beauty to side of chamber so charges don't path through group; melee in melee; ranged spread
  tank_facing: away from group (standard)
  los_or_lookaway: none

interrupt_priority:
  - Terrifying Roar (Beauty)              # ATTEMPT — UNVERIFIED interruptibility
  - Magma Spit (pups)                     # likely interruptible

dispel_priority: []                       # no standard dispels; fear-counter abilities are not dispels

heroic_delta: |
  - Pups hit harder; Berserker Charge stun longer; Terrifying Roar fear harder to break without off-class fear-counter
  - Loot: heroic Beauty drops a notable trinket (Tia's Grace per some sources — UNVERIFIED, not bot-critical)

bot_role_summary:
  tank: tank-and-spank; pulls Beauty to side of chamber
  healer: ready for sudden movement (group fears + charge); pre-emptive HoTs
  melee: stay behind boss; brace for charge stuns hitting random players
  ranged: spread for charge; cast-while-moving when feared (fear breaks casts)

skip_default: true                        # default bot behavior: SKIP this boss
clear_condition: group_leader_explicit_request OR farm_loot_mode
```

---

## Boss 5 — Ascendant Lord Obsidius

```yaml
boss: ascendant_lord_obsidius
npc_id: 39705
location: final encounter, throne-room style chamber
hp_pct_phase_triggers: []
notes: |
  Single-phase but with a recurring tank-swap event. Three Shadow of Obsidius
  adds patrol pre-pull. The "swap" is not MT/OT rotation in a 5-man — it's
  the single tank picking up the new clone-as-boss when Obsidius "becomes" a
  clone.

phases:
  - id: 1
    name: regular
    trigger: pull
    end_condition: hp_pct == 0
    description: |
      Tank holds Obsidius; periodically Obsidius applies Twilight Corruption
      (debuff on tank) AND Crepuscular Veil (debuff on random non-tank); a
      Shadow of Obsidius clone "takes over" the active boss role; tank
      re-acquires the new active boss/clone.

mechanics:
  - name: Twilight Corruption (debuff on tank)
    spell_id: 76188                      # UNVERIFIED
    cast_by: obsidius
    cast_kind: melee_application
    target: tank
    effect: stacking shadow-flavor melee debuff on tank
    interruptible: false
    dispel_type: none                    # do not dispel
    swap_signal: SECONDARY                # clone-rotation event is primary swap signal
    stack_count_for_swap_normal: UNVERIFIED   # 3 vs 5 vs irrelevant — see survey Disagreement #7

  - name: Crepuscular Veil (debuff on player)
    spell_id: 76189                      # verified 2026-05-10 via DBC (was 75476 = "Dusk Shroud"). DBC also has 76190 — likely heroic variant; both share the name
    cast_by: obsidius
    cast_kind: scripted
    target: ally_random_non_tank
    effect: shadow-flavor debuff; visually shrouds player
    interruptible: false
    dispel_type: magic
    DISPEL_BLACKLIST: true                # CRITICAL — bot must NOT auto-dispel this debuff
    notes: |
      Standard strat is "do not dispel" — debuff is part of the swap mechanic.
      Heroic dispel-window controversy unresolved (see survey Disagreement #9);
      bot defaults to NEVER dispel.

  - name: Clone activation (Shadow of Obsidius rotation)
    spell_id: UNVERIFIED                 # scripted event
    cast_by: obsidius
    cast_kind: scripted
    effect: |
      One of the 3 Shadow of Obsidius adds becomes the "active" boss; the
      previous Obsidius unit transitions to clone status. Tank must re-acquire
      threat on the new active unit.
    swap_signal: PRIMARY                 # bot tank-swap trigger
    cadence: every ~30-45s (UNVERIFIED)

  - name: Shadow Prison / shadow puddles
    spell_id: 76686                      # verified 2026-05-10 via DBC (was 75763 = "Umbral Mending"). DBC also has 76687 — likely heroic variant; pick lower for normal
    cast_by: shadow_of_obsidius
    cast_kind: passive
    target: ground_around_clone
    effect: |
      Shadow zones near each clone. UNVERIFIED whether helpful (defensive,
      stand-in) or harmful (avoid) — see survey Disagreement #8. Bot DEFAULT
      is treat as harmful = avoid.

  - name: melee
    cast_by: obsidius
    cast_kind: melee
    target: tank

# Adds (pre-existing, not summoned)
adds:
  - name: Shadow of Obsidius
    count: 3
    notes: |
      Pre-existing in room before pull; do NOT need to be killed in standard
      strat. They function as mechanic-triggers (clone-rotation) rather than
      kill-priority targets.
    abilities:
      - name: shadow_aura (puddles)
      - name: melee (when activated as primary)

positioning:
  default: tank in CENTER; clones positioned at back of room; ranged/healer at max range, spread
  tank_facing: away from group (standard)
  los_or_lookaway: none
  avoid: corners where Shadow puddles tend to congregate

interrupt_priority: []                    # no confirmed kick-required casts

dispel_priority: []                       # NONE — Crepuscular Veil is dispel-blacklisted

heroic_delta: |
  - Faster clone-pull cycle
  - More damage from clones
  - Crepuscular Veil applies more frequently; group must position so affected player isn't in cleave range
  - Heroic dispel-window controversy unresolved (Disagreement #9); bot defaults to NEVER dispel regardless of difficulty

bot_role_summary:
  tank: TARGET-SWAP AWARENESS — re-acquire the new active boss/clone when Obsidius becomes a clone
  healer: DO NOT auto-dispel Magic during this fight; add instance-level dispel-blacklist for Crepuscular Veil spell ID
  melee: if you become Crepuscular Veil target, MOVE OUT of group to avoid cleave
  ranged: max range, spread; recover quickly when target changes
```

---

## Instance-wide notes

```yaml
instance_wide:
  curse_dispel_required: false
  disease_dispel_required: false
  poison_dispel_required: false
  magic_dispel_required: false              # only on Obsidius and BLACKLISTED there
  dispel_blacklist:
    - { spell_id: 75476, name: Crepuscular Veil, reason: Obsidius swap mechanic }  # ID WRONG — 75476 is "Dusk Shroud" (AoE aura); real Crepuscular Veil ID unknown. DO NOT ship until corrected
  group_buff_anomaly: none
  required_consumables: standard (flask + food + pots per spec)
```

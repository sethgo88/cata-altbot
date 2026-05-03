# Zul'Gurub — Encounter Facts

**Role-neutral mechanic data** for the bot's encounter handling. Per-role response logic lives in sibling files (`healer.md`, `ranged-dps.md`, `melee-dps.md`, `tank.md`). When a role doc references a mechanic, it cites the spell ID defined here.

Spell IDs verified at `wowhead.com/cata/spell=<id>` where consensus could be reached; many are flagged **UNVERIFIED** in this doc and aggregated in `docs/research/dbc-verification-checklist.md`. Strategy reconciled across 5+ sources — see `docs/research/zul-gurub-guide-survey.md` for the audit trail.

---

## Overview

```yaml
dungeon: zul_gurub
zone: zul_gurub
continent: eastern_kingdoms_stranglethorn_vale
difficulty_modes: [heroic_only]            # NO normal mode — instance is heroic-exclusive
level_range_normal: null
level_range_heroic: 85
min_ilvl_required: 346
boss_count: 5
optional_bosses: [cache_of_madness]        # 1 of 4 random spawns; requires Archaeology 225+
bosses_in_order:
  - high_priest_venoxis
  - bloodlord_mandokir
  - high_priestess_kilnara
  - zanzil
  - jin_do_the_godbreaker
patch_introduced: 4.1
group_skip_rule: |
  Post-4.3.0: party only needs to clear Venoxis + 2 of the 3 remaining mid-bosses
  (Mandokir / Kilnara / Zanzil) before engaging Jin'do. Bot default = full clear.

notes: |
  Outdoor zone with multiple gates; mostly ring-shaped path. Originally the
  vanilla 20-man Hakkar raid; revamped as a 5-man heroic in 4.1. Heroic-only
  (NO normal mode). Long for a 5-man — full clear ~45-60 min on launch-tier
  gear.

  No instance-wide buff/debuff. No instance-wide curse/disease/poison dispel
  requirement. Each boss has 1-2 unique mechanics; difficulty cluster is
  Venoxis (movement/interrupt) + Jin'do (parallel-zone teleport).
```

---

## Instance-wide notes

```yaml
instance_wide:
  curse_dispel_required: false
  disease_dispel_required: false
  poison_dispel_required: false
  magic_dispel_required: optional            # only Kilnara Lash of Anguish; uncertain (see survey Disagreement #4)
  dispel_blacklist: []                        # no fight-level blacklists
  group_buff_anomaly: none
  required_consumables: standard (flask + food + pots per spec)
  heroic_only: true                           # CRITICAL — no normal-mode tuning exists
  bot_implementation_gaps:
    - mandokir_decapitate_immunity_trigger    # scripted-event, not cast-bar
    - zanzil_cauldron_gameobject_interaction  # bot can't click env objects natively
    - jindo_phase_2_spirit_realm_teleport     # parallel-zone phase change
```

---

## Trash — notable mechanics

Only the trash entries with mechanics worth automating. Standard packs (basic Gurubashi grunts, raptor melee adds) are pure threat-and-DPS and not enumerated.

```yaml
trash:
  # Pre–Venoxis (entry → snake temple)
  - mob: Voodoo Acolyte
    abilities:
      - name: Shadow_Bolt
        interruptible: true
        priority: MUST_INTERRUPT
        notes: caster trash; CC valid (Polymorph, Hex, Sap)
    notes: opener-CC primary target

  - mob: Razzashi Cobra
    abilities:
      - name: melee_poison
        target: closest_player
    notes: melee mob; tank stacks; AoE-burn

  - mob: Gurubashi Headhunter / Berserker
    abilities:
      - name: melee
        target: tank
    notes: standard pulls; tank-and-spank

  # Pre–Mandokir (raptor packs)
  - mob: Razzashi Raptor
    abilities:
      - name: melee_cleave
        target: frontal_cone
    notes: tank face-away; pack-pulls; AoE-burn

  - mob: Razzashi Skitterer (rare)
    abilities:
      - name: poison_cast
        interruptible: true
        priority: MUST_INTERRUPT

  # Pre–Kilnara (panther temple approach)
  - mob: Bethekk Acolyte
    abilities:
      - name: Shadow_cast
        interruptible: true
        priority: MUST_INTERRUPT
    notes: humanoid caster; CC valid

  # NOTE: Pride of Bethekk sleeping panthers are inside Kilnara's room
  # — covered in Boss 3 mechanics, NOT trash

  # Pre–Zanzil (voodoo grove)
  - mob: Zanzili Spellcasters (first pack)
    abilities:
      - name: Voodoo_cast
        interruptible: true
        priority: MUST_INTERRUPT
    notes: |
      Tank-strat: drink Burning Blood cauldron buff for AoE clear
      (cauldron is at the entry; same mechanic as Zanzil boss fight)

  - mob: Rabid Gurubashi (second pack)
    abilities:
      - name: melee_fixate
        notes: NO threat table; fixates random player
    notes: |
      Tank-strat: drink Frostburn cauldron buff to stun-burst on strike;
      pre-fight cauldron usage teaches the mechanic for boss

  # Pre–Jin'do
  - mob: Gurubashi Spirit Warriors (2 on stairs)
    abilities:
      - name: melee
        target: closest_player
    notes: pre-fight kill before pulling Jin'do (some strats)

  - mob: Hakkari Bloodkeeper (rare patrols)
    abilities:
      - name: shadow_cast
        interruptible: true
```

### Trash skip notes

- **Cache of Madness chamber**: skippable; bot defaults to skip (requires Archaeology 225+ to even unlock; bot does not level Archaeology).
- A few packs in Mandokir's arena and Zanzil's grove can be wall-hugged on speed-clear.
- **Pride of Bethekk panthers** inside Kilnara's room: NOT trash (boss mechanic — see Boss 3).

### CC priorities

- Polymorph / Hex / Sap / Fear valid on troll humanoid casters (Voodoo Acolytes, Bethekk Acolytes, Hakkari Bloodkeeper, Zanzili Spellcasters)
- Raptors / cobras / berserkers / panthers are NOT standard CC targets
- Banish NOT applicable (no demons in instance)

---

## Boss 1 — High Priest Venoxis

```yaml
boss: high_priest_venoxis
npc_id: 52155
location: first encounter, snake-temple platform on raised altar
hp_pct_phase_triggers: [75, 25]
notes: |
  Three phases. Most movement-intensive fight in the instance. The skill-check
  encounter for the dungeon — interrupt rotation + spread mechanic + multi-
  phase positioning.

phases:
  - id: 1
    name: venomous_labyrinth
    trigger: pull
    end_condition: hp_pct == 75
    description: |
      Troll-form Venoxis on platform. Poison maze on ground (Venomous Effusion);
      interrupt-required Whispers of Hethiss; Toxic Link spread mechanic
      between two random players.

  - id: 2
    name: mortal_coil_unwinds
    trigger: hp_pct == 75
    end_condition: hp_pct == 25
    description: |
      Transforms to snake form via Blessing of the Snake God. Loses dodge/parry
      → +50% physical damage taken from melee. Drops Pool of Acrid Tears at
      furthest player; uses Breath of Hethiss cone.

  - id: 3
    name: bloodvenom
    trigger: hp_pct == 25
    end_condition: hp_pct == 0
    description: |
      Returns to altar. Summons one Bloodvenom tendril per alive player; kite.
      Ends with Venom Withdrawal stun on Venoxis: +100% damage taken — burst
      window with stacked CDs.

mechanics:
  - name: Whispers of Hethiss
    spell_id: 96466
    cast_by: high_priest_venoxis
    cast_kind: channel
    target: ally_random_non_tank
    cast_time_ms: 2000                       # windup before channel begins
    channel_duration_ms: 8000
    effect: |
      1850-2150 nature dmg every 0.3s for 8s on target. Heavy DoT-channel.
    interruptible: true
    dispel_type: none
    avoidable: false
    priority: MUST_INTERRUPT
    notes: |
      PRIMARY KICK TARGET. Bot interrupt priority CRITICAL. Rotate kicks across
      DPS/tank. Missed kick = ~50k+ nature damage on the targeted player.

  - name: Toxic Link
    spell_id: 96477
    cast_by: high_priest_venoxis
    cast_kind: scripted_aura
    target: 2_players_paired
    duration_ms: 10000
    effect: |
      Two random players linked. Damage scales with proximity — increases as
      they get closer. Breaks at >25y separation OR explodes for ~10000 dmg
      in 8y radius if broken near other players.
    interruptible: false
    dispel_type: none                        # not dispellable; spread to break
    avoidable: yes — spread linked players to >25y
    priority: GROUP_SPREAD

  - name: Word of Hethiss
    spell_id: UNVERIFIED
    cast_by: high_priest_venoxis
    cast_kind: cast
    target: pbaoe_self
    cast_time_ms: 3000
    effect: 46-53k nature dmg in 10y + knockback (pull-cast)
    interruptible: false
    avoidable: true
    avoidance: stand >10y at pull

  - name: Venomous Effusion (poison maze)
    spell_id: UNVERIFIED
    cast_by: high_priest_venoxis
    cast_kind: passive_ground
    target: ground_pattern
    effect: 9425-10575 nature dmg per tick if standing in patch
    interruptible: false
    avoidable: true
    avoidance: navigate around patches

  - name: Pool of Acrid Tears
    spell_id: UNVERIFIED
    cast_by: high_priest_venoxis
    phase: 2
    cast_kind: scripted_ground
    target: ground_at_furthest_player
    effect: |
      Spawns at furthest-from-boss player; grows over time.
      4712-5287 dmg/0.5s if standing in.
    avoidable: true
    avoidance: position pool at platform edge; do not stand at extreme range

  - name: Breath of Hethiss
    spell_id: UNVERIFIED
    cast_by: high_priest_venoxis
    phase: 2
    cast_kind: scripted_cone
    target: frontal_cone_15y
    duration_ms: 3000
    effect: 15000 nature dmg every 0.5s for 3s in cone
    avoidable: true
    avoidance: dodge cone arc (sides/rear)

  - name: Blessing of the Snake God (Phase 2 self-buff)
    spell_id: UNVERIFIED
    cast_by: high_priest_venoxis
    phase: 2
    cast_kind: passive_aura
    target: self
    effect: +50% physical damage; loses dodge/parry → +50% incoming melee dmg
    notes: melee burst window (boss takes more damage but hits harder too)

  - name: Bloodvenom (tendrils)
    spell_id: UNVERIFIED
    cast_by: high_priest_venoxis
    phase: 3
    cast_kind: scripted_summon
    target: ground_random
    effect: summons one tendril per alive player; tendrils chase fixated targets
    interruptible: false
    avoidable: partial
    avoidance: kite tendrils around platform; AoE-burn

  - name: Venom Withdrawal
    spell_id: UNVERIFIED
    cast_by: high_priest_venoxis
    phase: 3
    cast_kind: self_debuff
    target: self
    effect: stun on Venoxis; +100% damage taken
    notes: BURST WINDOW — stack offensive CDs here

  - name: Poison Cloud (Phase 3 environmental)
    spell_id: UNVERIFIED
    cast_kind: passive_ground
    target: altar_stairs
    effect: 9250-10750 dmg per 2s if standing on altar
    avoidable: true
    avoidance: do not stand on altar/stairs

positioning:
  default: |
    Phase 1: spread to >25y for Toxic Link safety. Tank centers boss.
    Phase 2: tank repositions boss for cone arc; ranged moderate spread (acid
      pool spawns at furthest player).
    Phase 3: kite tendrils; stack burst CDs.
  tank_facing: away from group (standard); rotate during Phase 2 cone
  ranged_spread: yes (Phase 1 & 2); cluster for Phase 3 burst
  los_or_lookaway: none

interrupt_priority:
  - Whispers of Hethiss (96466)             # CRITICAL — every cast

dispel_priority: []                          # no dispels required

heroic_delta: |
  Heroic-only fight. All numbers are heroic.

bot_role_summary:
  tank: pull boss to clean ground; 90° rotate during Phase 2 cone telegraph; lead tendrils to platform edge in Phase 3
  healer: heavy reactive heal on Whispers target; pre-shield Toxic Link partners; SAVE major CD for Phase 3
  melee: stay behind boss for Phase 2 cone; spread out of melee if Toxic-Linked with another melee
  ranged: maintain spread but NOT max range in Phase 2 (acid pool placement)
```

---

## Boss 2 — Bloodlord Mandokir

```yaml
boss: bloodlord_mandokir
npc_id: 52151
location: second encounter, raised arena with 8 Chained Spirits
hp_pct_phase_triggers: [20]
notes: |
  Single phase + execute. Mandokir is the only boss in Cata 5-man content with
  a true ONE-SHOT mechanic (Decapitate). Player immunity is the only counter.

  Recurring add cycle: Ohgan (raptor pet) spawns/respawns and must be killed
  on every cycle to preserve Chained Spirits.

phases:
  - id: 1
    name: regular
    trigger: pull
    end_condition: hp_pct == 20
    description: |
      Decapitate cycle (~30-45s). Ohgan present and must be killed on each
      Reanimate cycle. Devastating Slam dodging; Bloodletting heal-suppress.

  - id: 2
    name: frenzy
    trigger: hp_pct == 20
    end_condition: hp_pct == 0
    description: |
      Frenzy aura: +100% attack speed; Decapitate CD shortened. Burn phase
      with major CDs.

mechanics:
  - name: Decapitate
    spell_id: 96684
    cast_by: bloodlord_mandokir
    cast_kind: instant
    target: ally_random_non_tank
    cooldown_ms: 30000-45000                 # ~30-45s; hotfix 4.3 +5s adjustment
    effect: |
      INSTANT KILL on target. ~14.6-15.4M physical damage.
    interruptible: false                     # instant
    dispel_type: none
    avoidable: yes — IMMUNITY ABILITY ONLY
    counter:
      - Ice Block (Mage)
      - Divine Shield / Bubble (Paladin)
      - Hand of Protection (Paladin) on ally
      - Cloak of Shadows (Rogue)
      - Lichborne (Frost DK; physical immune component? UNVERIFIED)
      - Anti-Magic Shell DOES NOT block physical
      - Feign Death (Hunter) on cast detection
      - PvP trinket DOES NOT prevent (it's not a CC)
    priority: SCRIPTED_IMMUNITY_TRIGGER
    notes: |
      CRITICAL. Bot detects scripted-event telegraph (leap animation +
      voice line). Cast-bar detection insufficient (instant cast).
      bot_implementation FLAG.

  - name: Level Up
    spell_id: 96662
    cast_by: bloodlord_mandokir
    cast_kind: passive_aura
    target: self
    effect: +20% damage per stack; stacks each player kill (max 99)
    notes: implication = minimize player deaths beyond Decapitate

  - name: Devastating Slam
    spell_id: 96740
    cast_by: bloodlord_mandokir
    cast_kind: cast
    target: ground_directional_furrow
    cast_time_ms: 2000
    cooldown_ms: 10000-15000
    effect: 45° cone furrow; 190-200k physical dmg
    interruptible: false                     # UNVERIFIED — likely no
    avoidable: true
    avoidance: dodge laterally on cast

  - name: Bloodletting
    spell_id: 96776
    cast_by: bloodlord_mandokir
    cast_kind: scripted_dot
    target: ally_random
    duration_ms: 10000                       # ~10s; 5 ticks at 2s
    effect: |
      Drains 50% current HP every 2s for 10s. Boss heals for 50% of damage
      dealt (min 7500). HEALER ANTI-PATTERN: spam-healing during this is
      wasteful — drains target faster + heals boss.
    interruptible: false
    dispel_type: magic                       # UNVERIFIED dispellable; standard strat = DO NOT dispel
    DISPEL_DEFAULT: do_not_dispel
    avoidable: false
    priority: HEAL_SUPPRESS
    notes: |
      Bot healer logic: heal target ONLY if HP drops below 20% (life-saving);
      otherwise let Bloodletting run, then big heal post-expiry.

  - name: Frenzy
    spell_id: 96800
    cast_by: bloodlord_mandokir
    cast_kind: passive_aura
    target: self
    trigger: hp_pct <= 20
    effect: +100% attack speed; Decapitate CD shortened
    notes: BURN PHASE signal

  - name: Reanimate Ohgan
    spell_id: 96724
    cast_by: bloodlord_mandokir
    cast_kind: scripted
    target: ohgan_corpse
    effect: revives Ohgan at full HP after death
    notes: repeats throughout fight; group must kill Ohgan again on each respawn

  - name: Spirit's Vengeance (revive buff)
    spell_id: UNVERIFIED
    cast_by: chained_spirit
    cast_kind: aura
    target: revived_player
    effect: |
      +25% damage / +25% healing / -10% damage taken per stack.
      Stacks up to 10x on repeated revives.
    notes: revived player is BUFFED, not weakened — keep playing through

  - name: melee
    cast_by: bloodlord_mandokir
    cast_kind: melee
    target: tank

# Adds
adds:
  - name: Ohgan
    npc_id: 52157
    spawn_trigger: "first Decapitate (some sources at pull)"
    abilities:
      - name: melee
      - name: kills_chained_spirits
        notes: Ohgan attacks Chained Spirits; consumes them to prevent revives
    kill_priority: ABSOLUTE — drop boss DPS, kill Ohgan, return to boss
    respawn: Reanimate Ohgan (96724) — full-HP revive; repeat cycle

  - name: Chained Spirit
    npc_id: 52156
    count: 8
    notes: |
      Pre-existing in arena. PASSIVE FRIENDLY NPCs — not directly killable
      by players. Each one consumed when reviving a Decapitated player.
      Ohgan can damage/destroy them; finite resource (8 total).

positioning:
  default: |
    Tank holds Mandokir near center of arena. Group spreads moderately so
    Decapitate target randomization doesn't cluster damage. Melee in melee.
  tank_facing: away from group (standard)
  los_or_lookaway: none
  spread: yes — Decapitate target is random, no benefit to clustering

interrupt_priority: []                       # Devastating Slam non-interruptible

dispel_priority: []                          # Bloodletting NOT dispelled per standard strat

heroic_delta: |
  Heroic-only fight.

bot_role_summary:
  tank: hold threat on Mandokir; defensive CDs at Frenzy phase (20% HP)
  healer: |
    PRE-SHIELD Decapitate target if predictable; SUPPRESS heals during
    Bloodletting (heal only if HP < 20% emergency); reactive heal post-
    expiry.
  melee: |
    OHGAN KILL SWAP — drop Mandokir DPS on raptor spawn / Reanimate event;
    full focus on Ohgan; return to boss when raptor dies.
  ranged: same Ohgan-swap behavior; classes with rapid target swap (Frost
    Mage, Hunter) excel here.
```

---

## Boss 3 — High Priestess Kilnara

```yaml
boss: high_priestess_kilnara
npc_id: 52059
location: third encounter, panther temple chamber
hp_pct_phase_triggers: [50]
notes: |
  Two-phase boss + 16 sleeping panther adds in the room. Panther awakening
  is the dominant pre-engagement decision: clear panthers BEFORE Kilnara
  reaches 50% or Phase 2 becomes un-healable.

phases:
  - id: 0
    name: panther_clear
    trigger: pre-pull
    end_condition: 0-16 panthers cleared (depends on strat)
    description: |
      Pull panthers in 2-4 clusters; AoE burn each cluster. Some strats clear
      ALL 16 before Kilnara pull; others clear during Phase 1 hold-pattern.
      bot DEFAULT: pull and clear before engaging Kilnara.

  - id: 1
    name: troll_form
    trigger: kilnara pulled
    end_condition: hp_pct == 50
    description: |
      Standard caster boss. Tears of Blood + Shadow Bolt interrupts; Wave of
      Agony cone-dodge; Lash of Anguish DoT.

  - id: 2
    name: panther_form
    trigger: hp_pct == 50
    end_condition: hp_pct == 0
    description: |
      Transforms to Avatar of Bethekk. Haste; leap-bleed attacks. Any
      remaining sleeping panthers wake here — un-healable if many.

mechanics:
  - name: Tears of Blood
    spell_id: 96435
    cast_by: high_priestess_kilnara
    cast_kind: channel
    target: pbaoe_self
    effect: 8500-11500 shadow dmg every tick within 12y
    interruptible: true
    dispel_type: none
    avoidable: partial — stand >12y
    priority: MUST_INTERRUPT
    notes: PRIMARY KICK TARGET on Phase 1

  - name: Shadow Bolt
    spell_id: 96956
    cast_by: high_priestess_kilnara
    cast_kind: cast
    target: ally_random
    cast_time_ms: 2000
    effect: 11050-14950 shadow dmg
    interruptible: true
    dispel_type: none
    priority: SECONDARY_INTERRUPT
    notes: kick if rotation has slack; lower priority than Tears of Blood

  - name: Wave of Agony
    spell_id: 96457
    cast_by: high_priestess_kilnara
    cast_kind: scripted_cone
    target: frontal_wave
    effect: shadow dmg + knockback
    interruptible: false
    avoidable: true
    avoidance: dodge laterally (sides/rear)

  - name: Wail of Sorrow
    spell_id: 96948
    cast_by: high_priestess_kilnara
    cast_kind: ranged_cast
    target: ally_random
    range: 40y
    effect: 18500-21500 shadow dmg
    interruptible: false                     # UNVERIFIED — most reports say no

  - name: Lash of Anguish
    spell_id: 96958
    cast_by: high_priestess_kilnara
    cast_kind: applied_debuff
    target: ally_random
    duration_ms: 10000
    effect: initial dmg + 18500-21500 / sec for 10s
    interruptible: false
    dispel_type: magic                       # UNVERIFIED — dispel disagreement #4
    DISPEL_DEFAULT: dispel_if_class_has_magic_dispel AND target_hp_pct < 50

  - name: Avatar of Bethekk (Phase 2 transform)
    spell_id: UNVERIFIED
    cast_by: high_priestess_kilnara
    phase: 2
    cast_kind: passive
    target: self
    effect: phase transition; haste + leap-attack form
    trigger: hp_pct == 50

  - name: Gaping Wound
    spell_id: UNVERIFIED
    cast_by: high_priestess_kilnara (Phase 2) AND awakened panthers
    cast_kind: melee_leap
    target: ally_random
    effect: bleed; 9250-10750 dmg every 2s
    notes: physical bleed; not dispellable

# Adds
adds:
  - name: Pride of Bethekk panthers
    npc_id_area: 52345                       # UNVERIFIED area ID
    count: 16
    starting_state: SLEEPING (friendly-tagged pre-aggro)
    wake_trigger: any offensive ability landing on panther OR Kilnara hits 50% with sleepers remaining
    abilities:
      - name: Gaping_Wound (leap bleed)
      - name: melee
    kill_priority_during_clear: cluster-AoE; tank stacks 2-4 at a time
    bot_filter_flag: |
      sleeping panthers must be EXCLUDED from auto-target / auto-cleave to
      prevent accidental pull during cooldown rotations

positioning:
  default: |
    Panther clear: tank stacks 2-4 panthers; group AoE-burns. Major
    cooldowns on the second-to-last cluster.
    Kilnara fight: tank pulls Kilnara to a cleared zone; melee in melee
    behind boss; ranged at >12y to dodge Tears of Blood radius.
  tank_facing: away from group (standard)
  los_or_lookaway: none

interrupt_priority:
  - Tears of Blood (96435)                   # PRIMARY (group AoE)
  - Shadow Bolt (96956)                      # SECONDARY

dispel_priority:
  - Lash of Anguish (96958) on heroic if class has Magic dispel AND target hp_pct < 50

heroic_delta: |
  Heroic-only fight.

bot_role_summary:
  tank: |
    Cluster panthers for AoE; threat on Kilnara + adds during multi-pull.
    Hold Kilnara above 50% if any panthers remain.
  healer: |
    Heavy AoE during Tears of Blood (if missed); Lash of Anguish dispel if
    class has Magic dispel; sustained tank-heal during panther multi-pull.
  melee: AoE rotation during panther clears; cone-dodge during boss phase
  ranged: |
    AoE/cleave on panther clusters; ST focus on Kilnara during Phase 2;
    stand >12y to dodge Tears of Blood radius.
```

---

## Boss 4 — Zanzil

```yaml
boss: zanzil
npc_id: 52053                                # UNVERIFIED — Wowhead 4.3.4 page did not resolve
location: fourth encounter, voodoo grove with 3 cauldrons placed around arena
hp_pct_phase_triggers: []
notes: |
  Single-phase boss with a recurring elixir-color cycle. Defining mechanic:
  3 environmental cauldrons that the group must click in response to Zanzil's
  elixir colors.

  bot_implementation FLAG: cauldrons are clickable GameObjects, not spells.
  Without GameObject-interaction support in the bot framework, this fight
  is mechanically un-completable autonomously.

phases:
  - id: 1
    name: regular
    trigger: pull
    end_condition: hp_pct == 0
    description: |
      Regular DPS on Zanzil interleaved with elixir-color cycles. Each elixir
      summons a different threat; group drinks the matching cauldron buff to
      counter.

mechanics:
  - name: Voodoo Bolt
    spell_id: 96347
    cast_by: zanzil
    cast_kind: cast
    target: ally_random
    cast_time_ms: 2000
    effect: 33250-36750 shadow dmg
    interruptible: true
    dispel_type: none
    priority: MUST_INTERRUPT
    notes: PRIMARY KICK TARGET

  - name: Zanzili Fire
    spell_id: 96914
    cast_by: zanzil
    cast_kind: ground_line_cast
    target: linear_floor_pattern
    effect: 66000 initial + 50000/s for 5s
    interruptible: false
    avoidable: true
    avoidance: tank repositions Zanzil out of line; group steps off line

  - name: Terrible Tonic
    spell_id: 96348
    cast_by: zanzil
    cast_kind: ranged_toss
    target: ally_random
    effect: 30000 nature dmg
    interruptible: false                     # UNVERIFIED

  - name: Resurrection Elixir (Blue)
    spell_id: UNVERIFIED
    cast_by: zanzil
    cast_kind: scripted
    effect: summons Zanzili Berserker
    counter_cauldron: frostburn_formula
    notes: |
      Berserker fixates a player (no threat table). Tank/melee with
      Frostburn buff lands next strike → 15s stun + 65% max HP dmg = burst-kill.

  - name: Resurrection Elixir (Red)
    spell_id: UNVERIFIED
    cast_by: zanzil
    cast_kind: scripted
    effect: summons multiple Zanzili Zombies
    counter_cauldron: burning_blood
    notes: tank clusters zombies; melee/tank with Burning Blood AoE burns them

  - name: Zanzil's Graveyard Gas (Green)
    spell_id: UNVERIFIED
    cast_by: zanzil
    cast_kind: environmental_aura
    target: arena_wide
    effect: 5-10% max HP nature dmg per second across whole party
    counter_cauldron: toxic_torment
    notes: |
      ALL 5 PLAYERS must drink Toxic Torment for -90% nature damage taken;
      otherwise group OOM/dies in seconds.

# Cauldron mechanic — bot_implementation FLAG
cauldrons:
  - name: Frostburn Formula
    object_type: GameObject (clickable interactable)
    object_id: UNVERIFIED
    location: fixed_arena_position
    effect_on_drinker: |
      Next melee/auto-attack stuns target for 15s and deals 65% target max HP dmg.
    use_when: blue_elixir_active (Berserker spawned)
    safety_warning: |
      DOES NOT WORK ON ZANZIL (boss) — but will fire on next attack; ensure
      Berserker is the next-attack target. NEVER drink and then attack
      Zanzil first (waste of buff at minimum; UNVERIFIED if 65%-HP fires
      against boss).

  - name: Burning Blood
    object_type: GameObject
    object_id: UNVERIFIED
    location: fixed_arena_position
    effect_on_drinker: AoE fire damage to nearby enemies (4750-5250 fire dmg pulse)
    use_when: red_elixir_active (Zombies spawned) — drinker is melee anchor
    drinker_role: TANK or MELEE (cluster zombies, AoE-burn)

  - name: Toxic Torment
    object_type: GameObject
    object_id: UNVERIFIED
    location: fixed_arena_position
    effect_on_drinker: -90% nature damage taken for 30s
    use_when: green_elixir_active (Graveyard Gas) — ALL 5 PLAYERS DRINK
    drinker_role: ALL — entire party

# Adds
adds:
  - name: Zanzili Berserker
    spawn_trigger: blue_elixir
    abilities:
      - name: Thunderclap
        target: pbaoe
        effect: 12750-17250 nature dmg
      - name: Knock_Away
        target: tank_or_fixate
    kill_priority: HIGH (off-threat fixate; danger if not killed quickly)
    counter: Frostburn buff stun-burst

  - name: Zanzili Zombies
    spawn_trigger: red_elixir
    count: multiple
    abilities:
      - name: melee
    kill_priority: AOE_CLUSTER

positioning:
  default: |
    Tank holds Zanzil between 2 cauldrons (positions vary by arena).
    Group learns cauldron-color → location mapping pre-fight.
  tank_facing: away from group; reposition out of Zanzili Fire line
  cauldron_layout: 3 fixed positions; bot needs registered object IDs

interrupt_priority:
  - Voodoo Bolt (96347)                      # PRIMARY

dispel_priority: []                          # no dispels

heroic_delta: |
  Heroic-only fight.

bot_role_summary:
  tank: |
    Reposition Zanzil out of Zanzili Fire line. Drink Burning Blood (Red)
    to AoE-clear zombies; drink Toxic Torment (Green); DO NOT drink
    Frostburn near Zanzil (waste at minimum; risk if 65%-max-HP triggers
    on boss).
  healer: drink Toxic Torment on Green; reactive healing during elixir cycles
  melee: |
    Drink Frostburn for Berserker stun-burst; drink Burning Blood for
    Zombie AoE; drink Toxic Torment on Green.
  ranged: drink Toxic Torment on Green; less role in melee-proximity cauldrons

bot_implementation_flag: |
  CAULDRON GAMEOBJECT INTERACTION required. Bot framework must support:
    - GameObject discovery in environment (3 fixed cauldrons in this arena)
    - Context-aware "should drink" predicate per elixir cast event
    - Pathfinding to cauldron and use-action
  Without this, Zanzil is un-completable autonomously.
```

---

## Boss 5 — Jin'do the Godbreaker

```yaml
boss: jin_do_the_godbreaker
npc_id: 52148                                # UNVERIFIED — Wowhead 4.3.4 page did not resolve
location: final encounter, altar at the back of the instance
hp_pct_phase_triggers: [70]
notes: |
  Two-phase boss with a parallel-zone teleport in Phase 2. The Spirit Realm
  phase changes the players' map/phase ID; bot session must handle this
  transition cleanly.

  bot_implementation FLAG: parallel-zone teleport (Phase 2). Without zone-
  state-change handling, Phase 2 is un-completable autonomously.

phases:
  - id: 1
    name: welcome_to_the_great_show
    trigger: pull
    end_condition: hp_pct == 70
    description: |
      Jin'do solo on platform. Deadzone management — magic-damping ground
      zones spawn periodically. Group steps INTO Deadzone during Shadows
      of Hakkar cast (90% magic dmg reduction makes it survivable); steps
      OUT otherwise (90% cast speed reduction is a DPS/heal loss).

  - id: 2
    name: spirit_realm
    trigger: hp_pct == 70
    end_condition: all 3 chains broken
    description: |
      ENTIRE PARTY teleported to parallel zone (Spirit Realm). 3 Hakkar's
      Chains protected by Brittle Barrier. Group breaks barriers via
      Gurubashi Spirit Body Slam landing on chain location (creates Sunder
      Rift debuff). Twisted Spirit adds spawn in increasing waves
      (soft enrage). Shadow Spike dodge mechanic ongoing.

  - id: 3
    name: physical_realm_finish
    trigger: all 3 chains broken
    end_condition: hp_pct == 0
    description: party returns to physical realm; tank-and-spank Jin'do to 0

mechanics:
  - name: Deadzone
    spell_id: 97170
    cast_by: jin_do_the_godbreaker
    phase: 1
    cast_kind: scripted_ground
    target: ground_random
    duration_ms: persistent_until_replaced
    effect: |
      -90% magic damage taken inside; -90% cast speed inside.
      TACTICAL: stand IN during Shadows of Hakkar cast; OUT otherwise.
    interruptible: false
    avoidable: false                         # mechanic-required
    avoidance: encounter-specific TIMING — in/out based on Shadows of Hakkar cast bar
    priority: SCRIPTED_GROUND_ZONE
    notes: |
      ENCOUNTER-SPECIFIC OVERRIDE — normally ground AoE = avoid; here it is
      "stand in during specific cast window."

  - name: Shadows of Hakkar
    spell_id: 97172                          # alt 97173
    cast_by: jin_do_the_godbreaker
    phase: 1
    cast_kind: cast
    target: chain_burst_max_10_targets
    cast_time_ms: 3000
    effect: 121-128k shadow dmg per hit (un-mitigated)
    interruptible: false                     # UNVERIFIED — most reports non-interruptible
    avoidable: partial
    avoidance: be IN Deadzone during cast (-90% magic dmg = survivable)
    priority: GROUP_MOVE_INTO_DEADZONE

  - name: Shadow Spike
    spell_id: UNVERIFIED
    cast_by: jin_do_the_godbreaker
    phase: 2
    cast_kind: random_ground_target
    target: ally_random_position
    effect: explodes on impact; 63-77k dmg in 4y radius
    interruptible: false
    avoidable: true
    avoidance: move from ground markers

  - name: Body Slam (Gurubashi Spirit ability)
    spell_id: UNVERIFIED
    cast_by: gurubashi_spirit
    phase: 2
    cast_kind: leap
    target: ally_random
    effect: damage + knockback + applies Sunder Rift debuff
    notes: |
      TACTICAL: tank positions spirit so Body Slam lands ON A CHAIN
      LOCATION. Sunder Rift on chain = Brittle Barrier removed.

  - name: Sunder Rift (debuff)
    spell_id: UNVERIFIED
    cast_by: body_slam_proc
    phase: 2
    cast_kind: applied_debuff
    target: hit_target
    duration_ms: 20000
    effect: +100% damage taken
    notes: useful BOTH on chain (barrier removal) AND on spirit (damage amp)

  - name: Brittle Barrier
    spell_id: UNVERIFIED
    cast_by: hakkars_chains
    phase: 2
    cast_kind: passive_aura
    target: chain
    effect: chain immune to damage while barrier active
    removal: Sunder Rift / Body Slam landing on chain location

  - name: Frenzy (Gurubashi Spirit)
    spell_id: UNVERIFIED
    cast_by: gurubashi_spirit
    phase: 2
    cast_kind: passive_stacking_aura
    target: self
    effect: +20% attack speed per stack
    notes: soft-enrage on spirits

# Adds (Phase 2)
adds:
  - name: Gurubashi Spirit
    phase: 2
    abilities:
      - name: Body_Slam (chain-positioning mechanic)
      - name: Frenzy (soft-enrage)
    kill_priority: |
      Tank picks up; positions near chain so Body Slam lands on chain. Kill
      after all 3 chains broken OR if HP forces it.

  - name: Twisted Spirit
    phase: 2
    hp: ~24000
    spawn: increasing waves throughout Phase 2
    abilities:
      - name: melee_shadow
    kill_priority: AOE_CLEAVE
    notes: SOFT ENRAGE — accumulating waves overwhelm if not killed

  - name: Hakkar's Chains
    phase: 2
    count: 3
    state: protected_by_brittle_barrier
    objective: destroy all 3 to exit Spirit Realm
    attackable_when: brittle_barrier_removed (via Sunder Rift)

  - name: Gurubashi Spirit Warriors (pre-pull)
    phase: 0 (pre-fight)
    location: jin_do_stairs
    count: 2
    notes: kill before pulling Jin'do (some strats)

positioning:
  default: |
    Phase 1: tank pulls Jin'do near (but NOT on) where Deadzones spawn.
    On Shadows of Hakkar cast, group steps INTO active Deadzone; after cast,
    steps OUT.
    Phase 2 (Spirit Realm): party stands on chain locations; tank brings
    Gurubashi Spirit close so Body Slam lands on chain.
  tank_facing: away from group (standard)
  los_or_lookaway: none
  parallel_zone: Phase 2 = Spirit Realm (different zone/phase ID)

interrupt_priority: []                       # no confirmed kicks

dispel_priority: []                          # no dispels

heroic_delta: |
  Heroic-only fight.

bot_role_summary:
  tank: |
    Phase 1: reposition Jin'do near Deadzones (but not in them) so group
    can step IN/OUT cleanly.
    Phase 2: pick up Gurubashi Spirit, position so Body Slam lands on
    chain location.
  healer: |
    Phase 1: heavy AoE during Shadows of Hakkar; ensure positioned in
    Deadzone (or take 90%-reduced hit).
    Phase 2: sustained healing through Twisted Spirit waves + Body Slam
    impacts on debuffed players.
  melee: |
    Phase 1: standard melee; in/out of Deadzone with group.
    Phase 2: chain DPS when barrier down; AoE on Twisted Spirits between
    barrier windows.
  ranged: |
    Phase 1: max range (or in Deadzone); step in/out based on Shadows.
    Phase 2: ranged advantage for Shadow Spike dodge; cleave on Twisted
    Spirits.

bot_implementation_flag_phase2: |
  PHASE 2 PARALLEL ZONE TELEPORT. Bot session/state must handle:
    - Detect zone or phase ID change at 70% Jin'do HP
    - Maintain group references across the transition (party leader, tank,
      healer references must carry over)
    - Re-acquire targets on the other side (chains, spirits)
    - Detect return-to-physical-realm transition when 3 chains broken
  Without this, Jin'do Phase 2 is un-completable autonomously.

bot_implementation_flag_phase1: |
  DEADZONE IN/OUT TIMING. Encounter-specific override — normally ground AoE
  = avoid; here it's "stand in during specific cast window." Bot needs:
    - Detect Shadows of Hakkar cast START (3s warning) → MOVE INTO Deadzone
    - Detect Shadows of Hakkar cast END (resolves) → MOVE OUT of Deadzone
```

---

## Optional Boss — Cache of Madness (1 of 4 random spawns)

```yaml
boss: cache_of_madness
optional: true
default_skip: true
location: side chamber, requires Archaeology 225+ to unlock
hp_pct_phase_triggers: []
notes: |
  Random one-of-four spawn (Hazza'rah, Renataki, Wushoolay, Gri'lek). Bot
  defaults to SKIP — Archaeology dependency makes this practically
  unreachable for most bot runs.

  Brief mechanic notes preserved for completeness; if engaged, bot uses
  generic encounter heuristics (kite-from-fixate, dispel-roots if class has
  it, AoE on adds).

variants:
  - name: hazza_rah
    notes: nightmare illusion adds + sleep at 66/33%
  - name: renataki
    notes: vanish + thousand-blades; deadly poison stacks; 30% Frenzy
  - name: wushoolay
    notes: forked-lightning chain; lightning-cloud zones; spread mechanic
  - name: gri_lek
    notes: rupture-line ground effect; Avatar buff (kite); Pursuit + Entangling Roots

skip_default: true
clear_condition: group_leader_explicit_request AND archaeology_unlocked
```

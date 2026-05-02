# Throne of the Tides — Encounter Facts

**Role-neutral mechanic data** for the bot's encounter handling. Per-role response logic lives in sibling files (`healer.md`, `ranged-dps.md`, `melee-dps.md`, `tank.md`). When a role doc references a mechanic, it cites the spell ID defined here.

Spell IDs verified at `wowhead.com/cata/spell=<id>`. Strategy reconciled across 8 sources — see `docs/research/throne-of-the-tides-guide-survey.md` for the audit trail.

---

## Overview

```yaml
dungeon: throne_of_the_tides
zone: abyssal_depths
difficulty_modes: [normal, heroic]
level_range_normal: [80, 82]
level_range_heroic: 85
boss_count: 4
bosses_in_order:
  - lady_nazjar
  - commander_ulthok
  - mindbender_ghursha     # two-phase fight starting with Erunak Stonespeaker
  - ozumat                 # final escort/event encounter
notes: |
  Linear path with 3 trash gauntlets (naga, faceless, corruption). Final encounter
  is event-driven — Neptulon must survive. Heroic adds extra punishment to most
  mechanics; mechanics list below documents heroic delta inline.
```

---

## Trash — notable mechanics

Only the trash entries below have mechanics worth automating. Standard packs (basic murlocs, naga grunts) are pure threat-and-DPS and are not enumerated.

```yaml
trash:
  # Pre–Lady Naz'jar (naga gauntlet)
  - mob: Naz'jar Spiritmender
    npc_id: 50276
    abilities:
      - name: Healing Wave
        interruptible: true
        priority: MUST_INTERRUPT     # large heal — interrupting every cast is mandatory
      - name: Wrath
        interruptible: true
        priority: backup
      - name: Hex
        dispel_type: curse
    notes: priority kill or CC duplicate caster

  - mob: Naz'jar Sentinel
    abilities:
      - name: Crushing Depths
        dispel_type: none
        notes: stacking debuff, non-dispellable
      - name: Shellbreaker
        target: tank
      - name: Noxious Mire
        avoidable: true
        notes: ~5k Nature/s ground patch — tank kites off

  - mob: Naz'jar Invader
    abilities:
      - name: Slithering Assault
        dispel_type: enrage_purge      # Tranq Shot / Soothe / Purge

  - mob: Naz'jar Tempest Witch (trash)
    abilities:
      - name: Lightning Bolt
        interruptible: true
        priority: MUST_INTERRUPT
      - name: Lightning Surge
        dispel_type: magic
        notes: 5s delayed detonation; dispel before it pops

  - mob: Naz'jar Honor Guard (trash)
    abilities:
      - name: Arc Slash
        target: frontal_cone
        notes: tank face away

  - mob: Vicious Snap Dragon
    abilities:
      - name: Razor Jaws
        target: tank
        notes: stacking bleed; reset with stun (Kidney Shot, etc.)

  - mob: Naz'jar Ravager
    abilities:
      - name: Volatile Bolt
        avoidable: true
        notes: ground patches
      - name: Acid Barrage
        target: frontal_cone
        avoidable: true

  # Pre–Mindbender Ghur'sha (faceless wing)
  - mob: Faceless Watcher
    abilities:
      - name: Clenching Tentacles
        notes: lifts player; followed by Ground Pound — trinket / escape
      - name: Crush
        target: tank
        notes: constant; healer keeps tank topped

  - mob: Minion of Ghur'sha
    abilities:
      - name: Psionic Pulse
        avoidable: false
        notes: AoE — stunnable to lock down the cast

  - mob: Faceless Seer
    abilities:
      - name: Mind Flay
        interruptible: true
        priority: MUST_INTERRUPT

  - mob: Gilgoblin Aquamage
    abilities:
      - name: Water Bolt
        notes: ~5k Frost
      - name: haste_debuff
        dispel_type: magic

  # Pre–Ozumat (corruption hallway)
  - mob: Unstable Corruption
    abilities:
      - name: Wall of Corruption
        dispel_type: disease
        notes: triggers on death; stacking up to 20, ~250 DPS/stack — dispel between pulls

  - mob: Tainted Sentry
    abilities:
      - name: Swell
        interruptible: true
        dispel_type: enrage_purge
        notes: interrupt cast OR purge resulting buff
```

---

## Boss 1 — Lady Naz'jar

```yaml
boss: lady_nazjar
npc_id: 40586
location: first encounter, raised platform after naga gauntlet
hp_pct_phase_triggers: [60, 30]

phases:
  - id: 1
    name: regular
    trigger: pull
    end_condition: hp_pct == 60
    description: standard tank-and-spank with mechanics

  - id: 2
    name: waterspout_adds_first
    trigger: hp_pct == 60
    end_condition: all adds dead
    description: |
      Boss channels Waterspout, becomes immune (knockback melee, deflects spells).
      Spawns: 2x Naz'jar Tempest Witch, 1x Naz'jar Honor Guard.
    duration_estimate_ms: 60000

  - id: 3
    name: regular
    trigger: end of phase 2
    end_condition: hp_pct == 30
    description: resume DPS on boss

  - id: 4
    name: waterspout_adds_second
    trigger: hp_pct == 30
    end_condition: all adds dead
    description: same composition as phase 2

  - id: 5
    name: execute
    trigger: end of phase 4
    end_condition: hp_pct == 0
    description: burn boss

mechanics:
  - name: Summon Geyser
    spell_id: 75722
    cast_by: lady_nazjar
    cast_kind: instant_telegraph
    target: ground_random_player
    detonation_delay_ms: 5000
    effect: ~24-26k Frost + 5y knockback
    interruptible: false
    dispel_type: none
    avoidable: true
    avoidance: move out of telegraph circle before detonation
    notes: heroic adds water vortex eddies during waterspout phases (5s stun + ~5k Frost / 0.5s)

  - name: Fungal Spores
    spell_id: 76001
    cast_by: lady_nazjar
    cast_kind: projectile
    target: ally_random
    effect: Disease DoT, ~10-15k Nature / 3s for 15s, 5y splash
    interruptible: false
    dispel_type: disease
    avoidable: partial
    avoidance: spread to avoid splash; debuffed player should also move out of party
    uncertain:
      tick_damage: sources cite 10k (Icy Veins, Type-H-For-Heals) vs 15k (Wowhead) — likely normal vs heroic. Bot dispels regardless of value.

  - name: Shock Blast
    spell_id: 76008
    cast_by: lady_nazjar
    cast_kind: cast
    target: tank
    effect: ~70-79k Nature damage (tank-killer on heroic if uninterrupted)
    frequency_ms: ~10000-15000
    interruptible: true
    dispel_type: none
    avoidable: false
    priority: MUST_INTERRUPT
    notes: top-priority interrupt for the entire fight

  - name: Waterspout
    spell_id: 75683
    cast_by: lady_nazjar
    cast_kind: channel
    duration_ms: 60000
    target: self
    effect: boss immune; knocks back melee; deflects spells; summons add wave
    interruptible: false
    dispel_type: none
    avoidable: false
    notes: ends early when adds die; do NOT chase — kill adds

# Adds spawned during Waterspout phases
adds:
  - name: Naz'jar Honor Guard
    count: 1
    abilities:
      - name: Arc Slash
        spell_id: 75907
        target: frontal_cone
        effect: ~150% melee damage; undodgeable/unblockable/unparriable
        avoidable: yes — tank faces add away from group
      - name: Enrage
        spell_id: 22428
        trigger: hp_pct < 35
        effect: +50% attack speed, +20% size
        dispel_type: enrage_purge

  - name: Naz'jar Tempest Witch
    count: 2
    abilities:
      - name: Chain Lightning
        spell_id: 103637
        cast_kind: cast
        target: ally_3_jumps
        effect: 30% reduced per jump
        interruptible: true
        priority: MUST_INTERRUPT
      - name: Lightning Surge
        spell_id: 75992
        cast_kind: instant_debuff
        target: ally_random
        detonation_delay_ms: 5000
        radius: 8
        dispel_type: magic
        notes: dispel before detonation to prevent AoE

positioning:
  default: spread >=5y (Fungal Spores splash + Geyser knockback)
  tank_facing: face boss central, but face Honor Guard adds away from group during waterspout phases
  los_or_lookaway: none

interrupt_priority:
  - Shock Blast (boss)               # lethal-to-tank
  - Tempest Witch Chain Lightning
  - Tempest Witch Lightning Bolt     # secondary cast, varies by source

dispel_priority:
  - disease: Fungal Spores
  - magic: Lightning Surge (timed detonation — dispel before it pops)
  - enrage_purge: Honor Guard Enrage

heroic_delta: |
  - Shock Blast becomes lethal without interrupt (every cast).
  - Water vortex eddies appear during Waterspout — additional avoid-zones.
  - Adds hit substantially harder; CC strongly recommended on one Tempest Witch.
```

---

## Boss 2 — Commander Ulthok

```yaml
boss: commander_ulthok
npc_id: 40765
location: second encounter, faceless general in flooded chamber
hp_pct_phase_triggers: []
notes: single-phase ~5 minute execute; no HP-gated transitions

mechanics:
  - name: Squeeze
    spell_id: 76026
    cast_by: commander_ulthok
    cast_kind: targeted
    target: ally_random_non_tank
    effect: stuns 6s + ~5-12.5k Shadow / s
    interruptible: false
    dispel_type: none
    avoidable: false
    uncertain:
      damage: Wowhead 12.5k/s vs Wiki 5k/s — likely normal vs heroic. Bot treats as significant single-target spike (~30-50k+ over 6s).
    notes: pre-heal target; AoE heal supports

  - name: Dark Fissure
    spell_id: 76047
    cast_by: commander_ulthok
    cast_kind: cast_telegraph
    target: ground_in_front_of_boss
    initial_radius: 7
    effect: ~56-63k initial Shadow + persistent ~9-10k Shadow / s within 6y for ~60s
    interruptible: false
    dispel_type: none
    avoidable: true
    avoidance: move out of telegraph and stay clear of resulting fissure
    frequency_ms: ~15000-20000
    heroic_extra: fissures GROW and SPREAD over time, eating arena space; tank kites perimeter to leave clean ground

  - name: Curse of Fatigue
    spell_id: 76094
    cast_by: commander_ulthok
    cast_kind: instant
    target: ally_random
    effect: -50% movement / cast / melee / ranged speed for 15s
    interruptible: false
    dispel_type: curse
    avoidable: false
    priority: HIGH_DISPEL
    notes: dispel immediately — slowed players cannot escape fissures

  - name: Enrage
    spell_id: 76100
    cast_by: commander_ulthok
    effect: +50% physical damage 10s
    dispel_type: enrage_purge

positioning:
  default: tank kites Ulthok along the room perimeter; group stays mobile near tank
  movement_pattern: continuous repositioning by tank; fissures dropped along walls (heroic)
  los_or_lookaway: none

interrupt_priority: []   # no interruptible boss casts

dispel_priority:
  - curse: Curse of Fatigue (high — slowed players die in fissures)
  - enrage_purge: Ulthok Enrage

heroic_delta: |
  - Dark Fissure expands and persists, making the arena progressively unwinnable if mismanaged.
  - Squeeze hits harder.
  - Tight DPS check; Curse of Fatigue dispel becomes mission-critical.
```

---

## Boss 3 — Mindbender Ghur'sha (with Erunak Stonespeaker)

```yaml
boss: mindbender_ghursha
npc_id: 40788
companion_npc: erunak_stonespeaker
location: third encounter; two-stage. Erunak is mind-controlled by Ghur'sha until 50% HP, at which point Ghur'sha detaches and becomes the true boss.

phases:
  - id: 1
    name: erunak
    trigger: pull
    end_condition: erunak.hp_pct == 50
    description: |
      Players fight Erunak (the mind-controlled tauren shaman). Standard tank-and-spank
      with cone, melee debuff, and ground-spike mechanics.

  - id: 2
    name: ghursha
    trigger: erunak.hp_pct == 50
    end_condition: ghursha.hp_pct == 0
    description: |
      Ghur'sha (parasitic squid) detaches from Erunak. Erunak goes inert. Ghur'sha
      periodically Enslaves a random player.

mechanics_phase_1:
  - name: Emberstrike
    spell_id: 76165
    cast_by: erunak
    cast_kind: melee_debuff
    target: tank
    effect: +1885-2115 Fire on subsequent swings, 10s
    dispel_type: magic
    notes: dispel off tank to reduce ramp damage

  - name: Magma Splash
    spell_id: 76170
    cast_by: erunak
    cast_kind: cast
    target: frontal_cone_90deg
    range: 20
    effect: ~19-21k initial Fire + ~2k/s Fire DoT for 10s
    interruptible: false
    dispel_type: magic     # the DoT
    avoidable: true
    avoidance: tank face Erunak away from group; non-tanks stay behind

  - name: Lava Bolt
    spell_id: 76171
    cast_by: erunak
    cast_kind: cast
    target: ally_random
    effect: ~19-21k Fire + knockback
    interruptible: true
    priority: MUST_INTERRUPT

  - name: Earth Shards
    spell_id: 84931
    cast_by: erunak
    cast_kind: cast_telegraph
    target: ground_random
    effect: spike field, ~1885-2115 physical / 0.5s in 5y radius for 10s
    interruptible: false
    avoidable: true
    avoidance: move from spike telegraph

mechanics_phase_2:
  - name: Enslave
    spell_id: 76207
    cast_by: ghursha
    cast_kind: cast
    target: ally_random
    frequency_ms: ~60000
    effect: mind-controls victim with stat boost; victim must be DPS'd to 50% HP to break free
    interruptible: false   # not in flight (jump animation)
    dispel_type: none      # not removed by standard dispels — DAMAGE is the cure
    avoidable: false
    heroic_extra: enslaved player dies if not freed within 60s
    priority: CRITICAL_DPS_SWITCH
    anti_pattern: do NOT heal the enslaved player above 50% — they must drop below to break free

  - name: Mind Fog
    spell_id: 76230
    cast_by: ghursha
    cast_kind: instant_aoe
    target: ground_radius_10
    effect: silence/pacify + ~471-528 Shadow / 0.5s for 20s
    interruptible: false
    avoidable: true
    avoidance: move out of cloud

  - name: Absorb Magic
    spell_id: 76308
    cast_by: ghursha
    cast_kind: self_buff
    duration_ms: 3000-5000
    effect: absorbs all spell damage; Ghur'sha is healed for 3x absorbed
    interruptible: cast_only           # interrupt the cast if catchable
    dispel_type: magic_offensive       # purge/spellsteal preferred
    priority: HIGHEST_DISPEL_OFFENSIVE
    uncertain:
      duration: Wiki 3s vs Wowhead 5s — purge ASAP regardless
    notes: stop casting offensive spells if not purged within 1 GCD

  - name: Unrelenting Agony
    spell_id: 76339
    cast_by: ghursha
    cast_kind: channeled_aoe
    duration_ms: 10000
    effect: ~1885-2115 Shadow / s, all party members
    interruptible: false                # treat as not-interruptible (Wiki authoritative; some YouTube guides imply yes — UNCERTAIN, default to no for bot)
    avoidance_optional: LoS break if available (per Icy Veins); otherwise heal-through

positioning:
  phase_1: tank faces Erunak away from party (Magma Splash cone)
  phase_2: melee on boss; ranged spread to avoid Mind Fog overlap; ranged adjust to dodge fog clouds
  los_or_lookaway: optional LoS on Unrelenting Agony if geometry permits

interrupt_priority:
  - Lava Bolt (Erunak)
  - Absorb Magic cast (Ghur'sha) — purge preferred; interrupt cast if catchable

dispel_priority:
  - magic_offensive: Absorb Magic on Ghur'sha   # highest priority dispel in instance
  - magic: Emberstrike off tank
  - magic: Magma Splash DoT off cone-clipped allies

heroic_delta: |
  - Enslave kills the victim if not freed within 60s.
  - All damage values higher.
  - Tighter Mind Fog spacing.
```

---

## Boss 4 — Ozumat (Neptulon escort event)

```yaml
boss: ozumat
npc_id: 44566
location: final encounter, Neptulon's chamber. Defend Neptulon (water elemental) from Ozumat (kraken) across 3 phases. Neptulon dying ⇒ encounter resets.

phases:
  - id: 1
    name: filthy_tide
    trigger: pull / event start
    duration_estimate_ms: 60000
    description: |
      Waves of murlocs / casters / behemoth attack from chamber edges. Ozumat is
      not yet attackable. Adds must die fast to keep Neptulon up.

  - id: 2
    name: faceless_sappers
    trigger: end of phase 1
    end_condition: all 3 sappers dead
    description: |
      3x Faceless Sapper channel from above on Neptulon. Ground-level Blight Beasts
      spawn periodically. Ozumat spits Blight pools but is unattackable. Phase ends
      when all 3 Sappers die.

  - id: 3
    name: tidal_surge
    trigger: end of phase 2
    end_condition: ozumat.hp_pct == 0
    description: |
      Neptulon buffs the party (+500% max HP, +500% healing, +2000% damage).
      Ozumat becomes attackable. Race to kill before stacking Blight DoT
      becomes unhealable.

mechanics_phase_1:
  - mob: Deep Murloc Invader
    count: 6_per_pack
    pack_count: 3
    notes: melee-only; AoE-down quickly

  - mob: Vicious Mindlasher
    count: 3
    abilities:
      - name: Shadow Bolt
        effect: ~4.7-5.3k Shadow
        interruptible: true
      - name: Brain Spike
        effect: ~6.6-7.4k Shadow in 40y radius + 1k mana drain
        interruptible: true
        priority: MUST_INTERRUPT
        notes: TOP-PRIORITY phase-1 interrupt — party-wide damage + mana drain
      - name: Veil of Shadow
        difficulty: heroic_only
        effect: -50% healing 5s
        dispel_type: curse
        interruptible: true

  - mob: Unyielding Behemoth
    count: 1
    spawn_delay_ms: ~15000
    abilities:
      - name: Shadow Blast
        effect: ~23-26k Shadow in 5y + knockback
      - name: Blight Spray
        cast_kind: channel
        duration_ms: 4000
        target: frontal_cone_90deg
        range: 25
        effect: ~3.3-3.7k Shadow / s
        avoidance: tank faces Behemoth away from group AND Neptulon

mechanics_phase_2:
  - mob: Faceless Sapper
    count: 3
    location: ceiling
    notes: |
      Channel-tethered to Neptulon from above. Killing all 3 ends the phase.
      Standard kicks may not reach (ceiling targets) — ranged DPS responsibility.

  - mob: Blight Beast
    spawn: periodic
    abilities:
      - name: Aura of Dread
        difficulty: heroic_emphasis
        radius: 7
        effect: ~942-1057 Shadow / s + stacking +5% Shadow vulnerability per stack
        notes: tank kites; party MUST NOT cluster

  - boss_ability: Blight of Ozumat
    spell_id: 83561
    cast_by: ozumat
    cast_kind: ground_pool
    target: ground_random_player
    duration_ms: 30000
    radius: 10
    effect: stacking Shadow ticks
    avoidable: true
    avoidance: move out of pool

mechanics_phase_3:
  - buff: Tidal Surge
    cast_by: neptulon (party buff)
    target: party_all
    effect: +500% max HP, +500% healing received, +2000% damage dealt
    notes: keeps party alive vs stacking Blight while burning Ozumat

  - boss_ability: Blight of Ozumat (intensified)
    spell_id: 83561
    notes: |
      Constant ground spew across the entire room. Raid takes ~200 Shadow / s
      per stack, stacking up to 200. This is the soft enrage timer — burn Ozumat
      before stacks become unhealable.

  - mob_summon: Summon Blight of Ozumat
    spell_id: 83524

positioning:
  phase_1: party near Neptulon's pedestal; tank pulls Behemoth away to point cone outward
  phase_2: tank kites Blight Beasts on a circular path around the chamber; ranged DPS positioned to hit ceiling Sappers
  phase_3: stack on Ozumat; move only to dodge fresh Blight pools

interrupt_priority:
  - Brain Spike (Mindlasher, P1)         # mandatory — party-wide damage + mana drain
  - Mindlasher Shadow Bolt
  - Veil of Shadow cast (heroic only)

dispel_priority:
  - curse: Veil of Shadow (heroic)
  # no major magic dispels in this fight; Aura of Dread vulnerability is non-dispellable per most guides

heroic_delta: |
  - Mindlasher gains Veil of Shadow curse.
  - Aura of Dread vulnerability stacks faster on Blight Beasts.
  - Tighter P3 race; Tidal Surge buff still grants +2000% damage but Blight stacks ramp faster.
```

---

## UNCERTAIN items (sources disagree — confirm via DBC or in-game testing)

| Item | What's uncertain | Treatment in bot |
|---|---|---|
| Fungal Spores tick damage | 10k vs 15k per 3s — likely normal vs heroic | Bot dispels regardless of value |
| Squeeze damage | 5k/s (Wiki) vs 12.5k/s (Wowhead) — likely normal vs heroic | Treat as significant single-target spike (~30-50k+ over 6s) |
| Absorb Magic duration | 3s (Wiki) vs 5s (Wowhead) | Purge ASAP; don't depend on duration |
| Unrelenting Agony interruptibility | Wiki: not interruptible. Some YouTube guides imply yes. | Default: not interruptible (safer) |
| Add-kill priority on Lady Naz'jar Waterspout | Honor Guard first vs Witches first vs CC-then-Honor-Guard | Default: CC one Witch, kill Honor Guard first; otherwise kill Witches |
| "Beauty / pet pack" in ToTT | Misattributed by user; Beauty is in Blackrock Caverns | N/A for this dungeon |
| "Naga Mistral" mob name | Not present in current data; likely Naz'jar Tempest Witch | Treat as Tempest Witch in mechanics |

---

## Hooks for `AltbotCombat.cpp`

The YAML blocks above lift into the bot's encounter database:

| Doc block | Target structure |
|---|---|
| `phases` | `EncounterPhase[]` per boss — phase ID, trigger predicate, end condition |
| `mechanics` | `EncounterMechanic[]` — keyed on spell_id, evaluated when bot sees the cast event |
| `adds` / `mob` (trash) | `EncounterMob[]` — abilities table per add |
| `interrupt_priority` | `InterruptPriorityList` — ordered targets per phase |
| `dispel_priority` | `DispelPriorityList` — type-tagged with priority |
| `positioning` | `PositioningRule[]` — default + per-phase overrides |

Per-role response logic (when to use Spirit Link, which mob to focus, etc.) lives in the role docs and references this file's mechanics by `spell_id`.

# End Time — Encounter Facts

**Role-neutral mechanic data** for the bot's encounter handling. Per-role response logic lives in sibling files (`healer.md`, `ranged-dps.md`, `melee-dps.md`, `tank.md`). When a role doc references a mechanic, it cites the spell ID defined here.

Spell IDs verified at `wowhead.com/cata/spell=<id>` where consensus could be reached; many are flagged **UNVERIFIED** in this doc and aggregated in `docs/research/dbc-verification-checklist.md`. Strategy reconciled across 5+ sources — see `docs/research/end-time-guide-survey.md` for the audit trail.

---

## Overview

```yaml
dungeon: end_time
zone: end_time
zone_id: 5789                                # UNVERIFIED — verify in DBC
continent: caverns_of_time_tanaris            # accessed via CoT portal hub
difficulty_modes: [heroic_only]               # NO normal mode
level_range_normal: null                      # heroic-only
level_range_heroic: 85
ilvl_required_lfd: 353
ilvl_reward: 378
boss_count: 5                                 # 2 random echoes + Murozond + dynamic boss-roster of 4
echo_pool: [echo_of_baine, echo_of_jaina, echo_of_sylvanas, echo_of_tyrande]
echoes_per_run: 2                             # RANDOM — bot must reactively detect
final_boss: echo_of_murozond                  # always present
optional_bosses: []                           # all 5 required
bosses_in_run_order:
  - "{echo_A}"                                # random selection from echo_pool
  - "{echo_B}"                                # random selection from echo_pool (different from A)
  - echo_of_murozond                          # always last
content_patch: 4.3                            # Hour of Twilight content patch
gates: dragon_soul_raid                       # part of patch-4.3 troll-heroic-replacement set
companion_dungeons: [hour_of_twilight, well_of_eternity]  # other 2 in the 4.3 trio
notes: |
  Heroic-only 5-man. Hub-and-spoke layout: central Time Transit Device opens
  portals to 2 of 4 Dragonshrines. After both echoes are killed, the Bronze
  Dragonshrine portal opens with Echo of Murozond.

  No instance-wide buff/debuff. No instance-wide curse/disease/poison dispel
  requirement. Magic dispels MOSTLY blacklisted on Murozond (Temporal Blast
  is Hourglass-cleared, not dispel-cleared); Sylvanas Shriek slow is the
  notable Magic dispel.

  CRITICAL bot-implementation note: random echo selection means encounter
  profile loading must be REACTIVE, not pre-planned. Bot detects spawned
  echoes via NPC ID on first engagement OR by polling Time Transit Device
  portal targets at zone-entry.
```

---

## Random echo pool — bot detection logic

```yaml
random_echo_detection:
  pool:
    - { boss: echo_of_baine,    npc_id: 54431, shrine: obsidian_dragonshrine }
    - { boss: echo_of_jaina,    npc_id: 54445, shrine: azure_dragonshrine }
    - { boss: echo_of_sylvanas, npc_id: 54123, shrine: ruby_dragonshrine }
    - { boss: echo_of_tyrande,  npc_id: 54544, shrine: emerald_dragonshrine }

  detection_strategies:
    - id: STRATEGY_A_PORTAL_POLL
      description: |
        On zone-entry, poll the Time Transit Device's available portal
        destinations. Each open portal corresponds to a spawned echo's
        Dragonshrine. Returns 2 of 4 shrine IDs.
      reliability: UNVERIFIED — depends on portal-state being queryable from bot context
      preferred: yes (if available)

    - id: STRATEGY_B_NPC_ID_ON_ENGAGE
      description: |
        On first encounter-trigger (boss damaged or pulled), read the boss's
        NPC ID. Match to pool. Load corresponding encounter profile.
      reliability: HIGH — NPC ID is always available when boss is in combat
      preferred: fallback

    - id: STRATEGY_C_BOSS_FRAME_SCAN
      description: |
        Scan the WoW boss-frame UI for active boss names; match to pool.
      reliability: medium — UI state may lag

  bot_default: STRATEGY_B (most reliable; profile loads on engagement, not at zone entry)

  encounter_profile_loading: |
    On detected echo NPC ID, load the per-boss YAML block below as the active
    encounter profile. Discard the un-spawned echoes' profiles. After both
    echoes are dead, load echo_of_murozond profile.
```

---

## Trash — notable mechanics

Only the trash entries with mechanics worth automating. Standard packs are pure threat-and-DPS and not enumerated.

```yaml
trash:
  # Pre–Echo of Baine (Obsidian Dragonshrine)
  - mob: Time-Twisted Breaker
    abilities:
      - name: melee_cleave
        target: frontal_cone
        notes: tank face away
    notes: large physical mob; standard tank-and-spank

  - mob: Time-Twisted Drake
    abilities:
      - name: ranged_breath
        interruptible: UNVERIFIED
    notes: flying mob; tank pulls down via ranged threat (taunt)

  - mob: Time-Twisted Seer
    abilities:
      - name: caster_hardcast
        interruptible: true
        priority: MUST_INTERRUPT
    notes: caster — Polymorph / Hex / Sap valid; CC primary target if pull is risky

  # Pre–Echo of Jaina (Azure Dragonshrine)
  - mob: Risen Priest
    abilities:
      - name: heal_cast
        interruptible: true
        priority: MUST_INTERRUPT_HIGH
        notes: heals pack — kick mandatory
    notes: skull priority; CC valid (Polymorph)

  - mob: Risen Sorceress
    abilities:
      - name: Frostbolt_cast
        interruptible: true
        priority: MUST_INTERRUPT
    notes: caster; CC valid

  jaina_pre_fight_quest:
    name: Collect_Jaina_Fragments
    quantity: 16
    notes: |
      Group must collect 16 staff fragments scattered around the shrine
      before Echo of Jaina can be engaged. All players contribute pickups
      while clearing trash. Bot logic: on shrine-entry, scan for fragment
      gameobjects; assign collection in passing.

  # Pre–Echo of Sylvanas (Ruby Dragonshrine)
  - mob: Time-Twisted Rifleman
    abilities:
      - name: ranged_shot
        target: distant_random
    notes: ranged physical; LoS-pull or tank-taunt

  - mob: Risen Banshee
    abilities:
      - name: caster_hardcast
        interruptible: true
        priority: MUST_INTERRUPT
    notes: caster; CC valid

  # Pre–Echo of Tyrande (Emerald Dragonshrine) — UNIQUE TANK MECHANIC
  - mob: Tyrande_pre_fight_trash
    abilities:
      - name: shadow_shield
        passive: true
        effect: 90% damage reduction OUTSIDE moonbeams
        counter: drag mob INTO moonbeam to strip shield
    notes: |
      UNIQUE TANK MECHANIC. Trash mobs have a 90% damage-reduction shield
      that strips when they stand in a moonbeam. Tank must drag mobs into
      a rotating moonbeam (West → South → East → North, ~45s rotation) to
      enable damage. DPS does NOT open damage until mob is in beam.
      Encoded in tank.md as a fight-specific override.

  # Pre–Murozond (Bronze Dragonshrine)
  - mob: Bronze_dragonkin_trash
    notes: assorted casters + melee; CC valid; standard kill order

trash_skip_notes: |
  No documented LoS-skip routes. All trash on the path from Time Transit
  Device to each shrine is mandatory.

cc_priorities:
  polymorph_hex_sap_valid:
    - Time-Twisted Seer
    - Risen Priest
    - Risen Sorceress
    - Risen Banshee
  not_typical_cc_targets:
    - Time-Twisted Breaker
    - Time-Twisted Drake
    - Time-Twisted Rifleman
```

---

## Boss 1 (random) — Echo of Baine

```yaml
boss: echo_of_baine
npc_id: 54431
shrine: obsidian_dragonshrine
spawn_chance: 50%                              # 2 of 4 random pool
location: multi-platform area surrounded by lava
hp_pct_phase_triggers: []                      # no HP gates; mechanic-driven phase loss
notes: |
  DPS-RACE encounter. Single-phase mechanic-loss: each Pulverize sinks one
  platform. 4 platforms total → group rotates as platforms sink. Kill before
  platform 4 sinks or wipe by lava-stacking-damage.

phases:
  - id: 1
    name: starting_platform
    trigger: pull
    end_condition: first Pulverize
    description: tank-and-spank on starting platform; group on starting platform; ranged spread

  - id: 2
    name: platform_rotation
    trigger: each Pulverize event
    end_condition: hp_pct == 0 OR platform_4_sunk (wipe)
    description: |
      After each Pulverize, group swims/jumps to next platform. Tank
      maintains threat; melee follows; ranged repositions. DPS rate must
      kill boss before platform 4 sinks.

mechanics:
  - name: Pulverize
    spell_id: 101626                           # UNVERIFIED — see DBC list
    cast_by: echo_of_baine
    cast_kind: leap
    target: ranged_random
    cast_time_ms: ~instant_leap
    cooldown_ms: ~50000
    effect: ~30k physical damage on landing; sinks platform he lands on
    interruptible: false                       # leap, not hardcast
    dispel_type: none
    avoidable: partial
    avoidance: ranged spread; whoever Pulverize lands on takes hit + platform-sink
    priority: PLATFORM_ROTATION_TRIGGER
    notes: |
      Bot tank: do NOT chase Baine to ranged platform; let him leap, then
      group migrates AFTER landing. Boss returns to tank threat after leap.

  - name: Molten Mace                          # aka Molten Axe (wording varies — see survey Disagreement #1)
    spell_id: 101836                           # UNVERIFIED
    cast_by: echo_of_baine
    cast_kind: passive
    target: self_buff
    trigger: boss touches lava
    effect: +9750-10250 fire damage to melee swings
    interruptible: false
    dispel_type: none                          # boss buff; can be Purged?  UNVERIFIED
    avoidable: yes — tank avoids lava
    notes: |
      Tank-positional concern. NEVER drag boss into lava; this is a buff for
      the boss, NOT a buff for the tank. (Player-facing "Molten Fists"
      interpretation appears to be a misread — see survey Disagreement #1.)

  - name: Throw Totem
    spell_id: 101614                           # UNVERIFIED
    cast_by: echo_of_baine
    cast_kind: ranged_toss
    target: random_player
    cooldown_ms: ~20000-30000
    effect: ~58-61k physical damage + knockback to target
    interruptible: false                       # not hardcast
    dispel_type: none
    avoidable: false                           # but counterable
    counter_mechanic:
      action: pick_up_thrown_totem (interact with gameobject on ground)
      then: throw_totem_back_at_boss (interact again with totem-in-hand at boss)
      effect: boss stunned + 5% max HP damage + 50% damage-taken-up debuff for 20s
      bot_implementation_note: |
        FIGHT-SPECIFIC BOT ABILITY. Bot must reactively detect Throw Totem
        cast event, locate the totem gameobject on the ground, click-interact
        to pick up, then re-throw at boss. Not part of standard rotation
        engine. Assigned to ranged DPS by default (closer to ground location).
    priority: BOT_REACTIVE_INTERACTION

  - name: melee
    cast_by: echo_of_baine
    cast_kind: melee
    target: tank
    notes: standard tank melee; with Molten Mace adds fire damage

# No adds spawn during this fight
adds: []

positioning:
  default: |
    Tank: starting platform; boss centered; OUT of lava (avoid Molten Mace).
    Melee: in melee on boss; out of lava.
    Ranged DPS / healer: opposite platform from boss (Pulverize jumps to ranged).
    Spread among ranged players (Pulverize multi-impact prevention).
  tank_facing: away from group (standard, even though no confirmed cone — discipline)
  los_or_lookaway: none
  fight_specific_override: |
    PLATFORM ROTATION on Pulverize cast event:
    - Platform sinks ~3-5s after Pulverize lands
    - Group migrates to adjacent platform (swim through lava → stacking lava DoT)
    - DPS race: kill before platform 4 sinks

interrupt_priority: []                          # no kicks

dispel_priority: []                              # no dispels

heroic_delta: |
  HEROIC ONLY — no delta. All numerics here are heroic.

bot_role_summary:
  tank: hold boss on starting/current platform; never enter lava; chase across platforms when forced
  healer: pre-HoT before Pulverize; group lava-swim damage spikes between platforms
  melee: stay in melee; out of lava; hop platforms with tank
  ranged: spread on opposite platform; PRIMARY THROW-TOTEM HANDLER (pick up + throw back); race-DPS pressure
```

---

## Boss 2 (random) — Echo of Jaina

```yaml
boss: echo_of_jaina
npc_id: 54445
shrine: azure_dragonshrine
spawn_chance: 50%
location: open shrine area; high mobility (Blink)
hp_pct_phase_triggers: []
notes: |
  Pre-fight: collect 16 Jaina Fragments scattered around shrine while
  clearing trash (Risen Priest + Risen Sorceress). Boss engages on
  fragment-completion.

  High-mobility caster fight. Frequent Blink repositioning. Primary kick
  rotation load on Pyroblast + Frostbolt Volley.

pre_fight:
  quest: Collect 16 Jaina Fragments
  trash_priority: Risen Priest (heal-cast — kick mandatory) > Risen Sorceress (caster)

phases:
  - id: 1
    name: regular
    trigger: pull (post-fragment-collection)
    end_condition: hp_pct == 0
    description: |
      Continuous tank-and-spank with Pyroblast + Frostbolt Volley kick
      rotation; periodic Frost Blades cones + Flarecore embers + Blink
      repositioning. No HP gates.

mechanics:
  - name: Pyroblast
    spell_id: 101809                           # UNVERIFIED
    cast_by: echo_of_jaina
    cast_kind: cast
    target: tank
    cast_time_ms: ~3000
    cooldown_ms: ~15000-20000
    effect: ~50k fire damage + 12s DoT (~5k/3s)
    interruptible: true
    dispel_type: none                          # debuff is the DoT — magic, dispellable but DoT is too short for typical dispel rotation
    avoidable: false                           # interruptible-only avoidance
    priority: MUST_INTERRUPT_PRIMARY
    notes: TOP kick rotation target on this fight

  - name: Frostbolt Volley
    spell_id: 101810                           # UNVERIFIED
    cast_by: echo_of_jaina
    cast_kind: cast
    target: pbaoe_self_65y
    cast_time_ms: ~2000
    cooldown_ms: ~20000-30000
    effect: ~18k frost damage to all enemies in 65y + 30% movement slow 4s
    interruptible: true
    dispel_type: magic                         # slow is dispellable; group-wide so usually not worth single-dispel
    avoidable: false                           # group-wide range
    priority: MUST_INTERRUPT_SECONDARY

  - name: Flarecore
    spell_id: 101927                           # UNVERIFIED
    cast_by: echo_of_jaina
    cast_kind: scripted
    target: ground_orbs
    effect: |
      Spawns fire-orb ground patch. Detonate within ~5s by walking into
      it for ~5-50k damage to nearby player. If uncontested, ~94k room-wide
      explosion at natural detonation.
    interruptible: false
    dispel_type: none
    avoidable: partial
    avoidance: |
      MECHANIC: 1 designated player (highest-HP ranged DPS) walks ember to
      detonate within 5s. Tank does NOT detonate (already heavy single-
      target pressure from Pyroblast). See survey Disagreement #6.
    priority: DESIGNATED_DETONATOR_REACTIVE

  - name: Frost Blades
    spell_id: 101339                           # UNVERIFIED
    cast_by: echo_of_jaina
    cast_kind: cast
    target: forward_cone
    cast_time_ms: ~2000
    effect: 3 expanding frost projectiles in cone; hit = 5s stun
    interruptible: false                       # cone, not single-target hardcast
    dispel_type: none
    avoidable: true
    avoidance: stand BETWEEN blade lanes (3 projectiles spread; gaps between)

  - name: Blink
    spell_id: 101812                           # UNVERIFIED
    cast_by: echo_of_jaina
    cast_kind: instant
    target: self_teleport
    cooldown_ms: ~10000-15000
    effect: boss teleports to new room location
    interruptible: false
    dispel_type: none
    avoidable: false
    notes: tank chases on each Blink; melee follows; ranged repositions

  - name: melee
    cast_by: echo_of_jaina
    cast_kind: melee
    target: tank
    notes: caster boss — low melee damage

adds: []                                        # no adds; pre-fight trash is separate

positioning:
  default: |
    Tank: in melee on boss; chases Blink; faces away from group.
    Melee: behind boss; chase Blink with tank.
    Ranged DPS / healer: spread for Frostbolt Volley AoE; max range from
    boss for Frost Blades safety.
    Flarecore detonator: 1 ranged player (highest-HP) walks ember within 5s.
  tank_facing: away from group
  los_or_lookaway: none
  fight_specific_override: |
    Tank Blink-chase override: when Blink fires, all melee bots (including
    tank) re-engage at new boss location ASAP. Healer + ranged reposition
    standard.

interrupt_priority:
  - Pyroblast (echo_of_jaina)                   # PRIMARY — every cast
  - Frostbolt Volley (echo_of_jaina)            # SECONDARY — every cast

dispel_priority:
  - { spell_id: 101810, name: Frostbolt Volley slow, school: magic, priority: LOW, notes: dispel-on-tank if kick missed AND tank-mobility critical }

heroic_delta: |
  HEROIC ONLY — no delta.

bot_role_summary:
  tank: chase Blink fast; pre-shield before Pyroblast cast (if cast unkicked)
  healer: HEAVIEST interrupt-failure pressure; pre-HoT tank for Pyroblast; rotate dispel on Frostbolt slow if needed
  melee: chase Blink; Frost Blades cone awareness; standard rotation
  ranged: spread for Frostbolt Volley; designated Flarecore detonator (highest HP); kick rotation for Pyroblast / Frostbolt Volley
```

---

## Boss 3 (random) — Echo of Sylvanas

```yaml
boss: echo_of_sylvanas
npc_id: 54123
shrine: ruby_dragonshrine
spawn_chance: 50%
location: open shrine
hp_pct_phase_triggers: []
notes: |
  Single-phase tank-and-spank with periodic Calling of the Highborne
  ghoul-circle event (signature mechanic). Spread always for Black Arrow /
  Unholy Shot AoE; on Calling, group focuses 1 ghoul to break circle.

phases:
  - id: 1
    name: regular_with_calling_events
    trigger: pull
    end_condition: hp_pct == 0
    description: |
      Continuous tank-and-spank with Black Arrow / Unholy Shot / Shriek of
      the Highborne / Blighted Arrows. Periodic Calling of the Highborne
      events (~30-45s? UNVERIFIED cadence) interrupt regular DPS.

  - id: 1b
    name: calling_event
    trigger: Calling of the Highborne cast
    end_condition: 1 ghoul dead → circle broken → players escape gap
    description: |
      Sylvanas ascends; pulls group to her; 8 Risen Ghouls spawn in circle
      with shadowy links between them; ghouls march inward; group focuses
      1 ghoul to break circle; escape through gap before ghouls reach
      Sylvanas (Sacrifice = wipe).

mechanics:
  - name: Calling of the Highborne
    spell_id: UNVERIFIED
    cast_by: echo_of_sylvanas
    cast_kind: channel
    target: pbaoe_self_pull
    cast_time_ms: ~3000-5000
    effect: |
      Pulls all players to Sylvanas; spawns 8 Risen Ghoul adds in circle
      with Wracking Pain shadowy links between them; ghouls slow-march
      toward Sylvanas. Players must DPS 1 ghoul to break circle.
      If ghoul reaches Sylvanas: Sacrifice cast → wipe.
    interruptible: false                       # channel, mechanic-intended
    dispel_type: none
    avoidable: false                           # mechanic-intended event
    priority: GROUP_FOCUS_TARGET_SHIFT
    notes: |
      Bot logic: on cast detection, all DPS (including melee) shift target
      to LOWEST-HP ghoul (ties broken by closest to bot's position) — see
      survey Disagreement #5. Tank's threat doesn't matter to ghouls.

  - name: Black Arrow
    spell_id: UNVERIFIED
    cast_by: echo_of_sylvanas
    cast_kind: cast
    target: random_player
    cast_time_ms: ~2000
    effect: explosive projectile; ~10y radius AoE on landing
    interruptible: UNVERIFIED                  # likely no — cast is short
    dispel_type: none
    avoidable: true
    avoidance: spread 8-10y minimum between players

  - name: Unholy Shot
    spell_id: UNVERIFIED
    cast_by: echo_of_sylvanas
    cast_kind: cast
    target: random_player
    cast_time_ms: ~2000
    effect: missile barrage; hits up to 3 nearby + DoT
    interruptible: UNVERIFIED
    dispel_type: none
    avoidable: true
    avoidance: spread (multi-target = spread mitigates)

  - name: Shriek of the Highborne
    spell_id: UNVERIFIED
    cast_by: echo_of_sylvanas
    cast_kind: instant
    target: random_player
    effect: -50% movement speed for 30s + shadow damage tick
    interruptible: false
    dispel_type: magic
    avoidable: false
    avoidance: dispel slow before next Calling event (slowed player can't escape ghoul circle)
    priority: DISPEL_HIGH_BEFORE_CALLING

  - name: Blighted Arrows
    spell_id: UNVERIFIED
    cast_by: echo_of_sylvanas
    cast_kind: scripted
    target: ground_runes
    effect: ground rune marks; standing in rune > 5s = lethal damage
    interruptible: false
    dispel_type: none
    avoidable: true
    avoidance: move out of rune within 5s

  - name: Wracking Pain (ghoul aura)
    spell_id: UNVERIFIED
    cast_by: risen_ghoul
    cast_kind: passive
    target: ground_links_and_shadows
    effect: 1/sec damage tick for crossing link or standing in shadow zone behind ghoul
    interruptible: false
    dispel_type: none
    avoidable: true
    avoidance: do NOT cross links; kill 1 ghoul to break

  - name: Sacrifice (ghoul fail-state)
    spell_id: UNVERIFIED
    cast_by: risen_ghoul
    cast_kind: scripted
    trigger: ghoul reaches Sylvanas alive
    effect: ENCOUNTER WIPE — group cannot survive
    interruptible: false
    dispel_type: none
    avoidable: true
    avoidance: kill 1 ghoul before any reach Sylvanas

  - name: melee
    cast_by: echo_of_sylvanas
    cast_kind: melee
    target: tank

# Adds: 8 Risen Ghouls per Calling event
adds:
  - name: Risen Ghoul
    count: 8 (per Calling event)
    spawn_pattern: circle around Sylvanas
    abilities:
      - { name: Wracking Pain (link aura), see_mechanic: above }
      - { name: melee, target: closest_player }
      - { name: Sacrifice, on_reach_sylvanas: WIPE }
    notes: |
      KILL 1 GHOUL to break circle. Lowest-HP target wins (Disagreement #5).
      Achievement Severed Ties: kill 2+ ghouls per Calling — bot-optional.

positioning:
  default: |
    SPREAD always (Black Arrow + Unholy Shot + Blighted Arrows all AoE-target).
    8-10y spacing minimum.
  tank_facing: away from group
  los_or_lookaway: none
  fight_specific_override: |
    On Calling of the Highborne: group is pulled to boss → all DPS (incl
    tank-as-DPS-momentarily) focus 1 ghoul → escape through gap → re-spread.

interrupt_priority: []                          # no confirmed kicks

dispel_priority:
  - { spell_id: UNVERIFIED, name: Shriek of the Highborne slow, school: magic, priority: HIGH_BEFORE_CALLING, notes: dispel slowed player BEFORE next Calling event }

heroic_delta: |
  HEROIC ONLY — no delta.

bot_role_summary:
  tank: standard tank-and-spank; on Calling, focus ghoul kill (threat irrelevant to ghouls)
  healer: rotate dispels on Shriek slow before Calling; reactive heal Blighted Arrow targets; pre-HoT for Calling pull
  melee: spread always; on Calling, focus 1 ghoul (lowest HP); standard rotation otherwise
  ranged: spread always; on Calling, focus 1 ghoul; range advantage for Blighted Arrows movement
```

---

## Boss 4 (random) — Echo of Tyrande

```yaml
boss: echo_of_tyrande
npc_id: 54544
shrine: emerald_dragonshrine
spawn_chance: 50%
location: open shrine
hp_pct_phase_triggers: [80, 55, 30]
notes: |
  TOP-PRIORITY KICK FIGHT — Stardust must be interrupted every cast.
  HP-gated cast-haste stacks (Lunar Guidance @ 80% and 55%) make kicks
  progressively tighter. 30% HP burn phase (Tears of Elune).

  Pre-fight trash: tank drags mobs INTO rotating moonbeams to strip 90%
  damage-reduction shield (encoded in trash + tank.md).

phases:
  - id: 1
    name: opening
    trigger: pull
    end_condition: hp_pct == 80
    description: tank-and-spank with Stardust kick rotation; standard cast cadence

  - id: 2
    name: lunar_guidance_1
    trigger: hp_pct == 80
    end_condition: hp_pct == 55
    description: Boss gains Lunar Guidance stack 1 (+25% cast speed); kicks tighter

  - id: 3
    name: lunar_guidance_2
    trigger: hp_pct == 55
    end_condition: hp_pct == 30
    description: Boss gains Lunar Guidance stack 2 (+50% cast speed total); kicks very tight

  - id: 4
    name: tears_of_elune_burn
    trigger: hp_pct == 30
    end_condition: hp_pct == 0
    description: |
      Stars rain on player feet (~30k arcane in 4y radius). BURN PHASE —
      kill the boss; do NOT kite. Use Bloodlust + major CDs here.

mechanics:
  - name: Stardust
    spell_id: 102173                           # UNVERIFIED
    cast_by: echo_of_tyrande
    cast_kind: cast
    target: pbaoe_60y
    cast_time_ms: ~2000 (reduced by Lunar Guidance to ~1500 / ~1000)
    cooldown_ms: ~10000-15000
    effect: ~50k arcane damage to ALL players within 60y
    interruptible: true
    dispel_type: none
    avoidable: false                           # interrupt-or-eat
    priority: MUST_INTERRUPT_TOP_PRIORITY
    notes: |
      ABSOLUTE TOP PRIORITY KICK in End Time. Missing = ~50k group damage =
      potential wipe. Bot interrupt rotation: any class with kick on this
      target FIRST.

  - name: Moonbolt
    spell_id: 102193                           # UNVERIFIED
    cast_by: echo_of_tyrande
    cast_kind: cast
    target: tank
    cast_time_ms: ~2000
    effect: ~40k arcane damage on current target
    interruptible: UNVERIFIED                  # disagreement — see survey Disagreement #2
    dispel_type: none
    avoidable: false
    priority: MUST_INTERRUPT_SECONDARY (try if Stardust on cd)

  - name: Moonlance
    spell_id: 102149                           # UNVERIFIED
    cast_by: echo_of_tyrande
    cast_kind: cast
    target: ground_lance
    cast_time_ms: ~2000-3000
    effect: ground lance travels; splits into 3 at edge; ~50k arcane + 5s stun on hit
    interruptible: false                       # ground-targeted
    dispel_type: none
    avoidable: true
    avoidance: sidestep lance lane

  - name: Dark Moonlight
    spell_id: 102414                           # UNVERIFIED
    cast_by: echo_of_tyrande
    cast_kind: passive_aura
    target: 15y_radius_around_boss
    effect: -50% cast speed for casters within 15y
    interruptible: false
    dispel_type: UNVERIFIED                    # some sources call this dispellable
    avoidable: true
    avoidance: healers + caster DPS stand OUTSIDE 15y aura
    priority: HEALER_POSITIONAL_OVERRIDE

  - name: Lunar Guidance
    spell_id: 33589                            # UNVERIFIED — likely Cata-era re-ID needed
    cast_by: echo_of_tyrande
    cast_kind: passive_hp_gated
    target: self_buff
    triggers: [hp_pct == 80, hp_pct == 55]
    effect: +25% cast speed per stack (max 2 stacks = +50% total)
    interruptible: false
    dispel_type: UNVERIFIED                    # offensively purgable? — bot defaults to no
    avoidable: false
    notes: HP-gated; kicks become progressively tighter

  - name: Tears of Elune
    spell_id: 102241                           # UNVERIFIED
    cast_by: echo_of_tyrande
    cast_kind: passive_hp_gated
    target: ground_player_feet
    trigger: hp_pct == 30
    effect: continuous stars dropping at player feet; ~30k arcane in 4y radius
    interruptible: false
    dispel_type: none
    avoidable: true
    avoidance: reposition every few seconds; do NOT kite (burn phase)

  - name: Eyes of the Goddess / Piercing Gaze of Elune
    spell_id: UNVERIFIED
    cast_by: echo_of_tyrande
    cast_kind: scripted_spawn
    target: mobile_silence_orb
    effect: orb circles room; contact = 5s silence
    interruptible: false
    dispel_type: none
    avoidable: true
    avoidance: avoid orb path; spread minimizes silence-multi-hit

  - name: melee
    cast_by: echo_of_tyrande
    cast_kind: melee
    target: tank
    notes: caster boss — low melee

adds: []                                        # pre-fight trash is the moonbeam mechanic; no boss adds

positioning:
  default: |
    Tank: in melee on boss; takes Dark Moonlight aura penalty (no choice);
    faces boss away from group.
    Melee: behind boss; in 15y aura (cast speed irrelevant for melee).
    Ranged DPS / healer: OUTSIDE 15y Dark Moonlight aura — position at
    boundary; max range strategy ABANDONED in favor of "edge-of-aura" strategy.
  tank_facing: away from group
  los_or_lookaway: none
  fight_specific_override: |
    HEALER + RANGED DPS positioning override: stand OUTSIDE 15y Dark Moonlight
    aura. NOT max range; specifically just outside the aura boundary. Standard
    "ranged at max range" rule does not apply — being outside the aura is the
    constraint.

interrupt_priority:
  - Stardust (echo_of_tyrande)                   # ABSOLUTE TOP PRIORITY
  - Moonbolt (echo_of_tyrande)                   # secondary — try if Stardust on cd

dispel_priority: []                              # nothing confirmed dispel-required

heroic_delta: |
  HEROIC ONLY — no delta.

bot_role_summary:
  tank: standard tank-and-spank; chases boss minimally; faces away from group
  healer: pre-HoT before Stardust if kick uncertain; STAND OUTSIDE 15y Dark Moonlight aura (positional override)
  melee: behind boss; Moonlance dodge; Eyes of the Goddess avoidance; standard rotation
  ranged: PRIMARY STARDUST KICK PARTICIPANT; outside Dark Moonlight aura; burn at 30% (Tears of Elune)

achievement: { name: Moon_Guard, requirement: heroic + Stardust gauntlet without healer dmg, bot_optional: true }
```

---

## Boss 5 — Echo of Murozond (FINAL — always present)

```yaml
boss: echo_of_murozond
npc_id: 54432
shrine: bronze_dragonshrine
spawn: always (after both echoes are killed)
location: Bronze Dragonshrine
hp_pct_phase_triggers: []                      # single-phase with Hourglass-event interrupts
notes: |
  Single-phase DPS race with Hourglass-of-Time reset cushion. Floor fills
  with permanent Distortion Bombs over time; group runs out of safe space.
  Hourglass click resets cooldowns + HP/mana + clears bombs + revives
  dead players + teleports to start positions. Boss HP UNCHANGED by
  Hourglass. Hard cap of 5 uses per fight.

  THE PROMPT'S "TIME REPLAY" FRAMING: original prompt context describes
  Murozond as "boss replays player actions" — this is NOT the actual Cata
  4.3.4 mechanic. The real mechanic is the Hourglass rewinding TIME-STATE
  (positions, cooldowns, HP, ground patches) without recording or replaying
  player actions. The bot caution that maps to actual mechanics: don't
  waste offensive cooldowns immediately before a planned Hourglass click
  since they'll be reset and re-usable post-click. Captured as
  "Hourglass-aware CD pacing" in role docs.

phases:
  - id: 1
    name: regular
    trigger: pull
    end_condition: hp_pct == 0 OR hourglass_charges_exhausted_AND_arena_full (wipe)
    description: |
      Continuous tank-and-spank. Periodic Temporal Blast (every ~15s) ramps
      stacks. Periodic Distortion Bomb on random ranged → permanent ground
      patch. Tank holds boss with frontal cone facing AWAY (Infinite Breath).
      Hourglass clicked reactively when bombs ≥ 4 OR group HP < 30% OR
      stacks ≥ 4 → resets state.

mechanics:
  - name: Infinite Breath
    spell_id: UNVERIFIED
    cast_by: echo_of_murozond
    cast_kind: cast
    target: frontal_cone
    cast_time_ms: ~2000
    cooldown_ms: ~12000-15000
    effect: ~75k fire damage to all enemies in front
    interruptible: false                       # not kickable
    dispel_type: none
    avoidable: true
    avoidance: tank faces boss AWAY from group; group stays out of frontal arc

  - name: Temporal Blast
    spell_id: UNVERIFIED
    cast_by: echo_of_murozond
    cast_kind: passive_cycle
    target: pbaoe_self
    cooldown_ms: ~15000
    effect: ~25k shadow damage to nearby + applies stacking +10% arcane damage taken (20s duration)
    interruptible: false
    dispel_type: magic                         # but DO NOT dispel — Hourglass clears stacks
    DISPEL_BLACKLIST: true
    avoidable: false
    notes: |
      Stacks ramp Distortion Bomb damage taken. Hourglass click clears all
      stacks. Dispelling 5 stacks across 5 players = wasted GCDs vs.
      single Hourglass click. ADD TO DISPEL-BLACKLIST.

  - name: Distortion Bomb
    spell_id: UNVERIFIED
    cast_by: echo_of_murozond
    cast_kind: scripted
    target: ranged_random_player_location
    effect: |
      Orb travels to player's location; on landing, leaves PERMANENT 8y
      radius temporal-distortion zone dealing ~25k arcane/sec to all in
      zone. Persists until end-of-fight OR Hourglass click clears.
    interruptible: false
    dispel_type: none                          # ground patch, not unit aura
    avoidable: true
    avoidance: |
      Targeted player moves out before bomb lands. Floor fills over time;
      Hourglass click clears all bomb zones (one of the click's primary
      values).
    priority: GROUND_PATCH_AWARENESS

  - name: Tail Sweep
    spell_id: UNVERIFIED                       # see survey Disagreement #3 — existence uncertain
    cast_by: echo_of_murozond
    cast_kind: passive
    target: rear_cone
    effect: knockback + DoT (~15k/3s for 15s)
    interruptible: false
    dispel_type: none
    avoidable: true
    avoidance: melee positions to SIDE of boss (not strict rear) — see Disagreement #3 default
    notes: |
      Some sources omit this ability. Conservative default: assume present.
      Melee positioning rule: side, not strict rear.

  - name: melee
    cast_by: echo_of_murozond
    cast_kind: melee
    target: tank

# Encounter object — Hourglass of Time
encounter_object:
  - name: Hourglass of Time
    type: clickable_gameobject
    gameobject_id: UNVERIFIED
    location: fixed in arena (Bronze Dragonshrine center? UNVERIFIED exact spot)
    on_click_cast: Rewind Time
    cast_spell_id: UNVERIFIED
    effect: |
      All player cooldowns reset.
      All player HP/mana restored to 100%.
      All players teleported to starting positions.
      All Distortion Bomb ground zones cleared.
      All Temporal Blast stacks cleared.
      All dead party members revived (full HP/mana).
      BOSS HP UNCHANGED (damage on him persists).
    max_uses_per_fight: 5
    priority: BOT_REACTIVE_INTERACTION
    bot_implementation_notes: |
      ASSIGNED CLICKER: highest-mobility ranged DPS (Mage with Blink, Warlock
      with Demonic Circle) by default. Fallback: any ranged DPS bot. Tank
      and healer DO NOT click (they have positional/healing duties).

      CLICK-TRIGGER CONDITIONS (any-of):
      - Distortion Bomb count on floor >= 4
      - party_lowest_hp < 30%
      - Temporal Blast stack count on any player >= 4
      - any party member dead AND boss > 30% HP

      PRE-CLICK CD-PACING RULE:
      - If a click is imminent (next ~5s window), do NOT pop new offensive
        CDs (Bloodlust, Pillar of Frost, Avenging Wrath, etc.) — they'll
        be reset.
      - DO use any cooldowns that are currently active (sunk cost — they're
        live now).
      - Pop new CDs AFTER click (cooldowns just reset → free re-use).

# No adds
adds: []

positioning:
  default: |
    Tank: in melee; faces boss AWAY from group (Infinite Breath cone);
    migrates boss as Distortion Bombs land — never park boss on existing bomb.
    Melee: SIDE of boss (not strict rear, due to Tail Sweep) and not in
    Infinite Breath cone (front).
    Ranged DPS: spread far (Distortion Bombs target ranged; spread minimizes
    overlap of safe zones).
    Healer: at range; NOT clustered with ranged DPS; track Temporal Blast
    cycle for predictive heals.
  tank_facing: away from group (mandatory)
  los_or_lookaway: none
  fight_specific_override: |
    DYNAMIC POSITIONING — as Distortion Bomb zones fill arena, all roles
    must continuously reposition. This is one of the few fights where
    "max range" is wrong; the right rule is "most-distant safe spot from
    bombs and other ranged."

interrupt_priority: []                           # no kick-required casts

dispel_priority: []                              # NONE — Temporal Blast is BLACKLISTED
DISPEL_BLACKLIST_explicit:
  - { spell_id: UNVERIFIED, name: Temporal Blast, reason: Hourglass cleanup more efficient than per-stack dispels }

heroic_delta: |
  HEROIC ONLY — no delta.

bot_role_summary:
  tank: face boss away; migrate when bombs converge; pop major CD BEFORE Hourglass click (to maximize CD usage); track Tail Sweep rear-cone (Disagreement #3 conservative)
  healer: pace mana for full fight; DO NOT dispel Temporal Blast (Hourglass-cleared); save Mana Tide / Innervate for between Hourglass clicks; reactive heal Distortion Bomb targets
  melee: side-position; out of Infinite Breath front + Tail Sweep rear; standard rotation
  ranged: spread; PRIMARY HOURGLASS-CLICK CANDIDATE (highest-mobility ranged); standard rotation; CD-pace around clicks

hourglass_strategy:
  bot_default: CONSERVATIVE_REACTIVE
  click_triggers_any_of:
    - bomb_count_on_floor >= 4
    - party_lowest_hp_pct < 30
    - temporal_blast_stack_count_max >= 4
    - any_party_member_dead AND boss_hp_pct > 30
  budget: 5 charges total — aim to use all 5 across fight (don't end fight with unused charges)
  cd_pacing_rule: |
    If click imminent (next ~5s): suppress new offensive CD usage; let click reset
    happen first. After click: free CD re-use window.
```

---

## Instance-wide notes

```yaml
instance_wide:
  curse_dispel_required: false
  disease_dispel_required: false
  poison_dispel_required: false
  magic_dispel_required: partial               # Sylvanas Shriek slow only; Murozond Temporal Blast BLACKLISTED
  dispel_blacklist:
    - { spell_id: UNVERIFIED, name: Temporal Blast, scope: echo_of_murozond_fight, reason: Hourglass cleanup more efficient }
  dispel_priorities:
    - { spell_id: UNVERIFIED, name: Shriek of the Highborne slow, scope: echo_of_sylvanas_fight, priority: HIGH_BEFORE_CALLING }
    - { spell_id: 101810, name: Frostbolt Volley slow, scope: echo_of_jaina_fight, priority: LOW }
  group_buff_anomaly: none
  required_consumables: standard (flask + food + pots per spec)
  heroic_only: true
  patch: 4.3
  troll_heroic_replacement_set: [end_time, hour_of_twilight, well_of_eternity]
  gates_raid: dragon_soul

  bot_implementation_gaps:
    - id: random_echo_pool_detection
      description: bot must reactively detect which 2 of 4 echoes spawned
      strategies: see random_echo_detection block above
      priority: HIGH (encounter profile loading depends on this)

    - id: throw_totem_reactive_handler
      description: |
        Echo of Baine throws totem at random player; bot must detect
        ground-totem gameobject, click-pickup, click-throw-back at boss.
        Not a standard rotation operation.
      priority: MEDIUM (fight is winnable without if DPS is high enough,
        but the totem stun + 50% damage-up debuff is significant DPS aid)

    - id: hourglass_click_handler
      description: |
        Murozond fight requires bot to click an encounter gameobject
        (Hourglass of Time) under reactive conditions, with CD-pacing
        awareness. Fight-specific bot ability.
      priority: HIGH (fight is unwinnable without Hourglass usage)

    - id: tyrande_pre_fight_moonbeam_routing
      description: |
        Tank must drag trash mobs into rotating moonbeams (West → South →
        East → North, 45s rotation) to strip 90% damage-reduction shield.
        Fight-specific tank routing override.
      priority: MEDIUM (delays trash clear; not impossible without)

    - id: jaina_fragment_collection
      description: |
        Pre-Jaina-fight: 16 fragments scattered around shrine; group
        collects while clearing trash. Bot pickup-on-passing logic.
      priority: LOW (master can manually collect; bot blocks engagement
        if fragments incomplete)

    - id: sylvanas_calling_ghoul_focus_coordination
      description: |
        On Calling of the Highborne, all DPS bots must coordinate on the
        SAME ghoul (lowest HP / closest to bot) to break the circle in
        time. Explicit target-coordination state.
      priority: MEDIUM (1 ghoul kill is achievable with uncoordinated bots
        if DPS is high; coordination raises reliability)
```

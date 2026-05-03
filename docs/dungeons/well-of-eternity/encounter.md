# Well of Eternity — Encounter Facts

**Role-neutral mechanic data** for the bot's encounter handling. Per-role response logic lives in sibling files (`healer.md`, `ranged-dps.md`, `melee-dps.md`, `tank.md`). When a role doc references a mechanic, it cites the spell ID defined here.

Spell IDs verified at `wowhead.com/cata/spell=<id>` where consensus could be reached; many are flagged **UNVERIFIED** in this doc and aggregated in `docs/research/dbc-verification-checklist.md`. Strategy reconciled across 5+ sources — see `docs/research/well-of-eternity-guide-survey.md` for the audit trail.

---

## Overview

```yaml
dungeon: well_of_eternity
zone: well_of_eternity_caverns_of_time
continent: kalimdor_caverns_of_time
difficulty_modes: [heroic]                  # HEROIC-ONLY — no normal mode
level_range_normal: NA                       # does not exist
level_range_heroic: 85
min_item_level: 353
boss_count: 3
optional_bosses: []
bosses_in_order:
  - perotharn
  - queen_azshara
  - mannoroth_and_varothen
patch_released: 4.3.0
group_size: 5
notes: |
  Patch 4.3 troll-heroic-replacement set (with End Time + Hour of Twilight).
  Gates Dragon Soul access for badge progression.

  Linear scripted-escort with Illidan Stormrage. Three sections:
    Azshara's Palace approach (intro) → Courtyard of Lights (Peroth'arn) →
    Azshara's Palace (Queen Azshara) → Shores of the Well (Mannoroth & Varo'then).

  All players appear as NIGHT ELVES for the duration (cosmetic — no bot-logic
  impact on hitboxes / spell IDs / faction).

  This dungeon contains FIVE+ unique mechanics that the bot's default behavior
  does NOT handle correctly:
    1. Peroth'arn phase 2 — hide behind pillars to avoid Eye line-of-sight
    2. Azshara magi kill-order encoding + puppet-string interrupt-DPS swap
    3. Azshara Total Obedience guaranteed-interrupt (wipe if missed)
    4. Mannoroth tanked by Illidan NPC ally — bot tank must NOT taunt him
    5. Sword-pickup interaction (object click + targeted use on Mannoroth)
    6. Illidan / Tyrande / Malfurion friendly-NPC tracking (don't damage; track Tyrande HP)

  See bot_implementation_flags block at end of file.
```

---

## Trash — notable mechanics

```yaml
trash:
  # Section 1 — intro / Azshara's Palace approach
  - mob: Doomguard / Dreadlord patrols
    abilities:
      - name: melee
        target: closest_player
    notes: |
      Most patrols are SHADOWCLOAK-SKIP viable. Illidan applies Shadowcloak
      (party stealth) at start of dungeon; bot follows Illidan in stealth
      and avoids breaking stealth by running into mobs.

  - mob: Large Legion Demon (intro fight)
    abilities:
      - name: melee
    notes: |
      Mandatory kill at start of dungeon BEFORE Illidan applies Shadowcloak.
      Tank-and-spank.

  # Section 2 — Courtyard of Lights (pre-Peroth'arn)
  - mob: Satyr / Shivarra / Felguard pack (3 packs)
    abilities:
      - name: melee_cleave
        target: frontal_cone
    notes: |
      Each pack guards a CRYSTAL PILE. Kill pack, then RIGHT-CLICK the
      crystal. After all 3 crystals are activated, the larger demon mob
      blocking Peroth'arn dispels and you can engage him.
      CC viable: Sap, Polymorph, Fear, Hex on humanoid satyrs.
      Banish viable on demonic types.

  - mob: Crystal pile (interactable)
    abilities: []
    notes: |
      Object — right-click after pack is dead. NOT a kill target.
      Bot interaction-handler: detect Illidan's "click crystal" emote;
      use object on the nearest crystal pile.

  # Section 3 — Azshara's Palace (between Peroth'arn and Azshara)
  - mob: Highborne Astromancer
    abilities:
      - name: Frostbolt
        interruptible: true
        priority: MUST_INTERRUPT
        notes: caster hardcast; interrupt to stagger
    notes: humanoid; Polymorph / Hex / Sap valid

  - mob: Highborne Apprentice
    abilities:
      - name: melee
      - name: ranged_cast
        interruptible: true
    notes: hybrid melee/ranged; CC valid

  # Section 4 — Shores of the Well (between Azshara and final boss)
  - mob: Felguard / Felhound / Infernal trash
    abilities:
      - name: melee
      - name: AoE damage on death (Infernal)
    notes: |
      Same demon types appear in larger numbers during phase 2 of the final
      boss; trash here is a soft preview. Banish valid on most.
```

### Trash skip notes

- **Pre-Peroth'arn**: most demon patrols are **Shadowcloak-skip viable** when following Illidan. Bot follows Illidan during this stretch and stays clear of mob aggro radius.
- **Crystal piles** are mandatory interactions, not skips.
- **Highborne packs** between Peroth'arn and Azshara: usually clear, no major skip.
- **Shores trash**: variable; some groups skip to pull-line, others clear.

### CC priorities

- **Polymorph / Hex / Fear / Sap** valid on Highborne and Satyr humanoids
- **Banish** valid on Demonic types (Felguard, Doomguard) — Warlock-only
- **Drakonid / Infernal** types: CC-immune

---

## Boss 1 — Peroth'arn

```yaml
boss: perotharn
npc_id: 55085
location: Courtyard of Lights, after the 3 crystal piles
hp_pct_phase_triggers: [70, 20]              # phase 2 at ~70%, phase 3 (Endless Frenzy) at 20%
notes: |
  Two-phase fight with a unique stealth-detection minigame in phase 2. Phase 1
  is tank-and-spank with Fel Flames + Fel Decay. At ~70% HP, Drain Essence
  channel triggers; ILLIDAN INTERRUPTS it (not the player), and the boss
  vanishes. Players gain Shadowcloak; Eyes of Peroth'arn patrol; players hide
  behind pillars. 40s without detection = Enfeebled boss; phase 3 burn.

phases:
  - id: 1
    name: regular
    trigger: pull
    end_condition: hp_pct == ~70 AND drain_essence_cast_event
    description: |
      Tank-and-spank with Corrupting Touch tank-debuff ramp + Fel Flames
      ground patches + Fel Decay random-target heal-punish.

  - id: transition
    name: drain_essence_to_stealth
    trigger: drain_essence_cast (boss casts; players stunned by AoE; Illidan interrupts; boss vanishes)
    end_condition: shadowcloak_party_buff_applied
    description: |
      Boss vanishes; party gets Shadowcloak (stealth); Eyes of Peroth'arn
      summon and start patrolling.

  - id: 2
    name: stealth_eyes_phase
    trigger: shadowcloak_party_buff_applied
    end_condition: |
      40s elapsed without ANY player detected (success → Enfeebled debuff
      on boss)
      OR
      Eye detects player (Easy Prey; Endless Frenzy on boss; phase 3 starts)
    description: |
      Players hide behind pillars; do NOT engage; Eyes patrol the room.
      If detected: Easy Prey on player (8s stun + Shadowcloak removed);
      boss enters Endless Frenzy (+25% damage); fight transitions to phase 3.
      If not detected: boss becomes Enfeebled (+25% damage taken); phase 3
      starts with the burn favoring the party.

  - id: 3
    name: execute
    trigger: end of phase 2
    end_condition: hp_pct == 0
    description: |
      Boss re-emerges. If party succeeded: Enfeebled (+25% boss takes more
      damage). If party failed: Endless Frenzy (+25% boss does more damage).
      Endless Frenzy ALSO triggers naturally at 20% HP regardless. Burn fast.

mechanics:
  - name: Corrupting Touch
    spell_id: 104939
    cast_by: perotharn
    cast_kind: melee_application
    target: tank
    effect: stacking debuff +10% damage taken per stack
    duration_ms: 20000
    interruptible: false
    dispel_type: none                         # do NOT dispel; meant to ramp
    avoidable: false
    priority: TANK_HEAL_RAMP

  - name: Fel Flames
    spell_id: 108141
    cast_by: perotharn
    cast_kind: cast
    target: ally_random
    effect: |
      ~33-37k initial Fire damage on hit + ground patch ~12-13k Fire
      damage/sec for 30s
    interruptible: false
    dispel_type: none
    avoidable: true
    avoidance: move out of patch ASAP after impact
    priority: MOVE_OUT

  - name: Fel Decay
    spell_id: 105544
    cast_by: perotharn
    cast_kind: cast
    target: ally_random
    effect: |
      ~38-42k initial Shadow damage + ~8-9k/sec for 10s.
      HEALING the affected target deals damage to the caster (healer).
      Healer chooses: top off and take damage, or wait out HoT.
    interruptible: false
    dispel_type: shadow                       # technically dispellable, but bot DOES NOT auto-dispel
    DISPEL_BLACKLIST: true                    # do NOT auto-dispel — heal-or-not-heal IS the mechanic
    avoidable: false
    priority: HEAL_DECISION
    notes: bot healer logic: heal only if target.hp_pct < 50; let small ticks pass

  - name: Drain Essence
    spell_id: 104905
    cast_by: perotharn
    cast_kind: channel
    target: pbaoe_self
    cast_time_ms: ~scripted at ~70% HP
    effect: stuns all enemies + Shadow tick damage every second for 4s
    interruptible: false                       # Illidan interrupts, NOT players
    dispel_type: none
    avoidable: false
    priority: PHASE_2_TRIGGER_SIGNAL
    notes: |
      Bot must NOT waste kicks on Drain Essence. Illidan handles it.
      Use the cast event as the phase 2 trigger signal.

  - name: Easy Prey
    spell_id: UNVERIFIED
    cast_by: perotharn (triggered by Eye detection)
    cast_kind: scripted
    target: detected_player
    effect: removes Shadowcloak; STUNS 8s; marks player as boss target
    interruptible: false
    dispel_type: none                          # not dispellable
    avoidable: true (avoid via pillar-hiding)
    priority: PHASE_2_FAILURE_SIGNAL

  - name: Fel Quickening
    spell_id: UNVERIFIED
    cast_by: perotharn
    cast_kind: scripted (on Eye detection)
    effect: empowers boss when an Eye detects a player; combo with Endless Frenzy
    notes: alternate name for the phase 2 detection trigger; UNVERIFIED if separate from Easy Prey

  - name: Endless Frenzy
    spell_id: 105521
    cast_by: perotharn
    cast_kind: aura_self
    target: self
    effect: +25% damage done; execute mode
    triggers:
      - on_eye_detection (phase 2 failure)
      - hp_pct_le 20 (natural execute)
    interruptible: false
    priority: HEALER_PRESSURE_RAMP

  - name: Enfeebled
    spell_id: UNVERIFIED
    cast_by: perotharn
    cast_kind: aura_self (debuff)
    target: self
    effect: +25% damage TAKEN by boss (favors party); awarded if 40s phase 2 success
    triggers:
      - phase_2_success (40s, no detection)
    interruptible: false
    priority: PHASE_3_BUFF_FAVORABLE

  - name: Shadowcloak (player buff from Illidan)
    spell_id: UNVERIFIED
    cast_by: illidan_stormrage_npc
    cast_kind: party_buff
    target: party
    effect: stealth on all party members; broken by Easy Prey
    notes: |
      Same buff used during trash-skip in intro section. Phase 2 detection
      hides bot from Eyes IF no Eye has line-of-sight.

# Adds
adds:
  - name: Eye of Peroth'arn
    npc_id: UNVERIFIED
    count: ~3-5 (UNVERIFIED exact)
    spawn: phase 2 only
    abilities:
      - name: line_of_sight_detection
        target: any_player_in_LoS_within_range
        notes: |
          Mobile mob that patrols the room; if it sees a player, triggers
          Easy Prey + Endless Frenzy.
    DESPAWN_ON_PHASE_END: true                # despawns at end of stealth phase per hotfix
    KILL_PRIORITY: NEVER                       # bot DPS logic must blacklist Eyes
    notes: |
      Bot must NOT attack Eyes (they're scripted). Bot phase 2 logic must
      track Eye positions to plan pillar-hide pathfinding.

positioning:
  default_phase_1: |
    Tank pulls boss to one side of Courtyard chamber. Melee on boss.
    Ranged spread to minimize Fel Flames cluster risk.
  default_phase_2: |
    ALL PLAYERS HIDE BEHIND PILLARS. The room has multiple stone pillars
    that block Eye line-of-sight. Pathfind to a pillar that's not in any
    Eye's current LoS. Stay still for 40s OR until phase 3 transition.
  default_phase_3: re-engage at phase-1 position; burn execute
  tank_facing: away from group (standard); irrelevant in phase 2
  los_or_lookaway: PILLARS in phase 2 (terrain-based LoS)

interrupt_priority: []                        # Drain Essence is Illidan's job; no player kicks needed

dispel_priority:
  DISPEL_BLACKLIST:
    - { spell_id: 105544, name: Fel Decay, reason: heal-vs-decay decision IS the mechanic }
  notes: |
    Bot must NOT auto-cleanse Fel Decay (Shadow dispel). The heal-or-not-heal
    decision IS the encounter design.

heroic_delta: |
  Heroic-only encounter; numbers above ARE heroic. No normal-mode delta.

bot_role_summary:
  tank: |
    Phase 1: tank Peroth'arn; manage Corrupting Touch stacks (defensive CDs at ~5+ stacks).
    Phase 2: HIDE — there is no boss to tank. Pathfind to pillar; stay still.
    Phase 3: re-engage; burn.
  healer: |
    Phase 1: HoT-roll tank for Corrupting Touch ramp; reactive heal Fel Flames hits.
    Fel Decay: heal only if target.hp_pct < 50 (heal damages boss back at caster).
    Phase 2: HIDE behind pillar; do NOT cast (breaks stealth); pre-HoT before phase ends.
    Phase 3: standard healer; pre-CD if Endless Frenzy (failure path).
  melee: |
    Phase 1: standard rotation; move out of Fel Flames patches.
    Phase 2: HIDE behind pillar; no melee target available.
    Phase 3: execute rotation.
  ranged: |
    Phase 1: standard rotation; spread; move out of Fel Flames patches.
    Phase 2: HIDE behind pillar; no ranged DPS allowed (would break stealth).
    Phase 3: execute rotation.

bot_impl_flags:
  - name: phase_2_hide_from_eyes
    severity: MAJOR
    description: |
      Bot's default "stand near master / stand still in combat" behavior triggers
      Eye detection. Bot needs explicit phase_2 hide-state that:
      1. Identifies Eye of Peroth'arn mobs (NPC ID lookup)
      2. Identifies pillar terrain features (heuristic — solid mesh pillars)
      3. Pathfinds to pillar OUT of Eye line-of-sight
      4. Stays still 40s OR until phase 3 detected
      5. Healer / DPS bots do NOT cast (cast bar visible to Eyes? UNVERIFIED;
         conservative default = cast nothing)
  - name: drain_essence_npc_interrupt
    severity: MINOR
    description: |
      Bot must NOT waste kicks on Drain Essence. Illidan handles it.
      Use the cast event as phase-2 trigger.
```

---

## Boss 2 — Queen Azshara

```yaml
boss: queen_azshara
npc_id: 54853
location: Azshara's Palace, second encounter
hp_pct_phase_triggers: []                     # untargetable; no HP phase
notes: |
  Azshara is UNTARGETABLE (Shroud of Luminosity). Bot DPS logic must NOT
  target her. Encounter resolves by killing her 6 Enchanted Magi.

  Magi are 3 schools (Frost, Fire, Arcane), 2 of each. They activate in PAIRS.
  As each magus dies, a new one activates. When all 6 are dead, Azshara
  retreats and encounter ends.

  Periodic mechanics from Azshara:
    - Total Obedience (8s cast) — MUST INTERRUPT or wipe
    - Hand of the Queen / Servant of the Queen — random MC; kill puppet strings to free
    - Shroud of Luminosity — perma-invuln on Azshara herself

phases:
  - id: 1
    name: magi_pairs
    trigger: pull
    end_condition: all 6 magi dead
    description: |
      Continuous pair-rotation. Tank picks up newly-activated magus on each
      kill. Players DPS active magus; switch to puppet strings on MC event;
      interrupt Total Obedience whenever it casts.

mechanics_azshara:
  - name: Shroud of Luminosity
    spell_id: UNVERIFIED
    cast_by: queen_azshara
    cast_kind: passive_aura_self
    target: self
    effect: PERMANENT INVULNERABILITY — damage does nothing
    DPS_BLACKLIST: true                        # bot DPS logic must NOT target Azshara
    notes: |
      Bot must explicitly blacklist Azshara as a DPS target. If a bot
      auto-targets the boss frame (common default), it will waste rotation
      doing nothing. Force-target the active magus instead.

  - name: Total Obedience
    spell_id: 103241
    cast_by: queen_azshara
    cast_kind: cast
    target: pbaoe_party
    cast_time_ms: 8000
    cooldown_ms: ~scripted (UNVERIFIED periodic cadence)
    effect: charms ENTIRE party for 20s = WIPE
    interruptible: true
    priority: HIGHEST_INTERRUPT_OF_DUNGEON
    notes: |
      Most important interrupt in WoE. Bot interrupt rotation must reserve
      at least one kick for guaranteed coverage. 8s cast leaves room for
      multiple attempts but missing all = wipe.

  - name: Hand of the Queen / Servant of the Queen
    spell_id: 102334
    cast_by: queen_azshara
    cast_kind: scripted
    target: ally_random_non_tank
    effect: charms target player ("dance, puppets!") for 20s OR until puppet strings die
    interruptible: false
    dispel_type: none                          # NOT dispel-resolved
    resolution: kill_puppet_strings
    priority: SWITCH_TARGET_TO_STRINGS
    notes: |
      MC'd player can't be controlled / healed effectively. DPS switches
      to puppet strings (NPCs spawned above MC'd player); kill them to free.

mechanics_magi:
  - magus_school: arcane
    kill_priority: 1                           # FIRST per Disagreement #4 default
    abilities:
      - name: Arcane Bomb
        spell_id: UNVERIFIED
        cast_kind: ground_target
        effect: ~70-80k AoE + movement slow
        avoidable: true
        priority: MOVE_OUT
      - name: Arcane Shock
        spell_id: UNVERIFIED
        cast_kind: continuous AoE
        effect: continuous damage on nearby
        avoidable: partial (range)
    notes: kill first; bombs deal highest single-event damage in fight

  - magus_school: frost
    kill_priority: 2
    abilities:
      - name: Ice Fling
        spell_id: UNVERIFIED
        cast_kind: cast
        target: ally_random
        effect: ~25k Frost damage
        interruptible: UNVERIFIED
      - name: Coldflame
        spell_id: UNVERIFIED
        cast_kind: line
        effect: ~45k Frost damage line
        avoidable: true
        avoidance: sidestep
      - name: Blades of Ice / Ice Sheets
        spell_id: UNVERIFIED
        cast_kind: charge
        target: ally_random
        effect: charge attack; multi-hit if clustered
        avoidable: partial (spread)

  - magus_school: fire
    kill_priority: 3
    abilities:
      - name: Fireball
        spell_id: UNVERIFIED
        cast_kind: cast
        target: ally_random
        cast_time_ms: ~2500
        effect: ~25k Fire damage
        interruptible: true
        priority: MUST_INTERRUPT
      - name: Firebomb
        spell_id: UNVERIFIED
        cast_kind: ground_target
        effect: ~35k area damage + slow
        avoidable: true
      - name: Blast Wave
        spell_id: UNVERIFIED
        cast_kind: pbaoe_self
        effect: ~30k AoE around magus
        avoidable: true
        avoidance: range out

# Adds — magi (pre-spawned, activate in pairs)
adds:
  - name: Enchanted Magus (Arcane / Frost / Fire)
    count_total: 6 (2 per school)
    activation_pattern: 2 active at a time; new magus activates on kill
    kill_priority_order: [Arcane, Frost, Fire]   # Disagreement #4 default
    notes: |
      Magi are pre-spawned around the chamber but inactive. Tank acquires
      newly-active magus as each dies.

  - name: Puppet strings (NPC spawned above MC'd player)
    count_per_event: ~2 (UNVERIFIED exact)
    spawn: on Hand of the Queen MC event
    KILL_PRIORITY: ABSOLUTE                    # interrupt magus DPS to free MC'd player
    despawn: when MC'd player is freed

positioning:
  default: |
    Tank pulls active magus to one SIDE of palace, away from group, away
    from puppet-string spawn area. Melee on active magus. Ranged max range,
    spread for AoE coverage.
  tank_facing: away from group (standard)
  los_or_lookaway: none
  spread_distance: ~8y between ranged players

interrupt_priority:
  - Total Obedience (Azshara)                  # HIGHEST — wipe if missed
  - Fireball (Fire Magus)                      # high — healer pressure mitigation
  - other magus casts                          # opportunistic

dispel_priority: []                            # no dispels; MC resolved by killing strings

heroic_delta: |
  Heroic-only encounter; numbers above ARE heroic.

bot_role_summary:
  tank: |
    Pick up newly-activated magus on each kill. Stay out of ground patches.
    Position magus AWAY from group so AoE doesn't cleave.
  healer: |
    Long fight (~3 min) — mana pacing matters. Watch Total Obedience cast
    bar relentlessly; help interrupt if class has kick. Reactive heal during
    AoE pulses; HoT-rolled tank.
  melee: |
    DPS active magus; SWITCH to puppet strings on MC event. Interrupt Total
    Obedience. Spread for Coldflame line / Charge.
  ranged: |
    DPS active magus; SWITCH to puppet strings on MC event. Interrupt Total
    Obedience and Fireball. Max range; sidestep Coldflame.

bot_impl_flags:
  - name: kill_order_encoding
    severity: MAJOR
    description: |
      Bot DPS target-selection logic must encode magi kill order:
      1. Active Arcane Magus (if alive)
      2. Active Frost Magus (if alive)
      3. Active Fire Magus (if alive)
      4. Default (tank's target) otherwise
      Plus interrupt: puppet strings (highest target priority on MC event).

  - name: total_obedience_guaranteed_interrupt
    severity: MAJOR
    description: |
      Total Obedience missing = wipe. Bot interrupt rotation must guarantee
      at least one kick available for it. Reserve Wind Shear / Counterspell /
      Pummel / Kick / Mind Freeze / Skull Bash / Rebuke.

  - name: shroud_of_luminosity_dps_blacklist
    severity: MAJOR
    description: |
      Bot must NOT target Azshara. Default "boss frame" autotarget will waste
      DPS rotation. Force-target the active magus.

  - name: puppet_strings_dps_priority
    severity: MAJOR
    description: |
      On MC event, bot DPS must switch from active magus to puppet strings.
      String kill is time-sensitive (free MC'd player ASAP). Resume magus
      DPS after free.
```

---

## Boss 3 — Mannoroth and Varo'then (dual boss)

```yaml
boss: mannoroth_and_varothen
npc_id_mannoroth: 54969
npc_id_varothen: UNVERIFIED
location: Shores of the Well, final encounter
hp_pct_phase_triggers: [varothen_dies, mannoroth_75, 60, 45, 30, 5]
notes: |
  DUAL BOSS but with NPC-ally tank: Illidan tanks Mannoroth, players DPS
  Varo'then in phase 1. After Varo'then dies, sword pickup mechanic
  embeds the Magistrike Blade in Mannoroth (phase 2). Demon waves spawn
  at 75/60/45/30% Mannoroth HP. Tyrande gets stunned by Dreadlord
  Debilitators; players rescue her + stand in Moonbeam (Blessing of Elune).
  At ~5%, Malfurion closes the portal, Illidan grants Gift of Sargeras
  (heal+AoE), party finishes Mannoroth.

  TANK NOTE: in 5-man, the bot tank's "boss" target is Varo'then phase 1
  and demon waves phase 2. Mannoroth is on the encounter target list but
  is TANKED BY ILLIDAN, not by the player tank. Bot tank logic must NOT
  taunt Mannoroth.

phases:
  - id: 1
    name: varothen_dps
    trigger: pull
    end_condition: varothen.hp_pct == 0
    description: |
      Players DPS Varo'then. Illidan tanks Mannoroth (do NOT pull). Tyrande
      kills lesser demons at portal. Avoid Fel Firestorm patches everywhere.
      Spread for Magistrike arc on Varo'then.

      SOFT ENRAGE: if players ignore Varo'then too long, Mannoroth uses Fel
      Drain to sacrifice Varo'then and FULLY HEAL. Bot must keep Varo'then
      DPS active.

  - id: 2
    name: sword_embedded
    trigger: varothen.hp_pct == 0 AND sword_pickup_event
    end_condition: mannoroth.hp_pct == ~5 OR portal_close_event
    description: |
      Player picks up Magistrike Blade (object) and clicks on Mannoroth to
      embed. Mannoroth takes massive damage from Magistrike Arcs proc.
      Demon waves spawn at 75/60/45/30% Mannoroth HP:
        75%: Felhounds
        60%: Felguards
        45%: Doomguards
        30%: Infernals
      Stack near Illidan for Hand of Flame mitigation.

      Mid-phase: 2 Dreadlord Debilitators stun Tyrande. Players kill them;
      Tyrande's Moonbeam appears (Blessing of Elune ground area); stack on
      moonbeam for damage-vs-demons buff.

  - id: 3
    name: gift_of_sargeras_execute
    trigger: mannoroth.hp_pct == ~5 AND malfurion_portal_close
    end_condition: mannoroth.hp_pct == 0
    description: |
      Malfurion closes portal — demon spawns stop. Illidan grants Gift of
      Sargeras: 20% HP regen / 2s + 30k AoE fire / 2s near party. Stack
      tight for max benefit. Burn Mannoroth fast.

mechanics_varothen:
  - name: Magistrike (sword charge)
    spell_id: UNVERIFIED
    cast_by: varothen
    cast_kind: melee_charge
    target: tank + arc_to_2_more
    effect: |
      ~28-32k Fire damage to target + arcs to up to 2 nearby (3 total).
      Spread 8-10y to mitigate arc.
    interruptible: UNVERIFIED                  # try if class has kick
    avoidable: partial (spread)
    priority: SPREAD_REQUIRED

  - name: Charge
    spell_id: UNVERIFIED
    cast_by: varothen
    cast_kind: scripted
    target: ally_random
    effect: charges player; brief stun on impact
    avoidable: partial (spread)

  - name: Throw Glaive
    spell_id: UNVERIFIED
    cast_by: varothen
    cast_kind: ranged
    target: ally_random
    effect: ranged glaive (low priority secondary)

mechanics_mannoroth:
  - name: Fel Firestorm
    spell_id: UNVERIFIED
    cast_by: mannoroth
    cast_kind: scripted_periodic
    target: ground_random_pattern
    effect: |
      Rains 3-yard Fel Flames patches across the area for 12s. Each patch
      ~19-20k Fire damage/sec. Continuous; entire fight has Fel Firestorm
      hazards on the floor.
    interruptible: false
    dispel_type: none
    avoidable: true
    avoidance: constantly relocate
    priority: MOVEMENT_PRIMARY

  - name: Fel Drain
    spell_id: UNVERIFIED
    cast_by: mannoroth
    cast_kind: scripted_threshold
    target: varothen
    effect: |
      Sacrifices Varo'then; Mannoroth FULLY HEALS. Fires if Varo'then is
      ignored / DPS too low.
    interruptible: false                       # not player-interrupted
    priority: SOFT_ENRAGE_AVOID

  - name: Hand of Flame
    spell_id: UNVERIFIED
    cast_by: mannoroth
    cast_kind: passive_aura
    target: pbaoe_self
    effect: fire DoT on nearby; mitigated by standing near Illidan
    interruptible: false
    avoidable: partial (range out, or near Illidan for mitigation)

  - name: Magistrike Arcs (sword-embedded)
    spell_id: UNVERIFIED
    cast_by: mannoroth (auto-triggered after sword embed)
    cast_kind: passive_proc
    target: self
    effect: |
      Mannoroth's hits proc Magistrike Arcs which strike Mannoroth himself
      for massive damage (~2,000,000 per proc per some sources). Primary
      phase 2 damage source.
    priority: PASSIVE_DAMAGE_SOURCE

  - name: Shadow Bolt Volley                    # UNVERIFIED — only some sources list
    spell_id: UNVERIFIED
    cast_by: mannoroth
    cast_kind: cast
    target: pbaoe_party
    effect: shadow damage
    interruptible: UNVERIFIED
    priority: OPPORTUNISTIC_INTERRUPT

mechanics_demons_phase_2:
  - wave: 1
    trigger: mannoroth.hp_pct == 75
    demon: Felhound
    abilities:
      - melee
    behavior: tank pickup; AoE-burn

  - wave: 2
    trigger: mannoroth.hp_pct == 60
    demon: Felguard
    abilities:
      - melee_cleave
      - whirlwind (some sources)
    behavior: tank picks up; spread for cleave

  - wave: 3
    trigger: mannoroth.hp_pct == 45
    demon: Doomguard
    abilities:
      - shadow_bolt_cast
        interruptible: true
    behavior: ranged caster; interrupt + burn

  - wave: 4
    trigger: mannoroth.hp_pct == 30
    demon: Infernal
    abilities:
      - aoe_damage_aura
    behavior: high-AoE damage; Tyrande overwhelmed at this stage; rescue priority

  - rescue_event: dreadlord_debilitator
    npc_id: UNVERIFIED
    count: 2
    trigger: scripted (during phase 2, mid-demon-waves)
    abilities:
      - debilitate (stuns Tyrande)
    rescue: kill the 2 Dreadlord Debilitators; Tyrande recovers
    follow_up: Tyrande's Moonbeam (Blessing of Elune) ground area appears
    KILL_PRIORITY: ABSOLUTE during the rescue event

mechanics_ally_npcs:
  - name: Illidan Stormrage
    npc_id: UNVERIFIED                         # ~55532 per some sources, multiple Illidan IDs in DB
    role: |
      Phase 1: tanks Mannoroth (do NOT taunt off him)
      Phase 2: provides Hand of Flame mitigation aura when stacked nearby
      Phase 3: grants Gift of Sargeras buff
    abilities_player_relevant:
      - Shadowcloak (party stealth) — pre-fight escort + Peroth'arn phase 2
      - Tank Mannoroth (passive)
      - Gift of Sargeras (phase 3 buff)
    DAMAGE_BLACKLIST: true                     # bot must NOT target Illidan with damage

  - name: Tyrande Whisperwind
    npc_id: UNVERIFIED
    role: |
      Phase 1-2: kills lesser demons at portal
      Phase 2 mid: stunned by Dreadlord Debilitators; players rescue
      Phase 2 post-rescue: drops Moonbeam (Blessing of Elune) ground area
    abilities_player_relevant:
      - Blessing of Elune (Moonbeam) — stack on it for damage-vs-demons buff
    HP_TRACKING: required                      # bot detects stun/incapacitation via low HP
    DAMAGE_BLACKLIST: true

  - name: Malfurion Stormrage
    npc_id: UNVERIFIED
    role: |
      Phase 3 (~5% Mannoroth): closes portal; demon spawns stop
    abilities_player_relevant:
      - Portal close (scripted; phase 3 trigger signal)
    DAMAGE_BLACKLIST: true

mechanics_phase_3:
  - name: Gift of Sargeras (player buff from Illidan)
    spell_id: UNVERIFIED
    cast_by: illidan_stormrage
    cast_kind: party_buff
    target: party
    effect: |
      20% max HP regen every 2s + 30k AoE Fire damage every 2s near each player.
      Heal-aura + AoE damage execute mode.
    duration: until Mannoroth dies
    priority: STACK_TIGHT_FOR_MAX_BENEFIT

mechanics_sword_pickup:
  - name: Magistrike Blade (object pickup)
    object_id: UNVERIFIED
    spawn: on Varo'then's death
    interaction:
      step_1: walk to sword location
      step_2: right-click / use object (pickup)
      step_3: target Mannoroth
      step_4: activate the sword-throw player buff (use new ability bar item)
    effect: embeds blade; activates Magistrike Arcs proc damage on Mannoroth
    priority: HIGH                             # phase 2 trigger event
    notes: |
      Object interaction, NOT a spell rotation. Bot needs explicit handler
      for this. Default "DPS the boss" logic does NOT cover it. Designate
      ONE bot as sword-handler (typically a ranged DPS for travel speed).

# Adds summary
adds:
  - name: Felhound (wave 1 @ 75% Mannoroth HP)
  - name: Felguard (wave 2 @ 60%)
  - name: Doomguard (wave 3 @ 45%)
  - name: Infernal (wave 4 @ 30%)
  - name: Dreadlord Debilitator (mid-phase rescue event, x2)

positioning:
  default_phase_1: |
    Tank pulls Varo'then to one side. Spread 8-10y from each other for
    Magistrike arc mitigation. Constantly relocate out of Fel Firestorm
    patches.
  default_phase_2: |
    Tank picks up demon waves. Stack near Illidan for Hand of Flame mitigation.
    On Tyrande Moonbeam: stack on the Moonbeam ground area.
  default_phase_3: stack tight near party for Gift of Sargeras heal+AoE pulse
  tank_facing: away from group (standard)
  los_or_lookaway: none

interrupt_priority:
  - Doomguard shadow_bolt (phase 2)            # interrupt on rotation
  - Varo'then Magistrike (phase 1)             # UNVERIFIED interruptibility
  - Mannoroth Shadow Bolt Volley               # UNVERIFIED existence/interruptibility

dispel_priority: []                            # no dispel-resolved mechanics

heroic_delta: |
  Heroic-only encounter; numbers above ARE heroic.

bot_role_summary:
  tank: |
    Phase 1: tank Varo'then; do NOT taunt Mannoroth (Illidan).
    Phase 2: tank demon waves (Felhounds → Felguards → Doomguards → Infernals).
              Defensive CDs on cleave waves. Help kill Dreadlord Debilitators.
    Phase 3: stack near party; auto-attack Mannoroth.
  healer: |
    Long fight; mana pacing critical. Phase 2 mana CDs on demon-wave damage.
    Tyrande Moonbeam: stack on it (heals + buffs). Phase 3: Gift of Sargeras
    auto-heals — healer pressure DROPS; DPS-heal mode.
  melee: |
    Phase 1: DPS Varo'then; spread; movement-out of Fel Firestorm.
    Phase 2: AoE demon waves; on Magistrike DPS (sword embedded — Mannoroth
    takes massive damage); rescue Tyrande.
    Phase 3: burn Mannoroth.
  ranged: |
    Phase 1: DPS Varo'then; max range.
    Phase 2: SWORD-PICKUP role (one ranged designated); demon-wave AoE; kill
    Dreadlord Debilitators; stack on Moonbeam.
    Phase 3: burn Mannoroth.

bot_impl_flags:
  - name: mannoroth_npc_tanked_blacklist
    severity: MAJOR
    description: |
      Bot tank logic must NOT taunt or pull Mannoroth in phase 1. Illidan
      tanks him. Bot tank's phase-1 target is Varo'then. Encounter target
      list must distinguish "boss tanked by ally NPC" from "boss tanked by
      player tank." Bot DPS may DPS Mannoroth in phase 2-3 (sword embedded);
      tank is NOT primary aggro.

  - name: sword_pickup_handler
    severity: MAJOR
    description: |
      On Varo'then death, sword spawns as object. ONE bot must:
        1. Detect Varo'then death event
        2. Path to sword location (object position)
        3. Right-click / use object (pickup)
        4. Acquire Mannoroth as target
        5. Activate the new buff (sword throw; UI prompts on player action bar)
      Designation: typically a ranged DPS for travel speed; healer / tank
      backup. Bot needs explicit `WoEternity_Phase2_PickUpSword()` handler.

  - name: friendly_npc_tracking
    severity: MAJOR
    description: |
      Bot must track Illidan (tank Mannoroth), Tyrande (HP/stun), Malfurion
      (phase 3 trigger). Don't damage them; track Tyrande HP for Dreadlord
      Debilitator rescue trigger; track Illidan Gift of Sargeras buff for
      phase 3 entry.

  - name: tyrande_moonbeam_stack
    severity: MEDIUM
    description: |
      When Tyrande's Moonbeam ground area appears, bot stack-on-area logic
      must override default spread positioning. AoE buff + heal source.

  - name: demon_wave_target_priority
    severity: MEDIUM
    description: |
      Phase 2 demon waves staggered by Mannoroth HP. Bot DPS target priority:
        Dreadlord Debilitator (rescue event) > Doomguard (interrupt) >
        Infernal > Felguard > Felhound > Mannoroth.
```

---

## Instance-wide notes

```yaml
instance_wide:
  difficulty_modes: [heroic_only]
  level_gate: 85_min_ilvl_353
  curse_dispel_required: false
  disease_dispel_required: false
  poison_dispel_required: false
  magic_dispel_required: false
  dispel_blacklist:
    - { spell_id: 105544, name: Fel Decay (Peroth'arn), reason: heal-vs-decay decision IS the mechanic }
  group_buff_anomaly: |
    NIGHT ELF transformation aura on entire party for instance duration.
    Cosmetic only — does not affect bot logic / hitboxes / spell IDs.
    SHADOWCLOAK from Illidan during escort + Peroth'arn phase 2.
  required_consumables: standard (flask + food + pots per spec)

  ally_npcs:
    - illidan_stormrage      # tanks Mannoroth, escort stealth provider
    - tyrande_whisperwind    # demon-wave handler, Moonbeam buff source
    - malfurion_stormrage    # phase 3 portal-close trigger
  ally_npc_damage_blacklist: true              # bot must NOT damage these

  unique_mechanics_summary:
    - perotharn_phase_2_hide                   # LoS-aware pillar pathfinding
    - azshara_total_obedience_must_interrupt   # wipe if missed
    - azshara_shroud_of_luminosity_dps_blacklist
    - azshara_magi_kill_order                  # Arcane → Frost → Fire
    - azshara_puppet_strings_dps_switch        # MC event interrupt
    - mannoroth_tanked_by_illidan              # bot tank ignores Mannoroth phase 1
    - mannoroth_sword_pickup_handler           # object interaction
    - mannoroth_friendly_npc_tracking
    - mannoroth_tyrande_moonbeam_stack

bot_implementation_flags:
  major:
    - perotharn_phase_2_hide_from_eyes
    - azshara_kill_order_encoding
    - azshara_total_obedience_guaranteed_interrupt
    - azshara_shroud_of_luminosity_dps_blacklist
    - azshara_puppet_strings_dps_priority
    - mannoroth_npc_tanked_blacklist
    - mannoroth_sword_pickup_handler
    - mannoroth_friendly_npc_tracking
  medium:
    - mannoroth_tyrande_moonbeam_stack
    - mannoroth_demon_wave_target_priority
  minor:
    - perotharn_drain_essence_npc_interrupt
    - illidan_escort_shadowcloak_follow
```

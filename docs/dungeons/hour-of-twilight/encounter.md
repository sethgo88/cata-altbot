# Hour of Twilight — Encounter Facts

**Role-neutral mechanic data** for the bot's encounter handling. Per-role response logic lives in sibling files (`healer.md`, `ranged-dps.md`, `melee-dps.md`, `tank.md`). When a role doc references a mechanic, it cites the spell ID defined here.

Spell IDs verified at `wowhead.com/cata/spell=<id>` where consensus could be reached; UNVERIFIED items aggregated in `docs/research/dbc-verification-checklist.md`. Strategy reconciled across 7+ sources — see `docs/research/hour-of-twilight-guide-survey.md` for the audit trail.

---

## Overview

```yaml
dungeon: hour_of_twilight
zone: hour_of_twilight                       # Caverns of Time, Tanaris (instance instance segments visit Dragonblight → Wyrmrest)
continent: kalimdor_caverns_of_time
difficulty_modes: [heroic]                   # HEROIC ONLY — no normal-mode entry
level_range_normal: null                     # N/A
level_range_heroic: 85
ilvl_minimum: 353
boss_count: 3
optional_bosses: []                          # no optionals; entire instance is on the spine
bosses_in_order:
  - arcurion
  - asira_dawnslayer
  - archbishop_benedictus
patch: 4.3.0
gating: requires_well_of_eternity_completed_first
notes: |
  Linear path: Crystal Vice (Arcurion) → Galakrond's Rest (Asira) → Path of
  the Titans / Maw of Destruction → Wyrmrest Temple roof / Chamber of
  Aspects (Benedictus). Heroic-only; no normal-mode rotation.

  THRALL ESCORT — Thrall is an instance-wide NPC companion. He walks/runs
  with the party from start to finish, fights all 3 bosses with scripted
  support abilities, and IF HE DIES THE INSTANCE FAILS (must reset).
  Hotfixes after launch made Thrall immune to most trash damage but the
  death-fail rule still applies on bosses (Arcurion Icy Tomb).

  Dispel pressure is concentrated on Benedictus (Righteous Shear / Twilight
  Shear, Magic-school, every cycle). Other bosses have no dispel priority.

  No tank-swap on any boss. Single-tank fights throughout.
```

---

## Instance-wide notes

```yaml
instance_wide:
  difficulty_mode: heroic_only
  ilvl_gate: 353

  thrall_escort:
    enabled: true
    death_fail_rule: |
      If Thrall dies, the instance fails and must be reset. Bot logic must
      monitor Thrall's HP as a soft "raid-wide HP-priority" target — not as
      a healing target (he's not a party member, not directly healable in
      most strats), but as a fail-condition flag.
    follow_behavior: |
      Thrall walks/runs with the party between encounters at scripted speed.
      He has scripted pause points (monologue moments) where the party must
      wait. This is a MAJOR BOT-IMPLEMENTATION FLAG.
    bot_implementation_flag: |
      AltbotFollow.cpp does NOT have NPC-follow logic. Default bot follow-
      tank behavior incidentally tracks Thrall most of the time, but:
        1. If tank ranges out ahead of Thrall, bots will leave Thrall behind
        2. At Thrall's pause points, bots may continue auto-pathing forward
           past Thrall, breaking the escort sequence
        3. Bot has no way to defend Thrall specifically (taunt-onto-Thrall-
           threat is not implemented)
      WORKAROUND until AltbotFollow.cpp gains FollowNPCTarget(guid) mode:
      blacklist Hour of Twilight from bot LFD queue, OR rely on tank-bot to
      pace itself with Thrall's walk speed.

  thrall_combat_support:
    arcurion: |
      - Bloodlust (likely spell 2825) cast at boss 30% HP (Torrent of Frost
        execute window) — bot rotation/CD alignment trigger
    asira: |
      - Rising Fire Totem (UNVERIFIED ID) — periodic damage + health buff;
        bot stands in totem range during DPS check (Blade Barrier execute)
    benedictus_phase_1: |
      - Water Shell (UNVERIFIED ID) — protective bubble at scripted points
        timed to Wave of Virtue casts; standing in Water Shell grants damage
        immunity AND +100% damage buff
      - Chain Lightning — destroys 2 of 3 Purifying Light orbs; party only
        dodges 1
    benedictus_phase_2: |
      - NONE. At 60% HP transition, Benedictus traps Thrall in Twilight
        Prison. Thrall is REMOVED from combat for the rest of the fight.
        Bot logic must detect this transition (no more Water Shell, all 3
        orbs must be dodged, no Chain Lightning support).

  curse_dispel_required: false
  disease_dispel_required: false
  poison_dispel_required: false
  magic_dispel_required: true                  # ONLY on Benedictus (Shears)

  dispel_blacklist: []                         # no dispel-blacklist entries

  group_buff_anomaly: |
    Thrall's Rising Fire Totem (Asira fight) and Water Shell (Benedictus
    P1) are scripted external buffs that bots should detect and stand in.
    These are NOT player-cast buffs; they originate from a hostile-faction-
    flagged-as-friendly NPC.

  required_consumables: standard (flask + food + pots per spec)

  bot_critical_flags:
    - thrall_escort_npc_follow                # not implemented in AltbotFollow.cpp
    - asira_smoke_bomb_target_loss            # bot must wait through cloud, not chase
    - benedictus_phase_2_transform            # bot must detect Twilight Epiphany and re-acquire
    - benedictus_dispel_every_shear           # highest dispel-priority work in any Cata 5-man so far
```

---

## Trash — notable mechanics

Hour of Twilight has comparatively sparse trash. Only the trash entries with mechanics worth automating are enumerated; standard packs (Twilight grunts, low-HP clear mobs) are pure threat-and-DPS.

```yaml
trash:
  # Pre-Arcurion (Crystal Vice)
  - mob: Crystalline Elemental
    abilities:
      - name: melee
        target: closest_player
    notes: small frost elementals; AoE-burn; tank-and-spank

  - mob: Frozen Servitor (trash variant — separate from Arcurion's environmental Servitors)
    abilities:
      - name: Frost Bolt
        interruptible: true
        priority: MUST_INTERRUPT
        notes: hardcast caster — interrupt rotation
    notes: humanoid-elemental hybrid; standard caster trash

  # Pre-Asira (Galakrond's Rest)
  - mob: Twilight Assassin
    abilities:
      - name: stealth_engage
        target: random_player
        notes: appear from invisibility; tank may need to taunt off non-tank engagement
    notes: humanoid; CC valid (Sap, Polymorph, Hex)

  - mob: Twilight Bruiser
    abilities:
      - name: melee_cleave
        target: frontal_cone
        notes: minor frontal cleave; tank face away from group
    notes: humanoid melee; CC valid (Polymorph, Hex)

  # Pre-Benedictus (Path of the Titans / Maw of Destruction → Wyrmrest)
  - mob: Faceless Voidstalker
    abilities:
      - name: Shadow Bolt
        interruptible: true
        priority: MUST_INTERRUPT
        notes: hardcast caster; primary kick target
    notes: Old-God-flavor adds; usually NOT CC-able (some immune)

  - mob: Corrupted Slime
    abilities:
      - name: ground_acid_patch
        avoidable: true
        notes: leaves acidic ground patches on death/movement — stand out
    notes: slime; AoE-burn pulls

  - mob: Twilight Hammer Cultist (caster pack)
    abilities:
      - name: Shadow Bolt
        interruptible: true
        priority: MUST_INTERRUPT
    notes: humanoid caster; CC valid (Polymorph, Hex, Shackle Undead)
```

### Trash skip notes

- A few mob clusters can be **wall-hugged / LoS-skipped** in speed-clears; default bot behavior is **clear all spine trash**.
- **No optional boss / branch chamber** in this instance.

### Thrall escort during trash

```yaml
thrall_during_trash:
  walk_pace: scripted_npc_walk_speed         # may be slower than default tank movement
  pause_points: yes                          # Thrall stops to monologue at scripted points; party must wait
  immune_to_trash: yes (post-hotfix)         # but death-fail rule still applies
  bot_default: |
    Bot follows tank as usual. If tank pulls ahead of Thrall, bots may
    technically lose Thrall but he's hotfix-immune to trash so it doesn't
    fail the run UNLESS a pull aggros onto Thrall (rare).
  bot_implementation_gap: |
    AltbotFollow.cpp.FollowNPCTarget(thrall_guid) — NOT IMPLEMENTED.
    See instance-wide notes for workaround.
```

### CC priorities

- **Polymorph / Sap / Hex / Shackle Undead** valid on humanoid Twilight cultists/assassins/bruisers
- **Faceless Voidstalkers** are not standard CC targets
- **Crystalline Elementals** — Hex (frog) works on elementals; Polymorph does NOT (Polymorph is humanoid/beast/critter only in Cata 4.3.4)

---

## Boss 1 — Arcurion

```yaml
boss: arcurion
npc_id: 54590
location: Crystal Vice (canyon-floor encounter; Servitor adds on overhead ledges)
hp_pct_phase_triggers: [30]
notes: |
  Single-phase tank-and-spank with periodic Icy-Tomb-on-Thrall sub-events
  and environmental Frozen Servitor adds throwing Icy Boulders from
  canyon ledges. At 30% HP, boss begins Torrent of Frost — a persistent
  group-wide Frost AoE. Thrall casts Bloodlust at this point; party burns
  the boss before AoE outpaces healer throughput.

phases:
  - id: 1
    name: regular
    trigger: pull
    end_condition: hp_pct == 30
    description: tank-and-spank with Hand of Frost interrupts, Icy Tomb sub-events, Boulder dodge

  - id: 2
    name: torrent_execute
    trigger: hp_pct == 30
    end_condition: hp_pct == 0
    description: persistent Torrent of Frost group AoE; Thrall casts Bloodlust; DPS burn

mechanics:
  - name: Hand of Frost
    spell_id: 102593
    cast_by: arcurion
    cast_kind: cast
    target: tank
    cast_time_ms: 2000
    range: 30
    school: frost
    effect: heavy single-target nuke (~45-55k Frost damage on tank)
    interruptible: true
    dispel_type: none
    avoidable: false
    priority: MUST_INTERRUPT
    notes: |
      Bot interrupt priority HIGH. Rotate kicks across DPS/tank.
      Note — some community guides describe this as a "frontal cone of
      ice"; Wowhead spell data shows single-target Frost nuke. Tank
      positions away from group out of caution.

  - name: Chains of Frost
    spell_id: 102582
    cast_by: arcurion
    cast_kind: cast
    target: pbaoe_self
    radius: 65
    school: frost                            # Magic-dispel-eligible per most sources
    effect: ~9-11k Frost damage + 10s immobilize on all enemies
    interruptible: UNVERIFIED                 # has cast bar; kick may work but not strat-priority
    dispel_type: magic                       # technically dispellable; do_not_auto_dispel per Disagreement #5
    avoidable: false
    notes: |
      Periodic AoE wave. Most strats let immobilize tick out. Bot does NOT
      auto-dispel (wasted GCDs spam-dispelling 5 players). Prompt's
      "Glaciate frost ring AoE" likely refers to this — see survey
      Disagreement #1.

  - name: Icy Tomb
    spell_id: 103252
    cast_by: arcurion
    cast_kind: scripted
    target: thrall
    school: physical                         # Wowhead lists Physical school
    effect: encases Thrall in destructible ice tomb (HP pool, UNVERIFIED value)
    interruptible: false
    dispel_type: none
    avoidable: false
    priority: TARGET_SWITCH                   # bot DPS must target tomb to free Thrall
    notes: |
      CRITICAL — if tomb not broken, Thrall takes lethal damage and
      instance fails. Bot DPS must SWITCH TARGET from Arcurion to the
      Icy Tomb spawn until destroyed, then back to boss.

  - name: Torrent of Frost
    spell_id: 103962
    cast_by: arcurion
    cast_kind: scripted_channel
    target: pbaoe_self                       # group-wide Frost AoE
    school: frost
    effect: ~13-16k Frost damage / sec + 50% movement-speed slow
    trigger: hp_pct <= 30
    interruptible: false
    dispel_type: none
    avoidable: false
    priority: BURN
    notes: |
      Phase-2 execute. Persistent — must be burned through. Thrall casts
      Bloodlust at this trigger. Bot logic: align offensive cooldowns
      (Avenging Wrath, Berserk, Tiger's Fury, Pillar of Frost, etc.) to
      Bloodlust window. Healer pre-mana before this phase.

  - name: melee
    cast_by: arcurion
    cast_kind: melee
    target: tank
    school: frost                            # tinted tank melee
    effect: standard tank melee

# Adds (environmental, not killable in standard strat)
adds:
  - name: Frozen Servitor
    npc_id: 54600
    count: multiple (4-6 around canyon ledges)
    locked_to: overhead_ledges (cannot be reached by melee)
    abilities:
      - name: Icy Boulder
        spell_id: UNVERIFIED
        cast_kind: thrown_projectile
        target: ground_telegraph_random
        cast_time_ms: 3000                    # ~3s ground reticle warning
        damage: ~20k Frost on impact
        avoidable: true
        notes: visible telegraphed reticle on ground; move out before impact
    kill_priority: NONE                       # not killable in standard strat — environmental hazard
    notes: |
      Servitors stand on canyon ledges. They are NOT targeted/killed in
      standard strat. Their Icy Boulders are the only damage they deal.

  - name: Icy Tomb (boss-spawned destructible)
    spawned_by: arcurion (Icy Tomb mechanic)
    abilities:
      - name: encase_thrall
        effect: traps Thrall; ticks lethal damage to Thrall if not broken
    kill_priority: ABSOLUTE                   # MUST kill before Thrall dies
    notes: target-switch-to-tomb is the bot's primary DPS pivot

positioning:
  default: |
    Tank holds Arcurion roughly central in the canyon; do not stand under
    overhead ledges (Servitor-Boulder reticles overlap there).
    DPS / healer spread for Boulder-reticle movement; ranged at max range
    from boss for Chains of Frost wave.
    All players burst the Icy Tomb when it spawns.
  tank_facing: away from group (standard)
  los_or_lookaway: none

interrupt_priority:
  - Hand of Frost (boss)                     # MUST_INTERRUPT every cast

dispel_priority: []                          # Chains of Frost technically Magic but do_not_auto_dispel per Disagreement #5

heroic_delta: |
  N/A — heroic-only fight. All values listed are heroic.

bot_role_summary:
  tank: hold threat; tank-position central canyon (avoid overhead ledges); face away
  healer: tank Hand-of-Frost spikes are predictable (cast bar); group AoE during Torrent execute is heavy — pre-mana CD before 30%
  melee: cycle Hand-of-Frost interrupts; switch to Icy Tomb when spawned; move out of Boulder reticles; align CDs to Bloodlust window
  ranged: switch target to Icy Tomb when spawned; participate in interrupt rotation if class has kick; max range from boss
  thrall_npc: bot does NOT control Thrall; bot DOES detect Thrall's HP (fail-condition) and Icy Tomb spawn (target switch trigger)
```

---

## Boss 2 — Asira Dawnslayer

```yaml
boss: asira_dawnslayer
npc_id: 54968
location: Galakrond's Rest (open-air encounter; corpse of Galakrond visible)
hp_pct_phase_triggers: [30]
notes: |
  Single-phase fight with caster-punish mechanic (Mark of Silence + Throw
  Knife) and ground-place damage cloud (Choking Smoke Bomb). At 30% HP,
  Blade Barrier shield activates — only single hits ABOVE the threshold
  (~30-40k UNVERIFIED) damage past the shield. Party stands in Thrall's
  Rising Fire Totem buff for the damage check.

phases:
  - id: 1
    name: regular
    trigger: pull
    end_condition: hp_pct == 30
    description: tank-and-spank with Mark/Knife caster-punish + Smoke Bomb ground-place

  - id: 2
    name: blade_barrier_execute
    trigger: hp_pct == 30
    end_condition: hp_pct == 0
    description: Blade Barrier shield up; big-hit cooldowns required to break

mechanics:
  - name: Mark of Silence
    spell_id: 102726
    cast_by: asira_dawnslayer
    cast_kind: instant
    target: ally_random_caster                # marks a random spellcaster (party)
    school: physical
    duration_ms: 20000
    effect: |
      The marked target, IF THEY CAST A SPELL, triggers Asira to throw a
      knife at them. Mark itself is non-damaging.
    interruptible: false                      # instant
    dispel_type: none                        # physical, not dispellable
    avoidable: NA                             # mitigated by caster-bot stop-casting OR body-block positioning
    priority: SELF_GATE_CASTING               # caster bot must read self-debuff and gate rotation
    notes: |
      For DoT classes, ticks already on Asira are NOT casts. NEW DoT
      applications may or may not trigger — UNVERIFIED. Channels (Mind
      Flay, Drain Soul, Mind Sear) treated as casts; bot stops channeling.
      See survey Disagreement #6.

  - name: Throw Knife
    spell_id: 103587
    cast_by: asira_dawnslayer
    cast_kind: instant_projectile
    target: marked_player
    school: physical
    effect: |
      Knife travels in a STRAIGHT LINE; damages the FIRST player it hits
      (~10k physical). If hit-target is the Mark-of-Silence target,
      additionally silences for 2.5s.
    interruptible: false
    dispel_type: none
    avoidable: yes — body-blockable
    priority: BODY_BLOCK
    notes: |
      Standard mitigation: a non-marked player (tank or melee) stands
      between Asira and the marked caster, intercepting the knife.

  - name: Choking Smoke Bomb
    spell_id: 103790
    cast_by: asira_dawnslayer
    cast_kind: instant_ground_place
    target: ground_at_caster_feet
    radius: 8                                # some sources say 10
    school: nature
    duration_ms: 20000-40000                 # UNVERIFIED — sources disagree
    effect: |
      Ground cloud. Prevents target acquisition into/out of cloud; ticks
      ~5k Nature damage / 0.5-1s to anyone inside.
    interruptible: false
    dispel_type: none
    avoidable: yes
    priority: GROUND_AVOID + TANK_DRAG_OUT
    notes: |
      TANK behavior: drag Asira ~10y away from the cloud immediately so
      she's out of it. DPS/healer never enter the cloud. While Asira
      stands inside, she is UN-TARGETABLE from outside (LoS-block) — bot
      target-loss window of ~1-3s. This is the closest mechanic to the
      prompt's "Stealth phases — bot must wait through stealth windows."
      Bot must NOT chase into the cloud. See survey Disagreement #2.

  - name: Blade Barrier
    spell_id: 103562                          # initial; "Lesser Blade Barrier" UNVERIFIED follow-up
    cast_by: asira_dawnslayer
    cast_kind: instant
    target: self
    trigger: hp_pct <= 30
    school: physical
    effect: |
      Damage-absorb shield. Hits BELOW threshold reduce to 1.
      Threshold UNVERIFIED — sources disagree (30k, 40k, tiered with
      ~30k follow-up). Single hit ABOVE threshold breaks the shield.
    interruptible: false
    dispel_type: none
    avoidable: NA
    priority: BIG_HIT_COOLDOWN
    notes: |
      DPS rotation: prioritize big-hit abilities (Aimed Shot, Pyroblast,
      Frostfire Bolt, Chimera Shot, Killing Spree, Avenger's Shield,
      Eviscerate-at-5-CP, etc.) over filler. Multiple smaller hits all
      reduce to 1 — useless.
      Stand in Thrall's Rising Fire Totem for damage buff.

  - name: melee
    cast_by: asira_dawnslayer
    cast_kind: melee
    target: tank
    school: physical
    effect: standard tank melee

# Thrall support during this fight
thrall_support:
  - name: Rising Fire Totem (Thrall)
    spell_id: UNVERIFIED
    cast_kind: scripted_summon
    effect: damage + max-health buff for nearby players (~10y radius); buff stacks per source
    notes: party stands in totem's buff area, especially during Blade Barrier execute

# Adds
adds: []                                     # no adds during this fight

positioning:
  default: |
    Tank keeps Asira CENTRAL near Thrall's totems; on Smoke Bomb, drag
    Asira ~10y to leave the cloud behind.
    Casters position so a melee/tank is BETWEEN them and Asira (body-
    block knives).
    Melee stays within Thrall's Rising Fire Totem buff zone when possible.
  tank_facing: away from group (standard)
  los_or_lookaway: |
    Body-block-knives positioning: marked caster stands BEHIND a non-
    marked melee/tank relative to Asira's position.

interrupt_priority: []                       # no kick-required casts on this fight

dispel_priority: []                          # no dispels — Mark is physical, Smoke Bomb is environmental, Blade Barrier is on boss

heroic_delta: |
  N/A — heroic-only fight.

bot_role_summary:
  tank: drag-out-of-bomb is the key non-default behavior; hold threat through movement; stay near Thrall totems
  healer: low damage with clean positioning; pre-mana before Blade Barrier execute (long fight tail if shield isn't broken quickly)
  melee: body-block knives if you're not the marked target (stand between Asira and marked caster); align big-hit CDs to Blade Barrier
  ranged: |
    CRITICAL self-debuff handling — if you have Mark of Silence (102726),
    STOP CASTING for the 20s duration OR position behind a body-blocker.
    Major bot logic hook: caster bot must read self-debuff and gate rotation.
  thrall_npc: stand in Rising Fire Totem during execute; bot does NOT control Thrall

bot_critical_flags:
  - smoke_bomb_target_loss                    # bot must wait, not chase
  - mark_of_silence_self_debuff               # caster bot stop-casting trigger
  - blade_barrier_big_hit_priority            # rotation modifier during execute
```

---

## Boss 3 — Archbishop Benedictus

```yaml
boss: archbishop_benedictus
npc_id: 54938
location: Wyrmrest Temple roof / Chamber of Aspects
hp_pct_phase_triggers: [60]
notes: |
  Two-phase transform fight. The most mechanically dense encounter in the
  dungeon. Phase 1 ("The Betrayal"): Holy theme, Thrall is alive and
  fighting alongside with Chain Lightning + Water Shell support. At 60%
  HP, Twilight Epiphany transforms the boss AND traps Thrall in Twilight
  Prison (Thrall removed from combat). Phase 2 ("Twilight Ascension"):
  same kit shifted to Shadow school, NO Thrall support — all 3 orbs must
  be dodged unaided, no Water Shell for waves.

  HEALER DISPEL PRIORITY — Righteous Shear (P1) and Twilight Shear (P2)
  are Magic-school stacking debuffs that tick AoE damage. DISPEL EVERY
  APPLICATION.

phases:
  - id: 1
    name: the_betrayal_holy
    trigger: pull
    end_condition: hp_pct == 60
    description: |
      Holy/"Light" theme. Smite hardcasts (interrupt), Righteous Shear
      stacking AoE-debuff (DISPEL every cycle), Purifying Light orbs
      (3 spawn — Thrall destroys 2/3, party dodges 1), Wave of Virtue
      room-sweep (stand in Thrall's Water Shell).

  - id: 2_transition
    name: twilight_epiphany
    trigger: hp_pct == 60
    end_condition: transform_complete
    description: |
      Boss casts Twilight Epiphany. Transforms model + reveals Twilight
      true form. Casts Twilight Prison on Thrall (Thrall removed from
      combat for rest of fight). Bot must detect this transform event.

  - id: 2
    name: twilight_ascension
    trigger: end of phase 2_transition
    end_condition: hp_pct == 0
    description: |
      Shadow theme. Twilight Blast hardcasts (interrupt), Twilight Shear
      stacking AoE-debuff (DISPEL every cycle), Corrupting Twilight orbs
      (3 spawn — NO Thrall help, all 3 must be dodged), Wave of Twilight
      room-sweep (NO Water Shell — find a gap or use immunity).

mechanics:
  # Phase 1
  - name: Smite
    spell_id: UNVERIFIED                      # tank-cast; resolve in DBC
    cast_by: archbishop_benedictus
    phase: 1
    cast_kind: cast
    target: tank
    school: holy
    cast_time_ms: 2000                        # UNVERIFIED
    effect: heavy holy nuke on tank
    interruptible: true
    dispel_type: none
    priority: MUST_INTERRUPT
    notes: rotate kicks across DPS/tank

  - name: Righteous Shear
    spell_id: 103161
    cast_by: archbishop_benedictus
    phase: 1
    cast_kind: instant
    target: ally_random
    school: holy
    stack_max: UNVERIFIED                     # stacks; bot dispels asap regardless
    effect: |
      Stacking debuff on a player. Each tick deals Holy AoE damage (~7500
      base) to allies WITHIN 10y of the affected player.
    interruptible: false
    dispel_type: magic                       # CRITICAL — healer dispels every application
    priority: HIGH_DISPEL_PRIORITY
    notes: |
      Spread positioning ≥10y minimizes cleave damage. Healer dispels
      asap to prevent stack ramp.

  - name: Purifying Light
    spell_id: 103578
    cast_by: archbishop_benedictus
    phase: 1
    cast_kind: scripted_summon
    target: above_boss
    school: physical                         # summon mechanic
    effect: |
      Spawns 3 orbs above boss. Orbs path toward random players → impact
      = Purifying Blast.
    interruptible: false
    dispel_type: none
    avoidable: true
    notes: |
      Thrall destroys 2 of 3 orbs with Chain Lightning during P1. Party
      dodges the surviving orb. Move SIDEWAYS out of orb path.

  - name: Purifying Blast
    spell_id: 103651
    cast_by: purifying_light_orb
    phase: 1
    cast_kind: impact_trigger
    target: pbaoe_orb_impact
    radius: 6
    school: holy
    effect: ~80k Holy damage on impact
    interruptible: false
    avoidable: true
    avoidance: dodge orb path; stay 6y+ from impact point

  - name: Wave of Virtue
    spell_id: 103684
    cast_by: archbishop_benedictus
    phase: 1
    cast_kind: scripted_instant
    target: room_sweep
    radius: 15                                # damage radius
    school: holy
    effect: ~97-100k Holy damage + knockback; lethal contact
    interruptible: false
    dispel_type: none
    avoidable: true
    avoidance: stand in Thrall's Water Shell bubble (also gives +100% damage)
    priority: STACK_IN_WATER_SHELL

  # Phase 2 transition
  - name: Twilight Epiphany
    spell_id: UNVERIFIED                      # transform trigger
    cast_by: archbishop_benedictus
    phase: 1_to_2
    cast_kind: scripted
    trigger: hp_pct <= 60
    effect: |
      Boss transforms (model + school swap). Casts Twilight Prison on
      Thrall (Thrall removed from combat for rest of fight).
    interruptible: false
    priority: PHASE_TRANSITION_DETECT
    notes: |
      MAJOR BOT-IMPLEMENTATION FLAG. Bot must:
        1. Detect transform (boss model swap, aura set change)
        2. Re-acquire target reference (likely same NPC ID; visual changes)
        3. Discontinue waiting for Thrall support (no Water Shell, no
           Chain Lightning destroying orbs, no totem buffs)
        4. Adjust school detection on incoming damage (Holy → Shadow)

  - name: Twilight Prison (on Thrall)
    spell_id: UNVERIFIED
    cast_by: archbishop_benedictus
    phase: 1_to_2
    cast_kind: scripted
    target: thrall
    effect: removes Thrall from combat; he's not killable in this state but is unhelpful
    interruptible: false

  # Phase 2
  - name: Twilight Blast (cast)
    spell_id: 103777
    cast_by: archbishop_benedictus
    phase: 2
    cast_kind: cast
    target: tank
    school: shadow
    cast_time_ms: 2000                        # UNVERIFIED
    effect: heavy shadow nuke on tank
    interruptible: true
    dispel_type: none
    priority: MUST_INTERRUPT
    notes: P2 mirror of Smite

  - name: Twilight Shear
    spell_id: 103526
    cast_by: archbishop_benedictus
    phase: 2
    cast_kind: instant
    target: ally_random
    school: shadow
    stack_max: UNVERIFIED
    effect: stacking debuff; ticks Shadow AoE (~7500) to allies within 10y
    interruptible: false
    dispel_type: magic                       # CRITICAL — healer dispels every application
    priority: HIGH_DISPEL_PRIORITY
    notes: P2 mirror of Righteous Shear

  - name: Corrupting Twilight
    spell_id: 103769
    cast_by: archbishop_benedictus
    phase: 2
    cast_kind: scripted_summon
    target: above_boss
    school: physical                         # summon mechanic
    effect: |
      Spawns 3 orbs above boss; orbs path toward random players → impact
      = Twilight Blast (orb-impact variant). NO THRALL HELP — all 3
      orbs must be dodged.
    interruptible: false
    avoidable: true

  - name: Twilight Blast (orb impact)
    spell_id: UNVERIFIED                      # likely paired with 103769; collision with cast variant 103777
    cast_by: corrupting_twilight_orb
    phase: 2
    cast_kind: impact_trigger
    target: pbaoe_orb_impact
    radius: 6
    school: shadow
    effect: ~80k Shadow damage on impact
    interruptible: false
    avoidable: true

  - name: Wave of Twilight
    spell_id: 103781
    cast_by: archbishop_benedictus
    phase: 2
    cast_kind: scripted_instant
    target: room_sweep
    radius: 15
    school: shadow
    effect: ~97-100k Shadow damage + knockback; lethal contact
    interruptible: false
    dispel_type: none
    avoidable: true
    avoidance: |
      NO WATER SHELL. Survive by:
        - Finding a gap in the wave (it has openings)
        - Using mobility (Iceblock, Divine Shield, Cloak of Shadows,
          Anti-Magic Shell, Dispersion, Survival Instincts)
    priority: GAP_RUN_OR_IMMUNITY

  - name: melee
    cast_by: archbishop_benedictus
    cast_kind: melee
    target: tank
    school: physical                          # both phases

# Adds
adds: []                                     # no adds; orbs are environmental projectiles

positioning:
  default: |
    SPREAD ≥10y to minimize Shear AoE-cleave to allies.
    Tank holds boss central; rotate during waves.
    P1 wave: stack INTO Thrall's Water Shell bubble (immunity + 100%
    damage buff).
    P2 wave: find the GAP in the wave OR use an immunity cooldown.
  tank_facing: away from group (standard)
  los_or_lookaway: none
  spread_radius: 10                           # minimum gap between players to escape Shear AoE

interrupt_priority:
  - Smite (P1)                                # MUST_INTERRUPT every cast
  - Twilight Blast (P2)                       # MUST_INTERRUPT every cast

dispel_priority:
  - Righteous Shear (P1) — DISPEL every application — Magic
  - Twilight Shear (P2) — DISPEL every application — Magic

heroic_delta: |
  N/A — heroic-only fight.

bot_role_summary:
  tank: interrupt Smite/Twilight Blast on every cast; hold boss central; rotate during waves; defensive CD on Wave of Twilight in P2 (no Water Shell)
  healer: |
    DISPEL EVERY SHEAR APPLICATION (Righteous P1 / Twilight P2). Highest
    dispel-priority work in the dungeon. Pre-mana CD before phase 2
    transition (Thrall heal/totem support is gone in P2).
  melee: stay in spread ≥10y from allies despite being melee (Shear AoE is 10y); dodge orbs; gap-run waves in P2
  ranged: max range; dispel-Shear support (if class has Magic-dispel: Mass Dispel from Holy Priest); orb-dodge; gap-run P2 waves
  thrall_npc: |
    Phase 1: Thrall provides Water Shell (stand in for waves) and Chain
    Lightning (destroys 2 of 3 orbs). Bot detects scripted-event triggers.
    Phase 2: Thrall is gone (Twilight Prison) — no support.

bot_critical_flags:
  - phase_2_transform_detection                # detect Twilight Epiphany; re-acquire target
  - dispel_every_shear                         # highest-priority healer dispel pattern
  - p2_wave_gap_run                            # no immunity = move to gap
  - thrall_water_shell_p1_stack                # stack-in-bubble for P1 waves
```

---

## Bot AI hooks

```yaml
ai_hooks:
  instance_wide:
    - encounter_state_thrall_alive(true)       → continue normally
    - encounter_detect_thrall_hp_low            → fail-condition flag (not directly healable)
    - encounter_detect_thrall_pause_point       → tank/follow bots wait
    - encounter_detect_thrall_walk_resume       → bots resume follow

  arcurion:
    - encounter_detect_hand_of_frost_cast       → kick_rotation_participate
    - encounter_detect_icy_tomb_spawn(thrall)   → switch_target_to_tomb
    - encounter_detect_icy_tomb_destroyed       → switch_target_back_to_boss
    - encounter_detect_icy_boulder_telegraph    → move_out_of_reticle
    - encounter_state_boss_hp_pct <= 30         → torrent_execute_phase
    - encounter_detect_thrall_bloodlust         → align_offensive_cooldowns

  asira_dawnslayer:
    - encounter_detect_mark_of_silence(self)    → stop_casting OR position_behind_body_blocker
    - encounter_detect_throw_knife_cast(target) → if_self_not_marked AND in_path → body_block
    - encounter_detect_smoke_bomb_ground_place  → tank_drag_boss_out; others avoid_cloud
    - encounter_state_boss_in_cloud             → wait (target-loss; do NOT chase)
    - encounter_state_boss_hp_pct <= 30         → blade_barrier_phase
    - encounter_state_blade_barrier_active      → big_hit_cooldown_priority
    - encounter_detect_thrall_rising_fire_totem → stand_in_totem_buff

  archbishop_benedictus:
    - encounter_detect_smite_cast(P1)           → kick_rotation_participate
    - encounter_detect_twilight_blast_cast(P2)  → kick_rotation_participate
    - encounter_detect_righteous_shear_applied(target) → healer_dispel_asap
    - encounter_detect_twilight_shear_applied(target) → healer_dispel_asap
    - encounter_detect_purifying_light_orb_summon(P1) → orb_dodge_or_thrall_handles
    - encounter_detect_corrupting_twilight_orb_summon(P2) → orb_dodge_all_3
    - encounter_detect_wave_of_virtue_cast(P1)  → stack_in_thrall_water_shell
    - encounter_detect_wave_of_twilight_cast(P2) → gap_run_or_immunity_cooldown
    - encounter_state_boss_hp_pct == 60         → twilight_epiphany_imminent
    - encounter_detect_twilight_epiphany_cast   → phase_2_transition_detect; re_acquire_target
    - encounter_detect_thrall_twilight_prison   → discontinue_thrall_support_expectations
    - encounter_state_self_dispel_capable       → high_priority_shear_dispel_role

dispel_blacklist: []                           # no dispel-blacklist entries

target_switch_triggers:
  arcurion:
    - icy_tomb_spawn → switch to tomb
    - icy_tomb_destroyed → switch back to boss
  benedictus:
    - twilight_epiphany → re-acquire (likely same NPC ID, visual swap)

phase_detection:
  arcurion: hp_pct (30 = torrent execute)
  asira_dawnslayer: hp_pct (30 = blade barrier execute)
  archbishop_benedictus: hp_pct (60 = twilight epiphany transform)

interrupt_rotation_priority:
  arcurion: [Hand of Frost (102593)]
  asira_dawnslayer: []
  archbishop_benedictus: [Smite (P1, UNVERIFIED), Twilight Blast (P2, 103777)]

healer_dispel_rotation_priority:
  arcurion: []
  asira_dawnslayer: []
  archbishop_benedictus: [Righteous Shear (103161, P1), Twilight Shear (103526, P2)]
```

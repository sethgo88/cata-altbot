# Tank — Role Generalist

**Class-agnostic tank patterns** that apply across all content (dungeon, raid, world). Per-spec details (which spell, which CD name, active mitigation cycle) live in `docs/specs/{spec}.md`. Per-encounter overrides (threat order, kite path, defensive plan) live in `docs/dungeons/{dungeon}/tank.md`.

This doc is the central reference for *how a tank thinks* — active mitigation, threat-on-pull, cone facing, defensive CD pacing, kite path planning, anti-patterns. The bot's tank logic in `AltbotCombat.cpp` should be structured around the patterns documented here, with class-specific bindings injected from the spec doc.

---

## Active mitigation — Cata's defining tank concept

```yaml
active_mitigation:
  # 4.3.4 introduced active mitigation as a core tank mechanic.
  # Each tank class has a resource-driven self-mitigation cycle the bot must operate.

  principle: |
    Damage taken is no longer purely passive (block/dodge/parry chance) — tanks
    actively reduce incoming damage by spending resources on mitigation abilities.
    Skipping the active mitigation cycle = preventable damage = dead tank.

  per_class_cycles:
    protection_warrior:
      resource: Rage (generated from auto-attacks + abilities)
      mitigation: Shield Block (60s CD baseline, on-CD usage; +25% block chance for X sec)
      consumer: Heroic Strike (rage dump for damage; not mitigation but rage management)
      cycle: |
        - Generate rage via auto-attacks + Devastate
        - Spend on Shield Block when off CD (always)
        - Heroic Strike to prevent rage capping
        - Save big rage dump for Last Stand / Shield Wall windows

    protection_paladin:
      resource: Holy Power (3 max; generated from Crusader Strike, Judgment, Hammer of the Righteous)
      mitigation: Shield of the Righteous (3 HP cost — direct damage reduction)
      mitigation_alt: Word of Glory (3 HP cost — self-heal; Cata-tank-talented for additional armor buff)
      cycle: |
        - Build HP via 9-9-9 rotation (Crusader Strike → Judgment → Hammer; rotate)
        - Spend at 3 HP on Shield of the Righteous (direct mitigation)
        - OR Word of Glory if HP is dropping (heals + buffs armor with talent)
      anti_pattern: never let HP cap at 3 (wasted generation)

    blood_death_knight:
      resource: Runes (Blood / Frost / Unholy / Death — regenerate over time)
      mitigation: Death Strike (Frost rune + Unholy rune; absorbs ~20% of damage taken in last 5s as heal + 7s shield)
      cycle: |
        - Maintain Frost Fever + Blood Plague (Icy Touch + Plague Strike or Outbreak)
        - Death Strike on cooldown (every ~5s when both runes available)
        - Heart Strike for rune fillers when DS not available
        - Blood Shield (Cata-introduced) absorbs damage based on DS heal amount
      key_concept: |
        Blood DK self-heal is THE mitigation. Healers in 4.3.4 heal Blood DKs less
        than other tanks because the DK heals itself.

    guardian_druid (called "Feral Tank" in 4.3.4 — the Guardian spec name came in MoP):
      resource: Rage (generated from auto-attacks + abilities, similar to warrior)
      mitigation: Savage Defense (proc on critical strike; absorbs damage)
      mitigation_alt: Frenzied Regeneration (rage spend; converts rage to self-heal)
      cycle: |
        - Maintain Faerie Fire stack (debuff)
        - Mangle on cooldown (rage gen + bleed)
        - Pulverize / Lacerate stack management
        - Frenzied Regen reactive on damage spike
        - Savage Defense absorbs proc — no active button, but rotation should
          maximize crit-strike chance for proc uptime
```

The bot's tank logic must run the active mitigation cycle continuously — it's the largest source of effective HP through a fight.

---

## Threat-on-pull

```yaml
threat_opener:
  # The first 3-5 seconds of a pull determine whether the bot keeps aggro or loses it.

  pre_pull:
    - position relative to pull (LOS pull, run-in, ranged tag — depends on pull type)
    - apply pre-pull buffs (class-specific: Vigilance pre-application, Bone Shield, Last Stand prep)
    - confirm party is ready (healer in range, DPS not pre-engaged)

  pull_action:
    rule: |
      Open with the highest-threat ability or ability combination available.
      AoE pulls require AoE threat opener; single-target pulls can use
      single-target high-threat opener.

  class_openers:
    protection_warrior:
      single_target: Charge → Shield Slam → Devastate
      aoe: Heroic Leap → Thunder Clap (debuff) → Shockwave (talent stun + threat)
    protection_paladin:
      single_target: Avenger's Shield (ranged silence + threat) → Crusader Strike
      aoe: Avenger's Shield (3 jumps) → Consecration → Hammer of the Righteous
    blood_death_knight:
      single_target: Death Grip → Outbreak (no-resource diseases) → Heart Strike
      aoe: Death and Decay → Pestilence (spread diseases) → Heart Strike spam
    feral_tank_druid:
      single_target: Feral Charge: Bear → Mangle → Lacerate
      aoe: Feral Charge → Swipe → Thrash + Pulverize (Cata)

  threat_buffs_and_modifiers:
    vengeance: |
      Cata-introduced passive — tanks gain attack power from damage taken
      (up to a cap). Effectively means tanks deal more damage / generate more
      threat as fights progress. Bot logic should NOT need to do anything
      special; vengeance is automatic.
    inquisition (ret/prot pally): not a tank ability — ret-only
    salvation (warrior threat dump for raid members): no longer relevant in Cata

  threat_recovery:
    # If an ally pulls aggro
    actions:
      - taunt (per-class spell name; pulls boss to you for 3s threat lock)
      - high-threat ability to re-establish (Shield Slam, Hammer of the Righteous, Heart Strike)
      - threat-CD if available (Vengeance + Recklessness for warriors, etc.)
    coordination: |
      DPS / healer should reduce activity briefly during threat recovery.
      Bot's DPS / healer logic should detect tank-loss-aggro events and pause
      aggressive actions for 1-2 GCDs.

  taunt_uses:
    rule: taunt is FREE GCD usage — no cost — fire whenever boss aggros wrong target
    cd: 8s baseline across classes (Cata)
    class_names:
      warrior: Taunt
      paladin: Hand of Reckoning (Cata)
      death_knight: Dark Command + Death Grip (DG also pulls)
      druid (feral tank): Growl
```

---

## Cone facing & boss positioning

```yaml
cone_facing:
  # Which direction the tank points the boss matters.

  default_rule: face boss AWAY from the group
  rationale: |
    Many bosses have frontal cone abilities that bypass parry/dodge/block AND
    deal large damage to whoever is in the cone. Group standing in cone =
    multiple deaths.

  specific_cone_examples:
    - Honor Guard Arc Slash (ToTT) — undodgeable / unblockable / unparriable cone
    - Erunak Magma Splash (ToTT) — 90° frontal cone, 20y, 19-21k Fire + 2k/s DoT
    - Behemoth Blight Spray (ToTT) — 90° frontal cone, 25y, 4s channel
    - many raid bosses (encounter docs flag specifics)

  positioning_choices:
    face_into_wall: |
      When possible, position with boss back-to-wall. This lets you face boss
      INTO the wall (cone hits nothing) while group attacks from behind/sides.

    face_away_from_neptulon_or_protected_target:
      example: ToTT Ozumat P1 — face Behemoth away from Neptulon (not just away from group)
      generalization: |
        If encounter has a "must-protect" NPC (escort / ally / boss-helper), tank
        must face cone-equipped enemy AWAY from that NPC AND away from group.

  facing_change_mechanics:
    pivot_speed: ~180°/sec for player characters; same for bot
    cost: ~0.5s to fully reorient
    rule: |
      Pivot smoothly; don't strafe/run during pivot (causes parry/dodge bonus
      for boss while disoriented). Stop, pivot, resume.
```

---

## Defensive cooldown pacing

```yaml
defensive_cd_pacing:
  # Categories: minor (used reactively, short CD), major (used pre-emptively, long CD).

  minor_defensives:
    examples_by_class:
      warrior: Shield Block (60s — but also part of active mitigation), Spell Reflection (10s)
      paladin: Divine Protection (60s; -20% damage taken; 10s duration)
      death_knight: Bone Shield (60s; charges-based armor + dodge; Unholy spec specifically)
      druid: Barkskin (60s; -20% damage)
    pacing: rotate on cooldown when not saving for major
    target_use_rate: ~1 use per 60-90s

  major_defensives:
    examples_by_class:
      warrior: Shield Wall (5 min CD; -40% damage 12s), Last Stand (3 min; +30% max HP for 20s)
      paladin: Ardent Defender (3 min CD; -20% damage + emergency cheat-death); Lay on Hands (10 min; full self-heal)
      death_knight: Icebound Fortitude (3 min; -20% damage + stun immunity; 12s); Vampiric Blood (1 min; +35% max HP + self-heal 15s; Blood spec)
      druid: Survival Instincts (3 min; -50% damage; 12s); Frenzied Regeneration (3 min; rage→heal)
    pacing: |
      Save for KNOWN damage spikes (encounter-flagged) OR emergency response to
      missed interrupts / rotational misses.
    target_use_rate: 1-3 uses per encounter; stack with healer CDs

  emergency_cooldowns:
    examples_by_class:
      paladin: Lay on Hands (10 min; full self-heal — once per encounter)
      death_knight: Death Strike (active mitigation but also self-heal — works as emergency)
      druid: Frenzied Regeneration (rage dump for self-heal)
    use: when other defensives are CD AND tank.hp_pct < 30

cd_alignment:
  # How to stack cooldowns
  rule: |
    Stack minor + major + healer CD when a known-spike mechanic is incoming.
    Don't stack two same-tier CDs (minor + minor) — they don't multiply effectively;
    rotate them serially instead.

  cd_budget_per_encounter:
    short_5min_fight: 1 major + 3-5 minors typical
    long_10min_fight: 2-3 majors + 8-10 minors typical
    raid_boss_15min_fight: 3-5 majors + 12-15 minors typical
```

---

## Kite path planning

```yaml
kite_path:
  # When the encounter requires moving the boss (ground AoE pools, telegraphed
  # spread mechanics, etc.), tank must plan a kite path BEFORE the first kite move.

  principles:
    - one_way_route: don't backtrack — once you've moved away from a position, that ground is "used"
    - plan_full_loop: ensure the path returns to a clean area after a full lap
    - leave_clean_ground_for_party: kite path should preserve standing-room for melee + ranged

  examples:
    perimeter_kite: |
      Boss spawns Dark Fissure (ToTT Ulthok) — tank kites along the room
      perimeter, dropping fissures in a line. Group stays in the open center.

    circular_kite: |
      Multiple Beasts spawn (ToTT Ozumat P2) — tank circles the chamber
      counterclockwise (or clockwise, pick one), keeping Beasts ahead of the
      party. Lap takes ~30-45s.

  kite_during_party_repositioning:
    rule: |
      If party needs to relocate (ranged spread, healer to drink, etc.), tank
      slows the kite or reverses momentarily. Communication with party state.

terrain_awareness:
  pillar_los:
    use: rare in 5-mans; some raid encounters require LoS pulls (ranged hit pillar to break enemy cast)
    bot_action: per-encounter override; not a default behavior

  dropoffs_and_walls:
    rule: |
      Avoid kiting boss into a wall (corners trap melee; tank loses pivot
      space). Avoid dropoffs (boss might leash + reset).
```

---

## Add pickup

```yaml
add_pickup:
  # When adds spawn during a fight, tank must collect them.

  signals:
    - encounter.md flags add-spawn events with timestamps / phase triggers
    - real-time: bot detects new hostile units within X yards of party

  pickup_action:
    step_1: identify add(s) — single or multi
    step_2: choose threat ability:
      - single add: taunt + Shield Slam / Avenger's Shield / Death Grip
      - multi add: AoE threat opener (Thunder Clap / Avenger's Shield ricochet / DnD / Swipe)
    step_3: re-establish positioning (face away from group; cone outward if applicable)

  prioritization:
    - threat-on-healer first (healer aggro = wipe)
    - threat-on-ranged-DPS second (squishy)
    - threat-on-melee-DPS third (they're already in melee, less risk)

  example_scenarios:
    - "boss summons 2 adds at 60% HP" → encounter flags this in phase block; tank pre-positions for AoE pickup
    - "interrupt-rotation fails on add Healing Wave" → add lives longer; tank holds threat through extended add fight
    - "ally pulls aggro on uncolected add" → emergency taunt + threat regen
```

---

## Anti-patterns (cross-content)

```yaml
anti_patterns:
  - id: cone_facing_group
    rule: never face cone-equipped boss toward the group
    rationale: cones bypass standard avoidance; party deaths
    bot_implementation: |
      Boss-positioning logic computes "away from party centroid" heading and
      enforces it via movement before threat actions. Override only when boss
      has no cone.

  - id: kite_into_used_ground
    rule: do not backtrack into ground containing persistent AoE / fissure / pool
    rationale: party walks through used ground = damage event
    bot_implementation: |
      Maintain "no-stand zones" list with timestamp + center + radius. Tank
      pathing avoids these zones when computing kite direction.

  - id: skip_active_mitigation
    rule: do not let active-mitigation resource cap (Holy Power, Rage with Shield Block off CD, etc.)
    rationale: capped resource = wasted generation; missed mitigation = avoidable damage
    bot_implementation: |
      Active mitigation cycle runs as part of the rotation tier system. Per-spec
      rotation prioritizes mitigation spend at near-cap.

  - id: defensive_hoarding
    rule: do not save major defensives for a moment that may never come
    rationale: late = dead; CD comes back, but tank doesn't
    bot_implementation: |
      Major defensive CD use gated on `predicted_damage_in_next_3s > tank.current_hp * 0.5`
      OR `tank.hp_pct < 35 AND no_immediate_heal_available`.

  - id: taunt_off_cd_anti_pattern
    rule: |
      Do NOT taunt when YOU already have aggro and it's on cooldown for nothing.
      Taunt is for aggro RECOVERY, not refresh.
    bot_implementation: |
      Taunt gated on `not bot.has_threat(target) OR target.threat_target != bot`.

  - id: pivot_during_attack
    rule: do not strafe / run while the boss is mid-swing — boss attacks gain dodge/parry bonus on disoriented tank
    rationale: avoidable damage spike
    bot_implementation: |
      Movement / facing-change actions paused for 0.3s before an incoming boss
      auto-attack swing.

  - id: aoe_threat_without_aoe_damage_followup
    rule: do not use AoE threat (Thunder Clap, Avenger's Shield, DnD, Swipe) and then switch to single-target
    rationale: threat decays without sustain; adds break off after 3-5s
    bot_implementation: |
      AoE threat pulls require AoE-sustain rotation tier. If only one add exists,
      use single-target opener instead.

  - id: ignore_pull_los
    rule: do not pull bosses with line-of-sight required mechanics from open ground
    rationale: ranged DPS can't break LoS on their casts; bot's own threat is the only thing pulling it
    bot_implementation: |
      Pull-position logic per encounter — encounter doc flags LoS-required pulls;
      tank repositions before opener.
```

---

## Class-spec mapping table

Quick reference: how each Cata 4.3.4 tank spec realizes the role-level patterns. Per-spec docs declare these bindings.

| Pattern | Protection Warrior | Protection Paladin | Blood Death Knight | Feral Tank Druid (Guardian-equivalent) |
|---|---|---|---|---|
| Resource | Rage | Holy Power + Mana | Runes + Runic Power | Rage |
| Active mitigation | Shield Block (60s — on CD) + Heroic Strike (rage dump) | Shield of the Righteous (3 HP) + Word of Glory (3 HP, talented Cata) | Death Strike (every ~5s) + Blood Shield (auto-applied from DS) | Savage Defense (crit-proc absorb) + Frenzied Regeneration (rage spend self-heal) |
| Threat opener (single) | Charge → Shield Slam → Devastate | Avenger's Shield → Crusader Strike → Judgment | Death Grip → Outbreak → Heart Strike | Feral Charge: Bear → Mangle → Lacerate |
| Threat opener (AoE) | Heroic Leap → Thunder Clap → Shockwave | Avenger's Shield (3 jumps) → Consecration → Hammer of the Righteous | Death and Decay → Pestilence → Heart Strike spam | Feral Charge → Swipe → Thrash + Pulverize |
| Taunt | Taunt (8s) | Hand of Reckoning (8s, Cata) | Dark Command (8s) + Death Grip (35s, also pulls) | Growl (8s) |
| Major defensive | Shield Wall (5 min, -40%) + Last Stand (3 min, +30% HP) | Ardent Defender (3 min, -20% + cheat death) + Lay on Hands (10 min, full heal) | Icebound Fortitude (3 min, -20%) + Vampiric Blood (1 min, +35% HP + heal) | Survival Instincts (3 min, -50%) + Frenzied Regen (rage→heal) |
| Minor defensive | Shield Block (also active mitigation) + Spell Reflection (10s) | Divine Protection (60s, -20%) | Bone Shield (Unholy only) + Anti-Magic Shell (45s, magic absorb) | Barkskin (60s, -20%) |
| Self-heal | n/a (relies on healer + Last Stand HP buffer) | Word of Glory (Holy Power) + Lay on Hands | Death Strike (rune cycle) — primary survival tool | Frenzied Regen (rage→heal) |
| Stun / interrupt | Pummel (10s interrupt) + Concussion Blow (talent stun) + Shockwave (talent AoE stun) | Rebuke (15s interrupt) + Hammer of Justice (1 min stun) | Mind Freeze (10s interrupt) + Strangulate (silence, 2 min) | Skull Bash (15s interrupt + gap-closer) + Bash (1 min stun) |
| Mobility | Charge + Heroic Leap + Intervene | Speed of Light (talent) + Hand of Freedom (self) | Death's Advance (Cata? — talent) | Stampeding Roar (raid speed, 2 min) + Dash |
| Cleanse | n/a | Cleanse (magic with talent; disease + poison) | n/a (DK has no cleanse) | Remove Corruption (curse + poison) — only with Resto offspec |

Per-class spec docs flesh out exact spell IDs, CDs, and rotation cycles.

---

## How dungeon-level docs reference this doc

Per-dungeon tank docs (`docs/dungeons/{dungeon}/tank.md`) should:

1. Reference this doc by file path in the preamble
2. Only document **deviations** from the generalist patterns (encounter-specific threat order, kite path, defensive plan)
3. Not duplicate active mitigation, threat-opener, cone-facing, or anti-patterns documented here

---

## Open hooks for `AltbotCombat.cpp`

The bot's tank evaluation order each tick:

```
1. resolve threat target (current boss, add-pickup queue, taunt-recovery target)
2. enforce facing direction (away from group; cone-outward if cone-equipped)
3. enforce kite path constraint (avoid no-stand zones)
4. evaluate active mitigation cycle (resource generation + spend)
5. evaluate defensive CD plan (minor reactive, major pre-empt)
6. evaluate threat sustain (single-target high-threat OR AoE sustain depending on add count)
7. cast on resolved target
```

Class-specific bindings come from `docs/specs/{active_spec}.md`.
Encounter-specific overrides come from `docs/dungeons/{active_dungeon}/tank.md`.

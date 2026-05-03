# Zul'Aman — Encounter Facts

**Role-neutral mechanic data** for the bot's encounter handling. Per-role response logic lives in sibling files (`healer.md`, `ranged-dps.md`, `melee-dps.md`, `tank.md`). When a role doc references a mechanic, it cites the spell ID defined here.

Spell IDs verified at `wowhead.com/cata/spell=<id>` where consensus could be reached; many are flagged **UNVERIFIED** in this doc (most are TBC-carryover IDs whose Cata applicability needs DBC confirmation) and aggregated in `docs/research/dbc-verification-checklist.md`. Strategy reconciled across 5-7 sources — see `docs/research/zul-aman-guide-survey.md` for the audit trail.

---

## Overview

```yaml
dungeon: zul_aman
zone: zul_aman
continent: eastern_kingdoms_ghostlands
difficulty_modes: [heroic]                 # HEROIC ONLY — no normal-mode version
level_range_heroic: 85
boss_count: 6
optional_bosses: []                        # all 6 are required for clear; bear-mount timed run is bot-ignored
bosses_in_order:
  - akil_zon                               # Eagle aspect
  - nalorakk                               # Bear aspect
  - jan_alai                               # Dragonhawk aspect
  - halazzi                                # Lion aspect
  - hex_lord_malacrass                     # humanoid; random ability set
  - daakara                                # final boss; aspect-cycle through forms
notes: |
  Heroic-only Cata 5-man, originally a TBC 10-man raid revamped in patch 4.1.
  Mostly outdoor zone with multiple gates that unlock on boss kills.

  Optional bear-mount timed run: kill all 4 animal aspects (Akil'zon,
  Nalorakk, Jan'alai, Halazzi) within ~20 minutes for the Amani War Bear
  drop. Bot does NOT track time; bear-mount logic is OUT OF SCOPE.

  Heavy interrupt + dispel pressure throughout. This is one of the harder
  Cata 5-man heroics. No instance-wide curse/disease/poison dispel
  requirement. Magic dispels are per-encounter (Halazzi Frenzy/Flame Shock,
  some Hex Lord ability sets).

  Hex Lord Malacrass is NON-DETERMINISTIC — copies abilities from a random
  class each pull. Bot must detect his ability set at runtime and respond
  reactively.

  Daakara cycles through aspect forms (troll → bear → eagle → lynx →
  dragonhawk). Bot must detect form changes and switch playbook per form.
```

---

## Trash — notable mechanics

Only the trash entries with mechanics worth automating. Standard packs (basic Amani'shi grunts, low-elite mobs) are pure threat-and-DPS and not enumerated.

```yaml
trash:
  # Pre-Akil'zon (eagle gate approach)
  - mob: Amani'shi Tribesman
    abilities:
      - name: melee_cleave
        target: frontal_cone
        notes: tank face away
      - name: Hex
        interruptible: true
        priority: SHOULD_INTERRUPT
        notes: short-cast hex; kick on cooldown
    notes: humanoid; CC valid (Polymorph, Hex, Sap, Banish-no, Repentance)

  - mob: Amani'shi Warbringer
    abilities:
      - name: Battle Shout
        interruptible: true
        priority: MUST_INTERRUPT
        notes: self/group buff cast — DENY by interrupting
    notes: large mob; high HP; tank-and-spank from front; kick rotation on Battle Shout

  - mob: Amani'shi Axe Thrower
    abilities:
      - name: Throw Axe
        target: ranged_random
        interruptible: false                # instant cast
        notes: LoS to control; or kill on sight
    notes: ranged thrower; LoS-pull viable

  - mob: Eagles (small flying)
    abilities:
      - name: melee
    notes: low-HP; AoE-burn priority

  # Pre-Nalorakk (bear path)
  - mob: Amani'shi Beast Tamer
    abilities:
      - name: melee
      - name: pet_command (controls Amani Bear)
    notes: |
      CC the Beast Tamer's pet (Amani Bear) via Hibernate (Druid) or
      Freezing Trap (Hunter). Most CC doesn't affect beasts.

  - mob: Amani Bear (pet of Beast Tamer)
    abilities:
      - name: melee
      - name: bear_charge
    notes: beast; CC limited to Hibernate / Freezing Trap; brute-force kill

  - mob: Amani'shi Savage
    abilities:
      - name: Burning Brand
        target: tank
        interruptible: false
        notes: stacking bleed; tank stays still, healer HoTs
    notes: melee; humanoid CC-able

  # Pre-Jan'alai (dragonhawk path)
  - mob: Amani Dragonhawk Hatcher
    abilities:
      - name: caster_hardcast
        interruptible: true
        priority: MUST_INTERRUPT
    notes: caster trash; Polymorph/Hex valid

  - mob: Amani Dragonhawk
    abilities:
      - name: melee
      - name: flame_breath_cone
        target: frontal_cone
    notes: beast; brute-force kill; tank face away

  # Pre-Halazzi (lion path)
  - mob: Amani Lynx
    abilities:
      - name: melee
    notes: beast; brute-force kill

  - mob: Amani'shi Tempest
    abilities:
      - name: Lightning Bolt
        interruptible: true
        priority: MUST_INTERRUPT
    notes: caster; Polymorph/Hex valid

  # Pre-Hex Lord
  - mob: Amani Berserker
    abilities:
      - name: melee_cleave
        target: frontal_cone
      - name: Whirlwind
        target: pbaoe_self
    notes: high-HP melee; AoE-pull viable; tank face away

  # Pre-Daakara
  - mob: Amani Wandering Spirit
    abilities:
      - name: ghost_hardcast
        interruptible: true
        priority: SHOULD_INTERRUPT
    notes: ghost caster; Mind Soothe / dispel ghost-flavor effects
```

### Trash skip notes

- Some patrols can be wall-hugged on speed-clears. Bear-mount runs skip aggressively but bot ignores timer.
- Hex Lord 4-troll-council adds: per most guides, **REMOVED in 5-man heroic** (UNVERIFIED — see survey Disagreement #5). Bot defaults to "no council adds" — falls back to standard add-pickup if any appear.

### CC priorities

- Polymorph / Hex / Sap / Repentance valid on Amani **humanoids** (Tribesman, Warbringer, Tempest, Beast Tamer, Berserker)
- **Beasts** (Bears, Lynxes, Dragonhawks): only Hibernate (Druid) or Freezing Trap (Hunter) work
- Eagles (small): too low-HP; AoE-burn

---

## Boss 1 — Akil'zon (Eagle Aspect)

```yaml
boss: akil_zon
npc_id: 23574                              # UNVERIFIED — TBC original; Cata recycle
location: first encounter, atop the eagle gate platform
hp_pct_phase_triggers: []
notes: |
  Single-phase encounter on a recurring cycle. Group must distinguish two
  OPPOSITE positioning modes: SPREAD for Static Disruption (targeted
  ground patches) and STACK for Eagle Storm (group-mitigation lift).

phases:
  - id: 1
    name: regular
    trigger: pull
    end_condition: hp_pct == 0
    description: |
      Tank-and-spank with Static Disruption (spread), Eagle Storm (stack),
      Call Lightning interrupts, and Soaring Eagle add-spawns on a
      repeating cycle.

mechanics:
  - name: Static Disruption
    spell_id: 43622                        # UNVERIFIED — TBC carryover (orig 43511)
    cast_by: akil_zon
    cast_kind: cast
    target: ally_random
    cast_time_ms: 1500
    cooldown_ms: 10000-15000
    effect: |
      Lands on targeted player; AoE Nature damage on landing AND drops
      persistent ground patch (~10s duration). Targeted player MUST
      MOVE OUT of group. Standers in the patch take continuous Nature
      damage.
    interruptible: false
    dispel_type: none                      # disagreement — community split; bot defaults to "do not dispel"
    avoidable: true
    avoidance: targeted_player moves out of group; everyone stays spread (3y+) by default
    priority: SPREAD_POSITIONING

  - name: Electrical Storm / Eagle Storm
    spell_id: 43648                        # UNVERIFIED
    cast_by: akil_zon
    cast_kind: cast_with_telegraph
    target: ally_random_then_pbaoe
    telegraph_ms: 3000-5000
    duration_ms: 6000
    effect: |
      Selects designated player; ~3-5s telegraph; player and any nearby
      players are LIFTED into a storm cloud where lightning damage is
      SHARED among lifted players. Solo'd Storm is ~lethal.
    interruptible: false
    dispel_type: none
    avoidable: false                       # mandatory mechanic
    avoidance: STACK tight (within ~3y) on telegraph
    priority: GROUP_STACK
    notes: |
      OPPOSITE positioning vs Static Disruption. Bot must distinguish:
      - Static Disruption telegraph → spread
      - Eagle Storm telegraph → stack

  - name: Call Lightning
    spell_id: 43661                        # UNVERIFIED
    cast_by: akil_zon
    cast_kind: cast
    target: ally_random
    cast_time_ms: 1500
    cooldown_ms: 12000
    effect: single-target Nature damage cast
    interruptible: true
    dispel_type: none
    avoidable: false
    priority: MUST_INTERRUPT
    notes: rotate kicks across DPS/tank

  - name: Soaring Eagles (add summon)
    spell_id: 43654                        # UNVERIFIED summon
    cast_by: akil_zon
    cast_kind: scripted
    target: platform_perimeter
    effect: small flying Eagle adds spawn around platform; melee-charge group
    interruptible: false                   # spawn event itself

  - name: melee
    cast_by: akil_zon
    cast_kind: melee
    target: tank

# Adds
adds:
  - name: Soaring Eagle
    count: 2-4 per spawn (UNVERIFIED)
    abilities:
      - name: melee
        target: closest_player
    notes: low HP; tank picks up via AoE threat OR ranged DPS burns down on spawn

positioning:
  default: SPREAD (3y+ between players) for Static Disruption
  eagle_storm_telegraph: STACK tight (~3y) on designated lift player
  tank_facing: away from group (no frontal cone but melee positioning)
  los_or_lookaway: none
  override: |
    POSITIONING TOGGLES — bot must switch between SPREAD and STACK based
    on Akil'zon cast detection:
    - Static Disruption → spread (default)
    - Eagle Storm telegraph → stack
    - Storm channel ends → return to spread

interrupt_priority:
  - Call Lightning (boss)                  # MUST_INTERRUPT every cast on heroic

dispel_priority: []                        # no confirmed dispels

heroic_delta: |
  All ZA is heroic-only — no normal mode to delta against. Listed here for
  consistency: Static Disruption tick higher; Eagle Storm lethal if soloed;
  Call Lightning unintercepted is healer-pressure.

bot_role_summary:
  tank: hold threat; pick up Soaring Eagles on spawn via AoE threat tools
  healer: pre-stack heal CD for Eagle Storm telegraph; reactive on Static Disruption splash
  melee: rotate Call Lightning kicks; AoE-burn Soaring Eagles
  ranged: SPREAD by default; STACK under designated lift player on Eagle Storm telegraph
```

---

## Boss 2 — Nalorakk (Bear Aspect)

```yaml
boss: nalorakk
npc_id: 23576                              # UNVERIFIED — TBC carryover
location: second encounter, bear shrine chamber after bear-aspect gate
hp_pct_phase_triggers: []                  # form swap is time-based (~30-45s; UNVERIFIED)
notes: |
  Single fight with alternating troll ↔ bear form. Form swap cadence
  ~30-45s (UNVERIFIED). Mangle bleed stacks reset on each form swap, so
  the single tank rides through stacks (NO tank-swap mechanic in 5-man
  heroic — see survey Disagreement #2).

phases:
  - id: 1
    name: troll_form
    trigger: pull (default)
    end_condition: form_swap (timer or HP threshold; UNVERIFIED)
    description: |
      Cast-heavy form; Mojo Volley repeatedly (interruptible). Melee
      contribution from Nalorakk weak. Group AoE-pressure.

  - id: 2
    name: bear_form
    trigger: form_swap
    end_condition: form_swap (timer or HP threshold; UNVERIFIED)
    description: |
      No casts; pure melee form with Brutal Swipe cone + Mangle bleed
      stacking on tank. Tank face-away from group; healer pre-HoTs.

  - id: cycle
    name: alternating
    description: troll ↔ bear cycle until kill

mechanics:
  - name: Mojo Volley
    spell_id: 43375                        # UNVERIFIED
    cast_by: nalorakk
    cast_kind: cast (channeled)
    target: pbaoe_random
    cast_time_ms: 1500
    duration_ms: 3000
    effect: multi-target Nature damage
    interruptible: true
    dispel_type: none
    avoidable: false
    priority: MUST_INTERRUPT
    notes: PRIMARY kick target on troll form

  - name: Brutal Swipe
    spell_id: 24237                        # UNVERIFIED — TBC carryover
    cast_by: nalorakk (bear form only)
    cast_kind: cast
    target: frontal_cone
    cast_time_ms: 2000
    cooldown_ms: 10000
    effect: heavy frontal cone Physical damage
    interruptible: false
    dispel_type: none
    avoidable: true
    avoidance: face boss away from group; melee position behind/side

  - name: Mangle
    spell_id: 44955                        # UNVERIFIED
    cast_by: nalorakk (bear form only)
    cast_kind: melee_application
    target: tank
    effect: stacking bleed on tank during bear form
    decay: stacks RESET on form swap to troll
    interruptible: false
    dispel_type: none                      # Physical bleed — not dispellable
    avoidable: false

  - name: Surge
    spell_id: 27797                        # UNVERIFIED
    cast_by: nalorakk
    cast_kind: instant
    target: ally_random
    cooldown_ms: 20000
    effect: charge + knockback + damage on impact
    interruptible: false
    dispel_type: none
    avoidable: partial
    avoidance: spread to prevent multi-hit

  - name: Lacerating Slash
    spell_id: 43267                        # UNVERIFIED — collision with Halazzi Saber Lash
    cast_by: nalorakk
    cast_kind: UNVERIFIED                  # may be tank-melee bleed or hardcast
    target: tank
    effect: DoT bleed on tank
    interruptible: UNVERIFIED
    dispel_type: none

  - name: melee
    cast_by: nalorakk
    cast_kind: melee
    target: tank

# Adds
adds: []                                   # no adds on Nalorakk

positioning:
  default: tank pins boss central; melee behind boss; ranged max range; healer max range
  tank_facing: away from group (Brutal Swipe cone in bear form; flexible in troll form)
  los_or_lookaway: none
  spread_for_surge: ranged spread to prevent Surge multi-hit

interrupt_priority:
  - Mojo Volley (boss, troll form)         # MUST_INTERRUPT every cast
  - Lacerating Slash (UNVERIFIED if interruptible)

dispel_priority: []                        # bleeds are Physical; no dispels

heroic_delta: |
  Heroic-only; consistency note. Mojo Volley unintercepted = group damage spike.
  Mangle stacks higher in bear form; tank defensive CD pacing required.

bot_role_summary:
  tank: face-management — face away in bear form (cone); rotate defensives during high Mangle stacks; ride through bleed (single tank)
  healer: pre-HoT before bear-form swap (predictable from form-change telegraph)
  melee: rotate kicks in troll form; standard behind-target in bear form
  ranged: spread for Surge charge; rotate kicks in troll form (Counterspell/Wind Shear/etc.)
```

---

## Boss 3 — Jan'alai (Dragonhawk Aspect)

```yaml
boss: jan_alai
npc_id: 23578                              # UNVERIFIED — TBC carryover
location: third encounter, egg-piled chamber after dragonhawk-aspect gate
hp_pct_phase_triggers: [35]                # major Hatch All Eggs trigger
notes: |
  Heavy AoE-add fight. Hatcher trolls patrol egg piles; if a Hatcher reaches
  an egg pile and channels uninterrupted, those eggs hatch into dragonhawk
  hatchlings. At 35% boss HP, ALL remaining eggs hatch (massive add-spawn).

  THE MANA-CHECK FIGHT of ZA heroic for healer. Save AoE healing CDs
  (Mana Tide / Innervate / Hymn / Spirit Link / Tranquility) for the 35%
  Hatch All Eggs phase.

phases:
  - id: 1
    name: regular_with_hatchers
    trigger: pull
    end_condition: hp_pct == 35
    description: |
      Tank-and-spank with Flame Breath (cone) + Fire Bomb ground patches.
      Hatcher trolls spawn periodically and walk toward eggs; kill them on
      sight (or interrupt their channel if they reach eggs).

  - id: 2
    name: hatch_all_eggs
    trigger: hp_pct == 35
    end_condition: all hatchlings dead
    description: |
      All remaining eggs hatch simultaneously. Heavy AoE-burn phase.
      Save AoE heal CDs for here. Tank uses AoE threat tools to grab
      hatchlings. DPS swaps to AoE rotation.

  - id: 3
    name: execute
    trigger: hatchlings dead
    end_condition: hp_pct == 0
    description: resume single-target on Jan'alai

mechanics:
  - name: Flame Breath
    spell_id: 43124                        # UNVERIFIED
    cast_by: jan_alai
    cast_kind: cast
    target: frontal_cone
    cast_time_ms: 2000
    cooldown_ms: 12000
    effect: heavy frontal cone Fire damage
    interruptible: false
    dispel_type: none
    avoidable: true
    avoidance: face boss away from group; melee behind/side; ranged out of arc

  - name: Fire Bomb
    spell_id: 43137                        # UNVERIFIED
    cast_by: jan_alai
    cast_kind: ground_target
    target: ally_random_position
    duration_ms: 10000
    effect: persistent fire ground patch; DoT on standers
    interruptible: false
    dispel_type: none
    avoidable: true
    avoidance: move out of patch

  - name: Hatch All Eggs
    spell_id: 43144                        # UNVERIFIED
    cast_by: jan_alai
    cast_kind: scripted
    trigger: hp_pct == 35
    effect: ALL remaining eggs hatch into dragonhawk hatchlings
    interruptible: false
    priority: PHASE_TRANSITION
    notes: predictable HP trigger; pre-position AoE heal CDs

  - name: Summon Hatcher
    spell_id: UNVERIFIED                    # collision with above
    cast_by: jan_alai
    cast_kind: scripted
    cadence: every ~30-45s (UNVERIFIED)
    effect: 2 Hatcher trolls spawn at side of chamber; walk toward eggs
    interruptible: false
    priority: KILL_ON_SIGHT

  - name: Hatcher's Hatch (channel)
    spell_id: 43734                        # UNVERIFIED
    cast_by: amani_dragonhawk_hatcher (add)
    cast_kind: channel
    target: egg_pile
    cast_time_ms: 3000-5000
    effect: hatches all eggs in target pile early
    interruptible: true
    priority: MUST_INTERRUPT
    notes: fallback if Hatcher not killed in time; primary response is KILL the Hatcher

  - name: Dragonhawk Hatchling melee
    cast_by: amani_dragonhawk_hatchling (add)
    cast_kind: melee
    target: closest_player

  - name: melee
    cast_by: jan_alai
    cast_kind: melee
    target: tank

# Adds
adds:
  - name: Amani Dragonhawk Hatcher
    count: 2 per spawn cycle (~30-45s cadence)
    abilities:
      - name: melee
      - name: Hatcher's Hatch (channel; see mechanic above)
    kill_priority: HIGH
    cc_options: [Polymorph, Hex, Freezing_Trap]   # humanoid
    notes: kill on sight; CC fallback if multi-hatcher overlap

  - name: Amani Dragonhawk Hatchling
    count: many (from Hatch All Eggs at 35%, plus per-hatcher)
    abilities:
      - name: melee
    notes: low HP; AoE-burn priority

positioning:
  default: |
    Tank pins Jan'alai in CENTER of chamber facing AWAY from group.
    Melee behind boss. Ranged max range. Healer max range.
  tank_facing: away from group (Flame Breath cone)
  los_or_lookaway: none
  hatch_phase: |
    Tank pulls boss + hatchlings to a clear area; uses AoE threat tools.
    Group remains spread for Fire Bomb avoidance.

interrupt_priority:
  - Hatcher's Hatch (Hatcher add)          # if Hatcher not killed in time
  # Jan'alai itself has no interruptible casts in standard rotation

dispel_priority: []                        # no dispels required

heroic_delta: |
  Heroic-only; consistency note. Hatchers tougher; Fire Bomb damage higher;
  35% Hatch All Eggs spawns more hatchlings.

bot_role_summary:
  tank: AoE threat tools CRITICAL — hatchlings will go for closest non-tank otherwise. Center pin; face away.
  healer: SAVE major mana CDs for 35% phase (Mana Tide / Innervate / Hymn / Spirit Link / Tranquility). Pre-cast AoE heal CDs before the hatch.
  melee: switch to AoE rotation on hatchling spawns (Bladestorm/Whirlwind/Divine Storm/Howling Blast/Magma Totem/Swipe-cat); kill Hatchers on sight
  ranged: switch to AoE rotation on hatchling spawns (Blizzard/Seed of Corruption/Multi-Shot/Mind Sear/Hurricane/Chain Lightning); kill Hatchers on sight
```

---

## Boss 4 — Halazzi (Lion Aspect)

```yaml
boss: halazzi
npc_id: 23577                              # UNVERIFIED — TBC carryover
location: fourth encounter, lion shrine after lion-aspect gate
hp_pct_phase_triggers: [66, 33]
notes: |
  Multi-phase by HP threshold. Halazzi splits at 66% and 33%, spawning a
  Spirit of the Lynx add at each split. Group AoE-burns the Spirit while
  pressuring Halazzi. Frenzy and Flame Shock require Magic dispels.

phases:
  - id: 1
    name: phase_1_lion
    trigger: pull
    end_condition: hp_pct == 66
    description: |
      Lion-form tank-and-spank. Saber Lash damage SPLIT between tank and
      one nearest melee. Frenzy enrage proc (Magic dispel). Flame Shock
      single-target debuff (Magic dispel).

  - id: 2
    name: split_at_66
    trigger: hp_pct == 66
    end_condition: spirit_of_lynx dead
    description: |
      Halazzi splits — Halazzi continues + Spirit of the Lynx add spawns.
      AoE-burn Spirit while pressuring Halazzi. Lightning Totem may appear
      (kill priority).

  - id: 3
    name: phase_2
    trigger: spirit dead
    end_condition: hp_pct == 33
    description: continued ST on Halazzi with reduced ability set

  - id: 4
    name: split_at_33
    trigger: hp_pct == 33
    end_condition: spirit_of_lynx dead
    description: second Spirit of the Lynx spawn; AoE-burn while continuing

  - id: 5
    name: execute
    trigger: spirit dead
    end_condition: hp_pct == 0
    description: final burn; Halazzi may enrage

mechanics:
  - name: Saber Lash
    spell_id: 43267                        # UNVERIFIED
    cast_by: halazzi
    cast_kind: cast
    target: split_tank_plus_nearest_melee
    cast_time_ms: 1500
    cooldown_ms: 10000
    effect: damage SPLIT between tank and ONE nearest melee
    interruptible: false
    dispel_type: none
    avoidable: partial
    avoidance: |
      Exactly ONE melee shares with tank (Saber Lash partner). Other
      melee position back/side to avoid sharing.
    priority: MELEE_POSITIONING_RULE

  - name: Frenzy
    spell_id: 43139                        # UNVERIFIED
    cast_by: halazzi
    cast_kind: proc
    target: self
    effect: self-buff giving haste/damage
    interruptible: false
    dispel_type: magic
    avoidable: false
    avoidance: |
      DISPEL via Tranquilizing Shot (Hunter), Soothe (Druid), Spell Steal
      (Mage). Spell Steal also gains the haste buff for the Mage.
    priority: SHOULD_DISPEL

  - name: Flame Shock (Halazzi)
    spell_id: 43411                        # UNVERIFIED
    cast_by: halazzi
    cast_kind: cast
    target: ally_random
    cast_time_ms: 1500
    effect: single-target Fire DoT debuff
    interruptible: UNVERIFIED               # treat as not interruptible
    dispel_type: magic
    avoidable: false
    avoidance: dispel as Magic
    priority: SHOULD_DISPEL

  - name: Lightning Totem
    spell_id: 43461                        # UNVERIFIED
    cast_by: halazzi
    cast_kind: summon
    target: ground_random
    effect: stationary totem; AoE Nature damage if alive
    interruptible: false
    priority: KILL_ON_SIGHT
    notes: cleave priority — kill alongside boss/lynx

  - name: Spirit of the Lynx (split add)
    cast_by: halazzi
    cast_kind: scripted (HP trigger 66 / 33)
    target: ground_near_boss
    effect: spawns Spirit of the Lynx add
    interruptible: false

  - name: Lynx Rush
    spell_id: UNVERIFIED                    # Cata addition — possibly absent in 5-man
    cast_by: spirit_of_the_lynx (add)
    cast_kind: instant
    effect: Cata charge mechanic on Spirit; UNVERIFIED whether 5-man

  - name: melee
    cast_by: halazzi
    cast_kind: melee
    target: tank

# Adds
adds:
  - name: Spirit of the Lynx
    count: 1 per split (66 and 33)
    npc_id: 24046                          # UNVERIFIED
    abilities:
      - name: melee
      - name: Frenzy (UNVERIFIED on Spirit)
    kill_priority: HIGH (or cleave with boss; see survey Disagreement #8)
    notes: AoE-burn while continuing Halazzi pressure

  - name: Lightning Totem
    count: 1 (intermittent)
    abilities:
      - name: nature_aoe
    kill_priority: HIGH
    notes: cleave during boss DPS; very low HP

positioning:
  default: |
    Tank pins Halazzi in center; ONE melee shares Saber Lash; other melee
    position back/side. Ranged max range; healer max range.
  tank_facing: away from group (standard)
  los_or_lookaway: none
  saber_lash_partner: exactly ONE melee in tank's melee range to share split

interrupt_priority: []                     # no kick-required hardcasts

dispel_priority:
  - Frenzy (boss buff)                     # SHOULD_DISPEL via Magic
  - Flame Shock (debuff on player)         # SHOULD_DISPEL via Magic

heroic_delta: |
  Heroic-only; consistency note. Lightning Totem hits faster; Spirit HP
  higher; Frenzy dispel timing more critical.

bot_role_summary:
  tank: pin boss; ensure exactly ONE melee shares Saber Lash
  healer: dispel Frenzy / Flame Shock when they appear; tank-heal during splits
  melee: ONE bot is Saber Lash partner; others position to AVOID share. Cleave Spirit + Halazzi or burn Spirit first per AoE-class capability.
  ranged: target-priority swap on Spirit spawns; kill Lightning Totem on sight; dispel-class participates
```

---

## Boss 5 — Hex Lord Malacrass

```yaml
boss: hex_lord_malacrass
npc_id: 24239                              # UNVERIFIED — TBC carryover
location: fifth encounter, after the four-aspect gates open
hp_pct_phase_triggers: [20]                # Spirit Bolts trigger
notes: |
  NON-DETERMINISTIC FIGHT. Hex Lord copies abilities from a RANDOM CLASS
  each pull (e.g., Frost Mage / Shadow Priest / Resto Druid / Affliction
  Warlock — see survey for full table). Bot must DETECT his ability set
  at runtime and switch to the appropriate response profile.

  Always-present mechanics: Soul Drain channel (interruptible), Spirit
  Bolts at sub-20% (AoE damage spike), standard tank-melee.

  4-troll-council adds REMOVED in 5-man heroic per most guides (UNVERIFIED;
  see survey Disagreement #5).

phases:
  - id: 1
    name: regular
    trigger: pull
    end_condition: hp_pct == 20
    description: |
      Tank-and-spank with Soul Drain interrupts + class-set abilities
      (rolled at pull-start). Bot detects ability set in first 30s and
      adopts appropriate response profile (interrupt/dispel/positioning).

  - id: 2
    name: spirit_bolts_burn
    trigger: hp_pct == 20
    end_condition: hp_pct == 0
    description: |
      Spirit Bolts AoE on group; high pressure. Healer pre-shields and
      AoE heals. DPS continues burn; tank uses defensive CDs.

mechanics:
  - name: Soul Drain
    spell_id: 43706                        # UNVERIFIED
    cast_by: hex_lord_malacrass
    cast_kind: channel
    target: ally_random
    duration_ms: 3000
    effect: drains mana AND HP from target
    interruptible: true
    dispel_type: none
    avoidable: false
    priority: MUST_INTERRUPT
    notes: PRIMARY kick target; missed kick = mana drain on target + healing pressure

  - name: Spirit Bolts
    spell_id: 43383                        # UNVERIFIED
    cast_by: hex_lord_malacrass
    cast_kind: cast
    target: pbaoe_self
    cast_time_ms: 3000
    trigger: hp_pct < 20
    effect: AoE Shadow nuke on group
    interruptible: UNVERIFIED               # some sources say interruptible
    dispel_type: none
    avoidable: false
    priority: GROUP_HEAL_TRIGGER

  - name: melee
    cast_by: hex_lord_malacrass
    cast_kind: melee
    target: tank

  # NON-DETERMINISTIC — copied class set
  - name: copied_class_ability_set
    cast_by: hex_lord_malacrass
    cast_kind: VARIABLE
    target: VARIABLE
    effect: |
      Hex Lord rolls a random class at pull-start and casts 2-3 abilities
      from that class's spec rotation throughout the fight. Examples:
        - Frost Mage: Frostbolt (cast, interrupt) + Ice Lance (instant) +
          Frost Nova (instant; root group) + Iceblock (self CD)
        - Shadow Priest: Shadow Word: Pain (DoT, dispel Magic) + Mind
          Blast (cast, interrupt) + Vampiric Touch (DoT, dispel Magic)
        - Holy Priest: Heal (cast, INTERRUPT — high priority)
        - Resto Druid: Healing Touch (cast, interrupt) + Tranquility
          (channel, INTERRUPT)
        - Affliction Warlock: DoTs + Drain Life channel (interrupt) +
          curses (dispel)
        - (full table in survey)
    interruptible: depends_on_class_set
    dispel_type: depends_on_class_set
    priority: REACTIVE_DETECTION
    notes: |
      Bot detects ability-set spell IDs at runtime (first 30s of pull)
      and applies the matching response profile. Default fallback if
      unrecognized: interrupt all hardcasts; dispel Magic on group;
      spread for unknown ground effects.

# Adds
adds: []                                   # 4-troll council REMOVED in 5-man heroic per most sources

positioning:
  default: tank pins central; melee behind boss; ranged max range; healer max range
  tank_facing: away from group (standard)
  los_or_lookaway: none
  spread_for_ground_effects: |
    If Hex Lord's class set includes Mage Flamestrike / Druid Starfall /
    Warlock Shadowfury / etc., ranged spreads (5y+) to mitigate.

interrupt_priority:
  - Soul Drain (boss)                      # ALWAYS — MUST_INTERRUPT
  - class_set_hardcasts                    # depends on roll; high priority for healer-class rolls (Resto Druid Tranquility, Holy Priest Heal, Resto Shaman Healing Wave)

dispel_priority:
  # Reactive — depends on class roll
  - shadow_priest_set: [Shadow Word: Pain, Vampiric Touch]   # Magic dispel
  - affliction_warlock_set: [Curse_of_Agony]                  # Curse dispel
  - disc_priest_set: [Power Word: Shield]                     # Magic dispel (rarely useful to dispel; situational)

heroic_delta: |
  Heroic-only; consistency note. Soul Drain damage higher; Spirit Bolts
  at sub-20% lethal if group not topped; class-set scales with heroic.

bot_role_summary:
  tank: standard pin; defensive CD on Soul Drain if interrupt fails; reactive on class-set
  healer: REACTIVE DISPEL based on detected class set; pre-shield for Spirit Bolts at sub-20%
  melee: HIGHEST kick density of any ZA fight — Soul Drain + class hardcasts
  ranged: HIGH kick density; spread for class-set ground effects

bot_implementation_required: |
  HEX LORD ABILITY-SET DETECTION PASS — bot architecture must support a
  per-encounter override profile populated at runtime from observed casts.
  See healer.md / ranged-dps.md / melee-dps.md / tank.md for per-role
  detection and response logic. This is the most complex fight in ZA for
  bot logic.
```

---

## Boss 6 — Daakara

```yaml
boss: daakara
npc_id: 23863                              # UNVERIFIED — TBC original (Zul'jin); Cata version may have new ID
location: final encounter, throne room atop Zul'jin's altar
hp_pct_phase_triggers: []                  # form changes time-based ~25-30s OR HP-based; UNVERIFIED — see survey Disagreement #6
notes: |
  ASPECT-CYCLE BOSS. Daakara cycles through forms (troll → bear → eagle →
  lynx → dragonhawk) — but most guides report Cata version uses a
  RANDOM SUBSET of 2-3 aspects per pull, not all 4. Form change cadence
  ~25-30s (UNVERIFIED).

  Bot must DETECT form change via boss aura and SWITCH playbook per form.
  Each form has its own response profile; conservative default on form-
  change is face-away + spread + pre-shield until form aura confirms.

phases:
  - id: 1
    name: troll_form
    trigger: pull (always default)
    end_condition: form_swap (~25-30s OR HP threshold; UNVERIFIED)
    description: |
      Default form. Whirlwind Axe AoE + Grievous Throw bleed (must
      interrupt). Standard tank-and-spank with kick rotation.

  - id: 2
    name: variable_form
    trigger: form_swap
    end_condition: form_swap or hp_pct == 0
    description: |
      Daakara assumes one of: bear (Charge + Bear Claw cone), eagle
      (Cyclone + Static Charge ground patches), lynx (Claw Rage burst +
      Lynx Rush multi-target charge), dragonhawk (Flame Whirl + Pillar
      of Fire ground patches).

  - id: cycle
    name: alternating_or_random_subset
    description: form cycle continues until kill (subset varies per pull)

mechanics_troll_form:
  - name: Whirlwind Axe
    spell_id: 24236                        # UNVERIFIED — TBC carryover
    cast_by: daakara (troll form)
    cast_kind: instant
    target: pbaoe_self
    cooldown_ms: 15000
    effect: AoE Physical damage; axes whirl around boss area
    interruptible: false
    dispel_type: none
    avoidable: partial
    avoidance: ranged max-range; melee tank through

  - name: Grievous Throw
    spell_id: 43093                        # UNVERIFIED
    cast_by: daakara (troll form)
    cast_kind: cast
    target: ally_random
    cast_time_ms: 2000
    effect: heavy bleed DoT on target
    interruptible: true
    dispel_type: none                      # Physical bleed
    avoidable: false
    priority: MUST_INTERRUPT
    notes: PRIMARY kick on troll form

mechanics_bear_form:
  - name: Charge (bear)
    spell_id: 24033                        # UNVERIFIED
    cast_by: daakara (bear form)
    cast_kind: instant
    target: ally_random
    effect: charge + knockback + damage
    interruptible: false
    avoidance: spread for charge

  - name: Bear Claw
    spell_id: 24239                        # UNVERIFIED
    cast_by: daakara (bear form)
    cast_kind: melee_cone
    target: frontal_cone
    effect: heavy frontal cone Physical damage
    interruptible: false
    avoidance: face boss away from group

mechanics_eagle_form:
  - name: Cyclone (eagle)
    spell_id: 43661                        # UNVERIFIED — collision with Akil'zon Call Lightning
    cast_by: daakara (eagle form)
    cast_kind: ground_target
    target: ally_random_position
    effect: tornado ground patch; player must move out
    interruptible: false
    avoidance: move out

  - name: Static Charge (eagle)
    spell_id: 43622                        # UNVERIFIED — collision with Akil'zon Static Disruption
    cast_by: daakara (eagle form)
    cast_kind: cast
    target: ally_random
    effect: similar to Akil'zon Static Disruption — targeted Nature ground patch
    interruptible: false
    avoidance: targeted player moves out; everyone spread (3y+)

mechanics_lynx_form:
  - name: Claw Rage
    spell_id: 43149                        # UNVERIFIED
    cast_by: daakara (lynx form)
    cast_kind: melee
    target: tank
    cooldown_ms: 10000
    effect: rapid melee combo; significant burst damage on tank
    interruptible: false
    avoidance: tank major defensive CD on form-transition (Disagreement #9)
    priority: TANK_BURST_DEFENSIVE

  - name: Lynx Rush
    spell_id: 43151                        # UNVERIFIED
    cast_by: daakara (lynx form)
    cast_kind: charge_chain
    target: multiple_allies_in_sequence
    effect: charges multiple targets in sequence — party-wide damage
    interruptible: false
    avoidance: spread

mechanics_dragonhawk_form:
  - name: Flame Whirl
    spell_id: 43213                        # UNVERIFIED
    cast_by: daakara (dragonhawk form)
    cast_kind: ground_patches
    target: ground_around_boss
    effect: drops Fire ground patches
    interruptible: false
    avoidance: spread + move out

  - name: Pillar of Fire
    spell_id: UNVERIFIED                    # Cata addition
    cast_by: daakara (dragonhawk form)
    cast_kind: ground_target
    target: ally_random_position
    effect: targeted Fire pillar; move out
    interruptible: false
    avoidance: move out

  - name: melee
    cast_by: daakara (all forms)
    cast_kind: melee
    target: tank

# Adds
adds: []                                   # no adds confirmed

positioning:
  default: tank pins central; melee behind; ranged max range; healer max range
  tank_facing: away from group (always); rotate facing during form changes
  los_or_lookaway: none
  form_specific:
    troll: standard
    bear: face-away CRITICAL (Bear Claw cone); spread for Charge
    eagle: SPREAD (3y+) for Static Charge ground patches; move out of Cyclone
    lynx: tank takes burst (defensive CD); spread for Lynx Rush chain
    dragonhawk: SPREAD; move out of fire patches

interrupt_priority:
  - Grievous Throw (troll form)            # PRIMARY kick on troll form

dispel_priority: []                        # no Magic dispels confirmed; bleeds are Physical

heroic_delta: |
  Heroic-only; consistency note. Form damage scales up; Lynx Claw Rage
  burst higher; dragonhawk fire patches harder to dodge.

bot_role_summary:
  tank: detect form change via boss aura; rotate facing + defensives per form. MAJOR defensive CD on Lynx-form transition.
  healer: detect form change → switch playbook (eagle pre-spread, lynx burst tank, dragonhawk AoE-fire heals)
  melee: detect form change → reposition (cone in bear, ground patches in eagle/dragonhawk, spread in lynx)
  ranged: detect form change → spread/stack rules per form

bot_implementation_required: |
  DAAKARA FORM-DETECTION SUB-STATE — bot must implement a sub-state per
  form. Form change is detected via visible aura change on the boss
  (bear-form-aura, eagle-form-aura, lynx-form-aura, dragonhawk-form-aura,
  troll-form default). Each form has its own response profile. Conservative
  default on form-change: face-away + spread + pre-shield + reactive
  defensive CD until form aura is confirmed.
```

---

## Instance-wide notes

```yaml
instance_wide:
  difficulty_modes_supported: [heroic]      # NORMAL MODE DOES NOT EXIST
  curse_dispel_required: false              # only situationally on Hex Lord (Affliction Warlock roll)
  disease_dispel_required: false
  poison_dispel_required: false
  magic_dispel_required: true               # Halazzi Frenzy + Flame Shock; some Hex Lord rolls
  dispel_blacklist: []                      # NO global dispel-blacklist (unlike BRC's Crepuscular Veil)
  group_buff_anomaly: none
  required_consumables: standard (flask + food + pots per spec)
  bear_mount_timed_run:
    optional: true
    bot_handling: IGNORE                    # bot does not track timer; clears at own pace
    drop: amani_war_bear (mount)
    timer: ~20-25 minutes (UNVERIFIED — see Disagreement #10)
  hex_lord_runtime_detection: true          # bot must support per-encounter ability-set detection
  daakara_form_state_machine: true          # bot must implement per-form sub-state with aura-based detection
```

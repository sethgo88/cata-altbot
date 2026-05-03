# Grim Batol — Cataclysm 5-man Survey

**Date:** 2026-05-02
**Patch:** 4.3.4
**Audience:** bot-control engineer (cata-altbot)

---

## Overview

| Field | Value |
|---|---|
| Zone | Grim Batol (western edge of Twilight Highlands; entrance reached via Wetlands road around the mountain) |
| Continent | Eastern Kingdoms |
| Level range (normal) | 84-85 (item-level gating: loot req-level 83) |
| Level range (heroic) | 85 (loot req-level 85) |
| Group size | 5 (1 tank / 1 healer / 3 DPS) |
| Boss count | 4 required (no optional bosses) |
| Layout | Linear with one notable mid-zone bombing-run / drake-rescue gimmick on launch trash; in 4.3.4 Cata Classic the bombing-run skip is intact but optional |
| Cata-launch class | Yes — released alongside the original 7 launch 5-mans; gated as a heroic-progression dungeon (not entry-level) |
| Group buffs/debuffs | None unique to instance |

The instance is medium-length (~30-45 min on heroic for a geared group). Trash density is high, with a mid-instance "free the red drakes / bomb the courtyard" sequence that lets a coordinated group skip large packs. The four bosses are progressively more complex: Umbriss (adds + charge avoidance), Throngus (RNG weapon-stance phases), Drahga (Phase-2 Valiona descent + flame elemental kill-priority), Erudax (Shadow Gale safe-zone reversal + egg-corruptor kill-priority).

Grim Batol heroic was widely flagged at Cata launch as "near or at the top of the list in terms of difficulty" (Ten Ton Hammer; Engadget). The Erudax encounter in particular has a hard DPS check on Faceless Corruptors that wipes undergeared groups.

---

## Sources

This survey reconciles six community sources. Wowhead Cata is treated as authoritative for spell IDs and numeric values; strategy is cross-checked across the editorial guides.

| # | Source | URL | Notes |
|---|---|---|---|
| 1 | Wowhead Cata — encounter & spell pages | `wowhead.com/cata/npc=...` and `wowhead.com/cata/spell=...` | Primary source for spell IDs (Umbriss, Throngus, Drahga, Erudax NPC pages all yielded ability lists) |
| 2 | Icy Veins — Grim Batol Dungeon Guide | `icy-veins.com/cataclysm-classic/grim-batol-dungeon-guide` | Editorial; clean per-boss strategy; no spell-ID enumeration |
| 3 | Dexerto — Cata Classic Grim Batol Guide | `dexerto.com/world-of-warcraft/wow-cataclysm-classic-grim-batol-guide-2722508/` | 2024-era Cata-Classic-specific; phase trigger numbers (e.g., Phase 2 at 25% on Drahga) |
| 4 | Ten Ton Hammer — Cataclysm Dungeon Guide: Grim Batol | `tentonhammer.com/articles/cataclysm-dungeon-guide-grim-batol` | Original-Cata-era; bombing-run trash skip detail; heroic difficulty notes |
| 5 | Engadget — Cataclysm Dungeon Guide: Grim Batol (2010-12-08) | `engadget.com/2010-12-08-cataclysm-dungeon-guide-grim-batol.html` | Original-Cata-era; concise per-boss positioning |
| 6 | Glowberry / Glow's Branches — Resto Druid Grim Batol | `glowberry.wordpress.com/dungeon-guides/grim-batol/` | Healer-perspective; details on Throngus weapon-stance phase order and Erudax mana pacing |
| 7 | GuiasWow — Grim Batol Heroic and Normal Guide | `en.guiaswow.com/dungeons/guide-grim-batol-heroic-normal.html` | Tertiary; weapon-stance % numbers and heroic deltas |

Where guides disagreed, the disagreement is captured in **Disagreements** and the consensus position in **Strong consensus**.

Note: warcraft.wiki.gg's Grim Batol page covers lore only, not encounter mechanics, and was not used. Wowpedia 403'd; substituted with the WoWWiki archive for layout context.

---

## Strong consensus (table)

| Mechanic | Consensus |
|---|---|
| Umbriss Bleeding Wound | DoT on tank; 4-5 sources agree the heal-above-90%-HP-removes-debuff mechanic exists in Cata; treat as **stack-on-tank healing pressure** |
| Umbriss Blitz | Charge-on-random-non-tank; group **spreads** so charge doesn't multi-hit; charged player and people-in-path move out |
| Umbriss Ground Siege | Cone/PBAoE physical with stun; movable-out-of |
| Umbriss Frenzy | Triggers at ~30% HP (4/5 sources); some sources say 20% (one source) — see **Disagreement #1** |
| Umbriss Trogg adds | **Malignant Trogg must die away from boss** — Modgud's Malice buffs Umbriss damage by 100% (stacking) if it hits him on death. Kite or CC Malignant; AoE the Trogg Dwellers safely. **High kill-priority misroute risk**. |
| Throngus weapon stances | 3 weapon stances cycled randomly (~30s each per Icy Veins / GuiasWow): Sword, Mace, Shield. Mace is the kite-phase; Shield needs DPS-behind-boss positioning; Sword is highest tank-pressure phase. |
| Throngus Mighty Stomp / Cave In | Mighty Stomp is the "shake the walls" cast → Cave In ground patches drop on random players → move out of patches. Universal mechanic regardless of weapon stance. |
| Throngus Disorienting Roar | Sword-stance ability; -50% haste debuff (per Wowhead spell page); Glowberry classifies as **dispellable** (Magic) — see **Disagreement #2** |
| Drahga Phase 1 → Phase 2 | At ~25% HP (Dexerto, Icy Veins) — but multiple original-Cata-era sources say 50% (Glowberry, Ten Ton Hammer). Cata-Classic-era sources lean 25%. See **Disagreement #3**. |
| Drahga Invoked Flaming Spirit | Spawned by Invocation of Flame portal; **fixates a random player and runs at them**; group must kill it before it reaches target (Supernova on impact). Slows/stuns help. |
| Drahga Phase 2 Valiona | Drahga mounts Valiona at phase-2 trigger; Valiona has frontal Devouring Flames breath (heroic-only per some sources, baseline per others); Seeping Twilight ground patches on random players (move out). **Group must move behind Valiona during breath cast**. |
| Drahga Phase 3 (Valiona departs) | At ~10-20% Valiona HP, dragon flies off; Drahga returns to ground for execute (Phase 1 mechanics resume). 4/6 sources confirm this third phase. |
| Erudax Binding Shadows | Magic-school ranged debuff; **roots target** for ~8s; deals shadow damage and **heals Erudax for 3x the damage dealt** (per Dexerto/Icy Veins). Move out of telegraph. |
| Erudax Enfeebling Blow | Tank-targeted; knocks tank back ~400 yards-units; applies **Feeble Body** debuff (200% damage taken; 5s) — tank waits out before re-engaging. |
| Erudax Shadow Gale | The **safe spot is the eye of the storm** (center circle); damage in the OUTER zone, not the inner zone. **Reversal of normal "fire on floor = avoid" rule.** All players collapse to safe zone. |
| Erudax Faceless Corruptors | Spawn after Shadow Gale; walk toward Alexstrasza's Eggs at room entrance; cast Twilight Corruption on egg → if completed, egg hatches Twilight Hatchlings (additional adds). **Slow + burn corruptors before egg cast completes**. |
| Erudax Faceless Corruptor heroic count | **2 corruptors per Shadow Gale on heroic** (vs 1 on normal) — universal across sources. |
| Erudax Shield of Nightmares | Magic-school self-buff on Faceless Corruptor; reflects/triggers Manifested Nightmare AoE damage when attacked. Standard strat: **dispel** before continuing burn (Disc Priest / Holy Pally / Shaman / Druid). |
| Erudax Umbral Mending | Faceless Corruptor cast; heals nearby allies (including Erudax) for 20% HP. **Interrupt-required**. |
| Heroic Erudax DPS check | Hard DPS check; corruptors must die before reaching eggs OR before egg-corruption cast completes. Multiple sources flag this as the gating fight for heroic completion. |
| Curse/Disease/Poison dispel needs | None instance-wide; Magic dispel needed on **Erudax** (Shield of Nightmares — corruptor-only) and **Throngus** (Disorienting Roar, **UNVERIFIED dispellability** — see Disagreement #2). |

---

## Boss 1 — General Umbriss

NPC ID: 39625. First boss of Grim Batol; large Twilight-tainted Skardyn. Single-phase tank-and-spank with periodic charge avoidance, cone stuns, and trogg add waves where the kill-priority is critical.

### Phase structure

Single phase with ramping mechanics; **Frenzy** at ~30% HP (Disagreement #1) increases attack speed + damage by 50%.

1. Pull → standard tank-and-spank
2. Periodic Blitz (charge) on random player
3. Periodic Ground Siege (cone stun)
4. Periodic Summon Skardyn (1 Malignant Trogg + 3 Trogg Dwellers)
5. At ~30% HP: Frenzy enrage; execute phase

### Abilities

| Spell | ID | Cast / cooldown | School | Notes |
|---|---|---|---|---|
| Bleeding Wound | 74846 | applied on melee | Physical (bleed) | Tank-targeted DoT, 10s duration; community guides report it falls off when target heals above 90% HP (UNVERIFIED Wowhead-confirmed). Heavy healer pressure on tank if not cleansed. |
| Blitz | 74670 | ~3s telegraph cast | Physical | Charges to a random non-tank's location; massive physical damage + knockback within 6 yards. **Move out of telegraph**. |
| Ground Siege | 74634 | 2s cast | Physical | PBAoE / cone (sources disagree — Wowhead spell page lists 20-yard radius PBAoE; community guides describe a directional cone). Stuns 4s. **Movable-out-of**. |
| Frenzy | 96800 | scripted at 30% HP | Physical | Boss enrage; +50% attack speed and +50% damage. Some sources reference dispellability via Tranq Shot / Soothe / Shiv (UNVERIFIED in Cata 4.3.4). |
| Summon Skardyn | 74859 | scripted | — | Spawns 1 Malignant Trogg + 3 Trogg Dweller adds. Periodic. |

#### Add abilities

| Add | NPC ID | Ability | Spell ID | Notes |
|---|---|---|---|---|
| Malignant Trogg | 39984 | Modgud's Malice | 74699 | **The critical add ability.** When the Malignant Trogg dies, casts Modgud's Malice on nearby targets. **If it hits Umbriss, gives Umbriss +100% damage (stacking).** Kite/CC Malignant Trogg AWAY from boss before killing. |
| Malignant Trogg | 39984 | Modgud's Malady | 74837 | Stacking bleed on melee target. |
| Malignant Trogg | 39984 | Claw Puncture | 76507 | Standard add melee. |
| Trogg Dweller | 45467 | Claw Puncture | 76507 | Standard low-HP melee adds. |

### Interrupts required

- **Modgud's Malice** is a death-trigger (cast on Malignant Trogg's death), not a kickable cast. Bot logic: **kill Malignant Trogg far from boss**, not "interrupt the cast."
- No standard hard-cast interrupt requirements on Umbriss himself.

### Dispels required

- Bleeding Wound is **not dispellable** (Physical bleed). Heal through.
- Frenzy enrage may be soothable (Tranquilizing Shot / Soothe Animal / Shiv). UNVERIFIED in Cata 4.3.4 — flag for DBC.

### Avoidable damage

- **Blitz charge path** — affected player + anyone in path moves perpendicular
- **Ground Siege cone/PBAoE** — move out of telegraphed zone
- **Modgud's Malice on Umbriss** — positional discipline (Malignant Trogg dies far from boss)

### Tank-swap requirements

None. Single tank. Bleeding Wound stack pressure does not require tank-swap.

### Heroic delta

- Blitz reportedly **one-shots** undergeared players (Ten Ton Hammer)
- Trogg adds hit harder; bleed stacks more dangerous on melee
- Frenzy more punishing — defensive CD recommended at the 30% trigger
- One source (GuiasWow) describes a heroic "Infested Trogg explodes on death" variant; UNVERIFIED — see **Disagreement #4**

### Add-handling

- **Malignant Trogg**: tank picks up but **does NOT bring to boss**. Ranged DPS / off-tank kites. CC valid (Hex / Polymorph if humanoid-flagged — UNVERIFIED — Banish if Demon-flagged — UNVERIFIED). Kill **away from boss**.
- **Trogg Dwellers**: AoE-burnable safely (no Modgud's Malice on death). Tank can pick all up at boss; standard cleave.

### Healer-pressure spikes

- Bleeding Wound steady tank drain (predictable)
- Blitz on random player → instant hit (reactive heal)
- Frenzy at 30% → ramping tank damage (pre-shield / pre-HoT)

### Key positioning

- **Spread ranged** so Blitz doesn't multi-hit
- Tank in **center**, facing **away from group** (Ground Siege cone discipline)
- Malignant Trogg kited to a corner away from boss

### Bot-relevant role notes

- **Tank**: pick up boss + Trogg Dwellers; do NOT pick up Malignant Trogg if it can be kited by ranged. Mitigate Bleeding Wound stack pressure with active mitigation.
- **Healer**: predictable heavy tank healing; reactive heals on Blitz target.
- **Melee**: stay behind boss; brace for Ground Siege cone (move out at telegraph).
- **Ranged**: spread for Blitz; kite/CC Malignant Trogg.

---

## Boss 2 — Forgemaster Throngus

NPC ID: 40177. Second boss; an ettin smith in an arena chamber. Encounter is structured around a **random weapon-stance rotation** (Pick Weapon ability, ~30s cycle per stance). Each stance changes the fight character.

### Phase structure

Three rotating weapon stances + universal Mighty Stomp / Cave In mechanic regardless of stance.

1. **Sword stance** — high tank damage (Dual Blades + Thrash), Disorienting Roar AoE haste reduction
2. **Mace stance** — boss is slow (-70% movement) but +200-300% damage; Impaling Slam jumps to random; **kite phase** (tank does not stand-tank)
3. **Shield stance** — Personal Phalanx; boss roots and pacifies self; archers/flaming arrows from chamber walls; group **must DPS from behind** the shield direction

Mighty Stomp (74984) → Cave In (74987) ground patches drop throughout, regardless of weapon.

### Abilities

| Spell | ID | Cast / cooldown | School | Notes |
|---|---|---|---|---|
| Mighty Stomp | 74984 | ~2s cast | Physical | Universal AoE; triggers Cave In ground patches at random player locations. |
| Cave In | 74987 | instant ground deploy | Physical | 6-yard radius persistent ground patch; periodic ticks. **Move out**. |
| Disorienting Roar | 74976 | 1.5s cast | Physical | **Sword stance.** AoE -50% haste (melee, ranged, spell). Wowhead lists no dispel type ("n/a"); some guides claim it's removable — see **Disagreement #2**. |
| Dual Blades | 74981 | passive while sword | Physical | Sword stance; melee buff; rapid-strike pattern on tank. |
| Thrash | 47480 | passive while sword | Physical | Sword stance; bonus melee swings on tank. |
| Personal Phalanx | 74908 | applied on shield-pick | Physical | **Shield stance.** Boss roots + pacifies self; triggers Glancing Blows periodic. Group must DPS from BEHIND boss; archers/Flaming Arrows from walls during this phase. |
| Flaming Arrow | 45101 | ranged ground-deploy | Fire | Shield stance; archers fire down at floor in random patches; **avoidable**. |
| Encumbered | 75007 | passive while mace | Physical | Mace stance; -70% movement on boss. |
| Impaling Slam | 75057 | instant | Physical | Mace stance; jumps to random target, deals damage + 5s DoT. **Tank kites** to mitigate. |

#### Pick Weapon trigger

The "Pick Weapon" event is a scripted boss-ability swap (UNVERIFIED spell ID for the trigger itself; weapon-state is observable via debuff/aura on boss). Cycle is ~30s per stance, random order across stances.

### Interrupts required

- No confirmed kick-required hardcasts on Throngus itself.
- Disorienting Roar has a 1.5s cast — **possibly interruptible** (UNVERIFIED). If kickable, very high value (full-group haste cripple).

### Dispels required

- **Disorienting Roar** — Magic dispel **claimed by Glowberry** ("magic DOT requires dispelling"); Wowhead spell page lists dispel-type as n/a. **UNVERIFIED.** See **Disagreement #2**.
- No other dispels required on Throngus.

### Avoidable damage

- **Cave In ground patches** (universal) — move out
- **Flaming Arrow** patches (Shield stance) — move out
- **Lava patches behind boss path** (Mace stance) — some sources describe fire trails left behind boss as he walks; not in Wowhead spell list. UNVERIFIED — see **Disagreement #5**.
- **Impaling Slam landing** (Mace stance) — tank-kite reduces

### Tank-swap requirements

None. Single tank kites Mace phase; tanks normally Sword/Shield phases.

### Heroic delta

- Mace stance damage especially severe; tank cannot stand-tank — must kite continuously
- Disorienting Roar haste-reduction stacking (per GuiasWow heroic note: "Sword applies stacking magic damage")
- Flaming Arrows (Shield phase) hit harder; avoidance precision matters
- One source describes Mace stance leaving fire trails on heroic only (UNVERIFIED — Disagreement #5)

### Add-handling

- No spawned adds during the encounter. Background archers (Shield stance) are stationary chamber elements; they cannot be killed mid-fight in standard strat.

### Healer-pressure spikes

- **Sword stance**: highest steady tank damage — Dual Blades + Thrash burst
- **Mace stance Impaling Slam**: random-player burst damage + DoT — reactive heal on jump-target
- **Shield stance Flaming Arrow**: predictable group-wide ground damage if anyone fails to move

### Key positioning

- **Sword stance**: standard "behind boss" melee; ranged at max range
- **Mace stance**: tank kites in a circle around the chamber; melee follows at ~5-10 yards behind tank; ranged at max range from kite path
- **Shield stance**: **all DPS behind boss** (Phalanx 99% reduction from front); positional discipline enforced

### Bot-relevant role notes

- **Tank**: detect weapon stance via aura on boss; switch behavior:
  - Sword: face away, active mitigation rotation
  - Mace: kite in pre-planned arc; do NOT stand-tank
  - Shield: hold boss still (he roots himself), face shield away from DPS group
- **Healer**: heaviest mana drain in Sword stance; recover during Shield stance (boss is rooted; positioning heal-friendly)
- **Melee**: position discipline per stance; ALWAYS move out of Cave In patches
- **Ranged**: kite-friendly; cone-aware in Sword phase Disorienting Roar pulse

---

## Boss 3 — Drahga Shadowburner

NPC ID: 40319. Third boss; a Twilight's Hammer caster astride a perch overlooking the chamber. **Two-phase fight** (some sources call it three-phase), with a major mid-fight transition where Valiona descends and engages directly.

### Phase structure

| Phase | Trigger | End | Description |
|---|---|---|---|
| Phase 1 | pull | Drahga HP == 25% (consensus; UNVERIFIED — see Disagreement #3) | Drahga ground-cast Burning Shadowbolts; Invocation of Flame summons Invoked Flaming Spirit adds |
| Phase 2 | Drahga HP == 25% | Valiona departs (~Valiona HP == 20%) | Drahga **mounts Valiona**; Valiona engages directly with Devouring Flames cone breath, Seeping Twilight ground patches, Shredding Swipe melee cleave |
| Phase 3 (execute) | Valiona departs | Drahga HP == 0 | Drahga back on ground; Phase 1 mechanics resume; finish off |

### Abilities

| Spell | ID | Cast / cooldown | School | Notes |
|---|---|---|---|---|
| Burning Shadowbolt | 75245 | 1.5s cast | Fire+Shadow | Drahga's main attack; ranged single-target. **Possibly interruptible** (UNVERIFIED — kickable in standard strat by some guides). |
| Invocation of Flame | 75218 | instant (10s buff) | Physical | Drahga summons an Invoked Flaming Spirit at a portal location. |
| Supernova | 75238 | scripted on impact | Fire | Cast by **Invoked Flaming Spirit** when it reaches its fixated target — massive AoE / one-shot if not dead first. |
| Shredding Swipe | 75271 | melee cleave | Physical | **Valiona only (Phase 2)**; frontal cone melee on tank + anyone in cone. |
| Seeping Twilight | 75317 | ground-deploy | Shadow | **Valiona only (Phase 2)**; ground patches on random player; 6-9k Shadow damage + 50% slow; 1.25s pulse; **move out**. |
| Devouring Flames | UNVERIFIED | cast (~2-4s) | Fire | **Valiona only.** Frontal cone breath; sources call it "Devouring Flame" or "Call Shadow"; described as 180-degree breath; group must MOVE BEHIND Valiona during cast. Heroic-only per some sources. See **Disagreement #6**. |
| Valiona melee | — | melee | Physical | Standard tank melee while Valiona is tanked in Phase 2. |

#### Add: Invoked Flaming Spirit

| Field | Value |
|---|---|
| Spawn trigger | Invocation of Flame portal |
| Behavior | Fixates a random player; runs at them at high speed |
| Death trigger | If reaches target → Supernova (75238) → mass damage / one-shot |
| Counter | Slows + stuns + burst DPS; group switches off boss to kill spirit |
| Heroic | Spirit reportedly hits significantly harder; consensus is **drop-everything-and-kill** on heroic |

### Interrupts required

- **Burning Shadowbolt** (75245) — UNVERIFIED interruptibility. Worth attempting; not load-bearing if missed.
- **Devouring Flames / Devouring Flame** (Phase 2) — UNVERIFIED interruptibility. Most sources describe positioning (behind Valiona) as the counter, not interrupting.
- **Shredding Swipe** — melee cleave, not a cast. Not interruptible.

### Dispels required

- None confirmed on Drahga or Valiona.
- Seeping Twilight applies a 50% movement-speed slow — UNVERIFIED dispellability (likely Magic but not load-bearing; players walk out of patch instead).

### Avoidable damage

- **Invocation of Flame portal** — fire damage near portal
- **Invoked Flaming Spirit Supernova** — only if spirit reaches its target
- **Seeping Twilight ground patch** — move out
- **Devouring Flames cone** — move BEHIND Valiona
- **Shredding Swipe cone** — face Valiona away from group

### Tank-swap requirements

None. Single tank handles Drahga in P1; same tank picks up Valiona at phase transition.

### Heroic delta

- Invoked Flaming Spirits hit MUCH harder — guaranteed one-shot if they reach target
- Valiona's Devouring Flames may only exist on heroic (some sources call it baseline; Disagreement #6)
- More frequent Invocation of Flame casts during P2 (Valiona) — multiple spirits at once

### Add-handling

- **Invoked Flaming Spirit**: **drop everything**, switch to spirit, slow + burst kill before fixate-target dies
- Spirit fixates one player; that player kites if necessary while group bursts

### Healer-pressure spikes

- Phase 2 Devouring Flames if anyone caught in cone
- Tank damage from Valiona in Phase 2 (Shredding Swipe + melee)
- Spirit-fixation damage if spirit gets close to its target

### Key positioning

- **Phase 1**: standard tank-and-spank; ranged spread for Invocation of Flame portal RNG
- **Phase 2 (Valiona)**:
  - Tank holds Valiona FACING AWAY from group (Shredding Swipe cone)
  - Group repositions BEHIND Valiona on Devouring Flames telegraph
  - Group spreads from Seeping Twilight ground patches
- **Phase 3**: revert to Phase 1 positioning

### Bot-relevant role notes

- **Tank**: standard tank in P1; in P2, position Valiona where group can run behind on breath cast. **Pre-plan a "behind-Valiona" anchor point**.
- **Healer**: predictable damage spike on Devouring Flames if positioning is off; heavy reactive on spirit-impact.
- **Melee**: in P2, move behind Valiona on Devouring Flames cast; otherwise standard rear-arc DPS.
- **Ranged**: switch fire to Invoked Flaming Spirit on spawn; resume boss DPS once spirit dies. **Highest priority kill-priority swap** in the fight.

---

## Boss 4 — Erudax, the Duke of Below

NPC ID: 40484. Final boss; a Faceless / Old God minion in the rookery chamber filled with red dragon eggs. **Most complex 5-man encounter at Cata launch**.

### Phase structure

Single phase but with a recurring **Shadow Gale → Faceless Corruptor add wave** cycle. Boss does not have HP-gated phases; mechanics rotate on cooldown.

1. Pull → tank-and-spank with **Enfeebling Blow** tank knockback + **Binding Shadows** ranged drain
2. Periodic **Shadow Gale** — center safe zone; group collapses; whole-group damage
3. After Shadow Gale: **Faceless Corruptor** spawns at room entrance (1 normal / 2 heroic), walks toward Alexstrasza's Eggs
4. Corruptor casts **Twilight Corruption** on egg → if completed, egg hatches **Twilight Hatchlings**
5. Burn corruptor before egg cast completes; if egg already corrupted, kill hatchlings; **interrupt Umbral Mending** (corruptor heal cast)
6. Resume Erudax DPS; cycle repeats every ~25-40s

### Abilities

| Spell | ID | Cast / cooldown | School | Notes |
|---|---|---|---|---|
| Binding Shadows | 79466 | 1.5s cast | Shadow (Magic dispel) | Targets a random player; deals shadow damage + roots ~8s. **Heals Erudax for 3x damage dealt** (per community guides). Move out of telegraph; some sources flag dispellable (Magic). |
| Enfeebling Blow | 75789 | instant | Shadow | Tank-targeted melee; **knocks tank back** (~400 distance unit per Wowhead); applies Feeble Body. |
| Feeble Body | 75792 | applied via Enfeebling Blow | Shadow | **+200% damage taken** for 5s on the tank. Tank waits out before re-engaging; healer pre-shields if available. |
| Shadow Gale | 75694 | ~5s telegraph (community guides) / 1.25s on spell page | Shadow | Whole-room AoE; **safe zone is the EYE / center circle**. Reversal of normal "fire = avoid". All players collapse to center for cast duration. |

#### Adds — Faceless Corruptor (NPC 48844)

Spawn after each Shadow Gale at the room entrance; walk toward Alexstrasza's Eggs.

| Spell | ID | Cast / cooldown | School | Notes |
|---|---|---|---|---|
| Twilight Corruption | 75520 | channeled (~6s) | Physical (channel) | Cast on Alexstrasza's Egg; if completes, egg hatches 2 Twilight Hatchlings. **Slow + burst corruptor before completion**. |
| Shield of Nightmares | 75809 | self-buff | Magic | Corruptor self-applies; reflects Manifested Nightmare AoE damage to attackers. **Magic dispel** removes (Disc Priest, Holy Pally, Shaman, Druid). |
| Umbral Mending | 75763 | 2.5s cast | Shadow | Corruptor cast; heals Erudax + nearby allies for 20% HP (per Icy Veins/Dexerto). **MUST INTERRUPT.** |
| Siphon Essence | 75755 | scripted | Shadow | Drains essence from Alexstrasza's Eggs; secondary Corruptor mechanic. UNVERIFIED in Cata 4.3.4 strat. |
| melee | — | — | Physical | Standard add melee. |

Note: spell ID 75763 (Umbral Mending) collides with Karsh Cinderbreath's UNVERIFIED ID in BRC research. Resolve at DBC time.

#### Adds — Twilight Hatchling (NPC 39388)

Spawn from corrupted eggs (only if Twilight Corruption completes).

| Spell | ID | Notes |
|---|---|---|
| Twilight Blast | 99393 | Hatchling ranged attack; AoE damage to group. |

### Interrupts required

- **Umbral Mending (75763)** — Faceless Corruptor heal-cast. **MUST INTERRUPT.** High priority; bot interrupt rotation must engage on this cast.
- **Twilight Corruption (75520)** — Corruptor channel on egg. Ideally **prevent the cast from starting** by killing/slowing corruptor before reaching egg, but if started, **interrupt to prevent egg hatch** (UNVERIFIED if interrupt cancels channel).
- **Binding Shadows (79466)** — Erudax cast. UNVERIFIED interruptibility; standard strat is to move out, not kick.

### Dispels required

- **Shield of Nightmares (75809)** on Faceless Corruptor — **Magic dispel**. Removes the reflect/AoE buff so DPS can safely burn the corruptor.
- **Binding Shadows (79466)** on player — **Magic** per Wowhead spell page; root + drain. Some sources advocate dispel; standard strat is to step out of telegraph (root resolves on duration). **Treat as optional dispel** — bot does not auto-dispel by default; reactive only if root cripples movement.

### Avoidable damage

- **Binding Shadows telegraph** — move out
- **Shadow Gale outer zone** — collapse to center (REVERSAL of normal rule)
- **Twilight Hatchling AoE** — move away (if hatchlings spawned)

### Tank-swap requirements

None mandated. Feeble Body (200% damage taken) makes a tank-swap **theoretically helpful** but not required in 5-man composition. Tank simply waits 5s after being knocked back before re-engaging.

### Heroic delta

- **2 Faceless Corruptors per Shadow Gale** instead of 1 — DPS check tightens
- Shadow Gale damage higher — late entry to safe zone is fatal
- Enfeebling Blow knockback farther / more frequent
- Heroic-only achievement: "Don't Need to Break Eggs to Make an Omelet" — defeat without any Faceless Corruptor reaching Twilight Corruption cast

### Add-handling

- **Faceless Corruptor** (1 normal / 2 heroic): **drop everything**; slow + burn before reaching egg; dispel Shield of Nightmares; interrupt Umbral Mending
- **Twilight Hatchlings** (only if egg corrupted): AoE-burnable; secondary kill priority

### Healer-pressure spikes

- **Shadow Gale** — whole-group damage during cast; pre-AoE-heal CD aligned to the cast
- **Enfeebling Blow + Feeble Body** — tank takes +200% damage for 5s; pre-shield + reactive heal
- **Twilight Hatchling Twilight Blast** — group AoE if hatchlings spawn (preventable)

### Key positioning

- **Default**: tank holds Erudax in CENTER OF ROOM; ranged spread; melee behind boss
- **Shadow Gale telegraph**: ALL players collapse to center safe-zone circle
- **Faceless Corruptor spawn**: tank may pull off Erudax briefly OR ranged DPS handles corruptor independently while tank stays on boss
- **Eggs are at the room entrance / outer ring** — group must intercept corruptor before it reaches that ring

### Bot-relevant role notes

- **Tank**: hold Erudax central; absorb Enfeebling Blow knockback; wait 5s on Feeble Body debuff before re-engaging. On Faceless Corruptor spawn, **do not chase** — corruptor is DPS-handled.
- **Healer**: pre-AoE-heal CD on Shadow Gale telegraph; **do NOT auto-dispel Binding Shadows** (default — let it resolve); **DO dispel Shield of Nightmares** on corruptor.
- **Melee**: standard rear-arc; intercept corruptor on spawn (gap-close + slow); **interrupt Umbral Mending**.
- **Ranged**: spread for Binding Shadows; collapse to center on Shadow Gale; switch to corruptor on spawn; slow corruptor (Frost Mage Frostbolt slow, Hunter Concussive, Aff Warlock Curse of Exhaustion).

---

## Trash mechanics

The path through Grim Batol from start to Erudax includes the **bombing-run sequence** (drake-rescue) that lets coordinated groups skip large packs. Trash density is high; mob types vary by zone segment.

### Trash before Umbriss

- **Twilight Cult** humanoids — caster + melee; **Polymorph / Hex / Sap** valid on humanoid casters
- **Skardyn (Trogg)** packs — same model as Umbriss adds; melee with bleeds
- **Drakonids** at courtyard chokes — frontal cleaves; tank face-away

### Bombing-run sequence (mid-instance)

After Umbriss, the path opens into a courtyard with chained red drakes. Players free the drakes; group can mount and bomb large trash packs from above. **In Cata Classic 4.3.4 this skip is intact but optional**; many speedclear groups use it.

- **Bombing skip blacklists**: courtyard trogg packs (skippable)
- Engagement still required for Drahga-approach packs

### Trash before Throngus

- **Twilight cultists + Drakonid pairs** — caster CC valid
- **Twilight Demolisher** large mob with frontal cleave — tank face-away

### Trash before Drahga

- **Drakonid** packs in approach hallway — standard cleave
- **Twilight Beguilers** caster — Mind Control / Charm-cast (UNVERIFIED) — **interrupt-priority**
- **Captured / freed Red Drakes** — friendly NPCs; do not engage

### Trash before Erudax (rookery approach)

- **Faceless** humanoids — dark-magic casters; **interrupt-priority** on hardcasts
- **Mature Twilight Drake** — large flying NPC pull; cleave + tail swipe
- **Ambient Twilight Hatchlings** — low-HP eggs of the same type that spawn during Erudax encounter; AoE-burnable
- **Note**: the Rookery has **dragon eggs** as ambient environmental objects. These are NOT the same as Erudax-fight Alexstrasza's Eggs — those are encounter-specific.

### CC priorities

- **Polymorph / Hex / Sap / Banish** valid on Twilight humanoid casters
- **Hibernate** (Druid) valid on Twilight Drakes
- **Freezing Trap** (Hunter) valid on most non-elementals
- Skardyn / Trogg / Drakonid melee adds — generally not standard CC targets

### Skip-able trash

- **Bombing-run sequence** (post-Umbriss courtyard) — skippable via drake bombing
- Some packs along the rookery approach — wall-hug viable for organized groups

### Pull markers for CC

- Mixed humanoid+melee packs: skull on lead caster, cross/X on second caster, square on tertiary
- Drakonid packs: skull (kill order), no CC needed (immune to most CC)

---

## Disagreements

Numbered list of points where the reconciled guides do not agree. Bot logic should pick a default and note the deviation.

### Disagreement #1 — Umbriss Frenzy HP threshold

- **Source A (Wowhead spell page 96800):** triggers at "30% health" per encounter journal
- **Source B (Glowberry):** "20% Frenzy"
- **Source C (Ten Ton Hammer / Engadget):** "20% health"
- **Source D (Dexerto / Icy Veins / GuiasWow):** ~30% consensus

**Default:** **30%** (Wowhead-confirmed via encounter journal). Pop tank defensive at 35% HP threshold.

### Disagreement #2 — Throngus Disorienting Roar dispellability

- **Source A (Glowberry):** "magic DOT requires dispelling" — implicitly Magic dispel
- **Source B (Wowhead spell page 74976):** dispel-type "n/a"
- **Source C (Icy Veins / Dexerto):** doesn't mention dispel; lists as just an AoE haste reduction
- **Source D (GuiasWow):** does not flag as dispellable

**Default:** **Treat as non-dispellable** (Wowhead authoritative). Heal through. Flag for DBC verification — if Magic dispel is confirmed, healer dispels on cast-finish.

### Disagreement #3 — Drahga Phase 2 trigger HP%

- **Source A (Dexerto / Icy Veins):** 25% HP — Cata Classic-era sources
- **Source B (Glowberry / Ten Ton Hammer / Engadget):** 50% HP — original-Cata-era sources
- **Source C (GuiasWow):** 30%

**Default:** **25%** (Cata Classic-era is authoritative for our 4.3.4 target patch). Bot Phase 2 detection trigger at Drahga HP <= 25%.

### Disagreement #4 — Umbriss heroic Infested Trogg explosion

- **Source A (GuiasWow heroic delta):** "Infested Troggs explode on death, potentially enraging the boss"
- **Source B (Wowhead encounter journal):** does NOT list an Infested Trogg variant; only Malignant Trogg + Trogg Dweller
- **Source C (Icy Veins / Dexerto):** does not mention an Infested variant

**Default:** **Treat as not-present in Cata Classic 4.3.4.** GuiasWow may be referencing an original-launch variant later removed. Bot logic: treat ALL Trogg adds the same way (Malignant kited, Dwellers AoE'd safely). Flag for DBC.

### Disagreement #5 — Throngus Mace-stance lava trail

- **Source A (Icy Veins / Glowberry):** "Footsteps leave fire pools" / "Lava Patch" along boss kite path
- **Source B (Wowhead encounter journal):** does NOT list a Lava Patch ability for Throngus
- **Source C (Dexerto):** no mention

**Default:** **Cone-of-acceptance — assume lava patches exist** (multiple sources mention; Wowhead ability lists are sometimes incomplete for environment-spawned ground hazards). Bot: avoid all visible fire on floor during Mace stance, regardless of Wowhead listing. Flag for DBC.

### Disagreement #6 — Drahga Phase 2 Devouring Flames cone

- **Source A (Dexerto):** "Devouring Flames... cast in a random direction, severely damaging all enemies in front of her every second"
- **Source B (Glowberry / Engadget):** describes it as "Call Shadow" / "180-degree breath" — same mechanic, different name
- **Source C (Ten Ton Hammer):** "flame breath (4-second stun in frontal arc)"
- **Source D (GuiasWow):** describes only on heroic difficulty
- **Source E (Wowhead Drahga NPC page):** does NOT list Devouring Flames as a Drahga or Valiona ability — only Shredding Swipe (75271) and Seeping Twilight (75317)

**Default:** **Treat as a real Phase-2 mechanic** (5/6 sources confirm). Spell ID UNVERIFIED — flag for DBC. Bot positioning rule: on detection of Valiona breath cast, all DPS/healer move BEHIND Valiona; rule applies regardless of difficulty (conservative).

### Disagreement #7 — Erudax Binding Shadows dispel handling

- **Source A (GuiasWow):** "requires dispelling"
- **Source B (Icy Veins / Dexerto):** "simple counter: move out of targeted area"
- **Source C (Wowhead spell page 79466):** Magic-school; dispellable in principle

**Default:** **Position-out-of (move) is the primary counter; dispel is reactive only** if a player is rooted in a bad spot AND root duration exceeds healer tolerance. Bot does NOT auto-dispel Binding Shadows — this avoids dispel-spam wasting mana on a self-resolving 8s root.

### Disagreement #8 — Erudax Faceless Corruptor heal — Umbral Mending vs Siphon Essence

- **Source A (Icy Veins):** "Umbral Mending (restores 20% boss health per cast)"
- **Source B (Dexerto):** does not name the spell explicitly
- **Source C (Wowhead Erudax NPC page):** lists BOTH Umbral Mending (75763) and Siphon Essence (75755) on Faceless Corruptor

**Default:** **Both casts are interruptible and high-priority.** Umbral Mending = heals; Siphon Essence = drains eggs (worsens add wave). Bot interrupt priority: Umbral Mending > Siphon Essence > all other corruptor casts.

### Disagreement #9 — Bombing-run trash skip availability in Cata Classic 4.3.4

- **Source A (Ten Ton Hammer / Engadget):** "almost completely clear the trash to the first boss" via drake bombing — original launch
- **Source B (Icy Veins Cata Classic guide):** does not mention bombing-run skip
- **Source C (Glowberry):** does not mention

**Default:** **Bombing-run is intact in 4.3.4** (gameplay mechanic is in the dungeon's scripts; Icy Veins omitting it is editorial choice). Bot: do NOT assume skip; clear trash linearly. Skip is a player-decision optimization, not a bot behavior.

---

## DBC verification (UNVERIFIED items)

Spell IDs and stack values listed below need confirmation against TC's `Spell.dbc`. Add to `docs/research/dbc-verification-checklist.md` for resolution when the TC fork session opens.

| # | Spell / mechanic | Listed value | Why it matters for bot logic |
|---|---|---|---|
| 1 | Umbriss Bleeding Wound 90%-HP-removes-debuff | community-source claim | Critical for healer pacing — confirm whether mechanic exists or is misremembered Wrath-era ability |
| 2 | Umbriss Frenzy threshold | 30% (Wowhead) / 20% (some guides) | Tank defensive CD pacing |
| 3 | Umbriss Modgud's Malice radius | UNVERIFIED yards | Determines how far from boss Malignant Trogg must be killed |
| 4 | Throngus Disorienting Roar dispellability | n/a per Wowhead, Magic per Glowberry | Healer dispel logic toggle |
| 5 | Throngus Pick Weapon trigger ID | UNVERIFIED | Aura-detection signal for stance phase |
| 6 | Throngus Mace-stance lava trails | UNVERIFIED — community-only | Fire-on-floor avoid rule for Mace phase |
| 7 | Throngus Disorienting Roar interruptibility | 1.5s cast — UNVERIFIED | Bot interrupt rotation participation |
| 8 | Drahga Phase 2 trigger HP% | 25% (Cata Classic) vs 50% (original) | Phase detection logic |
| 9 | Drahga Burning Shadowbolt interruptibility | UNVERIFIED | Kick rotation participation |
| 10 | Drahga / Valiona Devouring Flames spell ID | UNVERIFIED — not on Wowhead NPC page | Cast detection for breath-positioning behavior |
| 11 | Drahga Devouring Flames heroic-only flag | UNVERIFIED — sources disagree | Bot difficulty-conditional logic |
| 12 | Erudax Binding Shadows dispel-handling | community-disagreement | Auto-dispel toggle |
| 13 | Erudax Shadow Gale cast duration | 1.25s on spell page / ~5s telegraph in guides | Pre-position window |
| 14 | Erudax Shadow Gale safe-zone radius | UNVERIFIED yards | Bot collapse-target-position |
| 15 | Faceless Corruptor Umbral Mending interrupt | confirmed cast 2.5s; interruptibility flag | Kick rotation must include this |
| 16 | Faceless Corruptor Twilight Corruption interrupt | UNVERIFIED — channel may not be kickable | Kill-priority vs kick choice |
| 17 | Faceless Corruptor Shield of Nightmares dispel | Magic per Wowhead spell page | Healer dispel rotation |
| 18 | Twilight Hatchling Twilight Blast cast time | UNVERIFIED | AoE-positioning if hatchlings spawn |
| 19 | Heroic Erudax 2-Corruptor confirmation | universal community claim | DPS-check sizing |
| 20 | Heroic Umbriss Infested Trogg variant | GuiasWow only — not on Wowhead | Add-handling pattern divergence |
| 21 | Modgud's Malice spell ID 74699 | Wowhead self-aura; community-listed | The "no AoE near boss" rule depends on this debuff hitting boss |

---

## Notes on prompt deviations

- **Skipped spec selection** as instructed — no spec recommendation section.
- **Heroic delta is per-boss** rather than a separate top-level section (matches BRC + Throne of the Tides format).
- **No optional bosses** in Grim Batol — unlike BRC's Beauty. The 4 listed are all required.
- **Bombing-run trash skip** captured under Trash section, not as a separate boss/mechanic. Default bot behavior: linear clear (skip is player-coordinated, not bot-automatable for v1).
- **Many spell IDs are UNVERIFIED** — Grim Batol shares several Wowhead Cata page incompletenesses with BRC (especially around environment-spawned ground hazards and "telegraph" cast variants). DBC verification is recommended before referencing any of items #6, #10, #14, #16, #18 as compile-time constants.
- **Drahga Phase 2 trigger %** is the most consequential disagreement (25% vs 50%); default to 25% per Cata Classic-era sources, but bot phase detection should ALSO listen for the Valiona-summon scripted event rather than relying purely on HP threshold.
- **Erudax Faceless Corruptor add wave** is the gating mechanic for the entire dungeon's heroic completion. Bot interrupt + dispel + kill-priority swap discipline on this fight is the highest-value coding effort across the bundle.
- Wowhead Cata is the spell-ID source of record; where it disagreed with Icy Veins / Dexerto / community-blogger sources, Wowhead won. Where Wowhead was silent, the disagreement was logged.

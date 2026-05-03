# Zul'Aman — Cataclysm 5-man Heroic Survey

**Date:** 2026-05-02
**Patch:** 4.3.4 (instance reintroduced as a 5-man heroic in 4.1)
**Audience:** bot-control engineer (cata-altbot)

---

## Overview

| Field | Value |
|---|---|
| Zone | Zul'Aman (Ghostlands, eastern Eastern Kingdoms) |
| Continent | Eastern Kingdoms |
| Original release | TBC patch 2.3.0 — 10-man raid |
| Cata revamp | Patch 4.1 — re-tuned as a 5-man heroic |
| Difficulty modes | **Heroic only** — no normal-mode version exists |
| Required level | 85 |
| Group size | 5 (1 tank / 1 healer / 3 DPS) |
| Boss count | 6 (4 animal-aspect + 2 troll bosses) |
| Layout | Mostly outdoor; gates unlock on boss kills |
| Gating | Standard Cata heroic (item level entry req); Dungeon Finder available |
| Group buffs/debuffs | None unique to instance |
| Optional achievement | Bear-mount timed run — kill all 4 animal aspects within ~20 minutes (Amani War Bear drop) |

The instance is medium-length (~40-60 min on first clears, ~25-35 min on geared/farm groups). It is **one of the harder Cata 5-man heroics** alongside Zul'Gurub: heavy interrupt pressure, multiple add-spawn fights, dispel calls on several encounters, and significant tank-swap and target-priority demands. Healer mana is tested on Jan'alai (the egg-hatching event drains AoE healing) and on Hex Lord (random class abilities can include healing-pressure mechanics).

The bear-mount timed run is **ignored by bot logic** — the bot does not track timers and clears at its own pace.

---

## Sources

This survey reconciles the following community sources. Wowhead Cata is treated as authoritative for spell IDs and numeric values; strategy is cross-checked across 5 guides.

1. **Wowhead Cata — encounter & NPC pages** (`/cata/npc=...`, `/cata/spell=...`) — primary for spell IDs, cast times, cooldowns, schools, debuff durations, hatcher add IDs
2. **Icy Veins — Zul'Aman Dungeon Strategy Guide** (`icy-veins.com/cataclysm-classic/zul-aman-dungeon-strategy-guide`)
3. **Warcraft Tavern — Zul'Aman Guide** (`warcrafttavern.com/cataclysm-classic/guides/zul-aman/`)
4. **Wowhead Cata Guide — Zul'Aman walkthrough** (community guide attached to the zone page)
5. **Warcraft Wiki — Zul'Aman** (`warcraft.wiki.gg/wiki/Zul'Aman`) — structural reference (boss list, NPC IDs, ability rosters, layout)
6. **wowtbc.gg — Zul'Aman strat notes** (cross-reference for heroic delta + Hex Lord ability table)
7. **web.archive.org snapshots** — original 4.1-era blog posts (Sacred Duty, Manalicious archives) for Hex Lord ability rotation specifics

Where guides disagree, the disagreement is captured in the **Disagreements** section and the consensus position is listed in **Strong consensus**.

---

## Strong consensus (table)

| Mechanic | Consensus |
|---|---|
| Heroic-only | No normal mode exists. All bot logic for ZA is heroic-mode logic. |
| Akil'zon Static Disruption | Targeted **ground-patch** that requires the targeted player to **move out** of the group (3-target spread); patch persists and damages standers. |
| Akil'zon Eagle Storm (Electrical Storm) | Group **stacks tightly** under one player who lifts everyone into the air (storm cloud) so the lightning damage is **shared/mitigated**. Stack is opposite of Static Disruption — bot must distinguish the two. |
| Akil'zon Soaring Eagles adds | Periodic spawns; **interrupt their cast** + AoE burn before they reach group. |
| Nalorakk form swap | Bear ↔ troll form alternation; troll-form casts are **interruptible** (Mojo Volley), bear-form is melee + Brutal Swipe cone + Mangle bleed. **No tank swap** in 5-man (single tank rides through Mangle stacks; defensive CDs on rotation). |
| Nalorakk Mangle bleed | Stacking bleed on tank in bear form; healer pre-emptively HoTs; stacks reset on troll-form swap. |
| Nalorakk Brutal Swipe | Frontal cone in bear form — face boss away from group. |
| Jan'alai egg phase | At 35% boss HP (and earlier "Hatcher" sub-events), eggs hatch into many small dragonhawk hatchlings — heavy AoE-burn phase. Two **Hatcher** trolls patrol the egg piles and must be killed/CC'd to prevent rolling waves. |
| Jan'alai Flame Breath | Cone breath; non-tank avoid arc. |
| Jan'alai Fire Bombs | Ground patches dropped during fight; stand-out-of. |
| Halazzi Phase 1 (lion form) | Saber Lash split-damage on **two players in melee** (tank and one melee); standard tank-and-spank otherwise. |
| Halazzi Phase 2 (split form) | At 66% and 33%, Halazzi splits into **Halazzi + Spirit of the Lynx**. Lynx adds appear; group AoE-burns lynx adds while still pressuring Halazzi. |
| Halazzi Lightning Totem | Stationary totem; **kill priority** alongside the boss. |
| Hex Lord Malacrass random abilities | Hex Lord copies **two abilities from a random class each pull** (e.g., Frost Mage spells + Shadow Priest spells). The fight is **non-deterministic** — bot must reactively detect which abilities he has and respond. |
| Hex Lord Soul Drain | Channel that drains mana/life from a player; **interruptible**. |
| Hex Lord Spirit Bolts | Heavy AoE damage at sub-20% (final-burn pressure mechanic). |
| Hex Lord 4 troll adds | Removed from the 5-man version per most guides — see **Disagreements #5** |
| Daakara/Zul'jin form-cycle | Final boss cycles through aspect forms (troll → bear → eagle → lynx → dragonhawk); each form has distinct mechanics; bot must detect form change and switch playbook. |
| Daakara form-change interval | Every ~25-30 seconds (UNVERIFIED) or HP-percentage-driven (UNVERIFIED) — see **Disagreements #6** |
| Daakara troll form (default) | Cast Whirlwind Axe + Grievous Throw bleed; **MUST interrupt** Throw cast. |
| Bear-mount timed run | Optional; bot ignores. |
| Heavy interrupt pressure | Mojo Volley (Nalorakk troll), Soul Drain (Hex Lord), Mojo of Mokrosh (some Hex Lord ability sets), various trash casts — interrupt rotation is mandatory. |
| Dispel pressure | Several encounters require Magic dispel (Static Disruption flavor effects, some Hex Lord ability sets). Curse / Disease / Poison dispels generally not required. |

---

## Boss 1 — Akil'zon (Eagle Aspect)

NPC ID: 23574 (Cata recycle of TBC NPC). First boss, atop the eagle gate platform.

### Phase structure

Single phase, on a recurring cycle:

1. Tank-and-spank with **Static Disruption** (targeted ground spread) on cooldown
2. **Soaring Eagle adds** spawn periodically (small flying adds; AoE-burn)
3. **Eagle Storm / Electrical Storm** — group-stack mechanic; everyone gathers under one player and gets lifted into a storm cloud where damage is shared/mitigated

### Abilities

| Spell | ID | Cast / cooldown | School | Notes |
|---|---|---|---|---|
| Static Disruption | 43622 (UNVERIFIED — original TBC 43511 reused; see DBC) | ~1.5s cast, ~10-15s cd | Nature | Targets a random player; on land deals AoE Nature damage AND drops a **persistent ground patch**. Targeted player must **move out of group**. Patch lasts ~10s. |
| Electrical Storm / Eagle Storm | 43648 (UNVERIFIED) | ~3-5s telegraph; channel | Nature | Selects one player to be lifted into a storm cloud; nearby players are **also lifted** if stacked. Damage is **shared among lifted players** — group MUST stack tight to mitigate. Solo'd Storm is ~lethal. |
| Call Lightning | 43661 (UNVERIFIED) | ~1.5s cast, ~12s cd | Nature | Single-target Nature damage cast — interrupt-priority on heroic. |
| Soaring Eagles (add summon) | 43654 (UNVERIFIED summon) | scripted spawn | — | Small adds spawn from the platform; melee/charge group. AoE burn priority. |
| melee | — | — | Physical | Standard tank-melee |

### Interrupts required

- **Call Lightning** — primary kick target on heroic.
- Soaring Eagles spawn-ability (if interruptible — UNVERIFIED).

### Dispels required

None canonical. Some guides claim Static Disruption dot-flavor is dispellable; see **Disagreements #1**.

### Avoidable damage

- Static Disruption ground patches (move out)
- Standing apart during Eagle Storm (must stack)

### Tank-swap requirements

None. Single tank.

### Heroic delta

- Static Disruption tick damage higher; missing the move-out is healer-pressure
- Eagle Storm lethal if a player isn't stacked
- Add-burn pressure tighter — failed AoE = adds reach healer

### Add-handling

- Soaring Eagles: tank can pick up via AoE threat OR ranged DPS / melee burn them down on spawn. Low HP. AoE burn priority on spawn event.

### Healer-pressure spikes

- Eagle Storm channel (predictable ~3-5s telegraph; pre-stack pre-shield)
- Static Disruption splash on group if target doesn't move
- Add-cleave damage during spawn

### Key positioning

- **Default**: spread (3y+ between players) for Static Disruption splash
- **Eagle Storm telegraph**: STACK tight under designated player (usually healer or one specific DPS)
- **Bot must distinguish two opposite positioning modes**: Static Disruption = spread / Eagle Storm = stack

### Bot-relevant role notes

- **Tank**: maintain threat; pick up Soaring Eagles on spawn; rotate defensives during Eagle Storm if soloing the lift
- **Healer**: pre-stack heal cooldown for Eagle Storm; reactive single-target heals on Static Disruption targets
- **Melee**: in melee already; rotate Call Lightning kicks; AoE burn Soaring Eagles
- **Ranged**: standard ranged spread when not in Eagle Storm; STACK under designated player when storm telegraphs

---

## Boss 2 — Nalorakk (Bear Aspect)

NPC ID: 23576. Second boss after the bear-aspect gate, in the bear shrine chamber.

### Phase structure

Single fight with **alternating troll ↔ bear form** every ~30-45s (UNVERIFIED cadence):

1. **Troll form** (default opener): casts Mojo Volley repeatedly (interruptible); melee weak
2. **Bear form**: shapeshifts into bear; gains Brutal Swipe (frontal cone) and Mangle (tank bleed); no casts — pure melee
3. Cycle repeats

### Abilities

| Spell | ID | Cast / cooldown | School | Notes |
|---|---|---|---|---|
| Mojo Volley | 43375 (UNVERIFIED) | ~1.5s cast, channeled | Nature | Multi-target Nature damage cast in troll form. **MUST INTERRUPT** every cast — primary kick target. |
| Brutal Swipe | 24237 (UNVERIFIED — TBC carryover) | ~2s cast, ~10s cd | Physical | Frontal **cone** in bear form. Face boss away from group. Heavy hit. |
| Mangle | 44955 (UNVERIFIED) | melee-application | Physical | Stacking bleed on tank during bear form. Stacks reset on troll-form swap. Healer pre-HoT before bear swap. |
| Surge | 27797 (UNVERIFIED) | instant, every ~20s | Physical | Charge to a random player; KNOCKBACK + damage. Spread mitigates multi-hit. |
| Lacerating Slash | 43267 (UNVERIFIED) | ~1.5s cast (troll form?) | Physical | DoT bleed on tank. UNVERIFIED whether troll or bear cast. |
| melee | — | — | Physical | Standard tank-melee |

### Interrupts required

- **Mojo Volley** — primary kick target. Bot interrupt priority: HIGH.
- **Lacerating Slash** if confirmed castable (UNVERIFIED).

### Dispels required

- Lacerating Slash bleed is **not magic dispellable** (Physical school per most guides). No standard dispel calls.

### Avoidable damage

- Brutal Swipe cone (face boss away)
- Surge charge multi-hit (spread)

### Tank-swap requirements

**No tank swap on 5-man heroic.** Mangle bleed stacks reset on troll-form swap, so the single tank can ride through stacks. Some guides recommend defensive-CD rotation during peak bear-form Mangle stacks — see **Disagreements #2**.

### Heroic delta

- Mojo Volley unintercepted = group-wide damage spike
- Mangle stacks higher; tank defensive CD pacing more important
- Form-swap cadence may be faster (UNVERIFIED)

### Add-handling

None.

### Healer-pressure spikes

- Bear form Mangle ramp (predictable; pre-HoT)
- Surge charge target (random; reactive heal)

### Key positioning

- **Tank**: pin Nalorakk in center; **face away from group** during bear form (Brutal Swipe cone)
- **Melee**: behind boss in bear form; behind/side in troll form (no cone in troll)
- **Ranged**: max range; spread for Surge charge
- **Healer**: max range; ready for Mangle stack ramps

### Bot-relevant role notes

- **Tank**: face management is key (cone in bear, none in troll). Defensive CD on bear-form transition.
- **Healer**: pre-HoT before bear-form swap (predictable from form-change telegraph).
- **Melee**: rotate kicks in troll form; standard behind-target in bear form.
- **Ranged**: spread for Surge; rotate kicks in troll form (Counterspell, Wind Shear, etc.).

---

## Boss 3 — Jan'alai (Dragonhawk Aspect)

NPC ID: 23578. Third boss after the dragonhawk-aspect gate, in the egg-piled chamber.

### Phase structure

Phase-by-HP encounter with **Hatcher events** triggered at scripted timers:

1. **Phase 1** — tank-and-spank with **Flame Breath** (cone) and **Fire Bomb** ground patches; **Hatchers** patrol egg piles periodically and trigger **Hatch All Eggs** if not killed in time
2. **At 35% boss HP** — major **Hatch All Eggs** event: all remaining eggs in the chamber hatch into dragonhawk hatchlings; heavy AoE-burn phase

The two **Hatcher trolls** are NPCs that walk along the egg piles toward the eggs; if they reach an egg and channel, that egg hatches early. Killing or CC'ing the hatcher prevents the channel.

### Abilities

| Spell | ID | Cast / cooldown | School | Notes |
|---|---|---|---|---|
| Flame Breath | 43124 (UNVERIFIED) | ~2s cast, ~12s cd | Fire | Frontal **cone** breath. Face boss away from group. |
| Fire Bomb | 43137 (UNVERIFIED) | ground-target | Fire | Ground patch dropped at random player position; lasts ~10s; deals fire DoT to standers. |
| Hatch All Eggs | 43144 (UNVERIFIED) | scripted | — | At 35% boss HP, ALL remaining eggs hatch; massive add-spawn. AoE-burn the hatchlings. |
| Summon Hatcher | 43144 (UNVERIFIED — collision with above) | scripted | — | Hatcher trolls spawn periodically; walk toward egg piles. |
| Hatcher's Hatch (channel) | 43734 (UNVERIFIED) | ~3-5s channel | Nature | If a Hatcher reaches an egg pile and channels, that pile of eggs hatches. **Interruptible** OR kill the Hatcher first. |
| Dragonhawk Hatchling melee | — | — | Physical | Small add melee; trivial individually |
| melee | — | — | Physical | Standard tank-melee |

### Interrupts required

- **Hatcher's Hatch channel** — interrupt OR kill the Hatcher first. Lower priority than killing the Hatcher outright.

### Dispels required

None canonical.

### Avoidable damage

- Flame Breath cone (face/positioning)
- Fire Bomb ground patches (move out)

### Tank-swap requirements

None. Single tank.

### Heroic delta

- Hatchers tougher — must be killed promptly or two hatchers compound
- Fire Bomb damage higher
- 35% Hatch All Eggs event spawns more hatchlings; AoE-burn intensity higher

### Add-handling

- **Hatchers**: kill priority is HIGH the moment they spawn; interrupt their channel if they reach eggs
- **Dragonhawk Hatchlings** (from hatched eggs): low-HP, melee group; AoE-burn priority. Tank picks up via AoE threat.

### Healer-pressure spikes

- 35% Hatch All Eggs phase — heavy AoE damage from many hatchling melees
- Fire Bomb DoTs on slow-movers
- Sustained mana drain — this fight is **the main mana-test of ZA heroic**

### Key positioning

- **Tank**: pin Jan'alai in **center of room** facing **away from group**; AoE-pickup hatchlings on spawn
- **Melee**: behind boss; rotate to hatchlings during 35% phase
- **Ranged**: max range; AoE rotation when hatchlings spawn
- **Healer**: max range; pre-cast AoE heal CDs before 35% trigger

### Bot-relevant role notes

- **Tank**: AoE threat tools (Thunder Clap / Swipe / Death and Decay / Consecration / Avenger's Shield) are CRITICAL on this fight — hatchlings will go for closest non-tank otherwise
- **Healer**: this is the **mana check** of ZA. Save Mana Tide / Innervate / Hymn of Hope / Spirit Link for the 35% Hatch All Eggs phase. Pre-HoT before the hatch.
- **Melee**: switch to AoE rotation (Bladestorm / Whirlwind / Divine Storm / Howling Blast / Magma Totem / Swipe-cat) on hatchling spawns
- **Ranged**: switch to AoE rotation (Blizzard / Seed of Corruption / Multi-Shot / Mind Sear / Hurricane / Chain Lightning) on hatchling spawns

---

## Boss 4 — Halazzi (Lion Aspect)

NPC ID: 23577. Fourth boss after the lion-aspect gate.

### Phase structure

Multi-phase by HP threshold:

1. **Phase 1** (100% → 66%) — **lion form**: tank-and-spank with **Saber Lash** (split damage between tank + nearest melee), **Frenzy** enrage, **Flame Shock** debuff
2. **Phase 1.5 transition at 66%** — Halazzi **splits** into Halazzi (continues) + **Spirit of the Lynx** (add). Group must AoE-burn the Spirit while continuing pressure on Halazzi
3. **Phase 2** (66% → 33%) — Halazzi continues with reduced abilities; Spirit of the Lynx dies; **Lightning Totem** appears periodically (kill priority)
4. **Phase 2.5 transition at 33%** — second split; another Spirit of the Lynx
5. **Phase 3** (33% → 0%) — execute phase; Halazzi enrages

### Abilities

| Spell | ID | Cast / cooldown | School | Notes |
|---|---|---|---|---|
| Saber Lash | 43267 (UNVERIFIED) | ~1.5s cast, ~10s cd | Physical | Damage **split between tank and one nearest melee**. Tank should be in melee with at least one DPS to share. |
| Frenzy | 43139 (UNVERIFIED) | proc | — | Self-buff giving Halazzi haste/damage; **dispellable as Magic** by Tranquilizing Shot / Soothe / Spell Steal / etc. |
| Flame Shock (Halazzi) | 43411 (UNVERIFIED) | ~1.5s cast | Fire | Single-target debuff; periodic fire damage; **dispellable as Magic** if class can. |
| Lightning Totem | 43461 (UNVERIFIED) | summon | — | Stationary totem; AoE Nature damage if alive. **KILL PRIORITY** alongside boss. |
| Spirit of the Lynx (split add) | 24046 (UNVERIFIED NPC ID) | scripted | — | Spawns at 66% and 33%. Has its own abilities (melee, possibly Frenzy). |
| Lynx Rush (UNVERIFIED) | (Cata addition) | instant | Physical | Some Cata sources mention a charge mechanic on the Spirit; UNVERIFIED. |
| melee | — | — | Physical | Standard tank-melee |

### Interrupts required

- No primary kick targets. Frenzy is dispelled, not kicked. Flame Shock is dispelled.

### Dispels required

- **Frenzy** — dispel as Magic if class can (Hunter Tranquilizing Shot, Druid Soothe, Mage Spell Steal — Spell Steal also gains the haste buff).
- **Flame Shock** debuff — dispel as Magic if class can (Priest Dispel Magic, Mage Spell Steal target, Paladin Cleanse, Shaman Cleanse Spirit).

### Avoidable damage

- Standing in Lightning Totem AoE
- Bunching melee for Saber Lash splash (tank + one melee = correct; tank + 3 melee = mass damage)

### Tank-swap requirements

None.

### Heroic delta

- Lightning Totem kills group faster if not killed
- Spirit of the Lynx has more HP
- Frenzy dispel timing more critical

### Add-handling

- **Spirit of the Lynx** (split add): AoE-burn while maintaining boss DPS. Re-spawns at 33% second split.
- **Lightning Totem**: kill on sight; respawns periodically.

### Healer-pressure spikes

- Spirit-of-Lynx splits at 66% and 33% (transition damage)
- Frenzy enraged auto-attacks on tank if not dispelled
- Flame Shock fire ticks if not dispelled

### Key positioning

- **Tank**: hold Halazzi in center; melee with one DPS to share Saber Lash
- **Melee**: only ONE melee in Saber Lash split position with tank; other melee at side/back to avoid sharing
- **Ranged**: max range; ready to swap to Spirit of the Lynx on split
- **Healer**: max range; dispel-ready

### Bot-relevant role notes

- **Tank**: ensure exactly ONE melee shares Saber Lash; avoid melee bunching
- **Healer**: dispel Frenzy / Flame Shock when they appear
- **Melee**: one melee bot is the "Saber Lash partner" — share with tank; other melee bots position to AVOID sharing
- **Ranged**: target-priority swap on splits; kill Lightning Totem on sight

---

## Boss 5 — Hex Lord Malacrass

NPC ID: 24239. Fifth boss; humanoid troll. **The non-deterministic boss** — copies abilities from a random class each pull.

### Phase structure

Single phase fight with periodic high-pressure mechanics:

1. **Tank-and-spank** with **Soul Drain** channel (interruptible) on cooldown
2. Hex Lord casts **two abilities chosen from a copied class spec** at the start of the fight (e.g., Frost Mage spells: Frostbolt + Ice Block; Shadow Priest: Shadow Word: Pain + Vampiric Touch; etc.)
3. **At sub-20% HP** — **Spirit Bolts** AoE burn pressure mechanic

The class-ability set is **randomly chosen each pull** from the player class roster (Mage, Priest, Rogue, Warrior, Druid, Paladin, Shaman, Hunter, Warlock, DK). **The bot must reactively detect which abilities Hex Lord has and respond.**

### Abilities (always-present)

| Spell | ID | Cast / cooldown | School | Notes |
|---|---|---|---|---|
| Soul Drain | 43706 (UNVERIFIED) | ~3s channel | Shadow | Drains mana from a player. **MUST INTERRUPT.** Heavy mana drain + damage. |
| Spirit Bolts | 43383 (UNVERIFIED) | ~3s cast (sub-20% only) | Shadow | AoE Shadow nuke; cast at low HP. Group-wide damage spike. |
| Siphon Soul | 44033 (UNVERIFIED — UNVERIFIED whether 5-man) | scripted | Shadow | Original raid had Siphon Soul reset/copy mechanic — UNVERIFIED if present in 5-man |
| melee | — | — | Physical | Standard tank-melee |

### Abilities (random; one of these sets per pull)

| Class set | Abilities | Bot response |
|---|---|---|
| **Frost Mage** | Frostbolt (cast), Ice Lance (instant), Frost Nova (instant; root group), **Iceblock** | Interrupt Frostbolt; spread to mitigate Frost Nova; healer reactive on damaged players |
| **Fire Mage** | Fireball, Pyroblast (cast), Flamestrike (ground patch), **Combustion** | Interrupt hardcasts; move out of Flamestrike; healer pre-shields |
| **Arcane Mage** | Arcane Barrage, Arcane Blast, **Mirror Image** | Interrupt; cleave Mirror Images |
| **Shadow Priest** | Shadow Word: Pain (DoT), Mind Blast (cast), **Vampiric Touch** (DoT, dispellable Magic) | Interrupt Mind Blast; **dispel Magic DoTs** |
| **Holy Priest** | Heal (Hex Lord heals himself; cast — interrupt) | Interrupt Heal — top kick priority on this set |
| **Disc Priest** | Power Word: Shield, Renew, Holy Fire | Interrupt; spell-steal Shield if Mage |
| **Combat/Assn Rogue** | Vanish, Eviscerate, Kidney Shot stun | Tank holds threat; reactive heal |
| **Sub Rogue** | Shadowstep, Premeditation | Reactive heal target |
| **Arms/Fury Warrior** | Mortal Strike, Whirlwind | Standard tank-and-spank flavor |
| **Prot Warrior** | Shield Slam | Standard |
| **Feral Druid** | Mangle, Lacerate, Rip | Bleed dispels (none — Physical) |
| **Resto Druid** | Healing Touch (cast — interrupt), Rejuvenation, **Tranquility** (channel — interrupt) | Interrupt heals — top kick priority |
| **Balance Druid** | Wrath, Starfire, **Starfall** | Interrupt; AoE damage avoidance |
| **Holy Paladin** | Holy Shock, Flash of Light, **Holy Light** | Interrupt heals |
| **Prot Paladin** | Shield of the Righteous | Standard |
| **Ret Paladin** | Crusader Strike, Templar's Verdict | Standard |
| **Elemental Shaman** | Lightning Bolt, Chain Lightning, Lava Burst | Interrupt hardcasts |
| **Enhancement Shaman** | Stormstrike, Lava Lash | Standard tank-and-spank flavor |
| **Resto Shaman** | Healing Wave (cast — interrupt), Riptide, **Healing Stream Totem** | Interrupt heals |
| **BM Hunter** | Steady Shot, Arcane Shot, **pet** | Standard ranged DPS flavor; pet cleave |
| **MM Hunter** | Aimed Shot, Steady Shot | Interrupt Aimed Shot |
| **SV Hunter** | Explosive Shot, Black Arrow | Standard |
| **Affliction Warlock** | Corruption, Curse of Agony, **Drain Life** (channel — interrupt) | Interrupt Drain Life; dispel DoTs |
| **Demonology Warlock** | Shadow Bolt, **Metamorphosis** | Interrupt; standard |
| **Destruction Warlock** | Incinerate, Conflagrate, **Shadowfury** | Interrupt; AoE stun avoidance |
| **Frost DK** | Obliterate, Frost Strike, **Howling Blast** | Standard tank-and-spank flavor |
| **Unholy DK** | Death Coil, **Summon Gargoyle**, diseases | Standard |
| **Blood DK** | Death Strike, **Vampiric Blood** | Standard tank-and-spank flavor |

The exact ability subset varies — guides disagree on which 2-3 abilities are chosen per class set. See **Disagreements #4**.

### Interrupts required

- **Soul Drain** — always present; primary kick target
- **Class-set hardcasts** — depends on roll (Mage Frostbolt, Priest Heal, Druid Tranquility, etc.)

### Dispels required

- Depends on roll. **Magic dispels** likely needed if Shadow Priest / Affliction Warlock / Disc Priest set (DoTs and shields)
- **Curse dispels** if Affliction Warlock set (Curse of Agony) — Druid Remove Corruption / Mage Remove Curse

### Avoidable damage

- Class-set ground patches (Flamestrike, Starfall, Shadowfury) — move out
- Spirit Bolts at sub-20% (unavoidable AoE)

### Tank-swap requirements

None confirmed in 5-man heroic.

### Heroic delta

- Soul Drain damage higher
- Class-set abilities scale with heroic damage values
- Spirit Bolts at sub-20% lethal if group not topped

### Add-handling

- **Class-set may summon adds** (Mirror Image, Gargoyle, hunter pet, etc.) — kill priority depends on add type
- **No always-present adds** in 5-man version (per most sources; see **Disagreements #5**)

### Healer-pressure spikes

- Soul Drain unintercepted = mana + HP drain on target
- Spirit Bolts at sub-20% (group-wide AoE burn)
- Class-set DoTs on group (if Shadow Priest / Affliction set)

### Key positioning

- **Tank**: pin Hex Lord central; face away from group
- **Melee**: behind boss; ready to kick on rotation
- **Ranged**: max range; spread for class-set ground patches
- **Healer**: max range; dispel-ready

### Bot-relevant role notes

- **Tank**: standard tank flow; defensive CD on Soul Drain if interrupt fails
- **Healer**: **reactive dispel logic** — dispel Magic DoTs and curses based on what Hex Lord rolls. Dispel-blacklist is empty (unlike Obsidius Crepuscular Veil)
- **Melee**: **HIGH interrupt priority** — Soul Drain + class hardcasts
- **Ranged**: **HIGH interrupt priority**; spread positioning; reactive AoE-avoid

### Bot implementation note

This is the **most complex** fight in ZA for bot logic. The bot must:
1. Detect Hex Lord's chosen ability set on pull (UI-readable; spell IDs visible on his cast bar)
2. Match the detected ability set to a known response profile
3. Apply the correct response (interrupt-priority, dispel-class, positioning) for the duration of the fight
4. Default to "interrupt all hardcasts; spread for unknown ground effects; dispel Magic on group" if the ability set is unrecognized

Recommended bot architecture: **Hex Lord ability-set detection pass** at the start of the encounter populates a per-encounter override profile that the per-role response logic reads.

---

## Boss 6 — Daakara (formerly Zul'jin in TBC; Cata version is Daakara reigniting Zul'jin's spirit)

NPC ID: 23863 (TBC original); Cata version may have a new ID — UNVERIFIED. Final boss.

### Phase structure

**Aspect-cycle** boss. Daakara cycles through forms (each ~25-30s; UNVERIFIED whether time-based or HP-based):

1. **Troll form** (default) — Whirlwind Axe + Grievous Throw bleed
2. **Bear form** — Charge + Bear Claw cleave (cone)
3. **Eagle form** — Lightning ground patches (similar to Akil'zon Static Disruption)
4. **Lynx form** — Claw Rage; melee-pressure tank with quick attacks
5. **Dragonhawk form** — Flame Whirl ground fire; periodic firebreath

The cycle order varies by guide; see **Disagreements #6**. Cata version **does NOT cycle through all 4 animal aspects every fight** per most sources — Daakara picks a subset (~2 forms) per pull.

### Abilities (troll form — always present)

| Spell | ID | Cast / cooldown | School | Notes |
|---|---|---|---|---|
| Whirlwind Axe | 24236 (UNVERIFIED — TBC carryover) | instant, ~15s cd | Physical | Throws axes that whirl around boss area; AoE damage. |
| Grievous Throw | 43093 (UNVERIFIED) | ~2s cast | Physical | **MUST INTERRUPT.** Throw that applies a heavy bleed DoT. |
| melee | — | — | Physical | Standard tank-melee |

### Abilities (bear form)

| Spell | ID | Cast / cooldown | School | Notes |
|---|---|---|---|---|
| Charge | 24033 (UNVERIFIED) | instant | Physical | Charges a random player; KNOCKBACK. |
| Bear Claw | 24239 (UNVERIFIED) | melee cone | Physical | Frontal cone in bear form. Tank face-away. |

### Abilities (eagle form)

| Spell | ID | Cast / cooldown | School | Notes |
|---|---|---|---|---|
| Cyclone | 43661 (UNVERIFIED) | ~1s cast | Nature | Tornado ground patch; player must move out. |
| Static Charge | 43622 (UNVERIFIED — collision with Akil'zon) | targeted | Nature | Similar to Akil'zon Static Disruption. |

### Abilities (lynx form)

| Spell | ID | Cast / cooldown | School | Notes |
|---|---|---|---|---|
| Claw Rage | 43149 (UNVERIFIED) | melee, ~10s cd | Physical | Rapid melee combo on tank; significant burst damage. |
| Lynx Rush | 43151 (UNVERIFIED) | charge | Physical | Charges multiple targets in sequence — PARTY-WIDE damage if not dispelled or stopped. |

### Abilities (dragonhawk form)

| Spell | ID | Cast / cooldown | School | Notes |
|---|---|---|---|---|
| Flame Whirl | 43213 (UNVERIFIED) | ground patches | Fire | Drops fire patches around boss; spread + move-out. |
| Pillar of Fire (UNVERIFIED) | (Cata) | targeted | Fire | Targeted fire pillar; move-out. |

### Interrupts required

- **Grievous Throw** (troll form) — primary kick on troll
- Other forms have no kick-required hardcasts in most reports

### Dispels required

- Grievous Throw bleed is **Physical** — not dispellable
- Cata-form effects mostly Physical/Fire/Nature but not standardly dispellable

### Avoidable damage

- Eagle-form ground patches (Cyclone, Static Charge)
- Dragonhawk-form fire patches (Flame Whirl)
- Bear-form Charge knockback (spread)
- Lynx-form Lynx Rush (target spread)

### Tank-swap requirements

None confirmed.

### Heroic delta

- Form damage scales up across the fight
- Lynx form Claw Rage burst higher; defensive CD pacing critical
- Dragonhawk fire patches harder to dodge

### Add-handling

None confirmed.

### Healer-pressure spikes

- **Form transitions** (predictable telegraph)
- Lynx form Claw Rage burst on tank (highest)
- Dragonhawk fire patch DoTs on slow-movers

### Key positioning

- **Tank**: hold central; face away from group on bear form; rotate facing on form swap
- **Melee**: behind boss; rotate during eagle/dragonhawk forms (ground patches)
- **Ranged**: max range; spread
- **Healer**: max range; ready for form transitions

### Bot-relevant role notes

- **Tank**: detect form change (visual + buff) and adjust facing/defensive CDs accordingly
- **Healer**: detect form change → switch playbook (eagle = pre-spread heals, lynx = burst tank heals, dragonhawk = AoE-fire heals)
- **Melee**: detect form change → reposition (cone awareness in bear, ground-patch awareness in eagle/dragonhawk)
- **Ranged**: detect form change → spread/stack rules differ per form

### Bot implementation note

The bot must implement a **form-detection sub-state** for Daakara. Each form has its own response profile. Form change is detected via visible aura change (bear-form aura, eagle-form aura, etc.) on the boss. Default playbook on form-change: **conservative defaults** (face-away, spread, pre-shield) until form aura confirms specific form.

---

## Trash mechanics

Zul'Aman trash is significant — heavy interrupt and CC pressure. Several pulls require specific positioning. Skippable trash is minimal.

### Trash before Akil'zon (Eagle gate approach)

- **Amani'shi Tribesman** — melee mob with frontal cleave; tank face-away. Interruptible **Hex** cast.
- **Amani'shi Warbringer** — large mob with **Battle Shout** raid-buff cast; **MUST INTERRUPT** to deny self-buff.
- **Amani'shi Axe Thrower** — ranged thrower; interrupt or LoS to control.
- **Eagles** (small flying adds before Akil'zon) — low-HP; AoE-burn.

### Trash between Akil'zon and Nalorakk (Bear path)

- **Amani'shi Beast Tamer** — handler with bear pet; CC pet (Polymorph/Hex) to single-target the tamer
- **Amani Bear** — beast; **non-CC-able** (most CC doesn't affect beasts in Cata); brute-force kill
- **Amani'shi Savage** — melee with **Burning Brand** stack-bleed; tank stays still, healer HoTs

### Trash between Nalorakk and Halazzi (Dragonhawk + Lion paths)

- **Amani Dragonhawk Hatcher** (Jan'alai approach) — caster trash; Polymorph/Hex valid
- **Amani Lynx** — beast; brute-force kill
- **Amani'shi Tempest** — caster with **Lightning Bolt** hardcast; **MUST INTERRUPT**

### Trash before Hex Lord

- **Hex Lord's Council** (4 troll adds) — most guides report these are **removed in 5-man heroic**; some report a single add or none. **UNVERIFIED**; see **Disagreements #5**.
- **Amani Berserkers** — high HP melee; AoE-pull viable

### Trash before Daakara

- **Amani Wandering Spirits** — caster ghosts; interrupt / dispel
- Final approach mobs are mostly trash; CC viable on humanoids

### CC priorities

- **Polymorph / Hex / Sap / Bind Elemental / Banish / Repentance** valid on Amani **humanoids** (Tribesman, Warbringer, Tempest, Beast Tamer)
- **Beasts** (Bears, Lynxes, Dragonhawks) — most CC doesn't apply; use Hibernate (Druid) or Freezing Trap (Hunter) only
- **Eagles** (small) — too low-HP to bother; AoE-burn

### Skip-able trash

- Some patrols can be wall-hugged on speed-clears
- The bear-mount timed run skips as much trash as possible — but bot ignores the timer

### Pull markers for CC

- Caster packs: skull on lead, cross/X on second caster (CC), square on third
- Multi-mob beast packs: skull on largest beast (priority), AoE the rest

---

## Disagreements

Numbered list of points where the 5-7 reconciled guides do not agree. Bot logic should pick a default and note the deviation.

### Disagreement #1 — Akil'zon Static Disruption dispellability

- **Source A (Icy Veins):** Static Disruption ground-patch is environmental; not dispellable
- **Source B (Warcraft Tavern):** The targeted-player flavor effect is a Magic debuff and CAN be dispelled to remove the patch's link to the player
- **Source C (Wowhead):** Spell tooltip ambiguous; community comments mostly describe move-out, not dispel

**Default:** treat as **non-dispellable**; the standard response is **move out of group**, not dispel. Bot dispel logic ignores Static Disruption.

### Disagreement #2 — Nalorakk Mangle stacks: tank-swap or ride-through

- **Source A:** Single tank rides through Mangle stacks (5-man comp); stacks reset on form swap
- **Source B:** Off-tank picks up at high stacks (raid carryover; doesn't apply to 5-man)
- **Source C:** Defensive CD rotation is sufficient

**Default:** **single tank rides through with defensive CDs on bear-form transition**. No tank-swap mechanic for the 5-man heroic.

### Disagreement #3 — Jan'alai Hatcher kill vs CC priority

- **Source A:** Kill the Hatcher on sight (highest priority)
- **Source B:** CC the Hatcher (Polymorph/Hex/Freezing Trap) and let boss tick away
- **Source C:** Interrupt the Hatcher's channel only when it reaches eggs

**Default:** **kill the Hatcher on sight** — simplest bot logic, prevents recurring CC management. Interrupt the channel as a fallback if Hatcher reaches eggs before kill.

### Disagreement #4 — Hex Lord ability-set composition

- **Source A:** 2 abilities per class set (e.g., Mage = Frostbolt + Iceblock)
- **Source B:** 3 abilities per class set
- **Source C:** Variable — depends on which class is rolled

**Default:** assume **2-3 abilities per set**; bot detects all visible casts/auras on Hex Lord during the first 30s and builds the active-set profile dynamically. Don't hard-code; **detect at runtime**.

### Disagreement #5 — Hex Lord 4-troll-add council

- **Source A:** Council adds are **removed** in 5-man heroic (Cata simplification)
- **Source B:** Council adds are **present** but with reduced HP
- **Source C:** Council adds **respawn** if killed too quickly

**Default:** treat as **NOT present** in 5-man heroic. If any adds appear, fall back to add-pickup logic on the tank.

### Disagreement #6 — Daakara form-cycle: time-based or HP-based, all-4 or subset

- **Source A:** Form changes every ~25s (time-based); cycles through all 4 animal aspects
- **Source B:** Form changes at HP thresholds (75/50/25); subset of 2-3 aspects per pull
- **Source C:** Random; ~30s cadence; subset of 2 aspects

**Default:** assume **time-based ~30s cadence with random subset of 2-3 aspects per pull**. Bot detects form change via boss aura, not via timer or HP. Each form has its own response profile.

### Disagreement #7 — Akil'zon Eagle Storm stack threshold

- **Source A:** All players stack tight (within 2y)
- **Source B:** Players within ~5y are lifted
- **Source C:** Range is ~8y

**Default:** **stack within ~3y of designated player**. Conservative tightness — over-stacking is harmless on this mechanic.

### Disagreement #8 — Halazzi Spirit of the Lynx kill order

- **Source A:** Burn Spirit first (priority), then return to Halazzi
- **Source B:** Cleave Spirit + Halazzi together (don't drop boss DPS)
- **Source C:** Tank Spirit; AoE both

**Default:** **cleave both** if AoE-spec; **kill Spirit first** if pure single-target. Pragmatic split — bot picks based on AoE-class capability.

### Disagreement #9 — Daakara Lynx-form Claw Rage tankability

- **Source A:** Lynx form Claw Rage is the highest tank burst in the fight; major defensive CD required
- **Source B:** Standard melee; defensive CD optional
- **Source C:** Lynx form is short; ride through with stack/HoTs

**Default:** **major defensive CD on Lynx-form transition** (Shield Wall / Survival Instincts / Vampiric Blood / Icebound Fortitude / Pain Suppression equivalent).

### Disagreement #10 — Bear-mount timed-run gate

- **Source A:** 20-minute timer
- **Source B:** 25-minute timer
- **Source C:** No timer in Cata version; original TBC had a chest event

**Default:** **bot ignores timer entirely.** No bear-mount logic in bot scope.

---

## DBC verification (UNVERIFIED items)

Spell IDs and stack values listed below are from community-guide cross-reference and TBC-carryover assumptions, not direct DBC lookup. Add to `docs/research/dbc-verification-checklist.md` for resolution when the TC fork session opens.

| # | Spell / mechanic | Listed value | Why it matters for bot logic |
|---|---|---|---|
| 1 | Akil'zon Static Disruption | 43622 | Ground-patch detection; "spread" trigger |
| 2 | Akil'zon Electrical Storm / Eagle Storm | 43648 | Stack-up trigger (opposite of #1) |
| 3 | Akil'zon Call Lightning | 43661 | Interrupt rotation entry |
| 4 | Nalorakk Mojo Volley | 43375 | PRIMARY kick target on troll form |
| 5 | Nalorakk Brutal Swipe | 24237 | Tank cone-facing trigger |
| 6 | Nalorakk Mangle | 44955 | Tank bleed stack tracking |
| 7 | Nalorakk Surge | 27797 | Spread positioning trigger |
| 8 | Jan'alai Flame Breath | 43124 | Tank cone-facing trigger |
| 9 | Jan'alai Fire Bomb | 43137 | Ground-patch avoidance |
| 10 | Jan'alai Hatch All Eggs | 43144 | 35% HP phase trigger |
| 11 | Jan'alai Hatcher's Hatch | 43734 | Interrupt trigger |
| 12 | Halazzi Saber Lash | 43267 | Melee splash positioning rule |
| 13 | Halazzi Frenzy | 43139 | Magic-dispel trigger |
| 14 | Halazzi Flame Shock | 43411 | Magic-dispel trigger |
| 15 | Halazzi Lightning Totem | 43461 | Kill-priority NPC ID |
| 16 | Hex Lord Soul Drain | 43706 | PRIMARY kick target |
| 17 | Hex Lord Spirit Bolts | 43383 | Sub-20% AoE trigger |
| 18 | Hex Lord random ability-set composition | NPC IDs of class-set spells | Per-pull detection logic — verify spell IDs for at least the most-common rolls (Mage, Priest, Druid, Paladin) |
| 19 | Daakara Grievous Throw | 43093 | PRIMARY kick on troll form |
| 20 | Daakara Whirlwind Axe | 24236 | AoE-avoidance |
| 21 | Daakara Charge (bear form) | 24033 | Knockback prediction |
| 22 | Daakara Bear Claw | 24239 | Tank cone-facing trigger |
| 23 | Daakara Cyclone (eagle form) | 43661 (collision with #3?) | Ground-patch detection |
| 24 | Daakara Static Charge (eagle form) | 43622 (collision with #1) | Resolve naming conflict in DBC |
| 25 | Daakara Claw Rage (lynx form) | 43149 | Tank burst defensive trigger |
| 26 | Daakara Lynx Rush (lynx form) | 43151 | Multi-target charge avoidance |
| 27 | Daakara Flame Whirl (dragonhawk form) | 43213 | Ground-patch detection |
| 28 | Daakara form-change cadence | ~25-30s OR HP-threshold | Form-detection logic timing |
| 29 | Hex Lord 4-troll council presence in 5-man | UNVERIFIED — likely absent | Add-pickup logic if present |
| 30 | Daakara NPC ID (Cata version) | 23863 (TBC) or new | Boss-detection trigger |
| 31 | Akil'zon Soaring Eagle add NPC ID | UNVERIFIED | AoE-target prioritization |
| 32 | Halazzi Spirit of the Lynx NPC ID | 24046 (UNVERIFIED) | Add-detection on splits |
| 33 | Jan'alai Hatcher NPC ID | UNVERIFIED | Kill-priority NPC ID |
| 34 | Jan'alai Dragonhawk Hatchling NPC ID | UNVERIFIED | AoE-target detection |

Spell IDs 43622 and 43661 appear under multiple bosses (Akil'zon and Daakara eagle form) — verify whether Cata reuses TBC IDs across bosses or assigns new IDs. Resolve at DBC-check time.

Hex Lord ability-set spell IDs are **deferred** — bot detection logic reads visible cast bars rather than hard-coding all class-set spell IDs. Only the most-common 2-3 sets need verification for tuning.

---

## Notes on prompt deviations

- **Heroic-only dungeon** — no normal-mode section; entire survey assumes heroic difficulty.
- **6 bosses, not 4 + 1 optional** — survey sections expanded accordingly.
- **Hex Lord non-deterministic ability set** — explicitly flagged as a runtime-detection problem rather than a static spell-ID lookup. Bot architecture must support per-encounter ability-set detection.
- **Daakara aspect-cycle** — explicitly flagged as a state-machine problem; each form is a sub-state with its own response profile. Bot must detect form change via boss aura.
- **Bear-mount timed run** — explicitly excluded from bot scope.
- **CC pull-markers** were rolled into the Trash section, matching the BRC format.
- **Many spell IDs are UNVERIFIED** — Zul'Aman is a TBC-revamp dungeon and Wowhead Cata pages often reuse original TBC spell IDs without explicit Cata confirmation. The DBC verification list is unusually long because of this.
- **No instance-wide buff/debuff** — nothing instance-level to model.
- **Dispel pressure is per-encounter, not instance-wide** — Halazzi Frenzy/Flame Shock are dispel calls; Hex Lord depends on roll. There is no "always-on" dispel rule for ZA.
- Wowhead Cata is the spell-ID source of record; where it disagreed with Icy Veins / Warcraft Tavern, Wowhead won. Where Wowhead was silent (most TBC-carryover spells), the value was carried forward from TBC sources with UNVERIFIED markers.

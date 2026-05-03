# Lost City of the Tol'vir — Cataclysm 5-man Survey

**Date:** 2026-05-02
**Patch:** 4.3.4
**Audience:** bot-control engineer (cata-altbot)

---

## Overview

| Field | Value |
|---|---|
| Zone | Lost City of the Tol'vir (southern Uldum) |
| Continent | Kalimdor (Uldum) |
| Level range (normal) | 83–85 |
| Level range (heroic) | 85 |
| Group size | 5 (1 tank / 1 healer / 3 DPS) |
| Boss count | 4 required (Lockmaw + Augh = 1 encounter on normal; Augh is heroic-only standalone-ish reappearance per some sources) |
| Layout | Outdoor city ruins with three terrace tiers; Husam first, then a fork — Lockmaw OR Barim — and Siamat on the high terrace via slipstreams (wind-vortex teleporters) |
| Cata-launch class | Yes — one of the two "advanced" launch heroics (alongside Halls of Origination and Grim Batol) gated behind the entry-tier Cata 5-mans |
| Group buffs/debuffs | None instance-wide |

The instance is medium-length (~30-45 min on heroic). Trash density is moderate-to-high; the path is "outer courtyard (Husam) → second tier with two side rooms (Lockmaw left-ish, Barim right-ish; order is group-choice) → slipstreams up to high terrace → Siamat platform." Lockmaw and Barim can be cleared in either order; both must die before slipstreams to Siamat are accessible.

The defining instance-level mechanics are:
- **Heavy poison/disease dispel pressure** — Lockmaw's Viscous Poison (Poison) and Augh's Paralytic Blow Dart (Poison) are both Poison-school dispels with significant damage, and Barim's Plague of Ages is a Disease-school disease that bounces — together this is the **canonical Cata heroic dispel-mana-test dungeon** (similar role that Karsh plays for Blackrock Caverns).
- **Barim's split-phase** — at 50% HP Barim's Repentance fires; the boss enters an immune phase while the group fights a Harbinger of Darkness add (some sources describe this as a "spirit realm" / shadow-phase split). Whether the group is teleported to a separate map or just stays in place is mildly disputed across guides; what is **not** disputed is that there's a script-driven phase transition the bot must detect and respond to, including a stun (Repentance) and a kill-priority reset (Soul Fragments → Harbinger of Darkness).

---

## Sources

This survey reconciles the following community sources. Wowhead Cata is treated as authoritative for spell IDs and numeric values; strategy is cross-checked across 5 guides.

1. **Wowhead Cata — encounter & NPC pages** (`/cata/npc=...`, `/cata/spell=...`) — primary for spell IDs, cast times, schools, dispel types, debuff durations
2. **Icy Veins — Lost City of the Tol'vir Dungeon Guide** (`icy-veins.com/cataclysm-classic/lost-city-of-the-tol-vir-dungeon-guide`)
3. **Dexerto — WoW Cataclysm Classic Lost City of Tol'vir Guide** (`dexerto.com/world-of-warcraft/wow-cataclysm-classic-lost-city-of-tolvir-guide-2722502/`)
4. **Altered Gamer — Lost City of Tol'vir Instance Guide** (`alteredgamer.com/wow-instances/108302-the-seat-of-power-lost-city-of-tolvir-instance-guide/`)
5. **Guiaswow — Guide to Lost City of the Tol'vir** (`en.guiaswow.com/dungeons/guide-lost-city-tolvir-...`)
6. **Ten Ton Hammer — Cataclysm Preview: The Lost City of the Tol'vir** (`tentonhammer.com/articles/cataclysm-preview-the-lost-city-of-the-tol-vir`)
7. **Warcraft Wiki** (`warcraft.wiki.gg/wiki/Lost_City_of_the_Tol%27vir`) — cross-reference for boss names and structure (loot tables only; no ability detail)

Where guides disagree, the disagreement is captured in the **Disagreements** section and the consensus position is listed in **Strong consensus**.

---

## Strong consensus (table)

| Mechanic | Consensus |
|---|---|
| General Husam — Mystic Trap | Boss throws fire-damage traps at random player positions; traps arm after a brief delay and detonate on contact. **Move out of trap radius before they arm.** |
| Husam — Detonate Traps (heroic) | Heroic-only ability that prematurely detonates all armed traps on a 5s countdown. Group must clear away from existing traps before the countdown ends. |
| Husam — Bad Intentions | Charge + throw mechanic — boss picks up a random player and slams them into a wall. **Unavoidable;** healer reacts to the spike. Some guides describe it as a knockback against pillars. |
| Husam — Shockwave | Visible ground X-pattern (4 directional lines) that erupt with damage. **Move off the lines.** Tank-included avoidance. |
| Husam — Hammer Fist | Tank-targeted multi-hit melee combo (4 swings over 2s). Healer pre-spike. |
| Husam — kite pattern | **Tank kites Husam in a slow circle** around the chamber so trap zones accumulate where the group has already been; do not let the boss park. |
| Lockmaw — Dust Flail | **Frontal-rear cone** (tail attack) — DO NOT stand behind boss. Reduces accuracy 50% to anyone hit; tank face-management critical. |
| Lockmaw — Viscous Poison | Random-target ground poison; **Poison-school dispel, 12s DoT.** Heavy heroic healer mana drain; **canonical mana-test of the dungeon.** |
| Lockmaw — Scent of Blood | Bleed on random player + summons Frenzied Crocolisk adds that fixate on bleed target. AoE-burn the adds; tank picks up. |
| Lockmaw — Venomous Rage (30% HP) | Self-buff: +25% damage at low HP. Some guides flag as enrage-strippable; bot default treats as **non-removable** (conservative). |
| Lockmaw → Augh | Augh appears at low HP / after Lockmaw dies (sources disagree on exact trigger — see **Disagreement #1**). Augh is **immune to damage while Lockmaw lives**. |
| Augh — Paralytic Blow Dart | Random-target poison + 4.3k/2s DoT + 40% attack-speed slow. **Poison-school dispel.** Critical for tank if landed. |
| Augh — Whirlwind | Augh spins, dealing high physical damage in melee range; kite-by-tank, melee back off. |
| Augh — Smoke Bomb | Stuns nearby enemies (esp. tank) for 3s — DPS may need to taunt-relay. |
| Barim — Phase 1 (Light) | Boss tank-and-spank with Plague of Ages (Disease dispel), Heaven's Fury (avoid ground patches), Fifty Lashings (proc-buff on tank), Blaze of the Heavens add (heroic). |
| Barim — Plague of Ages | **Disease-school dispel.** 9s DoT (Nature damage); jumps to nearby allies on dispel/expire. **Spread to limit jump targets.** |
| Barim — Heaven's Fury | Holy ground pillar at random player location; 12y radius; ~9s persistent damage. **Move out.** |
| Barim — Fifty Lashings | Self-buff on Barim — next 10 melee swings deal +5000 Physical bonus. Tank/healer cooldown trigger. |
| Barim — Blaze of the Heavens (heroic) | Phoenix-style add in Phase 1 (heroic only per most sources; some say all difficulties). Either burn or kite-around-room. |
| Barim — Repentance (50% HP) | Phase transition: AoE stun on group (6s); creates **Hallowed Ground** holy DoT zone; Barim becomes immune. Soul Sever fires; group fights Harbinger of Darkness. |
| Barim — Harbinger of Darkness | Phase 2 add. Tank picks up; fires Soul Sever (creates Soul Fragments) and Wail of Darkness. **Adds (Soul Fragments) must be killed/CC'd before reaching Harbinger** — they buff Harbinger if they reach it. |
| Barim — Phase 2 ends | Harbinger dies → Phase 1 resumes; Barim re-engageable. Cycle repeats until Barim dies. |
| Siamat — Deflecting Winds (P1) | Boss damage-reduction shield (90% DR) until all 3 Servants die. **Burn Servants first.** |
| Siamat — Servants of Siamat (3) | Large air elementals; killing all 3 ends P1. Each casts Lightning Nova (PBAoE) and Thunder Crash (target burst). On heroic, Servants drop **Lightning Charge** stacks on group when killed (+10% damage / +10% haste / +33% damage taken — see **Disagreement #5**). |
| Siamat — Minions of Siamat (continuous) | Small adds on ~30s cycle in BOTH phases. Self-deplete (10% HP / 2s) — they die on their own. Cast Chain Lightning (spread!). On low HP they form **Tempest Storm** (6y moving cloud DoT). |
| Siamat — Cloud Burst (P1) | Random-target ground cloud; 5y radius; bursts after 3s with knock-up + Nature damage. **Move out.** |
| Siamat — Storm Bolt | Ranged Nature damage on random player. Constant low-grade pressure. |
| Siamat — Wailing Winds (P2 entry) | 6s knockback storm at P2 transition; deals damage and **knocks players around** the platform — knockoff hazard. Stay center. |
| Siamat — Gathered Storms (P2) | Siamat absorbs nearby Tempest Storms from Minions, gaining a stacking 25s "blast 2 players for damage" effect (heroic: hits ALL players). Minions become avoidance priority in P2. |
| Lockmaw poison-dispel mana drain | **Heavy** healer mana sink on heroic — Viscous Poison, Augh's Paralytic Blow Dart, Vicious Bite stacks all hit dispel-or-die windows. Bot healer must pace mana around this fight as the dungeon's primary OOM-test. |
| Curse dispel needs | None on bosses |
| Disease dispel needs | **Yes — Barim Plague of Ages (82622).** Mandatory dispel on heroic. |
| Poison dispel needs | **Yes — Lockmaw Viscous Poison (81630), Augh Paralytic Blow Dart (84799).** Heavy dispel pressure. |
| Magic dispel needs | None confirmed on bosses (Barim's Heaven's Fury and Repentance are non-dispellable script effects) |

---

## Boss 1 — General Husam

NPC ID: 44577. First boss — outer courtyard. Single-phase encounter; mechanics overlap (traps + shockwave + charges) on a tight timer, demanding constant movement.

### Phase structure

Single phase, on a repeating cycle:

1. Tank-and-spank with periodic **Hammer Fist** combo (multi-hit on tank)
2. Recurring **Mystic Trap** placement (3 random players' locations)
3. Recurring **Bad Intentions** charge + throw on random non-tank
4. Recurring **Shockwave** X-pattern ground eruption
5. **(Heroic)** **Detonate Traps** — premature simultaneous detonation of all live traps after a 5s warning

### Abilities

| Spell | ID | Cast / cooldown | School | Notes |
|---|---|---|---|---|
| Mystic Trap | 83171 | instant | Fire | Trap placed at random player; arms after ~2s; detonates on player contact for 48,749 damage (heroic). 7y radius. **AVOID.** |
| Detonate Traps (heroic only) | 91263 | 5s warning | Fire | Premature mass-detonation of all armed traps. Group must move clear before countdown ends. |
| Bad Intentions | 83113 | instant | Fire (charge) | Charges random non-tank player, throws them at impact point. ~43,875-46,125 damage + stun. **UNAVOIDABLE** — healer reactive. |
| Hammer Fist | 83655 | melee combo | Physical | 4 attacks over 2s (one per 0.5s), each at 75% melee damage. Tank-targeted. |
| Shockwave | 83445 | ~2s telegraph | Physical | 4 lines erupt outward in X-pattern; ~43,875 damage within 4y of each line. Visible warning on ground; **move off lines.** |
| melee | — | — | Physical | Standard tank melee |

### Interrupts required

None — Husam has no kick-required hardcasts.

### Dispels required

None.

### Avoidable damage

- **Mystic Trap** (move out of trap zones; don't path through arming traps)
- **Shockwave lines** (visible X-pattern; move off)
- **Detonate Traps countdown** (heroic — clear all current trap zones before fire)

Bad Intentions (charge) and Hammer Fist (melee combo) are unavoidable.

### Tank-swap requirements

None.

### Heroic delta

- **Detonate Traps** ability added (mass premature detonation)
- More traps spawned per cast (~5 vs ~3)
- Damage scales as expected
- **Tank kite pattern is mandatory** — slow circle around the room so traps accumulate behind the group, not in front

### Add-handling

None during fight.

### Healer-pressure spikes

- Bad Intentions impact damage + stun (predictable in trigger; spike landing on the charged player)
- Hammer Fist combo on tank (predictable cooldown)
- Detonate Traps mass detonation if anyone is caught in a trap zone (heroic)

### Key positioning

- **Tank**: kite Husam in a slow circle so the group's clean ground stays ahead, trap zones accumulate behind
- **Group**: follow the tank; move off Shockwave lines; avoid stepping into trap zones
- **Ranged**: max range from boss but stay inside the kite path so they don't lag behind

### Bot-relevant role notes

- **Tank**: kite-pattern AI — distinguish "move boss to safe ground" from "stand and tank." Active environmental hazard avoidance is the new behavior.
- **Healer**: spike-react on Bad Intentions and Hammer Fist; pre-shield where possible.
- **Melee**: stay behind boss; follow kite path; move off Shockwave lines.
- **Ranged**: max range, follow kite path; very mobile fight — cast-while-moving classes preferred.

---

## Boss 2 — Lockmaw (and Augh)

NPC ID: 43614 (Lockmaw); Augh is a separate NPC that emerges at low HP / after Lockmaw dies (see **Disagreement #1**). Second boss option (Lockmaw and Barim are clearable in either order).

### Phase structure

1. **Lockmaw phase** — tank Lockmaw with face management for Dust Flail; dispel Viscous Poison; AoE adds from Scent of Blood; heroic enrage at 30%
2. **Augh phase** — Augh becomes attackable when Lockmaw dies (or appears at Lockmaw low-HP per some sources); tank kites Augh through Whirlwind; dispel Paralytic Blow Dart

On normal mode some sources describe Augh as a brief "jump-in" interruption rather than a full second phase; on heroic, Augh is a full standalone fight after Lockmaw's death.

### Abilities — Lockmaw

| Spell | ID | Cast / cooldown | School | Notes |
|---|---|---|---|---|
| Viscous Poison | 81630 | instant | Nature | **Poison-school dispel.** Random ground glob; 5y radius; immediate damage + 30% slow + 12s DoT (4,161/tick normal, 9,249/tick heroic). **DISPEL on heroic; mana sink.** |
| Dust Flail | 81644 | 5s channel | Nature | **Rear cone** (tail) — players behind boss take 11,423/sec for 5s with +50% damage per second standing in. Also 50% accuracy reduction 10s. **DO NOT stand behind.** |
| Scent of Blood | 81690 | instant | Physical | Bleed on random player; ~1,850-2,150 damage / 2s for 30s; summons Frenzied Crocolisks that fixate on bleed target. |
| Venomous Rage | 81706 | passive | — | At 30% HP: +25% damage on Lockmaw. Some guides report enrage-strippable — bot default: **treat as non-removable** (conservative). |
| melee | — | — | Physical | Standard tank melee |

### Abilities — Augh

| Spell | ID | Cast / cooldown | School | Notes |
|---|---|---|---|---|
| Paralytic Blow Dart | 84799 | instant | Nature | **Poison-school dispel.** Random target; 7,861 immediate Nature damage + 4,300/2s DoT 9s + 40% attack-speed slow. **DISPEL ASAP** — especially if landed on tank. |
| Whirlwind | 1680 | channel | Physical | Augh spins; high physical damage to anyone in melee. Tank-kite or back-off behavior. |
| Smoke Bomb | 7964 | instant | Physical | 5y AoE around Augh; **3s stun**. Tank likely affected; DPS may need to taunt-relay through stun window. |

### Abilities — Frenzied Crocolisk (Scent of Blood adds)

| Spell | ID | Cast / cooldown | School | Notes |
|---|---|---|---|---|
| Vicious Bite | 81677 | melee | Physical | Stacking bleed on melee target; up to 3 stacks. Adds path to Scent-of-Blood-bleeding player. |

### Interrupts required

None. Lockmaw has no hardcasts; Augh has no hardcasts.

### Dispels required

- **Viscous Poison (81630) — Poison** — primary heroic mana sink, 12s DoT
- **Paralytic Blow Dart (84799) — Poison** — high priority on tank target

Both are **Poison-school**; healer must have Cleanse Spirit / Poison Cleansing Totem / Cleanse / Cure Poison available. **Heroic Lockmaw is the canonical mana-test of this dungeon** — bot healer must save Mana Tide / Innervate / Hymn of Hope / equivalent for this fight.

### Avoidable damage

- **Stand-behind Lockmaw** — Dust Flail rear cone
- **Stand-in Viscous Poison ground patches** — 5y radius DoT
- **Augh Whirlwind** — melee in melee range only when boss is stationary

### Tank-swap requirements

None standard. **Augh's Smoke Bomb can stun the tank** — if the bot has only one tank-class player, DPS may need to off-tank during the 3s stun window.

### Heroic delta

- Augh is a full second-phase fight (vs normal where Augh is more of a brief intrusion per some sources — see **Disagreement #1**)
- Viscous Poison damage roughly 2x normal (9,249/tick vs 4,161/tick)
- Venomous Rage 30% enrage hits harder
- More crocolisk adds from Scent of Blood

### Add-handling

- **Frenzied Crocolisks** from Scent of Blood — fixate on the bleed target. Tank pulls them off via taunt OR group AoE-burns immediately. Low HP.
- They stack Vicious Bite on whoever they're hitting; melee'd target can stack to 3.

### Healer-pressure spikes

- **Viscous Poison ticks unmitigated by dispel** (steady drain on whoever stands in)
- **Paralytic Blow Dart on tank** (large initial + DoT + slow)
- **Augh Whirlwind on group** if positioning fails
- **Venomous Rage 30%** — Lockmaw damage spike

### Key positioning

- **Lockmaw**: tank faces boss away from group with **tail pointed away from group too** (tail = Dust Flail rear cone). Practical: tank pulls Lockmaw to a wall and stands at the wall facing center; group stands to the side of the boss (not front, not behind).
- **Augh**: kite-by-tank around the room; melee back off during Whirlwind
- **Crocolisks**: AoE-burn at scent-target (likely a DPS); tank picks up if they path past tank

### Bot-relevant role notes

- **Tank**: critical face management for Lockmaw (front + back are both hazards; group must side-stand). Augh kite when Whirlwind active.
- **Healer**: **THIS IS THE DUNGEON'S MANA TEST**. Pre-cast Mana Tide / equivalent. Dispel Viscous Poison and Paralytic Blow Dart on rotation; do not chase off-target overheals during.
- **Melee**: NEVER stand behind Lockmaw. Side-position. Step out of Augh Whirlwind.
- **Ranged**: spread for crocolisk adds; cast at side angle to avoid Dust Flail rear-cone path.

---

## Boss 3 — High Prophet Barim

NPC ID: 43612. Third boss option (Lockmaw and Barim are clearable in either order). Two-phase encounter with a script-driven Phase 1 ↔ Phase 2 transition that may be **bot-implementation hazardous** (see **Disagreement #2** and the bot-impl flag in encounter.md).

### Phase structure

1. **Phase 1 (Light / Begone Infidels)** — Barim attackable; mechanics include Plague of Ages (Disease dispel), Heaven's Fury (ground avoid), Fifty Lashings (tank-buff on boss), Blaze of the Heavens phoenix add (heroic per most sources)
2. **Phase 2 (Dark / Kneel and Repent)** — triggered at 50% HP via **Repentance** (group AoE stun + Hallowed Ground holy zone); Barim becomes immune; **Harbinger of Darkness** add spawns; group fights Harbinger while managing Soul Fragments (must die before reaching Harbinger, else Harbinger heals/buffs)
3. **Phase 1 resume** — when Harbinger dies, Phase 2 ends; Barim re-engageable. Cycle: 100% → 50% → P2 → P1 resume → 0%

The "spirit realm" descriptor used in some sources implies a separate map / phased instance during P2. **What actually happens in 4.3.4** is debated across guides; likely the group remains in the same room and the Harbinger spawns in place. **Bot implementation flag**: if the encounter teleports players to a separate instanced phase, the bot's threat / position / target tracking must handle the transition. Default assumption: same room, Barim becomes immune, Harbinger pops in. See **Disagreement #2**.

### Abilities — Barim (Phase 1)

| Spell | ID | Cast / cooldown | School | Notes |
|---|---|---|---|---|
| Plague of Ages | 82622 | instant, 45y range | Nature | **Disease-school dispel.** 8,840-11,960 immediate + 4,948-5,751/2s for 9s. **JUMPS to nearby allies on dispel/expire** — spread to limit jump targets; dispel sequentially. |
| Heaven's Fury | 81942 | instant ground-place | Holy | 12y radius ground zone; 18,037-20,962 Holy damage every 1.5s for 9s. **Move out of zone.** |
| Fifty Lashings | 82506 | instant self-buff | Physical | Self-buff on Barim: next 10 melee swings deal +5,000 bonus Physical. Tank/healer cooldown trigger. |
| Blaze of the Heavens (add) | NPC TBD; abilities UNVERIFIED | — | Fire | Phoenix add (heroic-only per most sources; a few sources suggest all difficulties — see **Disagreement #3**). Either burn or kite indefinitely. |
| melee | — | — | Physical | Standard tank melee |

### Abilities — Barim (Phase 2 transition)

| Spell | ID | Cast / cooldown | School | Notes |
|---|---|---|---|---|
| Repentance | 81947 | scripted | Holy | At 50% HP. **6s group AoE stun** + creates **Hallowed Ground** holy DoT zone (9,620-11,180 damage / 5s within 13y). Stun is non-dispellable. |

### Abilities — Harbinger of Darkness (Phase 2)

NPC: 43927 (Harbinger of Darkness — "dark phoenix" per Wowhead).

| Spell | ID | Cast / cooldown | School | Notes |
|---|---|---|---|---|
| Wail of Darkness | 82533 | periodic | Shadow | ~7,000 Shadow damage to all players on tick. Steady AoE pressure. |
| Soul Sever | 82255 | instant, 4s periodic | Physical | Creates **Soul Fragment** mob that moves toward Harbinger. **Must be killed / CC'd / slowed before reaching.** If reaches Harbinger: Merged Souls. |
| Merged Souls | 82263 | passive (on Soul Fragment touching Harbinger) | — | Heals Harbinger 41,625-48,375 + 20% damage buff. **Avoid by killing/slowing Soul Fragments.** |

### Interrupts required

None confirmed. Plague of Ages, Heaven's Fury, Repentance, Soul Sever — all instant.

### Dispels required

- **Plague of Ages (82622) — Disease** — mandatory on heroic; jumps if not dispelled. Multi-target = stagger dispels.
- No other dispels on this fight.

### Avoidable damage

- **Heaven's Fury** (move out of ground zone)
- **Hallowed Ground** (Phase 2 — move to clear ground after Repentance stun ends)
- **Soul Fragments** must not reach Harbinger (kill/slow/stun)

### Tank-swap requirements

None standard. Tank holds Barim → stun-window during Repentance → tank picks up Harbinger of Darkness → Harbinger dies → tank re-acquires Barim.

### Heroic delta

- Damage scales (Plague of Ages 4,947 → 5,751/tick on heroic; Heaven's Fury implied scaling)
- **Blaze of the Heavens phoenix add** in Phase 1 (heroic per most sources)
- Soul Fragments more numerous / faster on heroic (UNVERIFIED specifics)
- Plague dispel pressure higher (faster jump cycles)

### Add-handling

- **Blaze of the Heavens** (heroic Phase 1) — phoenix; ranged DPS slow-burn or kite around the room
- **Harbinger of Darkness** (Phase 2) — primary kill target; tank picks up
- **Soul Fragments** (Phase 2; spawned by Soul Sever) — DPS kill / CC / slow before reaching Harbinger

### Healer-pressure spikes

- **Plague of Ages on multiple players** (Disease bounce)
- **Heaven's Fury** if anyone gets caught in zone
- **Repentance transition** — 6s stun on entire group; all players take Hallowed Ground damage during stun if positioning failed
- **Wail of Darkness** continuous group damage in Phase 2
- **Phase 2 → Phase 1 transition** — Harbinger death effect (some sources mention burst shadow damage on Harbinger death — UNVERIFIED specific spell)

### Key positioning

- **Phase 1**: spread (Plague of Ages bounce limit) but stay in healer range; move off Heaven's Fury zones
- **Phase 1 (heroic)**: ranged DPS handles Blaze of the Heavens phoenix
- **Repentance transition**: be ready to move out of Hallowed Ground zone the moment stun breaks
- **Phase 2**: tank holds Harbinger somewhere away from boss; melee attacks Harbinger; ranged kills Soul Fragments before they reach Harbinger
- **Phase 1 resume**: re-engage Barim from spread

### Bot-relevant role notes

- **Tank**: target swap on Phase 1 → Phase 2 (Barim → Harbinger) and back. Harbinger pickup mandatory on transition.
- **Healer**: dispel Plague of Ages priority; pre-shield before Repentance stun (predictable at 50% HP); post-stun reactive AoE heal.
- **Melee**: spread in P1 (limit Plague jump); attack Harbinger in P2; Soul Fragment intercept if no ranged.
- **Ranged**: P1 — handle Blaze (heroic); P2 — kill Soul Fragments fastest (priority over Harbinger DPS).
- **Bot-impl hazard**: phase-transition stun (6s) + scripted phase change. Bot must NOT panic-cast during the stun (cast attempts are wasted). Bot must detect Harbinger spawn and treat as new threat target.

---

## Boss 4 — Siamat

NPC ID: 44819. Final boss; reached via slipstreams (wind-vortex teleporters) on the high terrace after both Lockmaw and Barim die. Two-phase encounter on a circular wind-platform with knockoff hazard.

### Phase structure

1. **Phase 1 (Winds of the South, Rise!)** — Siamat is **invulnerable** (Deflecting Winds: 90% damage reduction). Three Servants of Siamat spawn sequentially (one every ~45s or on previous Servant's death). Killing all 3 ends Phase 1. Throughout, Minions of Siamat spawn every ~30s and self-deplete; they generate Tempest Storm clouds at low HP. Cloud Burst targets random players. Storm Bolt continuous low pressure.
2. **Phase 2 (Cower Before the Storm!)** — Triggers when 3rd Servant dies. **Wailing Winds** opens phase (6s knockback + damage). Siamat attackable. Continued Minion spawns and Storm Bolts. **Gathered Storms**: Siamat absorbs nearby Tempest Storm clouds, gaining stacking 25s "blast 2 players" effect (heroic: hits all players).

### Abilities — Siamat

| Spell | ID | Cast / cooldown | School | Notes |
|---|---|---|---|---|
| Storm Bolt | 73564 | instant, ranged | Nature | Random player; 7,068-7,931 damage P1 / 14,137-15,862 damage P2. |
| Cloud Burst | 83051 | 3s warning | Nature | Ground cloud at random player; 5y radius; 21,645-25,155 Nature damage + knock-up if standing in when bursts. **Move out.** |
| Deflecting Winds (P1 boss buff) | UNVERIFIED ID (likely on boss aura table) | passive | — | Until 3rd Servant dies: 90% damage reduction on Siamat. **Burn Servants first.** |
| Wailing Winds | 83066 | 6s channel at P2 entry | Nature | 3,607-4,192 damage / sec for 6s + violent knockback. **Stay center of platform** — knockoff hazard. |
| Gathered Storms (P2) | 84987 | scripted | Nature | When Siamat absorbs a Tempest Storm: blasts 2 random players (heroic: ALL players) for 2,405-2,795 Nature / sec for 25s. **Avoid Tempest Storms reaching Siamat.** |
| melee | — | — | Physical | Standard melee |

### Abilities — Servant of Siamat (3 in P1)

NPC ID: 45269.

| Spell | ID | Cast / cooldown | School | Notes |
|---|---|---|---|---|
| Lightning Nova | 65279 | channel/PBAoE | Nature | 13,828-16,071 damage to all within 10y. **Stay outside Servant 10y range** unless tanking. |
| Thunder Crash | 84522 | scripted, on death? (UNVERIFIED — see **Disagreement #4**) | Nature | 30,062-34,937 damage to current target. Some sources call this a death-effect AoE; others a target-burst hit while alive. Default: treat as **on-Servant-death AoE** — group spreads around Servants when low HP. |
| Lightning Charge (heroic, on Servant death) | UNVERIFIED ID — some sources reference "at 4% HP" trigger | — | — | **Heroic only.** Servant deaths drop Lightning Charge buff on group: +10% damage / +10% haste / +33% damage taken. Achievement-relevant (3 stacks at end). See **Disagreement #5**. |

### Abilities — Minion of Siamat (continuous, both phases)

NPC ID: 44704.

| Spell | ID | Cast / cooldown | School | Notes |
|---|---|---|---|---|
| Chain Lightning | 103637 | hardcast | Nature | Random player + arcs to 4 additional within 10y; 10,175-11,825 initial, 70% reduction per arc. **Spread to limit arcs.** |
| Tempest Storm | 83446 | self-cast at low HP | Nature | At 5% HP, Minion forms 6y moving storm cloud; 10,237-13,162 damage / sec to anyone underneath. **Kite cloud away from Siamat** in P2 (else Gathered Storms). |
| Depletion | 84550 | passive | — | Self-DoT: -10% HP / 2s. **Minions die on their own — do not waste DPS on them.** |

### Interrupts required

- **Minion Chain Lightning** — interruptible per most sources; recommended on heroic. Bot interrupt rotation — primary kick target on Siamat.

### Dispels required

None confirmed.

### Avoidable damage

- **Cloud Burst** ground clouds (move out)
- **Tempest Storm** moving clouds (move clear; do not let reach Siamat in P2)
- **Wailing Winds** P2 entry (stay center to avoid knockoff)
- **Lightning Nova** Servant PBAoE (stay outside 10y unless tanking)
- **Thunder Crash** if death-AoE (spread on low-HP Servant)

### Tank-swap requirements

None.

### Heroic delta

- **Lightning Charge** stacks on group from Servant deaths (achievement-relevant; bot need not optimize for achievement, just survive)
- **Gathered Storms hits ALL players** (vs 2 players normal) — significantly higher healer pressure in P2
- Tighter mechanic windows; faster Servant cycle implied
- Kill Lightning Charge / Servant interaction is the key heroic delta

### Add-handling

- **Servants (3 in P1)**: kill priority absolute; tank picks up; group stays out of Lightning Nova range
- **Minions (continuous, both phases)**: **DO NOT KILL** — they self-destruct; killing wastes DPS and may trigger early Tempest Storm. Interrupt Chain Lightning as a free-action.
- **Tempest Storms**: in P2, kite/avoid; do not let reach Siamat (Gathered Storms trigger)

### Healer-pressure spikes

- **Servant Lightning Nova** on group (if anyone in 10y)
- **Wailing Winds P2 entry** (6s channeled raid damage)
- **Gathered Storms P2** (especially heroic — hits all)
- **Cloud Burst** on cluster
- **Storm Bolt** continuous low pressure

### Key positioning

- **Tank**: pin Siamat near platform center; Servants brought to tank; Minions tanked-by-self (they self-destruct)
- **Group**: spread (Chain Lightning arc avoidance); outside Servant 10y; **stay center for Wailing Winds knockback**
- **P2**: same as P1 spread + clear Tempest Storms from Siamat path

### Bot-relevant role notes

- **Tank**: standard threat on Servants; pin Siamat; Minions self-deplete (don't taunt them, just hold position)
- **Healer**: P2 Gathered Storms is the spike; pre-cooldown
- **Melee**: kill Servants in P1; switch to Siamat in P2; do NOT cleave Minions
- **Ranged**: same kill order; interrupt Minion Chain Lightning on rotation; spread to break arcs

---

## Trash mechanics

Critical trash has unique mechanics worth automating. Standard packs (basic Tol'vir grunts, low-elite mobs) are pure threat-and-DPS and not enumerated.

### Trash before Husam (outer courtyard)

- **Tol'vir Stoneshaper** — caster trash. Hardcasts an Earth Shock-style Nature damage cast (some sources call it Lightning Bolt / Earth Bolt — name varies, but the cast is **interruptible**). **MUST_INTERRUPT.** CC valid (Polymorph / Hex / Sap / Banish if elemental — need NPC type confirmation, default to humanoid CC).
- **Neferset Plaguebringer** — caster trash with **disease-flavor casts**. Interrupt; CC valid.
- **Neferset Theurgist** — magic caster; interrupt.

### Trash between Husam and Lockmaw / Barim

- **Tol'vir Oathblade** — melee with cleave/swipe; tank face-management.
- **Oathsworn Axemaster** — heavy melee; possibly cleaves; tank-and-spank.
- **Hahk'Zor** (named patrol — UNVERIFIED) — pat that can wipe groups if pulled into another pack.

### Trash before Siamat (slipstream / high terrace)

- **Wind-flavor adds** (NPC types vary by source) — air elementals or Tol'vir guards. Mostly tank-and-spank.
- **Skarf Gorespitter** (UNVERIFIED named patrol) — high HP single mob.

### CC priorities

- **Polymorph / Hex / Sap / Repentance** valid on humanoid Tol'vir / Neferset trash
- **Banish** for any ethereal / elemental trash (UNVERIFIED specific mob types)
- **Stoneshapers** are likely humanoid → standard humanoid CC

### Skip-able trash

- A few patrols can be wall-hugged or pulled cleanly with line-of-sight
- **No major optional branch** like Beauty in Blackrock Caverns — both Lockmaw and Barim are mandatory before Siamat unlocks

### Pull markers for CC

- **3-pull caster groups** (Stoneshapers / Plaguebringers): skull on lead caster (kill priority), cross/X on second (CC)
- **Mixed melee+caster packs**: skull on caster, melee tanked

---

## Disagreements

Numbered list of points where the reconciled guides do not agree. Bot logic should pick a default and note the deviation.

### Disagreement #1 — Lockmaw → Augh trigger and phase structure

- **Source A (Wowhead NPC + Icy Veins):** Augh is "immune to damage while Lockmaw lives" — clear two-phase: Lockmaw first, Augh second.
- **Source B (Altered Gamer):** Augh "randomly jumps in, whirlwinding" during Lockmaw fight on normal mode; Augh as standalone fight is heroic-only.
- **Source C (Dexerto):** Augh is "after a brief break" once Lockmaw dies — sequential.
- **Source D (Ten Ton Hammer):** "Aug returns with full health after dying and must be defeated again" implying Augh dies once during Lockmaw fight then re-emerges.

**Default:** Treat as **two-phase: Lockmaw alive → Augh immune; Lockmaw dies → Augh becomes attackable**. On heroic, Augh is a full second-phase fight. Bot detects Augh-as-active-target via `target.is_attackable(Augh) AND target.hp_pct < 100`.

### Disagreement #2 — Barim Phase 2 spirit-realm split: same room or separate phase?

- **Source A:** Phase 2 happens in the same room; Barim becomes immune; Harbinger pops in.
- **Source B (some forum strats):** Players are pulled into a "spirit realm" — a separate phased instance — for Phase 2.
- **Wowhead NPC pages:** No explicit teleport mechanic listed; Repentance is described as a stun + AoE, not a phase-shift.

**Default:** Treat as **same room, no teleport**. Barim becomes immune; Harbinger spawns in the room; group fights Harbinger while avoiding Hallowed Ground zone. **Bot-impl flag**: if private-server scripts implement a phase teleport, bot's threat / target / position tracking must handle it gracefully — record this as a **bot-implementation hazard** in encounter.md.

### Disagreement #3 — Blaze of the Heavens phoenix add: heroic-only or all difficulties?

- **Source A (Icy Veins, Dexerto):** Heroic-only Phase 1 add.
- **Source B (Altered Gamer):** Spawns in all difficulties; one of Barim's standard P1 mechanics.
- **Source C (Ten Ton Hammer):** Doesn't mention.

**Default:** Assume **heroic-only** but program detection on add-spawn event so behavior is correct regardless of difficulty.

### Disagreement #4 — Siamat Servant Thunder Crash: live cast or death effect?

- **Source A (Wowhead NPC page):** Thunder Crash is a 30k damage attack on the Servant's current target (i.e., live cast — the tank takes it).
- **Source B (Altered Gamer):** "Stay away from Servants immediately after they die to avoid their Thunder Crash death ability" — death AoE.
- **Source C (Dexerto):** Death effect on killed Servants.

**Default:** Treat as **both possible** — bot stays at 11+ yards from low-HP Servants AND tank pops defensive on Thunder Crash cast warning. Prevents both scenarios.

### Disagreement #5 — Heroic Lightning Charge stacking mechanic

- **Source A (Dexerto):** Servants emit Lightning Charge at 4% HP; +10% damage / +10% haste / +33% damage taken; achievement requires 3 stacks at end.
- **Source B (Altered Gamer):** Lightning Charge mentioned in the achievement context but not as a continuous mechanic.
- **Source C (Wowhead NPC):** No explicit Lightning Charge stack listing in the Siamat NPC page (only Servant abilities listed).

**Default:** Treat as **heroic-only achievement-relevant buff**; bot does not attempt to optimize for achievement. Stay alive through stacking damage taken; treat as **healer pressure scaling with Servant kill count**.

### Disagreement #6 — Trash mob CC on Tol'vir Stoneshaper

- **Source A:** Polymorph valid (humanoid).
- **Source B:** Stoneshaper is elemental-aspect; needs Banish.
- **Source C:** Hex (frog) works regardless.

**Default:** **Hex (Shaman)** is the safest — works on both humanoid and elemental. Polymorph as fallback. Banish if Mage / Warlock and target turns out to be elemental. If first CC fails, swap CC class on next pull.

### Disagreement #7 — Augh appears at Lockmaw 30% (some sources) vs Lockmaw death (others)

- **Source A (Icy Veins):** "Immune to damage while Lockmaw lives" — Augh is alive during Lockmaw fight but unattackable until Lockmaw dies.
- **Source B (Altered Gamer):** Augh "randomly jumps in" mid-Lockmaw on normal — interactive presence during Lockmaw fight.
- **Source C (Dexerto):** Augh appears after Lockmaw "in two distinct phases."

**Default:** Bot model: **Augh is present in the room from pull but unattackable until Lockmaw dies**. Augh's intermittent jump-in mechanics on normal mode (per Source B) treated as movement / Whirlwind hazard during Lockmaw fight. Group avoids Augh's location until ready to engage.

### Disagreement #8 — Heroic Plague of Ages dispel: yes or "let it ride"?

- **Source A:** Dispel ASAP (standard disease-dispel logic).
- **Source B:** Wait for it to time out; dispel jumps to nearby ally and refreshes.
- **Source C:** Dispel only when the carrier is isolated (no jump targets in 8y).

**Default:** **Dispel always** if disease-cleanse is available; spread to limit jump radius. The "let-it-ride" approach increases total damage taken — even if a dispel-jump occurs, the new carrier's clock resets and total ticks decrease.

### Disagreement #9 — Siamat platform knockoff is wipe or recoverable?

- **Source A (Icy Veins, Dexerto):** Falling off the platform during Wailing Winds is a wipe.
- **Source B (Altered Gamer):** Knockoff is recoverable — slipstream re-deposits players.

**Default:** Treat as **wipe risk**. Stay center during Wailing Winds; do not stand at platform edge.

---

## DBC verification (UNVERIFIED items)

Spell IDs and stack values listed below are from community-guide cross-reference, not direct DBC lookup. Spell IDs that reconciled across Wowhead Cata + at least one strategy guide are NOT flagged here.

| # | Spell / mechanic | Listed value | Why it matters for bot logic |
|---|---|---|---|
| 1 | Husam Bad Intentions effective range / pillar-slam path | 100y radius listed | Bot must not predict landing position; just react to charged-target hp drop |
| 2 | Husam Detonate Traps countdown timing | 5s warning | Bot escape window from arming traps |
| 3 | Husam Mystic Trap arming time | ~2s (sources vary) | Bot trap-escape decision window; predicate `aura.expires_in_ms(trap_arm) < N` |
| 4 | Lockmaw Venomous Rage enrage-strippable | UNVERIFIED | If strippable, Hunter Tranquilizing Shot / Druid Soothe / etc. would be high-priority; default treats as non-removable |
| 5 | Augh appearance trigger | "Lockmaw dies" vs "Lockmaw 30%" vs "random jump-in" | Bot fight-state logic — when does Augh become attackable? |
| 6 | Augh Smoke Bomb tank-stun duration | 3s | Off-tank coverage window |
| 7 | Barim Repentance stun duration | 6s | Bot post-stun reaction timing |
| 8 | Barim Hallowed Ground radius | 13y | Bot post-stun escape radius |
| 9 | Barim Soul Sever periodic | 4s tick | Bot Soul Fragment intercept cadence |
| 10 | Barim Phase 2 → Phase 1 transition spell | UNVERIFIED — Harbinger death effect | Bot detection of Barim re-engageable |
| 11 | Blaze of the Heavens NPC ID | UNVERIFIED | Bot add-target identification for heroic P1 |
| 12 | Blaze of the Heavens spawn condition (heroic vs all) | UNVERIFIED | Difficulty-gated add detection |
| 13 | Harbinger of Darkness NPC ID | 43927 (per Wowhead) | Confirmed but not flagged elsewhere — verify |
| 14 | Soul Fragment NPC ID | UNVERIFIED | Bot Soul Fragment kill-target identification |
| 15 | Siamat Deflecting Winds spell ID | UNVERIFIED | Bot phase-detection: P1 ↔ P2 trigger |
| 16 | Siamat Lightning Charge spell ID (heroic) | UNVERIFIED | Bot stack-tracking on group for healer pressure scaling |
| 17 | Siamat Servant low-HP threshold for Thunder Crash | UNVERIFIED (some say "at 4%") | Bot Servant kill-and-step-back timing |
| 18 | Servant of Siamat Lightning Nova radius | 10y | Bot range avoidance from Servants |
| 19 | Minion of Siamat Tempest Storm trigger HP | 5% | Bot Minion-low-HP avoidance trigger |
| 20 | Minion Chain Lightning interruptibility | likely yes | Bot interrupt-rotation primary on Siamat |
| 21 | Plague of Ages jump radius | UNVERIFIED — likely 8y | Bot spread distance for Disease bounce limit |
| 22 | Trash spell IDs (Stoneshapers, Plaguebringers, etc.) | UNVERIFIED across the board | Trash interrupt rotation specifics |
| 23 | Heroic Detonate Traps cooldown / cadence | UNVERIFIED | Bot heroic Husam kite frequency |
| 24 | Tol'vir Stoneshaper creature_type (humanoid vs elemental) | UNVERIFIED | CC selection logic (Polymorph vs Banish vs Hex) |

Add to `docs/research/dbc-verification-checklist.md` for resolution when the TC fork session opens.

---

## Notes on prompt deviations

- **Lockmaw poison-dispel mana drain is flagged as the canonical mana-test of this dungeon** per the prompt — equivalent to Karsh's role in Blackrock Caverns. The encounter.md and healer.md docs both highlight this.
- **Barim spirit-phase split is flagged as a bot-implementation hazard** in encounter.md instance-wide notes. Default assumption is "same room, no teleport" but the bot's encounter handler must be defensive against the alternative interpretation if the private-server scripts implement a phase teleport.
- **Augh is treated as part of the Lockmaw encounter**, not a separate boss — it's mechanically a phase 2 of the Lockmaw fight. Surveys some sources count as 5 bosses; Wowhead canonical structure lists 4.
- **Heroic Lightning Charge mechanic** on Siamat is acknowledged but bot does not optimize for the 3-stack achievement — survival is the only goal.
- **No Beauty-equivalent optional boss** — both Lockmaw and Barim are mandatory before Siamat unlocks. There's no "skip path" the bot needs to flag.
- **Wowhead Cata is the spell-ID source of record**; where it disagreed with strategy guides, Wowhead won.
- **Many trash mob spell IDs are UNVERIFIED** — community guides rarely list trash spell IDs precisely. The DBC checklist contains a generic "trash spell IDs" entry rather than enumerating each.
- **Schools and dispel-types confirmed via Wowhead** for the load-bearing dispels: Plague of Ages = Disease, Viscous Poison = Poison, Paralytic Blow Dart = Poison. These are the doc-critical entries; everything else is secondary.

# End Time — Cataclysm 5-man Survey

**Date:** 2026-05-02
**Patch:** 4.3 / 4.3.4 (Hour of Twilight content patch)
**Audience:** bot-control engineer (cata-altbot)

---

## Overview

| Field | Value |
|---|---|
| Zone | End Time (a shattered future Dragonblight, accessed via Caverns of Time, Tanaris) |
| Continent | Kalimdor (CoT portal hub) |
| Level range (normal) | **N/A — no normal mode exists** |
| Level range (heroic) | 85 only |
| Item level requirement (LFD heroic) | iLvl 353 |
| Item level reward | iLvl 378 (equivalent to normal Firelands) |
| Group size | 5 (1 tank / 1 healer / 3 DPS) |
| Boss count | **5** total per run, drawn from a 6-boss roster: 2 of 4 random "Echo of {hero}" bosses + Echo of Murozond (always last) |
| Random echo pool | Echo of Baine · Echo of Jaina · Echo of Sylvanas · Echo of Tyrande |
| Final boss | **Echo of Murozond** — Bronze Dragonshrine, always spawns after the two echoes are dead |
| Layout | **Hub-and-spoke** — central Time Transit Device teleports group to Dragonshrine instances; two echo shrines selected at random |
| Optional bosses | None — all 5 encounters are required for completion / quest credit |
| Cata-tier class | **Patch 4.3 troll-heroic-replacement** dungeon; gates Dragon Soul raid (along with Hour of Twilight + Well of Eternity) |
| Group buffs/debuffs | None instance-wide |

End Time is one of three "Hour of Twilight" patch-4.3 5-mans (alongside Hour of Twilight and Well of Eternity). Together the three serve as the troll-heroic replacement set in Cata's final content patch. **Heroic-only** — there is no normal-mode version. End Time is the **first** of the three (LFD-rotation order); completing it unlocks the questline that progresses through Hour of Twilight → Well of Eternity → Dragon Soul.

The instance has a unique **2-of-4 random boss** mechanic that the bot must reactively detect. Each run, the Time Transit Device (central hub) opens portals to **two of four** Dragonshrines:

- Echo of Baine — Obsidian Dragonshrine
- Echo of Jaina — Azure Dragonshrine
- Echo of Sylvanas — Ruby Dragonshrine
- Echo of Tyrande — Emerald Dragonshrine

After both echoes are killed, the Bronze Dragonshrine portal opens and the group fights **Echo of Murozond** (the corrupted future Nozdormu). The Murozond fight is centered on a unique encounter object — the **Hourglass of Time** — that resets cooldowns, HP, mana, and removes ground hazards when clicked, with a hard cap of 5 uses per fight.

End Time is regarded as the easiest of the three 4.3 troll heroics. It serves as the entry-level introduction to the patch's mechanics. Mechanically it is mostly tank-and-spank with one or two unique mechanics per boss. A geared group clears in ~15-20 min on heroic.

---

## Sources

This survey reconciles the following community sources. Wowhead Cata is treated as authoritative for spell IDs and numeric values; strategy is cross-checked across 5 distinct authors.

1. **Wowhead Cata Classic — Encounter Journal pages** (`/cata/npc=...`) — primary source for spell IDs and ability numerics. Echo of Jaina (`npc=54445`), Echo of Baine (`npc=54431`), Echo of Sylvanas (`npc=54123`), Echo of Tyrande (`npc=54544`), Murozond (`npc=54432`).
2. **Icy Veins — End Time Dungeon Guide (Cataclysm Classic)** — `https://www.icy-veins.com/cataclysm-classic/end-time-dungeon-guide` — strongest editorial coverage; per-boss strategy + Hourglass usage planning + achievement notes.
3. **Warcraft Wiki — per-boss tactics pages** — `warcraft.wiki.gg/wiki/Echo_of_Baine`, `Echo_of_Jaina`, `Echo_of_Sylvanas`, `Echo_of_Tyrande`, `Murozond_(tactics)` — best for spell IDs and exact damage values.
4. **Warcraft Tavern — End Time Dungeon Guide** — `warcrafttavern.com/cataclysm/guides/end-time-dungeon-guide/` — secondary editorial; cross-check on positioning + interrupt priorities.
5. **EpicCarry — End of Time Dungeon Guide (Cata Classic)** — `epiccarry.com/blogs/wow-cataclysm-classic-end-of-time-guide/` — community guide; useful for confirming heroic-only status + iLvl gear gates.
6. **TenTonHammer — Patch 4.3 End Time Heroic Guide** (original-Cata era) — `tentonhammer.com/articles/wow-patch-4-3-end-time-heroic-dungeon-guide` — original-launch perspective with HP totals + Hourglass tactical notes.
7. **WowWiki Archive / Wowpedia — Hourglass of Time, per-boss pages** — fallback for original-Cata values and strategy nuance.
8. **Tentonhammer + Wowhead community guide threads** — heroic-only confirmation; trash composition (Time-Twisted Breaker / Drake / Seer / Rifleman packs).

Where guides disagree, the disagreement is captured in the **Disagreements** section and the consensus position is listed in **Strong consensus**.

---

## Strong consensus (table)

| Mechanic | Consensus |
|---|---|
| **Heroic-only dungeon** | Confirmed across all sources — no normal mode. Level 85 entry; LFD iLvl 353 gate. |
| **2 of 4 echoes per run, random selection** | Confirmed across all sources. Pool: Baine, Jaina, Sylvanas, Tyrande. The bot CANNOT pre-plan which two will spawn — must reactively detect. |
| **Murozond is always 5th boss** | Confirmed. Bronze Dragonshrine portal opens only after both echoes are dead. |
| **Hub-and-spoke layout** | Time Transit Device in center; portals to Dragonshrines open in scripted order (echoes first, Murozond last). |
| **Echo of Jaina — interrupt Pyroblast + Frostbolt Volley** | Pyroblast is heavy tank damage (single-target); Frostbolt Volley is AoE. Both are interruptible per Wowhead encounter journal. Pyroblast prioritized. |
| **Echo of Jaina — Flarecore embers** | Spawn on cast; must be detonated within ~5s by walking into them; if uncontested, room-wide AoE damage. **Designated player walks the embers**, NOT melee just running over them randomly. |
| **Echo of Jaina — Frost Blades 5s stun** | Three frost projectiles in cone; stand between blade lanes to avoid. |
| **Echo of Baine — kill before platform 4 sinks** | Hard DPS check. Pulverize sinks one platform per cast (~50s). 4 platforms total → can lose 3, must kill before 4th sinks. |
| **Echo of Baine — Throw Totem mechanic** | Boss throws totem at random player; **player picks up + throws back at boss** for stun + 5% max HP damage + 50% damage-taken-up debuff. ~20s window. **Bot must reactively pick up + return totem**. |
| **Echo of Baine — Molten Axe lava interaction** | Standing in lava grants Baine a **damage-up buff** (Molten Axe / Molten Mace — name varies by source). Tank avoids lava. **Note Disagreement #1** about whether DPS should intentionally enter lava for any benefit. |
| **Echo of Sylvanas — Calling of the Highborne ghoul circle** | Sylvanas channels; pulls group to her; 8 Risen Ghouls spawn in circle with shadowy links between them; ghouls march inward; group must DPS **one ghoul** to break the link and escape through the gap before ghouls reach Sylvanas (Sacrifice = wipe). |
| **Echo of Sylvanas — Wracking Pain DoT** | Crossing a shadowy link between ghouls or standing in shadow zones behind ghouls applies Wracking Pain ticks. Avoid links; kill ONE ghoul to create the gap. |
| **Echo of Sylvanas — spread for Black Arrow / Unholy Shot** | Both are AoE-around-target / multi-target; spread 8-10y minimum between players. |
| **Echo of Tyrande — interrupt Stardust as TOP PRIORITY** | Stardust is 60y AoE arcane, ~50k damage; missing the kick = wipe-grade pressure. **Highest-priority interrupt of any boss in End Time.** |
| **Echo of Tyrande — Lunar Guidance haste stacks** | Buff stacks at 80% HP and 55% HP; +25% cast speed per stack. Makes Stardust harder to kick — but kicking it is still the priority. |
| **Echo of Tyrande — Tears of Elune at 30% HP** | Stars rain on player feet, 4y radius arcane. Burn through this phase — **don't kite**, kill the boss. |
| **Echo of Tyrande — Dark Moonlight aura** | -50% cast speed within 15y of boss; healers + caster DPS stay outside the aura. Tank is inside (necessarily). |
| **Echo of Tyrande — Eyes of the Goddess / Piercing Gaze** | Mobile silence-orb circles room; avoid contact. |
| **Murozond — Hourglass of Time** | Click-interactable encounter object; cast **Rewind Time** which: (a) resets all player cooldowns, (b) restores HP/mana to full, (c) clears Distortion Bombs from arena, (d) revives dead party members, (e) teleports players to start positions. **Does NOT affect Murozond's HP** — damage on him persists. **Hard cap of 5 uses per fight.** |
| **Murozond — Distortion Bomb** | Targets random player; lands at their location; leaves 8y radius temporal-distortion zone for 25k arcane/sec until end of fight or Hourglass reset. **Permanent floor hazard** — over time arena fills. |
| **Murozond — Temporal Blast** | Hits all nearby players every 15s; applies stacking +10% arcane-damage-taken debuff, 20s duration. Stacks ramp damage; Hourglass clears the stacks. |
| **Murozond — Infinite Breath** | Frontal cone fire breath; standard tank-face-away. Some sources include a **Tail Sweep** behind boss (knockback + DoT). |
| **Murozond — DPS race with Hourglass cushion** | Strategy is "kill before Hourglass charges run out." Higher-DPS groups use early to reset CDs for burst windows; lower-DPS groups save for emergencies. |
| **Hourglass click assignment** | Best on a **mobile / port-capable ranged** (Mage with Blink, Warlock with Demonic Circle) for fast safe travel to the hourglass. Bot default: assign to the ranged DPS with the highest mobility. |

---

## Boss 1 (random) — Echo of Baine

NPC ID: 54431. Random selection from echo pool. Located at Obsidian Dragonshrine — multi-platform area surrounded by lava.

### Phase structure

Single phase with progressive platform loss. The fight is structured around a hard DPS check: kill Baine before he sinks all 4 platforms.

1. Pull on starting platform; tank-and-spank
2. Periodic **Pulverize** — leaps to a ranged player and sinks the platform he lands on
3. Group rotates platforms over the course of the fight; lava swim between platforms applies stacking damage
4. **Throw Totem** event — boss tosses totem at random player; player retrieves + throws back for stun + damage debuff
5. Kill before platform 4 sinks → wipe by lava-stacking-damage

### Abilities

| Spell | ID | Cast / cooldown | School | Notes |
|---|---|---|---|---|
| Pulverize | 101626 (UNVERIFIED — see DBC list) | leap, ~50s cd | Physical | Boss leaps to ranged player; deals ~30k physical damage on landing; **sinks the platform** he lands on. Group must rotate platforms. |
| Molten Mace / Molten Axe | 101836 (UNVERIFIED) | passive (lava-triggered) | Fire | Buff acquired when boss touches lava — adds ~10k fire damage to melee swings. Tank avoids lava to prevent. **Wording varies across sources — see Disagreement #1.** |
| Throw Totem | 101614 (UNVERIFIED) | ranged toss, ~20-30s cd | Physical | Tosses totem at random player for ~60k damage + knockback. **The thrown totem can be picked up + thrown back at boss** for stun + 5% max HP damage + 50% damage-taken-up debuff for 20s. |
| Melee | — | — | Physical | Standard tank melee |

### Interrupts required

None. Pulverize is a leap, not a hardcast; Throw Totem is a leap-toss, not a kickable hardcast.

### Dispels required

None.

### Avoidable damage

- **Lava** — group avoids; tank specifically avoids to prevent Molten Mace buff on boss
- **Pulverize landing zone** — ranged players spread so Pulverize doesn't multi-impact
- **Sinking platform** — when Pulverize lands, that platform sinks; everyone on it must swim/jump to next platform

### Tank-swap requirements

None.

### Heroic delta

End Time is heroic-only — no delta exists. All numerics here are heroic numerics.

### Add-handling

None. No adds spawn during this fight; mechanic is purely environmental.

### Healer-pressure spikes

- Lava-swim damage when Pulverize forces platform rotation (group-wide)
- Throw Totem direct hit (~60k single-target on whoever it lands on)
- Standard tank melee (low pressure)

### Key positioning

- **Tank**: holds boss on starting platform; **stays out of lava**
- **Melee DPS**: in melee on boss; stays out of lava
- **Ranged DPS / healer**: spread to opposite platform (Pulverize jumps to ranged) — survives platform sink because they have room to move
- After Pulverize sinks a platform, **everyone shifts to the next platform**

### Bot-relevant role notes

- **Tank**: keep boss centered; never voluntarily enter lava (Molten Mace buff is detrimental); be ready to swim to next platform
- **Healer**: ranged-positioning; HoT-roll group through lava-swim damage spikes; pre-shield Pulverize target
- **Melee**: in melee; hop platforms when forced; no lava
- **Ranged**: **PRIMARY THROW-TOTEM HANDLER** — bot must reactively detect Throw Totem cast event, locate the totem on the ground, click-interact to pick up, then re-throw at boss. This is a **fight-specific bot ability** not in the standard rotation engine.
- All roles: **DPS race awareness** — push damage; kill before platform 4 sinks

---

## Boss 2 (random) — Echo of Jaina

NPC ID: 54445. Random selection from echo pool. Located at Azure Dragonshrine. **Pre-fight requires gathering 16 Jaina staff fragments** scattered around the shrine before she will engage.

### Phase structure

Single phase with stacking mechanics. Boss Blinks frequently — high-mobility fight.

1. Pre-fight: collect 16 Jaina fragments (all DPS contributes); clear Priest + Sorceress trash first
2. Pull triggered when fragments are gathered; tank picks her up
3. Continuous **Frostbolt Volley + Pyroblast** rotation; bot interrupt rotation primary kick load
4. Periodic **Frost Blades** cone + **Flarecore** embers + **Blink** repositioning
5. Push DPS; no enrage timer per se but mechanics compound

### Abilities

| Spell | ID | Cast / cooldown | School | Notes |
|---|---|---|---|---|
| Pyroblast | 101809 (UNVERIFIED) | ~3s cast, ~15-20s cd | Fire | Heavy single-target hit on tank (~50k) + 12s DoT (~5k/3s). **Interruptible** — kick rotation primary target. |
| Frostbolt Volley | 101810 (UNVERIFIED) | ~2s cast, ~20-30s cd | Frost | AoE 65y range — ~18k frost damage to all enemies + 30% slow 4s. **Interruptible** — kick rotation secondary. |
| Flarecore | 101927 (UNVERIFIED) | scripted | Fire | Spawns fire-orb ground patches; **detonate within 5s by walking into them** for ~5-50k damage to nearby player; if uncontested, room-wide ~94k explosion. **Mechanic: designated player(s) sacrifice HP to detonate early.** |
| Frost Blades | 101339 (UNVERIFIED) | ~2s cast | Frost | 3 expanding frost projectiles in forward cone; hit = 5s stun. **Stand BETWEEN blade lanes to avoid.** |
| Blink | 101812 (UNVERIFIED) | ~10-15s cd | Arcane | Boss teleports to a new room location; tank chases. High mobility. |
| Melee | — | — | Physical | Mage caster boss has low melee — tank takes minimal melee damage |

### Interrupts required

- **Pyroblast** — PRIMARY kick target. Heavy tank damage if landed.
- **Frostbolt Volley** — SECONDARY kick target. AoE so smaller-per-player but compounds with Pyroblast pressure.

### Dispels required

- **Frostbolt Volley slow** — Magic, slow component dispellable on tank if kick missed (low priority — slow is short).

### Avoidable damage

- **Frost Blades** — stand between cone projectiles
- **Flarecore** — detonate early (1 player walks into) OR avoid (room-wide explosion if no one detonates — bad)
- **Blink follow-up** — tank chases boss; melee follow tank

### Tank-swap requirements

None.

### Heroic delta

Heroic-only — no delta.

### Add-handling

None during boss fight. Pre-fight trash includes **Priests** (caster, kill priority) and **Sorceresses** (caster) — clear before engaging Jaina.

### Healer-pressure spikes

- Pyroblast direct hit on tank (if not interrupted) + 12s DoT — biggest tank pressure
- Frostbolt Volley AoE — group-wide light damage
- Flarecore detonator — whoever walks the ember takes 5-50k single-target

### Key positioning

- **Tank**: holds boss; **chases on Blink** (boss moves frequently); face away from group (no actual cone but discipline)
- **Melee**: behind boss; chase tank on Blink
- **Ranged DPS / healer**: spread for Frostbolt Volley AoE positioning; max range from boss for Frost Blades safety
- **Flarecore designated detonator**: 1 player walks ember within 5s — typically melee (closer) or whoever has highest HP buffer

### Bot-relevant role notes

- **Tank**: chase Blink fast; Mage classes typically port to predictable spots — bot can pre-plan if tracking Blink scripts (UNVERIFIED reliability)
- **Healer**: heaviest interrupt-failure pressure on this fight; pre-HoT tank before Pyroblast cast
- **Melee**: Frost Blades cone awareness; chase Blink
- **Ranged**: Frostbolt Volley spread; can be Flarecore detonator if HP is full (controlled damage)
- **All DPS with kicks**: Pyroblast > Frostbolt Volley interrupt rotation
- **Pre-fight**: bot should support fragment collection (each player picks up adjacent fragments while clearing trash)

---

## Boss 3 (random) — Echo of Sylvanas

NPC ID: 54123. Random selection from echo pool. Located at Ruby Dragonshrine.

### Phase structure

Single phase with periodic **Calling of the Highborne** ghoul-circle event (the fight's signature mechanic).

1. Pull; tank-and-spank with periodic Black Arrow / Unholy Shot / Shriek of the Highborne ranged casts
2. Periodic **Calling of the Highborne** event — Sylvanas ascends into the air; pulls all players to her; spawns 8 Risen Ghouls in a circle with shadowy links between them; ghouls march inward toward Sylvanas
3. Group focuses **one ghoul** to break the circle; escape through the gap before ghouls reach Sylvanas (Sacrifice = wipe)
4. Repeat

### Abilities

| Spell | ID | Cast / cooldown | School | Notes |
|---|---|---|---|---|
| Calling of the Highborne | UNVERIFIED | ~3-5s channel | Shadow | Pulls group to Sylvanas; spawns 8 Risen Ghouls in circle; ghouls march inward; **kill 1 ghoul to break circle**; ghouls reaching Sylvanas cast Sacrifice = wipe |
| Black Arrow | UNVERIFIED | ~2s cast | Shadow | AoE projectile on random player; ~10y radius |
| Unholy Shot | UNVERIFIED | ~2s cast | Shadow | Multi-target missile barrage; hits up to 3 nearby players + DoT |
| Shriek of the Highborne | UNVERIFIED | instant | Shadow | Slow on random player (-50% MS, 30s) + shadow damage tick |
| Blighted Arrows | UNVERIFIED | scripted | Shadow | Ground rune marks; standing in rune > 5s = lethal damage |
| Wracking Pain (ghoul aura) | UNVERIFIED | passive | Shadow | Standing in shadowy link between ghouls OR shadow zone behind ghoul = damage tick per second |
| Risen Ghoul melee | UNVERIFIED | melee | Physical | Slow march toward Sylvanas; melee on closest player |
| Sacrifice (ghoul fail-state) | UNVERIFIED | scripted on reaching Sylvanas | Shadow | If a ghoul reaches Sylvanas alive, the encounter is effectively wiped |

### Interrupts required

None confirmed. No hardcast-kick targets.

### Dispels required

- **Shriek of the Highborne slow** — Magic; -50% MS for 30s. Dispellable; high-value when Calling of the Highborne ghoul-event is incoming (slowed player can't escape ghouls in time).

### Avoidable damage

- **Black Arrow / Unholy Shot** — spread 8-10y minimum between players
- **Blighted Arrows ground runes** — move out of rune within 5s
- **Ghoul shadow zones** — during Calling of the Highborne, do NOT cross links / stand in shadows

### Tank-swap requirements

None.

### Heroic delta

Heroic-only — no delta.

### Add-handling

- **8 Risen Ghouls** spawn during Calling of the Highborne; **focus 1 ghoul** to break the circle; remaining ghouls die when boss dies (or are reset on next Calling)
- Achievement **Severed Ties** requires killing 2+ ghouls during Calling (high-DPS groups only)

### Healer-pressure spikes

- Multi-target Unholy Shot tick (3 players DoT)
- Blighted Arrows ground-rune damage if affected player is slow to move
- Wracking Pain ticks during Calling event if anyone clips a link
- **Worst case**: ghoul reaches Sylvanas → Sacrifice → wipe (not healable)

### Key positioning

- **Spread always** (Black Arrow, Unholy Shot, Blighted Arrows all need spread)
- **During Calling of the Highborne**: group is pulled to boss; immediately focus 1 ghoul (closest to raid marker / pre-call); kill it; escape through gap; spread again

### Bot-relevant role notes

- **Tank**: standard tank-and-spank; on Calling event, focus the ghoul kill (tank's threat doesn't matter to ghouls — they march scripted toward boss)
- **Healer**: rotate dispels on Shriek slows when Calling is imminent; reactive heal Blighted Arrow targets
- **Melee + Ranged**: spread always; on Calling, **all DPS focus same ghoul** — explicit target-coordination state for the bot ("everyone on ghoul N closest to raid marker / lowest HP ghoul")
- **Achievement note**: Severed Ties (kill 2+ ghouls during Calling) is bot-optional

---

## Boss 4 (random) — Echo of Tyrande

NPC ID: 54544. Random selection from echo pool. Located at Emerald Dragonshrine. **Pre-fight trash mechanic**: trash mobs have 90% damage reduction outside moonbeams; tank drags mobs into rotating moonbeam to remove the shield.

### Phase structure

Single phase with two HP-gated cast-haste stacks + 30% HP execute phase.

1. Pre-fight trash: kill mobs by dragging them into rotating moonbeams (West → South → East → North, ~45s rotation)
2. Pull boss; tank-and-spank with **Stardust** kick rotation as primary load
3. **Lunar Guidance** stack 1 at 80% HP (+25% cast speed)
4. **Lunar Guidance** stack 2 at 55% HP (+50% cast speed total)
5. **Tears of Elune** at 30% HP — stars rain on feet, burn phase

### Abilities

| Spell | ID | Cast / cooldown | School | Notes |
|---|---|---|---|---|
| Stardust | 102173 (UNVERIFIED) | ~2s cast, ~10-15s cd | Arcane | 60y AoE; ~50k arcane to ALL players. **PRIMARY KICK TARGET** — must be interrupted every cast. Highest-priority interrupt of any boss in End Time. |
| Moonbolt | 102193 (UNVERIFIED) | ~2s cast | Arcane | Direct ~40k arcane on current target. **Interruptible? UNVERIFIED — see Disagreement #2.** |
| Moonlance | 102149 (UNVERIFIED) | ~2-3s cast | Arcane | Ground lance travels along floor; splits into 3 at light beam edge; ~50k + 5s stun on hit. Avoid by moving out of lane. |
| Dark Moonlight | 102414 (UNVERIFIED) | passive aura | Shadow | -50% cast speed within 15y of boss. Affects healers + caster DPS — they stand outside aura. |
| Lunar Guidance | 33589 (UNVERIFIED — likely re-used Cata spell ID) | passive, HP-gated | — | Boss buff: +25% cast speed. Stack 1 at 80% HP, stack 2 at 55% HP (=+50% total). |
| Tears of Elune | 102241 (UNVERIFIED) | passive, HP-gated | Arcane | At 30% HP: stars continuously drop at player feet; ~30k arcane in 4y radius. **Burn phase** — kill the boss; do NOT kite. |
| Eyes of the Goddess / Piercing Gaze of Elune | UNVERIFIED | scripted spawn | Holy | Mobile silence-orb circles room; contact = silence. Avoid path. |
| Melee | — | — | Physical | Caster boss, low melee |

### Interrupts required

- **Stardust** — TOP PRIORITY interrupt of the entire dungeon. 60y AoE; missing the kick = ~50k group damage = potential wipe
- **Moonbolt** — secondary if available (UNVERIFIED interruptibility)

### Dispels required

None confirmed. (Some sources mention Dark Moonlight as a dispellable aura on the boss — UNVERIFIED, not bot-critical since healers position out of the 15y aura.)

### Avoidable damage

- **Dark Moonlight aura** — 15y around boss; healers + casters stay outside
- **Moonlance ground lance** — sidestep
- **Eyes of the Goddess** — mobile silence orb; avoid contact
- **Tears of Elune** at 30% HP — small puddles on feet; reposition

### Tank-swap requirements

None.

### Heroic delta

Heroic-only — no delta.

### Add-handling

None. (Pre-fight trash is the moonbeam mechanic — tank-only routing.)

### Healer-pressure spikes

- **Stardust uninterrupted** = group-wide ~50k AoE = top healer pressure
- **Tears of Elune** at 30% HP = constant ground-fire damage
- **Lunar Guidance** stack 2 = Stardust kicks become tighter; missed kicks more frequent

### Key positioning

- **Tank**: in melee on boss; takes the Dark Moonlight aura penalty (no choice)
- **Melee**: behind boss; in 15y aura (cast speed irrelevant for them)
- **Ranged DPS / healer**: **OUTSIDE 15y Dark Moonlight aura** — position at boundary; cast at boss from outside the aura
- **All players**: dodge Moonlance + Eyes of the Goddess + Tears

### Bot-relevant role notes

- **Tank**: standard tank-and-spank; chase boss minimally; face boss away from group (Moonbolt single-target so no cone but discipline)
- **Healer**: pre-HoT before Stardust if kick is uncertain; **stand outside Dark Moonlight aura** — bot positioning override (ranged-edge positioning, not max-range)
- **Melee**: standard behind-boss; Moonlance dodge
- **Ranged DPS**: **PRIMARY STARDUST KICK PARTICIPANT** — bot interrupt rotation top priority; stand outside Dark Moonlight aura
- **All players**: at 30% HP, **burn — do NOT kite Tears of Elune**, kill boss

### Achievement note

**Moon Guard** (heroic) — survive Tyrande's Stardust gauntlet phase without healer taking damage. Bot-optional; flag for achievement-mode runs.

---

## Boss 5 — Echo of Murozond (Final Boss)

NPC ID: 54432. **Always present**, fought after both echoes are dead. Located at Bronze Dragonshrine. The fight's signature mechanic is the **Hourglass of Time** click-object.

### Phase structure

Single phase, DPS-race-with-resets. The Hourglass of Time is a click-interactable object that resets the encounter state (cooldowns, HP, mana, ground hazards) but NOT the boss's HP. Hard cap of 5 uses per fight.

1. Pull; tank picks up Murozond
2. Periodic **Temporal Blast** every 15s — group AoE + stacking +10% arcane damage taken
3. Periodic **Distortion Bomb** on random ranged player → permanent 8y arcane ground patch
4. Continuous **Infinite Breath** frontal cone on tank
5. Floor fills with Distortion Bombs over time → group runs out of safe space
6. **Click Hourglass** → all bombs cleared, cooldowns reset, HP/mana full, dead players revived, players teleported to start positions; **boss HP unchanged**
7. Repeat with 5 charges; if charges exhausted before boss dies → wipe

### Abilities

| Spell | ID | Cast / cooldown | School | Notes |
|---|---|---|---|---|
| Infinite Breath | UNVERIFIED | ~2s cast, ~12-15s cd | Fire | Frontal cone breath on tank direction; ~75k fire damage. **Tank faces boss away from group** (standard cone discipline). |
| Temporal Blast | UNVERIFIED | passive ~15s cycle | Shadow + Arcane debuff | ~25k shadow to all nearby + applies stacking debuff (+10% arcane damage taken, 20s). Stacks ramp Distortion Bomb damage taken. **Cleared by Hourglass.** |
| Distortion Bomb | UNVERIFIED | scripted, ranged-targeted | Arcane | Launched at random player's location; lands at their position; leaves **permanent** 8y radius ground patch dealing ~25k arcane/sec until end-of-fight or Hourglass click. Floor fills over time. |
| Tail Sweep | UNVERIFIED | passive (some sources) | Physical | Cone behind boss; knockback + DoT (~15k/3s for 15s). Some sources include this; others omit — see Disagreement #3. **Melee position to side, not strictly behind.** |
| Melee | — | — | Physical | Standard tank melee |
| **Hourglass: Rewind Time** | UNVERIFIED (encounter object spell, not boss-cast) | click-cast | — | When clicked: all player cooldowns reset; HP/mana to 100%; all players teleported to starting positions; all Distortion Bomb ground zones cleared; Temporal Blast stacks cleared; dead players revived. **Boss HP unchanged.** Hard cap: **5 uses per fight.** |

### Interrupts required

None confirmed. No hardcast spells from Murozond himself.

### Dispels required

- **Temporal Blast stacks** — Magic-school debuff. **Generally NOT dispelled** — it's group-wide and the Hourglass clears it. Dispelling 5 stacks is wasted GCDs vs. 5 stacks * 5 players. Hourglass is the cleaner clear.

### Avoidable damage

- **Distortion Bomb landing zones** — move out before bomb lands; over time arena fills, forcing positioning
- **Infinite Breath** — stay out of frontal cone
- **Tail Sweep (if present)** — stay off direct boss-rear position (melee at side)

### Tank-swap requirements

None.

### Heroic delta

Heroic-only — no delta.

### Add-handling

None.

### Healer-pressure spikes

- **Temporal Blast tick + ramping +10% arcane debuff** = compounding group damage
- **Distortion Bomb hit on slow-mover** = single-target heavy
- **End-of-charges scenario**: if Hourglass is exhausted and arena is full of bombs, group HP drops fast = wipe-grade

### Key positioning

- **Tank**: hold boss with frontal cone facing AWAY from group; **migrate boss as Distortion Bombs land** — never park boss on top of an existing bomb; minimize melee movement
- **Melee**: behind/side boss; avoid Tail Sweep direct rear
- **Ranged DPS**: **spread far** — Distortion Bombs target ranged, so spread minimizes the chance two bombs land in the same safe zone
- **Healer**: at range, NOT clustered with ranged DPS; track Temporal Blast cycle

### Hourglass usage strategy (bot logic)

| Strategy | When to use Hourglass |
|---|---|
| **Conservative** (low DPS) | Use when arena has 4+ Distortion Bombs OR group HP collapsing OR 3+ Temporal Blast stacks. Save first 2 charges for emergencies; aim to use all 5 across fight. |
| **Aggressive** (high DPS) | Use early to reset cooldowns for burst windows. Stack Bloodlust + major CDs immediately post-Hourglass. Aim for 1 click ~25%, 50%, 75% boss HP. |
| **Bot default** | **Conservative**: track (a) bomb count on floor, (b) Temporal Blast stack count, (c) party HP. Click when: bombs ≥ 4 OR stacks ≥ 4 OR party_lowest_hp < 30%. Reactive, not pre-planned. |

### Bot-relevant role notes

- **Tank**: face boss away; migrate when bombs converge; pop major CD around Hourglass click (CD is reset post-click, so use BEFORE)
- **Healer**: pace mana for full fight; **don't dispel Temporal Blast** (Hourglass handles it); save Mana Tide / Innervate for between Hourglass clicks
- **Melee**: side-position; avoid Tail Sweep area; standard rotation
- **Ranged DPS**: **spread**; Hourglass-click designation goes to highest-mobility ranged (Mage Blink, Warlock Demonic Circle); standard rotation otherwise
- **HOURGLASS CLICK BEHAVIOR**:
  - **Clicker bot logic**: click when condition met; **before clicking, ensure offensive CDs / Bloodlust have been spent** (they reset on click — don't waste a freshly-popped CD)
  - **Non-clicker bots**: don't fight for the click; let designated bot handle it
  - **Cooldown timing rule**: if Hourglass click is imminent (next tick), do NOT pop new offensive CDs; let click reset them; pop after click
- **The "time replay" caution**: original prompt context describes a "boss replays player actions" mechanic. **In Cata 4.3.4, this is not the actual mechanic** — the Hourglass rewinds **time state** (positions, cooldowns, HP, ground patches), not boss-recorded-actions. The bot caution maps to: **don't waste cooldowns immediately before a planned Hourglass click** since they'll be reset and re-usable post-click.

---

## Trash mechanics

End Time has thematic trash per shrine. Most is tank-and-spank with low CC value. Echo-of-Tyrande shrine has the unique **moonbeam-shield-strip** mechanic.

### Trash before Echo of Baine (Obsidian Dragonshrine)

- **Time-Twisted Breaker** — large physical mob; melee cleave; turn away from group
- **Time-Twisted Drake** — flying mob; ranged interrupt valuable; tank pulls down
- **Time-Twisted Seer** — caster; Polymorph / Hex / Sap valid; **interruptible casts**

Each pull typically has 1 of each. Standard kill order: skull on Seer (caster, kills first), then Breaker, then Drake.

### Trash before Echo of Jaina (Azure Dragonshrine)

- **Risen Priest** — caster trash; healing-cast — **kick / interrupt mandatory** so they don't heal pack
- **Risen Sorceress** — caster; Frostbolt-cast — interruptible

Pre-fight: collect 16 Jaina staff fragments while clearing trash. Standard pulls.

### Trash before Echo of Sylvanas (Ruby Dragonshrine)

- **Time-Twisted Rifleman** — ranged physical mob; LoS / pull to tank
- **Risen Banshee** — caster; standard interrupt rotation

### Trash before Echo of Tyrande (Emerald Dragonshrine)

- Mobs have **90% damage reduction outside moonbeams**. Tank must drag mobs INTO rotating moonbeam to strip the shield. Beam pattern: West → South → East → North on ~45s rotation.
- This is a **fight-specific tank override** — DPS waits for tank to position mob; do not open damage until mob is in beam.

### Trash before Murozond (Bronze Dragonshrine)

- **Dragonkin trash** — assorted casters + melee; CC valid but not required. Standard kill order.

### CC priorities

- **Polymorph / Hex / Sap valid** on Risen Priest, Risen Sorceress, Risen Banshee, Time-Twisted Seer
- Mobs of Drake / Dragonkin types are typically immune or low-value CC targets

### Skip-able trash

- All trash is mandatory by spawn-pattern; no LoS-skip routes documented

### Pull markers for CC

- Risen Priest pulls: **skull on Priest** (must die or be CC'd first), cross/X on Sorceress
- Caster packs: skull on lead caster

---

## Disagreements

Numbered list of points where the 5+ reconciled guides do not agree. Bot logic should pick a default and note the deviation.

### Disagreement #1 — Echo of Baine: Molten Axe / Molten Mace lava mechanic

- **Source A (Wowhead encounter journal):** The buff is acquired by **the boss** when Baine touches lava — adds fire damage to his melee. Implication: **only the tank avoids lava** (tank-positional concern only).
- **Source B (Icy Veins / EpicCarry):** Implies melee DPS can ALSO enter lava for some benefit — potentially gain a "Molten Fists" buff on the player. Wording is ambiguous; some guides describe a player-facing version.
- **Source C (Warcraft Wiki):** Describes Molten Mace as a boss-side weapon ignite (Spell ID 101836) — boss-only buff.

**Default:** treat as **boss-only buff**. Tank avoids lava. **DPS does NOT enter lava** — the player-facing "Molten Fists" interpretation appears to be a misread or a different mechanic. Conservative default avoids accidental lava-deaths.

### Disagreement #2 — Echo of Tyrande: Moonbolt interruptibility

- **Source A:** Stardust is the only confirmed interruptible cast; Moonbolt is single-target tank damage and is NOT interruptible.
- **Source B (Wowhead spell page implications):** Moonbolt is listed as a "cast" — possibly interruptible.

**Default:** **Stardust is the kick priority**; if Stardust is on cooldown for the rotation and Moonbolt is being cast, **try the kick** (low cost). Don't lose the Stardust kick window for a Moonbolt attempt.

### Disagreement #3 — Murozond: Tail Sweep inclusion

- **Source A (Warcraft Wiki tactics):** Lists Tail Sweep as a Murozond ability — cone behind boss, knockback + 15s DoT.
- **Source B (Icy Veins, Warcraft Tavern):** Does NOT list Tail Sweep — describes only Infinite Breath (front), Temporal Blast (PBAoE), Distortion Bomb (ranged target).

**Default:** **Assume Tail Sweep is present** (conservative). Melee positioning rule: **side of boss, not strict rear**, to avoid both cone (front) and Tail Sweep (rear) zones if Tail Sweep does exist.

### Disagreement #4 — Murozond: Hourglass usage strategy (early vs. late)

- **Source A (Icy Veins):** Use early to reset cooldowns for burst phases — **aggressive** strategy.
- **Source B (TenTonHammer):** Save for emergencies; use when group HP collapses or arena fills with bombs — **conservative** strategy.
- **Source C (Warcraft Wiki):** Spaced usage — every ~25% boss HP; balanced.

**Default:** **Conservative reactive** — click when any of: arena bomb count ≥ 4 OR group lowest HP < 30% OR Temporal Blast stack count ≥ 4. The bot's predict-burst-window heuristic is too unreliable for the aggressive strategy; reactive triggers are safer.

### Disagreement #5 — Echo of Sylvanas: ghoul kill priority

- **Source A:** Kill the ghoul **closest to the floating raid marker** (pre-set marker designates the gap).
- **Source B:** Kill the ghoul with **lowest HP** if multiple are damaged.
- **Source C:** Kill any single ghoul; doesn't matter which.

**Default:** **Lowest-HP ghoul** (ties broken by closest to bot's current position). The "raid marker" assumes a coordinated group with a marker-setter — bot groups don't necessarily have this. Lowest-HP ensures the kill happens fastest = highest probability of breaking the circle in time.

### Disagreement #6 — Echo of Jaina: Flarecore designated detonator

- **Source A:** Designated DPS walks into ember to detonate (controlled small-AoE damage).
- **Source B:** Embers should be ignored if HP is low; let them go to natural ~94k explosion (group AoE heal).
- **Source C:** Tank should NEVER detonate (already heavy single-target pressure from Pyroblast).

**Default:** **Highest-HP ranged DPS bot detonates** (mage / warlock with strong self-shields). Tank does NOT detonate. If no ranged is in good HP state, melee DPS (who is in range anyway) detonates.

### Disagreement #7 — Murozond: dispelling Temporal Blast stacks

- **Source A:** Temporal Blast is dispellable Magic; dispel before stack 3-4 to reduce damage.
- **Source B:** Don't dispel — Hourglass clears the stacks; dispels are wasted GCDs.
- **Source C:** Mass-dispel-only (Disc/Holy Priest) — single-target dispels are too slow vs. 5 players * stacks.

**Default:** **Don't dispel** — Hourglass cleanup is more efficient. Add to dispel-blacklist for the encounter.

---

## DBC verification (UNVERIFIED items)

Spell IDs and stack values listed below are from community-guide cross-reference, not direct DBC lookup. Add to `docs/research/dbc-verification-checklist.md` for resolution when the TC fork session opens.

| # | Spell / mechanic | Listed value | Why it matters for bot logic |
|---|---|---|---|
| 1 | Echo of Baine — Pulverize | 101626 | Platform-rotation trigger detection |
| 2 | Echo of Baine — Molten Mace / Molten Axe | 101836 | Tank-avoid-lava trigger |
| 3 | Echo of Baine — Throw Totem | 101614 | Bot reactive interaction (pick up + throw back) |
| 4 | Echo of Baine — NPC ID | 54431 | Encounter detection |
| 5 | Echo of Jaina — Pyroblast | 101809 | Primary kick rotation target |
| 6 | Echo of Jaina — Frostbolt Volley | 101810 | Secondary kick rotation target |
| 7 | Echo of Jaina — Flarecore | 101927 | Detonate-or-avoid decision |
| 8 | Echo of Jaina — Frost Blades | 101339 | Cone-avoidance trigger |
| 9 | Echo of Jaina — Blink | 101812 | Tank chase trigger |
| 10 | Echo of Jaina — NPC ID | 54445 | Encounter detection |
| 11 | Echo of Sylvanas — Calling of the Highborne | UNVERIFIED | Ghoul-circle event detection |
| 12 | Echo of Sylvanas — Black Arrow | UNVERIFIED | Spread-trigger |
| 13 | Echo of Sylvanas — Unholy Shot | UNVERIFIED | Spread-trigger |
| 14 | Echo of Sylvanas — Shriek of the Highborne | UNVERIFIED | Slow-dispel-priority |
| 15 | Echo of Sylvanas — Blighted Arrows | UNVERIFIED | Ground-rune avoidance |
| 16 | Echo of Sylvanas — Wracking Pain (ghoul aura) | UNVERIFIED | Ghoul-circle damage detection |
| 17 | Echo of Sylvanas — NPC ID | 54123 | Encounter detection |
| 18 | Echo of Tyrande — Stardust | 102173 | **TOP-PRIORITY KICK TARGET — verify ID** |
| 19 | Echo of Tyrande — Moonbolt | 102193 | Possible secondary kick target |
| 20 | Echo of Tyrande — Moonlance | 102149 | Sidestep trigger |
| 21 | Echo of Tyrande — Dark Moonlight | 102414 | Healer/caster positioning override (15y aura) |
| 22 | Echo of Tyrande — Lunar Guidance | 33589 | HP-gated cast-haste; verify Cata-era ID (33589 looks pre-Cata; may have been re-IDed) |
| 23 | Echo of Tyrande — Tears of Elune | 102241 | 30% HP burn-phase trigger |
| 24 | Echo of Tyrande — Eyes of the Goddess / Piercing Gaze | UNVERIFIED | Mobile silence-orb avoidance |
| 25 | Echo of Tyrande — NPC ID | 54544 | Encounter detection |
| 26 | Murozond — Infinite Breath | UNVERIFIED | Tank cone-facing trigger |
| 27 | Murozond — Temporal Blast | UNVERIFIED | Stack tracking + Hourglass click trigger |
| 28 | Murozond — Distortion Bomb | UNVERIFIED | Ground-patch tracking + arena fill trigger |
| 29 | Murozond — Tail Sweep | UNVERIFIED | Disagreement #3 — confirm existence |
| 30 | Murozond — Hourglass: Rewind Time | UNVERIFIED | Encounter-object click cast; click-detection trigger |
| 31 | Murozond — NPC ID | 54432 | Encounter detection |
| 32 | Hourglass-of-Time encounter object GameObject ID | UNVERIFIED | Click-target identification |
| 33 | Hourglass max charge count | 5 | Verify hard cap; bot logic enforces |
| 34 | End Time zone ID | 5789 | Encounter-detection scope |

Spell IDs starting with `1018xx` and `1019xx` and `1014xx` are within the patch-4.3 range and consistent with end-of-Cata content. Most are derived from Wowhead encounter-journal pages.

---

## Notes on prompt deviations

- **Murozond's "time replay" mechanic clarification**: the prompt context describes a unique mechanic where "the boss replays the players' actions — bot must NOT do anything unsafe during the recording window" and that "bot replays its own actions; bot should NOT cast offensive cooldowns / use trinkets during the recording phase since they'll get re-used unsafely during the replay." **In the actual Cata 4.3.4 mechanic, this is not exactly correct** — the Hourglass of Time **rewinds time-state** (player positions, cooldowns, HP, mana, ground patches) but does NOT record-and-replay player actions. The bot caution that translates to actual mechanics is: **don't waste offensive cooldowns immediately before a planned Hourglass click** since the click resets the cooldowns and the bot loses the "second use" benefit. Also: **don't use trinkets/CDs late in the Hourglass cycle** — they'll be reset shortly after, so use them earlier in the cycle. This is captured in `dungeons/end-time/encounter.md` and the role docs as "Hourglass-aware CD pacing."
- **Random echo pool detection**: the bot must reactively detect which 2 of 4 echoes spawned. Suggested implementation: zone-load → poll Time Transit Device portal targets OR detect first-engaged echo's NPC ID and load corresponding encounter profile. This is flagged as a bot-implementation gap.
- **Heroic-only**: no normal-mode delta sections. Each boss section omits the standard "Heroic delta" subsection or notes "heroic-only — no delta."
- **Tank-swap requirements**: present per-boss for consistency with BRC and ToT formats; in End Time, no boss requires a tank swap.
- **Tyrande pre-fight moonbeam mechanic** documented in trash section, not boss section, since it's a tank-routing pre-fight mechanic.
- **Achievement notes**: Severed Ties (Sylvanas) and Moon Guard (Tyrande) noted but bot-optional. Not in standard run logic.
- **Hourglass click-handler designation**: documented as a fight-specific bot ability with assignment heuristic (highest-mobility ranged → Mage > Warlock > Hunter > others). Cross-referenced in `ranged-dps.md` for End Time.
- **Wowhead Cata** is the spell-ID source of record for confirmed IDs; UNVERIFIED items derived from community guide cross-reference are flagged. End Time was a 4.3 patch dungeon — late enough that most spell IDs are well-documented, but encounter-object IDs (the Hourglass GameObject) are typically harder to pin without DBC access.
- **No instance-wide buff/debuff** unlike some Cata 5-mans.
- **No instance-wide curse/disease/poison dispel requirement**. Magic dispels MOSTLY blacklisted (Murozond Temporal Blast = Hourglass-cleared, not dispel-cleared). Sylvanas Shriek slow is the one Magic dispel that has value.

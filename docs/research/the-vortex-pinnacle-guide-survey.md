# The Vortex Pinnacle — Cataclysm 5-man Survey

**Date:** 2026-05-02
**Patch:** 4.3.4
**Audience:** bot-control engineer (cata-altbot)

---

## Overview

| Field | Value |
|---|---|
| Zone | The Vortex Pinnacle (floating spires above southeastern Uldum) |
| Continent | Skywall (elemental plane access via Uldum) |
| Level range (normal) | 82–84 |
| Level range (heroic) | 85 |
| Group size | 5 (1 tank / 1 healer / 3 DPS) |
| Boss count | 3 (Grand Vizier Ertan, Altairus, Asaad) |
| Optional bosses | None — three required, no skippable encounter |
| Layout | Three floating subzones — **Cyclone Summit** (Ertan) → **Nimbus Rise** (Altairus) → **Temple of Asaad** (Asaad). Connected by **Slipstream** tornado-vehicles between platforms. Falling = teleport back to dungeon entrance (instance entrance, not death — but treated as significant time loss). |
| Cata-launch class | Yes — released alongside Throne of the Tides, Blackrock Caverns, Halls of Origination, Lost City of the Tol'vir, The Stonecore, and Grim Batol as the Cata launch 5-mans |
| Group buffs/debuffs | Boss-specific positional auras (Upwind / Downwind of Altairus); **no** instance-wide aura |

The instance is short (~25-35 min on heroic for a geared group, ~45 min on first launch-tier clears). Trash density is moderate; the path is "entrance → Cyclone Summit (Ertan) → first slipstream → Nimbus Rise (Altairus) → second slipstream → Temple of Asaad (Asaad)." Several trash packs are skippable via Slipstream movement or wall-hugging.

The dungeon's defining gimmick is the **Slipstream** vehicle-traversal mechanic: players click a tornado near the dungeon entrance (and at checkpoints after each boss) and ride it across open sky to the next platform. **Falling off a platform** ports the player back to the entrance — survivable, but a significant time penalty. This is the single most important bot-implementation flag for the dungeon (see Trash section + bot-followup).

---

## Sources

This survey reconciles the following community sources. Wowhead Cata is treated as authoritative for spell IDs and numeric values; strategy is cross-checked across 5+ guides.

1. **Wowhead Cata — encounter & NPC pages** (`/cata/npc=...`, `/cata/spell=...`) — primary for spell IDs, cast times, debuff durations, damage ranges
2. **Warcraft Wiki (warcraft.wiki.gg)** — `https://warcraft.wiki.gg/wiki/Asaad`, `/Altairus`, `/Grand_Vizier_Ertan` — strongest source for ability tables (NPC IDs, damage ranges, durations, damage schools)
3. **Icy Veins — Vortex Pinnacle Dungeon Guide (Cataclysm Classic)** (`icy-veins.com/cataclysm-classic/the-vortex-pinnacle-dungeon-guide`) — strategy reference, slipstream description, role overview
4. **Icy Veins — retail Vortex Pinnacle guide** (`icy-veins.com/wow/the-vortex-pinnacle-dungeon-guide`) — most complete trash listing reachable by WebFetch (mechanics carry over from Cata original; abilities and IDs unchanged for trash)
5. **Engadget WoW Insider Cata launch guide (2010)** (`engadget.com/2010-12-06-cataclysm-dungeon-guide-vortex-pinnacle.html`) — original-Cata strategy notes
6. **Glow's Branches — Vortex Pinnacle healer-perspective dungeon guide** (`glowberry.wordpress.com/dungeon-guides/the-vortex-pinnacle/`) — Resto Druid healer-flavor guide; valuable for Asaad Static Cling dispel cadence ("only second cast between storms")
7. **MMO-Champion forum — Heroic Asaad tip thread** (`mmo-champion.com/threads/809064`) — Static Cling dispel + jump-avoidance details, Magic dispeller requirement
8. **Ten Ton Hammer — Cata dungeon guide** (`tentonhammer.com/articles/cataclysm-dungeon-vortex-pinnacle`) — confirms wind-direction strategy, role positioning
9. **Altered Gamer — Vortex Pinnacle dungeon guide** (`alteredgamer.com/wow-dungeons/108180-the-clouds-above-uldum-vortex-pinnacle-dungeon-guide`) — partial; useful cross-check
10. **GotWarcraft — Heroic Vortex Pinnacle Boss Guide** (`gotwarcraft.com/heroic-vortex-pinnacle-boss-guide`) — heroic-specific tactics

Sources that 403'd or 404'd despite multiple attempts: warcrafttavern.com (403), wowpedia.fandom.com (403), wowwiki-archive.fandom.com (403). Strategy content from these is reflected indirectly via web-search excerpts.

Where guides disagree, the disagreement is captured in the **Disagreements** section and the consensus position is listed in **Strong consensus**.

---

## Strong consensus (table)

| Mechanic | Consensus |
|---|---|
| Ertan Cyclone Shield | 8 cyclones rotate around Ertan; group **stays inside the ring** to avoid Storm's Edge; Cyclones periodically retract toward Ertan dealing damage + slow to anyone they overlap (player can dodge through gap) |
| Ertan Storm's Edge | Constant Nature DoT on anyone outside the cyclone ring; punishes "stay at max range" defaults |
| Ertan Lightning Bolt | Boss casts Nature Lightning Bolt on current target (tank); **interruptible** is the consensus though one source treats it as "interrupt to ease healer" rather than mandatory |
| Ertan Lurking Tempest | Heroic-only adds; spawn at platform edges; **only attack players who are facing them** — group plays "look-away" so they harmlessly cast Lightning Bolt at no one or "play dead" |
| Altairus Twisting Winds | Heroic-only mini-tornadoes patrol the platform; contact = damage + knockup + fall hazard |
| Altairus Chilling Breath | Frontal **120° cone** Frost AoE on a random target; Altairus stays aimed at chosen target; group spreads, target re-positions to point cone away from group |
| Altairus Call the Wind | Wind direction shifts every ~30s; players gain **Upwind of Altairus** (haste/move buff) when boss is downwind of them, or **Downwind of Altairus** (debuff) when boss is upwind. Group repositions on shift. |
| Altairus Lightning Blast | Anti-leash punishment if a player tries to leave the platform; not relevant to in-platform combat |
| Asaad Skyfall Star | Asaad summons Skyfall Star adds throughout fight; they cast Arcane Barrage on random players; **kill priority absolute** (ranged DPS swap) |
| Asaad Static Cling | **Heroic-only**; Asaad casts an 18s root debuff on (typically) ranged targets when ground-touching at cast end; **JUMP at cast end to avoid**; if missed, **Magic dispel** removes it (priority before Supremacy of the Storm) |
| Asaad Unstable Grounding Field | Triangular safe zone spawned on the ground; Asaad channels; everyone must be **inside the triangle** before Supremacy of the Storm fires |
| Asaad Supremacy of the Storm | Massive Nature damage AoE outside the triangle; one-shots anyone caught outside; ~6s duration channel |
| Asaad Chain Lightning | Random-target Nature damage; bounces 40y between targets at 50% reduced damage; **ranged spread minimum 15y** to break the chain |
| Slipstream traversal | Click tornado near platform edge → vehicle-mounted ride to next platform. Falling = teleport to dungeon entrance. Used between bosses + at group death-recovery. |
| Howling Gale trash bridge | Two Howling Gale obstacles on the bridge between Ertan and Altairus; standing in their AoE knocks players off the bridge (= teleport to entrance). DPS them down to disable; cross when safe. |
| Temple Adept healer trash | Caster trash on the Asaad approach; **interrupt Greater Heal**; CC priority target |
| Empyrean Assassin | Stealthed melee assassin trash; **interrupt Vapor Form** (heal-into-stealth combo) |
| Slipstream skip notes | First Skyfall Star pack near Asaad's chamber can be skipped by hugging the right wall and dropping to the staircase. Several Cyclone Summit packs can be slipstream-skipped. |

---

## Boss 1 — Grand Vizier Ertan

NPC ID: 43878. First boss, Cyclone Summit platform after the entrance and initial Slipstream traversal. Encounter is single-phase with periodic cyclone-retract events and (heroic) periodic Lurking Tempest spawns.

### Phase structure

Single phase, on a repeating cycle:

1. Ertan begins fight by casting **Cyclone Shield** — 8 cyclones orbit around her at ~30y radius
2. Group stacks **inside** the cyclone ring (between Ertan and the cyclone orbit path)
3. Cyclones periodically **retract toward Ertan** (~every 30s) — group dodges to avoid contact damage + slow debuff
4. **Heroic only**: Lurking Tempests spawn at platform edges every ~30-45s; group "looks away" so they don't activate
5. Ertan auto-attacks tank with **Lightning Bolt** throughout the fight

### Abilities

| Spell | ID | Cast / cooldown | School | Notes |
|---|---|---|---|---|
| Cyclone Shield | 86292 | passive | Nature | 8 cyclones orbit Ertan at ~30y; contact: 23,750-26,250 Nature dmg + 60% slow/cast/attack debuff for 12s. Retract event triggers periodic damage opportunity. |
| Storm's Edge | 86309 | passive | Nature | DoT on players **outside** the cyclone ring. 9,599-11,732 Nature damage/sec (per warcraft.wiki.gg). Forces the "stay inside the ring" positional rule. |
| Lightning Bolt | UNVERIFIED (likely 86331 or similar) | ~2-2.5s cast | Nature | Tank-focused damage cast: 8,500-11,500 Nature on current target. **Interruptible** per most sources. Bot interrupt priority MEDIUM (less critical than e.g. Quake on Rom'ogg, but kicks help healer). |
| Summon Tempest | 86340 | scripted | — | **Heroic only**; spawns Lurking Tempest adds at platform edges throughout the fight. |
| (Lurking Tempest) Lightning Bolt | UNVERIFIED | ~2s cast | Nature | Lurking Tempest auto-cast on whoever is facing it: 6,562-8,437 Nature dmg. Add "plays dead" / faces away if **no one** looks at it. |

### Interrupts required

- **Ertan Lightning Bolt** — tank-focused; interrupt eases healer pressure but is not mandatory. Bot interrupt priority MEDIUM.
- **Lurking Tempest Lightning Bolt** — only if the add has been activated (someone faced it). Better strategy: **don't activate** by camera-management.
- No other hardcasts.

### Dispels required

None.

### Avoidable damage

- **Cyclone Shield retract** — dodge through a gap when cyclones pull inward
- **Storm's Edge** — stay inside the cyclone ring (i.e., near boss)
- (heroic) **Lurking Tempest Lightning Bolt** — avoid by not facing the add (camera mgmt)

### Tank-swap requirements

None. Single tank.

### Heroic delta

- **Lurking Tempest adds spawn** (normal-mode does not have this — UNVERIFIED whether normal has any add spawn at all, treat as heroic-only)
- Cyclone retract damage higher
- Lightning Bolt hits harder (mandatory interrupt rotation rather than nice-to-have)

### Add-handling

- **Lurking Tempest** (heroic only): the *intended* play is **don't engage** — the group "looks away" (faces toward Ertan, away from platform edges). Tempests cast Lightning Bolt only at players facing them; if all players face Ertan, tempests harmlessly idle.
- If a player accidentally targets/faces a tempest, it activates and casts Lightning Bolt. Two strategies in the wild:
  - **A**: kill it ASAP (DPS swap)
  - **B**: have the tank reposition to face away while the activator stops looking
- **Default for bot:** treat tempest as DON'T-TARGET unless instructed. Camera/facing management is fragile bot logic, so a fallback "kill tempest if it activated and is casting" is a safer secondary rule. See **Disagreements #1**.

### Healer-pressure spikes

- Cyclone retract event (every ~30s): if a player gets clipped, they take 23-26k Nature + 60% slow debuff
- Tank Lightning Bolt damage is steady, not spiky
- Heroic: tempest activations (if mishandled) become a sudden ranged-damage spike

### Key positioning

- **All players stay inside the cyclone ring** — this is a stack-near-boss positioning that is unusual for ranged but mechanic-required
- **Tank** holds Ertan center, no movement except to dodge cyclone retracts
- **Heroic camera rule**: face Ertan (boss is in the room center), never face platform edges where Tempests spawn

### Bot-relevant role notes

- **Tank**: tank-and-spank with periodic cyclone-retract dodge; on heroic, face center to ignore tempest auto-attack from edge
- **Healer**: light pressure overall; pre-HoT before each cyclone retract event (predictable timer)
- **Melee**: trivial — already in melee, already inside ring
- **Ranged**: **stay inside the ring** — overrides default ranged-spread positioning. Bot ranged-DPS logic must allow "stack at melee range when encounter requires it" as a per-encounter override (similar to Skullcracker on Rom'ogg).
- **Heroic camera rule** is a fragile bot behavior — bots don't really "face" things the way players do; bot AI must instead **suppress targeting Lurking Tempests** entirely (don't acquire as target, don't auto-cast at them). If a tempest is already in combat (somehow activated), THEN engage.

---

## Boss 2 — Altairus

NPC ID: 43873. Second boss, Nimbus Rise platform (reached via Slipstream after Ertan + bridge trash including Howling Gales). Storm Drake elemental dragon.

### Phase structure

Single phase, with continuous **Call the Wind** direction shifts (every ~30s) and periodic random-target **Chilling Breath** cone casts.

1. Pull Altairus to platform center
2. **Call the Wind** establishes initial wind direction; players reposition so boss is *upwind* of them (player gets the **Upwind of Altairus** buff)
3. Direction shifts every ~30s — group repositions
4. Throughout: **Chilling Breath** on random targets (cone); on heroic, **Twisting Winds** mini-tornadoes patrol platform

### Abilities

| Spell | ID | Cast / cooldown | School | Notes |
|---|---|---|---|---|
| Call the Wind | UNVERIFIED | scripted (passive aura mechanic) | Nature | Establishes wind direction. Direction shifts every ~30s. Players upwind of Altairus get **Upwind of Altairus** buff (+30-100% haste, +30% movement); players downwind of Altairus get **Downwind of Altairus** debuff (-100% attack/cast speed, -30% movement). |
| Upwind of Altairus | UNVERIFIED | aura | — | Beneficial buff while positioned upwind. Some sources cite +100% haste, others +75%, others +30%. See **Disagreements #2**. |
| Downwind of Altairus | UNVERIFIED | aura | — | Punishment debuff — same magnitudes as Upwind, inverted sign |
| Chilling Breath | 88308 | ~2s cast (UNVERIFIED) | Frost | Frontal 120° cone, random-target. Damage 25,500-34,500 Frost (normal); 47,500-52,500 (heroic). Boss STAYS aimed at chosen target through the cast — target moves to point cone away from group, OR group spreads. |
| Twisting Winds | 88314 | passive (heroic only) | Nature | Heroic-only patrolling mini-tornadoes. Contact: 11,700-12,300 Nature dmg + knockup. Knockup carries fall-off-platform risk. |
| Lightning Blast | UNVERIFIED | reactive | Nature | Anti-leash — fires only when a player tries to leave the platform. Damage 55,500-64,500. Not relevant to in-platform combat. |
| Melee | — | — | Physical | Standard tank-melee (Storm Drake auto-attack) |

### Interrupts required

None confirmed. Chilling Breath is a hardcast but most sources do not flag it as kick-required (cast time is short and the cone-avoidance strategy handles the damage). One source notes it's interruptible — UNVERIFIED, see **Disagreements #3**.

### Dispels required

- **Downwind of Altairus** is described as a debuff — but it is **NOT** dispellable per all reconciled guides. It is a **positional** debuff: stand upwind of the boss to remove it. **Bot dispel logic should NOT attempt to dispel Downwind of Altairus** — it would be wasted GCD.

### Avoidable damage

- **Chilling Breath cone** — re-position out of cone, OR the target re-points the cone
- **Twisting Winds** (heroic) — sidestep the patrolling tornadoes; their movement is slow
- **Downwind of Altairus** — positional, "avoid" by repositioning upwind

### Tank-swap requirements

None.

### Heroic delta

- **Twisting Winds** spawns (heroic-only) — significant movement burden
- Chilling Breath damage roughly doubles (~50k vs ~30k)
- The Upwind/Downwind direction shift is the same; just less forgiving on missed timing

### Add-handling

None. Altairus has no adds.

### Healer-pressure spikes

- Chilling Breath on a low-HP target (especially heroic ~50k) is the canonical spike
- Twisting Winds knockup → fall hazard → potentially a player teleporting to the entrance (= dead-to-fight for ~30-60s of run-back)

### Key positioning

- **Tank** holds Altairus near platform center, facing AWAY from group — group needs to be on the upwind side, so tank also wants the upwind buff
- **All players** maintain "boss is downwind of me" positioning, repositioning every ~30s on direction shift
- **Chilling Breath target** repositions to point cone away from group (or group spreads to limit clipping)
- **Heroic Twisting Winds**: continuous evasion; do not stand still

### Bot-relevant role notes

- **Tank**: standard tank-positioning + wind-direction reposition every ~30s. Tank wants Upwind buff like everyone else.
- **Healer**: pre-HoT Chilling Breath target if predictable; reposition on direction shift; manage Twisting Winds dodging on heroic
- **Melee**: in melee already; reposition with the group on wind shift; dodge Twisting Winds
- **Ranged**: spread for Chilling Breath cone, then bunch up upwind for the buff; reposition on wind shift
- **Wind-direction logic** is a per-tick spatial query: "where is the boss relative to me, given current wind direction?" Bot needs an `aura.active(self, UPWIND_OF_ALTAIRUS)` check + a position-reset trigger on direction-change events.

---

## Boss 3 — Asaad, Caliph of Zephyrs

NPC ID: 43875. Final boss, Temple of Asaad platform (reached via Slipstream after Altairus + Temple trash including Adepts and Empyrean Assassins). Djinn boss; this is the dungeon's standout encounter.

### Phase structure

Single phase, on a repeating cycle of three event types:

1. Periodic **Skyfall Star** summons → ranged DPS swap to kill add
2. Periodic **Chain Lightning** on random target → group spread (15y+)
3. Periodic **Static Cling** (heroic only) → JUMP at cast end OR Magic-dispel
4. **Unstable Grounding Field → Supremacy of the Storm** combo every ~50-70s — group stacks inside triangle

### Abilities

| Spell | ID | Cast / cooldown | School | Notes |
|---|---|---|---|---|
| Skyfall Star (summon) | UNVERIFIED summon | scripted | — | Spawns Skyfall Star adds throughout fight |
| Skyfall Star Arcane Barrage | UNVERIFIED | continuous cast | Arcane | Skyfall Stars cast back-to-back Arcane Barrage on random players. Damage 10,450-11,550 (per warcraft.wiki.gg, retail-lifted; **Cata 4.3.4 may use 5,400-6,600** per legacy listing — see **Disagreements #4**). **Kill priority ABSOLUTE** for ranged DPS. |
| Chain Lightning | 87622 | ~2s cast (UNVERIFIED) | Nature | Damage 25,437-29,562 initial; bounces 40y between targets, **50% reduced damage per bounce**. **Group spreads 15y+** to break the chain. |
| Static Cling | 87618 | ~2s cast | Nature (Magic dispel type) | **Heroic only.** Cast targets typically ranged; players with feet on the ground at cast END are rooted for **18s**. **AVOID by jumping** before cast finishes. **MAGIC DISPEL** removes it (priority dispel, especially before Supremacy of the Storm). |
| Unstable Grounding Field | 86911 | ~5s channel | Nature | Asaad channels a triangular safe zone on the platform; spell-targeting **redirected away** from units inside the field. Channel ends → Supremacy of the Storm fires. |
| Supremacy of the Storm | 86930 | 6s channel | Nature | Massive AoE damage outside the field; **one-shots** anyone outside. 19,475-21,525 Nature dmg per 0.5s ticks (per warcraft.wiki.gg). |
| Lightning Strike (auto-attack) | UNVERIFIED | melee | Nature | Asaad auto-attacks tank with Nature damage (he is a caster, not melee — but he has a primary-target auto-attack) |

### Interrupts required

- **Static Cling cannot be interrupted** (per all sources) — only avoided (jump) or dispelled (Magic).
- **Unstable Grounding Field cannot be interrupted** (mechanic-intended).
- **Chain Lightning is a hardcast** — UNVERIFIED whether interruptible; some sources imply it is, but the standard strat is "spread, take it, heal through." **Interrupt priority LOW.**
- **Skyfall Star Arcane Barrage** — UNVERIFIED interruptibility. Stars die fast (low HP), so kill > interrupt is the standard play.

### Dispels required

- **Static Cling (87618)** is **Magic** dispel type. **Critical priority** when cast right before Unstable Grounding Field — rooted players need to reach the triangle. If group composition has no Magic dispeller, Static Cling targets are likely lost (or use class-specific immobilize-breaks: Hand of Freedom, Lichborne, Druid powershift, Cleanse, Anti-Magic Shell on every other cast).
- **Glow's healer guide** notes a refinement: Asaad casts Static Cling **twice** between each Supremacy of the Storm — only the **second** application *strictly* needs dispelling (the first wears off in time). On normal mode this is fine; on heroic it is mana-conservative. **Bot default**: dispel every Static Cling application (simpler, mana-cost is minor on a 5-man heroic).

### Avoidable damage

- **Chain Lightning bounces** — break by spreading 15y+
- **Supremacy of the Storm** — must be inside the Unstable Grounding Field triangle
- **Skyfall Star Arcane Barrage** — only avoidable by killing the star fast
- **Static Cling root** — JUMP at cast end (heroic only)

### Tank-swap requirements

None.

### Heroic delta

- **Static Cling** is heroic-only — entirely new mechanic
- Skyfall Star damage higher; spawn rate slightly faster
- Chain Lightning damage higher
- Supremacy of the Storm tick damage may be higher (UNVERIFIED — most sources just call it "one-shots regardless")

### Add-handling

- **Skyfall Stars**: appear continuously throughout the fight
- **Kill priority**: ranged DPS swaps to whatever Skyfall Star is currently up. Star dies fast (low HP). DO NOT ignore — Arcane Barrage damage compounds.
- Tank does not pick up Skyfall Stars — they cast on random ranged anyway, so tank threat is irrelevant. Tank stays on Asaad.

### Healer-pressure spikes

- **Skyfall Star Arcane Barrage** — steady ranged-target damage; unpredictable target
- **Chain Lightning** — bounces between players who failed to spread; whole-group spike
- **Static Cling root + Supremacy of the Storm** — if a rooted player can't reach the triangle, they die. **Pre-dispel before Grounding Field finishes channeling** is the canonical save.
- **Supremacy of the Storm** — survivable inside the triangle, but heavy ticking damage during the channel even inside

### Key positioning

- **Tank** holds Asaad in platform center
- **All players** maintain 15y+ spread for Chain Lightning
- **Triangle event** — when Unstable Grounding Field appears, ALL players move into the triangle. Triangle position is **random per cast** — not a memorized spot.
- **Heroic Static Cling** — JUMP at cast end if you are a likely target

### Bot-relevant role notes

- **Tank**: standard tank position; one of the simpler tank fights in the dungeon
- **Healer**: dispel Static Cling on heroic (Magic-dispel role); reactive heal on Chain Lightning, Skyfall Star damage, Supremacy of the Storm tick
- **Melee**: maintain spread (yes, even melee — Chain Lightning bounces find melee easily); move into triangle on cast; dodge Skyfall Stars if they path through melee zone
- **Ranged**: PRIMARY responsibility = kill Skyfall Stars on spawn; spread for Chain Lightning; on heroic, JUMP at Static Cling cast end (timing-aware) OR rely on dispel
- **Triangle-detection** is a spatial query (point-in-triangle); the triangle is a per-cast 3-vertex polygon. Bot AI needs a `point_in_triangle(self.pos, triangle)` predicate + a "move to nearest point in triangle" pathing override for the Grounding Field event.
- **JUMP timing for Static Cling** is sub-second precision — too risky to rely on for bots. **Bot default = lean on dispel** for Static Cling rather than try to time the jump.

---

## Trash mechanics

The notable trash. Standard packs (basic flying wisps, low-HP elementals) are pure threat-and-DPS and not enumerated.

### Cyclone Summit area (pre-Ertan)

- **Wild Vortex** — caster trash; interruptible wind cast; spawns small adds
- **Gust Soldier** — applies a dispellable damage-over-time debuff on tank/melee
- **Armored Mistral** — has a "Cloud Guard" ability that makes them resistant to ranged attacks (counter: melee in)
- **Cloud Prince** — mini-boss style; **cannot be CC'd**; spawns small adds; AoE damage on death (Turbulence cast — defensive CD recommended)
- **Lurking Tempest** (also appears as trash, not just on Ertan): "play dead" mechanic — face away to ignore, or kill if engaged

### Bridge between Ertan and Slipstream-to-Altairus

- **Howling Gale** (×2 on bridge) — environmental hazard / standing AoE that can knock players off the bridge (= teleport to entrance). DPS them down to disable; cross while they are stunned/disabled.

### Nimbus Rise area (pre-Altairus)

- **Turbulent Squall** — caster trash; interruptible cast; **buff to dispel** (Magic dispel — purge / spellsteal / Mass Dispel valid)
- **Empyrean Assassin** — stealth-style melee; **interrupt Vapor Form** (re-stealth / heal-into-stealth combo); applies stacking **Lethargic Poison** on tank — Poison dispel valid (Cleanse, Cleanse Spirit on Resto Shaman, Abolish Poison)
- **Young Storm Dragon** — most dangerous trash mob in the dungeon; AoE damage + frontal breath cone; tank with care; healer can use the platform's Slipstream as escape if HP collapses

### Temple of Asaad area (pre-Asaad)

- **Servant of Asaad** — physical melee; charges with **Sure Strike**; mostly tank-and-spank
- **Temple Adept** — caster healer trash; **interrupt Greater Heal** (mandatory, otherwise the rest of the pack heals to full); also casts **Holy Smite** (interruptible). CC priority = Adept first when multi-CC team. **Mind Control** on the Adept is canonically excellent — they heal your group instead.
- **Executor of the Caliph** — large melee; AoE damage; uninterruptible defensive buff (wait it out, no kick available)
- **Minister of Air** — caster; Nature damage; nearby "Grounding Field" environmental object grants immunity if standing inside (player exploits this against this mob)
- **Skyfall Star (trash version)** — small pack near Asaad; ranged DPS swap. The first pack here can be **skipped** by hugging the right wall.

### CC priorities

- **Polymorph / Hex / Sap / Banish** valid on Tol'vir humanoids (Servant of Asaad, Temple Adept, Executor) and on most elementals
- **Cloud Prince** — **immune to CC** (don't try; AoE-burn instead)
- **Lurking Tempest (trash)** — face-away mechanic obviates the need for CC if managed
- **Empyrean Assassin** — stealth mob; **Sap** at start works; otherwise Hammer of Justice / fear chains
- **Mind Control** on Temple Adept is a strong play — they heal your group instead of theirs

### Skip-able trash

- **Several Cyclone Summit packs** can be Slipstream-skipped by riding the tornado past them
- **First Skyfall Star pack near Asaad** can be wall-hugged on the right and dropped past
- **Bridge Howling Gales** must be disabled (can't be skipped — bridge is the only path)

### Slipstream traversal notes

- **Click the tornado** with the "enter vehicle" cursor icon → vehicle-mounted ride to the next platform
- **Falling off the platform** (running off edge, knocked off by Howling Gale, etc.) → teleport to dungeon entrance
- After defeating each boss, a Slipstream becomes available near the platform that takes the group to the next zone
- After group death/recovery, a Slipstream near the dungeon entrance returns the group to the furthest checkpoint reached

This is the **single biggest bot-implementation flag** for the dungeon. See encounter.md instance-wide notes.

---

## Disagreements

Numbered list of points where the reconciled guides do not agree. Bot logic should pick a default and note the deviation.

### Disagreement #1 — Heroic Lurking Tempest handling: ignore or kill?

- **Source A (Icy Veins, warcraft.wiki.gg)**: ignore them — face away from platform edges and they "play dead"
- **Source B (Ten Ton Hammer)**: kill them with cleave AoE — the camera-management is too fragile in real groups
- **Source C (some original-Cata guides)**: kill the first one to demonstrate, then ignore subsequent

**Default for bot:** **Don't auto-target** Lurking Tempests. If one becomes hostile (somehow activated), **then** acquire as target and kill. Bot behavior: tempest is on a **target-suppression list** while inactive.

### Disagreement #2 — Upwind of Altairus haste magnitude

- **Source A (warcraft.wiki.gg)**: 100% increased attack/casting speed
- **Source B (Ten Ton Hammer)**: 75% attack/casting speed
- **Source C (other guides)**: 30% (likely just movement, mis-attributed)
- **Source D (Wowhead community)**: actual Cata 4.3.4 buff is +30% haste + +30% movement (not +100%)

**Default for bot:** the magnitude doesn't actually drive bot behavior — the bot reacts to the **aura presence**, not the magnitude. Treat Upwind as "good, want to maintain" and Downwind as "bad, reposition." DBC-verify magnitudes for cosmetic/UI purposes only.

### Disagreement #3 — Altairus Chilling Breath interruptible

- **Source A (most guides)**: not flagged as interruptible
- **Source B (one source)**: interruptible

**Default:** **don't include in interrupt rotation**. If interrupt is off CD, attempt is fine, but don't time-budget for it.

### Disagreement #4 — Skyfall Star Arcane Barrage damage range

- **Source A (warcraft.wiki.gg)**: 10,450-11,550 Arcane (this looks retail-lifted)
- **Source B (Cata-era listing)**: 5,400-6,600 Arcane
- **Source C (heroic encounter listing)**: ~2,700-3,300 Arcane (per warcraft.wiki.gg's Asaad-page version of the Skyfall mention)

**Default:** 4.3.4 Cata Classic damage is **closer to 5,400-6,600 normal / 10,000-11,000 heroic** based on the level-85 healer-tuning context. Doesn't drive bot behavior (bot still kills star ASAP regardless), but flag for DBC verify.

### Disagreement #5 — Static Cling dispel-cadence

- **Source A (Glow's healer guide)**: Asaad casts Static Cling TWICE between each Supremacy of the Storm — only the **second** application strictly needs dispelling
- **Source B (most other guides)**: dispel every Static Cling

**Default:** **dispel every Static Cling** application. Mana cost is negligible vs the cost of letting a rooted player die during Supremacy. The "only second" optimization is real but is a heroic-mode optimization for very mana-tight comps; bot defaults to safer behavior.

### Disagreement #6 — JUMP-avoidance for Static Cling

- **Source A**: jump at cast end → avoid root entirely
- **Source B**: dispel after the fact → root is brief
- **Source C**: combine — JUMP if you have spare GCD, dispel if rooted lands

**Default for bot:** **DO NOT rely on JUMP timing**. Bot AI cannot reliably hit the sub-second jump window. Default is to **eat the root and dispel** (or use class-specific immobilize-break: Cleanse / Hand of Freedom / Lichborne / etc.). For melee bots, this means ranged might get rooted during Supremacy of the Storm — group needs to position so rooted players are ALREADY in the triangle area when Static Cling lands.

### Disagreement #7 — Cyclone Shield retract: dodge or eat?

- **Source A**: dodge through gaps in the cyclone wall during retract
- **Source B**: eat the contact damage; healer compensates
- **Source C**: positional (stay in dead zones between cyclones) — most cyclones don't ever overlap a stationary player

**Default:** **stationary positioning works** (Source C). Stand near boss; cyclones retract toward boss but the stack-in-melee positioning is the tightest spot, where cyclone overlap is unlikely. If a cyclone clips a player, healer compensates. Bot behavior: stay near boss, don't try to dodge actively.

### Disagreement #8 — Howling Gale handling

- **Source A**: kill them with DPS on the bridge (they're killable, low HP)
- **Source B**: they're environmental objects, not killable; just disable temporarily by attacking
- **Source C**: avoid the AoE entirely; cross at edges

**Default:** treat them as objects to **DPS down** (whether they "die" or "disable" is mechanically equivalent for the strat). Bot tank engages → DPS bursts → cross when AoE stops → DPS resumes on next gale. **All bots stay on tank's path** to avoid edge-dropping.

### Disagreement #9 — Cloud Prince Turbulence

- **Source A**: ranged DPS uses defensive CDs during Turbulence cast; melee out-ranges
- **Source B**: full-group AoE DPS down before Turbulence resolves
- **Source C**: tank-only takes Turbulence; rest spread

**Default:** Cloud Prince is a **kill-fast** target. AoE-burn before Turbulence completes; if it lands, healer compensates. Bot ranged DPS uses defensive on visible AoE-cast.

---

## DBC verification (UNVERIFIED items)

Spell IDs and stack values listed below are from community-guide cross-reference + Wowhead URL inference. Add to `docs/research/dbc-verification-checklist.md` for resolution when the TC fork session opens.

| # | Spell / mechanic | Listed value | Why it matters for bot logic |
|---|---|---|---|
| 1 | Ertan Cyclone Shield | 86292 | Encounter detection — shield-active = stay-in-ring positioning rule |
| 2 | Ertan Storm's Edge | 86309 | Outside-ring DoT — "stay near boss" override trigger |
| 3 | Ertan Lightning Bolt | UNVERIFIED (likely 86331) | Interrupt priority list (medium) |
| 4 | Ertan Summon Tempest | 86340 | Add-spawn detection (heroic) |
| 5 | Ertan Cyclone Shield retract cadence | ~30s | Predictable healer pre-HoT signal |
| 6 | Altairus Chilling Breath | 88308 | Cone-avoidance trigger; damage school = Frost (resistance check) |
| 7 | Altairus Twisting Winds | 88314 | Heroic-only patrol-tornado evasion trigger |
| 8 | Altairus Call the Wind / Upwind / Downwind | UNVERIFIED | Aura presence drives positioning logic; magnitude cosmetic |
| 9 | Altairus Lightning Blast | UNVERIFIED | Anti-leash damage; not bot-relevant unless bot tries to leave platform |
| 10 | Altairus wind-direction shift cadence | ~30s | Reposition-on-shift trigger |
| 11 | Asaad Chain Lightning | 87622 | Spread-trigger; bounce range = 40y |
| 12 | Asaad Static Cling | 87618 | Magic-dispel priority entry; root duration 18s |
| 13 | Asaad Unstable Grounding Field | 86911 | Triangle-spawn event; positional-override trigger |
| 14 | Asaad Supremacy of the Storm | 86930 | Channel duration 6s; outside-triangle = death |
| 15 | Skyfall Star summon | UNVERIFIED | Add-spawn detection; ranged-DPS-swap trigger |
| 16 | Skyfall Star Arcane Barrage damage | 5,400-6,600 normal / 10,000-11,000 heroic (UNVERIFIED) | Healer-pressure modeling |
| 17 | Static Cling root duration | 18s | Dispel-priority window |
| 18 | Asaad Static Cling cast end JUMP window | sub-second | Bot defaults to dispel (no JUMP) — verify if precise timing exists |
| 19 | Howling Gale (trash) HP / killable status | UNVERIFIED | Bot DPS-target acquisition |
| 20 | Lurking Tempest target-suppression on facing | UNVERIFIED | Bot target-acquisition exclusion |

---

## Notes on prompt deviations

- **No optional bosses**: Vortex Pinnacle has only **3 bosses, all required** (unlike Blackrock Caverns which has Beauty as an optional 4th). The encounter doc therefore has no skip-default boss section.
- **Heroic-only mechanics** are unusually concentrated on this dungeon: Lurking Tempest spawns (Ertan), Twisting Winds (Altairus), Static Cling (Asaad). Bot's heroic-detection branch is meaningful here.
- **Slipstream traversal is THE bot-implementation flag** — flag for `AltbotFollow.cpp` extension. Vehicle-entry is a known gap (similar to Oculus drake fight, but lighter-weight: this is just a one-shot vehicle ride, not a vehicle-combat phase). Documented in encounter.md instance-wide notes for followup.
- **Falling-off-platform** = teleport to dungeon entrance. Bot path-finding must avoid platform-edge approaches; "knocked off" by Howling Gale or Twisting Winds is a real hazard.
- **Bot camera/facing mechanics** (Lurking Tempest, Chilling Breath) — bots don't really "face" in a player-meaningful way. Bot AI treats these as **target-acquisition rules** (don't acquire tempest as target = harmless) rather than camera-direction rules.
- **Static Cling JUMP-avoidance** is technically the optimal play but bot defaults to **dispel-as-fallback** because sub-second JUMP timing is unreliable for AI.
- **Many spell IDs are confirmed via Wowhead URL inference** (URL was `wowhead.com/cata/spell=NNNNN/spell-name`). Where the URL was confirmed in search results, the ID is treated as VERIFIED. Where only the spell name was confirmed, ID is UNVERIFIED.
- Wowhead Cata is the spell-ID source of record; warcraft.wiki.gg is the strongest secondary for ability tables (NPC IDs, damage ranges, durations).
- **Mistral Slipstreams** mentioned in the prompt: this refers to either the trash mob "Armored Mistral" (slipstream-themed elemental) OR the "Slipstream" tornado-traversal vehicles themselves. Cata-original "three players who interact with crystals" mentioned in the prompt does **not** match any reconciled-guide content for Vortex Pinnacle — it may be a confusion with another encounter (e.g., Throne of the Tides Lady Naz'jar's pillar event). **No "three players + crystals" event** appears in this dungeon per all 10 sources reconciled.

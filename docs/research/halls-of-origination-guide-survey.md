# Halls of Origination — Cataclysm 5-man Survey

**Date:** 2026-05-03
**Patch:** 4.3.4
**Audience:** bot-control engineer (cata-altbot)

---

## Overview

| Field | Value |
|---|---|
| Zone | Halls of Origination (interior of the Uldum pyramid) |
| Continent | Uldum (Kalimdor) |
| Level range (normal) | 84–85 |
| Level range (heroic) | 85 |
| Group size | 5 (1 tank / 1 healer / 3 DPS) |
| Boss count | 7 (3 required + 4 "Constructs", with 3 of the 4 Constructs technically optional in original 4.0 but ALL counted as required for full clear / Cata Classic LFD path) |
| Layout | **Non-linear hub-and-spoke** — central Vault-of-Lights chamber branches out to the four Construct halls (Setesh, Ammunae, Isiset, Rajh) |
| Cata-launch class | Yes — released with Cata launch as one of the two "endgame" 5-mans (alongside Throne of the Tides) but with Heroic-only intended difficulty |
| Group buffs/debuffs | None unique to instance (no instance-wide aura) |
| Notable: longest Cata 5-man | Yes — 7 bosses, 35–55 minutes typical |

The instance is the **longest Cata 5-man** in terms of boss count. Layout is a central pyramid chamber (the Vault of Lights) with four side halls. The path is "**Anhuur** (gate boss) → **Ptah** (after sand corridor) → **Vault of Lights warden gauntlet + Anraphet** → **four Constructs in any order** (Setesh, Ammunae, Isiset, Rajh)." **Rajh is the de-facto final boss** as he's at the top of the pyramid; the other three Constructs are mechanically equivalent in tier and are typically cleared first. In original 4.0 patch notes, three of the four Constructs were "optional"; in practice the LFD random-dungeon system flags them all required for completion credit, so the bot treats all 7 as mandatory. Skips are still possible if a group leader explicitly requests them.

---

## Sources

This survey reconciles the following community sources. Wowhead Cata is treated as authoritative for spell IDs and numeric values; strategy is cross-checked across 5 distinct guides.

1. **Wowhead Cata — encounter & NPC pages** (`/cata/npc=...`, `/cata/spell=...`) — primary for spell IDs, cast times, boss/add NPC IDs, debuff effects
2. **Icy Veins — Halls of Origination Dungeon Guide** (`icy-veins.com/cataclysm-classic/halls-of-origination-dungeon-guide`) — boss-by-boss strategy with heroic deltas
3. **Wowhead Cata Guide — Halls of Origination Strategy** (overview) — community-edited
4. **Dexerto — Cata Classic HoO guide** — concise mechanics + heroic differences
5. **Glow's Branches — Halls of Origination dungeon guide** (glowberry.wordpress.com) — original 4.0-era player-perspective walkthrough; canonical source for trash gauntlet / drink-windows / "Earthrager and 3-of-4 end bosses optional" framing
6. **GotWarcraft — Heroic Halls of Origination Boss Guide** — heroic-specific deltas
7. **Ten Ton Hammer — Cataclysm: Halls of Origination Guide** — early-Cata strategy reference, beta-derived
8. **Warcraft Wiki — Vault of Lights (Halls of Origination)** — canonical zone description for the elemental warden gauntlet
9. **Blizzplanet — Setesh / Anhuur strategy posts** — original-Cata heroic walkthroughs
10. **Altered Gamer — Cataclysm Heroic Dungeon Guide: Halls of Origination** — broad heroic reference

Where guides disagree, the disagreement is captured in the **Disagreements** section and the consensus position is listed in **Strong consensus**.

---

## Strong consensus (table)

| Mechanic | Consensus |
|---|---|
| Anhuur Shield-of-Light | Triggers at **66% and 33%** HP; boss is **immune to damage and interrupts** during the shield |
| Anhuur lever / beacon mechanic | **Two players** drop into the pit (one each side), kill **Pit Vipers**, then activate **Beacons of Light** with 8s cast (10s on heroic) — both must be activated to drop the shield |
| Anhuur Reverberating Hymn | **Stacking AoE** while shield is up; all guides agree damage escalates over the channel — drives the urgency of beacon activation |
| Anhuur Divine Reckoning | **Magic debuff** on random player; explodes after 8s for AoE damage AND **heals the boss** for 5x damage dealt — requires positioning (debuffed player moves OUT of group) |
| Ptah Tumultuous Earthstorm | At **50% HP**, boss disperses; **Dustbone Horrors (2)** + **Jeweled Scarabs (8)** spawn; AoE down adds; boss reforms |
| Ptah Quicksand | Random ground pools — slow + nature DoT; movement-out |
| Ptah Raging Smash | Tank cleave; melee positions side/rear |
| Vault of Lights warden gauntlet | **Four elemental wardens** (Water/Fire/Air/Earth) gate Anraphet; constant-spawning **Stone Trogg** trash ring around the chamber while wardens are alive |
| Anraphet spawn trigger | After 4th warden dies; Anraphet emerges and **AoE-kills all troggs** AND damages party (unavoidable burst) |
| Anraphet Alpha Beams | Random-target shadow AoE leaves **persistent void zones on heroic** (despawn on normal); MUST move |
| Anraphet Crumbling Ruin | Soft-enrage; stacks reduce **max HP by 10%**, up to 10 stacks → DPS check |
| Anraphet Nemesis Strike | Tank shadow DoT — **dispellable Magic** on tank; canonical dispel call for the fight |
| Isiset split phases | At **66% and 33%** HP, splits into 3 / then 2 Astral Familiars (Astral Rain, Celestial Call, Veil of Sky variants); kill familiars to despawn-and-reform |
| Isiset Supernova | **Face-away** mechanic — anyone facing her gets disoriented 4s; everyone turns away on cast |
| Isiset familiar kill priority | **Astral Rain first, then Celestial Call, then Veil of Sky** is the most-reported priority (some guides flip the latter two) |
| Ammunae Seedling Pods | Spawn periodically; must be killed before Rampant Growth converts them to Bloodpetal Sprouts (high damage adds) |
| Ammunae Wither | **Magic dispel on player** — slows attack/cast/movement 60% for 10s |
| Ammunae Consume Life Energy | Channel on player; **interruptible**; boss gains energy + heavy damage to target if uninterrupted |
| Setesh untankable | **No threat table** — cannot be tanked. Tank's job is **portal-add management** (kite/cleave Sentinels + Wurms; DPS the portals) |
| Setesh Anti-Magic Prison | Void Seeker cast on healer or tank → blocks magical healing; **MUST INTERRUPT** |
| Setesh Void Sentinel | 90% damage reduction (Void Barrier); ignore unless DPS skips portals; tank kites |
| Setesh Chaos Portal heroic | Portals **do NOT auto-close** on heroic; DPS must destroy portals to stop adds |
| Setesh Seed of Chaos | Detonate on approach; player damage **+100% for 15s** (some guides) when walked-on — beneficial, walk through it |
| Rajh Solar Energy | Boss starts at **100 energy**; spends 20/cast on Sun Strike / Sun Orb / Inferno Leap / Solar Winds; at **<10 energy** triggers **Blessing of the Sun** recharge phase |
| Rajh Blessing of the Sun | Recharge channel — boss takes increased damage AND **all players gain +100% damage for 8s**; the canonical burn window |
| Rajh Sun Strike | Wide AoE fire damage every 50y; tank DoT |
| Rajh Inferno Leap | Boss leaps to random player; **interruptible**; deals fatal fire damage on landing |
| Rajh Summon Sun Orb | Channel-summon orb; **interruptible** |
| Rajh Solar Winds | Roving fire tornado — kite-out |
| Trash CC priorities | Stone troggs (Setesh halls + Vault gauntlet) are **CC-able humanoid** types; Shadowlancers (Anraphet approach) good Polymorph/Sap targets |
| Drink windows | Top of each gauntlet stair landing (before Vault of Lights gauntlet, between mini-bosses if possible); boss-by-boss; **canonical mana-test = Setesh halls / Setesh fight** due to constant Anti-Magic Prison + add cleave |

---

## Boss 1 — Temple Guardian Anhuur

NPC ID: 39425. First boss, Chamber of Prophecy (entry hall after the entrance corridor). Single-phase tank-and-spank with two scripted "shield" interrupts that gate the kill.

### Phase structure

Three-phase cycle (driven by HP %, not timer):

1. **Phase 1 (100% → 66%)**: tank-and-spank with Burning Light AoEs and Divine Reckoning debuffs
2. **Shield phase 1 (66%)**: Shield of Light + Reverberating Hymn channel; **2 players drop** to pit, kill Pit Vipers, activate Beacons of Light
3. **Phase 2 (66% → 33%)**: same as P1
4. **Shield phase 2 (33%)**: same as Shield 1
5. **Phase 3 (33% → 0)**: final push; same mechanics as P1

### Abilities

| Spell | ID | Cast / cooldown | School | Notes |
|---|---|---|---|---|
| Burning Light | 75117 | ~10s ground duration | Holy | 4y radius ground AoE; 24-26k Holy/sec; **avoidable — move out** |
| Divine Reckoning | 75592 | 8s explode timer | Holy | Magic debuff on random player; detonates 7y AoE; **heals boss 5x dealt**. **Dispel Magic** to cancel before detonation; if not dispelled, debuffed player moves OUT of group |
| Shield of Light | 80747 | scripted at 66%/33% | — | Boss immune to damage AND interrupts; triggers Reverberating Hymn |
| Reverberating Hymn | 75322 | continuous channel | Holy | 400/4s baseline; **damage stacks/escalates** over channel — must drop shield ASAP |
| Pit Viper poison (Poison-tipped Fangs) | 74538 | from snake adds | Nature | 3.7-4.3k/3s for 15s; **dispel as Poison** (or just ignore on the snake-killer) |

### Adds

- **Pit Viper** (NPC 39444) — 6+ snakes patrol the pit beneath each side platform; spawn during Shield of Light phase. The 2 lever-runners must kill snakes to reach the lever.

### Interrupts required

- **Reverberating Hymn** — **CANNOT** be interrupted while shield is up (interrupt-immune); the only way to stop the song is to drop the shield via the beacons. Bot interrupt logic must NOT waste a kick on Hymn during shield.

### Dispels required

- **Divine Reckoning** — Magic dispel on whoever is debuffed; canonical dispel call for the fight (unless strat is "let it explode and just heal through").
- **Pit Viper poison** — Poison dispel on the snake-killer if hit; otherwise ignore.

### Avoidable damage

- Burning Light blue ground — 4y radius; move out
- Divine Reckoning detonation — debuffed player moves out of group; **DO NOT cluster**

### Tank-swap requirements

None.

### Heroic delta

- Beacon activation extends from **8s → 10s** cast (longer pit exposure)
- More Pit Vipers per side
- Reverberating Hymn damage scales harder; shield-down urgency increased

### Add-handling

- Pit Vipers handled by the 2 lever-runners (typically 2 DPS); tank stays topside on the platform
- Some guides recommend the healer go into the pit (unusual; see Disagreement #1)

### Healer-pressure spikes

- **Reverberating Hymn channel**: stacking AoE damage on whole group while shield is up
- Divine Reckoning detonation if not dispelled / not moved out
- Pit Vipers' poison DoT on lever-runners

### Key positioning

- **Tank** holds boss centrally on the upper platform, facing away from group
- **2 DPS** drop into pits (one east, one west) on each Shield of Light cast
- **Healer + 1 DPS** stay topside; healer free-positions; topside-DPS keeps up boss damage between shields
- **Beacon of Light cast** is interruptible by mob damage — pit-runners must clear vipers BEFORE cast

### Bot-relevant role notes

- **Tank**: tank-and-spank topside; face boss away from group; do not need to drop into pit (some strats put tank in pit on heroic — see Disagreement #1).
- **Healer**: stay topside; raid-heal Reverberating Hymn; dispel Divine Reckoning (Magic) on affected player.
- **Melee**: melee boss on platform; on shield, **drop into nearest pit** to clear vipers + flip beacon. **bot_implementation flag**: requires understanding of "drop off platform" navigation — non-trivial.
- **Ranged**: drop into other pit on shield; clear vipers with AoE; flip beacon; come back up.
- **Door event positioning** is the most critical bot-implementation hazard for this boss — drop-zone path-finding is a special case the bot must handle.

---

## Boss 2 — Earthrager Ptah

NPC ID: 39428. Second boss, in the sand corridor after Anhuur. The boss room is a wide sandy chamber with idle camels around the perimeter (mountable for 50% movement speed bonus — the canonical "Halls of Origination camel race").

### Phase structure

Two-phase:

1. **Phase 1 (100% → 50%)**: tank-and-spank with Earth Spike + Quicksand + Flame Bolt
2. **Tumultuous Earthstorm (50%)**: boss **disperses** (becomes untargetable); **Dustbone Horror** (2) + **Jeweled Scarab** (8) adds spawn; party AoEs adds while a sandstorm tornado roams (heroic-only sand vortex)
3. **Phase 2 (post-add)**: boss reforms after adds die; resume P1 mechanics; final burn

### Abilities

| Spell | ID | Cast / cooldown | School | Notes |
|---|---|---|---|---|
| Flame Bolt | 75540 | 5s channel | Fire | Channels fire damage to 2 random players (3 on heroic/Mythic) |
| Earth Spike | 75339 | 4s ground delay | Nature | Erupts after 4s; AoE + knock-up within 4y; move-out |
| Raging Smash | 83650 | melee | Physical | Cleave — 150% damage on tank + 2 nearest enemies; melee positions to side/rear |
| Quicksand | 75547 | passive ground | Nature | 7y radius sand pool; nature DoT every 2s + 50% slow; move-out |
| Tumultuous Earthstorm | (scripted) | at 50% HP | — | Boss disperses; sandstorm tornado on heroic; adds spawn |
| Smash (Dustbone Horror) | 75453 | melee | Physical | Add-melee at 125%; tank picks up |

### Adds

- **Dustbone Horror** (NPC 40808) — 2 spawn at 50% HP; large skeleton; tank picks up
- **Jeweled Scarab** (NPC 40458) — 8 spawn at 50% HP; low HP; AoE-burn

### Interrupts required

- **Flame Bolt** is a 5s channel — **interruptible** per most guides. Lower priority than other kicks but a free interrupt window.

### Dispels required

None.

### Avoidable damage

- **Earth Spike** — 4s telegraph; move out
- **Quicksand** pools — visible on floor; move out
- **Sandstorm tornado** (heroic only) — kite-out

### Tank-swap requirements

None.

### Heroic delta

- Flame Bolt targets **3 players** instead of 2
- During Tumultuous Earthstorm, a **moving sand vortex** spawns and roams the room — kite-out
- More Quicksand pools
- Dustbone Horrors hit harder

### Add-handling

- AoE down scarabs first (8 low-HP); horrors second (2 high-HP); tank picks up horrors via AoE-threat
- **Camel mounts** around the room give 50% speed buff — useful during add phase for kiting tornado on heroic

### Healer-pressure spikes

- 5s Flame Bolt channels (predictable)
- Earth Spike on multiple players (group spread for multi-spike)
- Add phase cleave damage

### Key positioning

- **Tank** central; face boss away from group; rotate boss during Quicksand spawns
- **Melee** behind/side; out of cleave arc
- **Ranged** spread to avoid Earth Spike multi-target; max range
- During add phase: stack-AoE on scarab cluster; tank-pulled horrors fed into the cleave

### Bot-relevant role notes

- **Tank**: standard tank-and-spank with positioning. **AoE management during burrow phase** is key — tank picks up Dustbone Horrors immediately on spawn, scarabs come to closest-player.
- **Healer**: Flame Bolt channels are healer pressure; pre-shield Flame Bolt targets if predictable.
- **Melee**: side/rear; do NOT stand in Quicksand; do NOT stand in front (Raging Smash cleave).
- **Ranged**: max range; spread; AoE on add cluster.
- **Camels are world-mountable** — bot mount logic could leverage them but it's non-trivial; default is "ignore camels" and run on foot.

---

## Boss 3 — Vault of Lights warden gauntlet (mini-boss tier) + Anraphet

This is mechanically **two encounters in one room**:

### Part A — Elemental Warden Gauntlet

The Vault of Lights chamber has 4 mini-bosses on side platforms. While any warden is alive, the chamber is **continuously spawning Stone Trogg trash** that the tank/group must hold. Each warden is a soft tank-and-spank with 1 unique mechanic.

| Warden | Approx mechanic |
|---|---|
| **Flame Warden** | Periodic fire nova ("Raging Inferno"); spread out before nova |
| **Water Warden** | Bubbles trap players; group breaks the bubbles to free trapped player |
| **Air Warden** | Whirlwind tornadoes roam; kite-out |
| **Earth Warden** | Knockback nova; spread to avoid collective throw |

Wardens can be killed in any order. Stone Troggs continue spawning until ALL 4 are dead. **Drink window: top of stair landing BEFORE entering chamber. No drink time during the gauntlet.**

### Part B — Anraphet (NPC 39788)

After the 4th warden dies, **Anraphet awakens** and casts an **unavoidable room-wide AoE** that kills all remaining troggs and damages the party (~30-50% group HP loss, depending on gear). Heal-through; do not panic. Then he becomes targetable.

### Phase structure (Anraphet)

Single phase, recurring 3-ability cycle:

1. **Nemesis Strike** on tank (DoT)
2. **Alpha Beams** on random target (AoE)
3. **Crumbling Ruin** stack (max-HP reduction)
4. *(occasional)* **Omega Stance** — 8s pulsing AoE channel

### Abilities

| Spell | ID | Cast / cooldown | School | Notes |
|---|---|---|---|---|
| Nemesis Strike | 75604 | melee | Shadow | Tank shadow DoT; **Magic dispel** on tank — canonical dispel call |
| Alpha Beams | 76184 | channel 3s | Shadow | Channels at random player; 5y radius beam every 0.5s for 3s; **heroic: leaves persistent void zone** |
| Omega Stance | 75623 | 8s channel | Shadow | Pulses AoE shadow damage every 1s; reduces movement 50% for 4s |
| Crumbling Ruin | 75609 | post-Beam/Stance | Shadow | Reduces max HP **10% per stack, up to 10 stacks**; **soft enrage** |

### Adds

- **Stone Troggs** during warden gauntlet — humanoid, CC-able, AoE-burn target
- No adds during Anraphet himself

### Interrupts required

- None on Anraphet (no hardcasts that demand kicks)
- Stone Trogg casters during gauntlet: interruptible casts; rotate kicks

### Dispels required

- **Nemesis Strike on tank** — Magic dispel; CRITICAL (tank shadow DoT will pressure healer)

### Avoidable damage

- **Alpha Beams** — random target; **MOVE OUT of beam path immediately**
- **Heroic void zones** — persistent ground patches from Alpha Beams; movement-puzzle as fight progresses
- **Omega Stance** — unavoidable AoE; pop healer cooldown; group stacks for AoE heal efficiency

### Tank-swap requirements

None.

### Heroic delta

- Alpha Beams **persistent void zones** — the floor fills with hazards over time; group must navigate
- Omega Stance hits harder
- Crumbling Ruin stacks faster — DPS check tightens

### Add-handling (Vault gauntlet)

- Stone Troggs spawn continuously while wardens alive
- Tank holds aggro on troggs + active warden simultaneously
- AoE classes excel here (DK Death and Decay, Pally Consecration, Druid Swipe, etc.)
- **Drink between mini-bosses if HP/mana allow** — but the gauntlet doesn't pause; troggs keep coming

### Healer-pressure spikes

- **Initial Anraphet awakening AoE** — unavoidable, big spike
- Omega Stance channel
- Tank Nemesis Strike DoT (drives dispel call)
- Crumbling Ruin compresses healer's max-HP, making heals less effective

### Key positioning

- **Vault gauntlet**: stack-near-tank for cleave AoE; spread for Earth Warden knockback / Fire Warden nova
- **Anraphet**: tank centrally; ranged/healer max range, spread
- **Heroic void zones**: occupy clean ground; rotate group around the room as zones accumulate

### Bot-relevant role notes

- **Tank**: AoE-threat during gauntlet (high reactivity needed — troggs spawn continuously); standard tanking on Anraphet.
- **Healer**: dispel Nemesis Strike Magic on tank; pop AoE heal cooldown for Omega Stance. **Mana pacing** through the gauntlet is the second OOM-test (after Setesh).
- **Melee**: AoE on troggs; standard ST on Anraphet from rear; move-out of Alpha Beams.
- **Ranged**: AoE / spread on troggs; max range on Anraphet; movement-priority for void zones.

---

## Boss 4 — Isiset, Construct of Magic

NPC ID: 39587. Construct boss in the Seat of Magic. Magical / arcane theme.

### Phase structure

Three "split" phases driven by HP thresholds (66% / 33%):

1. **Phase 1 (100% → 66%)**: Isiset solo; Astral Rain + Celestial Call + Veil of Sky abilities; Supernova face-away
2. **Phase 2 (66%)**: Isiset splits into **3 Astral Familiars** (mirror images); kill ONE — that ability is permanently disabled; **the other 2 empower remaining instances**; Isiset reforms
3. **Phase 3 (33%)**: Isiset splits into **2 Astral Familiars** (the surviving ones); kill ONE; final Isiset reform with one ability remaining
4. **Final burn (sub-33%)**: Isiset with one ability; kill

### Abilities

| Spell | ID | Cast / cooldown | School | Notes |
|---|---|---|---|---|
| Supernova | 74136 | ~2-3s cast | Arcane | 24-26k Arcane damage AoE; **disorients players FACING the boss** for 4s; **face away on cast** |
| Astral Rain | 74135 | 8s channel | Arcane | Star-shower AoE; arcane every 3s for 8s; group-positional |
| Celestial Call | 74364 | scripted | — | Summons **Celestial Familiar** add(s); they cast Arcane Barrage |
| Veil of Sky | 74133 | scripted | — | Damage shield + reflect; **dispellable Magic via Spellsteal/Purge** |
| Arcane Barrage (Familiar) | 74374 | cast 2.5s | Arcane | Standard caster nuke; 9-10k arcane |

### Mirror-image NPCs (split phase)

| Familiar | NPC ID | Empowered ability |
|---|---|---|
| Astral Rain (familiar) | 39720 | doubles/triples Astral Rain damage when its sister-familiars die |
| Celestial Call (familiar) | 39721 | summons additional Celestial Familiars (1→2→3 cumulative) |
| Veil of Sky (familiar) | 39722 | absorb/reflect scales 130k → 260k → 390k |

### Add-handling

- **Astral Familiars (mirror images)** — kill ONE per split phase; sister-familiars despawn when Isiset reforms
- **Celestial Familiars** (summoned by Celestial Call) — small adds with Arcane Barrage; AoE-burn or focus down

### Kill-priority within split

Most guides say: **Astral Rain familiar first**, then **Celestial Call**, then **Veil of Sky**. Rationale — Astral Rain damage scales the worst as it empowers (5.4k → 6.6k → 9.0k per tick); leaving it for last means raid-wide AoE pressure during final phase. (Some guides flip Veil and Celestial — see Disagreement #2.)

### Interrupts required

- **Arcane Barrage** (Familiar adds) — interruptible; lower priority than the boss face-away mechanic but free kicks if available

### Dispels required

- **Veil of Sky** — dispellable Magic via **Spellsteal** (Mage) or **Purge** (Shaman); Priest Mass Dispel works. Dispel/steal removes the shield and the reflect — significant DPS gain.

### Avoidable damage

- **Supernova** — face-away mechanic; non-positional damage but disorient-on-face
- **Astral Rain** — ground star-shower; move-out

### Tank-swap requirements

None.

### Heroic delta

- All damage values higher (no novel mechanics)
- Familiar HP higher; split phase takes longer

### Healer-pressure spikes

- Astral Rain channels (8s of AoE)
- Final phase if Astral Rain familiar wasn't killed first

### Key positioning

- **Tank** holds Isiset central, facing AWAY from group (Supernova face-away)
- **All players** turn AWAY from Isiset on Supernova cast
- **Ranged + healer** max range, spread for Astral Rain
- During split: focus-fire on assigned familiar (kill priority above)

### Bot-relevant role notes

- **Tank**: face boss away on Supernova; tank-and-spank otherwise.
- **Healer**: cooldown for Astral Rain channel; standard tank healing.
- **Melee**: positional behind boss; turn-away on Supernova cast — **bot_implementation flag**: face-away mid-combat is unusual; bot must temporarily reverse facing.
- **Ranged**: face-away on Supernova; max range; assist with Astral Rain familiar kill.
- **Mage / Shaman / Priest**: **Spellsteal/Purge/Mass Dispel** Veil of Sky if encountered.

---

## Boss 5 — Ammunae, Construct of Life

NPC ID: 39731. Construct boss in the Seat of Life. Plant / nature theme.

### Phase structure

Single phase with continuous add management:

1. **Pull → kill**: Ammunae channels Consume Life Energy on a player while Seedling Pods spawn periodically; pods must be killed before Rampant Growth turns them into Bloodpetal Sprouts; periodic Wither debuff dispels and Spore add spawns

### Abilities

| Spell | ID | Cast / cooldown | School | Notes |
|---|---|---|---|---|
| Wither | 76043 | ~2s cast | Nature | Debuff on random player; **−60% attack/cast/move speed for 10s**; **dispellable Magic** |
| Consume Life Energy | 75665 | 4s channel | Nature | Channels on player; 8k/sec + stun; **boss gains 12 energy/sec while channeling**; **MUST INTERRUPT** |
| Rampant Growth | 75790 | at 100 energy | Nature | Converts ALL Seedling Pods to Bloodpetal Sprouts; deals 29-34k Nature AoE; **the soft-enrage** |
| Noxious Spores | 75702 | passive cloud | Nature | 6y cloud after Spore add dies; 10-12k/1.5s; lasts 30s; **MOVE OUT** |
| Pod Energize (passive) | (UNVERIFIED) | passive | — | Each living Seedling Pod buffs Ammunae: +5% damage, +30% melee haste — drives kill priority |

### Adds

- **Seedling Pod** (NPC 51329) — passive plant; boss buff while alive; converts to Bloodpetal Sprout on Rampant Growth
- **Bloodpetal Blossom** (NPC 40622) — converted/grown pod; 150% melee + bleed (3.6-4.7k/2s); active threat
- **Spore** (NPC 16286) — slow-moving sphere; on death drops Noxious Spores cloud

### Interrupts required

- **Consume Life Energy** — primary kick target; the channel feeds boss energy AND damages target
- **Wither** — interruptible per most guides; lower priority than CLE

### Dispels required

- **Wither** — Magic dispel; debuffed player is essentially incapacitated for 10s without dispel

### Avoidable damage

- **Noxious Spores** clouds — move out; persistent 30s
- **Seedling Pod buff** (passive) — kill pods to remove

### Tank-swap requirements

None.

### Heroic delta

- Pods spawn more frequently
- Pods actively heal the boss (per some guides — see Disagreement #3)
- Higher-damage Bloodpetal Sprouts

### Add-handling

- **Seedling Pods** — kill priority HIGH (passive boss-buff source); ranged DPS focus
- **Bloodpetal Sprouts** — active threat; tank picks up if conversion happens; AoE down
- **Spores** — kill, then move out of cloud

### Healer-pressure spikes

- Consume Life Energy channel target (8k/s + stun)
- Wither debuffed player (can't cast, can't move)
- Rampant Growth burst (29-34k AoE)

### Key positioning

- **Tank** central on Ammunae; face away from group
- **Ranged DPS** focus pods + spores; move out of clouds
- **Melee** Ammunae; help spore kills if close
- **Healer** max range; movement-aware (clouds)

### Bot-relevant role notes

- **Tank**: standard tanking + adds pickup on conversion.
- **Healer**: dispel Wither; raid-heal Rampant Growth; tank-heal CLE channel target.
- **Melee**: target-swap to Sprouts on conversion; otherwise boss DPS.
- **Ranged**: kill Seedling Pods on spawn (high priority); kill Spores; move out of clouds.

---

## Boss 6 — Setesh, Construct of Destruction

NPC ID: 39732. Construct boss in the Seat of Destruction. **The canonical mana-test** of Halls of Origination.

### Phase structure

Single chaotic phase; Setesh has **NO threat table** (untankable). Continuous portal-add cycling.

1. **Pull**: Setesh attacks random players with Chaos Bolt; **Chaos Portals** open periodically; adds spawn from portals; Seed of Chaos buffs scattered
2. **Repeat until kill**: kill portals on heroic (auto-close on normal); manage adds; burst when DPS-buffed by Seed of Chaos

### Abilities

| Spell | ID | Cast / cooldown | School | Notes |
|---|---|---|---|---|
| Chaos Bolt | 50796 | 2s cast | Shadow | Random target; 10-13k unresistable; **NOT interruptible** in most reports |
| Reign of Chaos | 77030 | scripted | Shadow | Spawns void bubbles; 7.8k/sec to anyone in bubble; move-out |
| Chaos Blast | 76681 | 5s ground delay | Shadow | Lands after 5s; persistent void zone; move-out (this is the cone-style one some guides describe) |
| Seed of Chaos | 76870 | passive ground | Shadow | Player walks into seed → detonates → **+100% damage buff for 15s**; **walk through deliberately** |
| Summon Chaos Portal | (scripted) | every ~30s | — | Opens portal; Sentinel + Wurms + Seeker spawn (heroic: portal open until destroyed) |

### Adds (per portal)

| Add | NPC ID | Key ability | Notes |
|---|---|---|---|
| Void Sentinel | 41208 | **Void Barrier** (63710) — 90% damage reduction; **Charged Fists** (77238) — physical buff | Skip-DPS; tank kites with roots/slows |
| Void Seeker | 41371 | **Anti-Magic Prison** (76903) — traps player, prevents magical healing | **MUST INTERRUPT** — healer-blocking |
| Void Wurm | 41374 | low HP; pairs spawn | AoE-burn |

### Interrupts required

- **Anti-Magic Prison** (Void Seeker) — **PRIMARY KICK TARGET**; if landed on healer or tank, that player loses healing for the duration
- Reign of Chaos / Chaos Blast — not interruptible (scripted)

### Dispels required

- **Anti-Magic Prison** — UNVERIFIED whether dispellable; most guides say "interrupt the cast" not "dispel afterward"

### Avoidable damage

- **Reign of Chaos** void bubbles — move out
- **Chaos Blast** ground patches — move out (5s telegraph)
- **Seed of Chaos** — **WALK INTO** for damage buff (only mechanic in dungeon where ground effect is positive)

### Tank-swap requirements

None — boss is untankable. "Tank" role is **add-tank for portal adds**.

### Heroic delta

- **Portals do NOT auto-close** — DPS must destroy them (this is the single biggest heroic change in the dungeon)
- Faster portal cycle / more adds per portal
- Anti-Magic Prison casts more frequent
- Mana-drain pressure increases

### Add-handling

- **Tank** kites Void Sentinels (rooted/slowed if possible); maintains threat on Wurms; intercepts Seekers if loose
- **DPS** burns portals (heroic) → Wurms → Seekers (kick first)
- **Sentinel** is a low-DPS-priority "wall" — only kill if no portals/Wurms/Seekers active
- DK tank trick: Chains of Ice spam keeps adds slow (per Dexerto)

### Healer-pressure spikes

- **Anti-Magic Prison** if cast lands — affected player can't be healed magically
- Chaos Bolt unresistable damage on whoever is targeted (random)
- **MANA DRAIN** is constant — healer mana through Setesh is the most-tested moment in the dungeon

### Key positioning

- **Tank** mobile — kites adds around the room; doesn't pin Setesh (untankable)
- **Healer** max range; movement-friendly positioning
- **DPS** mobile; portal-priority means lots of target swaps
- **Seed of Chaos** detonation — walk through deliberately for the +100% damage buff

### Bot-relevant role notes

- **Tank**: this is **the most non-standard tank fight in Cata 5-mans**. No threat to maintain on boss. Bot tank logic must:
  - Detect "untankable boss" state — DON'T try to threat Setesh
  - Switch to add-tank mode — kite/cleave Sentinels + Wurms + Seekers
  - Use slows/roots aggressively (Chains of Ice on DK; Frost Trap-style for any class)
- **Healer**: **CANONICAL DUNGEON MANA-TEST**. Constant pressure from Anti-Magic Prison + add cleave + boss random hits + portal-add mana-drains. Pop Mana Tide / Innervate / Hymn of Hope here. **bot_implementation flag**: this is the dungeon's mana-stress check — flag healer behavior accordingly.
- **Melee**: portal priority; AoE on Wurms; interrupt Seekers; walk-into Seed of Chaos.
- **Ranged**: same priority; Seed of Chaos walk-into; portal kills on heroic.

---

## Boss 7 — Rajh, Construct of the Sun

NPC ID: 39378. Construct boss at the top of the pyramid (de-facto final boss). Sun / fire theme. Rajh is **energy-resource-driven**.

### Phase structure

Energy-based cycle:

1. **Active phase (energy 100 → 0)**: Rajh casts abilities at 20-energy each; cycle through Sun Strike / Sun Orb / Inferno Leap / Solar Winds
2. **Recharge phase (energy <10)**: triggers **Blessing of the Sun** — boss channels in center of room, 8s; players get **+100% damage** AND boss takes increased damage; **canonical burn window**; AoE damage on group
3. **Repeat** until kill

A geared group can kill Rajh in **one Blessing-of-the-Sun window** (the achievement "Sun of a..." rewards killing him before he completes a full recharge phase).

### Abilities

| Spell | ID | Cast / cooldown | School | Notes |
|---|---|---|---|---|
| Sun Strike | 73872 | scripted | Fire | 18-21k Fire damage to ALL players within 50y (raid-wide); **applies undispellable tank DoT** |
| Summon Sun Orb | 80352 | 3s channel | Fire | Summons orb in 13y radius; **interruptible** |
| Inferno Leap | 87653 | 3s telegraph | Fire | Leaps to random player; 10y radius fatal damage on landing; **interruptible**; targets get +50% movement to escape |
| Solar Winds | 74108 | scripted | Fire | Roving fire tornado(s); 4y radius; **kite-out** |
| Blessing of the Sun | 76355 | 8s channel | Fire | Recharge phase; AoE damage on group; **+100% player damage buff during channel**; boss takes increased damage |
| Solar Fire | 89131 | heroic-only | Fire | Persistent flame patches dropped by Solar Winds movement |

### Adds

- **Sun Orb** (summoned by Summon Sun Orb) — temporary; AoE burst
- **Solar Winds tornado** — environmental hazard; not killable

### Interrupts required

- **Summon Sun Orb** — **PRIMARY KICK** on its 3s channel
- **Inferno Leap** — **PRIMARY KICK** on its 3s telegraph; saves a random player from a leap-kill
- Sun Strike — NOT interruptible (scripted)

### Dispels required

- Sun Strike's tank DoT is **UNDISPELLABLE** — heal through

### Avoidable damage

- **Sun Orb** landing — move out of orb radius
- **Inferno Leap** landing — leap target moves AWAY (50% speed buff helps escape)
- **Solar Winds** tornadoes — kite-out
- **Solar Fire** patches (heroic) — move out

### Tank-swap requirements

None.

### Heroic delta

- **Solar Fire** (89131) — persistent flame patches drop along Solar Winds movement; the floor fills up
- All damage values higher
- Achievement-window tightens (one-Blessing kill window)

### Healer-pressure spikes

- Sun Strike raid-AoE (every cycle)
- Tank Sun Strike DoT (continuous)
- Blessing of the Sun channel (group-AoE while +100% damage buff is up — risky balance)

### Key positioning

- **Tank** central; face away from group (no front cone but spread is good)
- **All players** spread for Inferno Leap (so leap doesn't multi-hit)
- **Solar Winds** kite — group spread + run-out lanes
- **Blessing of the Sun**: stack-OR-spread depending on healer's AoE preference; group survives the channel and bursts boss

### Bot-relevant role notes

- **Tank**: standard tank; defensive CDs through Sun Strike DoT.
- **Healer**: predictable Sun Strike AoE; Blessing of the Sun heavy throughput; Solar Winds movement healing.
- **Melee**: kite-out Solar Winds; interrupt Sun Orb / Inferno Leap; max-burst during Blessing.
- **Ranged**: max range; kick rotation on Sun Orb / Inferno Leap; max-burst during Blessing.
- **Cooldown alignment**: save offensive CDs (Bloodlust if not used; Berserk; Avenging Wrath; Combustion) for the Blessing of the Sun window — this is the canonical "burn window" of the fight.

---

## Trash mechanics

Halls of Origination has **the most trash** of any Cata launch 5-man, due to the Vault gauntlet and the Setesh halls add-cycle.

### Trash before Anhuur

- **Anubisath sentinels** — high-HP humanoids; tank-and-spank with cleave
- **Earthen Guardians** — patrol; CC-able

### Trash before Ptah (sand corridor)

- **Anubisath patrols** — periodic patrols; pull individually
- **Quicksand** environmental — visible pools; avoid
- **Scarab swarms** — small AoE-able cluster mobs

### Vault of Lights gauntlet (BEFORE Anraphet) — see Boss 3 above

- **4 Elemental Wardens** — water / fire / air / earth mini-bosses
- **Stone Troggs** — continuous-spawn humanoid trash (CC-able); AoE-burn
- **Drink window**: BEFORE entering chamber (top of stair landing); no drink time during gauntlet

### Setesh Halls — pre-Setesh (CANONICAL MANA TRASH)

The hall leading to Setesh has a gauntlet-style add cycle that drains mana even before the boss. Per Glow's Branches: "during the gauntlet, stick close to the tank and drink at stair landings."

- **Void Sentinels** (mini, in trash form) — same Void Barrier mechanic
- **Shadowlancers** — humanoid CC-target (good Polymorph/Sap target)
- Constant small-pull cycle drains mana — **healer drinks at stair landings between pulls**

### Trash before Isiset / Ammunae / Rajh

- **Anubisath / Atramedes-style elite mobs** — standard tank-and-spank
- **Caster trash** with frontal channels — turn away

### CC priorities

- **Polymorph / Sap / Hex / Banish** valid on Anubisath humanoids, Stone Troggs, Shadowlancers
- **Earthen Guardians** sometimes immune (Elementals); Bind Elemental works
- Void Sentinels (trash form) — generally NOT CC'd; AoE-burn or skip-tank

### Skip-able trash

- **Setesh Halls partial skip**: Some guides describe wall-hugging or stealth-route to bypass certain Setesh trash packs (group composition dependent)
- **Pre-Construct trash** mostly CAN be skipped if running for speed; not standard for LFD

### Pull markers for CC

- Stone Trogg packs in Vault gauntlet: skull on lead caster, square on second
- Setesh halls packs: cross/X on the Shadowlancer (Polymorph), skull on a melee

---

## Disagreements

Numbered list of points where the 5+ reconciled guides do not agree. Bot logic should pick a default and note the deviation.

### Disagreement #1 — Anhuur lever / pit assignments

- **Source A (Icy Veins):** "2 DPS drop into pits"
- **Source B (Glow's Branches):** "tank or healer can drop on heroic if DPS gear is too low to clear vipers fast"
- **Source C (Dexerto):** "Heroic requires whole party to move together as a unit while designated DPS flip switches"

**Default:** **2 DPS drop into pits**, healer + tank + 1 DPS stay topside. Heroic-difficulty composition adjustments are encounter-runtime decisions; bot defaults to "2 DPS drop." Tank/healer drop is a fallback only.

### Disagreement #2 — Isiset familiar kill order

- **Source A:** Astral Rain → Celestial Call → Veil of Sky
- **Source B:** Astral Rain → Veil of Sky → Celestial Call (Veil's reflect grows scariest)
- **Source C:** kill whichever has **lowest HP first** (mathematical, but unprincipled)

**Default:** **Astral Rain first** (consensus, drives raid AoE), then **Celestial Call** (consensus on second), then Veil of Sky last. Bot logic uses a fixed kill-order; Mage/Shaman/Priest can Spellsteal/Purge/Mass-Dispel Veil of Sky if encountered.

### Disagreement #3 — Ammunae heroic Seedling Pod heal effect

- **Source A:** Pods on heroic actively HEAL the boss (boss DPS check tightens)
- **Source B:** Pods only provide passive damage/haste buff; no direct heal

**Default:** treat pods as **kill-priority HIGH on both difficulties**; don't differentiate bot logic. The buff alone is enough motivation.

### Disagreement #4 — Setesh "ignore portals and burst" vs "kill portals" strategy

- **Source A (Glow's Branches):** Conservative — destroy portals before third wave to prevent Anti-Magic Prison
- **Source B (Dexerto / Icy Veins):** Aggressive — burst boss with cooldowns; ignore portals; tank kites all adds
- **Source C:** depends on group HP — both work

**Default:** **Hybrid** — kill PORTALS on heroic (portals don't auto-close); on normal portals auto-close, so burst boss while tank holds adds. Bot decision: heroic = portal priority, normal = boss-burst priority. **Always kick Anti-Magic Prison regardless.**

### Disagreement #5 — Setesh Anti-Magic Prison: dispel or kick?

- **Source A:** Interrupt the cast (most reports)
- **Source B:** Dispellable Magic if landed (UNVERIFIED — some say yes, some say no)
- **Source C:** "Just heal-through if it lands"

**Default:** **Always kick Anti-Magic Prison cast**. Dispellability is UNVERIFIED — bot does not attempt magic dispel on it; if cast lands, healer triages.

### Disagreement #6 — Rajh kill-window: one Blessing or two?

- **Source A (gear-equipped Cata Classic):** Kill in one Blessing of the Sun (achievement window)
- **Source B (early-Cata original):** Two Blessings is normal; one is exceptional

**Default:** bot does not gate behavior on achievement windows; **save offensive CDs for Blessing of the Sun**, burst hard. Whether kill is 1 or 2 Blessings is a gear/composition outcome.

### Disagreement #7 — Vault of Lights warden order

- **Source A:** Kill wardens in any order; doesn't matter
- **Source B:** Specific order recommended (Earth → Air → Fire → Water for damage uniformity)
- **Source C:** Kill closest first

**Default:** **any order** — wardens are mechanically independent. Bot picks closest-to-tank for AoE-cleave efficiency.

### Disagreement #8 — Anhuur pit healer-presence

- **Source A:** Healer NEVER drops into pit (healer must keep raid alive topside through Reverberating Hymn)
- **Source B:** On heroic, sometimes the healer drops with 1 DPS to assist with vipers, healing on the way
- **Source C:** "The 2 lever-runners are 2 DPS, never the healer"

**Default:** **healer NEVER drops** — staying topside is required to keep the topside group alive through the channel. Bot healer logic for Anhuur shield phase: stay topside, raid-heal Reverberating Hymn.

### Disagreement #9 — Setesh trash gauntlet drink window

- **Source A:** Drink at stair landing BEFORE Setesh halls
- **Source B:** Drink between every other pull (non-canonical; speed-clear groups don't drink at all)
- **Source C:** Drink at stair landing AFTER Anraphet, BEFORE Setesh

**Default:** **always full drink at stair landing AFTER Anraphet, BEFORE Setesh halls**. Setesh fight is the dungeon's mana-test; entering OOM is a fight loss.

### Disagreement #10 — Earthrager Ptah: optional or required

- **Source A (original 4.0 patch notes / Glow's Branches):** "Earthrager and 3 of 4 end bosses are optional" — only Anhuur, Anraphet, and Rajh required
- **Source B (Cata Classic LFD):** All 7 bosses required for LFD completion credit
- **Source C:** "Required for full clear; optional for Valor-cap speed runs"

**Default:** treat ALL 7 as **required** for LFD bot behavior. Skip behavior reserved for explicit group-leader instruction.

---

## DBC verification (UNVERIFIED items)

Spell IDs and stack/timing values listed below are from community-guide cross-reference, not direct DBC lookup. Add to `docs/research/dbc-verification-checklist.md` for resolution when the TC fork session opens.

| # | Spell / mechanic | Listed value | Why it matters for bot logic |
|---|---|---|---|
| 1 | Anhuur Burning Light | 75117 | Avoidable AoE — bot move-out trigger |
| 2 | Anhuur Divine Reckoning | 75592 | Magic dispel target + 8s detonation timer |
| 3 | Anhuur Shield of Light | 80747 | Phase trigger; bot stops DPS-on-boss; signals door event start |
| 4 | Anhuur Reverberating Hymn | 75322 | AoE-heal cooldown alignment |
| 5 | Pit Viper poison (74538) | 74538 | Snake kill priority for lever-runners |
| 6 | Anhuur beacon cast time | 8s normal / 10s heroic | Bot lever-runner timer |
| 7 | Ptah Flame Bolt | 75540 | Channel interrupt window |
| 8 | Ptah Earth Spike | 75339 | Move-out 4s telegraph |
| 9 | Ptah Raging Smash | 83650 | Tank cleave; melee positioning |
| 10 | Ptah Quicksand | 75547 | Move-out ground hazard |
| 11 | Anraphet Nemesis Strike | 75604 | **Magic dispel on tank** — canonical dispel call |
| 12 | Anraphet Alpha Beams | 76184 | Move-out + heroic void zone marker |
| 13 | Anraphet Omega Stance | 75623 | AoE healer cooldown alignment |
| 14 | Anraphet Crumbling Ruin | 75609 | Soft-enrage — DPS check signal |
| 15 | Isiset Supernova | 74136 | **Face-away mechanic** — bot facing override |
| 16 | Isiset Astral Rain | 74135 | Move-out channel |
| 17 | Isiset Veil of Sky | 74133 | **Spellsteal/Purge/Mass Dispel target** for hybrid casters |
| 18 | Isiset Celestial Call | 74364 | Familiar summon |
| 19 | Ammunae Wither | 76043 | **Magic dispel on player** |
| 20 | Ammunae Consume Life Energy | 75665 | **MUST INTERRUPT** — primary kick |
| 21 | Ammunae Rampant Growth | 75790 | Soft-enrage; convert pods |
| 22 | Ammunae Noxious Spores | 75702 | Move-out cloud |
| 23 | Setesh Chaos Bolt | 50796 | Random-target damage; not interruptible |
| 24 | Setesh Reign of Chaos | 77030 | Move-out bubbles |
| 25 | Setesh Chaos Blast | 76681 | Move-out 5s ground delay |
| 26 | Setesh Seed of Chaos | 76870 | **WALK INTO** for +100% damage buff |
| 27 | Void Seeker Anti-Magic Prison | 76903 | **MUST INTERRUPT** — primary kick |
| 28 | Void Sentinel Void Barrier | 63710 | DPS-skip flag (90% damage reduction) |
| 29 | Void Sentinel Charged Fists | 77238 | Tank kite trigger |
| 30 | Rajh Sun Strike | 73872 | Raid-AoE healer cooldown alignment + tank DoT |
| 31 | Rajh Summon Sun Orb | 80352 | **MUST INTERRUPT** — kick rotation |
| 32 | Rajh Inferno Leap | 87653 | **MUST INTERRUPT** — kick rotation |
| 33 | Rajh Solar Winds | 74108 | Kite-out tornado |
| 34 | Rajh Blessing of the Sun | 76355 | **OFFENSIVE CD WINDOW** — bot cooldown alignment |
| 35 | Rajh Solar Fire (heroic) | 89131 | Heroic-only persistent fire patches |
| 36 | Pit Viper NPC | 39444 | Add NPC ID |
| 37 | Dustbone Horror NPC | 40808 | Add NPC ID |
| 38 | Jeweled Scarab NPC | 40458 | Add NPC ID |
| 39 | Astral Familiar NPCs (3 split forms) | 39720 / 39721 / 39722 | Kill-priority targets in Isiset split phase |
| 40 | Seedling Pod NPC | 51329 | Kill-priority target |
| 41 | Bloodpetal Blossom NPC | 40622 | Active-threat add |
| 42 | Spore NPC | 16286 | Move-out target |
| 43 | Void Sentinel NPC | 41208 | Tank-kite target |
| 44 | Void Seeker NPC | 41371 | Kick-priority caster |
| 45 | Void Wurm NPC | 41374 | AoE-burn target |
| 46 | Anhuur dispel-blacklist | none | Divine Reckoning IS dispellable; no blacklist needed |

NPC IDs are mostly cross-confirmed via Wowhead Cata pages (`/cata/npc=<id>`); spell IDs from Wowhead Cata spell pages with one or two cross-references.

---

## Notes on prompt deviations

- **Skipped spec selection** as instructed — no spec recommendation section.
- **Heroic delta is per-boss** (matches BRC / ToT format).
- **No optional / skip-default boss** — unlike Beauty in BRC, Halls of Origination has no canonical-skip side branch. Original 4.0 had Earthrager and 3-of-4 Constructs flagged optional, but in Cata Classic LFD all 7 are required for completion. **Bot default: clear all 7 bosses.**
- **Vault of Lights is treated as a single combined "boss 3" entry** — the 4 elemental wardens + Anraphet are mechanically chained (warden death → Anraphet auto-spawn). They share the same chamber. Splitting them into separate boss entries would suggest tactical separation that doesn't exist (you cannot leave Vault between warden and Anraphet — the AoE awakening is automatic).
- **Setesh untankable** is the most non-standard bot tank requirement in any Cata 5-man — flagged for `bot_implementation` heavily.
- **Anhuur door event** (lever-runners) is the most non-standard bot navigation requirement — flagged for `bot_implementation` heavily.
- **Setesh halls is the canonical mana-test** — flagged in the survey for healer.md emphasis.
- **Rajh is final boss**. The four Constructs are technically equal-tier; convention places Rajh last as the "Sun" / top-of-pyramid encounter and as the most mechanically demanding. Bot default order: Anhuur → Ptah → Anraphet → Setesh → Ammunae → Isiset → Rajh (puts mana-tests early so drink windows align before Rajh).
- **Many spell IDs are UNVERIFIED** — Halls of Origination launch-tier dungeon, community guides cross-reference each other rather than DBC. Recommend verifying items #1–#5 (Anhuur), #11–#14 (Anraphet — Nemesis Strike especially as it's the canonical dispel call), #19–#20 (Ammunae — interrupt + dispel calls), #27 (Anti-Magic Prison — interrupt call), #34 (Blessing of the Sun — CD alignment) before any bot encounter logic references these constants compile-time.
- **No instance-wide buff/debuff** — same as BRC; no Halls-of-Origination-specific aura.
- Wowhead Cata is the spell-ID source of record; where Wowhead data was thin (heroic delta details, kill priorities), Icy Veins / Glow's Branches / GotWarcraft filled in. Where guides disagreed, the disagreement was logged.

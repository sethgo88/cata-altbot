# The Stonecore — Cataclysm 5-man Survey

**Date:** 2026-05-02
**Patch:** 4.3.4
**Audience:** bot-control engineer (cata-altbot)

---

## Overview

| Field | Value |
|---|---|
| Zone | The Stonecore (Deepholm — west side of Temple of Earth) |
| Continent | Deepholm (Maelstrom) — flying-mount entrance |
| Level range (normal) | 82–84 (mid-tier launch Cata 5-man, post-BRC) |
| Level range (heroic) | 85 |
| Group size | 5 (1 tank / 1 healer / 3 DPS) |
| Boss count | 4 required, no optional pre-boss |
| Layout | Mostly linear with one switchback ramp; minor branching around the Slabhide pre-boss approach |
| Cata-launch class | Yes — released 4.0.3a; one of the Deepholm levelling 5-mans paired with Throne of the Tides for entry into Heroic gating |
| Group buffs/debuffs | None unique to instance (no instance-wide aura) |

The instance is mid-length (~25-35 min on heroic for a geared group; on launch tier it was infamous as the hardest of the entry-Heroic 5-mans, primarily due to **Ozruk's Paralyze → Shatter** combo). Trash density is moderate-high; the path is "entrance ramp → Millhouse Manastorm trash event → first chamber (Corborus) → switchback ramp + Crystalspawn Giant + Magmalord packs → Slabhide chamber → Ozruk corridor → Azil's chamber (Heart of Destruction)." There is **no optional/skippable boss** unlike BRC's Beauty.

The Stonecore's signature mechanic is **Ozruk's Paralyze → Shatter** combo on Heroic: an 8-second stun followed by a wipe-tier AoE that requires either a HoT/DoT to break the stun or specific damage-source tricks. It is the primary skill check of the launch-tier Heroic gate.

---

## Sources

This survey reconciles the following community sources. Wowhead Cata is treated as authoritative for spell IDs and numeric values; strategy is cross-checked across 5 guides.

1. **Wowhead Cata — encounter, NPC, and spell pages** (`/cata/npc=...`, `/cata/spell=...`) — primary for spell IDs, cast times, schools, durations, damage values
2. **Icy Veins — Stonecore Strategy Guide** (`icy-veins.com/cataclysm-classic/the-stonecore-dungeon-guide`)
3. **Warcraft Wiki — boss & trash pages** (`warcraft.wiki.gg/wiki/Slabhide`, `Corborus`, etc.)
4. **Dexerto Cataclysm Classic walkthrough** (`dexerto.com/.../wow-cataclysm-classic-stonecore-guide-...`)
5. **Engadget original-Cataclysm guide** (`engadget.com/2010-12-06-cataclysm-dungeon-guide-the-stonecore.html`) — launch-era mechanics
6. **Ten Ton Hammer Cataclysm guide** (`tentonhammer.com/articles/cataclysm-guide-the-stonecore`)
7. **MMO-Champion Tank's Guide thread** (attempted; 403 — content cross-referenced via search excerpts)
8. **WowEngineer farming notes & Syrco Owl beta notes** — supplementary anecdotal sources

Where guides disagree, the disagreement is captured in the **Disagreements** section and the consensus position is listed in **Strong consensus**.

---

## Strong consensus (table)

| Mechanic | Consensus |
|---|---|
| Corborus has two cycling phases | Ground phase (Crystal Barrage + Dampening Wave) → Burrow phase (Thrashing Charges + Rock Borer adds); recurs until kill |
| Corborus Crystal Barrage = spread-out | Group **spreads** (5 yard radius per tick, 4-second channel); **opposite of Rom'ogg/BRC stack rule** |
| Corborus Dampening Wave is dispellable | **Magic** debuff with 15k heal-absorb (heroic; 4k normal); top dispel priority on Obsidius-style "always dispel ASAP" rule |
| Corborus Rock Borers must die fast | They cast Rock Bore (15s 10-stack bleed); leftover stacks ramp tank damage in next ground phase |
| Slabhide ground/air cycling | Ground phase with Sand Blast cone + Lava Fissure → at scripted intervals lifts off, drops Stalactites, then channels Crystal Storm (heroic-only damage) requiring LoS-break behind stalactites |
| Slabhide Sand Blast = frontal cone | 60-degree cone, 15-yard range — tank faces away from group; ranged stays out of arc |
| Slabhide Lava Fissure | Ground-target on random player; 5s telegraph (3s heroic) → fire eruption + lava pool (10s normal / 30s heroic). Move out of crack |
| Slabhide stalactite avoidance | Visible ground texture telegraph; persists 30-35s; usable as LoS shield on heroic Crystal Storm |
| Ozruk Ground Slam = run through boss | 3-second cast, 4-yard radius in front of boss; **everyone (tank included) runs through to behind** |
| Ozruk Spike Shield → Shatter cycle | 10s shield (any melee/ranged hit causes 5-stack bleed) → at expiration **Shatter fires a 5-yard AoE Frost burst — wipes group if not at range** |
| Ozruk Bulwark spell-reflect | 10s buff; **all spells reflected** — caster bots stop casting for the duration |
| Ozruk Paralyze (heroic) | 8-second stun applied with Shatter; **damage taken removes the stun**, including DoT ticks (Hunter Serpent Sting, Warlock Corruption, etc.) — must have a DoT or HoT-via-melee already applied to break |
| Azil two-stage cycling | Stage 1 ground (Force Grip + Gravity Well + Curse of Blood + Devout Followers) → Stage 2 levitation w/ Energy Shield + Seismic Shard tombstones; cycles on time/HP |
| Azil Force Grip is interruptible | **Channeled** (1.5s cast, 5s channel); **kick** the channel on the tank — high interrupt priority |
| Azil Curse of Blood is dispellable | **Curse** type — Mage Remove Curse, Druid Remove Corruption (when talented), Shaman Cleanse Spirit (talented) |
| Azil Gravity Well | Pulls into center, ramping shadow damage; 20s normal duration; on heroic shrinks 25% per kill (use to AoE-grind Devout Follower adds) |
| Azil Devout Follower adds | Low-HP Twilight cultists; spawn in waves of 9-12 per cycle; AoE-burn while keeping Force Grip kicked |
| Heroic dispel pressure | Dampening Wave (Corborus, Magic) + Curse of Blood (Azil, Curse) — **two dispel-required encounters**, one of each type |
| Tank-swap requirement | None on any boss (single-tank fight); Corborus tank takes Dampening Wave heal-absorb + Rock Borer Bleed; Ozruk tank cannot avoid Ground Slam by line, must run through |
| Millhouse Manastorm trash event | Pre-Corborus event — Millhouse + 3-5 Stonecore mobs (Berserker, Earthshaper, Flayer); CC-required; Earthshaper's **Force of Earth** transformation cast must be interrupted |

---

## Boss 1 — Corborus

NPC ID: 43438. First boss, in The Winding Halls after the entry corridor and Millhouse Manastorm event. Cycles between ground phase (mostly stand-and-cast mechanics) and a Burrow phase with adds.

### Phase structure

Two-phase cycling:

1. **Ground phase** — tank-and-spank with periodic Crystal Barrage (spread) and Dampening Wave (dispel)
2. **Burrow phase** — boss submerges and is unattackable; **4 Thrashing Charges** erupt from random player locations (lethal direct hit), and **Rock Borer adds** spawn that must be AoE'd before they apply Rock Bore stacks
3. Boss resurfaces; cycle repeats. Burrow trigger is **scripted on a timer**, not HP-based (UNVERIFIED exact cadence; community estimates ~30-45s of ground phase between burrows — see **Disagreements #1**)

### Abilities

| Spell | ID | Cast / cooldown | School | Notes |
|---|---|---|---|---|
| Crystal Barrage | 86881 | 4s channel | Physical | Channeled; ticks every 0.5s in 5y radius around target. **Move out** (target moves; non-targets stay clear of pulled-up location). Heroic spawns Crystal Shard adds every 0.5s. |
| Dampening Wave | 82415 | instant | Shadow | 60y radius PBAoE; applies heal-absorb (4k normal / 15k heroic). **Magic dispel** — top priority. |
| Burrow | 26381 | scripted | — | Boss becomes unattackable. UNVERIFIED — Wowhead lists the spell as Cata generic Burrow; Stonecore-specific scripted entry. |
| Thrashing Charge | 81828 | 2.9s cast (per Wowhead) | Physical | Erupts from underground; 40-yard radius effect (the "leap path"); knockback 150; ~141k damage on heroic. **Direct hit lethal**; move out of dust-cloud telegraphs. |
| Rock Bore | 80028 | instant (Rock Borer add) | Physical | 15s bleed, 4k damage per 3s on heroic. Up to 10 stacks. Tank-targeted. |
| Crystal Shard (heroic) | UNVERIFIED summon | scripted | Physical | Heroic-only mob spawned by Crystal Barrage; ~7.5k HP; explodes if reaching player |

### Interrupts required

- None. No hardcast interruptible casts on Corborus himself. (Burrow and Thrashing Charge are scripted, not interruptible.)

### Dispels required

- **Dampening Wave (82415)** — Magic. Dispel **off the tank as priority**, then rest of group. **High dispel priority on every cast.**

### Avoidable damage

- **Crystal Barrage** target ground patch (5y radius for 4s)
- **Thrashing Charge** path during Burrow (dust-cloud telegraph; treat as void zone for the 2.9s cast)
- **Crystal Shard** add path on heroic (AoE them or kite away)

### Tank-swap requirements

None. Single tank.

### Heroic delta

- Crystal Barrage spawns Crystal Shards (extra AoE-add load)
- Dampening Wave heal-absorb is 15k (vs 4k normal) — dispel **mandatory** every cast
- Rock Bore bleed is 4k/3s vs 1k/3s — Rock Borers must die before tank ramp gets ugly
- Thrashing Charge damage scales hard (~141k) — direct hit is a one-shot

### Add-handling

- **Rock Borers** during Burrow phase — spawn at random player locations; **3 per spawn** (UNVERIFIED count — sources differ; Wowhead doesn't list a fixed count). AoE-burn before the 4 Thrashing Charges resolve.
- **Crystal Shards** on heroic Crystal Barrage — moving objects; AoE them in the path. Ranged DPS handles.

### Healer-pressure spikes

- Dampening Wave (predictable: every ~10-15s; healer pre-shields tank, waits for dispel before re-healing)
- Rock Bore bleed stacks on tank (ramp damage if Rock Borers live too long)
- Thrashing Charge direct hit (rare; near-lethal — recovery heal)

### Key positioning

- **Group spreads (5 yards apart minimum)** during ground phase — Crystal Barrage damage radius is 5y, no overlap
- During Burrow phase, watch for dust-cloud telegraphs — move perpendicular to the charge axis
- Tank stays in center of room so all Burrow exit points are reachable for re-acquire

### Bot-relevant role notes

- **Tank**: standard tank-and-spank in ground phase; during Burrow, tank picks up Rock Borers (low priority) and re-acquires Corborus on resurface
- **Healer**: dispel Dampening Wave **immediately** off tank (highest dispel priority of the dungeon); reactive heal during Crystal Barrage is normal
- **Melee**: stay on boss in ground; AoE Rock Borers in Burrow; dodge Thrashing Charge
- **Ranged**: spread for Crystal Barrage; AoE Crystal Shards on heroic; Thrashing Charge is the survival mechanic — predict erupt point from dust cloud

---

## Boss 2 — Slabhide

NPC ID: 43214. Second boss, encountered in The Overlook chamber. Triggered by killing the elemental pre-boss trash that drops from the ceiling. Fight has a clear ground/air cycling phase structure.

### Phase structure

Two-phase cycling:

1. **Ground phase** — Sand Blast (frontal cone) on tank + Lava Fissure (random player ground-target). Standard tank-and-spank with cone awareness and ground-fire dodging.
2. **Air phase** — Slabhide flies up; rains **Stalactites** that drop on visible ground textures. After landing, channels **Crystal Storm** (heroic-only damaging channel) that requires **LoS-break behind a stalactite pillar** to survive on heroic.
3. Cycle repeats. Air phase is **time-triggered** (UNVERIFIED exact cadence; community consensus ~60s between air phases — see **Disagreements #2**)

### Abilities

| Spell | ID | Cast / cooldown | School | Notes |
|---|---|---|---|---|
| Sand Blast | 80807 (UNVERIFIED) | instant | Nature | 60° forward cone, 15y range. ~47k heroic damage. **Tank-faces-away**; ranged out of arc. NOT interruptible (instant). |
| Lava Fissure (target) | 80803 | instant ground-target | Fire | Spawns crack on player; 5s normal / 3s heroic delay |
| Eruption (the actual damage) | 80800 | instant | Fire | Triggered by Lava Fissure; 5y radius fire damage + spawns lava pool |
| Lava Pool | UNVERIFIED | passive | Fire | ~24k Fire damage/sec; 10s normal / 30s heroic |
| Stalactite | 80643 | instant ground-target | Physical | Air-phase drop; ground-texture telegraph; **knockback** + heavy damage. Persists ~30-35s; blocks LoS and movement. |
| Crystal Storm (heroic only) | 92265 | 2.5s cast → 6s channel | Physical (per spell page) / Nature (per text) | Channels for 6s after landing from air phase; ticks every 0.1s in 8y radius. **LoS breaks the damage** — hide behind stalactites. UNVERIFIED school discrepancy — see DBC list. |

### Interrupts required

- None — Sand Blast and Lava Fissure are instant; Crystal Storm is a channel (not kick-style; mitigated by LoS, not interrupt)

### Dispels required

- None.

### Avoidable damage

- **Sand Blast cone** (frontal; tank faces away from group)
- **Lava Fissure crack** (5s/3s telegraph; move off the crack before eruption)
- **Lava Pool** (persistent ground fire after eruption)
- **Stalactite drop** (ground texture; move out of marked spot)
- **Crystal Storm** on heroic (LoS-break behind stalactite pillar)

### Tank-swap requirements

None.

### Heroic delta

- Lava Fissure delay shortens (5s → 3s)
- Lava Pool persists longer (10s → 30s); ground becomes increasingly hazardous
- **Crystal Storm channel adds** as a heroic-only mechanic — group must position behind stalactite pillars during the 6s channel
- Stalactite damage hits harder (one-shot territory if direct hit on heroic per Dexerto)

### Add-handling

None during the boss fight. (Slabhide pre-boss trash includes elementals that drop from ceiling — handled in **Trash** section.)

### Healer-pressure spikes

- Lava Fissure damage on the player who didn't move out (rare)
- Crystal Storm channel on heroic if LoS broken late (group damage spike)
- Stalactite hit on slow movement (random spike)

### Key positioning

- **Tank**: face boss away from group during ground phase
- **Melee**: behind boss; respond to Lava Fissure under feet by moving out
- **Ranged**: max range, out of cone arc; respond to Lava Fissure
- **All players during air phase**: avoid Stalactite ground textures; on heroic, **plan a stalactite-pillar pair to LoS Crystal Storm** before the channel starts
- **Healer**: always have line-of-sight to group; stalactites can break healing line — reposition proactively

### Bot-relevant role notes

- **Tank**: standard tank-and-spank with cone facing; rotate boss face during Lava Fissure under tank to keep cone safe
- **Healer**: reposition for stalactite LoS (this is a movement-aware fight); on heroic, the LoS-break Crystal Storm window is 6s of zero healing — pre-shield/pre-HoT before the channel
- **Melee**: **dodge Lava Fissure** is the main melee skill check; respond to ground-texture telegraph
- **Ranged**: Crystal Storm LoS-break on heroic — bot must "hide behind designated stalactite pillar" → encounter-specific positioning override

---

## Boss 3 — Ozruk

NPC ID: 42188. Third boss, in the Chamber of Fanatics corridor. The signature challenge of the dungeon and the launch-Heroic skill check.

### Phase structure

Single phase, cyclically using a **Spike Shield → Shatter → Bulwark → Ground Slam** rotation (sources don't agree on the exact order — see **Disagreements #3**). On heroic, **Paralyze** is added to the Shatter cycle, creating the famous Paralyze → Shatter wipe-tier combo.

The fight is structured around three rules:
1. **Run through boss** when Ground Slam casts (everyone, tank included)
2. **Stop attacking when Spike Shield is up** (melee bleed); **stop casting when Bulwark is up** (spell reflect)
3. **Get to ≥ 5 yards from boss** when Shatter fires (5y radius AoE)

### Abilities

| Spell | ID | Cast / cooldown | School | Notes |
|---|---|---|---|---|
| Ground Slam | 78903 | 3s cast | Physical | Self-centered AoE; 4-yard radius **in front** of boss. **Run through boss to behind**. ~94k heroic damage. Triggers Rupture on heroic. |
| Rupture | 1943 (Heroic) | follow-up to Ground Slam | Physical | 3 lines of rock spikes erupting in boss's facing direction; ~141k damage + knockback up. |
| Elementium Spike Shield | 78835 | instant | Arcane | 10s self-buff; melee/ranged attackers gain bleed (300/sec, 5 stacks max). On heroic, this is the buff that **triggers Paralyze on expiration** alongside Shatter. |
| Elementium Bulwark | 78939 | instant | Shadow | 10s self-buff; **all spells reflected** (100% reflect on heroic). Caster bots stop casting. |
| Shatter | 77715 | instant (on shield expiration) | Frost | 5y radius AoE, ~84k+ heroic damage. **Move to 5+ yards** before shield expires. |
| Paralyze (Heroic only) | 92426 | instant (on Shatter) | Arcane | 8s stun applied with Shatter; **damage taken removes the stun** (Magic dispellable, but DoT ticks also break it). Damage at expire if not broken. |

### Interrupts required

- **Ground Slam** is technically a 3-second cast but is **NOT interruptible** in standard play (most guides report immune to interrupt). One older guide ("murallocator.org") claims interruptibility — treat as **UNVERIFIED**, see **Disagreements #4**.
- All other casts are instant (Spike Shield, Bulwark, Shatter, Paralyze) — not kick-able.

### Dispels required

- **Paralyze (92426)** is **Magic** dispel-type per Wowhead spell page. **However**, the standard heroic strat is **NOT to dispel** — it relies on damage breaks (DoT/HoT ticks). Healer dispelling is a backup. Bot logic: **prefer the damage-break method** because it's reliable per-tick; dispel is a last-resort save.

### Avoidable damage

- **Ground Slam** (3s cast → run through boss before resolution)
- **Shatter** (5y AoE on shield expire → maintain ≥ 5y when 10s shield timer runs out)
- **Paralyze** (heroic) — break with damage taken, or healer dispel as backup
- **Spike Shield bleed** (don't melee/ranged-attack while shield is up — but on heroic, you NEED the bleed stack to break Paralyze. **Conflict** — see **Disagreements #5**)

### Tank-swap requirements

None.

### Heroic delta

- **Paralyze added** to Shatter cycle — 8-second stun
- Rupture (post-Ground Slam) adds spike-line damage and knockback
- Spike Shield bleed deals 600/sec instead of 300/sec
- The skill check is whether the group has a **DoT applied** that ticks during the Paralyze window to break the stun before it expires (and before Shatter resolves)

### Add-handling

None.

### Healer-pressure spikes

- Shatter resolution (5y AoE damage to anyone too close)
- Paralyze expiration damage (heroic; if not broken)
- Ground Slam direct hit (if a player fails to run through)
- Rupture knockback + damage (heroic)

### Key positioning

- **Tank pulls Ozruk into the corridor** and tanks against a wall — minimizes knockback room
- **All players run through boss** during Ground Slam cast (no facing-only tank)
- **All players step to ≥ 5y** at the 9-second mark of Spike Shield (1s before Shatter)
- On heroic, **all players have a DoT or HoT-via-melee applied** before Shatter so Paralyze breaks immediately

### Bot-relevant role notes

- **Tank**: tank-against-wall positioning; **run through boss** on Ground Slam (deliberate "into-AoE-then-out" movement); pop defensive on Spike Shield expiration
- **Healer**: do NOT auto-dispel Paralyze on heroic by default — let DoTs handle it; dispel is a fallback if the bot detects the stun is about to expire and no damage is breaking it. Pre-HoT before Shatter resolves so HoT ticks break Paralyze instantly.
- **Melee**: **stop melee when Spike Shield is up** (avoid bleed stacks ramping) **EXCEPT on heroic** where 1-2 stacks of bleed are acceptable to ensure Paralyze breaks immediately. Run through boss on Ground Slam.
- **Ranged casters**: **stop hardcasts when Bulwark is up** (100% reflect = self-damage). **Continue DoT applications** because DoTs already on the boss tick safely. On heroic, an active DoT on the boss is the Paralyze-break — do not let DoTs expire near the Shatter cast.

---

## Boss 4 — High Priestess Azil

NPC ID: 42333. Final boss, in Heart of Destruction. Two-stage cycling encounter with adds, dispels, and a unique vertical phase.

### Phase structure

Two-stage cycling:

1. **Stage 1 — Ground** — Force Grip channel on tank (interruptible kick), Gravity Well ground-targets that pull and damage, Curse of Blood (curse-dispel), Devout Follower add waves (9-12 low-HP cultists per wave)
2. **Stage 2 — Levitation** — Azil flies to her altar and gains Energy Shield (75% damage reduction); casts **Seismic Shard** 3 times (massive ground-target tombstone projectiles); more adds spawn during this stage
3. Cycles. Stage 2 trigger is **scripted on time** (UNVERIFIED — community consensus is ~60-90s between stage transitions)

### Abilities

| Spell | ID | Cast / cooldown | School | Notes |
|---|---|---|---|---|
| Force Grip | 79351 | 1.5s cast → 5s channel | Physical | Channeled lift on target (typically tank); 70k+ damage per slam tick. **Channeled = interruptible**. **HIGH KICK PRIORITY**. |
| Gravity Well | 79249 | instant | Shadow | Spawns at random player; 10y radius pull; ramping shadow damage to center. 20s normal duration; on heroic shrinks 25% per kill. |
| Curse of Blood | 16098 | 2s cast | Shadow | **Curse** dispel-type. Increases physical damage taken by 5000 (15s duration). **Mage Remove Curse / Druid Remove Corruption / Shaman Cleanse Spirit (talented)**. |
| Devout Follower (add) | NPC 42428 | scripted spawn | — | Low-HP cultists; 9-12 per wave. AoE-burn. |
| Energy Shield (Stage 2) | 79050 | 2s cast | Arcane | 75% damage reduction self-buff during levitation; deals 47k arcane + knockback to enemies within 5y on creation. |
| Seismic Shard | 79002 | scripted | Physical | Massive ground-target tombstone; 3.5y radius; ~942k-1057k damage on direct hit (heroic). **Cast 3 times in sequence in Stage 2**. |

### Interrupts required

- **Force Grip (79351)** — HIGH PRIORITY. Channeled cast; kick interrupts the channel and stops the slam-damage. Bot interrupt rotation must be ready every cast.

### Dispels required

- **Curse of Blood (16098)** — Curse dispel. Cast frequently on Stage 1; remove ASAP because it stacks with melee on a non-tank target.

### Avoidable damage

- **Gravity Well center** — pull mechanic; players must move OUTWARD against the pull or be dragged in for ramping damage
- **Seismic Shard tombstones** — visible ground-target telegraph; sidestep before impact (3.5y radius)
- **Energy Shield knockback** — when Azil enters Stage 2, anyone within 5y is knocked back; standard "stay at range when she lifts off"

### Tank-swap requirements

None.

### Heroic delta

- Gravity Wells **shrink only by killing units** (Devout Followers passing through them, players self-destructing, etc.) — strategic add-funnel through wells
- More Devout Followers per wave (9-12+ on heroic)
- Seismic Shard heroic damage is wipe-tier on direct hit
- Curse of Blood applied more frequently — dispel uptime is a dispel-cooldown management problem
- "Rotten to the Core" achievement (60 disciples in 10s) implies extra Disciple spawns on heroic — UNVERIFIED separate add type vs Devout Follower

### Add-handling

- **Devout Followers** — spawn in waves of 9-12 per cycle; 1-3 waves per Stage 1 (UNVERIFIED count — see **Disagreements #6**). **AoE-burn**. Funnel through Gravity Wells on heroic to shrink the wells while killing adds.

### Healer-pressure spikes

- Force Grip slam damage on tank if not interrupted (predictable: every 6+ seconds; interrupt rotation must be tight)
- Stage transition Energy Shield knockback (predictable; pre-HoT before transition)
- Seismic Shard direct hit (random; near-lethal)
- Curse of Blood + Devout Follower melee on the curse target (compounding)

### Key positioning

- **Tank**: holds Azil where she can be kicked (within kick range of melee); doesn't chase Force Grip lift
- **Stage 1 Gravity Well response**: move OUT of the well's pull radius (10y); on heroic, position the well in the path of incoming Devout Followers to shrink it
- **Stage 2 positioning**: spread out widely; the 3 Seismic Shards target separate ground spots — group not stacked = lower chance of cross-hit
- **Healer**: free positioning; stay at range from Energy Shield knockback when Stage 2 transitions

### Bot-relevant role notes

- **Tank**: tank Azil in the center; do NOT panic when Force Grip lifts (the kick will land); during Stage 2, focus on Devout Followers (the boss is 75% mitigated)
- **Healer**: **dispel Curse of Blood** ASAP (high-priority Curse dispel — bot decurses on detection); reactive heal during Force Grip channel (kick will end it but not before some damage)
- **Melee**: in melee on boss in Stage 1; **kick Force Grip** every cast (primary interrupt duty); switch to AoE on Devout Followers
- **Ranged**: **kick Force Grip** if class has the kick (Counterspell, Wind Shear, Silencing Shot); AoE Devout Followers; in Stage 2, sidestep Seismic Shards; do NOT cluster

---

## Trash mechanics

### The Millhouse Manastorm event (pre-Corborus)

Immediately after the entrance corridor descent, the player is greeted by **Millhouse Manastorm** (NPC 43391) flanked by **3-5 Stonecore mobs** (typically 2 Stonecore Berserkers + 2 Stonecore Earthshapers, but composition varies by source). This is the **hardest pull of the dungeon** per multiple guides — harder than Corborus per the Wiki page.

- **Millhouse Manastorm** — fragile caster; **Pyroblast** + **Frostbolt** hardcasts; **kick** his casts; he flees at low HP to next pull
- **Stonecore Berserker** — Charge (81574) + Spinning Slash (81568, 4s channel, 5y radius bleed-stacking AoE). **Kite-able**; back away from Spinning Slash channel.
- **Stonecore Earthshaper** — Force of Earth (81459, **5s cast**, transforms into earth elemental) — **MUST INTERRUPT** every cast. Failure to interrupt = group wipe per multiple sources.
- Trash adds may also include Stonecore Flayers (high-DPS AoE flay attack) and Crystalspawn Giants (large mobs with environmental knockback)

CC priority on this pull: Polymorph the Earthshaper(s); Hex / Sap / Repentance the Berserker; nuke Millhouse first.

### Pre-Slabhide trash

- **Stonecore Magmalord (NPC 42789)** — Ignite (single-target fire DoT) + Magma Eruption (AoE around target locations). Interrupt Ignite if 2s cast (UNVERIFIED).
- **Crystalspawn Giants** — large physical mobs; tank-and-spank but knockback aware
- **Stonecore Flayers** — fast cleave AoE (Flay attack); melee positions side/rear

### Pre-Ozruk trash

- **Twilight cultists** — humanoid casters; CC valid (Polymorph, Hex, Sap, Fear)
- **Earth elementals** — interrupt-required casters (UNVERIFIED specific abilities)
- The corridor narrows here — choose pull positions to avoid Ozruk patrol overlap

### Pre-Azil trash

- **Twilight followers** — humanoid casters; CC valid; some have Mind Control (UNVERIFIED) → break with dispel/cleanse
- Patrolling **Stonecore Earthshapers** in the hall

### CC priorities

- Polymorph / Hex / Fear / Sap / Repentance valid on humanoid Stonecore (Berserker, Earthshaper, Flayer, Twilight cultists)
- Earth elementals + Crystalspawn Giants are typically immune to humanoid CC (treat as nukes)
- Magmalord may or may not be CC-able (UNVERIFIED — humanoid CC tested by some, unclear)

### Skip-able trash

- A few mob clusters between Slabhide and Ozruk can be wall-hugged / LoS-skipped on speed-clears
- **No optional bosses** — entire spine must be cleared

### Pull markers for CC

- **Millhouse pull**: skull on Millhouse (kill first); sheep/hex on Earthshaper; sap/hex on Berserker
- Earthshaper packs: skull on Earthshaper (priority interrupt OR kill)
- Magmalord packs: skull on Magmalord; standard tank-and-spank

---

## Disagreements

Numbered list of points where the 5+ reconciled guides do not agree. Bot logic should pick a default and note the deviation.

### Disagreement #1 — Corborus Burrow phase trigger

- **Source A (Wowhead Cata page):** scripted on a timer; no HP threshold listed
- **Source B (Icy Veins, Engadget):** describes "periodic" burrows without specifying trigger
- **Source C (Dexerto):** describes Burrow as "submerges periodically"

**Default:** treat as **time-triggered** (~30-45s between burrows); bot uses scripted-event detection (boss target becomes unattackable + Rock Borer spawns) rather than HP-pct. Resolves the same way regardless of trigger model.

### Disagreement #2 — Slabhide air phase trigger

- **Source A:** time-triggered (~60s between air phases)
- **Source B (Wowhead community):** scripted, no specified cadence
- **Source C (Engadget):** "after a short time"

**Default:** treat as **time-triggered scripted event**. Bot uses event-detection (boss flies up = Slabhide.is_flying = true) rather than timer; this self-syncs to the real cadence whatever it is.

### Disagreement #3 — Ozruk Spike Shield → Shatter cadence and order

- **Source A (Wowhead spell page):** Spike Shield is 10s; Shatter triggers on expiration
- **Source B (Warcraft Wiki Ozruk):** sequence is Bulwark → Spike Shield → Shatter cyclically; Ground Slam is interspersed
- **Source C (Icy Veins):** sequence ambiguous; describes "periodic" Shatter

**Default:** treat the cycle as **scripted with detectable triggers**:
- Spike Shield aura on boss (78835) → **stop melee/ranged attacks** (or take 1-2 bleed stacks on heroic for Paralyze break)
- Bulwark aura on boss (78939) → **stop hardcasts**
- Shield expires + Shatter cast → **be at ≥ 5y**
Bot does not need to know the cadence; reactive on aura-state changes is sufficient.

### Disagreement #4 — Ozruk Ground Slam interruptibility

- **Source A (Wowhead spell page):** 3s cast; no interrupt flag listed
- **Source B (most strategy guides):** "non-interruptible — run through"
- **Source C (Murallocator):** "interrupt-capable players should prioritize Force Grip" — this looks like it's mixing up Force Grip (Azil) with Ground Slam (Ozruk)

**Default:** **NOT interruptible**. Bot does not attempt to kick Ground Slam; treats the 3s cast as a "run through boss" trigger. Murallocator's mention is likely a transcription error or confusion with Force Grip.

### Disagreement #5 — Ozruk Spike Shield melee policy on heroic

- **Source A (Icy Veins):** "stop melee when Spike Shield is up"
- **Source B (Wowpedia/Warcraft Wiki):** the bleed effect from attacking during Spike Shield is **the way to break Paralyze** — melee SHOULD attack 1-2 times to apply the bleed
- **Source C (Dexerto):** "stack intentionally to mitigate"

**Default:** **on normal** — stop melee when Spike Shield is up (avoid pointless bleed stacks). **On heroic** — accept 1-2 bleed stacks deliberately so the bleed DoT ticks break Paralyze when it's applied. This is the canonical "pre-apply DoT before Shatter" strategy. Ranged DoT classes (Warlock, Shadow Priest, Hunter Serpent Sting) provide this passively without the bleed-stack pain.

### Disagreement #6 — Azil Devout Follower wave count and timing

- **Source A (Warcraft Wiki):** spawns "to assist" — no count
- **Source B (Wowhead community):** 9-12 per wave; 1-3 waves per Stage 1
- **Source C (Engadget):** "frequently summons followers"

**Default:** treat as **scripted spawn waves**. Bot detects spawn event (NPC 42428 entering combat) and switches to AoE rotation; doesn't need to know the cadence. AoE-burn until clear, then return to boss/Force-Grip-kick rotation.

### Disagreement #7 — Azil Curse of Blood scaling values

- **Source A (Wowhead spell page):** +5000 physical damage taken
- **Source B (Warcraft Wiki):** +1000 physical damage taken (older value)

**Default:** use Wowhead's **+5000 heroic** value as canonical. The discrepancy is likely a Cata Classic re-tuning vs original 4.3.4. Numerical exact value doesn't affect bot logic — Curse of Blood is **always dispelled on detection** regardless of magnitude.

### Disagreement #8 — Slabhide Crystal Storm school

- **Source A (Wowhead spell page 92265):** Physical school
- **Source B (Warcraft Wiki + most guides):** Nature school
- **Source C (Icy Veins):** "Nature damage"

**Default:** use the spell-page (Physical) value as authoritative for game mechanics, but flag for DBC verification. Bot logic: treat as **damage school agnostic** for survival purposes (LoS-break is the response regardless of school).

### Disagreement #9 — Heroic Stonecore Berserker aggro mechanic

- **Source A (forum.warmane.com search excerpt):** "you cannot keep aggro on the Berserker — he ignores threat"
- **Source B (Warcraft Wiki):** lists Charge + Spinning Slash; no fixed-target / threat-ignore mechanic mentioned
- **Source C (Wowhead):** silent

**Default:** treat Berserker as **standard threat** until proven otherwise. The "cannot keep aggro" claim is not corroborated by primary sources. Warmane is a private server; behavior may differ from retail Cata 4.3.4. Flag for DBC/game-data verification at integration time.

### Disagreement #10 — Slabhide elementals as triggering pre-boss vs trash mob

- **Source A (Engadget):** elemental adds drop in before Slabhide flies down — they're a separate pre-boss event
- **Source B (Wowhead):** Slabhide is the encounter; elementals are trash leading up

**Default:** treat as **trash-then-boss** — clear elementals first; Slabhide pull begins on her descent.

---

## DBC verification (UNVERIFIED items)

Spell IDs and stack/timing values listed below need confirmation against TC's `Spell.dbc` files. Add to `docs/research/dbc-verification-checklist.md` for resolution when the TC fork session opens.

| # | Spell / mechanic | Listed value | Why it matters for bot logic |
|---|---|---|---|
| 1 | Corborus Burrow scripted entry | 26381 | Burrow event detection — bot phase-state machine |
| 2 | Corborus Burrow phase cadence | UNVERIFIED (~30-45s estimate) | If used as primary timer trigger; otherwise event-driven |
| 3 | Corborus Rock Borer add count per spawn | UNVERIFIED (sources say 3) | Add-AoE rotation timing |
| 4 | Corborus Crystal Shard heroic spawn | UNVERIFIED summon ID | Heroic-only AoE add detection |
| 5 | Slabhide Sand Blast | 80807 | Frontal cone tank-positioning trigger |
| 6 | Slabhide Lava Fissure target | 80803 | Ground-fire move-out trigger |
| 7 | Slabhide Eruption (the actual damage) | 80800 | Movement-time-window before fire damage |
| 8 | Slabhide Stalactite | 80643 | Air-phase ground-target avoid trigger |
| 9 | Slabhide Crystal Storm | 92265 | LoS-break trigger; school discrepancy (Physical vs Nature) |
| 10 | Slabhide air phase trigger | UNVERIFIED scripted | Phase-state detection |
| 11 | Ozruk Ground Slam | 78903 | "Run through boss" trigger; cast time 3s |
| 12 | Ozruk Spike Shield | 78835 | Stop-melee trigger; pre-Paralyze-DoT setup |
| 13 | Ozruk Bulwark | 78939 | Stop-hardcast trigger; spell reflect |
| 14 | Ozruk Shatter | 77715 | Move-to-5y trigger; school is Frost per spell page |
| 15 | Ozruk Paralyze (heroic) | 92426 | Damage-break or dispel-fallback trigger |
| 16 | Ozruk Rupture (heroic) | 1943 | Post-Ground-Slam knockback awareness |
| 17 | Azil Force Grip | 79351 | HIGH-PRIORITY interrupt; channeled |
| 18 | Azil Gravity Well | 79249 | Pull-mechanic ground-target avoid |
| 19 | Azil Curse of Blood | 16098 | Curse dispel trigger; +5000 phys damage taken |
| 20 | Azil Devout Follower NPC | 42428 | Add detection; AoE rotation switch |
| 21 | Azil Energy Shield | 79050 | Stage transition detection; knockback awareness |
| 22 | Azil Seismic Shard | 79002 | Tombstone ground-target avoid; cast 3 times in Stage 2 |
| 23 | Devout Follower wave count per Stage 1 | UNVERIFIED (1-3 waves of 9-12) | AoE-burn rotation duration |
| 24 | Heroic Disciple of Twilight separate NPC | UNVERIFIED | Achievement implies separate add type from Devout Follower |
| 25 | Stonecore Earthshaper Force of Earth | 81459 | **MUST INTERRUPT** trash flag |
| 26 | Stonecore Earthshaper Ground Shock | 81530 | Frontal-cone trash positioning |
| 27 | Stonecore Earthshaper Lava Burst | 81576 | Interruptible ranged cast |
| 28 | Stonecore Berserker Spinning Slash | 81568 | Bleed-AoE channel — kite mechanic |
| 29 | Stonecore Berserker Charge | 81574 | Charge-target prediction |
| 30 | Stonecore Magmalord Ignite + Magma Eruption | UNVERIFIED IDs | Trash interrupt + ground-fire avoid |
| 31 | Slabhide Crystal Storm school discrepancy | Physical (spell) vs Nature (text) | If school routing matters for bot resist/cooldown calculus |
| 32 | Stonecore Berserker "cannot hold aggro" mechanic | UNVERIFIED | Threat-anomaly flag if true |

---

## Notes on prompt deviations

- **No optional/skippable boss section** — Stonecore has no equivalent to BRC's Beauty. All 4 bosses are required. This shortens the per-boss section count to 4 from BRC's 5.
- **"Tank-swap requirements"** is included per-boss even when the answer is "none," for consistency with the BRC reference. None of the 4 bosses has a tank-swap requirement.
- **CC pull-markers** rolled into the Trash section rather than separate (matches BRC format).
- **Many spell IDs are confirmed via Wowhead spell pages** — Stonecore is better-documented than BRC because of the iconic Ozruk fight; ~75% of bot-relevant spell IDs are verified at Wowhead. Remaining UNVERIFIED items concentrate around (a) trash-mob spell IDs, (b) phase cadence values, (c) Stage 2 Azil add wave timing.
- **Disagreement count is higher than BRC** (10 vs 9) — primarily because of the Ozruk Spike Shield melee policy (Disagreement #5) which has heroic-vs-normal divergence and the Crystal Storm school discrepancy (#8).
- **Heroic skill check = Ozruk** — explicitly flagged. The Paralyze → Shatter combo is the Cata launch heroic gating mechanic. Bot heroic Stonecore logic must handle this as a dedicated case.
- **Dispel pressure is higher than BRC** — Stonecore has TWO dispel-required encounters (Corborus Magic, Azil Curse) plus a CONDITIONAL dispel target (Ozruk Paralyze). BRC was effectively zero dispels in standard play (with Crepuscular Veil being a dispel-blacklist).
- **Interrupt pressure on Azil is high** — Force Grip every 6+ seconds is a kick-rotation challenge. This is the dungeon's primary interrupt-rotation skill check.
- **Wowhead Cata is the spell-ID source of record**; where it disagreed with strategy guides, Wowhead won. Where Wowhead was silent, the disagreement was logged.
- **Web-archive fallback was attempted** for Engadget and MMO-Champion threads but `web.archive.org` is blocked from this environment. Engadget retrieved directly; MMO-Champion content cross-referenced via search excerpts only.

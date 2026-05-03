# Zul'Gurub — Cataclysm 5-man Survey

**Date:** 2026-05-02
**Patch:** 4.3.4 (encounter introduced 4.1)
**Audience:** bot-control engineer (cata-altbot)

---

## Overview

| Field | Value |
|---|---|
| Zone | Zul'Gurub (Stranglethorn Vale, southern peninsula) |
| Continent | Eastern Kingdoms |
| Level range (normal) | **N/A — heroic-only** |
| Level range (heroic) | 85 |
| Min ilvl required | 346 (LFD) |
| Group size | 5 (1 tank / 1 healer / 3 DPS) |
| Boss count | 5 required + 1 optional (Cache of Madness) |
| Layout | Outdoor zone with multiple gates; mostly ring-shaped path |
| Patch | Introduced **4.1** (April 2011) as the first 5-man heroic re-imagining of a vanilla raid (the original 20-man Hakkar raid) |
| Group buffs/debuffs | None unique to the instance |
| **Heroic-only** | YES — there is **no normal-mode version**. All entries scale at heroic difficulty (ilvl 346 gear gate) |

The instance is long for a 5-man — full clear runs ~45-60 min on launch-tier gear, ~25-35 min once the group is well-geared. The path is the original Vanilla ZG ring layout: enter through Gurubashi village → Venoxis temple (snake) → Mandokir's arena (raptor) → Kilnara's panther temple → Zanzil's voodoo grove → Jin'do's altar at the back. **Patch 4.3.0 added a skip-mechanic** allowing groups to face Jin'do after killing only 2 of the 4 mid-bosses (Venoxis, Mandokir, Kilnara, Zanzil), but Venoxis appears to remain unskippable as the gate-keeper.

---

## Sources

This survey reconciles the following community sources. Wowhead Cata is treated as authoritative for spell IDs and numeric values; strategy is cross-checked across 5 guides.

1. **Wowhead Cata — encounter & NPC pages** (`/cata/npc=...`, `/cata/spell=...`) — primary for spell IDs, cast times, schools, debuff durations
2. **Icy Veins — Zul'Gurub Cataclysm Classic Dungeon Guide** (`icy-veins.com/cataclysm-classic/zul-gurub-dungeon-guide`)
3. **Warcraft Wiki — High Priest Venoxis (tactics)** (`warcraft.wiki.gg/wiki/High_Priest_Venoxis_(tactics)`)
4. **Warcraft Wiki — Bloodlord Mandokir (tactics)** (`warcraft.wiki.gg/wiki/Bloodlord_Mandokir_(tactics)`)
5. **Warcraft Wiki — High Priestess Kilnara, Zanzil, Jin'do** (per-boss tactics pages on the same wiki)
6. **MMO-Champion Full Zul'Gurub Heroic 5-man Boss Guide** (forum thread; community-vetted)
7. **GuiasWoW — Zul'Gurub Heroic Guide** (`guiaswow.com/dungeons/guide-zulgurub-zul-gurub-heroic.html`)
8. **MMOExp — Zul'Gurub Cata dungeon guide** (cross-reference)
9. **Engadget WI 4.1 PTR Zul'Gurub running notes** (early-launch coverage; useful for original-launch behavior)

Where guides disagree, the disagreement is captured in the **Disagreements** section.

---

## Strong consensus (table)

| Mechanic | Consensus |
|---|---|
| Heroic-only, ilvl 346 gate | Confirmed across all sources — there is no normal mode |
| Boss order | **Venoxis → Mandokir → Kilnara → Zanzil → Jin'do**; mid-bosses can be done in flexible order, but Venoxis is the gate boss |
| 4.3.0 skip-rule | Group only needs 2 of 4 mid-bosses cleared to engage Jin'do; bot must handle "skip remaining mid-bosses" branch |
| Optional boss | **Cache of Madness** — random spawn (Hazza'rah / Renataki / Wushoolay / Gri'lek) requires Archaeology 225+ to unlock; bot defaults to **skip** |
| Venoxis Whispers of Hethiss | **MUST INTERRUPT every cast** — heavy nature DoT-channel on a player |
| Venoxis Toxic Link | Spreads two players to >25y or the link explodes; healer/group movement mechanic |
| Venoxis poison maze | Ground patches, navigated like a maze; Phase 1 hazard |
| Venoxis Phase 2 transition | At ~75% HP, transforms to snake form (loses dodge/parry, takes +50% damage); breath cone + acid pools |
| Venoxis Phase 3 | At ~25% HP, returns to altar, summons tendrils (Bloodvenom); ends with **Venom Withdrawal** stun → burst window |
| Mandokir Decapitate | **Instant-kill** on a non-tank random target; only counter is immunity (Iceblock, Bubble, Lichborne, Cloak of Shadows) |
| Mandokir Chained Spirits | 8 spirits in arena revive Decapitated players; revived player gains **Spirit's Vengeance** stacking buff (+25% dmg/healing/-10% dmg taken per stack) |
| Mandokir Ohgan | Raptor pet that hunts Chained Spirits; **kill priority absolute** when summoned/reanimated |
| Mandokir Reanimate Ohgan | Ohgan resurrects after death; group must repeatedly kill the raptor |
| Mandokir Devastating Slam | 2s-cast directional ground-furrow; dodge laterally |
| Mandokir Bloodletting | 50%-current-HP-per-tick DoT for 10s; healing through it is inefficient — drop the target low and burst-heal after |
| Mandokir Frenzy | At 20% HP, attack speed doubles; burst-down phase |
| Kilnara Phase 1 (troll form) | Tears of Blood (interruptible AoE) + Wave of Agony (cone, dodge) + Lash of Anguish (DoT) + Wail of Sorrow (ranged hit) |
| Kilnara dormant panthers | **16 sleeping panthers** (Pride of Bethekk) in the room before pull; if any offensive ability lands on them they wake up — must be cleared in controlled groups before pushing 50% |
| Kilnara Phase 2 (panther form) | At 50% HP, transforms to Avatar of Bethekk; gains haste, leaping bleed attacks |
| Zanzil cauldron mechanic | **3 environmental cauldrons** (Frostburn / Burning Blood / Toxic Torment); group clicks them in response to Zanzil's elixir colors |
| Zanzil elixir colors | Blue → Berserker add; Red → Zombie wave; Green → Graveyard Gas (raid-wide nature DoT) |
| Zanzil cauldron pairing | Blue→Frostburn (stuns berserker), Red→Burning Blood (AoE zombies), Green→Toxic Torment (90% nature reduction = survive gas) |
| Zanzil Voodoo Bolt | **Interruptible** shadow nuke; high priority kick |
| Zanzil Zanzili Fire | Linear arcane line on the ground; tank repositions boss |
| Jin'do Phase 1 | **Deadzones** on the ground — 90% magic dmg reduction inside / 90% cast speed reduction; group must be inside when Shadows of Hakkar lands, outside otherwise |
| Jin'do Shadows of Hakkar | 3s-cast chain-lightning-style shadow burst; 121-128k per hit (un-mitigated) |
| Jin'do Phase 2 transition | At 70% HP, **entire party teleported into Spirit Realm** (parallel zone) |
| Jin'do Spirit Realm | 3 chains binding Hakkar must be broken; chains protected by **Brittle Barrier** that can only be removed by a Gurubashi Spirit's **Body Slam** landing on the chain location |
| Jin'do Twisted Spirits | Soft-enrage adds spawning in increasing waves; weak (24k HP) but accumulate |
| Jin'do Shadow Spike | Player-targeted projectiles in spirit realm; dodge-from-feet mechanic |
| Jin'do return | After all 3 chains break, party returns to physical realm and burns Jin'do |
| Curse/Disease/Poison dispel | Limited need — Lash of Anguish (Kilnara) is dispellable Magic on some sources; otherwise no instance-wide dispel pressure |

---

## Boss 1 — High Priest Venoxis

NPC ID: **52155** (Wowhead-confirmed). First mandatory boss. Snake-temple platform encounter; the most movement-intensive fight in the instance and reportedly the highest skill-check.

### Phase structure

Three phases, transitions are HP-gated (not time-gated):

1. **Phase 1 — Venomous Labyrinth** (100 → ~75% HP): troll form on ground; poison maze + Whispers + Toxic Link
2. **Phase 2 — The Mortal Coil Unwinds** (~75 → ~25% HP): snake form; cone breath + acid pools; loses dodge/parry, takes +50% physical damage
3. **Phase 3 — Bloodvenom** (~25 → 0% HP): returns to altar, summons tendrils, ends in Venom Withdrawal stun (burst window)

Some sources report only 2 phases (combining Phase 2 and 3) — see **Disagreements #1**.

### Abilities

| Spell | ID | Cast / cooldown | School | Notes |
|---|---|---|---|---|
| Whispers of Hethiss | 96466 | channel, ~2s windup | Nature | **Interruptible.** Random non-tank target; 1850-2150 nature dmg every 0.3s for 8s. **MUST INTERRUPT EVERY CAST.** |
| Toxic Link | 96477 | scripted | Nature | Links two players; takes increasing damage as they approach each other. Break at >25y. **Player-coordination** mechanic; explodes for ~10000 dmg in 8y if broken in cluster. |
| Word of Hethiss | UNVERIFIED | scripted | Nature | Initial cast at pull; 46-53k nature dmg in 10y radius + knockback |
| Venomous Effusion (poison maze) | UNVERIFIED | passive | Nature | Ground patches that form a maze pattern. 9425-10575 dmg/tick if standing in. |
| Pool of Acrid Tears | UNVERIFIED | scripted | Nature | **Phase 2.** Spawns near furthest player; grows over time; 4712-5287 dmg every 0.5s if standing in. |
| Breath of Hethiss | UNVERIFIED | scripted cone | Nature | **Phase 2.** 15y cone; 15000 dmg every 0.5s for 3s; cone-dodge mechanic |
| Blessing of the Snake God | UNVERIFIED | passive aura | — | **Phase 2 buff on Venoxis.** +50% physical damage but loses dodge/parry → +50% incoming melee damage. |
| Bloodvenom | UNVERIFIED | scripted | Nature | **Phase 3.** Summons one tendril per alive player; tendril chases targets; AoE damage on contact |
| Venom Withdrawal | UNVERIFIED | scripted debuff on boss | — | **Phase 3.** Self-applied stun on Venoxis at low HP; +100% damage taken; **burn window** |
| Poison Cloud | UNVERIFIED | passive | Nature | **Phase 3** environmental; altar/stairs are unsafe — 9250-10750 dmg per 2s |

### Interrupts required

- **Whispers of Hethiss** (96466) — primary kick target. Bot interrupt priority: **CRITICAL — every cast on rotation**.

### Dispels required

None confirmed. Toxic Link is **not dispellable** — managed by spreading apart.

### Avoidable damage

- Venomous Effusion poison maze (Phase 1)
- Pool of Acrid Tears (Phase 2)
- Breath of Hethiss cone (Phase 2)
- Bloodvenom tendrils chase players (Phase 3) — kite around the platform
- Phase 3 altar floor poison

### Tank-swap requirements

None. Single tank.

### Heroic delta

Heroic is the only difficulty. All numbers above are heroic.

### Add-handling

- **Phase 3 tendrils**: one per alive player; kite on the platform. AoE rotation viable.

### Healer-pressure spikes

- Whispers of Hethiss DoT on whoever is targeted (high if not interrupted)
- Toxic Link explosion if break triggers near other players
- Phase 3 Bloodvenom tendril contact

### Key positioning

- **Phase 1**: spread to ranged-spread distance for Toxic Link safety; be ready to walk apart on link cast; navigate around poison maze patches
- **Phase 2**: tank pulls boss to clean ground; group avoids cone arc; **acid pools spawn at furthest player** — so ranged should not spread excessively (creates pool placement chaos)
- **Phase 3**: kite tendrils around the platform; stack burst CDs for Venom Withdrawal window

### Bot-relevant role notes

- **Tank**: pull boss to clean position when poison patches accumulate; rotate 90° when Phase 2 cone telegraphs; lead tendrils away in Phase 3
- **Healer**: heavy reactive heal on Whispers target; pre-shield Toxic Link partners; **save major CD for Phase 3** (Venom Withdrawal burst is also when boss melee ramps)
- **Melee**: stay behind boss for Phase 2 cone; Toxic Link spread is a positional check (melee bot must walk OUT of melee range if linked with another melee — significant DPS loss but mandatory)
- **Ranged**: maintain spread; walk in/out of poison patches as they cycle; **do NOT stand at max range during Phase 2** (you'll be the acid pool target; pull pool placement to the edge of the platform)

---

## Boss 2 — Bloodlord Mandokir

NPC ID: **52151** (Wowhead-confirmed). Second mandatory boss. Raptor-mounted troll on a small arena; the most read-the-room-quick encounter due to one-shot Decapitate + add-priority Ohgan.

### Phase structure

Single phase with a hard execute trigger:

1. **Regular phase** (100 → 20% HP): Decapitate cycle + Devastating Slam dodging + Bloodletting healing + Ohgan add management
2. **Frenzy phase** (20% → 0% HP): boss attack speed doubles; only Decapitate + melee, but Decapitate cycle is faster

### Abilities

| Spell | ID | Cast / cooldown | School | Notes |
|---|---|---|---|---|
| Decapitate | **96684** | instant; ~30-45s cd | Physical | **INSTANT KILL** on a random non-tank target. ~14.6-15.4M physical dmg. **Counter: immunity only** (Iceblock, Bubble, Lichborne, Cloak of Shadows, Anti-Magic Shell does NOT block physical). 4.3-era hotfix increased CD by 5s. |
| Level Up | **96662** | passive proc | — | Each player Mandokir kills (Decapitate or otherwise) grants +20% damage stack on him; up to 99 stacks. **Strict implication: do not let extra deaths happen.** |
| Devastating Slam | **96740** | 2s cast; ~10-15s cd | Physical | Leaps to a random spot, ground-furrow in 45° cone direction. 190-200k physical dmg; **dodge laterally**. |
| Bloodletting | **96776** | scripted DoT | Shadow | DoT on tank or random; drains 50% current HP per tick over 10s; **leeched** (boss heals for 50% of damage dealt; min 7500). **Healing during this is wasteful** — let HP drop, big heal after expiry. |
| Frenzy | **96800** | passive at 20% HP | — | Attack speed +100%; Decapitate CD shortened. Burn phase. |
| Reanimate Ohgan | **96724** | scripted | Nature | Mandokir resurrects Ohgan at full HP after the raptor dies. Repeatable. |
| Ohgan's Orders (summon Ohgan) | UNVERIFIED | scripted | — | Initial Ohgan spawn after first Decapitate (some sources say at pull) |
| Spirit's Vengeance (revived player buff) | UNVERIFIED | aura on revived player | — | +25% dmg/healing, -10% dmg taken; **stacks up to 10x** if player dies and is revived multiple times |
| Revive (Chained Spirit) | UNVERIFIED | scripted | — | Chained Spirit ghosts in arena revive Decapitated players. **8 spirits total**; finite resource. |

### Interrupts required

None — Mandokir has no interruptible casts. Devastating Slam has a 2s cast bar but is not interruptible (informal reports; **UNVERIFIED**).

### Dispels required

- Bloodletting (96776) is **Shadow**; dispel type: **Magic** per most reports, but **DO NOT DISPEL** in standard strat — it's intended healing-trap mechanic.

### Avoidable damage

- **Decapitate** — only avoidable via immunity ability (insta-cast); player can pre-pop on cast telegraph
- **Devastating Slam** — strafe out of the arc on cast bar
- Ohgan attacks on Chained Spirits — must kill Ohgan first

### Tank-swap requirements

None.

### Heroic delta

Heroic-only fight.

### Add-handling

- **Ohgan**: kill priority **ABSOLUTE**. Damages and consumes Chained Spirits (which the group needs alive to revive Decapitated players). If Ohgan kills all 8 spirits, the next Decapitate is unrecoverable.
- **Reanimate Ohgan**: every time Ohgan dies, Mandokir revives him. Cycle repeats throughout the fight. **DPS swap pattern**: Ohgan up → swap to raptor → kill → Mandokir up → swap back to boss until Ohgan revives.
- **Chained Spirits** (8 total): pre-existing in arena; not directly killable by players; **passive friendly NPCs** that consume to revive a player.

### Healer-pressure spikes

- **Decapitate target dies instantly** — healer can't react; player must self-immune. Healer focus is on whoever was just revived (Spirit's Vengeance + low HP).
- **Bloodletting target's HP drops to ~0** intentionally; healer must NOT panic-heal during, but **must** heal after expiry.
- Frenzy phase melee on tank.

### Key positioning

- **Tank**: hold Mandokir near center of arena; do not move into Chained Spirit clusters (they'll be cleaved)
- **Group**: spread for Decapitate target randomization (no cleave); melee stays in melee
- **Devastating Slam**: dodge laterally on cast

### Bot-relevant role notes

- **Tank**: hold threat; pop major defensive at Frenzy phase
- **Healer**: pre-shield Decapitate target if predictable; do NOT spam-heal Bloodletting target; reactive heal post-expiry
- **Melee**: side-positioned for Devastating Slam; **Ohgan kill swap** on raptor spawn / reanimate event — drop Mandokir DPS, full focus on raptor
- **Ranged**: same Ohgan-swap behavior; some classes (Frost Mage, Hunter) excel at the rapid target swap

### bot_implementation flag — Decapitate immunity timing

- Decapitate fires on a CD of ~30-45s. Bot must use class immunity (Iceblock, Divine Shield/Hand of Protection, Cloak of Shadows, Lichborne) on impending cast.
- **Telegraph signal**: Mandokir leaps + voice line ("YOU ARE NEXT!" — UNVERIFIED game text). Implementation needs scripted-event detection, not cast bar (it's instant).
- **Ohganot So Fast! achievement**: kill Mandokir without ever killing Ohgan. Bot ignores this; default is kill Ohgan on every spawn.

---

## Boss 3 — High Priestess Kilnara

NPC ID: **52059** (per prompt context; Wowhead-confirmed via 4.1 patch notes). Third mandatory boss; panther temple.

### Phase structure

Two phases, HP-gated:

1. **Phase 1 — Troll Form** (100 → 50% HP): standard caster boss with interruptible AoE + cone + dodge
2. **Phase 2 — Panther Form (Avatar of Bethekk)** (50 → 0% HP): physical melee form; haste; bleed-leap attacks

**Critical pre-pull**: 16 sleeping **Pride of Bethekk** panthers in the room. Any offensive ability landing on a panther wakes it. Group must clear panthers in controlled batches before pushing Kilnara to 50%, or panthers wake en masse during Phase 2.

### Abilities

| Spell | ID | Cast / cooldown | School | Notes |
|---|---|---|---|---|
| Tears of Blood | **96435** | channel cast | Shadow | **Interruptible.** Streams blood from Kilnara's eyes; AoE shadow dmg in 12y around her (8500-11500). **HIGH PRIORITY KICK.** |
| Shadow Bolt | **96956** | ~2s cast | Shadow | **Interruptible.** Single-target nuke; 11050-14950 shadow dmg. Secondary kick target. |
| Wave of Agony | **96457** | scripted cone | Shadow | **Not interruptible.** Knock-back wave in front; **dodge laterally**. |
| Wail of Sorrow | **96948** | ranged cast | Shadow | 40y ranged hit; 18500-21500 dmg; targets random player |
| Lash of Anguish | **96958** | applied debuff | Shadow | Initial dmg + DoT (18500-21500/sec for 10s). **Possibly dispellable Magic** — see **Disagreements #4** |
| Avatar of Bethekk (Phase 2 form) | UNVERIFIED | passive | — | Transforms at 50% HP; gains haste + leap attacks |
| Gaping Wound | UNVERIFIED | melee leap | Physical | **Phase 2.** Leap attack causing bleed (9250-10750 / 2s). Applied by Phase 2 Kilnara AND awakened Pride of Bethekk panthers. |
| Pride of Bethekk (panthers) | UNVERIFIED | passive | — | 16 dormant panthers; wake on any offensive cast on them OR if Kilnara enters Phase 2 with them still asleep |

### Interrupts required

- **Tears of Blood** (96435) — **CRITICAL** kick (group AoE)
- **Shadow Bolt** (96956) — secondary; kick if rotation has slack

### Dispels required

- **Lash of Anguish** (96958) — **MAYBE dispellable Magic**; community sources disagree (see **Disagreements #4**). Default: try-to-dispel on heroic.

### Avoidable damage

- Wave of Agony cone (Phase 1)
- Phase 2 leap targeting (move when telegraphed)
- Awakened panthers' bleeds compound; controlled pulls before push to 50% prevent this

### Tank-swap requirements

None.

### Heroic delta

Heroic-only fight.

### Add-handling — KEY MECHANIC

- **Pride of Bethekk** — 16 panthers in the room
- **DO NOT push 50%** until all panthers cleared
- **Pull pattern**: tank grabs 2-4 panthers at a time; AoE/CC them; rotate cooldowns
- **Soft alternative**: pull whole pack with cooldowns (heroic-geared groups)
- **Strat**: clear panthers FIRST (pre-pull or interleaved), THEN engage Kilnara; OR engage Kilnara, hold above 50%, clear panthers, then push

### Healer-pressure spikes

- Tears of Blood unkicked = AoE dmg on whole group
- Lash of Anguish DoT on player (heavy)
- Phase 2 leap chain bleeds

### Key positioning

- Tank pulls Kilnara to a cleared zone (panther-free)
- Ranged at max range; Wave of Agony cone arc-aware
- Panthers AoE'd from clusters; tank stacks them

### Bot-relevant role notes

- **Tank**: cluster panthers for AoE; hold threat on Kilnara + adds during multi-pull
- **Healer**: heavy AoE during Tears of Blood (if missed); Lash of Anguish dispel if class has Magic dispel
- **Melee**: AoE rotation during panther clears; cone-dodge during boss phase
- **Ranged**: AoE/cleave on panther clusters; **single-target focus** on Kilnara during Phase 2; stand at >12y to dodge Tears of Blood radius

### bot_implementation flag — sleeping-panther target filter

- Sleeping panthers are **friendly-tagged but flip hostile** on first damage. Bot's auto-target / cleave-target filter must NOT include sleeping panthers in cleave range during cooldown-DPS windows; otherwise group accidentally aggros entire room.

---

## Boss 4 — Zanzil

NPC ID: **52053** (per prompt context; community-sourced). Fourth mandatory boss; voodoo grove. The "drink the right potion / use the right cauldron" puzzle fight.

### Phase structure

Single phase with a recurring elixir-color cycle:

1. Regular DPS on Zanzil
2. Zanzil periodically uses an **elixir** (Blue / Red / Green) — each summons a different threat
3. Group **clicks the matching cauldron** to acquire a counter-buff
4. Resolve the threat; resume DPS

### Abilities

| Spell | ID | Cast / cooldown | School | Notes |
|---|---|---|---|---|
| Voodoo Bolt | **96347** | ~2s cast | Shadow | **Interruptible.** Random target; 33250-36750 shadow dmg. **HIGH-PRIORITY KICK.** |
| Zanzili Fire | **96914** | ground line cast | Arcane | Linear arcane attack on the ground; 66000 initial + 50000/s for 5s. Tank repositions boss out of line. |
| Terrible Tonic | **96348** | ranged toss | Nature | Random player target; 30000 nature dmg |
| Resurrection Elixir (Blue) | UNVERIFIED | scripted | — | Summons **Zanzili Berserker** that fixates a player (no threat table). Counter: **Frostburn cauldron** (next melee = stun + 65% max HP dmg) |
| Resurrection Elixir (Red) | UNVERIFIED | scripted | — | Summons multiple **Zanzili Zombies**. Counter: **Burning Blood cauldron** (AoE fire dmg around drinker) |
| Zanzil's Graveyard Gas (Green) | UNVERIFIED | environmental | Nature | Fills arena with gas (5-10% max HP / sec nature dmg). Counter: **Toxic Torment cauldron** (-90% nature damage) — **all 5 players must drink** |
| Zanzili Berserker — Thunderclap | UNVERIFIED | melee | Nature | Berserker AoE; 12750-17250 nature dmg |
| Zanzili Berserker — Knock Away | UNVERIFIED | melee | Physical | Berserker stun/knockback |

### Interrupts required

- **Voodoo Bolt** (96347) — primary kick target

### Dispels required

None — debuffs are environmental / cauldron-managed, not dispels.

### Avoidable damage

- Zanzili Fire line on the ground
- Berserker Thunderclap (range out)
- Graveyard Gas (must drink Toxic Torment cauldron — not avoidable by movement)

### Tank-swap requirements

None.

### Heroic delta

Heroic-only fight.

### Add-handling

- **Zanzili Berserker** (Blue elixir): single melee add; fixates a player (off-threat). Counter with Frostburn buff → next melee stuns it for 15s and deals 65% max HP. Burst it down.
- **Zanzili Zombies** (Red elixir): multiple weaker melee adds. Counter with Burning Blood AoE buff; tank-cluster + AoE.

### Healer-pressure spikes

- Voodoo Bolt on a player if unkicked
- Graveyard Gas tick on whole group (mitigated by Toxic Torment)
- Berserker fixate damage on the fixated player
- Zanzili Fire if standing in the line

### Key positioning

- Tank holds Zanzil **between two cauldrons** so reaches are short
- Cauldrons are in fixed positions in the arena
- Group must learn cauldron locations and elixir-color → cauldron mapping

### Cauldron mechanic — bot_implementation FLAG

- **Cauldrons are environmental clickable objects**, not spells. The bot framework must support **GameObject interaction**:
  - `Frostburn Formula` → stun-buff on next melee
  - `Burning Blood` → AoE fire damage to nearby enemies (drinker is melee-cleave anchor)
  - `Toxic Torment` → -90% nature damage taken (entire group drinks)
- Bot needs a registered "interactable encounter object" entry per cauldron with a context-aware "should drink" predicate driven by Zanzil's elixir-cast event
- **Without this, Zanzil is mechanically un-completable by autonomous bots** — flagged as a Phase-3+ implementation prerequisite

### Bot-relevant role notes

- **Tank**: drink **Burning Blood** on Red elixir for AoE clear of zombies; drink **Toxic Torment** on Green; **NOT Frostburn** (Frostburn deals 65% max HP self-damage on bosses → **never** drink near Zanzil)
- **Healer**: drink **Toxic Torment** on Green; otherwise minimize cauldron use, focus heals
- **Melee**: drink Frostburn for Berserker burst; drink Burning Blood for Zombies; drink Toxic Torment on Green
- **Ranged**: drink Toxic Torment on Green; less role in Frostburn/Burning Blood (those favor melee proximity)

---

## Boss 5 — Jin'do the Godbreaker

NPC ID: **52148** (per prompt context). Final mandatory boss. The most mechanic-heavy fight in the instance — features a **parallel-zone teleport** in Phase 2 (the Spirit Realm) which is a critical bot implementation gap.

### Phase structure

Two phases:

1. **Phase 1 — Welcome to the Great Show!** (100 → 70% HP): Jin'do solo on his platform; Deadzone management + Shadows of Hakkar mitigation
2. **Phase 2 — Spitting in the Face of a God** (70 → 0% HP): party teleported to **Spirit Realm**; break 3 chains binding Hakkar; return to physical realm to finish

### Phase 1 abilities

| Spell | ID | Cast / cooldown | School | Notes |
|---|---|---|---|---|
| Deadzone | **97170** | scripted ground-place | Shadow | **Persistent ground zone**: -90% magic damage taken / -90% cast speed inside. **Tactical use**: stand inside DURING Shadows of Hakkar; OUT otherwise (to avoid cast speed penalty) |
| Shadows of Hakkar | **97172**, 97173 | 3s cast | Shadow | Chain-burst shadow attack; up to 10 targets; 121-128k shadow dmg per hit. **Counter: stand in Deadzone during cast** (90% reduction makes it survivable) |
| (Phase 1 melee) | — | melee | Physical | Standard tank melee |

### Phase 2 abilities (Spirit Realm)

| Spell | ID | Cast / cooldown | School | Notes |
|---|---|---|---|---|
| Shadow Spike | UNVERIFIED | random ground-targeted | Shadow | Player-targeted projectile; explodes on impact for 63-77k in 4y radius. **Dodge from feet** mechanic |
| Body Slam (Gurubashi Spirit) | UNVERIFIED | leap | Physical | Spirit jumps to random player; deals damage + knockback + applies **Sunder Rift** debuff (+100% damage taken for 20s) |
| Sunder Rift (debuff) | UNVERIFIED | aura | — | +100% damage taken on debuffed target; **breaks Brittle Barrier on chains** if Body Slam hits the chain location |
| Brittle Barrier (chain shield) | UNVERIFIED | passive aura on chain | Arcane | Each of the 3 Hakkar's Chains has this barrier; **only removable by Sunder Rift / Body Slam landing on chain location** |
| Hakkar's Chains (3 chains) | UNVERIFIED | objective | — | Attackable when Brittle Barrier is removed; 3 chains total |
| Frenzy (Gurubashi Spirit) | UNVERIFIED | passive | — | Stacking attack-speed buff on spirits; soft-enrage |
| Twisted Spirit (adds) | UNVERIFIED | passive | — | Weak adds (24k HP); spawn in increasing waves; soft-enrage |

### Interrupts required

- **Shadows of Hakkar** is a 3s cast — but the strat is **mitigate via Deadzone**, not interrupt. Reportedly **non-interruptible**.
- Twisted Spirits and Gurubashi Spirits have melee abilities, no interruptible casts

### Dispels required

None confirmed.

### Avoidable damage

- Outside Deadzone during Shadows of Hakkar = death
- Inside Deadzone outside Shadows of Hakkar = -90% cast speed (DPS / heal loss)
- Shadow Spike ground markers (Phase 2)

### Tank-swap requirements

None.

### Heroic delta

Heroic-only fight.

### Add-handling

- **Phase 2 Gurubashi Spirits**: tank picks up; positioned **near a chain** so Body Slam lands on chain location
- **Phase 2 Twisted Spirits**: AoE-cleave; soft enrage if not killed
- **Pre-fight**: 2 Gurubashi Spirit Warriors stand on Jin'do's stairs — kill before pull (some sources)

### Healer-pressure spikes

- Shadows of Hakkar tick if not in Deadzone
- Phase 2 sustained shadow + body slam pressure
- Twisted Spirits cleave damage if not killed

### Key positioning

**Phase 1**: tank pulls Jin'do near (but NOT on) where Deadzones spawn. On Shadows of Hakkar cast, group steps **into** the active Deadzone. After cast, group steps **out** to avoid cast-speed penalty.

**Phase 2**: party stands on chain locations; tank brings Gurubashi Spirit close so its Body Slam lands on the chain. Sunder Rift then exposes the chain. Repeat for all 3 chains.

### Bot-relevant role notes — IMPLEMENTATION FLAGS

- **Phase 2 Spirit Realm is a parallel zone teleport** (similar to Old Gods phase mechanics in Ulduar / Dragon Soul). The bot session may not handle zone-state changes mid-combat correctly — see implementation flags.
- **Tank**: complex positioning decisions per phase
- **Healer**: position to be IN Deadzone during Shadows; sustained healing in Phase 2 spirits
- **Melee**: chain DPS in Phase 2; AoE on Twisted Spirits
- **Ranged**: same; ranged advantage for Shadow Spike dodge

### bot_implementation flag — Spirit Realm phase

- **The party is teleported to a parallel zone instance during Phase 2**. This is unlike normal phase transitions — it changes the player's `m_mapId` / phase ID.
- Bot implementation **must handle**:
  - Detect zone/phase transition mid-encounter
  - Maintain group reference (party leader, tank target, etc.) across the transition
  - Re-establish target acquisition on the other side
  - Detect return-to-physical realm transition
- **Without this, Jin'do Phase 2 is un-completable autonomously** — flagged as P3+ implementation prerequisite

### bot_implementation flag — Deadzone in/out timing

- Deadzone is a **ground-AoE** zone. Bot must distinguish "stand in" from "stand out" based on Shadows of Hakkar cast bar:
  - **Cast START** (3s warning) → group **moves IN**
  - **Cast END** (resolves) → group **moves OUT**
- This is unusual — normally ground AoE = avoid. Encounter-specific override required.

---

## Optional Boss — Cache of Madness (Hazza'rah / Renataki / Wushoolay / Gri'lek)

NPC IDs: VARIABLE (one of four spawns randomly). Optional encounter — chest in the instance is unlocked by **Archaeology skill 225+**.

### Default behavior

**SKIP.** Bot does not have Archaeology levelled, and the encounter offers no gating value.

### Brief mechanic notes (for completeness)

- **Hazza'rah**: Nightmare Illusions — sleep + illusion adds at 66/33%
- **Renataki**: Vanish + Thousand Blades; deadly poison stacks; 30% Frenzy
- **Wushoolay**: Forked Lightning chain; Lightning Cloud zones; spread mechanic
- **Gri'lek**: Rupture Line ground effect; Avatar buff (kite); Pursuit + Entangling Roots

### Bot-relevant role notes

- Default **skip**. If group leader explicitly directs engagement, bot uses generic encounter heuristics (kite-from-fixate, dispel-roots if class has it, AoE on adds).

---

## Trash mechanics

### Pre-Venoxis (entry → Venoxis temple)

- **Gurubashi Headhunters / Berserkers**: melee adds; standard pulls
- **Voodoo Acolytes** (caster trash): Shadow Bolt hardcasts; **interrupt**; CC (Polymorph, Hex, Fear) valid
- **Razzashi Cobras**: poison-melee adds; tank stacks for AoE

### Mandokir's arena approach

- **Bloodlord Adds / Razzashi Raptors**: pack pulls; tank stacks; AoE-burn
- **Razzashi Skitterer** (rare): caster with poison; interrupt

### Kilnara temple approach

- **Bethekk Acolytes**: Shadow casters; **CC + interrupt**
- **Pride of Bethekk panthers** in the boss room (covered in Boss 3)

### Zanzil grove

- **Spellcasters group (first pack)**: tank uses **Fire (Burning Blood)** cauldron buff for AoE
- **Two Rabid Gurubashi (second pack)**: no threat table (fixate); use **Frost (Frostburn)** cauldron to stun-burst
- **Zanzili guards**: standard melee; tank-and-spank

### Jin'do altar approach

- **2 Gurubashi Spirit Warriors** on the stairs leading up — pre-fight kill (some strats)
- **Hakkari Bloodkeeper** patrols (rare) — interruptible casts

### CC priorities

- Polymorph / Hex / Sap / Fear valid on troll humanoids (Gurubashi, Hakkari, Zanzili, Bethekk casters)
- Raptors / panthers / cobras / berserkers are NOT standard CC targets
- Banish NOT applicable (no demons in instance)

### Skip-able trash

- Some packs in Mandokir's arena and Zanzil's grove can be wall-hugged on speed-clear
- **Cache of Madness** room is the optional branch

### Pull markers for CC

- Skull on lead caster, X/cross on second CC target
- Tank pulls humanoid + raptor pack with cooldowns (raptors AoE-cleave casters)

---

## Disagreements

### Disagreement #1 — Venoxis phase count (2 vs 3)

- **Source A (Warcraft Wiki tactics page):** 3 distinct phases — labyrinth, snake transformation, altar/Bloodvenom finale
- **Source B (Icy Veins / GuiasWoW):** 3 phases at 100/75/25
- **Source C (some MMO-Champion threads):** 2 phases — combine snake form and altar phase
- **Source D (MMOExp):** simply describes "transforms via Blessing of the Snake God"

**Default**: 3 phases per the Wiki tactical breakdown. Bot phase-detection uses **HP thresholds (75% and 25%)** as primary triggers.

### Disagreement #2 — Mandokir Decapitate cooldown / cast count

- **Source A (Warcraft Wiki, post-4.3 hotfix note):** cd increased by 5s in 2011-12-20 hotfix
- **Source B (older Icy Veins / launch notes):** original cd ~30s
- **Source C (community):** cd in 4.3.4 era is ~30-45s

**Default**: assume ~30-45s window between casts. Bot uses **scripted-event detection** (the leap animation / chat message), not a CD timer, since exact cadence varies.

### Disagreement #3 — Mandokir Ohgan kill priority vs achievement strat

- **Source A (most guides):** kill Ohgan on every reanimate cycle; absolute priority
- **Source B (achievement guide for Ohganot So Fast!):** never kill Ohgan; tank-and-spank Mandokir despite escalating Decapitates

**Default**: kill Ohgan on every cycle. Bot does NOT pursue Ohganot So Fast! achievement (requires multi-revive coordination beyond bot scope).

### Disagreement #4 — Kilnara Lash of Anguish dispellability

- **Source A (Wowhead spell page — UNVERIFIED):** Magic dispel
- **Source B (community guides):** healers don't dispel; heal through it
- **Source C (some guides):** dispel only if damage spikes

**Default**: **dispel if class has Magic dispel and target is below 50% HP**, otherwise heal through. Conservative.

### Disagreement #5 — Kilnara panther wake mechanic — "pull-some" vs "pull-all"

- **Source A (Icy Veins / GuiasWoW):** pull 2-4 panthers at a time, AoE-burn each cluster
- **Source B (heroic-geared groups):** pull all 16 with cooldowns; chain-AoE
- **Source C:** clear panthers BEFORE engaging Kilnara; some guides clear during Phase 1 hold-pattern

**Default**: pull 2-4 at a time pre-engage. Bot tank uses cluster-grab; AoE rotation for melee/ranged. Save big AoE CDs for the final 1-2 clusters.

### Disagreement #6 — Zanzil Frostburn safety

- **Source A:** Frostburn deals 65% target max HP — **never drink near Zanzil** (one-shots if it crits boss)
- **Source B:** Frostburn explicitly **does not work on bosses**, only on adds — safe to drink
- **Source C:** ambiguous — "use only on Berserker"

**Default**: drink Frostburn ONLY when Berserker is active and tank/melee will land the next strike on the Berserker (not Zanzil).

### Disagreement #7 — Jin'do Phase 2 chain count

- **Source A (most guides):** 3 chains
- **Source B (some early sources):** 4 chains
- **Source C:** 3 + Hakkar visible at center

**Default**: 3 chains. Bot Phase 2 logic uses chain-count = 3 for completion check.

### Disagreement #8 — Jin'do Spirit Realm Twisted Spirit handling

- **Source A:** Cleave AoE on Twisted Spirits as they spawn
- **Source B:** Ignore early waves; focus chains; AoE only when wave count overwhelms
- **Source C:** Specific achievement (Spirit Twister) requires killing 20 within 15s — group-level coordination

**Default**: AoE on every wave; bot does NOT pursue Spirit Twister achievement (timing is tight).

### Disagreement #9 — 4.3 skip-rule mechanic

- **Source A (Warcraft Wiki):** post-4.3.0, only 2 of 4 mid-bosses needed for Jin'do
- **Source B (community):** Venoxis is gate; the other 3 are optional
- **Source C (older sources):** all 4 required

**Default**: post-4.3.4 rule applies — Venoxis required, then **2 of 3 remaining mid-bosses** (Mandokir / Kilnara / Zanzil) before Jin'do. Bot defaults to clear all 5 (full clear) unless group leader explicitly opts skip.

### Disagreement #10 — Cache of Madness as standard content

- **Source A:** Optional, requires Archaeology
- **Source B:** Sometimes listed as 6th boss in counts
- **Source C:** Bonus encounter, separate loot table

**Default**: **OPTIONAL** — bot defaults skip, no Archaeology dependency.

---

## DBC verification (UNVERIFIED items)

Spell IDs and stack values listed below are from community-guide cross-reference, not direct DBC lookup. Add to `docs/research/dbc-verification-checklist.md` for resolution when the TC fork session opens.

| # | Spell / mechanic | Listed value | Why it matters for bot logic |
|---|---|---|---|
| 1 | Venoxis Whispers of Hethiss | 96466 | Primary interrupt rotation target |
| 2 | Venoxis Toxic Link | 96477 | Spread-distance trigger |
| 3 | Venoxis Word of Hethiss (pull cast) | UNVERIFIED | Pull-survival check |
| 4 | Venoxis Pool of Acrid Tears | UNVERIFIED | Phase 2 ground hazard detection |
| 5 | Venoxis Breath of Hethiss | UNVERIFIED | Phase 2 cone-dodge trigger |
| 6 | Venoxis Bloodvenom (Phase 3) | UNVERIFIED | Tendril spawn detection |
| 7 | Venoxis Venom Withdrawal | UNVERIFIED | **Burst-window trigger** (CDs stacked here) |
| 8 | Mandokir Decapitate | 96684 | Immunity-pop trigger |
| 9 | Mandokir Level Up | 96662 | Stack-count tracking (not actionable but useful) |
| 10 | Mandokir Devastating Slam | 96740 | Cone-dodge trigger |
| 11 | Mandokir Bloodletting | 96776 | **Heal-suppress trigger** (do not panic-heal during) |
| 12 | Mandokir Frenzy | 96800 | 20%-HP burn-phase signal |
| 13 | Mandokir Reanimate Ohgan | 96724 | Ohgan-respawn detection |
| 14 | Mandokir Spirit's Vengeance (revive buff) | UNVERIFIED | Revived-player tracking |
| 15 | Kilnara Tears of Blood | 96435 | **Primary kick** (group AoE) |
| 16 | Kilnara Shadow Bolt | 96956 | Secondary kick |
| 17 | Kilnara Wave of Agony | 96457 | Cone-dodge trigger |
| 18 | Kilnara Wail of Sorrow | 96948 | Reactive heal trigger |
| 19 | Kilnara Lash of Anguish | 96958 | Magic-dispel maybe (Disagreement #4) |
| 20 | Kilnara Phase 2 Avatar of Bethekk | UNVERIFIED | Phase transition detection |
| 21 | Kilnara Pride of Bethekk panther IDs | UNVERIFIED (NPC 52345 area) | Sleeping-panther target filter |
| 22 | Zanzil Voodoo Bolt | 96347 | **Primary kick** |
| 23 | Zanzil Zanzili Fire | 96914 | Tank-reposition trigger |
| 24 | Zanzil Terrible Tonic | 96348 | Reactive-heal trigger |
| 25 | Zanzil Resurrection Elixir Blue/Red/Green | UNVERIFIED | Cauldron-decision trigger |
| 26 | Zanzil Graveyard Gas | UNVERIFIED | All-drink-Toxic-Torment trigger |
| 27 | Zanzil cauldron object IDs (Frostburn / Burning Blood / Toxic Torment) | UNVERIFIED | **Cauldron interaction** (bot_implementation flag) |
| 28 | Jin'do Deadzone | 97170 | **In/out timing trigger** |
| 29 | Jin'do Shadows of Hakkar | 97172, 97173 | Cast-start = move into Deadzone |
| 30 | Jin'do Shadow Spike (Phase 2) | UNVERIFIED | Ground-marker dodge |
| 31 | Jin'do Body Slam (Gurubashi Spirit) | UNVERIFIED | Chain-positioning trigger |
| 32 | Jin'do Brittle Barrier (chain shield) | UNVERIFIED | Chain-attackable detection |
| 33 | Jin'do Sunder Rift debuff | UNVERIFIED | Damage-amplification + barrier-break trigger |
| 34 | Jin'do Twisted Spirit | UNVERIFIED (NPC) | Add-spawn detection |
| 35 | Jin'do Phase 2 zone/phase ID | UNVERIFIED | **Spirit Realm transition detection** (bot_implementation flag) |

---

## Notes on prompt deviations

- **Heroic-only**: Zul'Gurub has no normal-mode equivalent. The "Heroic delta" sections per boss are kept for structural consistency with the BRC template but contain only "heroic-only fight" boilerplate. No normal-mode boss numbers exist.
- **Cache of Madness** is included as an optional boss (matching Beauty's role in BRC) but with even less depth — bot defaults to skip and the encounter has 4 randomized variants, so per-variant strategy is cursory.
- **bot_implementation flags** are unusually heavy for this dungeon — three full mechanics require framework features that may not exist:
  - Mandokir Decapitate immunity-trigger (scripted event, not cast-bar)
  - Zanzil cauldron interaction (GameObject click)
  - Jin'do Phase 2 Spirit Realm parallel-zone teleport
- These are flagged inline AND aggregated in the per-boss `bot_implementation flag` callouts.
- **Spell IDs are heavily UNVERIFIED** for Phase 2 Jin'do, Phase 3 Venoxis tendrils, and all 4 Cache-of-Madness variants. Recommend prioritizing #1, #8, #15, #22, #28 (the kick/dodge/scripted-event triggers) for DBC pass.
- **Wowhead Cata is the spell-ID source of record**; community-guide IDs (forums, MMO-Champion) cross-checked when Wowhead pages 404'd.
- **No instance-wide buff/debuff** — unlike some Cata 5-mans (e.g., Halls of Origination's heat aura), Zul'Gurub has no instance-level aura to model.
- Where Wowhead Cata pages returned 404 (`npc=52051/zanzil` did not resolve to a Cata-version page), community sources backstopped the IDs and they were marked UNVERIFIED.

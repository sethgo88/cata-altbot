# Hour of Twilight — Cataclysm 5-man Survey

**Date:** 2026-05-02
**Patch:** 4.3.4
**Audience:** bot-control engineer (cata-altbot)

---

## Overview

| Field | Value |
|---|---|
| Zone | Hour of Twilight (Caverns of Time, Tanaris) |
| Continent | Kalimdor (instance segments span Dragonblight → Wyrmrest Temple via the Bronze Dragonshrine timeline) |
| Level range (normal) | **N/A — heroic only** |
| Level range (heroic) | 85 |
| Minimum item level (heroic) | 353 (LFD gate) |
| Group size | 5 (1 tank / 1 healer / 3 DPS) |
| Boss count | 3 (no optionals) |
| Layout | Linear; Thrall NPC accompanies the party for the entire instance |
| Cata patch | 4.3.0 — one of three "Hour of Twilight" 5-mans (End Time, Hour of Twilight, Well of Eternity) gating Dragon Soul |
| Gating | Requires completion of Well of Eternity to unlock (story-locked) |
| Group buffs/debuffs | **Thrall** as instance-wide escort NPC; provides **Rising Fire Totem** (damage + health buff) and **Water Shell** (damage-reduction shield + 100% damage buff during waves) at scripted points |

The instance is short (~15-25 min on heroic for a geared group). It is the second of the three 4.3 troll-replacement heroics chronologically (Wowhead/community ordering: End Time → Hour of Twilight → Well of Eternity), and one of the three pre-Dragon-Soul gated heroics. It exists **only at heroic difficulty**; the LFD queue offers no normal-mode entry. Trash is sparse; the path is "Crystal Vice (Arcurion) → Galakrond's Rest (Asira) → Path of the Titans / Maw of Destruction → Wyrmrest Temple roof (Benedictus)." The defining structural feature is the **Thrall escort**: he walks/rides with the party from start to finish, fights alongside on every boss, and **if he dies, the instance fails (must be reset)**. Hotfixes after launch made him unable to despawn or be killed by trash, but the death-fail rule still applies on bosses.

---

## Sources

This survey reconciles the following community sources. Wowhead Cata is treated as authoritative for spell IDs and numeric values; strategy is cross-checked across 5+ guides.

1. **Wowhead Cata — encounter & NPC pages** (`/cata/npc=...`, `/cata/spell=...`) — primary for spell IDs, cast times, schools, debuff durations
2. **Icy Veins — Hour of Twilight Dungeon Guide** (`icy-veins.com/cataclysm-classic/hour-of-twilight-dungeon-guide`)
3. **Warcraft Tavern — Hour of Twilight Dungeon Guide** (`warcrafttavern.com/cataclysm/guides/hour-of-twilight-dungeon-guide/`) — strategic overview, role notes
4. **Warcraft Wiki / Wowpedia — boss tactics pages** (`warcraft.wiki.gg/wiki/Archbishop_Benedictus_(tactics)`, `wowpedia.fandom.com/wiki/Hour_of_Twilight`) — phase mechanics, ability tables
5. **WoW Insider 4.3 Guide** (engadget archive) — original-launch strategy depth
6. **Bag Overflow / Power Up Online / Ten Ton Hammer 4.3 dungeon guides** — original-Cata-era strategy with role-specific notes
7. **Epic Carry / LFCarry Cata Classic guides** — Cata-Classic-era refresh on item-level + queue requirements
8. **Wowhead Cata Classic dungeon overview guide** (`wowhead.com/cata/guide/dungeons/hour-of-twilight-overview`)

Where guides disagree, the disagreement is captured in the **Disagreements** section and the consensus position is listed in **Strong consensus**.

---

## Strong consensus (table)

| Mechanic | Consensus |
|---|---|
| Difficulty mode | **Heroic only**; no normal-mode entry exists. LFD requires ilvl 353. |
| Thrall escort | Thrall walks/runs with the party throughout the instance; fights all 3 bosses; **his death = instance fail** (must reset). Group must defend him against scripted attacks. |
| Thrall combat support | Thrall is **not** mechanically passive — he assists each boss with a unique support pattern (Bloodlust on Arcurion, Rising Fire Totem on Asira, Water Shell + Chain Lightning on Benedictus phase 1). |
| Arcurion Hand of Frost | **Interruptible** tank-targeted nuke; high interrupt priority. Some sources call this a "frontal cone" but Wowhead lists it as a single-target Frost-school nuke (30y range). The **prompt's "frontal cone of ice" maps to this ability** but Wowhead spell data shows single-target — bot treats as single-target nuke but tank still positions away from group out of caution. |
| Arcurion Icy Tomb on Thrall | **Critical mechanic** — Thrall is encased; party MUST DPS the tomb to free him. If unbroken too long, Thrall takes lethal damage → fail. Bot DPS must target the tomb, not the boss, when Tomb spawns. |
| Arcurion ground patches / Frozen Servitor adds | **NPC 54600** (Frozen Servitor) on canyon ledges throw **Icy Boulders** with telegraphed ground reticles (~20k damage); avoid by movement. Servitors are **not killable** in standard strat — they are environmental hazards. |
| Arcurion Torrent of Frost (30%) | At 30% boss HP, channels persistent group-wide Frost AoE. Heavy heal-throughput phase; **Thrall casts Bloodlust** at this point so DPS burn the boss before the channel kills the group. The prompt's **"Glaciate frost ring AoE"** is most likely this Torrent of Frost (or the immobilizing wave **Chains of Frost** which is Magic-school and dispellable with Mass Dispel) — see **Disagreements #1**. |
| Asira Mark of Silence + Throw Knife | Mark targets random caster (silenceable spec); when marked target casts, Asira hurls a knife. **Knife travels in a line and damages the FIRST player it hits**. Standard play: a non-marked player (tank or melee) **body-blocks** the knife by standing between Asira and the marked player. |
| Asira Choking Smoke Bomb | Ground-place 8-10y radius nature-damage cloud; **prevents target acquisition into/out of the cloud** (effective stealth/LoS-block for whoever stands in it). The boss frequently drops bombs at her feet, then the tank drags her out. **Bomb is the closest thing in this fight to a "stealth phase"** — the prompt's "Smoke Bomb (group disable + Stealth phases)" maps to this; Asira does not literally Vanish/Stealth — see **Disagreements #2**. |
| Asira Blade Barrier (30%) | At 30% HP Asira shields herself: incoming damage **below ~30-40k threshold reduces to 1**. Strat: stand in **Thrall's Rising Fire Totem** for the damage buff and use big-hit cooldowns to break the shield. Threshold value disagreed on (30k vs 40k vs tiered) — see **Disagreements #3**. |
| Asira "Walking Bomb" / explosive trap | **NOT FOUND in any reconciled guide**. The prompt's "Walking Bomb explosive trap on tank" does NOT match Cata Hour of Twilight Asira's known kit. The only bomb is the Choking Smoke Bomb (ground-placed AoE, not a tank-attached trap). **Bot logic should not assume a Walking Bomb mechanic exists** — flag for verification. See **Disagreements #4**. |
| Benedictus phase 1 — Holy / "Light" theme | Smite-style hardcasts on tank (interrupt!), **Righteous Shear** stacking AoE-flavor debuff (10y radius, **dispellable Magic**), **Purifying Light** orbs (3 spawn; players dodge), **Wave of Virtue** room-sweep (lethal contact). Thrall provides **Water Shell** bubble for the wave. |
| Benedictus phase 2 — Twilight transform (60%) | At 60% HP, **Twilight Epiphany** transforms boss into Twilight form AND **traps Thrall in a Twilight Prison** (Thrall is removed from combat, no totem support). Same kit but shadow-school: **Twilight Blast** (interrupt), **Twilight Shear** (10y AoE debuff, **dispellable Magic**), **Corrupting Twilight** orbs, **Wave of Twilight** sweep. |
| Benedictus dispel pressure | **CRITICAL** — Righteous Shear (phase 1) and Twilight Shear (phase 2) **must be dispelled** by the healer. They tick AoE damage to allies within 10y of the affected player; un-dispelled stacks compound rapidly. This is the only Cata 4.3 troll-heroic with a hard dispel-priority requirement on every cycle. |
| Benedictus orb avoidance | Purifying Light / Corrupting Twilight orbs spawn 3 at a time; in phase 1 Thrall destroys 2/3 with Chain Lightning so players only need to dodge 1; in **phase 2 there is no Thrall help**, all 3 orbs must be dodged. |
| Benedictus wave avoidance | Wave of Virtue / Wave of Twilight crosses the platform; lethal on contact. **Phase 1**: stand in Thrall's Water Shell bubble (also gives +100% damage). **Phase 2**: no shield — find a gap in the wave or use mobility/immunity (Iceblock, Divine Shield) to survive. |
| Heroic-only dispel pressure | Benedictus Shear dispels are the load-bearing dispel work in the entire instance. Arcurion's Chains of Frost is **technically Magic-dispellable** (Mass Dispel) but rarely strat-relevant — see **Disagreements #5**. |
| Curse/Disease/Poison dispel needs | None on bosses. |
| Tank swap | None on any boss. Single-tank fights throughout. |

---

## Boss 1 — Arcurion

NPC ID: **54590**. First boss, the Crystal Vice. Frost ascendant guarding the path. Encounter is single-phase with periodic Icy-Tomb-on-Thrall sub-events plus a hard 30%-HP execute phase (Torrent of Frost) where Thrall casts Bloodlust.

### Phase structure

1. **Pull** — tank-and-spank with periodic Hand of Frost on tank (interrupt), periodic Icy Tomb on Thrall (DPS the tomb to free), Frozen Servitor adds throwing Icy Boulders from canyon ledges (move out of telegraphs)
2. **30% HP** — Torrent of Frost channeled persistent group AoE; Thrall casts Bloodlust; DPS burn

### Abilities

| Spell | ID | Cast / cooldown | School | Notes |
|---|---|---|---|---|
| Hand of Frost | 102593 | 2s cast, 30y range | Frost | **Interruptible.** Heavy single-target nuke on tank (~45-55k Frost damage). Bot interrupt priority: HIGH. |
| Chains of Frost | 102582 | cast, 65y radius | Magic (Frost) | Wave-of-ice AoE (~9-11k) that **immobilizes** all enemies for 10s. **Magic-dispellable** (technically) but the immobilize is short enough that most strats don't dispel. |
| Icy Tomb | 103252 | scripted channel | Physical | Encases **Thrall** in a destructible ice tomb. Party DPS must break it before Thrall takes lethal damage. **Tomb has its own HP pool** — UNVERIFIED exact HP value. |
| Torrent of Frost | 103962 | scripted at 30% HP | Frost | At ≤30% HP, channels group-wide Frost AoE (~13-16k/sec) + 50% movement-speed slow. Persistent — must be burned through. Thrall casts Bloodlust to compensate. |
| Frozen Servitor (NPC 54600) | — | — | — | Adds on canyon ledges throwing **Icy Boulders** (~20k damage) with telegraphed ground reticles. **Not killable** in standard strat — environmental hazard. |
| Icy Boulder (Servitor projectile) | UNVERIFIED | telegraphed ~3s | Frost | Ground reticle → impact damage; players move out before impact. |

### Interrupts required

- **Hand of Frost** — primary kick target. Bot interrupt priority: HIGH. Rotate kicks across DPS/tank.
- Chains of Frost has a cast bar (UNVERIFIED interruptibility); kick rotation may hit it but most strats don't prioritize.

### Dispels required

- Chains of Frost is Magic-school and theoretically dispellable; most strats let the immobilize tick out. Bot: **do not auto-dispel** unless party is positioned for a critical kite.

### Avoidable damage

- **Icy Boulder ground reticles** — move out (~3s warning).
- **Torrent of Frost** — unavoidable AoE during the 30% execute; healed through.

### Tank-swap requirements

None. Single tank.

### Heroic delta

This is heroic-only — there is no "delta" comparison. All values listed are heroic.

### Add-handling

- **Frozen Servitors** are environmental, not killable. Do not target them.
- **Icy Tomb** is technically a "spawn" — it is the only mob the party should target other than the boss while it is up.

### Healer-pressure spikes

- Hand of Frost on tank (predictable; interrupt-or-heal-through)
- Torrent of Frost channel (constant group damage from 30% to kill)
- Icy Boulder hits if a DPS is slow to move

### Key positioning

- **Tank** holds Arcurion roughly central in the canyon; do not stand under ledges (Servitor-Boulder reticles overlap there)
- **DPS / healer** spread for Boulder telegraph movement; ranged at max range from boss for Chains of Frost
- **All players** burst the **Icy Tomb** when it spawns (DPS break)

### Bot-relevant role notes

- **Tank**: hold threat; interrupt Hand of Frost; pop defensive on Torrent of Frost channel.
- **Healer**: tank-Hand-of-Frost spikes are predictable (cast bar); group AoE during Torrent execute is heavy.
- **Melee**: cycle interrupts on Hand of Frost; switch to Icy Tomb when it spawns; move out of Boulder reticles.
- **Ranged**: same — **switch target to Icy Tomb when it spawns**, then back to boss; participate in interrupt rotation if class has a kick. Bot logic must detect Icy Tomb as a target switch event.
- **Thrall NPC**: **bot DOES NOT control Thrall** — but bot must detect "Thrall encased" as an event. Bot ALSO must not let Thrall die (mostly handled by DPS-breaking the tomb).

---

## Boss 2 — Asira Dawnslayer

NPC ID: **54968**. Second boss, Galakrond's Rest. Twilight assassin (former Sin'dorei rogue named Asira Sunbright). Encounter is single-phase but with a hard 30%-HP defensive shield (Blade Barrier).

### Phase structure

Single-phase with two sub-mechanics:

1. **Pull** — tank-and-spank with rotating Mark of Silence + Throw Knife sub-event and Choking Smoke Bomb ground placements
2. **30% HP** — Blade Barrier shield reduces all damage below threshold to 1; party stands in **Thrall's Rising Fire Totem** for the damage-+health buff and pops cooldowns to break the shield with single big hits

### Abilities

| Spell | ID | Cast / cooldown | School | Notes |
|---|---|---|---|---|
| Mark of Silence | 102726 | instant, 20s duration | Physical (debuff) | Marks a random spellcaster. **When the marked target casts a spell**, Asira throws a knife at them. The mark itself is non-damaging — the punishment fires only on cast. Bot **caster** logic: if marked, **stop casting** OR position so a non-marked ally is between you and Asira (knife hits the ally first). |
| Throw Knife | 103587 | instant projectile | Physical | Travels in a straight line; **damages the first ally hit** (~10k physical). If hit-target is the Mark-of-Silence target, additionally silences for 2.5s. Body-blockable. |
| Choking Smoke Bomb | 103790 | instant ground-place, 8-10y radius, ~20-40s duration | Nature | Cloud blocks targeting in/out + 5k Nature damage / 0.5-1s tick. Tank drags Asira OUT of the bomb; players outside don't enter. Bomb stays where placed. |
| Blade Barrier | 103562 (or **Lesser Blade Barrier** for the 25-30k tier) | instant at 30% HP | Physical | Damage-absorb shield: hits **below threshold reduce to 1**. Threshold ~30-40k (UNVERIFIED — see Disagreements #3). Breakable by a single hit ABOVE threshold. |
| Melee | — | — | Physical | Standard melee on tank |

### Interrupts required

- **None confirmed kick-required.** Mark of Silence and Choking Smoke Bomb are instant. Throw Knife is a projectile, not a cast. Blade Barrier is instant.

### Dispels required

- **None.** Mark of Silence is **physical**, not dispellable. Smoke Bomb is environmental, not on a player. Blade Barrier is on the boss (not dispellable by player).

### Avoidable damage

- **Choking Smoke Bomb** — exit/avoid the cloud. Tank drags boss out; DPS/healer don't walk through it.
- **Throw Knife** — unavoidable for the marked player IF they cast; mitigated by body-block.

### Tank-swap requirements

None.

### Heroic delta

Heroic-only — no normal-mode comparison.

### Add-handling

None during the fight.

### Healer-pressure spikes

- Smoke Bomb damage on anyone caught (rare with proper positioning)
- 30% Blade Barrier execute (long fight tail if shield isn't broken quickly = drained mana)

### Key positioning

- **Tank** keeps Asira in the **center of the room near Thrall's totems**; when she drops Smoke Bomb, drag her ~10y so the cloud is left behind
- **Casters** position so a melee/tank is between them and Asira (body-block knives)
- **Melee** stay within Thrall's Rising Fire Totem buff zone when possible

### Bot-relevant role notes

- **Tank**: drag-out-of-bomb is the key non-default behavior; hold threat through movement.
- **Healer**: minimal damage if positioning is clean; save mana for Blade Barrier execute.
- **Melee**: body-block knives if you're not the marked target (stand between Asira and the marked caster).
- **Ranged caster bot**: if you have **Mark of Silence on you, STOP CASTING** for the 20s duration (or position behind a body-blocker). This is a major bot-logic hook — caster bot must read self-debuff and gate its rotation. **For DoT classes (Aff Warlock, Shadow Priest), DoT ticks already on Asira are fine — only CASTS proc the knife.** UNVERIFIED whether channels (Drain Soul, Mind Flay) count as "casts" for this debuff — see **Disagreements #6**.
- **Thrall NPC**: drops Rising Fire Totem at scripted points — bot should **stand in the totem buff** during Blade Barrier execute and during DPS check windows.

### Bot bug-flag (NEW — primary impl flag for this encounter)

- **Smoke Bomb stealth-window hazard**: when boss is inside her own Choking Smoke Bomb cloud, **she becomes untargetable from outside the cloud** (LoS-block). A bot may aggressively chase the vanished target into the cloud (bad) or stand idle with no target (also bad). **Correct behavior: hold position, wait for tank to drag boss out, then re-acquire.** This is the closest thing to the prompt's "Stealth phases — bot must wait through stealth windows."

---

## Boss 3 — Archbishop Benedictus

NPC ID: **54938**. Final boss, Wyrmrest Temple roof / Chamber of Aspects. Two-phase fight; the most mechanically dense encounter in the dungeon. Boss reveals himself as a Twilight agent at 60% HP.

### Phase structure

Two-phase, transform-driven:

1. **Phase 1 — The Betrayal (100% → 60%)**: Holy/"Light" theme. Smite hardcasts on tank, Righteous Shear AoE-debuff dispel pressure, Purifying Light orbs (3 — Thrall destroys 2/3 with Chain Lightning), Wave of Virtue room-sweep (Thrall casts Water Shell to bubble the party). Thrall is alive and fighting alongside.
2. **Twilight Epiphany transition (at 60% HP)**: Benedictus reveals true form, transforms (visually + school swap), **traps Thrall in Twilight Prison** (Thrall removed from combat). Boss becomes Phase 2.
3. **Phase 2 — Twilight Ascension (60% → 0%)**: Shadow theme. Twilight Blast hardcasts on tank, Twilight Shear AoE-debuff dispel pressure, Corrupting Twilight orbs (3 — **no Thrall help**, all 3 must be dodged), Wave of Twilight room-sweep (**no Water Shell** — find a gap or use immunity).

### Abilities

| Spell | ID | Cast / cooldown | School | Phase | Notes |
|---|---|---|---|---|---|
| Smite | UNVERIFIED | cast | Holy | 1 | Tank-targeted hardcast. **Interruptible — primary kick.** |
| Righteous Shear | 103161 | instant, stacking | Holy | 1 | **Magic-dispellable** stacking debuff on a player; ticks Holy AoE damage to allies within 10y. **HEALER MUST DISPEL.** |
| Purifying Light | 103578 | scripted summon | Physical (orb summon) | 1 | Spawns 3 orbs above boss; orbs path toward random players → impact = Purifying Blast. Thrall destroys 2/3 with Chain Lightning; party dodges the third. |
| Purifying Blast | 103651 | impact-trigger | Holy | 1 | Orb-impact AoE (~80k) within 6y. Avoid by dodging the orb. |
| Wave of Virtue | 103684 | scripted instant, 15y radius | Holy | 1 | Room-sweep wave (~97-100k Holy + knockback). **Lethal contact.** Stand in Thrall's **Water Shell** bubble (which also gives +100% damage). |
| Twilight Epiphany | UNVERIFIED | scripted at 60% HP | Shadow | 1→2 | Transform event. Removes Thrall from combat (Twilight Prison). Triggers school swap on remaining mechanics. |
| Twilight Blast (cast) | 103777 | cast | Shadow | 2 | Tank-targeted hardcast. **Interruptible — primary kick.** Phase-2 mirror of Smite. |
| Twilight Shear | 103526 | instant, stacking | Shadow | 2 | **Magic-dispellable** stacking debuff; ticks Shadow AoE to allies within 10y. **HEALER MUST DISPEL.** Phase-2 mirror of Righteous Shear. |
| Corrupting Twilight | 103769 | scripted summon | Physical (orb summon) | 2 | Spawns 3 orbs; **NO Thrall help** — all 3 must be dodged. |
| (Twilight) Blast — orb impact | UNVERIFIED (likely paired with 103769) | impact-trigger | Shadow | 2 | Orb-impact AoE (~80k Shadow, 6y radius). |
| Wave of Twilight | 103781 | scripted instant, 15y radius | Shadow | 2 | Room-sweep (~97-100k Shadow + knockback). **No Water Shell available** — survive by gap-running or immunity (Iceblock, Divine Shield, Cloak of Shadows, Anti-Magic Shell). |
| melee | — | — | Physical | both | Standard tank melee |

### Interrupts required

- **Smite (P1)** — MUST_INTERRUPT every cast.
- **Twilight Blast (P2)** — MUST_INTERRUPT every cast.

### Dispels required

- **Righteous Shear (P1)** — DISPEL EVERY APPLICATION (Magic-dispel).
- **Twilight Shear (P2)** — DISPEL EVERY APPLICATION (Magic-dispel).
- These are the **load-bearing dispels** for the entire instance.

### Avoidable damage

- Purifying Light / Corrupting Twilight orbs — dodge sideways out of orb path
- Wave of Virtue — stand in Water Shell (P1 only)
- Wave of Twilight — gap-run or use immunity (P2)

### Tank-swap requirements

None.

### Heroic delta

Heroic-only — no normal-mode comparison.

### Add-handling

None — orbs are environmental projectiles, not killable adds in standard strat.

### Healer-pressure spikes

- Tank Smite / Twilight Blast hardcasts (interrupt-or-heal)
- Righteous/Twilight Shear stack damage if dispels lag
- Wave damage if a player gets clipped (often lethal — recovery is body-rez, not heal)
- Phase 2 transition spike: no more Thrall mitigation

### Key positioning

- **Spread** ≥10y to minimize Shear cleave to allies
- **Tank** holds boss central; rotate during waves to maintain melee uptime
- **Phase 1 wave**: stack INTO Thrall's Water Shell bubble (positioning gives both immunity and 100% damage buff)
- **Phase 2 wave**: find the **gap** in the wave (it has openings) or use an immunity cooldown

### Bot-relevant role notes

- **Tank**: interrupt Smite/Twilight Blast on every cast; manage waves (move boss away from Thrall's prison location for clean orb dodges).
- **Healer**: **DISPEL EVERY SHEAR APPLICATION** — this is the highest dispel-priority pattern in any Cata 5-man bot encounter so far. Pre-mana-CD before phase 2 (Thrall heals/totems are gone in P2).
- **Melee**: stay in spread despite being melee (Shear AoE radius is 10y); dodge orbs; gap-run waves in P2.
- **Ranged**: max range; dispel-Shear support (if class has Magic-dispel — Shaman Cleanse Spirit doesn't dispel Magic in 4.3.4, but Priest Mass Dispel does); orb-dodge.
- **Phase 2 transform detection**: bot must detect the **Twilight Epiphany** scripted event (boss model swap) and:
  - Re-acquire target (boss may be flagged as same NPC ID, but visual/aura set changes)
  - Discontinue waiting for Thrall support (no more Water Shell, no more Chain Lightning destroying orbs)
  - Adjust caster auto-attack/spell school detection (Holy → Shadow)
- This is a **major bot-implementation flag** — see encounter.md instance-wide notes.

---

## Trash mechanics

Hour of Twilight has comparatively sparse trash compared to other Cata 5-mans. The Thrall escort is the dominant gameplay outside boss encounters.

### Trash before Arcurion (Crystal Vice approach)

- **Crystalline Elementals** — small frost-themed elementals; tank-and-spank; light Frost auto-attack. AoE-burn pulls.
- **Frozen Servitors** (separate from Arcurion's environmental Servitors) — humanoid-elemental hybrid; some sources list a **Frost Bolt** hardcast (interruptible).

### Trash before Asira (Galakrond's Rest approach)

- **Twilight Assassins** — stealthed humanoids that appear from invisibility (visible warning circle on ground OR sudden engagement). **Tank may need to taunt** if they engage a non-tank first. CC-able via Sap/Polymorph/Hex (humanoid).
- **Twilight Bruisers** — melee humanoids; minor frontal cleave.

### Trash before Benedictus (Path of the Titans / Maw of Destruction → Wyrmrest)

- **Faceless Voidstalkers** — caster Old-God-flavor adds with **Shadow Bolt** hardcasts (interruptible).
- **Corrupted Slimes** — slimes that leave acidic ground patches.
- A few **Twilight Hammer cult** caster packs (Shadow Bolt interrupt rotation).

### CC priorities

- **Polymorph / Sap / Hex / Shackle Undead / Banish** valid on humanoids (Twilight Assassins, Twilight Bruisers, Twilight cult casters).
- **Faceless Voidstalkers** are not standard CC targets in 5-man (some are immune; rarely worth CC time).
- **Crystalline / Frozen elementals** — Hex (frog) works on elementals; Banish does NOT (Banish is demon/elemental? Banish is demon + elemental in Cata — UNVERIFIED).

### Skip-able trash

- A few mob clusters between bosses can be **wall-hugged / LoS-skipped** in speed-clear groups; default bot behavior is **clear all spine trash**.

### Thrall escort during trash

- **Thrall walks ahead/with the party between encounters.** Bot follow logic must NOT lose Thrall — see `AltbotFollow.cpp` gap.
- Thrall is **immune to trash damage post-hotfix** but the party should still defend him (loss of immunity is a known Blizzard tuning lever).
- Periodically Thrall stops, monologues, then continues — these are scripted pause points. Bot must wait through them rather than continue auto-pathing forward.

---

## Disagreements

Numbered list of points where the reconciled guides do not agree. Bot logic should pick a default and note the deviation.

### Disagreement #1 — Arcurion ground-AoE naming ("Glaciate" vs "Chains of Frost" vs "Torrent of Frost")

- **Prompt says**: "Glaciate frost ring AoE"
- **Wowhead Cata data**: there is **no spell called "Glaciate"** in Arcurion's kit. The two AoE candidates are:
  - **Chains of Frost (102582)** — Magic-school 65y wave, 10s immobilize, 9-11k damage. Cast periodically throughout fight.
  - **Torrent of Frost (103962)** — Frost-school persistent channel that begins at 30% HP, 13-16k/sec + 50% slow. Sustained damage.
- **No source uses the name "Glaciate"** — likely a mis-remembered name or aliasing of one of these two. Some original-Cata-era guides may have informally called Chains of Frost "Glaciate" — UNVERIFIED.

**Default:** treat the encounter as having **Chains of Frost** (periodic immobilize wave) AND **Torrent of Frost** (30%-HP persistent AoE). If the bot encounters a spell ID labeled "Glaciate" in DBC, alias it to whichever Frost-school AoE resolves at that ID.

### Disagreement #2 — Asira "Stealth phases" / Vanish

- **Prompt says**: "Smoke Bomb (group disable + Stealth phases — bot must wait through stealth windows)"
- **All reconciled sources**: Asira does **NOT** have a literal Vanish/Stealth phase. The closest mechanic is **Choking Smoke Bomb** — a ground-placed cloud that prevents target acquisition into/out of the cloud. When Asira stands inside the cloud (briefly, before the tank drags her out), she is effectively un-targetable from outside.
- This is functionally a **target-loss window** (~1-3s) but is not a Vanish/Stealth in the rogue sense.

**Default:** treat the bot's "wait through stealth window" behavior as **wait through smoke-bomb-clouds-target-loss** (~1-3s of "no valid target" until tank drags boss out). Bot should NOT aggressively chase into the cloud. If a long-duration true Stealth phase is found in DBC, bump this to a higher-priority bug fix.

### Disagreement #3 — Asira Blade Barrier threshold

- **Source A (Wowhead/Warcraft Tavern)**: 30,000 damage threshold; below = 1
- **Source B (Icy Veins/Ten Ton Hammer)**: 40,000 damage threshold
- **Source C (Wowpedia)**: tiered — initial 40k threshold; after first break, "Lesser Blade Barrier" with 25-30k threshold
- Wowhead "Lesser Blade Barrier" spell page: 30,000 absorb value

**Default:** treat as **two-tier Blade Barrier**: initial barrier needs ~40k single-hit to break; lesser barrier replaces it with ~30k threshold. Bot DPS logic during this phase: **prioritize big-hit cooldowns** (Chimera Shot, Aimed Shot, Pyroblast, Frostfire Bolt, Killing Spree, Avenger's Shield) over filler. UNVERIFIED exact threshold.

### Disagreement #4 — Asira "Walking Bomb" mechanic

- **Prompt says**: "Walking Bomb explosive trap on tank"
- **All reconciled sources**: NO mechanic by this name appears in Asira's kit. The only "bomb" is the Choking Smoke Bomb (ground-placed). Asira has **no tank-attached explosive debuff** and no "walking bomb" mechanic.
- **Possible explanation**: confusion with another Cata 5-man (e.g., Stonecore's Slabhide / High Priestess Azil don't have it either; possible confusion with Ozumat in Throne of the Tides? or with Ulduar's XT-002 "Tympanic Tantrum"? — neither is a 4.3 troll-heroic boss). May be a prompt-author miss.

**Default:** **bot logic should NOT model a Walking Bomb on Asira.** Document the absence. If a future DBC scan turns up an Asira-attached debuff that matches "Walking Bomb" semantics, escalate. For now, treat this as the prompt confusing Smoke Bomb (ground-place) with "Walking Bomb" (tank-attached).

### Disagreement #5 — Arcurion Chains of Frost dispellability

- **Source A**: dispel with Mass Dispel to clear group immobilize (rare)
- **Source B**: don't bother — duration is short, most groups eat it
- **Source C**: Cleanse-Spirit (Shaman) doesn't work (it's Magic, not Curse — Cleanse Spirit is curse/poison only in Cata)

**Default:** **do NOT auto-dispel Chains of Frost.** If a bot has Mass Dispel (Holy Priest), it can be a situational use (e.g., during a Boulder reticle the immobilized player needs to move) but not standard. Spam-dispelling 5 people's immobilize is wasted GCDs.

### Disagreement #6 — Asira Mark of Silence: do channels and DoTs proc the knife?

- **Source A**: any spellcast (including channels and DoT applications) procs the knife
- **Source B**: only hardcasts proc; instants and ticks don't
- **Wowhead Mark of Silence aura procs at 100% on cast events** — likely fires on any cast event, but there is community disagreement on what counts as a "cast."

**Default:** for the Mark-of-Silence target ONLY: if class has the option, **stop casting non-instant spells for the 20s duration**. Instants (DoT applications, instant casts) probably don't proc but the 20s window is short enough that bot can just hold rotation for the timer. DoT TICKS already on the boss are not casts — they continue. Channels (Drain Soul, Mind Flay, Mind Sear, Hurricane) — bot defaults to **stop channeling** during the mark.

---

## DBC verification (UNVERIFIED items)

Spell IDs and stack values listed below are from community-guide cross-reference. Add to `docs/research/dbc-verification-checklist.md` for resolution when the TC fork session opens.

| # | Spell / mechanic | Listed value | Why it matters for bot logic |
|---|---|---|---|
| 1 | Arcurion Hand of Frost | 102593 | Tank interrupt priority |
| 2 | Arcurion Chains of Frost | 102582 | Magic-dispellable AoE; verify dispel decision |
| 3 | Arcurion Icy Tomb on Thrall | 103252 | Target-switch trigger; Thrall-defense priority |
| 4 | Arcurion Torrent of Frost (30%) | 103962 | Phase-2 detection; Bloodlust-from-Thrall trigger |
| 5 | Frozen Servitor NPC | 54600 | Environmental hazard ID; not a kill target |
| 6 | Icy Boulder (Servitor projectile) | UNVERIFIED | Ground reticle detection |
| 7 | Asira Mark of Silence | 102726 | Caster-bot self-debuff cast-suppression trigger |
| 8 | Asira Throw Knife | 103587 | Body-block target signal |
| 9 | Asira Choking Smoke Bomb | 103790 | Ground avoidance + target-loss window |
| 10 | Asira Blade Barrier | 103562 | Big-hit-cooldown trigger; threshold value |
| 11 | Asira Lesser Blade Barrier | UNVERIFIED (likely 103562 derivative) | Second-tier threshold |
| 12 | Benedictus Smite (P1) | UNVERIFIED | Tank interrupt priority — phase 1 |
| 13 | Benedictus Righteous Shear (P1) | 103161 | Healer dispel priority — phase 1 |
| 14 | Benedictus Purifying Light (P1) | 103578 | Orb-summon trigger |
| 15 | Benedictus Purifying Blast (P1) | 103651 | Orb-impact damage |
| 16 | Benedictus Wave of Virtue (P1) | 103684 | Wave detection; Water-Shell stack signal |
| 17 | Benedictus Twilight Epiphany | UNVERIFIED | Phase-2 transition trigger — CRITICAL for bot logic |
| 18 | Benedictus Twilight Blast (P2) | 103777 | Tank interrupt priority — phase 2 |
| 19 | Benedictus Twilight Shear (P2) | 103526 | Healer dispel priority — phase 2 |
| 20 | Benedictus Corrupting Twilight (P2) | 103769 | Orb-summon trigger — phase 2 |
| 21 | Benedictus Wave of Twilight (P2) | 103781 | Wave detection — phase 2; gap-run trigger |
| 22 | Twilight Prison on Thrall | UNVERIFIED | Phase-2 entry signal; Thrall-removed-from-combat flag |
| 23 | Thrall Water Shell buff (P1) | UNVERIFIED | Stack-in-bubble trigger; +100% damage buff detection |
| 24 | Thrall Rising Fire Totem (Asira fight) | UNVERIFIED | Stand-in-totem trigger; damage/health buff detection |
| 25 | Thrall Bloodlust (Arcurion 30%) | likely 2825 (standard Bloodlust) | Cooldown alignment trigger |
| 26 | Asira Mark-of-Silence cast-trigger semantics | UNVERIFIED | Whether channels/DoTs/instants proc knife |

---

## Notes on prompt deviations

- **Heroic-only instance**: confirmed by all sources. There is no normal-mode Hour of Twilight; the LFD entry is heroic-gated at ilvl 353.
- **3 bosses, no optional**: confirmed. Hour of Twilight has the smallest boss count of any Cata 5-man.
- **Thrall NPC escort**: confirmed across all sources. **Major bot-implementation flag** — `AltbotFollow.cpp` does NOT have NPC-follow logic. Bot may path-follow the tank by default, which incidentally tracks Thrall most of the time, but the explicit "follow Thrall during scripted walking segments and pause at his stop points" behavior is unimplemented. See instance-wide notes in encounter.md.
- **Prompt's "Glaciate"** does not match any spell name in reconciled sources. Mapped to **Chains of Frost** (periodic immobilize wave) or **Torrent of Frost** (30% execute) per Disagreement #1.
- **Prompt's "Stealth phases"** does not match a literal Stealth/Vanish on Asira. Mapped to the **Choking Smoke Bomb target-loss window** (~1-3s of un-targetable boss while in cloud) per Disagreement #2.
- **Prompt's "Walking Bomb"** does NOT match any Asira ability in reconciled sources. Treated as a likely prompt-author miss; bot does not model this mechanic. See Disagreement #4.
- **Phase 2 transform on Benedictus** confirmed as **Twilight Epiphany** at 60% HP. Bot must detect the transform event AND the **Twilight Prison** debuff on Thrall (Thrall is removed from combat for phase 2).
- **Spell IDs are mostly RESOLVED for the dispel-priority abilities** (Righteous Shear 103161, Twilight Shear 103526) and the wave/orb mechanics (103684, 103781, 103578, 103769, 103651). UNVERIFIED gaps are mostly the Smite/Twilight-Blast tank-cast IDs and Thrall's buff IDs — not blockers for design but needed for compile-time constants.
- **No instance-wide curse/disease/poison dispel** required. Magic-dispel is required on Benedictus only (Shears) and is the load-bearing dispel work for the entire dungeon.
- Wowhead Cata is the spell-ID source of record; where it disagreed with secondary sources, Wowhead won.

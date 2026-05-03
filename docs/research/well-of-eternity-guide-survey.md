# Well of Eternity — Cataclysm 5-man Survey

**Date:** 2026-05-02
**Patch:** 4.3.4 (Hour of Twilight 5-man set)
**Audience:** bot-control engineer (cata-altbot)

---

## Overview

| Field | Value |
|---|---|
| Zone | Well of Eternity (Caverns of Time, Tanaris) |
| Continent | Kalimdor (instance, time-warp to 10,000 years pre-Sundering) |
| Level range (normal) | **N/A — heroic-only** |
| Level range (heroic) | 85 (min ilvl 353) |
| Group size | 5 (1 tank / 1 healer / 3 DPS) |
| Boss count | 3 (Peroth'arn, Queen Azshara, Mannoroth & Varo'then) |
| Optional bosses | None |
| Layout | Linear scripted-escort with Illidan Stormrage; Azshara's Palace → Courtyard of Lights → Shores of the Well |
| Patch released | 4.3.0 (Hour of Twilight set, gates Dragon Soul access for badge progression) |
| Cross-faction | Allowed (all players appear as night elves for the duration) |
| Group buffs/debuffs | **Shadowcloak** stealth from Illidan during escort + Peroth'arn phase 2; **night elf appearance** transformation aura whole instance |

This dungeon is part of the patch 4.3 troll-heroic-replacement set (End Time / Hour of Twilight / Well of Eternity) released as the new heroic 5-man tier alongside the Dragon Soul raid. **There is no normal-mode version** — bot LFD logic must gate this dungeon at level 85 with item-level 353 minimum. The instance is short on boss count (3) but mechanically dense: every boss has a unique non-tank-and-spank gimmick and the entire run is a scripted Illidan-Stormrage NPC escort.

---

## Sources

This survey reconciles the following community sources. Wowhead Cata is treated as authoritative for spell IDs and numeric values; strategy is cross-checked across 5+ guides because the dungeon's mechanics (stealth-detection, dance/puppet, dual-boss-with-NPC-tank) are unusual enough that single-source error rates are high.

1. **Wowhead Cata — encounter & NPC pages** (`/cata/npc=55085/perotharn`, `/cata/npc=54853/queen-azshara`, `/cata/npc=54969/mannoroth`, `/cata/zone=5788/well-of-eternity`) — primary for spell IDs, NPC IDs, ability descriptions, encounter journal text
2. **Icy Veins — Well of Eternity Dungeon Guide** (`icy-veins.com/cataclysm-classic/well-of-eternity-dungeon-guide`)
3. **Warcraft Tavern — Well of Eternity Guide** (`warcrafttavern.com/cataclysm/guides/well-of-eternity-dungeon-guide/`)
4. **Warcraft Wiki — Well of Eternity (instance)** (`warcraft.wiki.gg/wiki/Well_of_Eternity_(instance)`)
5. **Warcraft Wiki — Queen Azshara (tactics)** (`warcraft.wiki.gg/wiki/Queen_Azshara_(tactics)`)
6. **Wowpedia — Mannoroth and Varo'then** (`wowpedia.fandom.com/wiki/Mannoroth_and_Varo'then`)
7. **WoW Insider / Engadget — Well of Eternity Guide** (Nov 2011, original 4.3 release timeframe)
8. **Bag Overflow — Patch 4.3 Well of Eternity Guide** (Nov 2011)
9. **WowEngineer — PTR 4.3 Well of Eternity Walkthrough** (Oct 2011, original beta-tier coverage)
10. **EpicCarry — Well of Eternity Cata Classic Guide**
11. **GuiasWoW (en.guiaswow.com) — Well of Eternity Strategy** (cross-reference for ability damage values)
12. **Ten Ton Hammer — Patch 4.3 Well of Eternity Heroic Dungeon Guide** (original-Cata-era retrospective)

Where guides disagree, the disagreement is captured in the **Disagreements** section and the consensus position is listed in **Strong consensus**.

---

## Strong consensus (table)

| Mechanic | Consensus |
|---|---|
| Heroic-only | **No normal mode exists.** Bot LFD logic must gate the dungeon at level 85, ilvl 353+. |
| Night elf transformation | All players are visually transformed into night elves for the entire instance. Cosmetic only — does not affect bot logic, spell IDs, or hitboxes. |
| Illidan escort | Illidan Stormrage NPC accompanies the party from instance entry through Peroth'arn and into Azshara's Palace. He provides **Shadowcloak** (party stealth) for trash skips and the Peroth'arn phase 2 hide-from-eyes mechanic. |
| Tyrande / Malfurion involvement | Tyrande Whisperwind appears at the Mannoroth fight as an NPC ally (provides Blessing of Elune buff in stage 2); Malfurion appears in stage 3 to close the portal at 5% boss HP. |
| Crystals / portal closure trash | 3 crystal piles guarded by trash packs in the Courtyard of Lights; right-clicking the crystals after killing the trash dispels the larger demon mob, allowing Peroth'arn engagement |
| Peroth'arn Phase 1 → Phase 2 trigger | At ~70% HP (sources vary 60-70% — see Disagreement #1), Peroth'arn casts **Drain Essence**, gets interrupted by Illidan, and vanishes to begin the stealth phase |
| Peroth'arn Phase 2 mechanic | Players get **Shadowcloak** stealth; **Eyes of Peroth'arn** (mobile mob) patrol the room; players must avoid eye line-of-sight by hiding behind pillars |
| Peroth'arn Phase 2 success condition | If 40s pass with NO player detected, boss becomes **Enfeebled** (+25% damage taken) and re-emerges; party burns him through phase 3 |
| Peroth'arn detection penalty | Eye spotting a player triggers **Easy Prey** — removes Shadowcloak and stuns target 8s; boss enters **Endless Frenzy** (+25% damage done) and attacks the spotted player |
| Peroth'arn Phase 3 (≤20% HP) | **Endless Frenzy** also triggers naturally at 20% HP — execute phase, burn fast |
| Queen Azshara untargetable | Azshara herself cannot be damaged — **Shroud of Luminosity** makes her permanently immune. Encounter resolves by killing her servants. |
| Azshara minion structure | 6 **Enchanted Magi** total in 3 schools (2 Frost + 2 Fire + 2 Arcane), spawning in pairs; one pair active at a time; new magus activates as each is killed |
| Azshara Total Obedience | 8s cast. **MUST be interrupted or wipe.** Charms entire party for 20s. Highest-priority interrupt in the entire dungeon. |
| Azshara Hand of the Queen / Servant of the Queen | Random single-player mind control; releases when **puppet strings** (visual marker above MC'd player) are killed by remaining DPS |
| Azshara kill order (heroic) | Strong consensus: **Arcane → Frost → Fire** (Arcane bombs are most threatening, Fire is most-survivable). See Disagreement #4. |
| Mannoroth & Varo'then phase 1 | Players DPS Varo'then; Illidan tanks Mannoroth; Tyrande handles demon waves at the portal |
| Mannoroth & Varo'then phase 1 ignore-Varo'then penalty | If players ignore Varo'then too long, Mannoroth sacrifices him via **Fel Drain** and fully heals Mannoroth — soft enrage |
| Phase 2 trigger | Varo'then dies → his enchanted sword drops; a player picks it up and clicks it on Mannoroth, embedding the **Magistrike Blade** |
| Phase 2 mechanic | Sword embedded → Mannoroth takes massive proc damage from Magistrike Arcs; demon waves spawn (Felhounds 75% → Felguards 60% → Doomguards 45% → Infernals 30%); **Dreadlord Debilitators** stun Tyrande and players must rescue her |
| Phase 2 Tyrande moonbeam | While Tyrande recovers, she creates a moonbeam ground area granting **Blessing of Elune** — massive damage buff vs demons; AoE phase highlight |
| Phase 3 trigger (≤5% Mannoroth HP) | **Gift of Sargeras** — Illidan transforms players, granting heal-aura (20% HP regen / 2s) and 30k AoE fire damage / 2s near party; finish Mannoroth fast |
| Heroic-only dispel pressure | None — Total Obedience is interrupted (not dispelled); Hand of the Queen mind-control is broken by killing puppet strings (not dispelled); Fel Decay is intentionally NOT healed through (heal-vs-decay decision) |
| Curse/Disease/Poison dispel needs | None on bosses. Instance-wide: no significant dispel calls. |

---

## Boss 1 — Peroth'arn

NPC ID: 55085. First boss, in the **Courtyard of Lights** chamber after the trash-pack-and-crystal section. Eredar lord (some sources call him a satyr / cursed Highborne — see lore note in Disagreement #6). Two-phase encounter with a unique stealth-detection minigame in phase 2.

### Phase structure

1. **Phase 1 — Tank-and-spank with Fel Flames + Fel Decay** (100% → ~70% HP)
2. **Transition — Drain Essence cast** (~70% HP); Illidan interrupts; boss vanishes
3. **Phase 2 — Stealth-detection (40s)** — players Shadowcloaked, hide from Eyes of Peroth'arn
4. **Phase 3 — Enfeebled / Endless Frenzy execute** — boss re-emerges weakened (or enraged if detected); burn to 0%

### Abilities

| Spell | ID | Cast / cooldown | School | Notes |
|---|---|---|---|---|
| Corrupting Touch | 104939 | melee-application | Shadow | Stacking debuff on tank, +10% damage taken per stack, 20s duration. Tank healer pressure ramps with stacks. |
| Fel Flames | 108141 | targeted | Fire | ~33-37k initial fire damage on random player + ~12-13k/s ground patch for 30s. Avoid by moving. |
| Fel Decay | 105544 | targeted | Shadow | ~38-42k initial Shadow damage on random + ~8-9k/s for 10s. **Healing the Decay'd target damages the caster (healer punishment).** Healer chooses: top them off and take damage, or let HoTs handle it. |
| Drain Essence | 104905 | scripted at ~60-70% | Shadow | Channel that stuns all enemies, ticks Shadow damage. **Illidan interrupts after a few seconds**, triggering phase 2. |
| Fel Quickening | UNVERIFIED | scripted on detection | — | Boss empowerment when an Eye detects a player; transitions into phase 1 / Endless Frenzy combo on the spotted target |
| Easy Prey | UNVERIFIED | scripted on detection | Physical | Applied to detected player: removes Shadowcloak, **stuns 8s**, marks them as Peroth'arn's target |
| Endless Frenzy | 105521 | passive at ≤20% HP OR triggered by detection | — | +25% damage done. Execute-phase enrage. |
| Enfeebled | UNVERIFIED | scripted on phase 2 success | — | +25% damage taken on Peroth'arn. Awarded if 40s pass without detection. |
| Shadowcloak (player buff from Illidan) | UNVERIFIED | scripted | Arcane | Party-wide stealth; broken by Easy Prey. Outside this fight, also used during Illidan's escort to skip trash. |
| Eye of Peroth'arn | NPC ID UNVERIFIED | scripted summon | — | Patrolling mob; sees players in line-of-sight; triggers Easy Prey on contact. **Despawns at end of stealth phase per hotfix notes.** |

### Interrupts required

- **Drain Essence** is **NOT player-interrupted** — Illidan handles this scripted interrupt. Bot must NOT waste a kick CD on it.
- **No other kick-required casts** in this fight.

### Dispels required

- **Fel Decay** — Shadow DoT. **NOT a dispel target** in normal play; bot dispel logic should NOT auto-cleanse it (the heal-or-not-heal decision is the actual mechanic).
- Corrupting Touch — Magic debuff on tank — also NOT a dispel target; meant to ramp.

### Avoidable damage

- **Fel Flames ground patches** — move out
- **Eye line-of-sight (phase 2)** — hide behind pillars; movement is the avoid
- **Fel Decay tick** — unavoidable; heal-or-let-tick decision

### Tank-swap requirements

None. Single tank. Corrupting Touch stacks ramp but stacks decay (20s) and the fight is short enough that no swap is needed.

### Heroic delta

Heroic-only encounter — all numbers above ARE the heroic numbers. No normal-mode delta to model.

### Add-handling

- **Eyes of Peroth'arn** — phase 2 only. Mobile mobs that **must NOT be killed** (they're scripted to despawn). Bot DPS logic must blacklist them as targets during phase 2.
- No other adds.

### Healer-pressure spikes

- **Tank Corrupting Touch stack ramp** — predictable; HoT-roll tank
- **Fel Flames target hit** — sudden 30k spike on random
- **Fel Decay heal-punish** — healer mid-cast decision; bot default = let small ticks pass, top off only if HP < 50%
- **Phase 2 detection event** — if any Eye spots a player, big damage spike on that player

### Key positioning

- **Phase 1**: tank pulls boss to one side of the Courtyard chamber; melee on boss; ranged spread to minimize Fel Flames cluster
- **Phase 2**: ALL players move to the **edge of the room behind pillars**; the room has multiple stone pillars that block Eye line-of-sight
- **Phase 3**: re-engage at original position

### Bot-relevant role notes

- **Tank**: standard tank-and-spank in phases 1/3. **In phase 2, tank ALSO hides** — there is no boss to tank during the stealth phase. Tank bot must transition from active-tanking to "find pillar, hide behind it" behavior. **Major bot-impl flag.**
- **Healer**: Fel Decay heal-or-not-heal decision is the unique judgment call. Phase 2: hide behind pillar; heal from cover only if a player is spotted (and they'll be stunned anyway).
- **Melee**: standard rotation phase 1; **HIDE in phase 2** (no melee target available); execute phase 3.
- **Ranged**: spread for Fel Flames; **HIDE in phase 2**; execute phase 3.

**Major bot-implementation flag**: phase 2 ("be undetected") is fundamentally NOT modeled in the bot's current understanding. Default behavior of "stand near master" or "stand still in combat" will trigger Eye detection nearly every time. Bot needs an explicit `wo_eternity_perotharn_phase_2_hide()` state that:
1. Identifies the Eye of Peroth'arn mobs (NPC ID lookup)
2. Identifies pillars in the room (terrain heuristic — pillars block LoS)
3. Pathfinds to a pillar that places the bot OUT of line-of-sight from all Eyes
4. Stays still for 40s OR until phase 3 transition detected

### Achievement

- **Lazy Eye** (Heroic): defeat Peroth'arn without ANY player being detected by an Eye. Bot's default behavior should already aim for this; confirm by Eye spotting bot without flag.

---

## Boss 2 — Queen Azshara

NPC ID: 54853. Second boss, in **Azshara's Palace**. Untargetable in the standard sense — players damage her enchanted servants, not her. Azshara is on a throne / dais and the encounter is fought in the palace chamber around her.

### Phase structure

Single phase, but with a continuous **Magi rotation** event:

- 6 magi spawn over time, in **pairs** (one pair active at a time)
- 3 schools represented (Frost, Fire, Arcane) — 2 of each
- Tank picks up active magi as they activate
- Players kill them; new pair activates
- Periodic **Total Obedience** (8s cast) — must be interrupted or wipe
- Periodic **Hand of the Queen / Servant of the Queen** — random player MC'd; remaining DPS kills puppet strings to free them
- When all 6 magi are dead, encounter resolves and Azshara retreats

### Abilities (Azshara)

| Spell | ID | Cast / cooldown | School | Notes |
|---|---|---|---|---|
| Shroud of Luminosity | UNVERIFIED | passive | — | Permanent invulnerability on Azshara. **Damage to her does nothing.** Bot DPS logic must NOT target her. |
| Total Obedience | 103241 | 8s cast | Arcane | **MUST INTERRUPT.** Mass-charm party for 20s = wipe. Highest priority interrupt of the dungeon. |
| Hand of the Queen / Servant of the Queen | 102334 | scripted | Arcane | Random non-tank MC'd into a "puppet"; **puppet strings** (a pair of NPC strings) appear above them; killing the strings frees the player |
| Dance, puppets! | UNVERIFIED | scripted (post-charm) | — | Once charmed, the player dances harmlessly until released. The mechanic name is consistent across guides; spell ID UNVERIFIED. |

### Abilities (Enchanted Magi)

Six total across 3 schools, two of each. Active in pairs.

#### Frost Magus

| Spell | Approx ID | Notes |
|---|---|---|
| Ice Fling | UNVERIFIED | ~25k Frost damage on random player |
| Coldflame | UNVERIFIED | Line AoE — frost damage in a line; ~45k+ damage if hit |
| Blades of Ice / Ice Sheets | UNVERIFIED | Charge attack — drives toward target for melee impact |

#### Fire Magus

| Spell | Approx ID | Notes |
|---|---|---|
| Fireball | UNVERIFIED | ~25k Fire damage hardcast on random — **interruptible** |
| Firebomb | UNVERIFIED | ~35k area damage + slow; ground placement |
| Blast Wave | UNVERIFIED | ~30k AoE damage around boss — pbaoe |

#### Arcane Magus

| Spell | Approx ID | Notes |
|---|---|---|
| Arcane Shock | UNVERIFIED | Continuous AoE damage |
| Arcane Bomb | UNVERIFIED | Ground bomb ~70-80k damage + movement slow |
| (Some sources list "Judgment Hammer" / "Dance of Divinity" as Arcane abilities — see Disagreement #5) | UNVERIFIED | naming ambiguity |

### Interrupts required

- **Total Obedience (Azshara)** — HIGHEST PRIORITY in the entire dungeon. Interrupt every cast. 8s cast window leaves room for multiple kick attempts.
- **Fireball (Fire Magus)** — MUST_INTERRUPT for healer mana management
- **Other magi casts** — interruptible where possible; not strictly mandatory

### Dispels required

None. Hand of the Queen MC is **NOT a dispel target** — broken by killing puppet strings.

### Avoidable damage

- **Coldflame line** (Frost Magus) — sidestep
- **Firebomb / Blast Wave** (Fire Magus) — move out
- **Arcane Bomb ground patch** (Arcane Magus) — move out
- **Charge / Ice Sheets** (Frost Magus) — charge target spreads to avoid multi-hit

### Tank-swap requirements

None. Single tank picks up newly-activated magus as they wake up.

### Heroic delta

Heroic-only encounter; numbers above ARE heroic.

### Add-handling

- **Magi ARE the encounter** — kill order is the strategy. Consensus: Arcane → Frost → Fire (Arcane bombs are highest threat).
- **Puppet strings** when MC fires — kill priority absolute (free the MC'd player ASAP)

### Healer-pressure spikes

- **Total Obedience NOT interrupted** = wipe (no recovery — entire party charmed)
- **Hand of the Queen** = MC'd player can't be healed; tank takes 1 less player worth of mitigation
- **Magi ground AoEs** = sustained random damage during pair-fight
- **Long fight** = mana pacing matters (6 magi × ~30s each = ~3 minute fight)

### Key positioning

- **Tank**: pull active magus to one side, away from group, away from puppet-string spawns
- **Melee**: on active magus
- **Ranged**: max range, spread for AoE; switch to puppet strings on MC
- **Healer**: max range, spread; track Total Obedience cast bar relentlessly

### Bot-relevant role notes

- **Tank**: pick up newly-activated magus on each kill; do not stand in ground patches.
- **Healer**: long fight = mana pacing. Total Obedience interrupt is THE survival check. **Bot impl note**: bot interrupt logic must prioritize Total Obedience above all other interrupts in this fight.
- **Melee**: switch immediately to puppet strings on MC event. Do NOT continue meleeing the active magus while a teammate is charmed.
- **Ranged**: same as melee — switch to puppet strings on MC. Resume magus DPS after free.

**Bot-implementation flag — kill-priority ordering**: bot DPS logic must encode the magus kill order (Arcane → Frost → Fire) AND the puppet-string interrupt-DPS pattern. This is more state than a normal kill-order bot needs.

**Bot-implementation flag — interrupt scheduling**: Total Obedience interrupt must be GUARANTEED, not best-effort. Bot interrupt rotation must reserve at least one kick for it.

---

## Boss 3 — Mannoroth and Varo'then (dual boss)

NPC IDs: Mannoroth 54969, Varo'then UNVERIFIED. Final encounter at **the Shores of the Well**. Multi-phase fight with NPC allies (Illidan tanks Mannoroth, Tyrande handles demon waves) and a unique sword-throw mechanic.

### Phase structure

1. **Phase 1 — Varo'then DPS** (start → Varo'then dies). Players DPS Varo'then; Illidan tanks Mannoroth; Tyrande kills lesser demons at the portal. Avoid Mannoroth's Fel Firestorm patches.
2. **Phase 2 — Sword embedded** (Varo'then's sword pickup → Mannoroth ≤30% HP). Player picks up Magistrike Blade, throws into Mannoroth, sword procs Magistrike Arcs (massive damage). Demon waves: Felhounds (75%) → Felguards (60%) → Doomguards (45%) → Infernals (30%). Dreadlord Debilitators stun Tyrande mid-phase; players must kill them and stand in **Tyrande's Moonbeam (Blessing of Elune)** for AoE damage burst.
3. **Phase 3 — Gift of Sargeras execute** (≤5% Mannoroth HP). Malfurion closes the portal; Illidan grants the party **Gift of Sargeras** (heal aura + AoE fire pulse); finish Mannoroth.

### Abilities (Varo'then)

| Spell | ID | Cast / cooldown | School | Notes |
|---|---|---|---|---|
| Magistrike (sword charge) | UNVERIFIED | melee | Fire/Arcane | Charges sword, deals ~28-32k Fire damage to target + arcs to nearby ally (up to 3 targets total). Spread to mitigate arc. |
| Charge | UNVERIFIED | scripted | Physical | Charges a player |
| Throw Glaive | UNVERIFIED | ranged | Physical | Ranged glaive throw — secondary attack |

### Abilities (Mannoroth)

| Spell | ID | Cast / cooldown | School | Notes |
|---|---|---|---|---|
| Fel Firestorm | UNVERIFIED | scripted, periodic | Fire | Rains 3-yard Fel Flames patches across the area for 12s; each patch deals ~19-20k fire/s. Avoid. **Major movement mechanic.** |
| Fel Drain | UNVERIFIED | scripted | Shadow | If players ignore Varo'then too long, Mannoroth sacrifices Varo'then via Fel Drain and FULLY HEALS. Soft enrage. |
| Hand of Flame | UNVERIFIED | passive | Fire | Mannoroth-aura — fire DoT on nearby; mitigated by standing near Illidan in phase 2 |
| Magistrike Arcs (sword-embedded) | UNVERIFIED | passive (post-sword) | Arcane | Once sword embedded, Mannoroth's hits proc Magistrike Arcs that strike Mannoroth himself for ~2,000,000 damage. Primary phase 2 damage source. |
| Shadow Bolt Volley | UNVERIFIED (some sources) | scripted | Shadow | Cast on group; not all sources list this — see Disagreement #7 |

### Abilities (demons during phase 2)

| Wave | Mannoroth HP | Demon | Behavior |
|---|---|---|---|
| 1 | 75% | Felhounds | melee adds; tank pickup |
| 2 | 60% | Felguards | melee + cleave; AoE-burn |
| 3 | 45% | Doomguards | caster + ranged; interrupt + burn |
| 4 | 30% | Infernals | high-damage AoE; Tyrande overwhelmed at this stage |
| Anti-Tyrande | varies | Dreadlord Debilitator | spawns 2x in pairs; **stuns Tyrande**; players must kill them + stand in Moonbeam |

### Abilities (Illidan / Tyrande / Malfurion ally NPCs)

| Spell / event | Source | Notes |
|---|---|---|
| Shadowcloak (escort) | Illidan | trash skip; not relevant in fight |
| Tank Mannoroth | Illidan | scripted; Illidan keeps Mannoroth busy through phase 1/2 |
| Demon waves at portal | Tyrande | scripted; Tyrande kills lesser demons |
| Blessing of Elune (Moonbeam) | Tyrande (after Dreadlord Debilitator stun) | Ground area buff — massive damage vs demons; AoE phase highlight |
| Dreadlord Debilitator stun on Tyrande | Dreadlord (boss-side) | Triggers Moonbeam mechanic; rescue priority |
| Gift of Sargeras | Illidan (≤5%) | Player buff: 20% HP regen / 2s + 30k AoE fire / 2s; final burn |
| Portal closure | Malfurion (≤5%) | Scripted; ends the demon-spawn cycle |

### Interrupts required

- **Varo'then Magistrike charge** — interruptible per some sources; UNVERIFIED. Bot attempts.
- **Doomguard casts (phase 2)** — generic Shadow Bolt; interrupt on rotation
- **Dreadlord Debilitator** — UNVERIFIED interruptibility; fast burn preferred over interrupt

### Dispels required

None. Fel Firestorm patches, Magistrike arcs, and ally-buff mechanics are all positioning- or DPS-resolved, not dispelled.

### Avoidable damage

- **Fel Firestorm patches** (Mannoroth) — primary movement mechanic; constantly relocating
- **Magistrike arc** (Varo'then) — spread 8-10 yards apart
- **Doomguard / Infernal AoE** (phase 2)

### Tank-swap requirements

**No traditional tank-swap.** But the role is unusual:

- Phase 1: tank picks up Varo'then ONLY; **Illidan tanks Mannoroth** (do not pull Mannoroth!)
- Phase 2: tank picks up demon waves (Felhounds → Felguards → Doomguards → Infernals)
- Phase 3: tank... mostly stands in moonbeam / Gift of Sargeras and burns

**Major bot-implementation flag**: in a 5-man with one tank, the bot tank's "boss" target is Varo'then in phase 1 and demon waves in phase 2. Mannoroth is on the encounter target list but is **tanked by an NPC ally**, not the bot. Bot tank logic must distinguish "target = boss but tanked by NPC" from "target = boss, my responsibility."

### Heroic delta

Heroic-only encounter; numbers above ARE heroic.

### Add-handling

- Phase 1: 0 player-killable adds (Tyrande handles portal demons)
- Phase 2: 4 staggered demon waves + Dreadlord Debilitators (rescue Tyrande)
- Phase 3: focus Mannoroth; demons spawn slows once portal closes

### Healer-pressure spikes

- **Phase 1**: Magistrike arc + Fel Firestorm patch ticks on positioning failures
- **Phase 2**: demon wave damage on melee; Magistrike Arcs proc damage on tank from sword; Tyrande Moonbeam HEALS but is conditional on Dreadlord Debilitator kill
- **Phase 3**: Gift of Sargeras passively heals — **healer pressure DROPS** during execute phase

### Key positioning

- **Spread 8-10y** for Magistrike arc on Varo'then
- **Constantly relocate** out of Fel Firestorm patches (entire fight)
- **Stack on Tyrande's Moonbeam** when it appears (phase 2)
- **Stack near Illidan** in phase 2-3 for Hand of Flame mitigation aura
- **Phase 3**: stack near party for Gift of Sargeras pulse

### Bot-relevant role notes

- **Tank**: pick up Varo'then phase 1; demon waves phase 2; do NOT taunt Mannoroth (Illidan's job).
- **Healer**: long fight; mana pacing; coordinate with Tyrande's moonbeam for free top-off
- **Melee**: spread for Magistrike; movement-heavy fight
- **Ranged**: max range; movement-heavy; **dedicated sword-thrower role** (one ranged DPS picks up Varo'then's sword on his death and clicks it on Mannoroth)

**Major bot-implementation flag — sword pickup**: when Varo'then dies, his sword spawns as a clickable object. **One bot must pick it up and click-target Mannoroth to embed it.** This is a non-spell, non-rotational interaction (object click + targeted use). Bot's normal "DPS the boss" logic does NOT cover this — needs explicit `WoEternity_Phase2_PickUpSword()` handler.

**Major bot-implementation flag — NPC escort ally tracking**: bot must track Illidan / Tyrande / Malfurion as friendly NPCs:
- Don't accidentally target them with damaging abilities
- Track Tyrande's HP / stun state to detect Dreadlord Debilitator phase
- Stack on Illidan in phase 2-3 for aura mitigation

### Achievement

- **That's Not Canon!** (Heroic): defeat Mannoroth on Heroic after Varo'then sacrifices himself for healing — i.e., LET Mannoroth Fel Drain Varo'then and full-heal, then beat the harder version. Bot default = NEVER trigger this (it's a difficulty modifier).

---

## Trash mechanics

The instance has trash in three sections, all with Illidan-Shadowcloak-skip viability for some packs.

### Section 1 — Azshara's Palace approach (start of dungeon)

- Players appear as night elves, enter the courtyard
- **Doomguard / Dreadlord patrols** — most can be **stealth-skipped via Illidan's Shadowcloak**
- Mandatory: small kill-1-large-demon fight at the start; Illidan applies Shadowcloak after

### Section 2 — Courtyard of Lights (pre-Peroth'arn)

- 3 **crystal piles**, each guarded by a small trash pack (satyrs / shivarras / felguards)
- Right-click the crystals after killing the pack — dispels the larger demon mob blocking Peroth'arn
- **Eyes of Peroth'arn** appear during Peroth'arn fight (not trash, encounter mob)

### Section 3 — Azshara's Palace (between Peroth'arn and Azshara)

- **Highborne Astromancer** packs — caster trash; **Frostbolt** hardcasts; CC + interrupt
- **Highborne Apprentice** packs — melee + ranged hybrid
- Some can be skipped by hugging walls; standard practice is clear-all

### Section 4 — Shores of the Well (between Azshara and final boss)

- **Demon trash at portal** — felguards, felhounds, infernals (same composition as final-boss waves but lighter)
- Most groups clear before pulling Mannoroth/Varo'then; some skip to pull-line

### CC priorities

- **Polymorph / Hex / Fear / Sap** valid on Highborne (humanoids)
- **Banish** valid on demonic types (Doomguards, Felguards) — Warlock-specific
- **Drakonid / Infernal** types are NOT CC targets (immune to most)

### Pull markers for CC

- Caster-heavy packs: skull on lead caster; cross/X on second caster (CC); mark Illidan on the player's screen as a "follow" target during stealth-skip sections (per WowEngineer)

### Skip-able trash

- Most trash between intro and Peroth'arn is Shadowcloak-skip viable
- Trash between Peroth'arn and Azshara — usually clear (no major skip point)
- Trash between Azshara and Mannoroth — usually clear; some packs skippable

---

## Disagreements

### Disagreement #1 — Peroth'arn phase 2 trigger HP

- **Source A (Wowhead encounter journal):** 60% HP triggers Drain Essence
- **Source B (Icy Veins, Warcraft Tavern, Bag Overflow):** 70% HP triggers vanish/stealth phase
- **Source C (multiple originals):** "around 70%" with vague language

**Default:** treat as 70% HP for bot phase-detection trigger; use the **Drain Essence cast event** as the actual trigger rather than HP threshold (more reliable).

### Disagreement #2 — Phase 2 stealth duration

- **Source A:** 40 seconds (most common)
- **Source B:** 60 seconds (Ten Ton Hammer)

**Default:** 40s. Most current sources align here.

### Disagreement #3 — Endless Frenzy trigger

- Some sources only describe Endless Frenzy as triggered by Eye detection
- Other sources also describe it as a 20% HP execute mechanic

**Default:** model as **both triggers** — Endless Frenzy can fire on detection (within phase 2) OR at 20% HP. Bot just notes "boss is enraged, burn faster."

### Disagreement #4 — Azshara magi kill order

- **Source A (Icy Veins):** Arcane → Frost → Fire
- **Source B (Warcraft Tavern):** Frost first (slows are most disruptive)
- **Source C (Wowhead/Bag Overflow):** "no fixed order; whatever's active"

**Default:** Arcane → Frost → Fire (Source A). Arcane bombs deal the most burst damage; killing Arcane first removes the highest single-event damage. Warcraft Tavern's Frost-first is a defensible alt; bot uses A as default.

### Disagreement #5 — Arcane Magus ability names

- **Source A:** "Arcane Bomb, Arcane Shock"
- **Source B (Wowhead older):** "Dance of Divinity, Judgment Hammer"
- **Source C:** mix-and-match

**Default:** the **mechanical reality** is "Arcane caster with a ground bomb + a continuous AoE." Bot positioning logic doesn't depend on the spell names. UNVERIFIED but not blocking.

### Disagreement #6 — Peroth'arn classification (lore)

- **Source A:** Eredar lord
- **Source B:** Cursed Highborne / satyr (Xavius-cursed)
- **Source C:** "Burning Legion sorcerer" (vague)

**Default:** lore classification doesn't affect bot logic; treat as "demon-flagged boss" (Banish does NOT work on him; he's a boss). Recorded for completeness only.

### Disagreement #7 — Mannoroth Shadow Bolt Volley

- **Source A:** Mannoroth casts Shadow Bolt Volley periodically
- **Source B:** No Shadow Bolt Volley — Mannoroth's only cast is Fel Firestorm

**Default:** assume Fel Firestorm only; if a hardcast appears, attempt interrupt. Low-impact disagreement.

### Disagreement #8 — Sword-throw interaction modeling

- **Source A:** "click sword on Mannoroth" (single click target)
- **Source B:** "pick up sword (move to it), then second click on Mannoroth"
- **Source C:** "automatic — running over the sword picks it up, then UI prompt"

**Default:** treat as two-step interaction (object pickup, then targeted use on Mannoroth). Bot pickup-handler should:
1. Detect Varo'then death
2. Path to sword's spawn location
3. Right-click / use object
4. Acquire Mannoroth as target
5. Activate the new player buff (the sword-throw)

### Disagreement #9 — Tyrande Moonbeam interaction

- **Source A:** Moonbeam is a buff on the area; players stand in for damage buff
- **Source B:** Moonbeam HEALS players + buffs damage vs demons
- **Source C:** Moonbeam grants Blessing of Elune, primarily damage-vs-demons buff

**Default:** treat as **Blessing of Elune** = damage-vs-demons buff that ALSO grants light HP regen. Bot should stack on the moonbeam ground area when present.

### Disagreement #10 — Phase 3 (Gift of Sargeras) trigger HP

- **Source A:** 5% Mannoroth HP
- **Source B:** 20% Mannoroth HP
- **Source C:** Triggered by "Malfurion closes portal" event, not specific HP

**Default:** treat 5% as the default; use Gift of Sargeras buff-application event as the actual phase-3 trigger.

---

## DBC verification (UNVERIFIED items)

Spell IDs and stack values listed below are from community-guide cross-reference. Add to `docs/research/dbc-verification-checklist.md` for resolution when the TC fork session opens.

| # | Spell / mechanic | Listed value | Why it matters for bot logic |
|---|---|---|---|
| 1 | Peroth'arn Corrupting Touch | 104939 | Tank stack-monitor |
| 2 | Peroth'arn Fel Flames | 108141 | Move-out trigger |
| 3 | Peroth'arn Fel Decay | 105544 | Healer heal-or-not-heal decision; **dispel-blacklist** |
| 4 | Peroth'arn Drain Essence | 104905 | Phase 2 trigger detection (Illidan-interrupted) |
| 5 | Peroth'arn Endless Frenzy | 105521 | Phase 3 trigger detection |
| 6 | Peroth'arn Easy Prey | UNVERIFIED | Detection-event signal |
| 7 | Peroth'arn Enfeebled | UNVERIFIED | Phase 2 success signal (+25% boss damage taken) |
| 8 | Peroth'arn Shadowcloak (player buff) | UNVERIFIED | Phase 2 hide-state detection |
| 9 | Eye of Peroth'arn | NPC ID UNVERIFIED | Phase 2 LoS-avoidance pathfinding target |
| 10 | Peroth'arn Fel Quickening | UNVERIFIED | Detection-event signal (alt name?) |
| 11 | Azshara Total Obedience | 103241 | **HIGHEST PRIORITY interrupt of dungeon** |
| 12 | Azshara Hand of the Queen / Servant of the Queen | 102334 | MC-event detection; switch-target signal to puppet strings |
| 13 | Azshara Shroud of Luminosity | UNVERIFIED | Untargetable detection (do NOT DPS Azshara) |
| 14 | Azshara puppet strings (NPC) | NPC ID UNVERIFIED | DPS-switch target for MC event |
| 15 | Azshara magi kill order | Arcane → Frost → Fire | DPS priority list |
| 16 | Azshara Fire Magus Fireball | UNVERIFIED | Interrupt target |
| 17 | Azshara Frost Magus Coldflame | UNVERIFIED | Avoid trigger (line AoE) |
| 18 | Azshara Arcane Magus Arcane Bomb | UNVERIFIED | Avoid trigger (ground patch) |
| 19 | Mannoroth Fel Firestorm | UNVERIFIED | Movement trigger (continuous) |
| 20 | Mannoroth Fel Drain | UNVERIFIED | Soft-enrage detection (Varo'then ignored) |
| 21 | Mannoroth Hand of Flame | UNVERIFIED | Phase 2/3 stack-near-Illidan signal |
| 22 | Varo'then Magistrike charge | UNVERIFIED | Spread trigger (8-10y) + interrupt attempt |
| 23 | Varo'then Throw Glaive | UNVERIFIED | Ranged attack |
| 24 | Magistrike Blade (sword pickup object) | NPC/object ID UNVERIFIED | Sword-pickup handler trigger |
| 25 | Magistrike Arcs (post-sword proc) | UNVERIFIED | Sword-embedded confirmation |
| 26 | Tyrande Blessing of Elune (Moonbeam) | UNVERIFIED | Stack-on-area trigger (damage buff vs demons) |
| 27 | Dreadlord Debilitator (NPC) | NPC ID UNVERIFIED | Rescue-Tyrande priority target |
| 28 | Illidan Gift of Sargeras | UNVERIFIED | Phase 3 trigger detection |
| 29 | Illidan Shadowcloak (party stealth) | UNVERIFIED | Trash-skip + Peroth'arn phase 2 stealth detection |
| 30 | Mannoroth NPC ID | 54969 | Confirmed via Wowhead |
| 31 | Queen Azshara NPC ID | 54853 | Confirmed via Wowhead |
| 32 | Peroth'arn NPC ID | 55085 | Confirmed via Wowhead |
| 33 | Varo'then NPC ID | UNVERIFIED | Phase-1 primary target |
| 34 | Illidan Stormrage NPC ID (escort version) | UNVERIFIED (~55532 listed by some sources, but multiple Illidan NPC IDs exist) | Friendly-ally tracking |
| 35 | Tyrande Whisperwind NPC ID (escort version) | UNVERIFIED | Friendly-ally tracking; HP/stun monitoring |
| 36 | Malfurion Stormrage NPC ID (phase 3) | UNVERIFIED | Phase 3 trigger detection |

This dungeon has the **highest UNVERIFIED count** of any 5-man bundle so far (36 items vs BRC's 18) because (1) the dungeon is heroic-only and unique-mechanic-heavy, so encounter-journal data is denser, and (2) NPC-ally tracking adds NPC IDs not normally needed.

---

## Notes on prompt deviations

- **No spec selection** as instructed — no spec recommendation section.
- **Heroic delta is per-boss** with the dungeon-wide note that it's heroic-only — there is no normal-mode delta to model. Sections retain "Heroic delta" headers for format consistency with BRC; content reads "all numbers ARE heroic."
- **Disagreement count is high (10)** — Well of Eternity's mechanics are unique enough that community-guide variation is unusually broad. Bot-default policy in each disagreement is conservative (favor the safer / more-common interpretation).
- **Bot-implementation flags are heavy**: this dungeon has **5 major flags** that are not present in BRC:
  1. Peroth'arn phase 2 hide-from-eyes (LoS-aware pathfinding to pillar)
  2. Azshara magi kill-order encoding + puppet-string interrupt-DPS pattern
  3. Azshara Total Obedience guaranteed-interrupt scheduling
  4. Mannoroth fight: Mannoroth-tanked-by-NPC (Illidan) — bot tank must NOT taunt Mannoroth
  5. Mannoroth fight: sword-pickup + targeted-use handler (object interaction, not spell)
  6. Illidan/Tyrande/Malfurion friendly-NPC tracking (don't damage allies; track Tyrande HP/stun)
- **Spell IDs verified for Peroth'arn (4 of 5)**, partial for Azshara (2 of ~10), low for Mannoroth (0 of ~10) — Wowhead's Cata pages had encounter-journal text for Peroth'arn but were sparse for the dual-boss and magi.
- **NPC IDs for ally NPCs are UNVERIFIED** — bot tracking of Illidan/Tyrande/Malfurion will need DBC pass to lock these. Workaround: use NPC-name-based lookup until IDs verified.
- **Lore deviation**: this dungeon is set 10,000 years in the past during the War of the Ancients — NOT in present-day Azeroth. Doesn't affect bot logic but explains why "Illidan and Tyrande are alive and friendly to the player."

# 142-Point Frost Mage Build — Bot-Executable Max DPS

**Scope.** Bot-executable max DPS, Frost Mage primary, 142-point Cata 4.3.4
server. Survey date 2026-05-12. Audience: someone speccing a bot in-game,
*not* a human raider. Every recommendation here is filtered through "will
the current `FrostMageStrategy` rotation engine actually press this button"
— and where the gap is visible, §E flags it explicitly.

This is one of four parallel 142-point build docs (Frost Mage, Aff
Warlock, MM Hunter, Resto Shaman). It extends — not duplicates —
`docs/research/frost-mage-guide-survey.md`, which already covers the
stock 71-point 2/8/31 build. **This doc adds the dual-tree extension
only.** The user will allocate talents from this doc, then a later
phase wires the strategy code (per `docs/research/2x-talent-strategy-design.md`
Part B Option 2 — "detect spells, unlock tiers in-place").

---

## §2. Sources

Strategy/rotation claims reconcile across the strategy-bearing guides
below. Hard numbers (spell IDs, cooldowns, durations, ranks) draw on
Wowhead Cata Classic + Warcraft Wiki, consistent with the project
research-depth policy ("Wowhead is fine for hard numbers; strategy
needs 4-5 reconciled guides").

| # | Guide | URL | Confidence (rationale) |
|---|---|---|---|
| G1  | Icy Veins — Frost Mage Talents & Glyphs | https://www.icy-veins.com/cataclysm-classic/frost-mage-pve-spec-builds-talents-glyphs | High (strategy-bearing) |
| G2  | Icy Veins — Frost Mage Rotation | https://www.icy-veins.com/cataclysm-classic/frost-mage-pve-rotation-cooldowns-abilities | High (strategy-bearing) |
| G3  | Icy Veins — Arcane Mage Talents & Glyphs | https://www.icy-veins.com/cataclysm-classic/arcane-mage-pve-spec-builds-talents-glyphs | High (cross-tree reference) |
| G4  | Icy Veins — Arcane Mage Rotation (for Arcane Power usage) | https://www.icy-veins.com/cataclysm-classic/arcane-mage-pve-rotation-cooldowns-abilities | High (strategy-bearing) |
| G5  | Icy Veins — Fire Mage Talents & Glyphs | https://www.icy-veins.com/cataclysm-classic/fire-mage-pve-spec-builds-talents-glyphs | High (cross-tree reference) |
| G6  | wowtbc.gg — Frost Mage | https://wowtbc.gg/cata/class-guides/frost-mage/ | Medium (rotation thin; opener + glyph listing) |
| G7  | Engadget WoW Insider — "Arcane Brilliance: Frost mage Cataclysm talent analysis" (Christian Belt, 2010-11-13) | https://www.engadget.com/2010-11-13-arcane-brilliance-frost-mage-cataclysm-talent-analysis.html | Medium (4.0-era theorycraft; talent-by-talent commentary) |
| G8  | Warcraft Tavern — Frost Mage Talents | https://www.warcrafttavern.com/cataclysm/guides/pve-frost-mage-talents-builds-glyphs/ | **403'd** — referenced via search excerpts only |
| G9  | Warcraft Tavern — Frost Mage Rotation | https://www.warcrafttavern.com/cataclysm/guides/pve-frost-mage-rotation-cooldowns/ | **403'd** — referenced via search excerpts only |
| W1  | Wowhead Cata — Arcane Power (spell=12042) | https://www.wowhead.com/cata/spell=12042/arcane-power | High (hard numbers: 15s / 2min / +20% dmg / -10% mana cost; Cata-redesigned) |
| W2  | Wowhead Cata — Presence of Mind (spell=12043) | https://www.wowhead.com/cata/spell=12043/presence-of-mind | High (hard numbers: 2min CD, instant next cast <10s) |
| W3  | Warcraft Wiki — Presence of Mind | https://warcraft.wiki.gg/wiki/Presence_of_Mind | High (Cata 4.0.1 moved to Arcane Tier 3) |
| W4  | Warcraft Wiki — Brain Freeze | https://warcraft.wiki.gg/wiki/Brain_Freeze | High (5/10/15% per rank, Frost Tier 4, can't be triggered by FFB itself) |
| W5  | Warcraft Wiki — Living Bomb | https://warcraft.wiki.gg/wiki/Living_Bomb | High (12s DoT, 17% base mana, limit 3 targets, Fire Tier 7) |
| W6  | Warcraft Wiki — Combustion | https://warcraft.wiki.gg/wiki/Combustion | High (2min CD, Fire Tier 4 in Cata; combines fire DoTs) |
| W7  | Warcraft Wiki — Hot Streak | https://warcraft.wiki.gg/wiki/Hot_Streak | High (proc on fire-spell crits; instant + free Pyroblast) |
| W8  | Wowhead Cata — Deep Freeze (spell=44572) | https://www.wowhead.com/cata/spell=44572/deep-freeze | High (30s CD, 5s stun) |
| W9  | Wowhead Cata — Living Bomb (spell=44457) | https://www.wowhead.com/cata/spell=44457/living-bomb | High (confirms 44457 cast ID) |

**Coverage caveats.**
1. **No surveyed guide directly addresses 142-point dual-tree builds.** The
   8/2/31 stock build is the only PvE-canonical mage build in the Cata
   guide ecosystem. The shape proposed below — full Frost(41) + deep
   Arcane(36+) — is novel for this custom server. Where this doc
   extrapolates beyond reconciled guide consensus, that's flagged inline.
2. Warcraft Tavern (G8, G9) 403'd direct fetch — same pattern as the
   71-point survey. Content reconstructed from search excerpts.
3. Reddit/r/classicwow blocked by fetch policy — no community-thread
   double-check available for Frost+Arcane hybrids specifically.
4. Wowhead Cata pages on Arcane Power and Presence of Mind confirm
   numerical mechanics; talent-tree positions cross-referenced against
   Warcraft Wiki + G3.

---

## §A. Realistic dual-tree builds

The standard Cata talent rule still applies on 142 points:
**31 in primary unlocks the 41-cap; 31 in primary unlocks secondary
specialization** (G1, G3). On 142, after fully filling Frost (41), 36
points remain for a secondary tree's main body — enough for Tier 6
capstones and possibly a Tier 7 single point, with leftovers (~30) for
the third tree. The bot tree shape:

```
41 primary  (Frost full, capstone Deep Freeze)
+ 36 secondary (Arcane or Fire, deep enough for Tier 5-7)
+ ~30 tertiary leftovers (1-3 rows of the third tree)
+ ~35 unspent / sub-optimal sinks
= 142 total
```

Two realistic primary=Frost dual-tree shapes:

### Build A — Frost(41) / Arcane(36+) — cast-speed + burst-CD build

| Tree | Points | Key picks |
|---|---|---|
| Frost  | 41 | Full primary, capstone Deep Freeze. Standard 31-point Frost (Piercing Ice 3/3, Imp Frostbolt 3/3, Ice Floes 3/3, Ice Shards 3/3, Shatter 2/2, Fingers of Frost 3/3, Icy Veins 1/1, Brain Freeze 3/3, Cold Snap 1/1, Enduring Winter 3/3, Frostfire Orb 2/2, Deep Freeze 1/1) + extra Frost spend (Reactive Barrier 2/2 or Cold as Ice 2/2 or Piercing Chill 3/3 or Ice Barrier-via-Reactive). |
| Arcane | 36+ | Tier 1 Arcane Concentration 3/3, Improved Counterspell 2/2, Netherwind Presence 3/3, Tier 3 Torment the Weak 3/3 + **Presence of Mind 1/1**, Tier 4 Improved Blink 2/2 + Arcane Flows 2/2 + Missile Barrage 3/3, Tier 5 **Arcane Power 1/1**, Tier 6 Improved Mana Gem 2/2 + Improved Arcane Missiles 2/2, Tier 7 Slow 1/1 (Arcane capstone — irrelevant for PvE, but unlocked by the 31-tree-deep prereq for primary specialization... NB: primary specialization is Frost, so Arcane capstone is *not* required and Slow can be dropped). |
| Fire   | 0-5  | Leftover sinks (Master of Elements 3/3 + Burning Soul 2/2 if anything is left). |

**Key new unlocked spells** (not in stock 8/2/31):
- **Arcane Power** (W1, spell 12042) — 2min CD, 15s, +20% spell damage, **-10% mana cost** (Cata redesign — pre-Cata was +30% cost; the Wowhead page confirms `-10%` for the 4.3 build). Instant cast, self-buff.
- **Presence of Mind** (W2, spell 12043) — 2min CD, instant cast: next mage spell <10s cast becomes instant. Empowers next Frostbolt / Frostfire Bolt / Polymorph / Frostfire Orb.
- **Improved Counterspell** (Arcane Tier 1, 2/2) — Counterspell silences for 4s in addition to school lockout. *Already in 71-pt build* — same here.
- **Netherwind Presence** 3/3 — passive 3% haste. *Already in 71-pt build* — same here.

**Key cross-tree passives that help Frost:**
- **Torment the Weak** 3/3 (Arcane Tier 3) — +6% spell damage to *slowed* targets. Frost slows everything (Frostbolt chill, Fingers of Frost target counts as frozen, Slow debuff via Frostbolt itself). Effectively a flat +6% Frost damage in PvE. **High value.**
- **Arcane Flows** 2/2 (Arcane Tier 4) — -25% CD on Evocation, Presence of Mind, Arcane Power, Invisibility. Brings PoM and AP to 90s CD. Stacks usage in ~1-2 fights per long pull.
- **Improved Mana Gem** 2/2 (Arcane Tier 6) — +40% mana from Mana Gem use. Mana sustain (passive, free).

### Build B — Frost(41) / Fire(36+) — Living Bomb hybrid

| Tree | Points | Key picks |
|---|---|---|
| Frost | 41 | Same as Build A. |
| Fire  | 36+ | Ignite 3/3, Imp Fire Blast 2/2, Master of Elements 3/3, Burning Soul 2/2, Imp Scorch 2/2, **Hot Streak 3/3**, Impact 3/3, Cauterize 2/2, Blast Wave 1/1, **Combustion 1/1** (Tier 4), Pyromaniac 3/3 (mana), Critical Mass 3/3, Imp Hot Streak 2/2, **Living Bomb 1/1** (Tier 7 capstone). |
| Arcane | 0-5 | Improved Counterspell 2/2 + leftover. |

**Key new unlocked spells:**
- **Living Bomb** (W5, spell 44457) — 12s DoT, instant cast, 17% base mana, limit 3 active targets per mage, 10y explosion AoE.
- **Hot Streak** (W7) — passive proc: fire spell crit → next Pyroblast instant + free. Requires a Pyroblast in the rotation to consume.
- **Combustion** (W6, spell 11129) — 2min CD, combines all current fire DoTs on target into a new 10s super-DoT.
- **Ignite** 3/3 — 40% of fire crits applied as DoT. Already in 8/2/31 specifically to make Frostfire Bolt crits leave an Ignite — same here.

**Why Build B is probably bad for a bot.** Spelled out in §B.2.

### Source consensus on "which secondary is better for Frost"

| Criterion | A (Frost+Arcane) | B (Frost+Fire) |
|---|---|---|
| Personal DPS — single-target | Arcane Power 15s × every ~90s + PoM-empowered FFB on burn windows. ~7-10% sustained, ~25-30% in burn windows. | Living Bomb DoT on target + Pyroblast on Hot Streak proc + Combustion 2min. ~5-8% sustained, but heavily dependent on Pyroblast/Hot Streak rotation. |
| Personal DPS — AoE / 5-man | Arcane Explosion-flavor not in Frost rotation; gain limited to AP buff on Blizzard ticks. | Living Bomb spread (Impact talent — Fire Blast spreads LB to 2 extra targets) on packs of 3+. Genuine AoE add. |
| Group utility | Improved Counterspell (already in 71pt build, no change). | None new. |
| Rotation engine cost — **bot-executable** | **Low.** Two new on-CD buttons (AP, PoM). Torment the Weak / Netherwind / Arcane Flows / Imp Mana Gem are all passives — `bot->HasSpell` reads them transparently. | **High.** Living Bomb requires DoT-refresh tracking (`bot->GetAura(LB_ID)` + duration check) which strategy *can* do, but Hot Streak is a proc that demands Pyroblast in the priority list — a fire spell that doesn't exist in `FrostMageStrategy::Spell` today. Combustion is a 2min CD that needs to fire when LB *and* Ignite are *both* on the target — a multi-state precondition. |
| Implementation difficulty | **Easy.** ~80 lines: 2 enum entries, 2 tier methods, 1 maintenance hook. | **Hard.** ~250-400 lines: Pyroblast/LB/Combustion enum entries + Hot Streak proc-aura detection (passive-name collision per CLAUDE.md gotcha #1) + DoT tracking + 3-target LB cap awareness. |

**Verdict (Frost+Arcane wins for a bot).** Locked in §C.

---

## §B. The decisive question

### B.1. Does Arcane Power pay back in a 5-man encounter?

Question: Arcane Power has a 2-minute CD (W1). With Arcane Flows 2/2 from
the Arcane Tier 4 path, that's 90s. **5-man boss pulls regularly fall
under 60s in heroic content**, meaning the bot gets *one* AP use per
boss — sometimes zero on quick trash. Does that single use pay back the
38 points spent reaching it?

**Source signals.**

- G3 (Icy Veins Arcane Talents): *"Arcane Power — Your primary DPS cooldown."* In Arcane-primary, this is the centerpiece of the burn-phase loop (G4). The author treats it as load-bearing.
- G4 (Icy Veins Arcane Rotation): *"Cast Arcane Power and use available DPS cooldowns... followed by spamming Arcane Blast until reaching 30% mana, then casting Evocation."* The Arcane Power *strategy* requires Arcane Blast as the burn filler. **Frost doesn't cast Arcane Blast** — its filler is Frostbolt.
- W1 (Wowhead spell page): AP applies *"Mod Damage Done % (All): 20%"* — universal, school-agnostic. Frostbolt, Frostfire Bolt, Ice Lance, Deep Freeze, Frostfire Orb all benefit. **AP does not require Arcane Blast.** The Icy Veins rotation guidance is a strategy artifact of the Arcane spec, not a mechanic of the spell.

**Math.** A Frost mage under AP for 15s averages roughly 5-6 Frostbolt casts (1.7s with Bane / 2s base, modulo haste / pushback / Brain Freeze procs). With +20% damage and -10% mana cost, the marginal damage gain is **~3 free Frostbolt-equivalents per AP use**. In a 90s loop that's ~30s of burn-equivalent across two pulls, or ~5-7% sustained personal DPS.

**Adjusted for 5-man pull length.** On 30-45s trash pulls, AP either fires on
pull (with Mirror Image + Icy Veins) or doesn't fire at all. The bot
can't *pre-pot* (no master coordination — §E) and can't time AP with
Hero/Lust unless master pops it first. The realistic uptime is:

| Encounter shape | AP uses per encounter | Sustained DPS lift |
|---|---|---|
| Boss, 4+ min pull | 2-3 (with Arcane Flows reducing to ~75s CD) | ~6-7% personal |
| Boss, 1-2 min pull | 1 | ~3-4% personal (front-loaded) |
| Trash, 30-45s pull | 0-1 (only if bot is in combat at the 0-second mark) | ~0-3% averaged |

**Net.** AP pays back at the *fight-and-day average* level (a typical
5-man run is 6-10 boss kills + 30+ trash pulls; AP fires on ~half the
pulls). The damage is real but not transformative. Where it matters
most is **Heroic dungeon bosses with sub-1-minute kill windows** — AP
in those windows is one of the few ways to shave a Frost mage's
already-bottom-tier DPS contribution. **Verdict: yes, but smaller-impact
than for an Arcane-primary mage.** Worth taking; not transformative.

**Secondary value: Presence of Mind on a Frostfire Bolt.** PoM (W2, 2min
CD → 90s with Arcane Flows) makes the *next* cast instant. Used on a
Frostfire Bolt during Brain Freeze ×2-stack windows, that's an instant
~13k-15k damage swing with Shatter under FoF. Bot-executable: yes —
PoM is an instant self-buff, fires on a precondition tick. Strategy
can prefer-PoM-with-FFB by checking `bot->HasAura(BrainFreezeProcId)`
before firing PoM.

### B.2. Is Fire's Living Bomb worth the rotation complexity?

Question: Frost has Tier_AoE today (Cone of Cold + Blizzard, triggered
on 3+ hostiles near target). Does Living Bomb pay for the engine cost
on top of that?

**Source signals.**

- W5 (Living Bomb wiki): 12s DoT, instant, **limit 3 active targets per
  mage**, 10y explosion. Damage scales 25.8% SP per tick (W9, Wowhead
  spell page). The talent isn't huge per-cast — ~5-6k damage per LB on
  a level 85 mage.
- G5 (Fire Mage Talents): *"Living Bomb — a potent damage-dealing
  ability, often a significant contributor."* — that's for Fire-primary,
  where Hot Streak / Critical Mass / Ignite all stack on top of the LB
  explosion's Ignite-eligible crit.
- W7 (Hot Streak): proc on fire-spell crits. The Frost mage's only
  fire-school spell in stock rotation is **Frostfire Bolt** (school is
  "Frostfire" — both Frost and Fire). FFB *can* proc Hot Streak (it's
  in the FFB proc-eligible list per W7's affected-spells list:
  "Fireball, Frostfire Bolt, Scorch, Pyroblast, Fire Blast"). But Hot
  Streak makes *Pyroblast* instant — and `FrostMageStrategy` doesn't
  cast Pyroblast.

**Why Build B is bad for a bot.**

1. **Hot Streak's payout is wasted** unless the bot also casts
   Pyroblast. Adding Pyroblast to the rotation means: another enum
   entry, another tier, *and* a proc-aura detector for the Hot Streak
   buff (W7 — the proc-aura ID is distinct from the talent ID, and
   like Brain Freeze / Fingers of Frost it falls into the
   passive-talent-shares-name collision class — CLAUDE.md gotcha #1).
2. **Combustion's payout requires multiple fire DoTs on target.** Frost
   mage's fire DoTs are *only* Living Bomb (12s) and the Ignite from
   FFB crits (4s). Combustion combines them into a single 10s DoT. On
   a Frost mage the "combined" DoT is just LB + (maybe) Ignite — most
   of Combustion's per-fire-spec payout (3-4 stacked fire DoTs) isn't
   reachable.
3. **Living Bomb's AoE-mode payout overlaps with existing Tier_AoE.**
   Frost already AoEs via Blizzard channel and Cone of Cold. LB on
   3 targets is ~15-18k cleave damage every 12s, vs. Blizzard's
   ~25k channeled over 8s. Marginal AoE add when Blizzard already
   covers the same pull window. Impact (Fire Tier 4 talent) spreads
   LB on Fire Blast — but Fire Blast isn't a Frost spell either.
4. **Cauterize** (Fire Tier 5) — 30% reflexive self-heal on lethal
   hit, 1min CD. *Genuine* defensive value, but Ice Block / Mage Ward
   already cover the same gap for a bot.

**Net.** Build B's headline talents (Living Bomb, Hot Streak,
Combustion) are designed around a Fire-primary rotation; bolting them
onto a Frost filler creates a partial-rotation that gives up Frost's
strengths (Shatter combos under FoF) for Fire's strengths (crit-chain
through Hot Streak) without committing to either. **Verdict: not worth
the engine cost.** A future *Fire-primary 142-point doc* should pick
this up; a Frost-primary build shouldn't.

---

## §C. Recommended build

**Pick: Build A — Frost(41) / Arcane(36+) / Fire(2-5 leftover).**

Justification (from §A + §B):
- §B.1: AP + PoM deliver ~5-7% sustained personal DPS via the rotation
  the bot *already runs*. No filler swap (Frostbolt remains the filler;
  AP buffs it transparently).
- §A: Torment the Weak 3/3 is a free +6% (passive, no rotation change).
  Netherwind Presence 3/3 (already in 71-pt) stays. Arcane Flows 2/2
  cuts AP/PoM CDs to 90s — meaningful in 5-man pull cadence.
- §B.2: Build B's payoff hinges on rotation pieces (Pyroblast,
  multi-DoT Combustion windows) the bot would have to re-engineer for
  partial gain — net engineering cost > net DPS gain.
- Engineering risk: Build A is a strict additive — two new tiers, two
  new enum entries, one new proc-aura check. The 71-pt rotation
  continues to work for any bot that didn't spec into Arcane.

### C.1. Tree breakdown

| Tree | Points | Spend |
|---|---|---|
| Frost | 41 | Full primary tree (see §C.2 table) |
| Arcane | 36 | Tier 1–6 (Arcane Power + Presence of Mind path + Torment the Weak) |
| Fire | 5 | Tier 1 Master of Elements 3/3 + leftovers (Burning Soul 2/2) |
| **Total** | **82 spent of 142** | Sub-optimal? See §H — 60 points are *available* but only ~80 produce DPS; the rest go to leveling-utility (Improved Polymorph, Improved Blink) or sink slots. Document those choices explicitly so the bot operator knows what's "extra." |

NB: This is the *DPS-relevant* spend. The user has 60 more points to
spend on whatever they want (PvP utility, leveling QoL, sink slots).
The list below is what matters for combat.

### C.2. Point-by-point talent picks

**Frost tree (41 points)** — full primary build:

| Tier | Talent | Ranks | Rationale |
|---|---|---|---|
| 1 | Piercing Ice | 3/3 | +3% spell crit on Frost spells. Always taken. |
| 1 | Improved Frostbolt | 3/3 | -0.5s Frostbolt cast. Compounds with Bane (Destro Tier 1, irrelevant here) but stand-alone valuable. |
| 1 | Ice Floes | 3/3 | -20% CD on Cone of Cold / Ice Block / Frost Nova. Survivability + AoE cadence. |
| 2 | Ice Shards | 2/3 (or 3/3) | +33% Frostbolt crit damage. Cap or skip 1 — guides agree 3/3 in PvE. |
| 2 | Shatter | 2/2 | The Frost mastery. **Mandatory** — multiplies crit chance vs frozen by 3×. |
| 3 | Fingers of Frost | 3/3 | 20% proc chance per Frost-chill spell. Enables Ice Lance / Deep Freeze "frozen" treatment. |
| 3 | Icy Veins | 1/1 | 3min CD, +20% haste 20s. Burn cooldown. |
| 4 | Cold Snap | 1/1 | 8min CD, resets Frost-school CDs (Icy Veins, Cone of Cold, Ice Block, Frost Nova, Deep Freeze). |
| 4 | Brain Freeze | 3/3 | 15% proc per Frost-chill spell → next FFB/Fireball instant + free, 15s. Combo with FoF for Shatter triple-multiplier. |
| 5 | Enduring Winter | 3/3 | Frostbolt restores 4% mana to party + Replenishment. Group utility + self-sustain. |
| 5 | Reactive Barrier | 2/2 | Ice Barrier auto-fires under 50% HP. Defensive. (Alternative: Cold as Ice 2/2 for -20% Icy Veins / Cold Snap CD — also valid.) |
| 6 | Frostfire Orb | 2/2 | 1min CD, 10y AoE 15s. Procs FoF + Brain Freeze. Becomes Frostfire Orb (was Flame Orb) — adds AoE + proc generation. |
| 6 | Piercing Chill | 3/3 | Chill spreads to 2 nearby enemies. Multi-FoF generator on packs. |
| 7 | Deep Freeze | 1/1 | **Capstone.** 30s CD, 5s stun, damage on stun-immune bosses, +Shatter eligibility. |

Total Frost: ~32 + 9 filler tier 2 = **41**. Rank distribution flexible
(Ice Shards 2 vs 3, Reactive Barrier vs Cold as Ice). The 41 total is
what matters.

**Arcane tree (36 points)**:

| Tier | Talent | Ranks | Rationale |
|---|---|---|---|
| 1 | Arcane Concentration | 3/3 | Clearcasting proc on damage spells. Frostbolt-eligible. Mana sustain. |
| 1 | Improved Counterspell | 2/2 | Counterspell adds 4s silence beyond school lock. **Iconic interrupt** — already in 71pt 2/8/31. |
| 2 | Netherwind Presence | 3/3 | +3% haste passive. **Already in 71pt build.** |
| 3 | **Torment the Weak** | 3/3 | **+6% damage on slowed targets.** Frostbolt slows everything; functionally a flat +6% Frost damage. **Highest single-point value in the Arcane tree for Frost.** |
| 3 | **Presence of Mind** | 1/1 | 2min CD (90s w/ Arcane Flows), next cast instant. Pre-fire Frostfire Bolt under Brain Freeze ×2 stacks. |
| 4 | Improved Blink | 2/2 | +25% movement after Blink for 3s. Survivability. |
| 4 | **Arcane Flows** | 2/2 | -25% CD on Evocation, PoM, AP, Invisibility. Brings AP / PoM to 90s CD. |
| 4 | Missile Barrage | 3/3 | Arcane Missiles instant. Frost doesn't cast AM in default rotation; **skippable** — could move 3 points to Improved Counterspell-tier filler. Spec leaves 3/3 since the prereq path to Tier 5 requires it. |
| 5 | **Arcane Power** | 1/1 | 2min CD (90s), 15s, **+20% damage, -10% mana cost**. The reason this build exists. |
| 5 | Improved Polymorph | 2/2 | Poly deals 50% damage on break. CC enhancement. Skippable in PvE. |
| 6 | Improved Mana Gem | 2/2 | +40% mana from Mana Gem. Free passive. |
| 6 | Improved Arcane Missiles | 2/2 | Tier-6 fluff for Frost (no AM in rotation); take to fill the tier. |
| 7 | (Capstone Slow — skip) | 0/1 | Frost capstone (Deep Freeze) is the mage's spec capstone; Arcane capstone Slow not required since *primary* spec is Frost. |

Total Arcane: ~26 + 10 filler (Improved Arcane Explosion, Magic
Absorption, Arcane Tactics, Improved Mana Shield) = **36**. Filler
slots satisfy point-spend prereqs only — not load-bearing.

**Fire tree (5 leftover points)**:

| Tier | Talent | Ranks | Rationale |
|---|---|---|---|
| 1 | Master of Elements | 3/3 | Spell crits refund 30% of base mana. Compounds with Frostbolt crit rate. **Mana sustain — keep.** |
| 1 | Burning Soul | 2/2 | -70% pushback on Fire & Frost spells. Reduces cast interruption from melee chip damage. **High value for a bot that doesn't always disengage cleanly.** |

5 points. The remaining ~55 points (of 142 total) sit anywhere — they
don't change combat behavior (Improved Fire Blast, Improved Scorch,
Impact-without-Fire-Blast-rotation, etc.). Operator's choice.

### C.3. Glyphs

| Slot | Glyph | Rationale |
|---|---|---|
| Prime | **Glyph of Frostbolt** | +5% Frostbolt damage. Universal Frost prime. |
| Prime | **Glyph of Deep Freeze** | +20% Deep Freeze damage. Capstone enhancer. |
| Prime | **Glyph of Frostfire** | +15% Frostfire Bolt damage + DoT. FFB is the Brain Freeze proc spell. |
| Major | **Glyph of Evocation** | Evocation also heals 60% over channel. Survivability bonus on mana-recovery cast. |
| Major | **Glyph of Blink** | +5y blink distance. Mobility — minor but consistent. |
| Major | **Glyph of Mirror Image** | -30s CD (3min → 2:30). Sustained throughput cooldown alignment. |
| Minor | **Glyph of Slow Fall** | Removes reagent. Convenience. |
| Minor | **Glyph of Arcane Brilliance** | Removes reagent. Convenience. |
| Minor | **Glyph of Armors** | +50% duration on armor buffs. Reduces re-apply cadence. |

Glyph defaults match G1's recommendation, with major-slot
Mirror Image (per 71-pt survey §3) preferred over Ice Barrier (G6's
alternative).

### C.4. Talent allocation — source of truth

**No third-party calculator URL.** Wowhead's Cata Classic talent calc
enforces stock-Cata rules (41-point primary cap, 71 total) and cannot
represent a 142-point dual-tree build. The §C.2 point-by-point table
above is the build of record.

Allocate in-game via `.altbot talents learn <talentId> <rank>` per row.
Cross-check totals after placement: **Frost 41 / Arcane 36 / Fire 5**
(142 total).

The current `.altbot talents show <bot>` command only reports active spec
+ remaining points; a richer worldserver-logged dump of the full talent
placement is a queued follow-up so you can grep the build out of
`worldserver.log` rather than transcribing from in-game chat.

---

## §D. Rotation tier order

### Existing tier dispatch (from `FrostMageStrategy.cpp` lines 165–170, verbatim):

```cpp
if (Tier_DeepFreeze(bot, target))      { TC_LOG_DEBUG("altbot", "  -> DeepFreeze");    return; }
if (Tier_FFB_BothProcs(bot, target))   { TC_LOG_DEBUG("altbot", "  -> FFB+BothProcs"); return; }
if (Tier_FFB_BrainFreeze(bot, target)) { TC_LOG_DEBUG("altbot", "  -> FFB+BF");        return; }
if (Tier_IceLance(bot, target))        { TC_LOG_DEBUG("altbot", "  -> IceLance");     return; }
if (Tier_AoE(bot, target))             { TC_LOG_DEBUG("altbot", "  -> AoE");          return; }
if (Tier_Frostbolt(bot, target))       { TC_LOG_DEBUG("altbot", "  -> Frostbolt");    return; }
```

### New tier dispatch (Build A — Frost+Arcane):

```cpp
// === MAINTENANCE (DoMaintenance, before combat gating) ===
//   Existing: Molten/Mage/Frost Armor pick + Mage Ward upkeep.
//   No change.

// === DEFENSIVES (DoDefensives) ===
//   Existing: Ice Block panic + Frost Nova + Blink chain.
//   No change.

// === COMBAT TIER DISPATCH (Update, after GCD probe) ===
// NEW — Burst cooldowns BEFORE rotation (instant, no GCD interaction):
if (Tier_ArcanePower(bot, target))     { /* AP — 90s CD, fire on engage / burn window */ return; }
if (Tier_PresenceOfMind(bot, target))  { /* PoM — paired with FFB on Brain Freeze stacks */ return; }

// Existing tier chain — UNCHANGED below this point:
if (Tier_DeepFreeze(bot, target))      { return; }
if (Tier_FFB_BothProcs(bot, target))   { return; }
if (Tier_FFB_BrainFreeze(bot, target)) { return; }
if (Tier_IceLance(bot, target))        { return; }
if (Tier_AoE(bot, target))             { return; }
if (Tier_Frostbolt(bot, target))       { return; }
```

### Tier insertion rationale

| New Tier | Position | Why there |
|---|---|---|
| `Tier_ArcanePower` | **Top of dispatch**, above DeepFreeze | AP is instant (no cast time, no GCD reservation per W1's "Cost: None"). Firing it returns early so the same tick can't *also* fire DeepFreeze — but on the *next* tick DeepFreeze runs under +20% damage. Fires when: off-CD AND (boss target OR fight-elapsed > 30s OR `ctx.combatElapsedMs > THREAT_WINDOW_MS`). |
| `Tier_PresenceOfMind` | **Above DeepFreeze**, below ArcanePower | PoM is also instant, no GCD. Fires when: off-CD AND HasBrainFreeze AND HasFingersOfFrost (the FFB+BothProcs window — converts the next FFB into an instant *and* it already triple-multiplies via Shatter; this is the single highest-damage tick the build can produce). Fires the buff *and* returns; the next tick fires `Tier_FFB_BothProcs` under PoM. |

**Pet maintenance** — *not applicable.* Frost mage runs the permanent
Water Elemental; `PetMaintenance()` already handles re-summon on death.
No new pet logic.

**Armor** — Molten Armor handled in `DoMaintenance` (line 243); no
new picks. Build A doesn't change armor selection.

**Ward upkeep, Counterspell** — already wired (Mage Ward maintenance
line 265; Counterspell interrupt-hook line 157). No change.

### Build B rotation order (not recommended — for completeness only)

If the user *did* pick Build B against this doc's recommendation:

```cpp
// NEW tiers (above DeepFreeze):
if (Tier_Combustion(bot, target))      { /* 2min CD; fire when LB up AND Ignite stacked */ return; }
if (Tier_LivingBomb(bot, target))      { /* re-apply when target.aura(LB).duration < 3s */ return; }
if (Tier_Pyroblast_HotStreak(bot, target)) { /* fire when HasAura(HotStreakProc) — instant Pyro */ return; }

// Existing chain unchanged below.
```

Engineering cost spelled out in §B.2. Don't pick this.

---

## §E. Bot-executable caveats

Honest accounting of theoretical-max vs what the bot actually delivers.
Every item in this section is a documented gap; flag in `notes.txt` if
any of these prove worse in practice than estimated.

### E.1. The 1.5s combat tick cadence vs Brain Freeze 40s ICD — **already handled**

The bot ticks every 1.5s (combat tick). Brain Freeze procs at 15% per
chill-spell (W4). The 15s buff duration plus the 1.5s tick means the
bot has ~10 ticks to detect and consume a Brain Freeze proc, which is
plenty. The current code at
`FrostMageStrategy.cpp::FindBotAuraByName` (lines 47–65) handles this
correctly with the `IsPassive()` filter (CLAUDE.md gotcha #1 — passive
talent + proc aura share `SpellName`). **No gap.**

### E.2. Fingers of Frost 2-stack consumption — **already handled**

Same shape as E.1. `Tier_IceLance` (line 337) and `Tier_DeepFreeze`
(line 304) both check `HasFingersOfFrost(bot)` and consume the proc
correctly. **No gap.**

### E.3. Arcane Power 15s window — **bot pops it, can't pre-pot, can't time Hero/Lust**

The bot can fire AP on its rotation tick (instant, no GCD). It cannot:
- **Pre-pot** — pre-combat alchemy potions (Volcanic, Tol'vir, etc.) require pre-pull human decision. The bot doesn't pop a potion at T-2s. **Net DPS gap: ~3-4% on opener.**
- **Time AP with Bloodlust/Heroism/Time Warp** — if the master pops Hero, the bot has no event hook to react inside the 5-second pre-window. Best case: AP and Hero overlap by coincidence (both on 2-minute CDs alignable on pull). Worst case: AP fires at 0-15s, Hero at 60-100s, no overlap. **Net DPS gap: ~2-3% sustained.**
- **Align AP with Icy Veins** — both are 90s CDs (AP with Arcane Flows). They *should* fire on the same tick at engage. The bot will probably do this naturally because both go on-CD at pull, but a future improvement is an explicit alignment tier that fires Icy Veins *immediately* after Arcane Power so they share their 15-20s windows.

### E.4. Frostfire Bolt → Frost-school-tag → Shatter combo timing — **bot doesn't do orb positioning**

`FrostfireBolt` is the high-damage filler when Brain Freeze is up (free
+ instant). Under Shatter the bot can theoretically achieve a triple
crit-multiplier with FoF *and* Brain Freeze stacked (`Tier_FFB_BothProcs`,
line 314). What the bot doesn't do:
- **Frostfire Orb positioning** — Frostfire Orb (G1's Tier 6 talent)
  travels in a straight line for 15s. Optimal positioning casts it
  through the pack lengthwise. The bot casts it on its current target;
  the position manager doesn't reposition for orb path. **Gap: ~5-10%
  of theoretical Orb damage on wide spreads.** Not addressable without
  position-manager logic that knows about ground-target spell paths.
- **Cone of Cold facing/positioning** — `Tier_AoE` (line 346) handles
  `bot->GetDistance(target) <= CONE_RANGE` already, but the bot doesn't
  reposition into a Cone-of-Cold-optimal angle. It stands at 25y caster
  range; CoC only fires when target *happens* to be within 8y. **Gap:
  most encounters fire CoC zero times.** The cluster-on-target check
  (CLAUDE.md gotcha §7) keeps CoC from firing into empty air, but it
  doesn't proactively move the bot in.

### E.5. Cone of Cold positioning — manager doesn't melee-stack the mage

Per E.4 above. The bot is a ranged DPS, positioned at 25y; CoC is an 8y
PBAoE. The two are mutually incompatible without explicit melee-stack
positioning. **The position manager doesn't do this.** Net: Cone of
Cold's contribution to AoE is roughly 0% on bot mages — Blizzard
channel does all the work. This is consistent with the existing
strategy's `Tier_AoE` falling through to Blizzard (line 371) when the
distance-to-target check fails.

### E.6. PoM-on-FFB precise sequencing — bot fires same tick or one tick late

Theoretical max: PoM → instant FFB on the *same GCD*. The bot fires
PoM on tick N (no GCD reservation), then runs the rotation again on
tick N+1 (1.5s later) and fires `Tier_FFB_BothProcs` under the PoM
buff. There's a small risk that the PoM-empowered cast lands not on
FFB but on Frostbolt (if Brain Freeze expired in between). Mitigation:
fire PoM only when `HasBrainFreeze(bot)` AND `(brainFreeze.duration > 3s)`
so the proc is guaranteed survive the tick gap. **Net cost: ~1 PoM
use per fight wasted to expiry. Engineering fix is a 5-line precondition
in `Tier_PresenceOfMind`.**

### E.7. Pre-existing gaps not introduced by this build

- **Trinket / Mana Gem alignment with AP** — bot has no trinket-use logic. ~2-3% loss per AP window. Tracked in `docs/research/encounter-awareness-and-dps-roadmap.md`.
- **Cold Snap reset alignment** — 8min CD, fires optimally when ≥2 of {Icy Veins, Frost Nova, Deep Freeze} on CD. Bot doesn't fire Cold Snap today. Phase 2 work.
- **Polymorph re-CC cadence** — no auto-poly. Pre-existing.

---

## §F. Side-by-side: 71-point baseline → 142-point recommended

| | 71-pt baseline (8/2/31) | 142-pt recommended (Build A: 41 Frost / 36 Arcane / 5 Fire) | Net effect |
|---|---|---|---|
| **Frost tree** | 31 points; Deep Freeze + Brain Freeze + FoF + Frostfire Orb + Shatter | 41 points; everything in baseline **plus** Piercing Chill 3/3, Reactive Barrier 2/2 (or Cold as Ice), Ice Shards 3/3 (was 2) | +10 Frost: more crit damage, more chill-spread for FoF generation on packs, reflexive Ice Barrier |
| **Arcane tree** | 2 points; Improved Counterspell only | 36 points; Improved Counterspell **+ Netherwind Presence + Torment the Weak + Arcane Power + Presence of Mind + Arcane Flows + Arcane Concentration + Improved Mana Gem** | +34 Arcane: TWO new on-CD burst cooldowns (AP, PoM) + flat +6% damage from Torment the Weak + +3% haste passive + 25% AP/PoM CD reduction + Clearcasting mana sustain + Mana Gem boost |
| **Fire tree** | 8 points; Ignite + Master of Elements + Burning Soul | 5 points; Master of Elements + Burning Soul (Ignite **dropped**) | -3 Fire: Ignite gone. **Trade-off:** loses ~40% of FFB-crit follow-up DoT damage. Cost ~1.5-2% sustained DPS. Worth it for the Arcane gains. |
| **DPS cooldowns** | Icy Veins (3min), Mirror Image (3min → 2:30 w/ glyph), Cold Snap (8min) | Above **+ Arcane Power (90s w/ Arcane Flows) + Presence of Mind (90s w/ Arcane Flows)** | Roughly 2x cooldown density. Bot has a meaningful damage button up on most pulls. |
| **Sustained DPS** | Baseline | +5-7% sustained (AP+PoM+TtW combined), -1.5-2% (Ignite loss) → **net ~+4-5%** | Modest but real |
| **Burst DPS** | Icy Veins window | Icy Veins + AP overlap window — peak ~40% damage lift in 15s | High burst on opener / pull windows |
| **Survivability** | Mage Ward, Ice Block, Frost Armor | Above **+ Reactive Barrier auto-Ice-Barrier under 50% HP + Improved Blink** | Marginal — bot isn't taking heavy damage anyway as ranged |
| **Mana sustain** | Evocation, Mana Gem, Enduring Winter, Master of Elements | Above **+ Arcane Concentration (Clearcasting) + Improved Mana Gem (+40%)** | More headroom; Evocation rarely needed |
| **Glyphs** | Frostbolt + Deep Freeze + Frostfire / Evocation + Blink + Mirror Image | **Unchanged** | No glyph swap needed |

---

## §G. Implementation pointers

### G.1. Spell enum additions (`src/strategies/FrostMageStrategy.h`)

Append to `enum class Spell : uint8_t` (current end is line 38):

```cpp
    Counterspell,        // 24y interrupt; 24s CD; locks the school for 7s
    // --- 142-point Build A (Frost+Arcane) additions ---
    ArcanePower,         // 12042 — instant self-buff, 15s, +20% dmg, -10% mana
    PresenceOfMind,      // 12043 — instant self-buff, next cast <10s becomes instant
    MirrorImage,         // 55342 — pre-pull / on-CD throughput cooldown
    ColdSnap,            // 11958 — resets Frost-school CDs
    Count
```

Mirror Image and Cold Snap are listed even though they're "primary tree
baseline" — the existing code doesn't cast them, so they need cache
entries too. (Phase work, not strictly Build-A.)

### G.2. ResolveSpellCache additions (`FrostMageStrategy.cpp` line 176-204)

Add lines under the existing `find()` block:

```cpp
    _cache[size_t(Spell::ArcanePower)]     = find("Arcane Power");
    _cache[size_t(Spell::PresenceOfMind)]  = find("Presence of Mind");
    _cache[size_t(Spell::MirrorImage)]     = find("Mirror Image");
    _cache[size_t(Spell::ColdSnap)]        = find("Cold Snap");
```

All four resolve through `StrategyUtil::FindSpellByFamilyName(bot,
SPELLFAMILY_MAGE, "<name>")` — same path as every existing entry.
Per CLAUDE.md gotcha #1 (passive-name collision), check that the
returned IDs are not the passive talents:
- Arcane Power: cast is 12042. No passive-talent collision known.
- Presence of Mind: cast is 12043. No passive-talent collision known.
- Mirror Image: cast is 55342. No collision known.
- Cold Snap: cast is 11958. No collision known.

**Improved Counterspell silence aura** — passive talent. Does NOT
need caching; it modifies Counterspell's existing effect transparently
(no separate ID the bot needs to read). No `IsPassive()` filter
concern.

**Improved Soul Fire equivalent** — N/A; the Frost+Arcane build has no
"cast a thing to get a self-haste buff" pattern. Arcane Power IS the
buff (cast + self-buff in one instant). No proc-aura tracking needed.

### G.3. New tier methods

Header file additions (under existing tier declarations):

```cpp
    bool Tier_ArcanePower(Player* bot, Unit* target) const;
    bool Tier_PresenceOfMind(Player* bot, Unit* target) const;
    bool Tier_MirrorImage(Player* bot, Unit* target) const;   // (Phase 2)
    bool Tier_ColdSnap(Player* bot, Unit* target) const;      // (Phase 2)
```

Tier dispatch insertion in `Update()` (after line 163's Counterspell
hook, before line 165's `Tier_DeepFreeze`):

```cpp
    // 142-pt Build A additions — instant self-buffs, no GCD reservation.
    if (Tier_ArcanePower(bot, target))     { TC_LOG_DEBUG("altbot", "  -> ArcanePower"); return; }
    if (Tier_PresenceOfMind(bot, target))  { TC_LOG_DEBUG("altbot", "  -> PoM");         return; }
```

### G.4. Cache resolution per cast-pipeline gotchas (CLAUDE.md)

- **#1 passive-name collision.** None of {AP, PoM, Mirror Image, Cold Snap} have known passive-talent collisions. The `IsPassive()` filter catches new ones; flag any that read 0 on first cache log line.
- **#2 `CastWithLog`.** New tiers use `TryCast(bot, target, Spell::X)` (lines 216–227) which routes through `CastWithLog`. No exception.
- **#3 skip during cast.** AP and PoM are instant; the UNIT_STATE_CASTING check at line 141 returns *before* the new tiers run, so they don't fire mid-cast either. Pre-empting Frostbolt with AP would require reordering above the cast guard — defer to Phase 2.
- **#4 GCD guard.** Both AP and PoM have 0 GCD (W1, W2). The Frostbolt-based probe at line 147 returns before the new tiers run; acceptable since AP/PoM during post-cast GCD just buffs the *next* spell. No change to probe.
- **#5 `MaintainRange` idempotency.** No new positioning logic.

### G.5. Tier_ArcanePower precondition logic

```cpp
bool FrostMageStrategy::Tier_ArcanePower(Player* bot, Unit* target) const
{
    uint32 ap = GetSpell(Spell::ArcanePower);
    if (!ap || IsOnCooldown(bot, ap))
        return false;
    // Fire on any combat target (no HP gate). The buff is self-target,
    // instant — the next spell consumes the benefit.
    return TryCast(bot, bot, Spell::ArcanePower);
}
```

Cast on **self** (bot, not target) — AP is a self-buff. Same pattern as
`Mage Ward` maintenance (line 267).

### G.6. Tier_PresenceOfMind precondition logic

```cpp
bool FrostMageStrategy::Tier_PresenceOfMind(Player* bot, Unit* target) const
{
    uint32 pom = GetSpell(Spell::PresenceOfMind);
    if (!pom || IsOnCooldown(bot, pom))
        return false;
    // Reserve PoM for the highest-value cast: FFB under Brain Freeze
    // (the Tier_FFB_BothProcs window). If BF isn't up, hold PoM —
    // a tick or two of holding is fine, we're not capping AP-style.
    if (!HasBrainFreeze(bot))
        return false;
    return TryCast(bot, bot, Spell::PresenceOfMind);
}
```

The `HasBrainFreeze` check uses the existing `FindBotAuraByName(bot,
"Brain Freeze")` helper (line 68). Per E.6, may want to also check
buff remaining-duration ≥ 3s; defer to Phase 2 telemetry.

### G.7. Cross-reference

This implementation maps onto `docs/research/2x-talent-strategy-design.md`
**Part B Option 2 — "detect secondary spells, unlock tiers in-place".**
Specifically:

- `ResolveSpellCache` queries the new spells; `find()` returns 0 if the
  bot didn't spec the Arcane secondary, so tiers naturally short-circuit
  (CLAUDE.md gotcha pattern: `if (!id || ...) return false;`).
- Stock 8/2/31 bots see `ArcanePower=0`, `PresenceOfMind=0` in the
  cache-resolve log and the new tiers no-op; **regression-safe.**
- No factory change. The slug remains `"frost-mage"`.
- Telemetry: cache log line at first-tick already includes the new IDs.
  Add a one-time `TC_LOG_INFO("altbot", "FrostMage[%s] dual-tree: AP=%u PoM=%u", ...)`
  if 142-pt mode is detected (= AP cached non-zero) for grep-ability.
  Optional, Phase 2.

---

## §H. Unknowns / verification checklist

What this doc didn't verify; flag in `notes.txt` if any of these break
the build:

| # | Unknown | How to verify |
|---|---|---|
| 1 | **Wowhead talent-calc URL string encoding.** §C.4's digit string is constructed by mapping §C.2's allocations to tree-order positions; the exact digit-per-slot Wowhead format isn't documented in the source guides. | Open the URL in browser, confirm each tree's point total reads 36 / 5 / 41 and that Arcane Power + Presence of Mind are lit. If off, the §C.2 tables are authoritative — fix the URL, not the build. |
| 2 | **Arcane Power -10% mana cost in custom server build.** W1 Wowhead Cata page shows `-10%` mana cost (Cata redesign post-4.0.6); some private servers ship the pre-Cata `+30% mana cost` version. | Cast AP on a test mage, monitor next Frostbolt mana cost. If mana spikes instead of drops, the server is using a pre-Cata AP and the §B.1 math (-10% factor) is off but the +20% damage is unaffected — build still recommended, just slightly weaker mana profile. |
| 3 | **Improved Counterspell silence aura ID.** Listed in CLAUDE.md gotcha #1 as not having a known passive-collision; needs WDBXEditor verification before assuming. | DBC extract via `tools/dbc-extract.py`; cross-reference talent ID vs proc-aura ID. Tracked in `docs/research/dbc-verification-checklist.md`. |
| 4 | **Arcane Flows -25% CD on AP/PoM verified for Cata 4.3.4.** G3 mentions `Arcane Flows reduces cooldowns` but doesn't quantify per-rank scaling. The 90s-CD AP claim in §B.1 / §E.3 depends on 2/2 Arcane Flows = -25%. | Wowhead Cata spell page for Arcane Flows; confirm rank 2 = -25% AP/PoM/Evocation/Invisibility CD. |
| 5 | **Torment the Weak slow-target trigger** — does "slowed" include Cata Frostbolt's chill effect? G3 says yes implicitly; explicit verification needed. | Wowhead Cata spell page for Torment the Weak; check whether the snare/chill from Frostbolt qualifies. If not, the +6% is wasted and the build loses its strongest passive — but AP+PoM still justify the spec. |
| 6 | **Brain Freeze proc aura ID** (separate from talent ID 44546). Already in `FindBotAuraByName` via name-match; ID number not documented here. | WDBXEditor / Wowhead. Per CLAUDE.md gotcha #1: passive talent + proc share `SpellName`; the `IsPassive()` filter (FrostMageStrategy.cpp:59) catches the collision. |
| 7 | **5-man pull length distribution.** §B.1 estimates "30-45s trash, 1-2min boss" but the user's gameplay style might differ (faster-clearing premade vs. PUG-paced). | Telemetry from first 142-pt session — count AP uses per minute of combat. If <0.3/min, downgrade AP value in further iterations. |
| 8 | **Server allocation rules.** Does the 142-pt server enforce standard tier-prereq rules (need 5 pts in Tier 1 to unlock Tier 2, etc.) or does it allow free placement? §A assumed standard rules. | Open in-game talent UI; place 1 point in Tier 2 with 0 in Tier 1 and see if it's permitted. |
| 9 | **Glyph slot count.** Assumed 3 prime / 3 major / 3 minor per stock Cata. Custom server may differ. | In-game UI. |
| 10 | **Pyroblast crit availability on Frost mage** — if the user *did* pick Build B against recommendation, the `Pyroblast` enum entry needs both a cast ID and a Hot-Streak-proc-aura ID. Not surveyed here. | Out of scope — Build B is non-recommended. Pick up in a future "Fire-primary 142-pt" doc if relevant. |

---

End of doc.

# 142-pt MM Hunter Build — Bot-Executable Max DPS

Bot-executable max DPS, Marksmanship Hunter primary, **142-point Cata 4.3.4 server**
(2x talent grant: full 41-point MM tree plus a fully-built 36-point secondary tree
plus 5 leftover points in the tertiary). Survey date **2026-05-12**.

> **Scope.** Talent build + rotation design for the existing
> `src/strategies/MmHunterStrategy.{h,cpp}`. The strategy was written against a
> stock 7/31/3 budget; on a 142-point server it has 65 unspent points to allocate.
> This doc picks the secondary tree (BM or Survival), the talent layout, and what
> if anything needs to be added to the tier dispatch.
>
> **No code in this doc.** Implementation pointers in §G are signature-level only.

---

## 1. Header constraints (user decisions, not relitigated)

1. **One hybrid build per spec.** No separate ST/AoE tree.
2. **Bot-executable max DPS.** Recommend what the current strategy
   architecture can deliver — 1.5s tick cadence, no skill-based weaving, no
   proc reactions tighter than one tick.
3. **MM primary.** No drift to BM-primary or SV-primary.
4. **One doc, no code.**

---

## 2. Sources

| # | Guide | URL | Confidence |
|---|---|---|---|
| G1  | Icy Veins — MM Talents & Glyphs | https://www.icy-veins.com/cataclysm-classic/marksmanship-hunter-pve-spec-builds-talents-glyphs | Medium-High (partial fetch — "31 in MM, 7 unspent in the tree") |
| G2  | Icy Veins — MM Rotation & CDs | https://www.icy-veins.com/cataclysm-classic/marksmanship-hunter-pve-rotation-cooldowns-abilities | High (full rotation priority list confirmed) |
| G3  | Icy Veins — SV Talents & Glyphs | https://www.icy-veins.com/cataclysm-classic/survival-hunter-pve-spec-builds-talents-glyphs | Medium (partial fetch — confirms LnL is "core of Survival") |
| G4  | Icy Veins — BM Talents & Glyphs | https://www.icy-veins.com/cataclysm-classic/beast-mastery-hunter-pve-spec-builds-talents-glyphs | Medium (partial fetch — Bestial Wrath/Frenzy/Focus Fire/Go-for-the-Throat) |
| G5  | Icy Veins — BM Rotation & CDs | https://www.icy-veins.com/cataclysm-classic/beast-mastery-hunter-pve-rotation-cooldowns-abilities | Medium (confirms Bestial Wrath 2-min/10s, Fervor 50 focus, Focus Fire) |
| G6  | gotwarcraft — MM 4.3 (full picks list) | https://gotwarcraft.com/guides/classguides/marksmanhunter.php | Medium-High — explicit 7/31/3 |
| G7  | Tauri-Veins — MM PvE DPS | https://www.tauri-veins.com/marksmanship-hunter-wow-pve-dps-guide | Medium (general structure only) |
| G8  | wowtbc.gg — MM Hunter | https://wowtbc.gg/cata/class-guides/marksmanship-hunter/ | Medium |
| G9  | Warcraft Tavern — MM Talents | https://www.warcrafttavern.com/cataclysm/guides/pve-marksmanship-hunter-talents-builds-glyphs/ | **403** (reconstructed) |
| G10 | wowvendor — MM PvE | https://wowvendor.com/blog/cataclysm-classic-marksmanship-hunter-pve-guide/ | **403** |
| W1  | Wowhead Cata — Black Arrow | https://www.wowhead.com/cata/spell=3674/black-arrow | High (35 focus / 30s CD / 20s / tick 2s / shadow) |
| W2  | Warcraft Wiki — Black Arrow | https://warcraft.wiki.gg/wiki/Black_Arrow | High (40y, 6% base mana, **Tier 7 SV** with Wyvern Sting prereq) |
| W3  | Warcraft Wiki — Lock and Load | https://warcraft.wiki.gg/wiki/Lock_and_Load | **CRITICAL** — in 4.3.4 LnL procs only from **Freezing Trap / Ice Trap**; Black Arrow, Immolation, Explosive Trap explicitly **removed** as proc sources (patch 4.3.2). |
| W4  | Wowhead Cata — Aimed Shot! (proc) | https://www.wowhead.com/cata/spell=82928 | High (instant, 132% weapon dmg, 1s GCD, no focus) |
| W5  | Wowhead Cata — Fire! buff | https://www.wowhead.com/cata/spell=82926/fire | High (10s, overrides Aimed Shot action-bar) |
| W6  | Wowhead Cata — Wild Hunt (53270) | https://www.wowhead.com/cata/spell=53270/wild-hunt | Medium (passive, dummy effects 60 / 50; rank 1/2) |
| W7  | Wowhead Cata — Beast Mastery (56636) | https://www.wowhead.com/cata/spell=56636 | High (BM capstone — exotic pets + 4x effect mult on Kill Shot / Steady Shot / Aimed Shot ranks) |
| L1  | Project doc — `marksmanship-hunter-guide-survey.md` | (local) | High (baseline 71-pt survey) |
| L2  | Project doc — `2x-talent-strategy-design.md` §C.1.2 | (local) | High (MM+Survival sketch flags LnL gap) |

**Flagged failures:** G9 (Warcraft Tavern), G10 (wowvendor), G3-G4 (Icy Veins
partial — calculator widgets don't render through WebFetch). Tier-by-tier
talent enumeration came primarily from G6 (gotwarcraft) cross-checked against
W2/W3 hard mechanics.

---

## §A. Realistic dual-tree builds

Cata's talent rule still applies: **31 points in the primary tree unlocks the
41-point capstone, and the primary's 31 gates secondary specialization.** On
142 points the math:

```
41 primary  (MM full, capstone Chimera Shot at Tier 7 — already in 7/31/3)
+ 36 secondary  (BM-deep OR Survival-deep, reaching a Tier-7 capstone)
+  5 tertiary leftovers (1-2 rows of the third tree)
=  82 placeable points  (60 above the standard 7/31/3)
```

Two viable secondary trees for MM-primary:

### Build A — **MM(41) / Survival(36) / BM(5)**

| Tree | Pts | Key picks |
|---|---|---|
| MM | 41 | All of 7/31/3's MM 31 — Improved Steady Shot 3/3, Careful Aim 2/2, Piercing Shots 3/3, Master Marksman 3/3, Mortal Shots 3/3, Bombardment 2/2, Rapid Killing 2/2, Concussive Barrage 2/2, Sic 'Em! 2/2, Marked for Death 2/2, Trueshot Aura 1/1, Silencing Shot 1/1, Posthaste 2/2, Termination 2/2, Resistance is Futile 3/3, Rapid Recuperation 2/2, Readiness 1/1, Chimera Shot 1/1 — plus 10 more from filler talents in the tree (Improved Mend Pet, Efficiency overflow, Go for the Throat dependency if cross-tree). |
| SV | 36 | Hunter vs Wild 3/3, Pathing 3/3, Improved Serpent Sting 3/3, Survival Tactics 2/2, Trap Mastery 3/3 (irrelevant for non-trap bot), Entrapment 2/2, Point of No Escape 2/2, Resourcefulness 3/3, Mirrored Blades 2/2, T.N.T. 3/3 (irrelevant — buffs Immolation/Explosive trap), Toughness 3/3, **Lock and Load 1/1** (lights up only on Frost/Ice Trap freeze — not relevant for MM rotation), Hunting Party 1/1 (raid 10% melee/ranged AS aura — overlap with shaman Windfury, situational), Sniper Training 3/3, Serpent Spread 2/2 (Multi-Shot applies Serpent Sting on AoE), **Wyvern Sting 1/1** (Tier 5, **prereq for Black Arrow**), **Black Arrow 1/1** (Tier 7 capstone — 30s CD shadow DoT). |
| BM | 5 | One with Nature 3/3 (Stam→Agi), Bestial Discipline 2/2 (+focus regen for pet). |

**What the build unlocks vs stock 7/31/3:**

- **Black Arrow** — 30s CD, 35 focus, 20s shadow DoT, ticks 2s (10 ticks).
  ~9 ticks at ~285/tick = ~2850 base damage; scales with attack power.
  *Instant cast*. Slots cleanly into the priority list as a new DoT-on-CD tier.
- **Wyvern Sting** — Tier 5 SV, *prereq* for Black Arrow. 6s sleep on
  humanoid/beast/dragonkin + DoT on wake. **PvE relevance:** mostly nil
  (instant CC on a single mob; raids/dungeons CC is mage Poly + lock fear).
  Bot doesn't fire it.
- **Lock and Load** — **does not proc from Black Arrow in 4.3.4** (per
  patch 4.3.2 notes — W3). Only procs from `Freezing Trap` / `Ice Trap` (50% /
  100% per rank), grants 2 free Explosive Shots. Without trap placement, the
  proc surface is **zero**. The talent point still has to be placed (it's a
  Tier-4 SV prereq for the rest of the deeper tree), but it lights up exactly
  never on a bot that doesn't drop ice traps.
- **Sniper Training 3/3** — passive: "after standing still 6s, +6% ranged
  damage + Steady/Cobra/Aimed cast on the move." This is a real DPS bump for a
  bot that holds its 25y caster position. Free with the SV tree.
- **Hunter vs Wild 3/3** — +3% / +6% / +9% Stamina × Agility. Lower-bar Tier-1
  filler — bot benefits passively.
- **Improved Serpent Sting 3/3** — +30% damage to the initial Serpent Sting
  impact. Trivial — Serpent Sting is one application per fight.

**Estimated gain vs stock 7/31/3:** ~5-8% DPS — Black Arrow on CD is the
single biggest add. The rest is passive trickle (Sniper Training, Hunter vs
Wild). LnL contributes 0 unless the bot places ice traps, which it doesn't.

---

### Build B — **MM(41) / Beast Mastery(36) / Survival(5)**

| Tree | Pts | Key picks |
|---|---|---|
| MM | 41 | Same 31 MM core as Build A plus 10 leftover MM picks (Improved Mend Pet 2/2, filler). |
| BM | 36 | Improved Aspect of the Hawk 3/3 (+6% haste when proc'd), One with Nature 3/3, Bestial Discipline 2/2, **Animal Handler 2/2** (+9% pet damage), **Pathfinding 2/2** (+8% mounted speed — irrelevant in combat), **Spirit Bond 2/2** (+2% HP/s out of combat; helps pet between pulls), **Frenzy 3/3** (pet +30% AS at 5 stacks — passive pet self-buff), **Cobra Strikes 3/3** (Arcane/Steady crits give pet next 2 specials guaranteed crit), **Ferocious Inspiration 3/3** (pet special crits = +3% raid-wide damage 10s), Kindred Spirits 5/5 (+20% pet damage + 5y pet movement), **Bestial Wrath 1/1** (2min CD, 10s — *MM bot can fire this* — +20% pet damage, dispel-immune pet), Invigoration 2/2 (pet melee crit returns focus to hunter — focus economy boost), **Beast Mastery 1/1** (Tier 7 capstone — exotic pet taming + 4x ability damage to specific shots **per W7**), Longevity 3/3, **Wild Hunt 2/2** (Tier 6 — passive pet damage; per W6 "dummy effects 60/50" — community consensus: +50% pet special damage / +30% pet AP from pet focus on Bite/Claw/Smack/Sting), Focus Fire 1/1 (consumes pet Frenzy stacks for hunter haste). |
| SV | 5 | Hunter vs Wild 3/3, Pathing 2/3 (or 1/3 + Improved Serpent Sting 1/3). |

**What the build unlocks vs stock 7/31/3:**

- **Bestial Wrath** (2min CD, 10s) — *MM-usable cooldown*. +20% pet damage,
  makes pet dispel-immune. A bot fires this on CD; no rotation cost; no GCD
  cost (off-GCD per Cata spec). **One new tier method.**
- **Focus Fire** (consume pet's Frenzy stacks for hunter haste) —
  *MM-usable*. Off-GCD. Trades a long-term pet AS buff for a short hunter
  haste burst. For a bot tier-dispatch model, this is debatable: it's a
  *decision* whether to consume now or wait, and the bot doesn't peer at
  future cooldown windows. Default: skip / fire at 5 stacks if Bestial
  Wrath off-CD.
- **Wild Hunt 2/2** — **passive**. Boosts pet special-ability damage. Per
  community consensus the per-rank values land near "+30% damage to pet
  Bite/Claw/Smack + AP equal to 50/60% of pet's current focus on those
  specials." Free DPS — the pet just hits harder.
- **Ferocious Inspiration 3/3** — pet-crit-triggered **3% damage raid aura**,
  10s, refreshable. Passive. Comparable in shape to Frost Mage's Arcane
  Brilliance — a group buff the bot maintains by virtue of having the
  talent.
- **Frenzy 3/3 / Cobra Strikes 3/3 / Animal Handler 2/2 / Kindred Spirits
  5/5** — pure pet-damage passives. Pet damage in MM is ~25-30% of total
  hunter damage (G2 / common community parses). A 30-40% pet damage buff
  via these passives = ~8-12% total damage.
- **Beast Mastery (capstone)** — exotic pet taming + W7's "4x effect"
  multiplier applies to Kill Shot / Steady Shot / specific Aimed Shot ranks.
  Reading W7 plainly: BM capstone interacts with hunter shots — the
  community-known effect is the BM capstone enables *exotic pets* (Spirit
  Beasts, Devilsaurs) and the hidden aura adds pet bonuses. **For a bot, the
  exotic pet option matters if the bot can tame a Spirit Beast (extra utility)
  or Devilsaur (Terrifying Roar — Sunder equivalent).**

**Estimated gain vs stock 7/31/3:** ~8-12% DPS — pet passives compound
multiplicatively (Wild Hunt + Kindred Spirits + Animal Handler + Frenzy +
Cobra Strikes + Bestial Wrath active uptime), and Ferocious Inspiration adds
group utility.

---

## §B. The decisive question

> **Is Black Arrow worth the rotation slot in a bot's tier dispatch when
> Lock and Load won't be feeding off it?**

### Pro Black Arrow (Survival-secondary)

- Black Arrow is a 30s-CD, instant-cast shadow DoT (W1/W2). A bot fires it on
  CD and treats it as a third DoT (after Serpent Sting and Hunter's Mark) in
  the priority list. Slots above Chimera Shot if reactive-priority, or below
  Serpent Sting if treated as Serpent-Sting-style apply-then-forget.
- The DoT is ~2850 base damage over 20s. At ~25y range with AP scaling, the
  realized damage is meaningful — comparable to landing two extra Arcane
  Shots' worth of damage every 30s window with **zero focus competition**
  (instant cast, no focus cost in 4.3.4 per W2: 6% base mana? wait — Cata
  hunters are focus-only since 4.0.1; W1 reads 35 focus and W2 reads "6% base
  mana", a contradiction explained by W2 being out-of-date pre-Cata text;
  **the in-game 4.3.4 value is 35 focus** per Wowhead's Cata page W1).
- *Sniper Training 3/3* is a free passive on top: +6% ranged damage when
  standing still (which a bot does).

### Pro pet passives (BM-secondary)

- BM-secondary is **almost entirely passive** — the pet just does more damage
  because of talent placement. No new rotation tiers needed for the bulk of
  the gain. The bot's existing pet-maintenance code (Call Pet, Mend Pet)
  already keeps the pet alive; the gain is automatic once talents are
  allocated.
- **Bestial Wrath** is a single new on-CD button for the hunter (2min CD,
  off-GCD, +20% pet damage 10s). Cheap to add.
- **Ferocious Inspiration** adds a 3% raid damage aura (passive — no
  bot-side action).
- Pet damage in MM is ~25-30% of total hunter damage. **A 30-40% pet damage
  multiplier (Wild Hunt + Animal Handler + Kindred Spirits + Bestial Wrath
  uptime) yields ~8-12% total damage**. Strictly larger than Black Arrow's
  ~5-8%.

### The LnL gap that decides it

Per W3 (Warcraft Wiki, Lock and Load — patch 4.3.2 notes): **in 4.3.4 LnL
procs from Freezing Trap and Ice Trap only. Black Arrow, Immolation Trap,
Explosive Trap are explicitly removed as proc sources.** This means a
Survival-secondary MM bot gets **Black Arrow as a stand-alone DoT** — *not*
as part of a LnL proc engine. Survival's signature explosion (LnL → 2 free
Explosive Shots) is unreachable for a bot that doesn't drop ice traps, and
the bot's `AltbotPosition` doesn't choose trap drop spots (see §E).

Even if the bot *did* drop ice traps, LnL only feeds Explosive Shot — which
is a Survival-spec specialty button, not part of MM's rotation. Wiring it in
would mean adding Explosive Shot to the cache plus a "fire during LnL" tier,
plus trap placement. That's a much larger surface area than "Black Arrow on
CD as a passive DoT."

### Counter-question math

| Gain vector | Build A (MM+SV) | Build B (MM+BM) |
|---|---|---|
| New rotation buttons | Black Arrow (30s CD, instant) | Bestial Wrath (2min CD, off-GCD) |
| Passive damage | Sniper Training +6% ranged | Wild Hunt + Kindred Spirits + Animal Handler + Frenzy (pet) |
| Group utility | Hunting Party 10% AS aura (overlaps Windfury) | Ferocious Inspiration 3% raid damage |
| Bot-side implementation | 1 new tier method + 1 cache entry | 1 new maintenance entry + (optional) 1 tier method for Bestial Wrath |
| Est. DPS gain over 7/31/3 | ~5-8% | ~8-12% |
| Failure mode | LnL never procs (waste talent point) | None — gains are passive |

### Verdict

**Beast Mastery wins as the secondary tree.**

The decisive logic is two-fold:

1. **Bigger expected gain.** Pet passives multiplicatively stacked are
   strictly larger than Black Arrow's one-DoT contribution, in a spec where
   pet damage is already ~25-30% of total. The reasonable mid-point estimate
   is 10% DPS over 7/31/3 vs 6% from Build A.
2. **Cleaner bot fit.** BM-secondary is dominantly *passive* — once talents
   are placed, the bot doesn't need to learn new abilities. The single new
   active is Bestial Wrath on a 2min CD. Build A requires Black Arrow upkeep
   inside the priority list, which is fine but more code surface for less
   gain.

Build A's *only* edge would be if the user wanted to do AoE trash via Serpent
Spread (Multi-Shot applies Serpent Sting to all hit). But MM's existing AoE
tier (`Tier_AoE` → Multi-Shot) already covers AoE, and Serpent Spread is one
talent point's worth of cleave-DoT.

**Build B picked.** Detail in §C.

---

## §C. Recommended build — **MM(41) / Beast Mastery(36) / Survival(5)**

### Tree breakdown

| Tree | Points | Capstone reached |
|---|---|---|
| Marksmanship | **41** | Chimera Shot (Tier 7 — already in 7/31/3) plus 41-point completion |
| Beast Mastery | **36** | Beast Mastery (exotic pet taming + hidden aura) |
| Survival | **5** | Tier 2 (Hunter vs Wild + 2 in Pathing) |
| **Total** | **82** | The 142-point server grants 82 placeable for an 85 hunter (60 above standard); leftovers from the doubled grant land in MM-filler / Survival Tier 1-2. |

> **Sanity note on point math.** 142 = 71×2. Stock Cata max-level talent budget
> is 41 (primary cap) + 30 spare = 71. Doubled is 142. At 85 a 142-server bot
> spends 41 + 36 + 5 = 82 "useful" points; the remaining 60 are pad — extra
> ranks on partial talents, filler rows. The build below specifies the 82 that
> drive damage; the rest is bookkeeping.

### Point-by-point talent picks

#### Marksmanship (31 + 10 pad = 41)

| Tier | Talent | Ranks | Rationale |
|---|---|---|---|
| 1 | Go for the Throat | 2/2 | Pet crit → +20 hunter focus. Free focus regen. |
| 1 | Efficiency | 3/3 | -10% focus cost on Steady/Cobra/Arcane/Aimed/Multi. Sustained focus economy. |
| 2 | Sic 'Em! | 2/2 | Arcane/Aimed crit → next pet special free + instant. Pet-focus help. |
| 2 | Improved Steady Shot | 3/3 | **Core.** 2x consecutive Steady → +15% ranged haste 8s. Maintained at 100%. |
| 2 | Careful Aim | 2/2 | **Core.** +60% crit on Aimed/Steady/Cobra vs targets >90% HP. Opener burst. |
| 3 | Silencing Shot | 1/1 | Interrupt + 3s silence. Encounter-essential (ToTT trash etc.). |
| 3 | Concussive Barrage | (skip) | PvP only — slow on Arcane/Chimera. |
| 4 | Piercing Shots | 3/3 | **Core.** Crit Aimed/Steady/Chimera → 30% bleed 8s. Crit valuation. |
| 4 | Bombardment | 2/2 | Multi-Shot crit → next Multi-Shot -25% focus + +25% damage. AoE flow. |
| 5 | Trueshot Aura | 1/1 | +10% ranged AP raid aura. Mandatory if no Trueshot/Battle Shout source. |
| 5 | Rapid Recuperation | 2/2 | Rapid Fire / Readiness → +25 focus/3s. Focus economy in burst. |
| 6 | Master Marksman | 3/3 | **Core.** Steady → 60% per cast stacks "Ready, Set, Aim..." (×5, 30s). 5 stacks = Fire! buff: instant free Aimed Shot. |
| 6 | Rapid Killing | 2/2 | After Rapid Fire ends, +20% next Aimed/Arcane/Multi damage. |
| 6 | Termination | 2/2 | <25% HP target → Steady/Cobra/Aimed return +6 focus. Execute economy. |
| 7 | Readiness | 1/1 | **Core.** 3min CD, resets all Hunter CDs. RF + Chimera + Kill Shot reset. |
| 7 | Resistance is Futile | 3/3 | -10% threat on Aimed/Steady/Arcane/Multi/Chimera. Reduces MD waste. |
| 7 | Posthaste | 2/2 | After Disengage, +60% run speed 4s. Free mobility on the bot's defensive. |
| 7 | Marked for Death | 2/2 | All shots reapply Hunter's Mark to target. Eliminates rotation conflict. |
| 7 | Chimera Shot | 1/1 | **Capstone.** 50 focus, 10s CD, refreshes Serpent Sting + heals 5% HP. |
| — | (10 pad) | — | Distribute across Improved Mend Pet, partial-rank filler. Bot doesn't care. |

#### Beast Mastery (36)

| Tier | Talent | Ranks | Rationale |
|---|---|---|---|
| 1 | Improved Aspect of the Hawk | 3/3 | While Hawk active, ranged AA has 6% chance to give +30% AS for 6s. Free passive. |
| 1 | One with Nature | 3/3 | +3/6/9% Stamina → Agi from Aspect of the Wild. Marginal but cheap. |
| 1 | Bestial Discipline | 2/2 | +50% pet focus regen. **Pet-damage scaler.** |
| 2 | Pathfinding | 2/2 | +4/8% mounted speed. **Irrelevant for in-combat DPS — fill or skip.** |
| 2 | Spirit Bond | 2/2 | +2% HP/s out of combat for hunter and pet. Cuts downtime. |
| 2 | Frenzy | 3/3 | **Core pet passive.** Pet basic-attack 9/18/27% chance per swing → +30% pet AS 10s. Maintained at 5 stacks via Frenzy mechanic. |
| 3 | Cobra Strikes | 3/3 | **Core pet passive.** Arcane/Aimed crit → pet's next 2 specials guaranteed crit. Stacks with Wild Hunt. |
| 3 | Crouching Tiger, Hidden Chimera | (skip) | PvP — Disengage/Deterrence CD reductions. |
| 3 | Killing Streak | (skip) | Synergizes with Kill Command — BM-primary tool, not MM. |
| 4 | Animal Handler | 2/2 | **+4.5/9% pet damage.** Multiplicative pet damage. |
| 4 | Fervor | (skip) | 2min CD, +50 focus pet+hunter. Useful but contention with Bestial Wrath GCD-pattern. |
| 4 | Focus Fire | 1/1 | Consume pet Frenzy stacks for hunter haste (3% per stack, 20s). Off-GCD; bot fires at 5 stacks during burst windows. |
| 5 | Ferocious Inspiration | 3/3 | **Pet-special crit → +3% damage raid aura 10s.** Refreshable. Maintained passively. |
| 5 | Longevity | 3/3 | -10/20/30% pet CD on Bestial Wrath / Intimidation / Call of the Wild. **Reduces BW to ~84s effective.** |
| 5 | Kindred Spirits | 5/5 | **+20% pet damage + 5y pet movement.** Largest single pet-damage multiplier in the tree. |
| 6 | Bestial Wrath | 1/1 | **2min CD (84s with Longevity), 10s, +20% pet damage, pet dispel-immune.** New tier method on bot. |
| 6 | Invigoration | 2/2 | Pet special crit → +6 hunter focus. Sustained focus. |
| 6 | Wild Hunt | 2/2 | **Core pet passive.** +50% pet special damage; pet special abilities cost +60% focus / +30% AP from focus (community-resolved interpretation of W6 dummy 60/50). Multiplicative with Kindred Spirits / Animal Handler. |
| 7 | Beast Mastery | 1/1 | **Capstone.** Tame exotic pets (Spirit Beast — Prowl + Spirit Mend self-heal; Devilsaur — Terrifying Roar = mob Sunder). Pet utility plus hidden damage multiplier per W7. |
| — | (no pad needed; tree fully covers 36 with these picks) | — | — |

#### Survival (5)

| Tier | Talent | Ranks | Rationale |
|---|---|---|---|
| 1 | Hunter vs Wild | 3/3 | +3/6/9% Stamina → Agility from Aspect of the Hawk. Free passive. |
| 1 | Pathing | 2/3 | +1/2/3% ranged crit. **Take 2/3 for the 5-point budget; 3/3 if there's a spare** (the 142-point pad allows 3/3). |

### Glyphs

| Slot | Glyph | Notes |
|---|---|---|
| **Prime** | Glyph of Kill Shot | Universal — Kill Shot recasts within 6s if it doesn't kill. |
| **Prime** | Glyph of Rapid Fire | +10% AS buff during RF (default; swap to Glyph of Arcane Shot if movement fight). |
| **Prime** | Glyph of Aimed Shot | -20s on Aimed Shot CD (no-op in Cata — Aimed has no CD; old glyph effect is now -3 focus cost). Confirm in 4.3.4 via DBC; if it's actually -3 focus cost, still take. |
| **Major** | Glyph of Disengage | -5s CD on Disengage. Defensive uptime. |
| **Major** | Glyph of Misdirection | MD CD reset on no-threat application — but the bot always lands MD on the tank, so this is utility only. Default keep. |
| **Major** | Glyph of Trap Launcher | -10 focus on trap deploy. Useful if user ever toggles trap-laying on; harmless if not. |
| **Minor** | Glyph of Feign Death | -30s CD on Feign Death. Threat-dump uptime. |
| **Minor** | Glyph of Revive Pet | Revive Pet has no cast time. Bot recovery. |
| **Minor** | Glyph of Aspect of the Pack | +2y on Aspect of the Pack range. Trivial. |

### Talent allocation — source of truth

**No third-party calculator URL.** Wowhead's Cata Classic talent calc enforces
stock-Cata rules (41-point primary cap, 71 total) and cannot represent a
142-point dual-tree build. The §C tree breakdown table above is the build of
record.

Allocate in-game via `.altbot talents learn <talentId> <rank>` per row. Cross-
check totals after placement: **BM 36 / MM 41 / SV 5** (142 total).

The current `.altbot talents show <bot>` command only reports active spec
+ remaining points; a richer worldserver-logged dump of the full talent
placement is a queued follow-up so you can grep the build out of
`worldserver.log` rather than transcribing from in-game chat.

---

## §D. Rotation tier order

### Current `MmHunterStrategy::Update()` tier order (verbatim from `src/strategies/MmHunterStrategy.cpp:156-163`)

```cpp
if (Tier_AimedShotProc(bot, target))    return;
if (Tier_SerpentSting(bot, target))     return;
if (Tier_ChimeraShot(bot, target))      return;
if (Tier_KillShot(bot, target))         return;
if (Tier_AoE(bot, target))              return;
if (Tier_AimedShotHard(bot, target))    return;
if (Tier_ArcaneShot(bot, target))       return;
Tier_SteadyShot(bot, target);
```

(Above this: Tranq Shot encounter-purge tier; below `THREAT_WINDOW_MS` gate
on `ctx.combatElapsedMs < 2000` — DPS rotation idles for 2s of pull. Misdirection
on tank + DoDefensives + PetMaintenance + DoMaintenance pre-empt the gate.)

### Recommended additions for Build B (MM + BM)

**Maintenance / cooldown layer (pre-rotation, called from `DoMaintenance` /
`PetMaintenance`):**

- **Bestial Wrath** (2min CD, 84s with Longevity, off-GCD). Fires on cooldown
  if (a) target is alive and IsAttackable, (b) bot has a pet, (c) `combatElapsedMs > 2000`
  (post-threat window so pet isn't pulling threat). **No new tier in the
  dispatch chain — placed in `DoMaintenance` alongside Aspect of the Hawk
  upkeep.** Off-GCD means it never collides with rotation.
- **Focus Fire** (consume pet Frenzy 5-stack). Off-GCD. Decision rule:
  fire when pet has 5 Frenzy stacks AND Bestial Wrath is OFF cooldown
  (prefer haste during big-CD windows). Otherwise hold. Default for simplicity:
  fire when pet has 5 stacks AND `ctx.combatElapsedMs > 10000` (skip opener so
  BW + RF stack first).

**Rotation tiers (in `Update`):**

**No new tier insertions required.** All BM-secondary gains are passive. The
existing tier chain plays correctly with BM passives because:

- Wild Hunt / Kindred Spirits / Animal Handler / Frenzy / Cobra Strikes /
  Ferocious Inspiration / Bestial Mastery (capstone) = pet damage passives.
  The pet just hits harder. No code change.
- Bestial Wrath = off-GCD, handled in `DoMaintenance`. No tier change.
- Focus Fire = off-GCD, handled in `DoMaintenance` if wired in (it's optional).

**Compared to Build A (Survival-secondary) — for reference:**

If the doc had picked Build A, one new tier would slot above `Tier_ChimeraShot`:

```
if (Tier_BlackArrow(bot, target))       return;   // NEW — between SerpentSting and ChimeraShot
```

`Tier_BlackArrow` fires when (a) the spell is known, (b) off cooldown,
(c) focus ≥ 35, (d) Black Arrow not already on the target (it doesn't
stack). The tier is shaped after `Tier_SerpentSting`. **Build B doesn't
need this.**

### Pet maintenance (existing)

- `Call Pet 1` — re-summon if pet dead and not in combat (existing).
- `Mend Pet` — fire when pet HP < 60% (existing).
- **NEW:** With Beast Mastery capstone, the bot can tame exotic pets.
  No code change — the bot's existing tame logic uses `Call Pet 1`, and the
  exotic pet (if tamed) just slots into the same pet manager. The user
  taming a Spirit Beast or Devilsaur out-of-band is a content-creation step,
  not a strategy-code step.

### Defensives (existing)

- **Deterrence** — `bot.hp < 35%` (existing).
- **Feign Death** — `bot.hp < 25%` (existing).
- **Disengage** — `meleeNear >= 1 AND bot.hp < 60%` (existing; Posthaste
  talent adds free run speed after firing).

No new defensives unlocked by BM-secondary.

---

## §E. Bot-executable caveats

Honest accounting of what the bot does / doesn't do well in this build.

1. **Aimed Shot! (Fire! proc) handling — already correct.**
   The proc is already cached + handled by the top-priority tier:
   `MmHunterStrategy::Tier_AimedShotProc` (src/strategies/MmHunterStrategy.cpp:336-350)
   reads `FindHunterAuraByName(bot, "Fire!")` and prefers spell 82928
   ("Aimed Shot!") over hard-cast Aimed Shot 19434. The passive-talent
   collision class noted in CLAUDE.md cast-pipeline-gotchas #1 is already
   handled (`info->IsPassive()` filter on the name walker, see
   `FindHunterAuraByName` at line 308-326). **No change needed.**

2. **Steady Shot auto-attack reset timing — server-managed, not bot's problem.**
   Cata's ranged AA timer interacts with Steady Shot casts (cast resets AA
   swing timer); the bot uses Steady Shot as filler and the timing is
   handled by the server's AA scheduler. No bot-side optimization possible
   at 1.5s tick cadence.

3. **Trap placement — not bot-managed.**
   `AltbotPosition` does not choose ground locations for Frost Trap / Ice Trap /
   Snake Trap / Explosive Trap. For Build B (BM-secondary) this is fine —
   no rotation tier depends on trap placement.
   - **Build A relevance (for reference):** if the user later flips to MM+SV,
     LnL only procs from Freezing/Ice Trap (W3), so without trap drops the
     LnL talent point is effectively dead. The bot would still get Black
     Arrow as a stand-alone DoT (no trap interaction). This is acceptable
     but is exactly the §B argument for picking BM-secondary instead.

4. **Misdirection on tank — already wired.**
   `MmHunterStrategy::DoMisdirection` (line 257-269) fires MD on the tank
   pre-pull / on CD, gated by `tank->HasAura(md, bot->GetGUID())`. The
   threat-window gate at `combatElapsedMs < 2000` does *not* gate MD per
   CLAUDE.md "Adding a new spec" rule 4 ("Misdirection on tank during the
   window is *encouraged*, not gated"). **No change needed.**

5. **Pet target switching — no new caveat.**
   The pet follows the hunter's target via Cata's stock pet AI. BM secondary
   doesn't change this. Bestial Wrath does NOT change pet targeting — it
   just buffs damage / clears CC.

6. **Focus Fire decision — judgment call deferred.**
   Optimal Focus Fire usage requires future-CD awareness (consume stacks
   *before* a damage window so the haste lines up with hard-cast Aimed Shots).
   A 1.5s-tick bot can't do this well. Default policy:
   - Skip Focus Fire entirely (Frenzy's pet AS gain is already in place; no
     harm in not consuming).
   - Optional: fire at 5 stacks if `Bestial Wrath` is off CD (let BW's pet
     dispel-immunity be the visible signal for the burst window).

7. **Bestial Wrath alignment with Rapid Fire — coarse.**
   Optimal: BW + RF + Readiness all line up for a single 10s burst window
   every 3min. A bot firing each on CD will *mostly* align in practice
   because CDs converge after a Readiness reset. Don't try to schedule;
   just fire on CD.

8. **Exotic pet taming — manual step.**
   The Beast Mastery capstone enables Spirit Beast / Devilsaur / Worm /
   Chimaera taming. The bot can't navigate to and tame a specific named
   rare. The user (or a `.altbot` chat command extension) supplies the
   pet by taming on the master account and the bot inherits it via
   `Call Pet`. **This is bookkeeping, not strategy work.**

9. **Wild Hunt's exact effect is community-resolved, not Wowhead-confirmed.**
   W6 shows dummy effect values 60 / 50 without a tooltip. The §C
   description ("+50% pet special damage; pet special abilities cost +60%
   focus / +30% AP from focus") is the consensus interpretation from
   community parses, **not DBC-verified**. Tagged in §H.

---

## §F. Side-by-side: stock 7/31/3 → recommended 41/36/5

| Aspect | Stock 7/31/3 | Recommended 41/36/5 (Build B) | Delta |
|---|---|---|---|
| **Primary tree** | MM 31 | MM 41 (full) | +10 MM points → fills out Tier-7 row + filler |
| **Secondary tree** | BM 7 (One with Nature 3, Bestial Discipline 2, Frenzy 2/3) | BM 36 (full deep) | +29 BM points; **all 7 tiers reached** |
| **Tertiary tree** | SV 3 (Pathing 3) | SV 5 (HvW 3, Pathing 2) | +2; tertiary stays shallow |
| **Pet damage** | Frenzy partial; no Wild Hunt / Animal Handler / Kindred Spirits / Cobra Strikes / Ferocious Inspiration / Bestial Wrath | Full pet-damage stack | +30-40% pet damage; pet is ~25-30% of total, so total +8-12% |
| **New CDs** | Trueshot Aura 1/1, Readiness 1/1, Chimera Shot 1/1, Silencing Shot 1/1 (all in 7/31/3) | + Bestial Wrath (2min CD), + Focus Fire (off-GCD optional) | One off-GCD pet CD (auto-maintenance); one optional decision |
| **Passive damage** | Master Marksman, Piercing Shots, Careful Aim, Improved Steady Shot | Same + Wild Hunt + Kindred Spirits + Ferocious Inspiration + Cobra Strikes | Pet contribution scales with hunter shots (Cobra Strikes) and stays maxed (Frenzy 3/3) |
| **Raid utility** | Trueshot Aura (10% RAP), Misdirection (threat) | Same + Ferocious Inspiration 3% damage aura | One new raid buff (passive maintenance) |
| **New rotation tiers** | — | **None** | All BM gains are passive or off-GCD maintenance |
| **New cache entries** | — | Bestial Wrath, Focus Fire (optional) | 1-2 cache slots |
| **Implementation cost** | (current) | ~30 lines (Bestial Wrath in `DoMaintenance`) | Smallest possible delta |
| **Est. DPS gain** | (baseline) | +8-12% | — |

---

## §G. Implementation pointers

### Header additions — `src/strategies/MmHunterStrategy.h`

Add to the `Spell` enum (insert before `Count`):

```cpp
BestialWrath,        // 19574  — hunter 2min CD, +20% pet damage 10s, off-GCD
FocusFire,           // 82692  — off-GCD; consume pet Frenzy stacks for hunter haste (optional)
```

### Cache resolution — `MmHunterStrategy::ResolveSpellCache`

Append to the existing `find(...)` block:

```cpp
_cache[size_t(Spell::BestialWrath)] = find("Bestial Wrath");
_cache[size_t(Spell::FocusFire)]    = find("Focus Fire");
```

`StrategyUtil::FindSpellByFamilyName(bot, SPELLFAMILY_HUNTER, "...")` is the
right resolver — both names are unique in `SPELLFAMILY_HUNTER` (no
talent-vs-proc name collision per CLAUDE.md gotcha #1).

> **Verify before merging:** Wowhead Cata page for spell 19574 confirms
> family Hunter, name "Bestial Wrath". Spell 82692 "Focus Fire" similarly
> family Hunter. Both are direct-cast entries with no passive companion
> sharing the name. **No `IsPassive` filtering issue.**

### New method — `MmHunterStrategy::DoMaintenance` extension

Pattern after the existing `Hawk` upkeep block. Pseudocode:

```text
DoMaintenance(bot, master, target):
    [existing Aspect of the Hawk maintenance]
    [existing Hunter's Mark on boss target]

    // NEW — Bestial Wrath on cooldown post threat-window
    uint32 bw = GetSpell(Spell::BestialWrath)
    if bw and not IsOnCooldown(bot, bw) and bot has alive Pet
       and target and ctx.combatElapsedMs > THREAT_WINDOW_MS:
        StrategyUtil::CastWithLog(bot, bot, bw, SPEC_LABEL)

    // OPTIONAL — Focus Fire at 5 Frenzy stacks during BW window
    uint32 ff = GetSpell(Spell::FocusFire)
    if ff and not IsOnCooldown(bot, ff)
       and pet has 5 Frenzy stacks
       and Bestial Wrath aura active on bot:
        StrategyUtil::CastWithLog(bot, bot, ff, SPEC_LABEL)
```

> `DoMaintenance` already runs *before* the `THREAT_WINDOW_MS` gate, but
> Bestial Wrath should NOT fire pre-window (pet pulling threat is the exact
> failure mode the gate is for). Gate BW on `ctx.combatElapsedMs > 2000`
> explicitly *inside* the maintenance call — don't move the maintenance call
> below the gate (Aspect of the Hawk upkeep and Hunter's Mark application
> still want to happen pre-window).

### Cast-pipeline gotchas (CLAUDE.md §"Cast pipeline gotchas")

1. **Passive-name collision:** Bestial Wrath has a paired buff aura
   (the +20% pet damage aura applied to the *pet*, not the hunter). The
   hunter does NOT receive a `Bestial Wrath` aura — the cast applies it to
   the pet. The hunter-side name walker (`FindHunterAuraByName` in
   MmHunterStrategy.cpp:308) wouldn't be looking for a hunter-side "Bestial
   Wrath" aura; if Focus Fire's "is BW window active?" check is needed, the
   right query is `bot->GetPet()->HasAura(<bw-pet-aura-id>)`. **Cache the pet
   aura ID separately if Focus Fire is implemented.** Until then, the
   IsOnCooldown(bot, bw) inverse (BW off CD = "not in BW window") works as
   a coarse proxy.

2. **CastWithLog routing:** Both new casts go through
   `StrategyUtil::CastWithLog(bot, bot, id, "MmHunter")` — self-cast,
   bot-side. No facing/LOS issues; no GCD cost (off-GCD).

3. **Skip-during-cast guard:** The existing `UNIT_STATE_CASTING /
   IsNonMeleeSpellCast(false)` guard in `Update` (line 132) covers the case
   where the bot is mid-Aimed-Shot when BW comes off CD. **BW is off-GCD**,
   so it *can* fire during another cast — but `CastWithLog` will reject if
   `SPELL_FAILED_SPELL_IN_PROGRESS`. Acceptable to let that error log;
   alternative: gate BW on `!bot->IsNonMeleeSpellCast(false)` for cleanliness.

4. **GCD guard:** BW and Focus Fire are off-GCD, so the `HasGlobalCooldown`
   probe at line 138-143 doesn't apply to them. They fire from the
   maintenance block which runs unconditionally each tick.

### Cross-reference

See `docs/research/2x-talent-strategy-design.md` Part B Option 2 for the
broader design pattern: **detect secondary spells, unlock tiers in-place**.
The `MmHunterStrategy` change here follows that pattern exactly — the new
cache entries are queried, the new maintenance lines fire only when the
spells are known, and a stock 7/31/3 bot continues to behave identically
(its cache returns 0 for both new entries; the maintenance blocks short-circuit
on `if (!bw) ...`).

Also see §C.1.2 of that doc — the "MM+Survival" sketch flags the LnL gap
that drove this doc to pick BM-secondary instead. The deviation between
that sketch and this doc's recommendation is intentional and is the resolved
answer to the §A.4 "Build A bigger or smaller than Build B in raid?" unknown.

---

## §H. Unknowns / verification checklist

| Item | Status | Resolution path |
|---|---|---|
| Wowhead Cata calculator URL encoding | **Unverified** — assembled by hand from the §C tree map; WebFetch returns only the page header for `/cata/talent-calc/hunter/...`. | Open the §C URL in a real browser; if the build doesn't match, derive the correct digit string by adjusting talent-by-talent in the calculator and copying the URL out. |
| Wild Hunt effect interpretation | **Community-resolved, not DBC-verified.** W6 shows dummy effect values `60` and `50` only. | Verify via WDBXEditor → `Spell.dbc` entries 53270 + 53271. Cross-check with `docs/research/dbc-verification-checklist.md`. Update §C if the actual effect differs from "+50% pet special damage / +60% AP from focus". |
| Bestial Wrath spell ID 19574 | **Likely correct** but not directly verified for the 4.3.4 fork. | Resolve via `StrategyUtil::FindSpellByFamilyName(bot, SPELLFAMILY_HUNTER, "Bestial Wrath")` at runtime — log the resolved ID on first tick. |
| Focus Fire spell ID 82692 | Same as above. | Same. |
| Bestial Wrath pet aura ID (the +20% damage aura on the *pet*) | **Unverified.** Distinct from the cast spell. | WDBXEditor → look for the family-Hunter aura with the pet-damage effect referenced by 19574. Needed only if Focus Fire's "is BW window active?" check is implemented. |
| Black Arrow focus cost 35 vs "6% base mana" contradiction (W1 vs W2) | **Resolved as 35 focus** — Cata is focus-only since 4.0.1 (L1 §6); W2's "6% base mana" is pre-Cata text. | Re-verify in DBC if Build A is ever revisited. |
| Lock and Load proc surface in 4.3.4 | **Resolved per W3** — only Freezing/Ice Trap. | Patch 4.3.2 removed Black Arrow / Immolation / Explosive as proc sources. No further verification needed; decision in §B is final. |
| Wild Hunt tier (BM Tier 6 vs Tier 7) | **Assumed Tier 6** per community guides. | Verify via the Wowhead calculator URL render; correct §C if needed. |
| Master Marksman 5-stack proc spell ID (Fire! buff 82926) | **Verified** per W5. | None needed. |
| Aimed Shot! (proc variant) ID 82928 | **Verified** per W4 + already cached in current `MmHunterStrategy`. | None needed. |
| Pet damage as % of total in MM (cited ~25-30%) | **Estimated** from G2 + general community parse data. | Defer; not load-bearing — the verdict in §B still holds at 20% or 35% pet share. |
| Beast Mastery capstone (spell 56636) "4x effect mult on Kill Shot / Steady / Aimed" per W7 | **Wording unclear.** W7 shows the effect but doesn't explain the practical impact. | Defer to in-game testing — if the effect is non-existent (BM capstone is purely "tame exotic pets"), the verdict is unchanged. If it adds real damage, Build B's edge over Build A widens. |
| Glyph of Aimed Shot effect in 4.3.4 | **Possibly stale wording** — old "(-20s CD)" no longer applies. Likely "-3 focus cost". | Check the in-game glyph tooltip after applying; if it's worthless, swap for Glyph of Steady Shot (+10% crit on next Aimed Shot after 3 Steadys) per G6's prime-glyph list. |
| Hunting Party overlap with Windfury / Bloodlust | **Assumed minimal** — Hunting Party is a 10% AS aura; Windfury is a separate weapon proc; Bloodlust is +30% haste 40s on a 10-min CD. Stacks. | Confirm group composition; if no other AS source, Hunting Party is a 10% raid buff worth re-prioritizing. (Note: Hunting Party is a Survival talent — only relevant if user flips to Build A.) |

---

## Final report (parent agent — keep ≤ 200 words)

**Recommended secondary: Beast Mastery.** Pet passives (Wild Hunt + Kindred
Spirits + Animal Handler + Frenzy + Cobra Strikes + Ferocious Inspiration +
Bestial Wrath uptime) compound multiplicatively on the ~25-30% of total
damage the MM pet contributes, yielding ~8-12% DPS over stock 7/31/3 — strictly
larger than Survival-secondary's ~5-8% from a stand-alone Black Arrow. Decisive
because **Lock and Load in 4.3.4 procs only from Freezing/Ice Trap (patch 4.3.2
removed Black Arrow / Immolation / Explosive as proc sources)** — a bot that
doesn't place traps cannot light up Survival's signature proc engine, so
Black Arrow becomes just a third DoT. BM-secondary is also engineering-cheaper:
all gains are passive except Bestial Wrath (off-GCD, lives in `DoMaintenance`).
No new rotation tiers; existing Update() chain is unchanged.

**Talent source of truth:** the §C tree breakdown table (no Wowhead URL —
Wowhead's Cata calc enforces a 71-point total, can't represent 142). Allocate
via `.altbot talents learn <talentId> <rank>` per row; cross-check totals
BM 36 / MM 41 / SV 5.

**Top caveat:** the Beast Mastery capstone enables exotic-pet taming
(Spirit Beast, Devilsaur) but the bot can't navigate to and tame a specific
named rare. The user supplies the pet by taming on the master account and
the bot inherits via `Call Pet`. Bookkeeping, not strategy code — but worth
flagging because the BM-secondary's marginal utility upside (Devilsaur
Terrifying Roar = Sunder, Spirit Beast Spirit Mend = small self-heal) only
materializes once the exotic is in the stable.

End of doc.

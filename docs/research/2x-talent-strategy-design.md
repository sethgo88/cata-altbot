# 2x Talent Points — Strategy Model Design

Research-only design doc. Worked example: Cata 4.3.4 Affliction Warlock on the
**142-point server config** (double the standard 71). At max level, a bot can
fully fill its 41-point primary tree *and* a 36+ point secondary tree, with
leftovers feeding the third tree's first 1-2 rows. The current strategy code
keys on primary tree only, so an Aff warlock who specced *also* into
Destruction or Demonology plays the bog-standard Aff rotation and ignores
half the talent investment.

Survey conducted 2026-05-12. Strategy/rotation content reconciled across the
guides listed in §2. Hard numbers (cast-time deltas, talent ranks, spell IDs,
buff durations) verified against Wowhead Cata Classic / Warcraft Wiki —
referenced inline.

> **Scope clamp.** This doc covers the *strategy model* — how the rotation
> engine should evolve. It does not cover the talent-spend side (how the
> server actually allocates 142 points, what spend policy the bot uses).
> That belongs in a separate doc covering `AltbotTalents.cpp` and the spend
> policy. The design here assumes the bot has whatever talents it has and
> the rotation reads them.

> **Terminology.** "Primary tree" = the tree the bot has 31+ points in
> (today's `GetPrimaryTalentTree()` returns this). "Secondary tree" = the
> other tree with significant investment. "Tertiary" = the third tree's
> handful of leftover points (≤5).

---

## 1. Why this matters now

On a stock 71-point server, an Aff warlock build is roughly **31/0/10** or
**32/0/9** (G2, G13 in §2). The 10 Destruction points buy `Bane` (Tier 1,
3 ranks, -0.5s Shadow Bolt / Immolate / Chaos Bolt cast time at max) and
`Shadow and Flame` (Tier 2, 3 ranks, +12% Shadow Bolt / Incinerate damage
and a 5% spell-crit debuff at 3/3). At 71 points the Aff warlock *cannot*
reach Destruction Tier 3 — `Improved Soul Fire` was moved to Tier 3 in 4.3
specifically to lock it out of Aff/Demo builds ([G_ISF]).

On 142 points, this constraint disappears. The bot reaches:

- **Destruction Tier 3** (`Improved Soul Fire` — 8% spell haste for 20s after
  Soul Fire cast; sustainable buff if Soul Fire fits in the rotation).
- **Destruction Tier 5** (`Backdraft` — 3-charge, 15s, 30% cast speed on
  Incinerate / Chaos Bolt after Conflagrate ([G_BD])).
- **Destruction Tier 7** (`Chaos Bolt` — 12s CD, fire-school nuke, ignores
  absorbs).
- **Demonology Tier 3** (`Master Demonologist` — passive damage bonus when
  Felhunter is the active pet, helps Aff out-of-the-box).
- **Demonology Tier 4** (`Demonic Empowerment` — +50% pet haste, +20% pet HP,
  12s).
- **Demonology Tier 6** (`Demonic Pact` — pet-crit-triggered 10% spellpower
  raid aura, 45s; flat 10% in Cata, not scaling with warlock SP ([G_DP])).

The single biggest item on that list for a primary-Aff bot is **`Bane` 3/3 +
`Shadow and Flame` 3/3**, both already in the existing 31/0/10 build, so the
stock spec doc already accounts for them. **Everything above** — Improved
Soul Fire, Backdraft, Chaos Bolt, Demonic Pact, Master Demonologist — is
what the bot is leaving on the table.

This is not a small leak. Demonic Pact alone is **a 10% raid-wide spellpower
buff** that the bot currently never applies; Improved Soul Fire is a sustained
8% personal haste buff on a 20s timer ([G_ISF]).

---

## 2. Sources

Strategy/rotation claims in this doc are reconciled across the five
guides marked **strategy-bearing** below. Hard numbers (spell IDs,
durations, cast-time deltas) draw on the wiki/wowhead sources, which is
fine per the project's research-depth policy.

| # | Guide | URL | Confidence |
|---|---|---|---|
| G_AFF_IV  | Icy Veins — Aff Warlock Talents & Glyphs | https://www.icy-veins.com/cataclysm-classic/affliction-warlock-pve-spec-builds-talents-glyphs | High (strategy-bearing) |
| G_DES_IV  | Icy Veins — Destro Warlock Talents & Glyphs | https://www.icy-veins.com/cataclysm-classic/destruction-warlock-pve-spec-builds-talents-glyphs | High (strategy-bearing) |
| G_DEM_IV  | Icy Veins — Demo Warlock Talents & Glyphs | https://www.icy-veins.com/cataclysm-classic/demonology-warlock-pve-spec-builds-talents-glyphs | High (strategy-bearing) |
| G_DES_WT  | Warcraft Tavern — Destro Rotation & CDs | https://www.warcrafttavern.com/cataclysm/guides/pve-destruction-warlock-rotation-cooldowns/ | High (strategy-bearing) |
| G_AFF_WT  | Warcraft Tavern — Aff Talents & Glyphs | https://www.warcrafttavern.com/cataclysm/guides/pve-affliction-warlock-talents-builds-glyphs/ | High (strategy-bearing) |
| G_DES_MMO | MMO-Champion thread 1018656 — Destruction 4.3 Guide | https://www.mmo-champion.com/threads/1018656-Destruction-A-Guide-(4-3) | Medium (community-authored) |
| G_DES_WB  | wowtbc.gg — Destruction Rotation & Talent Build | https://wowtbc.gg/cata/class-guides/destruction-warlock/ | Medium |
| G_DES_CYN | Cynwise — Destruction Raiding in 4.0.1 | https://cynwise.wordpress.com/2010/10/18/destruction-warlock-raiding-in-4-0-1/ | Medium (older patch) |
| G_DES_OG  | Overgear — Cata Destruction Warlock | https://overgear.com/guides/cataclysm-classic/destruction-warlock-guide/ | Medium |
| G_BD      | Wowpedia — Backdraft | https://wowpedia.fandom.com/wiki/Backdraft | High (hard numbers) |
| G_DP      | Wowpedia — Demonic Pact | https://wowpedia.fandom.com/wiki/Demonic_Pact | High (hard numbers) |
| G_ISF     | Warcraft Wiki — Improved Soul Fire | https://warcraft.wiki.gg/wiki/Improved_Soul_Fire | High (hard numbers; 4.3 patch note: 20s) |
| G_SAF     | Wowhead Cata — Shadow and Flame | https://www.wowhead.com/cata/spell=17801/shadow-and-flame | High (hard numbers) |
| G_CONF    | Wowhead Cata — Conflagrate | https://www.wowhead.com/cata/spell=17962/conflagrate | High (hard numbers — 10s CD, 16% base mana) |
| G_BANE    | Wowhead Cata — Bane | https://www.wowhead.com/cata/spell=17788/bane | High (hard numbers) |
| G_UA_WIKI | Wowpedia — Unstable Affliction | https://wowpedia.fandom.com/wiki/Unstable_Affliction | High — confirms UA/Immolate per-warlock exclusivity (since 3.2.0) |
| G_TC19790 | TrinityCore issue #19790 — Conflagrate vs. Immolate-from-other-caster | https://github.com/TrinityCore/TrinityCore/issues/19790 | Reference — confirms Conflag reads "Immolate on target" without caster filter on some cores |
| G_DEM_WT  | Warcraft Tavern — Demonology Talents & Glyphs | https://www.warcrafttavern.com/cataclysm/guides/pve-demonology-warlock-talents-builds-glyphs/ | High (strategy-bearing for §A.1) |

Coverage gaps: no surveyed guide directly addresses **doubled talent
budgets**. The 31/0/10 + secondary-tree-full hybrid is a novel build for the
custom server; the design has to extrapolate from each tree's standard
guide. That's flagged in §A.4.

---

# Part A — Affliction Warlock on 142 points

## A.1. Realistic dual-tree builds at 142 points

The standard Cata talent rule still applies: **31 points in primary required
to unlock the 41-point capstone, and 31 in the *primary* tree to unlock
secondary specialization** (G_AFF_IV). On 142 points the math:

```
41 primary  (Aff full)
+ 36 secondary  (Destro or Demo, deep enough for a tier-7 capstone)
+ ~30 tertiary leftovers (1-3 rows of the third tree)
+ ~35 unspent / sub-optimal sinks
= 142 total
```

Two realistic primary=Affliction dual-tree shapes for PvE:

### Build A — **Aff(41) / Destro(36) / Demo(0)** — *the cast-speed build*

| Tree | Points | Key picks |
|---|---|---|
| Aff   | 41 | Full primary tree, capstone `Haunt`. Standard 31/0/10 picks plus everything from Tier 4-7. |
| Destro | 36 | `Bane` 3/3, `Aftermath` 2/2 (daze on Conflag), `Shadow and Flame` 3/3, `Improved Soul Fire` 3/3, `Soulburn: Searing Pain` (Tier 4), `Backdraft` 3/3, `Shadowburn` 1/1, `Empowered Imp` 3/3 (only if Imp; skip on Felhunter), `Bane of Havoc` 1/1, `Chaos Bolt` 1/1 (capstone). |
| Demo  | 0–~ | Unspent or 1-2 leftover. |

Unlocked spells the **current `AffWarlockStrategy` doesn't use**:
- `Conflagrate` (17962) — 10s CD, 16% base mana, fire instant. Damage = 60% of
  current Immolate periodic-damage total ([G_CONF]).
- `Chaos Bolt` (50796) — 12s CD, 2.5s cast (1.5s with Bane 3/3), fire-school
  nuke, ignores absorbs.
- `Shadowburn` (17877) — execute-phase instant; ≤20% target HP, 5s CD,
  refunds shard on kill ([G_DES_WT]).
- `Soulburn: Searing Pain` for grip/tank-threat use (less relevant for a bot).
- `Bane of Havoc` (80240) — single-target debuff that copies % of damage to a
  second nearby target. AoE-mode tool. **Affliction can use this once
  talented even though the standard 31/0/10 doesn't reach it.**

Cross-tree damage **passives** that already help Aff's existing rotation:
- `Bane` 3/3 — -0.5s Shadow Bolt / Soul Fire / Chaos Bolt / Immolate cast
  time ([G_BANE]). Aff already takes this in 31/0/10.
- `Shadow and Flame` 3/3 — +12% Shadow Bolt damage, 100% chance to apply 5%
  spell-crit debuff for 30s ([G_SAF]). Aff already takes this.
- `Improved Soul Fire` 3/3 — 8% spell haste for 20s after Soul Fire cast
  ([G_ISF]). **Not reachable in 31/0/10; reachable here.**
- `Aftermath` 2/2 — daze on Conflag (utility; ignore for PvE DPS).
- `Backdraft` 3/3 — 30% cast-speed reduction for next 3 Incinerates/Chaos
  Bolts, 15s, after Conflag ([G_BD]). **High value if Conflag fits in
  rotation.**

### Build B — **Aff(41) / Demo(36) / Destro(0)** — *the pact-aura build*

| Tree | Points | Key picks |
|---|---|---|
| Aff   | 41 | Full primary tree, capstone Haunt. |
| Demo  | 36 | `Demonic Embrace` 3/3 (HP), `Dark Arts` 3/3 (pet damage), `Fel Synergy` 2/2 (pet HP regen), `Demonic Aegis` 3/3 (Fel Armor +30%), `Master Demonologist` 3/3 (Felhunter: +15% damage), `Impending Doom` 3/3, `Mana Feed` 3/3, `Inferno` 2/2, `Demonic Empowerment` (Tier 4), `Demonic Pact` 3/3 (Tier 6), `Hand of Gul'dan` (Tier 7 capstone). |
| Destro | 0  | Unspent or 1-2 leftover. |

Unlocked spells the **current `AffWarlockStrategy` doesn't use**:
- `Demonic Empowerment` (47193) — 12s pet buff, 60s CD. Felhunter form: +20%
  HP, +50% haste, +20% damage taken from pet. On-CD pet cooldown.
- `Hand of Gul'dan` (71521) — 15s CD, 2s cast, fire damage and applies
  `Cripple` debuff. Demo-spec rotation centerpiece, less synergistic with
  Aff's Shadow Bolt filler but adds a strong nuke.

Cross-tree passives that help Aff:
- `Demonic Aegis` 3/3 — Fel Armor buffs spellpower bonus by +30%. **Aff
  already maintains Fel Armor**, so this is free if reachable.
- `Master Demonologist` 3/3 — +15% spell damage while Felhunter is summoned.
  Aff's standing pet is Felhunter (G_AFF_IV §1.3). **Free damage.**
- `Demonic Pact` 3/3 — pet-crit-triggered raid aura, flat 10% spellpower for
  45s ([G_DP]). **Group buff** — analogous to Elemental Shaman's Totemic
  Wrath.

### Source consensus on "which secondary is better for DPS"

Stock Cata (71-point) builds *don't* face this question — Aff goes 31/0/10
for the Shadow Bolt buffs and that's it. The 142-point version is novel.
Best we can do is reconcile what each secondary brings:

| Criterion | A (Aff+Destro) | B (Aff+Demo) |
|---|---|---|
| Personal DPS — single-target | Adds Chaos Bolt + Conflag if Conflag fits (see §A.2). Backdraft helps if Conflag fits. Improved Soul Fire 8% personal haste if Soul Fire kept up. | +15% damage from Master Demonologist (passive, no rotation change). Demonic Pact 10% SP (passive, no rotation change). |
| Personal DPS — AoE | Bane of Havoc (single-target → cleave). Chaos Bolt is single-target. | Hand of Gul'dan adds a nuke. No real AoE addition. |
| Group utility | Aftermath daze (low). Shadow and Flame 5% crit debuff (already in 31/0/10). | **Demonic Pact 10% SP raid aura** (huge). |
| Rotation engine cost | Conflag/Immolate vs UA collision (§A.2) is a *real* design problem. Chaos Bolt is a clean addition (single-target nuke on CD). | Trivial — most gains are passive. Demonic Empowerment is a 60s-CD button. Hand of Gul'dan is a 15s-CD button. |
| **Implementation difficulty** | **Hard** (Conflag rotation conflict) | **Easy** (mostly passives + 2 buttons) |

**Verdict (worked example):** Build B (Aff+Demo) is the *cleaner* engineering
target — most of the gain is passive bonuses the rotation reads through
`bot->HasSpell` already. Build A (Aff+Destro) is the *bigger* personal-DPS
gain *if* the Conflag conflict can be resolved (and per §A.2 it can, with
caveats). The user-facing recommendation in Part D phases Build A first
specifically because it forces the harder design question and the result
generalizes.

---

## A.2. Does Conflagrate help an Aff rotation? — the central question

This is the one that decides whether a "Hybrid AffDestro" strategy is worth
writing.

### Hard facts

1. **Conflagrate requires Immolate on the target**, dealing damage = 60% of
   the total periodic damage Immolate would deal over its full duration
   ([G_CONF]). Cooldown 10s, 16% base mana, instant cast.
2. **`Unstable Affliction` and `Immolate` cannot coexist on the same target,
   *per warlock*.** The two spells are flagged exclusive at the spell-info
   level — applying one removes the other from the same caster. This rule
   has been in place since patch 3.2.0 ([G_UA_WIKI]). It's *per-caster*;
   another warlock's Immolate doesn't strip your UA.
3. **`Aftermath` (Destro Tier 2) and `Backdraft` (Destro Tier 5)** are the
   passives that gate around Conflag — Aftermath adds daze (irrelevant for
   DPS), Backdraft adds 30% cast speed for next 3 Incinerate/Chaos Bolt
   casts ([G_BD]).

### Why this is a rotation conflict for Aff

Aff's DoT-priority list is:
`Haunt > Bane of Doom > Corruption > UA > CotE > Shadowflame > Shadow Bolt > Drain Soul`
(consensus across 5 guides per `affliction-warlock-guide-survey.md` §3).

UA is a *high-priority* DoT. Dropping UA to maintain Immolate (and feed
Conflagrate) means trading:
- **Lost:** UA tick damage + the Pandemic refresh hook + Eradication-proc
  surface area (UA is one of the procc'ing DoTs).
- **Gained:** Immolate tick damage + a 10s-CD Conflagrate (~60% of Immolate
  total damage) + Backdraft 30% cast-speed window applied to ... what,
  exactly? Aff's filler is Shadow Bolt, not Incinerate; Backdraft buffs
  Incinerate/Chaos Bolt, **not Shadow Bolt** ([G_BD]).

The Backdraft point is decisive: a hybrid Aff+Destro bot that swaps UA for
Immolate to enable Conflag pays the rotation cost but **doesn't reap
Backdraft's main benefit** because Shadow Bolt isn't a Backdraft-eligible
spell. The only Backdraft-eligible spell in this build is Chaos Bolt (12s
CD; one cast per Backdraft window at best).

### Source signals on the question

No surveyed guide explicitly endorses a "swap UA for Immolate" hybrid Aff
build — because at 71 points, you can't reach Conflag/Backdraft from
primary-Aff anyway (Conflag is baseline at 30 but requires the Destro tree
to spec into the support talents; the 31/0/10 build deliberately stops at
Destro Tier 2). The closest analog is Cynwise's 4.0.1 Destruction guide
([G_DES_CYN]) which lays out the Destro-primary rotation: it explicitly
*requires* Immolate up at all times *and* makes Conflag the second-highest
priority after the Immolate-refresh check. That rotation only works
because Destro doesn't have UA in its rotation in the first place.

Wowtbc.gg's Destruction guide ([G_DES_WB]) confirms: Destruction's rotation
is `Immolate → Conflag → Bane of Doom → Curse of Elements → Soul Fire → Chaos
Bolt → Incinerate filler`. Note "Incinerate filler" — that's where Backdraft
pays off. Swapping to a Shadow-Bolt filler defeats Backdraft.

### Net answer

**Conflagrate is a net rotation loss for an Affliction-primary bot** unless
the bot *also* swaps its filler from Shadow Bolt to Incinerate, at which
point you've drifted toward Destro-primary anyway and the question is "does
Aff outperform Destro for the rest of the kit." The Mastery system makes
this even worse: Aff's mastery (Potent Afflictions, +13% baseline +1.63%/pt
DoT damage) only buffs shadow DoTs, not Immolate's fire-school periodic.

**What the bot SHOULD do with the new spells (Build A — Aff+Destro):**

| Spell | Use? | When |
|---|---|---|
| Conflagrate | **No** (default off) | Only if a user setting flips Aff into "fire hybrid" mode and Immolate replaces UA. Out of scope for the first cut. |
| Chaos Bolt | **Yes** | On CD as a single-target nuke. Doesn't conflict with the DoT priority — it slots above Shadow Bolt filler. Bane 3/3 cuts cast to 2.0s. |
| Shadowburn | **Yes** | Replaces / augments Drain Soul in execute (<25% HP target). 5s CD, instant, shard refund on kill. |
| Bane of Havoc | **Conditional** | Cleave situations only. Replaces Bane of Doom on the secondary target. Requires multi-target AoE-mode awareness which the bot already has via `Tier_AoE`. |
| Soul Fire | **Maintenance** | Cast once at pull to get Improved Soul Fire 8% haste; refresh when buff timer < 4s. Same pattern as Fel Armor maintenance. |
| Improved Soul Fire | **Passive** | Read via `bot->HasAura(<isf-buff-id>)` to decide on Soul Fire refresh. |
| Backdraft | **N/A** | Requires Conflag cast; not used. |

That's a clean addition to the existing tier chain. No UA swap, no Immolate.

**What the bot SHOULD do with the new spells (Build B — Aff+Demo):**

| Spell | Use? | When |
|---|---|---|
| Demonic Empowerment | **Yes** | On CD (60s). Pet cooldown — no rotation cost. |
| Hand of Gul'dan | **Yes** | On CD (15s). Slots above Shadow Bolt filler; 2.0s cast (1.5s with Bane). Note: applies `Cripple` debuff which doesn't conflict with anything Aff cares about. |
| Demonic Pact | **Passive** | Pet-crit-triggered aura. The bot doesn't do anything for it — but its log gets noisier (raid-buff applies fire constantly), so the cast log should suppress this. |
| Master Demonologist | **Passive** | +15% damage while Felhunter active. No rotation change. |
| Demonic Aegis | **Passive** | +30% Fel Armor SP bonus. Already maintained. |

Even cleaner — almost no rotation engine changes, just two new on-CD buttons.

---

## A.3. Rotation changes — concrete

Spell IDs to cache in addition to the existing `AffWarlockStrategy::Spell` enum:

### Build A (Aff+Destro)

```cpp
enum class Spell : uint8_t
{
    // ... existing entries unchanged ...
    ChaosBolt,           // 50796
    Shadowburn,          // 17877
    SoulFire,            // 6353
    BaneOfHavoc,         // 80240
    Conflagrate,         // 17962 — present but NOT cast by default
    Immolate,            // 348   — present but NOT cast by default
    Count
};
```

Resolution: `StrategyUtil::FindSpellByFamilyName(bot, SPELLFAMILY_WARLOCK, "...")`
handles all of these. Shadow and Flame, Bane, Backdraft, Improved Soul Fire,
Demonic Aegis, Master Demonologist are passive talents — they grant *passive*
spell effects that don't need caching; the bot just casts more often / faster
because they're on. The exception is **Improved Soul Fire's *proc aura***
(the 8% haste buff itself) which needs a separate cache slot for maintenance
detection — same pattern as `FrostMageStrategy`'s Brain Freeze / Fingers of
Frost handling (CLAUDE.md cast-pipeline-gotchas #1: passive-name collision —
test that `info->IsPassive()` filtering picks the right ID).

New tier dispatch (insertion order matters):

```
Tier_Haunt                  (existing, top priority)
Tier_BaneOfDoom             (existing)
Tier_Corruption             (existing)
Tier_UnstableAffliction     (existing)
Tier_AoE  (Seed of Corruption)
Tier_ImprovedSoulFireUp     (NEW — refresh Soul Fire if buff < 4s OR missing)
Tier_BaneOfHavocCleave      (NEW — single secondary mob present, no BoH up)
Tier_ChaosBolt              (NEW — on CD, single target)
Tier_HandOfGuldan           (NEW — Build B path; alternate Tier_ChaosBolt)
Tier_DrainSoul              (existing — pre-empted by Shadowburn below)
Tier_Shadowburn             (NEW — execute, instant, ≤20% HP)
Tier_ShadowBolt             (existing — filler)
```

### Build B (Aff+Demo)

```cpp
enum class Spell : uint8_t
{
    // ... existing entries unchanged ...
    DemonicEmpowerment,  // 47193
    HandOfGuldan,        // 71521
    Count
};
```

Tier dispatch additions:

```
Tier_DemonicEmpowerment     (NEW — pet on CD, 60s; route via maintenance not tier)
Tier_HandOfGuldan           (NEW — on CD)
```

Demonic Empowerment is a *pet cooldown* — `PetMaintenance` is the right home,
not `DoDefensives` (CLAUDE.md "Adding a new spec" rule 6: pure DPS pet CDs go
through maintenance, not the defensive/displacement category which gates on
HP).

### Maintenance addition for Build A

```cpp
void AffDestroWarlockStrategy::DoMaintenance(Player* bot, Unit* target)
{
    AffWarlockStrategy::DoMaintenance(bot, target);   // delegate to base

    // Improved Soul Fire upkeep — refresh when buff < 4s or missing.
    uint32 sf  = GetSpell(Spell::SoulFire);
    uint32 isf = GetSpell(Spell::ImprovedSoulFireBuff);   // proc-aura ID
    if (sf && isf && target && bot->GetDistance(target) <= 40.0f)
    {
        Aura* a = bot->GetAura(isf);
        bool needRefresh = !a || a->GetDuration() < 4 * IN_MILLISECONDS;
        if (needRefresh && !IsOnCooldown(bot, sf))
            TryCast(bot, target, Spell::SoulFire);
    }
}
```

**Cast-pipeline gotchas to apply (CLAUDE.md §"Cast pipeline gotchas"):**
- Soul Fire is a long-cast spell (~3s base, ~2.5s with Bane); the
  `UNIT_STATE_CASTING` / `IsNonMeleeSpellCast(false)` guard already in
  `AffWarlockStrategy::Update` covers it (cast in progress → skip rotation).
- Improved Soul Fire buff ID is **a different spell ID** than the Soul Fire
  cast (verify via WDBXEditor — `cata-altbot/tools/dbc-extract.py`).
  Failure to disambiguate cast-vs-proc-aura is the exact passive-aura
  collision class in CLAUDE.md #1.
- Chaos Bolt is fire-school, so the LOS-and-facing pre-cast in
  `CastWithLog` already covers it. No new logic.

### Note on cache resolution and Conflagrate

If the bot ever needs to *not* cast Conflagrate even when it's known, the
right pattern is **don't cache it** — `Spell::Conflagrate` is omitted from
the enum entirely. The cache resolver only fills slots the enum declares,
so an unlisted spell can never be selected by `TryCast`. This sidesteps the
"do we even know it?" question and keeps the strategy honest.

---

## A.4. Unknowns and unmeasured items

| Item | Why unknown |
|---|---|
| Actual server-side talent grant behavior | Does the 142-point server grant 142 points *and* let the player place them under standard tier-prereq rules, or does it skip rules? The doc assumes standard rules — i.e., 31 in primary still gates the 41-cap and the secondary requirement. If rules are relaxed, the dual-tree assumption could break (e.g., bot allocated 25/25/25 + scattered tertiary). |
| Does `LearnTalent` enforce prereqs on bots? | `AltbotTalents::Learn` calls `Player::LearnTalent` which TC validates internally — but `AltbotTalents::Show` doesn't yet show the *tree-totals* breakdown the dual-tree detection needs. Detector design depends on this. |
| Demonic Pact triggering on AltbotCombatLog | Pact aura applies ~constantly during combat. The cast log will fire on every pet-crit. Suppression policy (filter pet auras) is not yet specified. |
| Build A bigger or smaller than Build B in raid? | No surveyed guide answers the 142-point question. The verdict in §A.1 is reasoning from first principles, not a parse-tested claim. The user should treat the "Build B is engineering-easier" conclusion as load-bearing, and the "personal DPS ranking" as best-effort speculation. |
| Conflagrate + Immolate-from-different-caster | TC issue #19790 ([G_TC19790]) indicates 3.3.5 Conflag rejects when Immolate is owned by another caster. 4.3.4 fork may behave differently. Not relevant for the recommended default behavior (Conflag off) but worth checking before flipping the fire-hybrid mode on. |

---

# Part B — Strategy code model trade-offs

Four candidate architectures for handling dual-tree spec recognition. The
trade-offs are scored against: testability, cache-resolution fit,
factory-dispatch impact, telemetry/debug clarity, and code-volume.

## B.1. Option 1 — Stay primary-tree-only (do nothing)

**What:** `AltbotStrategyFactory` keeps keying on `GetPrimaryTalentTree()`
alone. `AffWarlockStrategy` ignores Conflag, Chaos Bolt, Hand of Gul'dan,
Demonic Pact, everything. The 142-point bot plays a 31/0/10 rotation
forever.

**Pros:**
- Zero new code.
- The existing `affliction-warlock-guide-survey.md` reflects what the bot
  does; no docs drift.
- No risk of breaking working specs.

**Cons:**
- Leaks 10-25% DPS on every dual-tree bot. For a Felhunter-Aff with full
  Demo unlock, just `Master Demonologist` + `Demonic Aegis` is 15-20% damage
  uncaptured. For Aff+Destro, Chaos Bolt + Shadowburn on CD is a constant
  trickle.
- Underdelivers on the server-specific feature the user is paying for.
- Telemetry won't even show *that* the bot has secondary-tree spells —
  CastWithLog logs the spells it casts, not the spells it could.

**Verdict:** Acceptable as a no-op interim. Not the long-term answer.

## B.2. Option 2 — Detect secondary spells, unlock tiers in-place

**What:** `AffWarlockStrategy` keeps a single class. `ResolveSpellCache`
queries every spell the dual-tree builds might unlock. The tier dispatch
checks `if (GetSpell(Spell::ChaosBolt)) { Tier_ChaosBolt(...); }` — tiers
fall through when the spell isn't known. The strategy *itself* doesn't know
or care whether the bot is "Aff+Destro" or "Aff+Demo"; it asks the spellmap.

**Pros:**
- **Already aligned with how `ResolveSpellCache` works.**
  `FindSpellByFamilyName` returns 0 for unknown spells; tiers naturally
  short-circuit on `if (!id) return false;` (see `Tier_BaneOfDoom`'s
  `agony ? agony : doom` pattern). The factory is unchanged.
- One strategy class to test. A unit test seeds the bot's spellmap to a
  matrix of {has Chaos Bolt, has Hand of Gul'dan, has both, has neither}
  and confirms the tier output for each.
- Telemetry is the simplest possible: cast log shows what the bot cast; the
  source of that cast is implicit in the spec label "AffWarlock".

**Cons:**
- Class grows. A naive `AffWarlockStrategy` with Build-A and Build-B
  additions inlined would be ~600 lines; tier-dispatch logic gets a lot of
  guards. (The split-by-concern remedies below help.)
- "Free cleave + nuke" tiers don't make sense to wire to **other** primary
  trees (e.g., Demo+Aff is a different strategy class entirely). The
  conditional logic is intrinsically primary-Aff-shaped, so there's no clean
  reuse across class instances.
- Debug ergonomics: if Chaos Bolt isn't firing, the question "does the bot
  *know* Chaos Bolt?" has to be answered via the resolve-cache log line, not
  a spec name. The existing log already does this — cache-table dump on
  first tick — so this is fine, just call out the rule.

**Verdict:** The *natural* fit for the existing codebase. Lowest risk.
Lowest code-volume. The trade-off is class size and a slight loss of
"the strategy name tells you the build" identifier in logs.

## B.3. Option 3 — Hybrid strategy classes per dual-tree combo

**What:** New classes `AffDestroWarlockStrategy : public AffWarlockStrategy`
and `AffDemoWarlockStrategy : public AffWarlockStrategy` that inherit and
override `Update` (or just add tiers). The factory inspects both the
primary tree *and* a "secondary tree indicator" — talent point counts, or a
specific marker spell from the secondary tree (e.g., `bot->HasSpell(50796)`
= Chaos Bolt → Destro-secondary) — and picks the strategy.

**Pros:**
- Clear identity. Logs say "AffDestroWarlock" or "AffDemoWarlock" — you
  know what rotation is running by reading one line.
- Inheritance lets shared logic live in the base; Build-A-only and
  Build-B-only behavior live in their respective subclasses.
- Tests have natural per-strategy isolation.

**Cons:**
- **Factory dispatch becomes a matrix.** Today the factory is one switch on
  primary tree. To pick `AffDestroWarlockStrategy` it has to inspect both
  trees. The natural input is point counts per tree — but `GetTalentPoints`
  isn't exposed in the same convenient way as `GetPrimaryTalentTree`. The
  fallback is "infer from marker spells" — `if (bot->HasSpell(CHAOS_BOLT))
  return AffDestro` — which works but couples the dispatcher to specific
  spell IDs.
- The combo explosion: warlock has 3×3 = 9 viable combos, mage has 9,
  hunter has 9. Worst case is 27 strategy classes for 9 specs. In practice
  most combos pick the same secondary (Demo for Aff/Destro/etc.), so it's
  ~3-4 hybrids per spec. Still a lot of files.
- Inheritance-of-strategy is **the only place in the codebase** the strategy
  classes currently inherit beyond the abstract base. Adding "is-a"
  inheritance under `AltbotStrategy` is a design departure.
- Cache resolution duplicates: base resolves Aff spells; subclass resolves
  secondary spells. Either two `ResolveSpellCache` paths or a virtual hook,
  both more code than Option 2.

**Verdict:** Justified if telemetry / log-readability is a hard
requirement, OR if Build A and Build B rotations diverge so much that
in-class branches become unreadable. Per §A.3 the rotation diff is
modest — 2-3 new tiers — so the case is weak.

## B.4. Option 4 — Talent-aware overlay (composition)

**What:** Base `AffWarlockStrategy` provides the primary rotation. Overlay
objects (`AffDestroOverlay`, `AffDemoOverlay`) implement a small interface:

```cpp
class AltbotStrategyOverlay
{
public:
    virtual void   ResolveSpellCache(Player* bot)                  = 0;
    virtual bool   DoMaintenance(Player* bot, Unit* target)        = 0;
    virtual bool   RunInsertionTier(Player* bot, Unit* target,
                                    int tierBefore, int tierAfter) = 0;
    virtual char const* GetName() const                            = 0;
};
```

The strategy holds `std::vector<std::unique_ptr<Overlay>> _overlays` and at
each tier-dispatch step asks each overlay "want to insert here?". The
overlay is picked at factory time, same logic as Option 3 (`HasSpell`
markers), but it composes into one strategy rather than subclassing.

**Pros:**
- Tier insertions are first-class: an overlay can say "I want a tier between
  position 5 and 6" without subclassing.
- The same overlay can compose with **multiple primary strategies** — e.g.,
  a hypothetical "DestroOverlay" might be reused under a Demo-primary
  strategy that also reaches into Destro. Reuse > inheritance.
- Telemetry: overlay name shows up in `CastWithLog` alongside the spec
  label, so the cast log self-documents.

**Cons:**
- **Heaviest design.** The overlay interface needs to know where to insert,
  which means tier positions become a stable enum across all strategies —
  another contract to maintain.
- Premature abstraction risk: today only one primary spec (Aff) has known
  dual-tree value. Building an overlay system for one consumer is
  speculative.
- Cast-pipeline gotchas (CLAUDE.md §"Cast pipeline gotchas" #2-#4) apply
  per-cast, not per-strategy — every overlay still has to route through
  `CastWithLog`, respect GCD, respect cast-in-progress. The interface
  surface for "does this overlay want to cast something?" duplicates the
  tier-method shape we already have.
- More indirection in the per-tick hot path. `_overlays[i]->RunInsertionTier(...)`
  in a loop vs. `Tier_ChaosBolt(...)` direct call.

**Verdict:** Right answer for a much later phase, when 2-3 specs have
dual-tree variants and the same overlay (e.g., DemoOverlay) is being reused
across primary strategies. Premature today.

## B.5. Comparison at a glance

| Aspect | Opt 1 (no-op) | Opt 2 (in-class) | Opt 3 (subclass) | Opt 4 (overlay) |
|---|---|---|---|---|
| Code volume for AffDestro v1 | 0 lines | ~150 lines | ~250 lines | ~400 lines |
| Factory dispatch change | None | None | Inspect both trees | Inspect both trees |
| Plays with existing `ResolveSpellCache` | N/A | **Natural fit** | Two cache paths | One per overlay |
| Telemetry — "what rotation is running?" | "AffWarlock" only | "AffWarlock" only | "AffDestroWarlock" | "AffWarlock + DestroOverlay" |
| Telemetry — "which spec spell fired?" | Cast log shows ID | Cast log shows ID | Cast log shows ID | Cast log shows ID + overlay tag |
| Testability | Trivial (no change) | Spellmap matrix | Per-class | Per-overlay × per-base |
| Risk of regressing existing Aff bots | None | Low (in-class guards) | Medium (subclass override) | Medium (insertion contract) |
| Future-proof for 3rd-tree leftovers | N/A | Same pattern keeps working | Combo-explosion | Best (compose 2 overlays) |

---

# Part C — Cross-cutting concerns

## C.1. Other specs affected

The 2x talent budget hits every spec the same way: the standard 31/0/10 or
31/2/8 single-tree-deep build expands to a 41/36/X dual-tree-full build.
Three concrete high-impact examples for the specs we already have or have
queued strategies for:

### C.1.1. Frost Mage — secondary Arcane unlock

Stock Frost Mage at 71 points is **8/2/31** (G_AFF_IV-equivalent for mage:
Icy Veins Cata Frost build). The 8 Arcane points pick up `Improved
Counterspell` and `Netherwind Presence`. At 142 points the secondary tree
reaches:

- **Arcane Tier 4: `Improved Polymorph`** — +50% damage to target after
  Poly breaks. Useless in PvE.
- **Arcane Tier 5: `Arcane Power`** — 2-minute CD, 15s, +20% spell damage at
  +20% mana cost. **Major DPS cooldown.** Not in stock Frost rotation
  because you can't reach it from 8/2/31.
- **Arcane Tier 5: `Presence of Mind`** — 2-min CD, next cast (non-channel)
  is instant. Already known to bots via the talent calculator URL; not
  cast today.
- **Arcane Tier 7 (Arcane capstone): `Slow`** — irrelevant for PvE DPS.

For a Frost+Arcane hybrid, **Arcane Power on CD** is the meaningful
addition. The strategy would gain one tier at the top: `Tier_ArcanePower`
that fires when off-CD during a damage window. The `FrostMageStrategy`
cache resolver picks it up naturally — Arcane Power is name-uniquely
"Arcane Power" in `SPELLFAMILY_MAGE`.

Frost+Fire secondary (Tier 5 `Combustion`) is structurally similar but
collides badly with Frost's school priorities. Less interesting.

**Effort estimate:** Identical to Aff+Destro Option 2 — one new tier, one
new spell-cache entry, ~50 lines.

### C.1.2. MM Hunter — secondary Survival unlock

Stock MM at 71 points reaches Marksmanship Tier 7 (`Master Marksman`,
Aimed Shot proc). The standard 7/5/29 build dips into Beast Mastery for pet
buffs.

At 142 points, MM can fully reach **Survival Tier 6 `Black Arrow`** (a
DoT on 30s CD that procs Survival's `Lock and Load`) and **Survival Tier 7
`Explosive Shot`** (Survival's signature ability, free + instant during
Lock and Load procs). The hybrid is "MM single-target rotation + Black
Arrow on CD + Explosive Shot during LnL procs."

Caveat: `Lock and Load` procs from *traps and Black Arrow*. MM doesn't
typically place Immolation Trap, so the LnL proc surface is just Black
Arrow's tick chance. Per the Aff+Destro analysis pattern in §A.2: **the
hybrid loses Survival's main proc generation**, so the gain is just "Black
Arrow as an extra DoT on the priority list." That's worth ~5-8% DPS,
modest.

`MmHunterStrategy` extension: one new tier `Tier_BlackArrow`,
prioritized between `Tier_SerpentSting` and `Tier_AimedShot`.
Implementation ~30 lines.

### C.1.3. Resto Shaman — secondary Elemental unlock

Stock Resto reaches Restoration Tier 7 (`Tidal Waves` capstone) and dips
into Elemental for `Concussion`. At 142 points, the secondary unlock
brings:

- **Elemental Tier 4: `Elemental Mastery`** — 3-min CD, 18s, +20% spell
  haste. **Support-DPS cooldown for the bot's tier-3 DPS rotation.**
- **Elemental Tier 4: `Call of Flame`** — increases Fire damage. Trivial
  for a Resto bot whose only Fire spell is Flame Shock as a leveling
  filler.
- **Elemental Tier 5: `Lava Flows`** — 6/12/18% Flame Shock damage. Useful
  during the bot's support-DPS phase if Flame Shock is in rotation.

Resto's "DPS rotation when group is healthy" is small (one or two tiers in
`RestoShamanStrategy`). Adding Elemental Mastery as an on-CD support
cooldown is a single new method on the strategy. **The healer gating in
CLAUDE.md §"Adding a new spec" rule 4 — healers gate only their support-DPS
tier — still applies; Elemental Mastery fires only when the healer is in
DPS mode.**

### C.1.4. Generalization

Across these examples the pattern repeats: each hybrid unlocks **1-3 new
on-CD buttons + 1-2 passive damage modifiers + 0-1 utility cooldowns**.
None of them require a rotation overhaul. None of them collide with a
primary-tree DoT in the way Aff+Destro collides with UA/Immolate. The
worked example (Aff Warlock) is genuinely the hardest, which is why it's
the right vehicle to design against.

## C.2. Factory dispatcher

Today's `AltbotStrategyFactory::SlugFromBot` reads:

```cpp
uint32 tree = bot->GetPrimaryTalentTree(bot->GetActiveSpec());
// switch on (class, tree) → slug
```

If we adopt Option 2 (recommended in Part D), **the factory is unchanged.**
Aff hybrids still slug as `"affliction-warlock"`; the strategy class
detects secondary fill at `ResolveSpellCache` time.

If we adopt Option 3 or 4, the factory needs a secondary-tree input. Two
realistic paths:

**(a) Talent-tab point sums.** Need a helper:

```cpp
// In a new AltbotTalentInspect.{h,cpp}, or as a method on factory:
//   Returns {primaryTabId, primaryPoints, secondaryTabId, secondaryPoints}
//   sorted descending by points. Skips trees < threshold (default 16).
struct TabSpend { uint32 tabId; uint8 points; };
std::array<TabSpend, 3> GetTabSpend(Player* bot);
```

This requires walking the talent DB rows the bot has learned, summing
ranks weighted by tab. Cata exposes this via `Player::GetTalentMap()` (or
the equivalent — verify in `Player.h:GetTalentMap()` per the porting-notes
Phase 4 references in CLAUDE.md).

**(b) Marker-spell sniff.** Cheaper, fragile:

```cpp
// In AltbotStrategyFactory:
// "Has Chaos Bolt and primary is Aff" → AffDestro.
// "Has Demonic Pact and primary is Aff" → AffDemo.
constexpr uint32 SPELL_CHAOS_BOLT     = 50796;
constexpr uint32 SPELL_DEMONIC_PACT   = 47236;   // verify
```

Marker sniff is simpler but creates a dependency on never-renaming-a-spell
in the DBC. Talent-tab sum is more robust and matches the factory's
existing "tree ID" abstraction level.

## C.3. Telemetry — distinguishing 71-pt vs 142-pt runs

This is mostly a `AltbotCombatLog` concern (the structured telemetry sink
referenced in `StrategyUtil.cpp::CastWithLog`). Three signals are
distinguishing:

1. **Talent budget at session start.** Log a one-time line per bot per
   session of the form:

   ```
   altbot.combat[BotName] talents: primary=affliction(41) secondary=destruction(36) tertiary=demonology(5)
   ```

   This is the *single* indicator that tells you "yes, this is a dual-tree
   bot." Without it the combat log shows new spell IDs firing and you
   have to look up what spec they belong to.

2. **Cache-resolve log includes secondary spells.** The existing
   first-tick log in `AffWarlockStrategy::Update` already dumps every
   cached spell ID. After the proposed changes it would print
   `ChaosBolt=50796 Shadowburn=17877 SoulFire=6353` for a Build-A bot and
   zeros for a stock 71-pt bot. This is automatic — it just needs the new
   enum entries.

3. **Per-cast tier tags.** `CastWithLog` already takes `tierIdx` /
   `tierName`. The new tiers (`Tier_ChaosBolt`, etc.) need their own
   tierNames so the combat log distinguishes a Build-A Aff bot's cast
   distribution from a stock Aff bot's. The cast count for `tierName="ChaosBolt"`
   being nonzero is a direct, log-greppable proxy for "this bot has the
   Destro hybrid active."

The minimum useful telemetry surface is **(1) alone**. (2) and (3) are
already implied by the proposed code changes.

---

# Part D — Recommended path forward

## D.1. Recommendation

**Adopt Option 2 (detect secondary spells, unlock tiers in-place).** Phase
the work so **Aff+Destro lands first** as the harder design case, validate
in-game and via combat-log review, then **Aff+Demo lands second** as the
mostly-passive follow-up.

Why Option 2:

1. **Lowest-risk extension of working code.** No factory changes, no class
   hierarchy changes, no new architectural primitives. The existing
   `ResolveSpellCache` + `Tier_*` chain handles "spell known? short-circuit
   if not" naturally — Aff's `Tier_BaneOfDoom` already does the
   `doom ? doom : agony` fallback for the very same shape of problem.
2. **The dual-tree rotation diff is genuinely small** per §A.3 — 2-3 new
   tiers. The "subclass per hybrid" cost in Option 3 isn't justified by
   the diff size.
3. **Telemetry is sufficient.** Per §C.3 the cache-resolve log already
   surfaces which spells the bot will use; a one-time talent-budget log
   line at session start (small, isolated change) plus the existing
   per-cast tier tags closes the observability gap.
4. **Falls back gracefully if the user changes their mind.** If Option 3
   later turns out to be needed, the per-tier methods in Option 2 lift
   cleanly into subclass overrides. If Option 4 turns out to be needed
   later, the same tier methods become Overlay implementations. The
   in-class step is a strict refactor predecessor to either.

Phase A first because it's the harder design case (UA/Immolate conflict,
Conflagrate analysis). Once Build A is shipped and the user has confirmed
combat-log shows Chaos Bolt firing alongside the standard DoT priority,
Build B drops in easily because it's mostly passive.

## D.2. First concrete coding work

**Files to change** (no code yet — paths and signatures only):

### D.2.1. `src/strategies/AffWarlockStrategy.h`

Extend the `Spell` enum:

```cpp
enum class Spell : uint8_t
{
    // ... existing 19 entries unchanged ...
    // --- Build A (Aff+Destro) additions ---
    ChaosBolt,           // 50796
    Shadowburn,          // 17877
    SoulFire,            // 6353  (cast spell)
    ImprovedSoulFire,    // proc-aura ID; resolve via FindBotAuraByName pattern
    BaneOfHavoc,         // 80240
    Count
};
```

Add new private tier methods:

```cpp
bool Tier_ChaosBolt(Player* bot, Unit* target) const;
bool Tier_Shadowburn(Player* bot, Unit* target) const;
bool Tier_BaneOfHavocCleave(Player* bot, Unit* target) const;
void MaintainImprovedSoulFire(Player* bot, Unit* target);
```

### D.2.2. `src/strategies/AffWarlockStrategy.cpp`

- Extend `ResolveSpellCache` with the new entries; reuse
  `StrategyUtil::FindSpellByFamilyName(bot, SPELLFAMILY_WARLOCK, "...")`
  for every named spell. The Improved Soul Fire proc-aura ID is name-shared
  with the passive talent — pattern after `FrostMageStrategy::FindBotAuraByName`
  with `info->IsPassive()` filter (CLAUDE.md cast-pipeline-gotchas #1).
- Call `MaintainImprovedSoulFire(bot, target)` from `DoMaintenance` after
  the existing armor/circle/CotE block.
- Insert the new tiers in `Update`'s tier dispatch in this order
  (priorities are tentative; tune via combat log):

  ```cpp
  if (Tier_Haunt(bot, target))                  return;
  if (Tier_BaneOfDoom(bot, target))             return;
  if (Tier_Corruption(bot, target))             return;
  if (Tier_UnstableAffliction(bot, target))     return;
  if (Tier_AoE(bot, target))                    return;
  if (Tier_BaneOfHavocCleave(bot, target))      return;   // NEW
  if (Tier_ChaosBolt(bot, target))              return;   // NEW
  if (Tier_Shadowburn(bot, target))             return;   // NEW (execute)
  if (Tier_DrainSoul(bot, target))              return;
  Tier_ShadowBolt(bot, target, mode);
  ```

  Note `Tier_Shadowburn` lives *above* `Tier_DrainSoul` because Shadowburn
  is instant and refunds a shard on kill — strictly better than Drain Soul
  while both are available in execute. (Cite: G_DES_WT and G_DES_OG both
  list Shadowburn above Drain Soul for execute when present.)

- **Each new tier follows the cast-pipeline gotchas** (CLAUDE.md §"Cast
  pipeline gotchas"):
  - Route through `StrategyUtil::CastWithLog` only (gotcha #2).
  - The existing `UNIT_STATE_CASTING` / `IsNonMeleeSpellCast(false)` guard
    in `Update` covers Soul Fire's 3s cast and Chaos Bolt's 2-2.5s cast
    (gotcha #3).
  - The existing `HasGlobalCooldown` probe via Shadow Bolt covers the GCD
    case (gotcha #4) — no change needed.

### D.2.3. `src/AltbotCombatLog.{h,cpp}` (one-line change for telemetry)

At session start (first cache resolve), log a single line of the form
documented in §C.3.(1). The existing cache-resolve `TC_LOG_DEBUG` already
covers what's needed; add a sibling `TC_LOG_INFO` with the talent breakdown
for grep convenience. Requires a `GetTabSpend(bot)` helper:

```cpp
// src/AltbotTalentInspect.h  (NEW header, no impl yet)
namespace AltbotTalentInspect
{
    struct TabSpend { uint32 tabId; uint8 points; };
    // Returns {primary, secondary, tertiary} sorted descending by points.
    // Trees with 0 points get tabId=0, points=0 in trailing slots.
    std::array<TabSpend, 3> Get(Player* bot);
    char const*             SlugForTab(uint8 cls, uint32 tabId);
}
```

`Get` walks the player's talent map; `SlugForTab` is a small switch
mirroring the constants already in `AltbotStrategyFactory.cpp`.

### D.2.4. Test plan

In-game smoke test on a level-85 Aff warlock bot with talents allocated
roughly **41 Aff / 36 Destro / 5 Demo**:

1. **Spell resolution.** First-tick log line should show non-zero values for
   `ChaosBolt`, `Shadowburn`, `SoulFire`, `BaneOfHavoc`. If any read 0, the
   talents weren't actually placed → reseed via `.altbot talents learn`.
2. **Improved Soul Fire upkeep.** After pull, bot should cast Soul Fire
   once, then re-cast every ~16s (4s before the 20s buff expires). Combat
   log should show `tierName="maint"` for the Soul Fire cast.
3. **Chaos Bolt cadence.** Bot should fire Chaos Bolt every 12s (CD-bound)
   on single-target. The cast log `tierName="ChaosBolt"` count should be
   roughly `fight_duration_seconds / 12`.
4. **Shadowburn replaces Drain Soul.** When target HP drops below ~20%,
   instead of channeling Drain Soul the bot should burst Shadowburn while
   continuing Shadow Bolt filler. (At 20-25% HP both are valid; below 20%
   Shadowburn wins.)
5. **Regression check.** A stock 31/0/10 Aff bot on the same server build
   should behave identically to before. The cache-resolve log for that bot
   should show `ChaosBolt=0 Shadowburn=0 SoulFire=0 BaneOfHavoc=0`, and
   no new-tier cast log lines should appear.

### D.2.5. Phase 2 — Aff+Demo (after Phase 1 validated)

Repeat for Build B with:

```cpp
enum class Spell : uint8_t
{
    // ... entries from Phase 1 ...
    DemonicEmpowerment,   // 47193
    HandOfGuldan,         // 71521
    Count
};
```

`Tier_HandOfGuldan` inserts in the same priority slot as `Tier_ChaosBolt`
(they're mutually exclusive in practice — a bot will have at most one of
the secondaries fully built). `DemonicEmpowerment` lives in
`PetMaintenance` (60s pet CD).

Demonic Aegis (Fel Armor SP+30%) and Master Demonologist (+15% damage
while Felhunter active) are pure passives — no code is needed; the bot
already maintains Fel Armor and runs Felhunter. **They just work** once
the talents are allocated.

Demonic Pact log spam (pet-crit-triggered raid aura) should be filtered at
the AltbotCombatLog sink, not the strategy. File a follow-up if it shows
up as noise in Phase 2 testing.

### D.2.6. Out of scope for this design

- **Conflagrate / Immolate fire-hybrid rotation.** Per §A.2, this requires
  a full re-derivation of the Aff DoT priority and reaps Backdraft only
  if filler swaps to Incinerate. Not in Phase 1, not in Phase 2. If the
  user wants a fire-hybrid playstyle later, it's a separate strategy
  (`AffDestroFireHybridWarlockStrategy`, dispatched via marker spell —
  fitting the Option 3 model). Treat that as an entirely new spec.
- **Talent spend automation.** The bot needs talents *placed* for any of
  this to work. `AltbotTalents::Learn` exists for manual placement; an
  auto-spend policy for the 142-point Aff+Destro / Aff+Demo build is
  out of scope for this design doc.
- **Other specs' dual-tree strategies.** Frost+Arcane, MM+Survival,
  Resto+Elemental are all foreshadowed in §C.1; their own design doc /
  guide-survey should land when those specs are scheduled.

---

## D.3. Hand-off checklist

- [ ] Read this doc end to end.
- [ ] Verify Improved Soul Fire **proc-aura spell ID** via WDBXEditor
  (`tools/dbc-extract.py`) — it's distinct from the cast spell ID and from
  the talent ID. Pattern after Brain Freeze / Fingers of Frost
  disambiguation in `FrostMageStrategy.cpp`. Note ID in
  `docs/research/dbc-verification-checklist.md`.
- [ ] Confirm `AltbotTalentInspect::Get(bot)` is viable against the 4.3.4
  fork's `Player::GetTalentMap()` API; if not, fall back to the marker-spell
  sniff per §C.2(b) for the telemetry log line only (don't let it gate
  strategy correctness).
- [ ] Sketch the Phase 1 patch as a single PR: enum extension, new tiers,
  ResolveSpellCache update, maintenance hook. Keep the diff under 250
  lines for review.
- [ ] Test plan in §D.2.4. If anything fails the regression check, the
  Option 2 in-class approach is the wrong call and Option 3 has to be
  revisited.

End of design.

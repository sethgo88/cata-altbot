# 142-Point Affliction Warlock — Recommended Build

**Scope.** Bot-executable max DPS, Affliction Warlock primary, on the
`cata-altbot` Trinity-Core 4.3.4 server with the 2x talent-points config
(142 points at level 85 instead of the stock 71). Survey date 2026-05-12.

**This doc is a refocus of `docs/research/2x-talent-strategy-design.md` Part A.**
That doc covers the strategy-model side and is the audit trail for the build
choices here; this doc is the concrete "what build, what spells, what tier
order" companion. `§A.2` etc. references the 2x-talent doc unless scoped.
For the 71-pt baseline see `docs/research/affliction-warlock-guide-survey.md`.

**Hard constraints baked in.** One hybrid build (not separate ST/AoE trees);
bot-executable rotation only (no skill-based weaving, no proc reactions
faster than the 1.5s tick); Affliction primary; no Conflagrate/Immolate
fire-hybrid rotation (§A.2 verdict — out of scope).

---

## §0. Sources

Strategy/rotation claims reconciled across strategy-bearing guides; hard
numbers (spell IDs, CDs, durations) draw on wiki/Wowhead sources.

| # | Guide | URL | Confidence |
|---|---|---|---|
| G_AFF_IV   | Icy Veins — Aff Warlock Talents & Glyphs | https://www.icy-veins.com/cataclysm-classic/affliction-warlock-pve-spec-builds-talents-glyphs | High (strategy-bearing) |
| G_DES_IV   | Icy Veins — Destro Warlock Talents & Glyphs | https://www.icy-veins.com/cataclysm-classic/destruction-warlock-pve-spec-builds-talents-glyphs | High (strategy-bearing) |
| G_DEM_IV   | Icy Veins — Demo Warlock Talents & Glyphs | https://www.icy-veins.com/cataclysm-classic/demonology-warlock-pve-spec-builds-talents-glyphs | High (strategy-bearing) |
| G_AFF_WT   | Warcraft Tavern — Aff Talents & Glyphs | https://www.warcrafttavern.com/cataclysm/guides/pve-affliction-warlock-talents-builds-glyphs/ | High (strategy-bearing) |
| G_DES_WT   | Warcraft Tavern — Destro Rotation & CDs | https://www.warcrafttavern.com/cataclysm/guides/pve-destruction-warlock-rotation-cooldowns/ | High (strategy-bearing) |
| G_BANE     | Wowhead Cata — Bane | https://www.wowhead.com/cata/spell=17788/bane | High (hard numbers) |
| G_SAF      | Wowhead Cata — Shadow and Flame | https://www.wowhead.com/cata/spell=17801/shadow-and-flame | High (hard numbers) |
| G_ISF      | Warcraft Wiki — Improved Soul Fire | https://warcraft.wiki.gg/wiki/Improved_Soul_Fire | High (hard numbers; 4.3 patch note: 20s) |
| G_BD       | Wowpedia — Backdraft | https://wowpedia.fandom.com/wiki/Backdraft | High (hard numbers) |
| G_DP       | Wowpedia — Demonic Pact | https://wowpedia.fandom.com/wiki/Demonic_Pact | High (hard numbers; flat 10% SP) |
| G_BOH      | Wowpedia — Bane of Havoc | https://wowpedia.fandom.com/wiki/Bane_of_Havoc_(warlock_talent) | High (hard numbers; 15% copy, 60s) |
| G_CHAOS    | Wowhead Cata — Chaos Bolt | https://www.wowhead.com/cata/spell=50796/chaos-bolt | High (12s CD, 2.5s cast, ignores absorbs) |
| G_SBURN    | Wowhead Cata — Shadowburn | https://www.wowhead.com/cata/spell=17877/shadowburn | High (5s CD, ≤20% exec, shard refund) |
| G_SURVEY   | `docs/research/affliction-warlock-guide-survey.md` | (internal) | High (baseline 71-pt survey + glyphs) |
| G_2X       | `docs/research/2x-talent-strategy-design.md` | (internal) | High (audit trail for build choices) |

Coverage gap: no surveyed guide directly addresses doubled talent budgets;
the dual-tree build is novel to this server and reconstructed from each
tree's standard guide. Per G_2X §A.4 the rotation diff is small, but the
Build-A-vs-Build-B personal-DPS comparison has no parse data.

---

## §A. Two realistic builds — short summary

> Condensed restatement of G_2X §A.1; that section is the audit trail.

Standard Cata tier-prereq rules apply (G_2X §A.1): 31 in primary unlocks
the 41-point capstone *and* gates a fillable secondary tree. Arithmetic:
41 primary + 36 secondary + 5 tertiary = 82 active; remaining 60 of 142
absorb into low-value picks. Anything beyond the rotation-relevant rows
is filler.

| Build | Primary (41) | Secondary (36) | Tertiary | Net DPS character |
|---|---|---|---|---|
| **A — Aff + Destro** | Aff full, Haunt capstone | Destro to **Chaos Bolt** capstone | ~5 Demo (Demonic Embrace HP, Dark Arts pet damage) | Personal DPS — adds a 12s-CD nuke and an instant execute. Shadow Bolt filler unchanged. |
| **B — Aff + Demo**  | Aff full, Haunt capstone | Demo to **Hand of Gul'dan** capstone | ~5 Destro (Bane + Shadow and Flame still get picked up here as a leftover) | Group DPS — Demonic Pact 10% raid SP + Master Demonologist +15% personal. Hand of Gul'dan is one extra on-CD button. |

Build A vs Build B is decided in §B. Build A's Conflagrate/UA-Immolate
collision is fully analyzed in G_2X §A.2 — verdict: **don't cast
Conflagrate, don't cast Immolate, don't drop UA.** Pick up Chaos Bolt and
Shadowburn cleanly above the existing tier chain instead. Load-bearing
for this doc; do not relitigate.

---

## §B. The decisive question — which secondary?

Per G_2X §A.1 final table, the two builds trade off this way:

| Criterion | Build A (Aff+Destro) | Build B (Aff+Demo) |
|---|---|---|
| Personal DPS — single-target | Chaos Bolt on 12s CD; Shadowburn execute below 20%. Improved Soul Fire +8% personal haste if SF kept up. | Master Demonologist +15% spell damage (Felhunter active). Demonic Aegis +30% Fel Armor SP bonus. Hand of Gul'dan on 15s CD. |
| Personal DPS — AoE | Bane of Havoc adds cleave by copying 15% damage to a secondary target. | No real AoE addition. |
| Group utility | Aftermath daze (irrelevant for PvE). Shadow and Flame -5% crit debuff (already in stock 31/0/10). | **Demonic Pact +10% SP raid aura, 45s** (G_DP). Huge if there are other casters in the group. |
| Rotation engine cost | Chaos Bolt is a clean addition (cooldown-bound, no DoT conflict). Shadowburn slots into execute. Two new tiers. | Mostly passive bonuses; Hand of Gul'dan is one button. Demonic Empowerment is a 60s pet CD. |
| Implementation difficulty | Easy (no Conflag, no UA swap — see §A.2 verdict) | Easy (mostly passive) |

G_2X §A.1 calls Build B "engineering-easier" because most of the gain is
passive (no rotation diff), and flags that Demonic Pact dominates in raids
but is much weaker in 5-mans. Both verdicts are correct.

### Applying the "bot-executable max DPS" framing

1. **Bot-executable** means anything the tier dispatcher handles on a 1.5s
   tick. Chaos Bolt (12s CD nuke) and Shadowburn (5s CD instant on ≤20%
   HP target) are exactly this shape — slot into the tier chain trivially.
2. **Max DPS for this bot** means the bot's own DPS first. Demonic Pact
   helps the *group*, not the warlock's personal numbers.

Combined verdict:

- **5-man content (project's primary use case — leveling 1-85 LFD + all
  Cata 5-mans):** pick **Build A (Aff+Destro)**. In a 5-man with one
  caster, Demonic Pact's 10% SP buffs at most 1-2 group members (melee
  zero, paladin partial). Chaos Bolt + Shadowburn adds executable
  personal DPS regardless of group comp.
- **25-man raid (eventually in scope):** pick **Build B (Aff+Demo)**.
  With 4-8 casters, Pact's 10% SP raid aura dominates anything Chaos
  Bolt adds personally. Personal DPS drops but raid total rises.

**This doc recommends Build A** because 5-man content is the primary use
case. Build B is a follow-up phase per G_2X §D.2.5.

---

## §C. Recommended build (Aff+Destro variant)

### §C.1. Tree breakdown

| Tree | Points | What it gets |
|---|---|---|
| Affliction | 41 | Full primary tree to Haunt capstone. Same picks as the 71-pt baseline in `affliction-warlock-guide-survey.md` §3 + filling in T4-T7 talents not reachable at 31 points. |
| Destruction | 36 | Bane 3/3, Shadow and Flame 3/3 (both already in stock build), plus everything that unlocks at T3+ that helps an Aff filler/nuke: Improved Soul Fire 3/3, Backdraft (skipped — see note), Shadowburn 1/1, Bane of Havoc 1/1, Chaos Bolt 1/1 (capstone). |
| Demonology | 5 (tertiary) | T1 only: Demonic Embrace 3/3 (HP), Dark Arts 2/3 (pet damage). Cheap stat bumps with no rotation cost. |

### §C.2. Point-by-point talent picks

**Affliction (41 points)** — full tree. Standard 31-pt picks from G_SURVEY
§3 + G_AFF_IV, extended to the T5-T7 talents the stock build can't reach:

| Tier | Talent | Ranks | Rationale |
|---|---|---|---|
| T1 | Improved Life Tap | 2/2 | +20% Life Tap mana return. Standard pick. |
| T1 | Doom and Glooom | 2/2 | -1.0s Bane of Doom / Agony cast. Standard pick. |
| T2 | Improved Corruption | 3/3 | +5% periodic damage per rank on Corruption. **Highest priority** Aff talent. |
| T2 | Jinx | 2/2 | CotE applies to nearby mobs (group utility). Standard pick. |
| T3 | Soul Siphon | 2/2 | +6% damage to drains per affliction effect on target. Standard pick. |
| T3 | Eradication | 3/3 | 6% per Corruption tick → +20% haste for 10s. Passive RNG. Standard pick. |
| T4 | Soul Swap | 1/1 | Talented baseline tool. Bot doesn't actively pre-spread today (see §E) but the talent is taken because of the future glyph + 41-pt requirement. |
| T4 | Siphon Life | 2/2 | Corruption ticks heal you for 0.5%. Sustain. |
| T4 | Curse of Exhaustion | 1/1 | (single point sink — utility) |
| T5 | Nightfall | 2/2 | Corruption can proc instant Shadow Bolt. Standard pick. |
| T5 | Shadow Embrace | 3/3 | SB+Haunt apply +5%/stack shadow DoT debuff (3-stack). Standard pick. |
| T5 | Improved Howl of Terror | 2/2 | -2.0s Howl cast → instant. Defensive value. |
| T6 | Pandemic | 1/1 | Drain Soul refreshes UA in execute. Standard pick. |
| T6 | Everlasting Affliction | 3/3 | SB / Haunt / Drain Soul refresh Corruption. Standard pick — eliminates re-Corruption upkeep. |
| T7 | **Haunt (Affliction capstone)** | 1/1 | The whole point of the tree — +20% shadow DoT damage on target. Highest-priority cast on the rotation list. |

Points spent: 30 visible on rotation-relevant picks; remaining 11 sink into
single-point fillers (Improved Drain Soul, etc.) — none rotation-relevant.

**Destruction (36 points)**:

| Tier | Talent | Ranks | Rationale |
|---|---|---|---|
| T1 | Bane | 3/3 | -0.5s on Shadow Bolt / Soul Fire / Chaos Bolt / Immolate cast time (G_BANE). **Critical** — buffs filler. |
| T1 | Aftermath | 2/2 | Daze on Conflag. Daze is irrelevant in PvE for an Aff bot that won't cast Conflag — single-point sink only because it unlocks T2. |
| T2 | Shadow and Flame | 3/3 | +12% Shadow Bolt damage + 100% chance for 5% spell-crit debuff for 30s (G_SAF). **Critical** — buffs filler. |
| T2 | Improved Immolate | 0/3 | **Skip** — bot doesn't cast Immolate (§A.2 verdict). |
| T3 | Improved Soul Fire | 3/3 | 8% spell haste for 20s after Soul Fire cast (G_ISF). **Maintained** as a buff — see §D and §G.2. |
| T3 | Bane of Havoc (T3 of Destro tree in 4.3) | 1/1 | Cleave tool — 15% damage copy to secondary target, 60s, 30s CD (G_BOH). Used in cleave mode. |
| T4 | Empowered Imp | 0/3 | **Skip** — Imp is not the Aff bot's pet (Felhunter is). |
| T4 | (single sink to advance) | 1 | Token point to advance to T5. |
| T5 | Backdraft | 0/3 | **Skip** — Backdraft only buffs Incinerate and Chaos Bolt, but the bot's filler is Shadow Bolt and Chaos Bolt cast is already on 12s CD. Without Conflag to proc it, Backdraft has nothing to consume. See G_2X §A.2 for the full reasoning. |
| T5 | Shadowburn | 1/1 | Execute spell — instant, 5s CD, ≤20% target HP, shard refund on kill (G_SBURN). |
| T6 | Burning Embers | 0/3 | **Skip** — buffs Imp/Infernal damage, neither is the standing pet. |
| T7 | **Chaos Bolt (Destro capstone)** | 1/1 | The whole point of the tree — 12s CD, 2.5s cast (2.0s with Bane 3/3), fire-school nuke that ignores absorbs (G_CHAOS). |

Rotation-relevant: 14 points. Remaining 22 sink into prereq-tax single-point
chains to reach T5 → T7 and low-value picks (Aftermath, Improved Searing
Pain, Pyroclasm) — none touch the rotation.

**Backdraft is intentionally skipped** (G_2X §A.2): Backdraft requires
Conflagrate to trigger, and even if triggered it only buffs Incinerate /
Chaos Bolt — but Aff's filler is Shadow Bolt, and Chaos Bolt is on a 12s
CD vs Backdraft's 15s/3-charge window. The buff has no rotation surface
to consume; the 3 points sink elsewhere with no DPS loss.

**Demonology (5 points, tertiary)**:

| Tier | Talent | Ranks | Rationale |
|---|---|---|---|
| T1 | Demonic Embrace | 3/3 | +9% stamina. Free survival. |
| T1 | Dark Arts | 2/3 | Pet damage / cast speed. Free pet DPS. |

**Total: 41 + 36 + 5 = 82 active.** The remaining 60 points (out of 142)
are server-policy filler; they don't change the rotation or the build's
character. See §H for the unknown around whether the server even
enforces tier-prereq when granting the 142.

### §C.3. Glyphs

Lifted from G_AFF_IV and G_SURVEY §3 (5/5 guide consensus); none change
for the 142-pt build because the glyph picks key on what the rotation
*casts*, not on what the talent tree spends.

**Prime glyphs (3):**

| Glyph | Effect |
|---|---|
| Glyph of Haunt | +3% Haunt damage. |
| Glyph of Corruption | 4% chance per Corruption tick to instant-cast Shadow Bolt. |
| Glyph of Unstable Affliction | -0.5s UA cast. |

**Major glyphs (3):**

| Glyph | Effect |
|---|---|
| Glyph of Life Tap | +20% Spirit for 20s after Life Tap. |
| Glyph of Shadow Bolt | -10% Shadow Bolt mana cost. |
| Glyph of Soul Swap | Soul Swap copies DoTs instead of removing them. |

**Minor glyphs (3):**

| Glyph | Effect |
|---|---|
| Glyph of Drain Soul | 50% increased shard refund chance. |
| Glyph of Unending Breath | +20s underwater breath. |
| Glyph of Ritual of Souls | Ritual no longer requires a soul shard. |

### §C.4. Talent allocation — source of truth

**No third-party calculator URL.** Wowhead's Cata Classic talent calc
enforces stock-Cata rules (41-point primary cap, 71 total) and cannot
represent a 142-point dual-tree build. The §C.2 point-by-point table above
is the build of record.

Allocate in-game via `.altbot talents learn <talentId> <rank>` for each row
in §C.2. The current `.altbot talents show <bot>` command only reports
active spec + remaining points; a richer worldserver-logged dump of the
full talent placement is a queued follow-up (so you can grep the actual
build out of `worldserver.log` rather than transcribing from in-game chat).

---

## §D. Rotation tier order (Aff+Destro)

The existing Update() tier chain in `AffWarlockStrategy.cpp:123-129` is:

```cpp
if (Tier_Haunt(bot, target))               return;     // line 123
if (Tier_BaneOfDoom(bot, target))          return;     // line 124
if (Tier_Corruption(bot, target))          return;     // line 125
if (Tier_UnstableAffliction(bot, target))  return;     // line 126
if (Tier_AoE(bot, target))                 return;     // line 127
if (Tier_DrainSoul(bot, target))           return;     // line 128
Tier_ShadowBolt(bot, target, mode);                    // line 129
```

After the build lands (per G_2X §A.3 and §D.2.2):

```cpp
if (Tier_Haunt(bot, target))               return;     // existing
if (Tier_BaneOfDoom(bot, target))          return;     // existing
if (Tier_Corruption(bot, target))          return;     // existing
if (Tier_UnstableAffliction(bot, target))  return;     // existing
if (Tier_AoE(bot, target))                 return;     // existing
if (Tier_BaneOfHavocCleave(bot, target))   return;     // NEW — cleave only
if (Tier_ChaosBolt(bot, target))           return;     // NEW — single-target nuke on CD
if (Tier_Shadowburn(bot, target))          return;     // NEW — execute phase
if (Tier_DrainSoul(bot, target))           return;     // existing
Tier_ShadowBolt(bot, target, mode);                    // existing — filler
```

Plus a maintenance call inside `DoMaintenance` for Improved Soul Fire
upkeep (the `MaintainImprovedSoulFire(bot, target)` hook per G_2X §A.3).

**Why this order:**

1. **Haunt at top** — refreshed on CD; carries +20% shadow-DoT debuff that
   every tick rides on. (Existing.)
2. **DoTs next (BoD → Corruption → UA)** — DoT priority dominates Aff's
   damage. Everlasting Affliction refreshes Corruption via SB / Drain
   Soul / Haunt; bot only re-casts on swap or dispel. (Existing.)
3. **AoE (SoC) before cleave/nuke** — 3+ packs win more from SoC than
   Chaos Bolt. (Existing; threshold = 3+ mobs within 10y of target.)
4. **Bane of Havoc cleave (NEW)** — exactly one secondary mob nearby, BoH
   not up. 60s duration / 30s CD; one cast per pack. Above Chaos Bolt
   because BoH's value comes from *subsequent* damage being copied.
5. **Chaos Bolt (NEW)** — 12s-CD nuke. Above Shadowburn because it's the
   bigger single cast and isn't HP-gated.
6. **Shadowburn (NEW)** — instant execute ≤20% HP. Above Drain Soul:
   instant + shard refund beats a 5s channel. G_DES_WT / G_DES_OG both
   list Shadowburn above Drain Soul; the priority carries.
7. **Drain Soul (existing)** — execute channel while Shadowburn is on CD.
8. **Shadow Bolt filler (existing)** — runs every other tick.

**Insertion mechanics.** New tiers follow the existing `Tier_*` shape —
short-circuit on missing cache / cooldown. Bots without the talents
(regression: stock 71-pt Aff) read zero in the new slots and fall
through unchanged. This is G_2X §B.2's "Option 2 in-place detection"
pattern.

---

## §E. Bot-executable caveats

The gap between "theoretical max-DPS Aff warlock" and "what the dispatcher
runs." Known gaps, not bugs to fix in Phase 1.

### §E.1. Soul Swap (DoT pre-spread) — not implemented

Soul Swap is the gold-standard target-swap / cleave-spread tool: snapshot
DoTs off A, reapply on B within 30s; with Glyph of Soul Swap the snapshot
*copies* instead of removes. The bot doesn't use it because correct
pre-spread requires predicting the master's next target — the dispatcher
only sees `master->GetTarget()` (no pre-swap event hook). The build
*takes* Soul Swap (Tier 4 talent, needed to reach T5+); the rotation just
doesn't fire it. Documented gap.

### §E.2. Pet swap (Felhunter ↔ Imp) — not implemented

Theoretical max-DPS swaps to Imp for cleave and back to Felhunter for
sustained ST. Out of scope: 6s summon cast + mana cost = ~12s of damage
traded; no "cleave-mode" signal so we'd thrash pets every 3+ mob pull.
Bot stays on Felhunter (G_SURVEY §3).

### §E.3. Mannoroth's Fury — not relevant for Felhunter

Mannoroth's Fury is a Demon Soul: Felguard ability (Demonology). The Aff
bot runs Felhunter, so it's never available. Listed for completeness.

### §E.4. Bane of Havoc target-tagging

BoH copies 15% of damage to a different nearby mob (G_BOH). Anchor
target is `master->GetTarget()` → natural primary = focus mob = correct.
**Edge case:** master swaps targets mid-cleave while BoH is on the
*original* primary — the bot's DoTs follow the swap but BoH's copy
points at the original until it expires or dies, then `target->HasAura(boh)`
fails and BoH re-applies on the new primary. Reapply latency acceptable
in Phase 1.

### §E.5. Demonic Pact log spam — Build B only

G_2X §A.4: Pact aura applies on every pet-crit, noisy in combat log. If
the user flips to Build B, filter at AltbotCombatLog not the strategy.
Out of scope for Phase 1.

### §E.6. Proc reactions — not faster than tick

Nightfall (instant SB from Corruption tick) and Eradication (+20% haste
10s from Corruption tick) are passive RNG procs — the filler tier fires
the instant SB it sees, the haste makes the next cast faster on its own.
No detection needed.

---

## §F. Side-by-side — stock 31/0/10 → recommended 41/36/5

| Aspect | Stock 31/0/10 (G_SURVEY) | Recommended 41/36/5 (this doc) |
|---|---|---|
| Aff capstone | Haunt (T7, 31 pts) | Haunt (T7, 41 pts — full tree) |
| Destro picks | Bane 3/3, Shadow and Flame 3/3, optional Improved Soul Fire 3/3 | Same + Bane of Havoc 1/1 + Shadowburn 1/1 + **Chaos Bolt 1/1** capstone |
| Demo picks | 0 | Demonic Embrace 3/3 + Dark Arts 2/3 |
| Single-target rotation | Haunt → BoD → Corruption → UA → AoE → Drain Soul → Shadow Bolt | Same + **Chaos Bolt** on CD + **Shadowburn** in execute + **Bane of Havoc** on cleave |
| Personal DPS gain | baseline | +Chaos Bolt every 12s; +Shadowburn replaces Drain Soul below 20% HP for instant burst; +Bane of Havoc on 2-target cleave (~15% extra) |
| Group utility gain | baseline (Curse of the Elements, Dark Intent) | unchanged (Aff utility doesn't extend with Destro picks) |
| Talent points spent | 31 + 10 = 41 (the stock 71-pt baseline) | 41 + 36 + 5 = 82 active (60 filler) |

**Net effect (back-of-envelope, 5-min single target, no parse data):**
filler ≈ 40% of damage, unchanged. Build adds ~25 Chaos Bolts (5min ×
60s / 12s) + ~8-10 Shadowburns in execute. Chaos Bolt ≈ 2x SB filler
damage. Rough: **+10-15% personal single-target DPS.** Cleave (2-target):
BoH adds **+12-15%** from the secondary copy. No parse data — see §H.

---

## §G. Implementation pointers

These are paths into the existing strategy code. **No code is written
here; this is the pointer list for whichever agent picks up Phase 1.**

### §G.1. `src/strategies/AffWarlockStrategy.h` — enum extension

Current enum at lines 20-42 ends with `HealthFunnel, Count`. Insert
new entries before `Count`:

```cpp
enum class Spell : uint8_t
{
    // ... existing 19 entries unchanged through HealthFunnel ...
    // --- Build A (Aff+Destro) additions ---
    ChaosBolt,           // 50796
    Shadowburn,          // 17877
    SoulFire,            // 6353  (cast spell)
    ImprovedSoulFire,    // proc-aura ID — verify via WDBXEditor; see §G.4
    BaneOfHavoc,         // 80240
    Count
};
```

Five new private tier-method declarations alongside the existing
`Tier_*` methods (currently at lines 66-72):

```cpp
bool Tier_ChaosBolt(Player* bot, Unit* target) const;
bool Tier_Shadowburn(Player* bot, Unit* target) const;
bool Tier_BaneOfHavocCleave(Player* bot, Unit* target) const;
void MaintainImprovedSoulFire(Player* bot, Unit* target);
```

### §G.2. `src/strategies/AffWarlockStrategy.cpp` — implementation pointers

**`ResolveSpellCache`** (currently lines 132-158) — extend with five new
`find(...)` calls following the existing pattern:

```cpp
_cache[size_t(Spell::ChaosBolt)]        = find("Chaos Bolt");
_cache[size_t(Spell::Shadowburn)]       = find("Shadowburn");
_cache[size_t(Spell::SoulFire)]         = find("Soul Fire");
_cache[size_t(Spell::ImprovedSoulFire)] = /* see §G.4 — name collision */;
_cache[size_t(Spell::BaneOfHavoc)]      = find("Bane of Havoc");
```

All five are `SPELLFAMILY_WARLOCK`; `StrategyUtil::FindSpellByFamilyName`
handles them. The exception is `ImprovedSoulFire`'s proc aura — see §G.4.

**`DoMaintenance`** (currently lines 237-266) — add a single call after
the CotE block at the end of the function:

```cpp
MaintainImprovedSoulFire(bot, target);
```

Implementation pattern (per G_2X §A.3 maintenance template):

```cpp
void AffWarlockStrategy::MaintainImprovedSoulFire(Player* bot, Unit* target)
{
    uint32 sf  = GetSpell(Spell::SoulFire);
    uint32 isf = GetSpell(Spell::ImprovedSoulFire);   // proc-aura ID
    if (!sf || !isf || !target)
        return;
    if (bot->GetDistance(target) > 40.0f)
        return;
    Aura* a = bot->GetAura(isf);
    bool needRefresh = !a || a->GetDuration() < 4 * IN_MILLISECONDS;
    if (needRefresh && !IsOnCooldown(bot, sf))
        TryCast(bot, target, Spell::SoulFire);
}
```

**`Update`** (currently lines 44-130) — insert the three new tier calls
into the dispatch chain between lines 127 (Tier_AoE) and 128 (Tier_DrainSoul):

```cpp
if (Tier_BaneOfHavocCleave(bot, target))   return;   // NEW
if (Tier_ChaosBolt(bot, target))           return;   // NEW
if (Tier_Shadowburn(bot, target))          return;   // NEW (execute)
```

### §G.3. Cache resolution — pattern

Every new spell resolves via `StrategyUtil::FindSpellByFamilyName(bot,
SPELLFAMILY_WARLOCK, "<name>")` — the same pattern used for the existing
19 entries. The matcher handles the two-pass castable-vs-helper
disambiguation (CLAUDE.md "Cast pipeline gotchas" #1).

### §G.4. Improved Soul Fire — passive-vs-proc-aura collision

**The gotcha.** "Improved Soul Fire" names two distinct spell IDs:
the **passive talent** (3-rank Destro tree talent, always applied) and
the **proc aura** (the 8% haste buff we need to read for maintenance).
The cast spell is "Soul Fire" (6353, different name — not affected).

If `find("Improved Soul Fire")` returns the *passive talent ID*, then
`bot->HasAura(isf)` reads true forever (passives are permanent auras)
and maintenance never refreshes Soul Fire.

This is the exact passive-name collision class CLAUDE.md gotcha #1
flags (Fingers of Frost 44544 vs 74396; Brain Freeze 44546 vs 57761).

**Fix:** pattern after `FrostMageStrategy.cpp::FindBotAuraByName` —
walk `bot->GetAppliedAuras()` matching by name, skip `info->IsPassive()`.
Lift the helper or promote to `StrategyUtil::FindBotAuraByName`.

**Pre-code verification:** dump the resolved ID via the cache-resolve
log line, compare to the WDBXEditor extract (`tools/dbc-extract.py`),
record both IDs in `docs/research/dbc-verification-checklist.md`.

### §G.5. Cast pipeline gotchas — already covered

CLAUDE.md "Cast pipeline gotchas" #2-#5 are already enforced by the
existing `AffWarlockStrategy::Update` plumbing:

| Gotcha | Coverage |
|---|---|
| #2 — Route every cast through `CastWithLog` | Existing `TryCast()` at line 181 does this; new tiers reuse it. |
| #3 — Skip rotation while casting/channelling | Existing `UNIT_STATE_CASTING` / `IsNonMeleeSpellCast(false)` guard at lines 108-109. Covers Chaos Bolt's 2.0-2.5s cast and Soul Fire's 2.5-3.0s cast trivially. |
| #4 — Skip rotation while GCD active | Existing Shadow Bolt GCD probe at lines 114-119. Same probe spell, same logic — no change needed. |
| #5 — `MaintainRange` is idempotent only when guarded | Existing `AltbotPositionManager::MakeRangedDpsIntent` at lines 87-88 — no change. |

**The new tiers literally do not need to think about #2-#5;** they just
plug into the existing dispatcher and inherit all the guards.

The one new gotcha-flavored item is §G.4 — the passive collision for
Improved Soul Fire — and the mitigation is "do what FrostMageStrategy
already does."

---

## §H. Unknowns / verification checklist

Lifted from G_2X §A.4 plus new items specific to this build's spell IDs.

| Item | Why unknown | Mitigation |
|---|---|---|
| **Server talent-grant policy** | Does the 142-pt server actually let the bot allocate 41/36/5 with tier-prereqs enforced, or is the grant looser (25/25/25 etc.)? The build above assumes standard rules. | Manual test on a level-85 Aff warlock bot post-grant; verify via `.altbot talents show`. |
| **Improved Soul Fire proc-aura ID** | The passive-name collision in §G.4. The talent ID, the cast spell ID, and the proc-aura ID are all distinct. | WDBXEditor extract via `tools/dbc-extract.py`; cross-check with the cache-resolve log on first tick. Record in `dbc-verification-checklist.md`. |
| **Chaos Bolt spell ID = 50796** | The 2x-talent design doc cites 50796 ([G_2X §A.3]). Confirmed in G_CHAOS. **Sanity-check on first cache resolve.** | `find("Chaos Bolt")` should return 50796 on a bot with the talent allocated. |
| **Shadowburn spell ID = 17877** | Same — G_2X §A.3 cites 17877. Confirmed in G_SBURN. | `find("Shadowburn")` sanity-check. |
| **Bane of Havoc spell ID = 80240** | G_2X §A.3 cites 80240. Confirmed in G_BOH. | `find("Bane of Havoc")` sanity-check. |
| **Soul Fire spell ID = 6353** | Universal — pre-Cata spell. | `find("Soul Fire")` sanity-check; should resolve unambiguously. |
| **Bane of Havoc copy mechanic in TC 4.3.4** | Wowpedia (G_BOH) lists 15% copy; some forks have implemented different multipliers. Bot only needs to *cast* BoH; the damage copy is server-side. | Manual test on a 2-mob pull — confirm secondary mob takes copied damage. |
| **Shadowburn shard refund** | The "shard refund on kill" in G_SBURN is a kill-credit-triggered effect. Confirm the bot benefits from refund (the bot's shard accounting can pick it up). | Manual test — start execute with 0 shards, observe shard count after kill. |
| **Improved Soul Fire haste stacking with Bloodlust** | ISF +8% is on top of haste rating. Stacks with Heroism/Bloodlust additively or multiplicatively? G_ISF doesn't specify. | Not blocking — maintenance still fires on its 16s rhythm regardless. Tune later if parse data emerges. |
| **Build A vs Build B parse comparison** | G_2X §A.4 already flags this. No surveyed guide compares the two on the 142-pt budget; the 5-man recommendation here is reasoning, not measurement. | Run both builds in-game, compare cast-log DPS totals over 10+ 5-man clears. |
| **Master target-swap mid-BoH** | §E.4 — degrades gracefully but not optimally. | Accept the latency; revisit when raid encounters force a target-swap meta. |
| **Demonic Pact log noise if user flips to Build B** | G_2X §A.4 + §E.5. | Out of scope for Phase 1. Filter at AltbotCombatLog when Build B lands. |

---

## §I. Cross-references

- **`docs/research/2x-talent-strategy-design.md`** — the source of truth
  for: §A.1 (dual-tree builds), §A.2 (Conflag/Immolate verdict — *do not
  reopen*), §A.3 (rotation tier order + enum extension), §A.4 (unknowns),
  §B (strategy-model trade-offs, recommended Option 2), §D.2 (Phase 1
  first-coding-step pointers).
- **`docs/research/affliction-warlock-guide-survey.md`** — the source of
  truth for: the baseline 71-pt 31/0/10 build, glyph picks (lifted into
  §C.3 verbatim), Aff DoT priority and Pandemic / Everlasting Affliction
  mechanics, pet choice (Felhunter standing pet).
- **`CLAUDE.md`** — cast-pipeline gotchas (referenced in §G.4 / §G.5);
  "Adding a new spec" rules (referenced in §C and §D); displacement
  defensives policy (referenced in §E for the existing Demonic Circle
  Teleport / Howl of Terror gating).
- **`src/strategies/AffWarlockStrategy.{h,cpp}`** — the file to edit;
  line numbers cited in §G.1, §G.2.

End of build.

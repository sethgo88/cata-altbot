# 142-Point Resto Shaman — Hybrid Healer + Aggressive Support DPS

**Scope.** Bot-executable hybrid healer with aggressive support DPS, Restoration
Shaman primary with Elemental secondary, 142-point Cata 4.3.4 server.
Survey date 2026-05-12. This doc is research and build planning only — no
code is written here. The companion code lives at
`/Users/seth.oharra/web/cata-altbot/src/strategies/RestoShamanStrategy.{h,cpp}`
and is extended in a later pass per §G.

**The user's choice (load-bearing).** When the group is safe, the bot should
*actually do DPS*, not just throw a Lightning Bolt out of guilt. That means
a real Elemental rotation — Flame Shock → Lava Burst → Chain Lightning →
Lightning Bolt — gated behind a healer-mode-vs-DPS-mode switch. Healing
*always* pre-empts DPS (CLAUDE.md "Adding a new spec" rule 4: healers gate
only their support-DPS tier). This is the design contract; everything below
flows from it.

**Cross-references.**
- `/Users/seth.oharra/web/cata-altbot/CLAUDE.md` — esp. healer rule 4 in
  "Adding a new spec" and §"Cast pipeline gotchas".
- `/Users/seth.oharra/web/cata-altbot/docs/research/resto-shaman-guide-survey.md` —
  baseline 71-point Resto.
- `/Users/seth.oharra/web/cata-altbot/docs/research/elemental-shaman-guide-survey.md` —
  load-bearing for the DPS-mode rotation specifics.
- `/Users/seth.oharra/web/cata-altbot/docs/research/2x-talent-strategy-design.md`
  §C.1.3 — the original Resto+Elemental sketch (one paragraph; this doc
  fleshes it out).
- `/Users/seth.oharra/web/cata-altbot/docs/research/affliction-warlock-guide-survey.md` —
  style template (§2 sources table format).

---

## §2. Sources

Strategy/rotation content reconciled across the strategy-bearing guides
below. Hard numbers cross-checked against the wiki/wowhead spell pages.

| # | Guide | URL | Confidence | Notes |
|---|---|---|---|---|
| R1 | Icy Veins — Resto Rotation & CDs | https://www.icy-veins.com/cataclysm-classic/restoration-shaman-pve-rotation-cooldowns-abilities | High | Strategy-bearing for heal mode. Confirms Lightning Bolt as the downtime/mana-recovery filler via Telluric Currents. |
| R2 | Icy Veins — Resto Talents & Glyphs | https://www.icy-veins.com/cataclysm-classic/restoration-shaman-pve-spec-builds-talents-glyphs | High | Strategy-bearing for talents. Confirms Concussion > Convection as the Elemental-tree dip. No point-by-point table — guide is conceptual. |
| R3 | wowtbc.gg — Resto Shaman | https://wowtbc.gg/cata/class-guides/restoration-shaman/ | Medium-High | Strategy-bearing. Confirms Focused Insight + Telluric Currents as Resto's offensive synergy talents. Lists the 4-totem layout (Stoneskin / Healing Stream / Wrath of Air / Flametongue). |
| R4 | Warcraft Tavern — Resto Shaman | https://www.warcrafttavern.com/cataclysm/guides/pve-restoration-shaman/ | Low (403) | Returned 403; reconstructed from earlier survey notes. |
| R5 | Wowhead — Resto Healer Rotation/CDs | https://www.wowhead.com/cata/guide/classes/shaman/restoration/healer-rotation-cooldowns-abilities-pve | Low | SPA — direct fetch returned only the page chrome. Referenced via prior survey reconciliation. |
| E1 | Icy Veins — Elemental Rotation & CDs | https://www.icy-veins.com/cataclysm-classic/elemental-shaman-pve-rotation-cooldowns-abilities | High | **Load-bearing for DPS-mode rotation.** Confirms exact priority: Searing Totem maintenance → Flame Shock on long-lived → Lava Burst on CD on FS target → Earth Shock at 7+ LS charges → Lightning Bolt filler. Chain Lightning at 3+ targets. |
| E2 | Icy Veins — Elemental Talents & Glyphs | https://www.icy-veins.com/cataclysm-classic/elemental-shaman-pve-spec-builds-talents-glyphs | High | Confirms talent identities (Elemental Mastery, Lava Flows, Rolling Thunder, Fulmination, Elemental Focus, Elemental Precision). Did not return tier positions; cross-checked against E3 and the Elemental survey. |
| E3 | Elemental Shaman Survey (project-internal) | `docs/research/elemental-shaman-guide-survey.md` | High | Project's own reconciled Elemental survey — DBC-verified spell IDs and rotation priorities. |
| W1 | Wowpedia — Lava Burst | https://warcraft.wiki.gg/wiki/Lava_Burst | High | Spell ID 51505. 2s cast, 8s CD. Auto-crit when Flame Shock is on target *at completion of cast* (timing caveat — see §E). |
| W2 | Wowpedia — Elemental Mastery | https://warcraft.wiki.gg/wiki/Elemental_Mastery | High | Cata: Tier 5 Elemental, 3-min CD, 15s duration, +20% spell haste + +15% Fire/Frost/Nature damage. (Wiki text — the in-game Cata 4.3.4 tooltip is the source of truth; the project survey records 18s in one place, 15s here. Flagged §H.) |
| W3 | Wowpedia — Flame Shock | https://warcraft.wiki.gg/wiki/Flame_Shock | High | Spell ID 8050. Instant, shared 6s shock CD with Earth Shock. 18s base duration; longer with Glyph of Flame Shock. |
| S1 | Project spell-ref CSV | `docs/data/spell-ref.csv` | High | DBC-verified for the IDs already present (Mana Tide 16190, Spirit Link 98008, Earthquake 61882, Nature's Swiftness 16188, Healing Stream 5394, Water Shield 52127, GHW 77472). |

**Coverage caveats.**
- Wowhead SPA pages (R5, talent-calc) cannot be content-scraped via
  WebFetch; verification of the calculator URL in §C must be performed
  manually before commit.
- R4 (Warcraft Tavern) 403'd; the totem-layout claim is reconstructed.
- W2 reports 15s Elemental Mastery duration; the prompt and the project
  Elemental survey both report 18s. Flagged in §H — verify against
  in-game tooltip on the 4.3.4 fork.

---

## §A. Realistic dual-tree builds

The standard Cata talent rule still applies: 31 in the primary tree to
unlock the spec capstone, 31 in the *primary* tree to unlock secondary
specialization bonuses. On 142 points the Resto-primary bot has:

```
41 Restoration  (full primary, capstone Tidal Waves)
+ 36 Elemental or Enhancement  (deep enough for a tier-7 capstone if desired)
+ 65 leftover  (5–6 tertiary tree rows or wasted on filler)
= 142 total
```

In practice, 41/36 leaves 65 points unallocated, far more than the third
tree can usefully absorb (Cata's third-tree first 2-3 rows are mostly
0.5%-here, 1%-there filler). The realistic build is `41/36/X` where X
captures 5-15 useful tertiary points and the rest sits unspent. **The
secondary tree is the load-bearing choice.**

### A.1. Build R+E — **Resto(41) / Elemental(36) / Enh(5)** — *the recommended build*

| Tier | Resto picks (41) | Elemental picks (36) | Enh picks (5) |
|---|---|---|---|
| T1 | Improved Healing Wave 3/3, Tidal Focus 3/3 | **Convection 3/3 OR Concussion 3/3** (see §C.1), Call of Flame 3/3 | Ancestral Resolve 2/2 |
| T2 | Ancestral Healing 3/3, Focused Insight 3/3, Totemic Focus 2/2 | **Reverberation 2/2** (shock CD −2s), Elemental Devastation passive | Toughness 3/3 |
| T3 | Nature's Guardian 3/3, Healing Way 1/1, Elemental Weapons 3/3 | **Elemental Focus 1/1** (clearcasting after crit), Elemental Fury 5/5 (crit dmg) | — |
| T4 | Cleansing Waters 2/2, Soothing Rains 1/1, Improved Water Shield 1/1 | **Improved Fire Nova 2/2**, **Elemental Reach 2/2** (FS range) | — |
| T5 | **Telluric Currents 2/2** (LB → mana), Mana Tide Totem 1/1, Nature's Blessing 3/3 | **Elemental Mastery 1/1** (Tier 5 — 3-min CD), Elemental Precision 3/3 | — |
| T6 | Ancestral Awakening 3/3, Mana Tide Totem 1/1, Nature's Swiftness 1/1, Focused Insight 3/3 | **Lava Flows 3/3** (+24% FS dmg, +30% LvB dmg), Rolling Thunder 3/3, Fulmination 1/1 | — |
| T7 | Telluric Currents 2/2, Tidal Waves 5/5 (capstone) | Earthquake 1/1, **Lava Burst 1/1** (capstone) | — |

Counts are illustrative — the survey tables don't agree to the row, and
guide content is conceptual rather than point-by-point (see §H). The
load-bearing picks for the bot are bolded:

- **Lava Burst (Elemental Tier 7 capstone, 1/1).** Unlocks the spell —
  without this talent, the bot cannot cast Lava Burst at all (W1, E2, E3).
  *The* gating talent for DPS mode.
- **Lava Flows 3/3.** +24% Flame Shock periodic damage + +30% Lava Burst
  damage. Free DPS-mode multiplier (E2, E3).
- **Elemental Mastery 1/1.** 3-min CD, +20% spell haste + 15% Fire/Frost/Nature
  damage. The headline DPS-mode cooldown (W2, E3 §3, E1).
- **Reverberation 2/2.** -2s shock CD (6s → 4s). **Critical** for keeping
  Flame Shock alive in DPS mode without crowding out heal GCDs.
- **Elemental Focus 1/1.** Clearcasting proc on crit — reduces next spell's
  mana cost by 40%. Free mana sustain in DPS mode.
- **Convection 3/3 OR Concussion 3/3.** Mana cost reduction vs. +15% Fire/Frost/Nature
  damage. Picked together with Elemental Precision (T5) for the
  Spirit→Hit conversion; see §C.1 for the trade-off.
- **Telluric Currents 2/2 (Resto T5/T7).** +40% Lightning Bolt damage
  returned as mana — turns the filler into a mana-positive cast (R1, R3).
  **Already prescribed by the project's existing Resto leveling research**
  (`resto-shaman-guide-survey.md` "Correction: Telluric Currents inclusion").

**Why this beats Build R+En:**

| Criterion | R+E (Elemental) | R+En (Enhancement) |
|---|---|---|
| Unlocks Lava Burst | **Yes** (capstone) | No |
| Unlocks Elemental Mastery (haste/dmg CD) | **Yes** (Tier 5) | No |
| Boosts Lightning Bolt | **Yes** (Concussion, Telluric Currents, Elemental Focus) | Minimal |
| Boosts Flame Shock | **Yes** (Lava Flows, Reverberation, Elemental Reach) | None |
| Reduces shock CD | **Yes** (Reverberation -2s) | No |
| Maelstrom Weapon (5-stack instant LB) | No | Yes — but **requires melee attacks**, which a Resto bot is not doing |
| Shamanistic Rage (defensive) | No | Yes — useful, but standalone |
| Pet Wolves (Feral Spirit) | No | Yes — out-of-rotation DPS button |
| **Net for a Resto-primary bot** | **Aggressive support-DPS rotation** | **Mostly dead-end** — the headline talents (Maelstrom Weapon, Stormstrike) require melee attacks the healer never makes |

### A.2. Build R+En — Resto(41) / Enhancement(36) — *the rejected alternative*

Enhancement's identity is Maelstrom Weapon (5-stack instant Lightning Bolt
from melee swings) + Stormstrike + Lava Lash + Flametongue/Windfury weapon.
**Every one of those requires the shaman to be in melee swinging.** A Resto
bot's positioning intent is *stay near party at heal-range* — see the
existing `RestoShamanStrategy::Update`'s `MakeHealerIntent` block. Putting
the bot in melee to proc Maelstrom Weapon:

1. Breaks heal-range to Chain Heal cluster.
2. Eats melee swings the bot is not geared for.
3. Triggers the CLAUDE.md displacement-defensive trap inversion — being in
   melee means *enemies* are in melee with the bot, which would invite
   spurious displacement-defensive logic to fire.

Enhancement does give Shamanistic Rage (30% damage reduction, 15s, 1-min
CD) and Feral Spirit (10% damage taken, 2 wolves 30s, 2-min CD). But those
are isolated buttons, not a rotation. Build R+En is mostly the existing
Resto rotation plus two more buttons — no real DPS uplift.

**Verdict: Build R+E (Resto+Elemental) is the recommended hybrid.** Build
R+En is documented here only for completeness.

---

## §B. The decisive question

**What's the heal-mode-vs-DPS-mode handoff threshold, and what Elemental
rotation can the bot reliably run inside the 1.5s combat tick?**

Three sub-questions answered concretely:

### B.1. When is the group "safe enough" to drop into DPS mode?

**Default threshold: no ally below 85% HP AND mana ≥ 50% (Normal mode) AND
combatElapsedMs ≥ 2000ms (the existing tank-threat window).** That is:

```
ShouldRunDpsMode(bot, master, party) ==
    AllAtFullHp(bot, master, 85.0f)        // no ally below 85%
    AND GetManaMode(bot) == ManaMode::Normal
    AND ctx.combatElapsedMs >= 2000        // tank-threat window
    AND !inDamageWindow                    // future: encounter-flagged spike
```

The current `Tier9_SupportDPS` uses **95%** (`DPS_ALL_HEALTHY_PCT`). That's
the right value for *idle* support-DPS (a single Lightning Bolt to top off
mana via Telluric Currents) but **too restrictive** for "aggressive DPS
mode." Guide consensus across R1, R3, E1 is that Resto's DPS-during-healing
mantra is "if no one needs healing **right now**, hit a damage button"; the
95% threshold means a single Flame Shock tick on the tank closes the DPS
window. **Lower to 85%** for DPS mode, **keep 95% as the strict cutoff** if
we want a layered "DPS-light vs DPS-full" distinction (deferred — §G says
land 85% first).

Refinement on guide consensus:
- R1 (Icy Veins Resto rotation): "Use Lightning Bolt to DPS during
  downtime in order to recover Mana through Telluric Currents." — endorses
  Lightning Bolt as the *minimum* DPS contribution.
- R3 (wowtbc.gg): Resto talents (Focused Insight, Telluric Currents) explicitly
  designed for offensive casts during downtime.
- E1: confirms the offensive priority list when running it (next sub-question).

Author judgment: 85% is the threshold floor. Above 85% on all allies, the
bot fires Elemental rotation. Below 85% on *any* ally, the bot returns to
the heal-tier chain.

### B.2. Can the bot run the Elemental rotation inside 1.5s ticks?

Yes, *with the existing cast-in-progress guard.* The Elemental rotation
priority (E1, E3):

```
1. Maintain Flame Shock on focus target  (instant, 6s CD shared with Earth Shock)
2. Lava Burst on CD (2s cast — 8s CD — requires Flame Shock on target)
3. Chain Lightning if ≥3 hostiles near target (2s cast — 3s CD)
4. Lightning Bolt as filler (2.5s cast)
```

The bot's 1.5s combat tick is shorter than every cast time except the
shock instants. The existing `Update()` already has the cast-in-progress
guard (lines 87-88 in current `RestoShamanStrategy.cpp`):

```cpp
if (bot->HasUnitState(UNIT_STATE_CASTING) || bot->IsNonMeleeSpellCast(false))
    return;
```

— and the GCD guard (lines 93-98). These are sufficient: during a Lava
Burst cast, the next 1-2 ticks early-return; on the tick after cast
completion, the rotation re-enters and picks the next-priority spell.
This is the same pattern Frost Mage / Aff Warlock / MM Hunter use; no new
infrastructure needed.

**One critical interaction: anchor target stickiness across master swap.**

The existing healer positioning intent anchors on the lowest-HP ally, not
on the master's target. For DPS mode, the rotation needs **the master's
target** as the focus (consistent with `Tier9_SupportDPS` at line 413:
`ObjectAccessor::GetUnit(*bot, master->GetTarget())`). If the master
target-swaps mid-fight (common in 5-man trash), the bot's Flame Shock is
now on the wrong mob.

**Resolution:** treat the DPS-mode focus as **the master's current target
at the tick boundary**. If the master swapped between ticks:
- The bot's prior Flame Shock continues to tick on the old target (free
  damage, fine).
- The bot re-applies Flame Shock to the new target on next DPS-mode tick
  (existing tier chain — Flame Shock priority 1).
- The bot loses one Lava Burst cycle while the new FS is being applied
  (acceptable; this is the same loss a human player takes on target swap).

Multi-target swap is not specifically optimized — the bot is *support*
DPS; not the focal damage dealer. Accept the loss.

### B.3. When does the bot pop Elemental Mastery?

**Trigger: DPS mode is active AND Lava Burst is off CD AND target is
high-priority (boss, named trash, or fight_remaining ≥ 30s).** EM is a
3-min CD; firing it on a 5-second trash mob is wasteful. The high-priority
filter is approximate — the bot doesn't have a robust "this is a boss"
signal today — so the practical heuristic is:

```
PopElementalMastery(bot, target) ==
    ShouldRunDpsMode()                           // already in DPS mode
    AND LavaBurst is off CD                       // line up the instant LvB
    AND target->GetMaxHealth() >= 5 * bot->GetMaxHealth()
        // proxy for "this is a meaningful target"
    AND EM is off CD
```

The MaxHealth proxy is the same shape as bosses-have-deep-HP-pools; it
captures named trash too, which is fine because named trash in Cata 5-mans
*does* live long enough to justify a 3-min CD pop. Below this threshold,
hold EM for the next pull. (E1 says "use on CD or save for priority
phases" — this is the priority-phase interpretation.)

---

## §C. Recommended build

### C.1. Tree breakdown

```
Restoration  41/71  (full)
Elemental    36/71  (deep into Tier 7 — Lava Burst)
Enhancement   5/71  (Ancestral Resolve 2 + Toughness 3 — survivability filler)
Unspent      ~60     (no productive home)
Total       142
```

**Point-by-point picks** (recommended; tier numbers are tree positions, not
character levels; ranks are filled rank/max):

#### Restoration (41/41)

| Tier | Talent | Ranks | Rationale |
|---|---|---|---|
| 1 | Improved Healing Wave | 3/3 | Cuts Healing Wave cast by 0.5s — baseline filler heal speed. |
| 1 | Tidal Focus | 3/3 | -5% mana cost on healing spells — core efficiency. |
| 2 | Ancestral Healing | 3/3 | +10% armor on heal target — tank synergy. |
| 2 | Totemic Focus | 2/2 | -50% totem mana cost — keeps the 4-totem layout cheap. |
| 2 | Focused Insight | 3/3 | After Shock: -75% mana / +30% healing on next direct heal. **Critical** — Flame Shock now buffs the next Healing Wave/GHW. This is the talent that *makes hybrid heal+DPS work*. |
| 3 | Nature's Guardian | 3/3 | -30% damage taken when HP < 35% — passive survivability. |
| 3 | Healing Way | 1/1 | +25% Healing Wave / GHW healing — direct heal scaling. |
| 4 | Cleansing Waters | 2/2 | Cleanse Spirit also heals — synergizes with existing curse-dispel preempt. |
| 4 | Improved Water Shield | 1/1 | Crit-restore mana on heal crit — Resurgence proxy. |
| 5 | Telluric Currents | 2/2 | **40% LB damage returned as mana** — turns the filler into mana-positive. (R1, R3.) |
| 5 | Nature's Blessing | 3/3 | +15% Int → SP — flat scaling. |
| 6 | Ancestral Awakening | 3/3 | Crit heals → smart-heal lowest ally for 30% — passive AoE healing. |
| 6 | Mana Tide Totem | 1/1 | The mana-regen cooldown. |
| 6 | Nature's Swiftness | 1/1 | Instant-cast Nature spell — emergency heal CD. |
| 7 | **Tidal Waves** (capstone) | 5/5 | -30% Healing Wave cast / +25% Healing Surge crit after Riptide. Already encoded in spec. |

#### Elemental (36/41)

| Tier | Talent | Ranks | Rationale |
|---|---|---|---|
| 1 | Convection | 3/3 | -6% Lightning Bolt / CL / Earth Shock / Lava Burst mana cost. Cheap. |
| 1 | Concussion | 3/3 | +6% Fire/Frost/Nature damage. Cheap and works in heal mode too (Flame Shock ticks during heal-mode pauses). |
| 1 | Call of Flame | 3/3 | +15% Fire damage and Magma/Searing Totem damage. |
| 2 | Reverberation | 2/2 | **-2s shock cooldown** — Flame Shock refresh window halved. Key for hybrid play. |
| 3 | Elemental Focus | 1/1 | Clearcasting after crit — 40% mana reduction on next 2 spells. Sustain. |
| 3 | Elemental Fury | 5/5 | +100% crit damage on Fire/Frost/Nature direct spells — Lava Burst's auto-crit becomes huge. |
| 4 | Elemental Reach | 2/2 | +20y range on shock / LB / CL — keeps the bot at heal-range while doing damage. |
| 4 | Improved Fire Nova | 2/2 | -3s Fire Nova CD — minor; alternative is Booming Echoes. |
| 5 | **Elemental Mastery** | 1/1 | **3-min CD, +20% haste, +15% Fire/Frost/Nature dmg, 15s.** The headline support-DPS cooldown. (W2, E1.) |
| 5 | Elemental Precision | 3/3 | Spirit → Hit conversion at 100%. Resto stacks Spirit; this turns it into spell hit for the DPS-mode casts. |
| 6 | Lava Flows | 3/3 | **+24% Flame Shock periodic dmg, +30% Lava Burst dmg.** Free DPS-mode multiplier. (E2, E3.) |
| 6 | Rolling Thunder | 3/3 | Lightning Bolt generates Lightning Shield charges. (Needed for Fulmination if we add Earth Shock to the rotation — currently we don't, but it's cheap.) |
| 7 | Lava Burst (capstone) | 1/1 | **The capstone — unlocks Lava Burst as a castable spell.** (W1.) |

#### Enhancement (5/41)

| Tier | Talent | Ranks | Rationale |
|---|---|---|---|
| 1 | Ancestral Resolve | 2/2 | -10% damage taken while casting. Passive survivability for a stationary caster. |
| 1 | Toughness | 3/3 | +10% stamina — passive HP floor. |

Five points isn't a tier-2 unlock; this is purely filler survivability. The
"third tree leftovers" pattern from `2x-talent-strategy-design.md` §C.1.3.

### C.2. Glyphs

**Prime (3)** — should mix healing with at least one Elemental glyph per
the prompt:

| Glyph | Effect | Rationale |
|---|---|---|
| Glyph of Earth Shield | +20% Earth Shield healing | Tank-heal staple — already in baseline Resto glyphs. |
| Glyph of Riptide | +6s Riptide duration | Maximizes Tidal Waves uptime + Riptide-on-tank HoT value. |
| **Glyph of Lava Burst** | +10% Lava Burst SP scaling | **The Elemental glyph.** DPS mode lives or dies on Lava Burst damage. |

Trade-off note: dropping Glyph of Earthliving Weapon (consensus baseline)
for Glyph of Lava Burst is the *aggressive-DPS* commitment. If a future
tuning pass finds the bot is heal-throughput-bound (Earthliving HoT
contribution matters), swap Lava Burst → Earthliving and accept the
DPS-mode loss.

**Major (3):**

| Glyph | Effect | Rationale |
|---|---|---|
| Glyph of Chain Heal | +1 Chain Heal jump | Direct healing throughput (R3 consensus). |
| Glyph of Healing Stream Totem | +20% Healing Stream healing | Passive raid healing — survives totem swaps. |
| Glyph of Stoneclaw Totem | Stoneclaw absorbs damage equal to 4× casters health | Emergency totem-shield — fine-tunable; alternates with Glyph of Healing Wave (+20% healing on the caster). |

**Minor (3):**

| Glyph | Effect | Rationale |
|---|---|---|
| Glyph of Renewed Life | No reagent for Reincarnation | Self-rez quality of life. |
| Glyph of Water Walking | No reagent for Water Walking | Quality of life. |
| Glyph of Astral Recall | -50% Astral Recall CD | Quality of life. |

(Minor glyphs are quality-of-life; identical to baseline Resto.)

### C.3. Talent allocation — source of truth

**No third-party calculator URL.** Wowhead's Cata Classic talent calc
enforces stock-Cata rules (41-point primary cap, 71 total) and cannot
represent a 142-point dual-tree build. The §C.1 / §C.2 tables above are
the build of record.

Allocate in-game via `.altbot talents learn <talentId> <rank>` for each
row. Tree-by-tree summary for cross-checking after placement:

- **Restoration (41):** every talent listed in §C.1 (Resto block) at the
  noted ranks; capstone Tidal Waves 5/5.
- **Elemental (36):** every talent listed in §C.1 (Ele block) at the
  noted ranks; capstone Lava Burst.
- **Enhancement (5):** Ancestral Resolve 2/2 + Toughness 3/3.

The current `.altbot talents show <bot>` command only reports active spec
+ remaining points; a richer worldserver-logged dump of the full talent
placement is a queued follow-up so you can grep the build out of
`worldserver.log` rather than transcribing from in-game chat.

---

## §D. Rotation tier order

The most divergent section. The strategy keeps the existing heal-tier
chain and **inserts a DPS-mode tier chain** in the slot currently occupied
by `Tier9_SupportDPS`.

### D.1. Existing heal-mode tier chain (unchanged)

Quoted from `src/strategies/RestoShamanStrategy.cpp::Update` (lines 114-123):

```cpp
if (Tier1_SelfEmergency(bot))                     return;
if (Tier2_TankEmergency(bot, master, mode))       return;
if (Tier3_Riptide(bot, master))                   return;
if (Tier4_SpiritLink(bot, master))                return;
if (Tier5_ChainHeal(bot, master, mode))           return;
if (Tier6_GHW(bot, master, mode))                 return;
if (Tier7_HealingSurgeNonTank(bot, master, mode)) return;
if (Tier8_HealingWaveFiller(bot, master))         return;
Tier9_SupportDPS(bot, master, mode, ctx);
```

The first 8 tiers are healing. Tier 9 is the support-DPS tier (single
Lightning Bolt today). **All 8 healing tiers continue to fire when any
ally is below threshold — DPS mode never pre-empts a heal.** This is the
healer rule from CLAUDE.md "Adding a new spec" rule 4 and the contract
this design preserves.

### D.2. New DPS-mode tier chain (replaces Tier9)

```cpp
// Replace Tier9_SupportDPS with a mode-gated DPS rotation.
if (ShouldRunDpsMode(bot, master, ctx))
{
    if (Tier9a_ElementalMastery(bot, master))       return;   // CD pop
    if (Tier9b_FlameShock(bot, target))             return;   // apply/refresh
    if (Tier9c_LavaBurst(bot, target))              return;   // CD on FS target
    if (Tier9d_ChainLightning(bot, target))         return;   // 3+ near target
    Tier9e_LightningBolt(bot, target);                        // filler
}
```

Tier semantics, in priority order:

| Tier | Trigger | Spell | Notes |
|---|---|---|---|
| 9a | EM off CD AND DPS mode AND target priority (§B.3 proxy) | Elemental Mastery | Self-buff instant — does NOT consume the cast tick. Falls through to 9b same tick. |
| 9b | Flame Shock missing OR remaining ≤ 3s | Flame Shock | Instant — Reverberation cuts shock CD to 4s. Apply on master's target. |
| 9c | Lava Burst off CD AND target has Flame Shock from this bot | Lava Burst | 2s cast (~1.5s under EM haste). **Must check `target->HasAura(FlameShock, bot->GetGUID())`** (§E.4). |
| 9d | `CountHostilesNearUnit(bot, target, 10y) ≥ 3` AND ≥ 3 hostiles in CL hit cone | Chain Lightning | 2s cast. Anchor on target, not on bot (CLAUDE.md "Adding a new spec" rule 7). |
| 9e | else | Lightning Bolt | 2.5s cast. Telluric Currents returns 40% damage as mana. |

**Critical: 9a fires its self-buff and falls through.** Elemental Mastery
is instant-cast and **does not** start a normal GCD on the next damage
spell (per W2, the spell's "next cast is instant" effect). The tier
chain *continues* on the same tick so 9b/9c immediately benefits from the
+20% haste and possibly the instant-cast effect. This is the same shape
as Nature's Swiftness in the existing heal chain (Tier 6 NS pop, then
fall through to GHW on the same tick).

> Implementation note: 9a's "fall through" requires `return` to NOT happen
> after the EM cast — i.e., 9a is structured as `MaybePopElementalMastery(...)`
> with no return guard, not as `Tier9a_ElementalMastery(...) return;`.
> Otherwise the same-tick LvB benefit is lost to the cast-in-progress
> guard on the next tick.

### D.3. ShouldRunDpsMode gate

```cpp
bool ShouldRunDpsMode(Player* bot, Player* master, AltbotTickContext const& ctx) const;
```

Implementation outline (no code — design only):

```
return  ctx.combatElapsedMs >= THREAT_WINDOW_MS              // 2000ms
    AND GetManaMode(bot) == ManaMode::Normal                   // mana ≥ 50%
    AND StrategyUtil::AllAtFullHp(bot, master, DPS_MODE_FLOOR_PCT)  // 85.0f
    AND MasterHasValidAttackTarget(bot, master)
    AND DistanceToTarget <= 30.0f                              // Elemental Reach 2/2 gives 30y on LB/CL/FS
    ;
```

`DPS_MODE_FLOOR_PCT = 85.0f` is the new constant (replaces or
parameterizes `DPS_ALL_HEALTHY_PCT`'s 95% for the *aggressive*
DPS-mode gate). Keep 95% as an alternate `IDLE_DPS_FLOOR_PCT` if the user
later wants a layered behavior — out of scope for this pass.

### D.4. Mode-flip behavior

- **Healing → DPS:** Heals fire, no ally is below 85%, next tick the
  rotation falls through to Tier 9 and enters DPS mode. **No state.**
  The gate is recomputed each tick.
- **DPS → Healing:** Any ally drops below 85%, next tick Tier 1-8 fires.
  In-flight Lava Burst (mid-2s-cast) continues to completion — the bot
  doesn't cancel it. CLAUDE.md cast-pipeline guard ensures no other spell
  is attempted during the cast. *On the tick after Lava Burst lands*, the
  heal tiers preempt. Cost: at worst one Lava Burst's worth of GCD lag
  before the first heal lands. This is acceptable per the prompt's
  aggressive-DPS stance.
- **Edge case: ally drops below 85% mid Lava Burst cast.** Bot finishes
  cast (no cancel), then next tick heals. The 2s cast worst case is
  identical to a human player mid-cast. CLAUDE.md does not endorse cast
  cancellation for re-prioritization (no current strategy does this).

### D.5. Totem rotation

Resto's 4 active totem slots, per R3 consensus reconstructed from §2:

| Slot | Default totem | Alternates | When to swap |
|---|---|---|---|
| Fire | Flametongue Totem | Searing Totem (DPS mode, single target), Magma Totem (DPS mode, AoE) | Default Flametongue (+6% spell damage party aura). In *aggressive DPS mode on a long-lived target*, swap to Searing for the extra single-target damage. Magma if a Cata trash AoE pack lives ≥ 15s. |
| Water | Mana Spring Totem | Healing Stream Totem, Cleansing Totem | Default Mana Spring (party mana regen). Healing Stream if the group is taking sustained low damage and you want HoT-style passive healing. Cleansing on poison/disease encounters. |
| Earth | Stoneskin Totem | Strength of Earth (melee party), Tremor Totem (fear encounters) | Default Stoneskin (-10% physical damage taken). SoE if 2+ melee in party. Tremor on Howling Banshee / Drudge Ghoul mechanics. |
| Air | Wrath of Air Totem | Wind Walk, Windfury (melee) | Default Wrath of Air (+5% spell haste). Never swap for a caster group. |

**Bot-side totem drop policy (forward-looking):** the current strategy
doesn't actively manage totem swaps; totems are out-of-scope for this
pass per the existing `CheckCooldowns` comment ("totem disambiguation
deferred"). Document the *intent* here so the future totem manager has a
spec:

1. **Pre-pull:** drop all 4 default totems (Flametongue, Mana Spring,
   Stoneskin, Wrath of Air).
2. **On totem expire:** redrop the same slot — totem duration is 5 minutes
   so this is rare.
3. **DPS mode active AND single-target AND fight ≥ 30s:** swap Fire to
   Searing. Otherwise keep Flametongue.
4. **AoE pack (4+ enemies clustered):** swap Fire to Magma for the pack
   duration.

Totem swaps cost 1 GCD per swap. The bot should never swap totems
*during* a heal-tier-9 DPS-mode rotation tick — totems live in
`DoMaintenance` (out-of-combat or low-priority in-combat), not in the
tier chain.

---

## §E. Bot-executable caveats

Honest accounting of what the bot can and cannot do.

### E.1. Lava Surge proc handling

**Lava Surge** is an Elemental talent (Tier 4) that gives Flame Shock
periodic ticks a chance to **reset Lava Burst's cooldown AND make the next
Lava Burst instant cast**. **This build does NOT take Lava Surge** (36/36
Elemental fills out the high-value picks first; Lava Surge sits at Tier 4
and competes for points with Improved Fire Nova / Elemental Reach).

Decision rationale:
- Lava Surge is a proc — the bot has to *react* on the next tick (CLAUDE.md
  cast-pipeline guard catches this naturally; the proc aura is what we'd
  check before casting LvB).
- Without Lava Surge, Lava Burst is strictly CD-bound at 8s (4s under EM
  haste). The bot's DPS-mode cadence is predictable: FS apply → LB filler
  × 2 → LvB → repeat.
- If a future tuning pass swaps to Lava Surge, the bot **must** check
  `bot->HasAura(LavaSurgeProcAura)` and prefer LvB at the start of every
  tick. **The proc aura ID is distinct from the talent ID — CLAUDE.md
  cast-pipeline-gotchas #1.** Verify via WDBXEditor before wiring.

For v1, omit Lava Surge from the talent build and from the rotation. Keep
it as a §H follow-up.

### E.2. Riptide HoT clipping

Riptide's HoT is 6s base (12s with Glyph of Riptide). Tier 3 refreshes
Riptide on tank when it's missing — but the existing tier doesn't check
the *remaining* HoT duration; it checks `!tank->HasAura(rt)`. **This is
fine** — refresh only happens when the HoT has expired, so clipping is
zero. The trade-off is that Tidal Waves uptime has a 0-6s gap each cycle
where Riptide is on CD (6s) and the HoT has fallen off but the cooldown
hasn't reset. This is in the source already, not new.

### E.3. Chain Heal jump path

Resto's Chain Heal jumps to the lowest-HP allies within 12y of the previous
target. **The bot does not choose jump targets** — the spell engine picks
them. The current Tier 5 fires Chain Heal on the lowest-HP ally as the
initial target and lets jumps happen. **DPS mode does NOT change this** —
Chain Heal is heal-mode-only. In DPS mode, **Chain Lightning** (different
spell) is the AoE cast.

### E.4. Lava Burst requires Flame Shock from THIS bot

**Critical gotcha.** Per W1, Lava Burst auto-crits "if the target is
affected by Flame Shock." But on a fork where multiple casters might be
attacking the same target, *whose* Flame Shock matters?

- The Wowpedia text says "affected by Flame Shock" without caster
  qualifier — but the project's existing analogous case (Aff warlock
  Conflagrate, see `2x-talent-strategy-design.md` §A.2 + G_TC19790) shows
  3.3.5/4.3.4 TC forks **may** require the caster owning the DoT.
- **Safe default for bot code:** check
  `target->HasAura(FlameShock, bot->GetGUID())` — i.e., the bot's own
  Flame Shock — before casting Lava Burst. This avoids the case where
  another shaman's Flame Shock is up but the bot's isn't, which on a
  caster-aware core would not grant the crit.
- If the master is also a shaman casting Flame Shock, the bot's Flame
  Shock might be removed when the master applies theirs (one Flame Shock
  per target per caster). The bot detects this via the
  `HasAura(spellId, casterGUID)` check on next tick and re-applies.

Verify the actual 4.3.4 fork behavior with a test: cast Flame Shock from
the master, cast Lava Burst from the bot, check for auto-crit. Log the
result. (Out of scope for this design pass; flag in §H.)

### E.5. Earth Shield maintenance

Earth Shield has 9 charges; consumed by incoming melee/spell damage. The
existing `DoMaintenance` only checks `!tank->HasAura(es)` — it refreshes
when Earth Shield is *missing*, not when charges are low. **Per the
resto-shaman survey §3, the recommended threshold is "missing OR charges
≤ 3"** — but the current code does the simpler missing-only check, which
is acceptable for v1 (charges only drop to 0 after Earth Shield expires,
not from damage in a way the bot can easily detect without DBC charge
tracking).

For the DPS-mode pass: no change. Earth Shield maintenance stays in
`DoMaintenance` and runs every tick before the rotation. The bot will
re-apply within 1 tick of Earth Shield disappearing.

### E.6. Spiritwalker's Grace

`Spiritwalker's Grace` (79206) is **not in the current Spell enum**. The
existing strategy code does not cast it. It's documented as a healer
movement CD in the resto spec doc (`docs/specs/resto-shaman.md`) but
deferred.

For this build pass: out of scope. SWG is a future addition that makes the
bot able to cast while moving — useful when group is spread (movement
encounters), but not load-bearing for the hybrid heal+DPS design. Flag in
§H.

### E.7. Mana sustain in DPS mode

The bot's `GetManaMode` returns `Crisis` at < 25%, `Conserve` at < 50%.
DPS mode requires `Normal`. **What stops the bot from draining itself in
DPS mode?**

- Lightning Bolt + Telluric Currents 2/2 = +40% damage returned as mana.
  Lightning Bolt is mana-positive when used on a damaged enemy.
- Elemental Focus (clearcasting after crit) = next 2 spells -40% mana.
  Procs roughly every 5-10 casts depending on crit rate.
- Water Shield baseline regen.
- Mana Spring Totem (party-wide).

The combined sustain is enough that DPS mode is mana-neutral to
mana-positive on a long fight. The `mode == ManaMode::Normal` gate is the
safety net — if mana drops below 50%, the bot returns to pure heal mode
(no DPS) and Healing Wave-fills back to full.

---

## §F. Side-by-side: stock Resto vs recommended Resto+Ele

| Item | Stock 31/0/10 Resto | Recommended 41/36/5 Resto+Ele |
|---|---|---|
| Resto tree depth | 31 (Tidal Waves) | **41** (full tree) |
| Elemental tree depth | 2-3 (Concussion or Convection) | **36** (through Lava Burst capstone) |
| Lava Burst available | No | **Yes** |
| Elemental Mastery available | No | **Yes (3-min CD)** |
| Lava Flows (+24% FS / +30% LvB) | No | **Yes** |
| Reverberation (shock CD -2s) | No | **Yes** |
| Elemental Focus (clearcasting) | No | **Yes** |
| Elemental Precision (Spirit→Hit) | No | **Yes (3/3)** |
| Telluric Currents | Yes (consensus 2/2) | Yes (2/2) |
| Focused Insight (post-shock heal buff) | Yes if speccd | Yes (3/3) — **synergizes with hybrid play** |
| Glyph of Lava Burst | No | **Yes** (replaces Glyph of Earthliving Weapon) |
| DPS-mode tier | Tier 9 = Lightning Bolt only | Tier 9 = full Elemental rotation (FS → LvB → CL → LB) |
| DPS-mode trigger | `AllAtFullHp(95%)` | `AllAtFullHp(85%)` + mana Normal + combat ≥ 2s |
| Healing rotation | Tiers 1-8 unchanged | Tiers 1-8 unchanged |
| Code: enum additions | None | FlameShock, LavaBurst, ChainLightning, ElementalMastery (LightningBolt already present) |
| Code: tier methods | Tier9_SupportDPS | Tier9a-e (EM, FS, LvB, CL, LB) + ShouldRunDpsMode gate |

---

## §G. Implementation pointers

Concrete extension points for `src/strategies/RestoShamanStrategy.{h,cpp}`.
**No code is written here**; this is the planning hand-off for a follow-up
implementation pass.

### G.1. `Spell` enum additions

Current enum (RestoShamanStrategy.h:21-35):

```cpp
enum class Spell : uint8_t
{
    HealingSurge = 0,
    HealingWave,
    GreaterHealingWave,
    Riptide,
    ChainHeal,
    EarthShield,
    WaterShield,
    SpiritLinkTotem,
    ManaTideTotem,
    LightningBolt,           // already present
    CleanseSpirit,
    Count
};
```

Add:

```cpp
    // --- DPS mode (Elemental secondary) ---
    FlameShock,              // 8050
    LavaBurst,               // 51505 (talent-gated; resolves 0 if Lava Burst capstone not learned)
    ChainLightning,          // 421
    ElementalMastery,        // 16166
    // (Optional future:) LavaSurgeProcAura — if Lava Surge talent is added.
```

### G.2. New tier methods + `ShouldRunDpsMode`

```cpp
private:
    bool ShouldRunDpsMode(Player* bot, Player* master,
                          AltbotTickContext const& ctx) const;

    // DPS-mode tier chain — replaces Tier9_SupportDPS when DPS mode is active.
    void MaybePopElementalMastery(Player* bot, Unit* target);
    bool Tier9b_FlameShock(Player* bot, Unit* target) const;
    bool Tier9c_LavaBurst(Player* bot, Unit* target) const;
    bool Tier9d_ChainLightning(Player* bot, Unit* target) const;
    bool Tier9e_LightningBolt(Player* bot, Unit* target) const;
```

`MaybePopElementalMastery` is intentionally `void` so the tier chain
*does not return* after the EM cast (see §D.2 same-tick fall-through note).

### G.3. `ResolveSpellCache` extensions

The new spells resolve via name-match — effect introspection isn't
distinctive enough (Chain Lightning has chained school damage, but so does
Lightning Bolt with talents in some configurations). Use:

```cpp
_cache[size_t(Spell::FlameShock)]
    = StrategyUtil::FindSpellByFamilyName(bot, SPELLFAMILY_SHAMAN, "Flame Shock");
_cache[size_t(Spell::LavaBurst)]
    = StrategyUtil::FindSpellByFamilyName(bot, SPELLFAMILY_SHAMAN, "Lava Burst");
_cache[size_t(Spell::ChainLightning)]
    = StrategyUtil::FindSpellByFamilyName(bot, SPELLFAMILY_SHAMAN, "Chain Lightning");
_cache[size_t(Spell::ElementalMastery)]
    = StrategyUtil::FindSpellByFamilyName(bot, SPELLFAMILY_SHAMAN, "Elemental Mastery");
```

`FindSpellByFamilyName` already filters `IsPassive()` candidates (CLAUDE.md
gotcha #1) and prefers castable variants — appropriate for all four
spells.

### G.4. Update the first-tick debug log

Add the new cache slots to the `TC_LOG_DEBUG("altbot", ...)` call in
`Update` so combat-log debug shows whether the talents are placed:

```
... FlameShock=%u LavaBurst=%u ChainLightning=%u ElementalMastery=%u
```

If `LavaBurst=0` on a bot intended to run hybrid mode, the Lava Burst
capstone isn't learned — diagnostic for talent allocation issues.

### G.5. Tier dispatch insertion

Replace `Tier9_SupportDPS(bot, master, mode, ctx);` (line 122) with:

```cpp
if (ShouldRunDpsMode(bot, master, ctx))
{
    Unit* target = ObjectAccessor::GetUnit(*bot, master->GetTarget());
    if (target && target->IsAlive() && bot->IsValidAttackTarget(target))
    {
        // Maintain 25-30y caster range. Elemental Reach 2/2 gives
        // FS/LB/CL 30y range; healer position intent already keeps the
        // bot near the lowest-HP ally, which is usually near master at
        // range. Don't call MaintainRange (CLAUDE.md gotcha #5: it
        // re-issues MoveChase) — let the position manager handle it.

        if (!bot->IsInCombat())
            bot->Attack(target, true);

        MaybePopElementalMastery(bot, target);    // self-buff, falls through

        if (Tier9b_FlameShock(bot, target))    return;
        if (Tier9c_LavaBurst(bot, target))     return;
        if (Tier9d_ChainLightning(bot, target)) return;
        Tier9e_LightningBolt(bot, target);
    }
}
```

### G.6. Cast-pipeline gotchas — applied

All five CLAUDE.md cast-pipeline gotchas apply:

1. **Passive talent name collisions.** Lava Surge proc aura (if we add it
   later) shares the name "Lava Surge" with the passive talent. Filter via
   `IsPassive()` (FindSpellByFamilyName already does this). Verify on
   first-tick log; if the cached ID matches the talent ID instead of the
   proc aura, fall back to a `FindBotAuraByName`-style helper with
   IsPassive() filter — same as FrostMageStrategy / MmHunterStrategy.
2. **Every CastSpell routes through `StrategyUtil::CastWithLog`.** Already
   the case in `RestoShamanStrategy::TryCast`. New tiers use TryCast.
   `SPEC_LABEL = "RestoShaman"` (already set at line 38).
3. **Skip rotation while a cast is in progress.** The existing guard at
   lines 87-88 covers Flame Shock (instant — fine), Lava Burst (2s),
   Chain Lightning (2s), Lightning Bolt (2.5s). No change needed.
4. **Skip rotation while GCD is active.** Existing Healing Wave probe at
   lines 93-98 covers the standard GCD bucket. Flame Shock / LB / CL all
   share the standard GCD category — covered. No change needed.
5. **`MaintainRange` is idempotent only when guarded.** Per §G.5, the
   strategy delegates positioning to the position manager (healer
   intent). Do NOT call `AltbotPosition::MaintainRange` directly from the
   DPS tier chain — that would re-issue MoveChase and produce
   SPELL_FAILED_MOVING (53) mid-cast.

### G.7. Cross-reference: 2x-talent-strategy-design.md §C.1.3

The original sketch for Resto+Elemental in
`/Users/seth.oharra/web/cata-altbot/docs/research/2x-talent-strategy-design.md`
§C.1.3 reads (paraphrased):

> Stock Resto reaches Restoration Tier 7 (Tidal Waves capstone) and dips
> into Elemental for Concussion. At 142 points, the secondary unlock
> brings Elemental Mastery (3-min CD, +20% spell haste — support-DPS
> cooldown), Call of Flame, Lava Flows. Adding Elemental Mastery as an
> on-CD support cooldown is a single new method on the strategy. Healer
> gating in CLAUDE.md §"Adding a new spec" rule 4 still applies.

This doc is the expanded form. The §C.1.3 sketch landed on Option 2
(in-class detection per Part B of the same doc) — same architectural
choice this doc adopts implicitly: extend `RestoShamanStrategy` in place,
no subclass.

### G.8. Phase-in plan

Suggested PR breakdown (no implementation in this doc, just sequencing):

1. **PR 1 — Spell cache + first-tick log.** Add the 4 new enum entries,
   resolve them via FindSpellByFamilyName, extend the cache-debug log
   line. Verify on a 41/36/5 bot that all 4 IDs resolve non-zero.
2. **PR 2 — `ShouldRunDpsMode` gate + FlameShock + LightningBolt only.**
   Lowest-risk addition: replace existing Tier9 with the gate, FS apply,
   LB filler. Run in 5-man trash; confirm bot applies Flame Shock and
   spams LB while group ≥ 85% HP, falls back to heal tiers below 85%.
3. **PR 3 — Add Lava Burst.** Verify HasAura check works on the 4.3.4
   fork (§E.4 — does it accept any-caster FS or require bot-caster FS?).
4. **PR 4 — Add Chain Lightning + Elemental Mastery.** Confirm CL fires
   on 3+-mob trash packs and EM pops on boss pulls only.
5. **PR 5 — Tuning.** Adjust DPS_MODE_FLOOR_PCT, EM target-priority
   heuristic, FS refresh window. Driven by combat-log review.

Each PR is bounded under ~150 lines per the 2x-talent-strategy doc's
size guidance.

---

## §H. Unknowns / verification checklist

What this doc did not verify and what should be checked before
implementation:

| Item | Why uncertain | How to verify |
|---|---|---|
| Wowhead talent calculator URL in §C.3 | Wowhead is a JS SPA; WebFetch returned no rendered content. URL pattern is the standard `<resto>-<ele>-<enh>` digit-per-talent scheme, but no live confirmation. | Load the URL in a browser; confirm the displayed talent allocation matches §C.1's tables. |
| Elemental Mastery exact duration (15s W2 vs 18s project survey) | W2 (Wowpedia) says 15s; `elemental-shaman-guide-survey.md` says 18s. Both refer to Cata 4.3.4. | Check in-game tooltip on the 4.3.4 fork, or WDBXEditor `Spell.dbc` row 16166 `Duration_ms`. |
| Lava Burst auto-crit caster requirement (§E.4) | Wowpedia says "if target affected by Flame Shock" without caster qualifier. TC fork behavior unverified. | Empirical test: master casts FS, bot casts LvB, check if LvB crits. If not, bot must own its FS. |
| Lava Surge proc aura ID (deferred) | Talent and proc-aura share a name — CLAUDE.md gotcha #1 territory. | If/when Lava Surge is added to the build, run WDBXEditor `Spell.dbc` query on `Name="Lava Surge"`; identify the cast/passive/proc rows. |
| Chain Lightning hit count formula | Chain Lightning hits up to N targets (3 base + Glyph). The bot's "≥3 hostiles near target" gate is approximate; the engine picks actual jump targets. | Confirm in-game that CL on a 5-pack hits 3 mobs (or 5 with talent/glyph). |
| Tank totem (Strength of Earth vs Stoneskin) | Spec disposition picks Stoneskin (physical damage reduction). Some encounters benefit more from SoE (melee strength buff). | Encounter-by-encounter tuning during 5-man playtest. |
| Glyph of Lava Burst vs Glyph of Earthliving Weapon | This doc swaps Earthliving → Lava Burst for the aggressive-DPS build. | Combat-log review: measure heal-throughput impact (Earthliving contribution as % of total heals) on a 5-min boss fight to confirm the swap is acceptable. |
| Reverberation rank (1/2 vs 2/2) | Doc takes 2/2 (-2s shock CD). 1/2 (-1s) frees a point for Storm Earth and Fire or other tier-2 picks. | Tuning decision; 2/2 is the safer DPS-mode commitment. |
| Totem swap automation (§D.5) | Documented as intent only; no code today. | Separate work item — see existing `CheckCooldowns` comment "totem disambiguation deferred." |
| Spiritwalker's Grace addition (§E.6) | Movement healing CD not in current enum. | Add when movement-aware healer behavior lands (separate doc). |
| Master target-swap behavior (§B.2) | The bot uses `master->GetTarget()` at tick boundary. Multi-target-swap-per-second wastes Flame Shock applications. | Combat-log review; if FS application rate is excessive, add a 2s debounce on target acquisition. |
| `Tier9` rename | Current code has `Tier9_SupportDPS` as a single method. After this design, Tier 9 becomes a sub-chain. Naming should reflect that. | Choose `Tier9_DpsMode` or similar; not load-bearing. |
| Earth Shield charge tracking | Existing maintenance checks "missing" only, not "charges ≤ 3". | Acceptable for v1 per §E.5; revisit if tank dies after a long string of swings without ES refresh. |

End of doc.

# Restoration Druid 4.3.4 — Multi-Guide Strategy Survey

Audit trail for the strategy decisions that will be encoded in `docs/specs/resto-druid.md`. The spec doc is the source of truth for the bot; this doc records *which guides agreed*, *where they disagreed*, and *which thresholds were the bot author's judgment call rather than guide consensus*.

Survey conducted 2026-05-02 by the bot author. Resto Druid is the **fourth and final** healer spec in the project (after resto-shaman, holy-paladin, holy-priest, disc-priest). The defining property that separates Resto Druid from the previous three is its **HoT-rolling philosophy**: instead of reactively casting direct heals when allies drop below an HP threshold, the druid maintains a slate of HoTs (Lifebloom 3-stack on tank, Rejuvenation on raid members taking sustained damage, Wild Growth on cluster damage) and uses direct heals primarily to (a) refresh the **Mastery: Harmony** buff and (b) handle emergencies. The spec doc rotation tier shape needs to reflect this — the top tiers are HoT maintenance, not HP-gated direct heals.

Numerical data (spell IDs, mana %, cast times, etc.) was sourced from Wowhead's Cata archive and Warcraft Wiki, summarized here only where it directly informs strategy. Per project source registry, hard numbers come from a single canonical reference (Wowhead Cata Classic / Warcraft Wiki); strategy/rotation/threshold content is what's reconciled across the guides below.

---

## Sources

| # | Guide | URL | Author / community | Date / patch | Confidence (rationale) |
|---|---|---|---|---|---|
| G1 | Icy Veins — Cata Classic Restoration Druid Rotation, Cooldowns & Abilities | https://www.icy-veins.com/cataclysm-classic/restoration-druid-pve-rotation-cooldowns-abilities | Icy Veins editorial | Cata Classic re-release (4.3.4 mechanics) | High — primary editorial source, returned readable rotation/cooldown content with a clear "non-Tree-of-Life rotation" vs "Tree-of-Life rotation" split. |
| G2 | Icy Veins — Cata Classic Restoration Druid Stat Priority | https://www.icy-veins.com/cataclysm-classic/restoration-druid-pve-stat-priority | Icy Veins editorial | Cata Classic | High — concrete numbers (128.05 haste rating per 1%, 12.51% Rejuv breakpoint, 7.12% Wild Growth/Efflorescence breakpoint, four breakpoint tiers tabulated). |
| G3 | Icy Veins — Cata Classic Restoration Druid Talent Builds & Glyphs | https://www.icy-veins.com/cataclysm-classic/restoration-druid-pve-spec-builds-talents-glyphs | Icy Veins editorial | Cata Classic | Medium — fetched but talent enumeration is summarized, not point-by-point. Glyph picks listed clearly. Notable: this Icy Veins page recommends Glyph of **Swiftmend** in the Prime slot, which conflicts with G1's "use Swiftmend to consume HoT and seed Efflorescence" framing. |
| G4 | wowtbc.gg — Cata Classic Restoration Druid | https://wowtbc.gg/cata/class-guides/restoration-druid/ | wowtbc.gg community | Cata Classic | Medium — readable rotation content, prime/major glyph list, identifies Empowered Touch as the Lifebloom-refresh enabler. Conflicts with Icy Veins on the 3rd Prime glyph slot (wowtbc.gg picks Swiftmend; in the rotation framing this matches G3). |
| G5 | Warcraft Wiki / Wowpedia — Restoration Druid + per-spell pages (Lifebloom, Tree of Life, Swiftmend, Wild Growth, Efflorescence, Rejuvenation, Nourish, Healing Touch, Regrowth, Tranquility, Innervate, Nature's Swiftness, Mastery: Harmony, Barkskin, Empowered Touch, Omen of Clarity, Malfurion's Gift) | https://warcraft.wiki.gg/wiki/Lifebloom (and ~15 spell-specific pages) | wiki community | covers original Cata 4.0–4.3 patch notes | High for hard mechanics (spell IDs, mana %, level requirements, patch-note refunds and target-counts); not strategic. Notable: the Tree of Life **spell** page (vs the form-shapeshift page) is sparse on the wiki and required cross-referencing Wowhead and treehealsgowoosh. |
| G6 | Tree Heals Go Woosh — "When to use the new Tree of Life" + "Cataclysm Final Grades: The State of the Resto Druid" | https://treehealsgowoosh.wordpress.com/2011/01/05/when-to-use-the-new-tree-of-life/ ; https://treehealsgowoosh.wordpress.com/2012/02/22/cataclysm-final-grades-the-state-of-the-resto-druid/ | Lissanna (community theorycrafter, "Restokin") | original 4.0 (Jan 2011) and 4.3 retrospective (Feb 2012) | High — era-correct, written by a long-running Resto Druid theorycrafter. The Jan 2011 article enumerates Tree of Life enhanced-spells exhaustively; the Feb 2012 retrospective frames the spec's HoT-rolling-plus-direct-heal Harmony hybrid clearly. |
| G7 | Cannot Be Tamed — "Cataclysm Restoration Druid Guide" | https://cannotbetamedblog.wordpress.com/resources/cataclysm-restoration-druid-guide/ | Cannot Be Tamed (community blogger, healer mains) | original 4.3 era | High — concrete numbers (917 haste for 5th Rejuv tick, 2005 haste for extra Wild Growth/Efflorescence/Lifebloom ticks), explicit "refresh Lifebloom without blooming wastes 3 GCDs" guidance, "Tree of Life at least twice per fight" cadence, "Tranquility 2-3 times per fight" cadence. Most specific community source on cooldown frequency. |
| G8 | FERALTREE — "Cataclysm Restoration Druid Guide" + "Harmony, Lifebloom, and You! (with logs!)" | http://feraltree.blogspot.com/p/cataclysm-restoration-druid-guide.html ; http://feraltree.blogspot.com/2011/10/harmony-lifebloom-and-you-with-logs.html | feraltree (community theorycrafter) | original 4.2 era (Oct 2011) | Medium-High — the Harmony-with-logs post is the single most-detailed Harmony refresh source: defines Harmony trigger spells (Nourish, Healing Touch, Regrowth, Swiftmend), recommends Nourish as the maintenance-tier Harmony refresher, frames "weave direct heals into raid-HoT rotation" as the central skill ceiling. |
| G9 | wowtbc.gg + Warcraft Tavern (search-excerpt only) | https://www.warcrafttavern.com/cataclysm/guides/pve-restoration-druid-rotation-cooldowns/ ; https://www.warcrafttavern.com/cataclysm/guides/pve-restoration-druid-stat-priority-reforging/ | Warcraft Tavern editorial | Cata Classic | Low (403 on direct fetch) — content reconstructed from search excerpts that quoted the pages. Used as tie-breaker when other sources disagreed. |

**Caveats on coverage:**

- **Wowhead Cata Classic guide pages are blocked from this fetch path** (consistent with the Holy Priest, Holy Paladin, and Resto Shaman surveys). The Wowhead overview page returned navigation chrome only.
- **Warcraft Tavern (G9) returned 403** to direct fetch on both rotation/stat sub-pages.
- **Wowpedia (Fandom) returned 403** to direct fetch; content reconstructed from Warcraft Wiki cross-references.
- The Tree of Life **spell page** on Warcraft Wiki redirects to a form/lore page; the actual spell mechanics page was 404.
- The Tree of Life duration is a known-disputed number. Multiple sources said 30 seconds; Wowhead spell tooltip says 25 seconds; per a search excerpt on patch 4.0.6, "Tree of Life was changed to last 25 seconds, down from 30 seconds." This survey adopts **25 seconds** as the 4.3.4 Cata Classic value (G5/Wowhead authoritative for hard numbers).
- None of the surveyed guides gave concrete numerical thresholds for self/tank/raid HP-emergency, mana conserve %, mana crisis %, drink %, or "Harmony refresh trigger HP%."

---

## Strong consensus (≥3 of the strategy-bearing guides agree)

| Item | Consensus | Spec disposition |
|---|---|---|
| **Resto Druid is HoT-roll-first, direct-heal-second.** | 7/7 | Spec rotation tier shape: Tier 1-3 = HoT maintenance; Tier 4 = Swiftmend; Tier 5 = Harmony refresher; Tier 6+ = HP-gated emergency direct heals. **Design departure from the other three healer specs.** |
| **Lifebloom 3-stack on the tank, maintained near 100% uptime via refresh-without-bloom.** | 6/6 | Tier 1 — keep 3-stack on `primary_tank`. Refresh BEFORE bloom via Healing Touch/Nourish/Regrowth via Empowered Touch. |
| **Mastery: Harmony refresh requires a direct heal cast every 10s.** Trigger spells: Nourish, Healing Touch, Regrowth, Swiftmend. | 5/5 | Tier 5 — if `harmony_buff_remaining_s < 4`, fire a direct heal. Default Harmony refresher = **Nourish**. Swiftmend on CD acts as opportunistic refresher. |
| **Wild Growth on cooldown** when ≥3 raid members within range take damage. | 7/7 | Tier 3 — `wild_growth_off_cd AND injured_allies_within_30y_of_anchor >= 3`. |
| **Swiftmend on cooldown** when there's an injured ally with Rejuvenation or Regrowth active. Spawns Efflorescence. | 6/6 | Tier 4 — `swiftmend_off_cd AND target_has_rejuv_or_regrowth AND target.hp_pct < 90`. |
| **Rejuvenation is the raid-distribute HoT** — applied to raid members taking sustained damage; not blanket-spammed. | 6/6 | Tier 2 — `target.hp_pct < 90 AND no_active_rejuv_on_target AND mana_pct > 40`. |
| **Tree of Life is a 3-min CD, 25-second-duration shapeshift cooldown.** +15% all healing, instant Regrowth, +120% armor, Lifebloom unlimited targets, Wild Growth +2 targets. | 6/6 | Cooldown profile — fire when raid hurting and CD up. |
| **Tranquility is a 5-target raid-channel CD** (8s channel, baseline 8 min CD reduced to **3 min** by Malfurion's Gift 2/2). | 5/5 | Cooldown profile — fire on raid emergency. |
| **Innervate is a 3-min CD mana cooldown** (free cast, no GCD). | 5/5 | Cooldown profile — fire on self when `self.mana_pct < 80`. Bot does NOT cast on ally healers (lacks mana awareness). |
| **Nature's Swiftness pairs with Healing Touch** for an instant + 50%-amplified emergency heal. 3-min CD. | 5/5 | Cooldown profile — fire when `tank.hp_pct < 30 OR ally.hp_pct < 25`. |
| **Barkskin** — 1 min CD, 12s duration, -20% damage. Use frequently. | 5/5 | Fire when `self.hp_pct < 70 AND in_combat`. |
| **Stat priority: Intellect > Spirit > Haste > Mastery > Crit** | 6/6 | Reforging guidance only. |
| **Primary haste breakpoint: 916-917 haste rating ≈ 12.5% raid-buffed** for 5th Rejuv tick. | 4/4 | Documented; no engine action. |
| **Glyph priority — Prime: Lifebloom + Rejuvenation + Swiftmend** | 4/4 | Spec uses these three primes. |
| **Glyph — Major: Rebirth + Healing Touch + Wild Growth** | 4/4 | Spec uses these three majors. |
| **Glyph — Minor: Mark of the Wild + Unburdened Rebirth + Dash** | 4/4 | Convenience minors, no rotation impact. |
| **Pre-pull rotation: Mark of the Wild → Lifebloom 3-stack on tank → Rejuvenation on tank → engage** | 3/4 | Spec encodes pre-pull sequence T-15s through T-1s. |
| **Empowered Touch is the Lifebloom-refresh enabler** at 100% (rank 2/2). | 4/4 | Direct heal on Lifebloom-stacked tank refreshes the stack. |
| **Omen of Clarity is the free-Regrowth proc.** Malfurion's Gift talent re-enables it from healing spells. | 4/4 | Tier 4.5 — when Clearcasting active, fire Regrowth. |
| **Efflorescence — 3-rank talent**, ground-AoE HoT on Swiftmend, 8y radius (NOT 30y), 7s duration, 3 most-injured smart-heal. | 3/3 | Auto-spawned by Swiftmend; placement bias toward cluster centroid. |
| **Tank-healing is Lifebloom 3-stack + Nourish/Healing Touch top-up** + Rejuv overlap. Hybrid raid+tank, not dedicated. | 4/4 | Spec is structurally hybrid; no tank-only/raid-only mode flag. |

---

## Disagreements

### 1. Tree of Life duration — 25s or 30s?

**Spec disposition**: **25 seconds, 3-min cooldown** (G5 wiki authoritative). 30s was original 4.0.1; nerfed in 4.0.6 — Cata Classic 4.3.4 = post-nerf.

### 2. Tree of Life Wild Growth targets — +2 or +3?

**Spec disposition**: **5 baseline → 7 in TOL form. With Glyph of Wild Growth slotted: 6 baseline → 8 in TOL.**

### 3. Glyph 3rd Prime — Swiftmend or Wild Growth?

**Spec disposition**: **Swiftmend in Prime, Wild Growth in Major.** 4/4 sources agree.

### 4. Secondary tree split — Balance or Feral?

**Spec disposition**: **Balance secondary: Moonglow 3/3 (-9% mana cost on key spells) + Nature's Majesty 2/2 (+4% spell crit).** Feral picks (Furor, Feral Swiftness) require Cat Form mid-fight which the bot is not designed to use.

### 5. Innervate threshold — 80% or lower?

**Spec disposition**: **`self.mana_pct < 80 AND innervate_off_cd AND in_combat`** (G1's recommendation). Adopts the aggressive threshold because Innervate is free + no-GCD.

### 6. Tranquility cooldown — 8 min or 3 min?

**Spec disposition**: **3 minutes** (assumes Malfurion's Gift 2/2 talent).

### 7. Lifebloom targeting outside vs inside Tree of Life

**Spec disposition**: **Single tank 3-stack outside TOL; spread to additional injured raid members during TOL.** Always prioritize maintaining the tank's 3-stack first.

### 8. Empowered Touch refresh reliability

**Spec disposition**: **2/2 Empowered Touch (100% refresh chance).** Bot's Lifebloom refresh predicate: any direct heal on the Lifebloom-stacked target refreshes the stack.

### 9. Wild Growth fired on 2 injured — yes or no?

**Spec disposition**: **3-injured threshold + mana floor `mana_pct > 30`.**

### 10. Nourish vs Healing Touch as Harmony refresher

**Spec disposition**: **Harmony refresher hierarchy: Swiftmend → Regrowth (Clearcasting proc) → Nourish (default) → Healing Touch (NS-paired only).**

### 11. Regrowth — situational or rotational?

**Spec disposition**: **Regrowth predicate = `Clearcasting_proc_active AND any_injured_ally`.** Mana-prohibitive without the proc.

### 12. Tree of Life — when to fire?

**Spec disposition**: **`raid.injured_count >= 4 AND raid.avg_hp_pct < 70 AND tol_off_cd`.** More aggressive than a player's "save for Bloodlust."

### 13. Tranquility — when to fire?

**Spec disposition**: **`raid.injured_count >= 5 AND raid.avg_hp_pct < 60 AND tranquility_off_cd`.**

### 14. Innervate target — self only or ally caster?

**Spec disposition**: **Self-cast only.** Post-4.2 nerf to ally-cast (5%) makes ally-cast not worth the GCD.

### 15. Living Seed — picked

**Spec disposition**: **3/3 Living Seed** standard build.

### 16. Glyph of Innervate — runtime swap-in

**Spec disposition**: **Default OFF; runtime swap-in for mana-stress encounters.**

### 17. Glyph of Healing Touch — picked

**Spec disposition**: **Slotted in Major.** Synergy with NS+HT pairing (CD reduction).

### 18. Tranquility during Tree of Life

**Spec disposition**: **TOL gives Tranquility +15% healing if cast in form; does NOT reduce Tranquility CD.** Bot ordering: TOL first (instant), then Tranquility immediately (channeled).

### 19. Pre-pull Lifebloom 3-stack timing

**Spec disposition**: **Mark of the Wild T-15s → Lifebloom T-7s → T-5s → T-3s → Rejuvenation T-1s.**

### 20. Efflorescence radius — 8y not 30y

**Spec disposition**: **8 yards** (G5 wiki authoritative). Tighter cluster predicate than Wild Growth.

---

## Items NOT addressed by ANY guide (pure author judgment)

| Spec item | Value chosen | Why |
|---|---|---|
| Self-emergency Barkskin trigger | self.hp_pct < 70 | G7 "use often"; bot interprets as aggressive auto-trigger. |
| NS+HT emergency trigger | tank.hp_pct < 30 OR ally.hp_pct < 25 | Guides "emergency" without quantification. |
| Tree of Life trigger | raid.injured_count >= 4 AND raid.avg_hp_pct < 70 | "Twice per fight" → fire eagerly. |
| Tranquility trigger | raid.injured_count >= 5 AND raid.avg_hp_pct < 60 | "2-3 per fight" cadence. |
| Innervate trigger | self.mana_pct < 80 | G1 explicit. |
| Wild Growth cluster trigger | wild_growth_off_cd AND injured_allies_within_30y_of_anchor >= 3 AND mana_pct > 30 | G7 "3+." |
| Swiftmend trigger | swiftmend_off_cd AND target_has_rejuv_or_regrowth AND target.hp_pct < 90 | "On CD" without HP gate. |
| Rejuvenation refresh | tank: missing OR <3s remaining; raid: missing AND target.hp_pct < 80 | "Selective spread." |
| Lifebloom refresh | lifebloom_remaining_s < 4 AND target == tank | "Refresh before bloom"; 4s buffer. |
| Nourish (Harmony) trigger | harmony_buff_remaining_s < 4 AND target.hp_pct < 90 | "Every 10s"; 4s remaining = proactive. |
| Regrowth (Clearcasting) trigger | clearcasting_proc_active AND any_injured_ally | Consensus "consume on Regrowth." |
| Mana conserve mode threshold | self.mana_pct < 50 | Cross-class default. |
| Mana crisis mode threshold | self.mana_pct < 25 | Cross-class default. |
| Out-of-combat drink threshold | 30% (force) / 95% (resume) | Cross-class default. |
| Combat-rez (Rebirth) priority | tank > healer > caster > melee | Cross-class generic. |
| Pre-pull Lifebloom 3-stack timing | T-7s/-5s/-3s | Heuristic; consensus is "stack pre-pull." |
| Stampeding Roar / Skull Bash | OFF (manual override) | Bot doesn't shapeshift mid-fight. |

---

## Author-judgment items — leveling-specific

```yaml
# Spell unlock breakpoints (sourced from Wowhead Cata Classic / Warcraft Wiki)
spell_unlocks:
  REJUVENATION:        {level: 3,  spell_id: 774}
  HEALING_TOUCH:       {level: 78, spell_id: 5185}    # raised in 4.0.6 from 3 to 78
  NOURISH:             {level: 8,  spell_id: 50464}   # lowered in 4.0.6 from 78 to 8
  REGROWTH:            {level: 12, spell_id: 8936}
  WILD_GROWTH:         {level: 60, spell_id: 48438}   # Resto spec
  LIFEBLOOM:           {level: 64, spell_id: 33763}
  TRANQUILITY:         {level: 68, spell_id: 740}
  INNERVATE:           {level: 28, spell_id: 29166}
  SWIFTMEND:           {level: 34, spell_id: 18562}   # Resto spec
  NATURES_SWIFTNESS:   {level: 32, spell_id: 17116}   # talent
  TREE_OF_LIFE:        {level: 81, spell_id: 33891}   # Resto capstone
  REBIRTH:             {level: 32, spell_id: 20484}
  MARK_OF_THE_WILD:    {level: 1,  spell_id: 1126}
  BARKSKIN:            {level: 32, spell_id: 22812}
  REVIVE:              {level: 12, spell_id: 50769}
  REMOVE_CORRUPTION:   {level: 14, spell_id: 2782}
  WRATH:               {level: 1,  spell_id: 5176}
  MOONFIRE:            {level: 4,  spell_id: 8921}
  # Talented:
  EFFLORESCENCE:       {requires_talent: true, talent_tier: 5}
  EMPOWERED_TOUCH:     {requires_talent: true, talent_tier: 4}
  NATURES_BOUNTY:      {requires_talent: true, talent_tier: 5}
  MALFURIONS_GIFT:     {requires_talent: true, talent_tier: 4}
  LIVING_SEED:         {requires_talent: true, talent_tier: 3}
  REVITALIZE:          {requires_talent: true, talent_tier: 5}
  # Mastery (Resto):
  MASTERY_HARMONY:     {level: 80, baseline_pct: 10, scaling_pct_per_mastery: 1.25}
```

| Default | Value | Why |
|---|---|---|
| Self-emergency HP threshold pre-60 | 25% | Cross-class. |
| Mana floor pre-60 | 0 | Cross-class. |
| Mana floor 60-70 (BC) | 55 / 30 | Cross-class. |
| Mana floor 70-80 (WotLK) | 45 / 25 | Cross-class. |
| Mana floor 80-85 (Cata) | 50 / 25 | Cross-class. |
| Pre-Lifebloom (lvl 1-63) | Skip Lifebloom; tank-heal via Rejuv + Nourish | Lifebloom unlocks at 64. |
| Healing Touch level-gate caveat | HT raised to lvl 78 in 4.0.6 | Pre-78, default direct heal is **Nourish** (lvl 8). |
| Pre-Wild Growth (lvl 1-59) | Skip; raid heal via Rejuv spread | WG unlocks 60. |
| Pre-Tranquility (lvl 1-67) | Skip | Tranquility unlocks 68. |
| Pre-Tree of Life (lvl 1-80) | Skip | TOL unlocks 81. |
| Pre-Mastery: Harmony (lvl 1-79) | Skip Harmony refresh; direct heals emergency-only | Mastery unlocks 80. |
| Pre-Innervate (lvl 1-27) | Skip mana CD | Unlocks 28. |
| Pre-Swiftmend (lvl 1-33) | Skip | Unlocks 34. |
| Pre-NS (lvl 1-31) | Skip NS+HT combo | Unlocks 32. |
| Pre-Barkskin (lvl 1-31) | Skip self-defensive CD | Unlocks 32. |
| Pre-Rebirth (lvl 1-31) | Skip combat-rez | Unlocks 32. |
| Solo questing rotation | Wrath spam → Moonfire → (Resto solo damage is weak; bot mandate is dungeon healing) | Per project scope. |
| Tank-healing default during dungeon leveling pre-64 | Rejuv + Nourish maintained; WG (60+) when Resto specced | Single-target Rejuv + Nourish until Lifebloom at 64. |
| Glyphs pre-25 | Skip | Glyph slots progressive. |
| Mark of the Wild on raid | Maintain on self always | Cross-class buff-maint. |

---

## Suggested next research passes

1. **Harmony refresh cadence** — telemetry: Harmony uptime % and HoT-tick-during-Harmony coverage.
2. **Lifebloom-via-Nourish-stack-build cadence** — measure rebuild-after-tank-swap GCD count.
3. **Wild Growth target count under Tree of Life + glyph** — verify saturation of 8-target cap.
4. **Swiftmend HoT-consumption with glyph OFF** — Rejuv vs Regrowth efficiency comparison.
5. **TOL + Tranquility pairing** — measure HPS uplift from +15% during channel.
6. **NS+HT pairing target** — lowest-HP vs always-tank vs always-lowest-raid.
7. **Innervate threshold** — telemetry vs G1's 80%.
8. **Efflorescence ground-AoE placement** — cluster centroid vs lowest-HP target.
9. **Pre-pull Lifebloom 3-stack timing** — T-7/-5/-3 vs T-3/-2/-1.
10. **Wild Growth on 2 injured** — does firing on 2 outperform 3?
11. **Rejuvenation blanket vs selective** — telemetry at high gear.
12. **Glyph of Innervate swap-in trigger** — mana-stress encounters.
13. **Tank-swap Lifebloom rebuild speed** — 3 GCDs default; could be faster?

---

## Source-coverage gaps for DBC verification

- **Harmony scaling formula** — base 10% direct + 10% HoT, +1.25% per mastery point. Mastery-rating-per-point at 85 (179.28 expected). DBC verification.
- **Tree of Life enhanced spell list** — DBC verification needed; wiki page was 404.
- **Swiftmend Efflorescence interaction without talent** — does Swiftmend without talent still place Efflorescence patch (at 0%)?
- **Glyph of Wild Growth target count effect** — confirm +1 target with +20% mana.
- **Lifebloom bloom-on-expiry healing amount** in Cata — small or no bloom?
- **Mastery: Harmony trigger spell list** — confirmed Nourish/HT/Regrowth/Swiftmend; does Tranquility trigger?
- **NS+HT 50% amplification** — confirm post-4.1.0 50% on HT.
- **Empowered Touch 100% refresh** — confirm no hidden ICD.
- **Omen of Clarity proc — Lifebloom-only or all heal ticks?** — Malfurion's Gift talent.
- **Clearcasting proc duration** — wiki says 15s post-4.2.
- **Tranquility level** — confirm 68.
- **Innervate ally-cast 5%** in 4.3.4.
- **TOL cooldown 3 min** vs 5 min discrepancy.
- **Wild Growth front-loaded tick curve** — engine model needs to encode this.

These are tagged for `docs/research/dbc-verification-checklist.md` style follow-up.

---

## Notes on prompt deviations

1. **Tree of Life duration 30s** → **Correction: 25 seconds** (4.0.6 nerf, in effect for Cata Classic 4.3.4).
2. **TOL Lifebloom auto-bloom** → TOL allows Lifebloom on unlimited targets; bloom-on-expiry is a Lifebloom mechanic regardless of TOL.
3. **Efflorescence radius 30y** → **Correction: 8 yards** (G5 wiki).
4. **Tranquility 8min/3min in TOL** → **Correction: 3min comes from Malfurion's Gift talent**, NOT from TOL form. TOL only adds +15% healing if cast in form.
5. **Wild Growth "8-target HoT"** → **Correction: 5 baseline / 6 with glyph / 7 in TOL / 8 with both**.
6. **Talent build "31 + 10 secondary"** → **Correction: 31 + 5 secondary** (~36 total points by level 85).
7. **Glyph Prime "Wild Growth?"** → **Correction: Swiftmend in Prime; WG in Major** (4/4 consensus).
8. **Healing Touch / Predatory Strikes / Tranquil Mind** → Predatory Strikes is Feral; Tranquil Mind not a real Cata talent. HT synergies: Empowered Touch (Lifebloom refresh) + Glyph of Healing Touch (-5s NS CD).

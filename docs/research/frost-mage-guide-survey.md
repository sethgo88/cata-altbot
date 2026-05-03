# Frost Mage 4.3.4 — Multi-Guide Strategy Survey

Audit trail for the strategy decisions that will be encoded in `docs/specs/frost-mage.md`. The spec doc is the source of truth for the bot; this doc records *which guides agreed*, *where they disagreed*, and *which thresholds were the bot author's judgment call rather than guide consensus*.

Survey conducted 2026-05-02 by the bot author. Numerical data (spell IDs, mana %, cast times, etc.) was sourced separately from Wowhead's Cata archive and Warcraft Wiki and is summarized here only where it directly informs strategy. Per project source registry, hard numbers come from the canonical reference set (Wowhead Cata Classic / Warcraft Wiki); strategy/rotation/threshold content is what's reconciled across the strategy-bearing guides below.

This is the **first ranged-DPS survey in the project** (after 5 healers: resto-shaman, holy-paladin, holy-priest, disc-priest, resto-druid). The project plan committed to Frost as the bot's mage spec; Section 1 below justifies that pick against the source consensus that Frost is *not* the meta DPS spec in 4.3.4.

---

## 1. Spec selection — Frost vs Fire vs Arcane (4.3.4 PVE)

**This section opens the survey because Mage has three viable specs and the project pick is non-obvious.**

### Source consensus on raw raid-DPS ranking (Dragon Soul / 4.3.4)

| Source | Fire | Arcane | Frost |
|---|---|---|---|
| Warcraft Tavern Cata DPS rankings (top-10% WCL parses, Phase 4 / Dragon Soul) | **3rd overall** | 11th overall | not in top 20 |
| wowtbc.gg Cata DPS rankings | top tier | mid tier | bottom tier |
| Simpleboost / Epiccarry / Aoeah Cata tier lists | A/S | B | F |
| Search-aggregated community consensus | "best damage spec throughout the entirety of the expansion" | "viable in P1 if mana managed; overshadowed by Fire later" | "predominantly a PvP spec... wouldn't be your go-to choice" |

The raw-DPS verdict is unambiguous: in single-target raid-DPS terms, **Fire >> Arcane >> Frost** in 4.3.4. Frost is widely placed F-tier for Dragon Soul.

### Why Frost is still the project pick

The project's primary use case is the leveling 1-85 LFD group bot, with all Cata 5-mans and raids as secondary. For a bot that fills *one* of five group slots and needs to be useful from level 1 through 85+, the decision criteria are not "highest single-target boss DPS" — they are: group utility, CC for leveling dungeons, interrupt presence, survivability, and leveling continuity.

Frost wins on **every** non-DPS criterion:

| Criterion | Frost | Fire | Arcane |
|---|---|---|---|
| **CC kit** | Polymorph + Frost Nova + Ring of Frost (lvl 83) + permanent Water Elemental Freeze | Polymorph only | Polymorph only |
| **Interrupt** | Counterspell baseline; Frost gets **Improved Counterspell** via 2 Arcane (silences 4s in addition to school-lock) | Same baseline | Same baseline |
| **Defensive toolkit** | **Most defensive spec.** Permanent Water Elemental tank-let; Frost Armor (15% phys reduction); Mage Ward + Mana Shield + Ice Block + Cold Snap reset | Baseline only | Baseline only |
| **Pet** | **Permanent Water Elemental** with 25s Freeze (FoF generator) | None | None |
| **Leveling viability** | Icy Veins explicitly: "the safest leveling option" | Mana-tight pre-60 | Mana-disastrous pre-60 |
| **Talent breakpoints sub-85** | FoF/Shatter/Brain Freeze online by ~30 | Critical Mass / Hot Streak deep | Mana issues compound |

**Spec disposition for the project: Frost.** Acknowledged downside: bottom-tier for mythic Dragon Soul parses. Documented in spec; not a blocker. Future option: a Fire spec doc as a sibling (out of scope for first pass).

---

## 2. Sources

| # | Guide | URL | Author / community | Date / patch | Confidence (rationale) |
|---|---|---|---|---|---|
| G1 | Icy Veins — Cata Classic Frost Mage Rotation, Cooldowns & Abilities | https://www.icy-veins.com/cataclysm-classic/frost-mage-pve-rotation-cooldowns-abilities | Icy Veins editorial | Cata Classic (4.3.4 mechanics) | High — primary rotation source. |
| G2 | Icy Veins — Cata Classic Frost Mage Stat Priority | https://www.icy-veins.com/cataclysm-classic/frost-mage-pve-stat-priority | Icy Veins editorial | Cata Classic | High — concrete numbers (17% hit cap, 1742 Hit Rating, 33.33% crit soft cap, conversion ratings at L85). |
| G3 | Icy Veins — Cata Classic Frost Mage Talent Builds & Glyphs | https://www.icy-veins.com/cataclysm-classic/frost-mage-pve-spec-builds-talents-glyphs | Icy Veins editorial | Cata Classic | Medium — confirms 8 Fire points for Ignite + Frostfire Bolt synergy; glyph picks. |
| G4 | Icy Veins — Cata Classic Frost Mage Leveling | https://www.icy-veins.com/cataclysm-classic/frost-mage-leveling | Icy Veins editorial | Cata Classic | High — clean spell-by-level table; talent priority path. |
| G5 | wowtbc.gg — Cata Classic Frost Mage | https://wowtbc.gg/cata/class-guides/frost-mage/ | wowtbc.gg community | Cata Classic | Medium — gives full opener sequence. |
| G6 | Warcraft Wiki — Frost spec + per-spell pages (Brain Freeze, Fingers of Frost, Shatter, Deep Freeze, Cold Snap, Icy Veins, Mirror Image, Time Warp, Counterspell, Polymorph, Frostbolt, Ice Lance, Frostfire Bolt, Ice Block, Mage Ward, Mana Shield, Water Elemental, Spellsteal, Arcane Brilliance, Frost Armor, Molten Armor, Mage Armor, Evocation, Frost Nova, Cone of Cold, Frostburn) | https://warcraft.wiki.gg/wiki/Brain_Freeze (and ~25 spell-specific pages) | wiki community | Original Cata 4.0–4.3 patch notes | High for hard mechanics. |
| G7 | gotwarcraft.com — Frost Mage Guide for PvP and PvE | https://gotwarcraft.com/guides/classguides/frostmage.php | community | Cata 4.3 | Medium-High — gives full talent build (2/8/31), Prime glyph trio, stat priority. |
| G8 | Engadget WoW Insider — "Arcane Brilliance: Frost mage Cataclysm talent analysis" (Christian Belt) | https://www.engadget.com/2010-11-13-arcane-brilliance-frost-mage-cataclysm-talent-analysis.html | community theorycrafter | Original Cata 4.0.x | Medium — talent-by-talent rationale. |
| G9 | Kuni's Frost Mage PVE Guide (Cataclysm) — MMO-Champion + Warmane mirror | https://www.mmo-champion.com/threads/813785-Kuni-s-Frost-Mage-PVE-Guide-(Cataclysm) ; https://forum.warmane.com/showthread.php?t=253661 | private-server raid theorycrafter | Original Cata 4.x | Low (403'd direct) — referenced via search snippets. |
| G10 | Warcraft Tavern — PvE Frost Mage Talents/Glyphs + Stat Priority/Reforging | https://www.warcrafttavern.com/cataclysm/guides/pve-frost-mage-talents-builds-glyphs/ | Warcraft Tavern editorial | Cata Classic | Low (403'd) — referenced via search snippets. |
| G11 | Wowhead Cata Classic Frost Mage guide pages | https://www.wowhead.com/cata/guide/classes/mage/frost/... | Wowhead editorial | Cata Classic | **Not directly fetched** — 404 on rotation/talent pages. |

**Caveats on coverage:**
- G10 / G11 / G9 returned 403/404 to direct fetch; content reconstructed from search excerpts.
- **Wowhead Cata Classic guide pages are blocked from this fetch path** (matches Holy Priest survey pattern).
- None of the surveyed guides gave concrete numerical thresholds for HP-emergency, mana conserve %, drink %, etc. Author judgment.

---

## 3. Strong consensus (≥3 of the strategy-bearing guides agree)

| Item | Consensus | Spec disposition |
|---|---|---|
| **Frost is utility-iconic, not top-DPS in 4.3.4.** Fire dominates, Arcane mid-tier, Frost bottom-tier for parse comps. | 5/5 | Documented in §1; bot picks Frost on utility/leveling/CC grounds. |
| **Talent build: 2 Arcane / 8 Fire / 31 Frost** — 8 Fire to reach Ignite (synergy with Frostfire Bolt); 2 Arcane typically Improved Counterspell. | 4/4 | Spec encodes 2/8/31 as canonical. |
| **Single-target priority — Deep Freeze on FoF > FFB on Brain Freeze + FoF > Frostfire Orb > Pet Freeze > FFB on Brain Freeze (no FoF) > Ice Lance on FoF > Frostbolt filler** | 4/4 | Spec rotation in priority order matching this exactly. |
| **Brain Freeze proc — 15% on Frost-school spells with chill — instant + free Frostfire Bolt for 15s.** | 5/5 | Tier 2/5 — combo with FoF for triple-multiplier; burn before expiry. |
| **Fingers of Frost proc — 20% per Frost spell with chill (3-rank: 7/14/20%). 2 charges, 15s. Causes next Ice Lance OR Deep Freeze to act as if frozen, +15% Ice Lance damage.** | 5/5 | Tier 1 Deep Freeze on FoF; Tier 6 Ice Lance on FoF. Always burn FoF before expiry. |
| **Shatter mechanic — multiplies crit chance against frozen targets ×3** (capped at 100%). Frozen via Frost Nova / Pet Freeze / FoF (synthetic) / Deep Freeze. | 5/5 | Spec assumes every Ice Lance / FFB / Deep Freeze cast is on a "shattered" target. Core damage multiplier. |
| **Deep Freeze — 30s CD, 5s stun, applies frozen status; on stun-immune bosses deals 1065-1334 base damage AND counts as frozen.** Spell ID 44572. | 5/5 | Tier 1 priority — fire when FoF active AND off-CD. Requires FoF or natural-freeze. |
| **Frostbolt is the filler / FoF-and-Brain-Freeze-generator.** 1.5s cast, 13% base mana. | 5/5 | Default filler when no Tier 1-6 trigger active. |
| **Cold Snap — 8 min CD, resets all Frost-school spell cooldowns** (Icy Veins, Cone of Cold, Ice Block, Frost Nova, Deep Freeze). | 5/5 | Aligned with Icy Veins for the burn window. |
| **Icy Veins — 3 min CD, 20s, +20% spellcasting haste, removes pushback.** | 5/5 | Cooldown profile — fire on pull (after Mirror Image precast). |
| **Mirror Image — 3 min CD, 30s, 3 copies cast Frostbolts, threat reduction.** Pre-cast 1.5s before pull. | 4/4 | Pre-pull at T-2s; subsequent recasts on CD. |
| **Time Warp — 5 min CD, 40s, +30% haste raid-wide ("Bloodlust" equiv).** Level 85. | 4/4 | Group-utility CD — fire on engage if no other Bloodlust class is in group. |
| **Counterspell — 24s CD, 7s school lockout (post-4.0.6), 40y range, level 9.** | 5/5 | Fire on enemy cast when interrupt-rotation-position OR no_other_interrupter_off_cd. |
| **Polymorph — 1.7s cast, 50s PVE, 35y, level 14.** | 5/5 | CC tier — auto only on cc-marked target in 5-man; OFF in raid. |
| **Mastery: Frostburn — 5% base + 2.5% per mastery point — bonus damage to all spells against frozen targets** AND bonus damage from Water Elemental. | 4/4 | Passive — no rotation-engine action. Implies bot must keep targets effectively-frozen via FoF. |
| **Stat priority — Int > Hit (17% / 1742) > Crit > Haste > Mastery** | 4/5 | Reforging guidance only. |
| **Glyphs — Prime: Frostbolt + Deep Freeze + Frostfire** | 5/5 | Universal. |
| **Glyphs — Major: Evocation + Blink + (Ice Barrier / Invisibility / Mirror Image)** | 4/4 | Spec defaults to Evocation + Blink + Mirror Image (sustained throughput). |
| **Default armor = Molten Armor** at max level. Mage Armor for mana-stress; Frost Armor reactive for melee. | 4/4 | Spec encodes Molten as default. |
| **Pre-pull rotation: Mirror Image → Frostbolt → Frostfire Orb → Pet Freeze → Deep Freeze → Icy Veins → Cold Snap → Deep Freeze again** | 4/4 | Encoded `pre_pull_sequence`. |
| **AoE rotation: Blizzard sustained + Cone of Cold burst + Flamestrike on 4+ sustained.** | 4/4 | Spec has AoE mode triggered by `enemies_within_15y_of_target >= 3`. |
| **Water Elemental — permanent pet for Frost spec at lvl 10** (Cata redesign). 25s CD Freeze AoE-roots and grants 2 FoF charges. | 4/4 | Re-summon on death. Freeze fires off-CD when enemies in 8y AND mage_FoF<2. |
| **Ice Block — 4 min CD, 10s full immunity** — panic button. | 4/4 | Defensive tier — fire when self.hp<20 AND no_other_save_ready. |
| **Mage Ward — 30s CD, 30s — absorbs Fire/Frost/Arcane.** Always-on. | 3/3 | Maintenance pass — recast every 30s in combat. |
| **Mana Shield is chip-mitigation cost-control, NOT primary defense.** | 3/3 | Only when MW on CD AND magic damage AND self.hp<50. |
| **Frost mage CC: Counterspell + Polymorph + Frost Nova + Ring of Frost** | 3/3 | Exposed to command interface. |

---

## 4. Disagreements

### 4.1. The 2 Arcane points — Improved Counterspell or filler?

**Spec disposition**: **2 in Improved Counterspell.** Reinforces the spec's iconic-interrupt rationale.

### 4.2. Early Frost — pick or skip?

**Spec disposition**: **Skip at max level (85 raid build).** Pick during leveling (10-40) — bot's `talent_loadout` differs by level bracket.

### 4.3. Frostfire Orb / Flame Orb talent — pick or skip?

**Spec disposition**: **Pick at 81.** Pre-81 the talent is empty and rotation skips Tier 3.

### 4.4. Major glyph slot — Ice Barrier vs Invisibility vs Mirror Image

**Spec disposition**: **Default Evocation + Blink + Mirror Image** (the MI glyph reduces CD by 30s — sustained throughput add).

### 4.5. Stat priority — Crit before or after Haste?

**Spec disposition**: **Int > Hit (17%) > Crit (33.33% soft cap) > Haste > Mastery.** Shatter ×3 amplifies crit value to soft cap.

### 4.6. Cold Snap timing — burn early or align with Bloodlust?

**Spec disposition**: **Cold Snap aligned to Icy Veins** in the opener; subsequent uses align with each Icy Veins recast.

### 4.7. Default armor — Molten or Mage?

**Spec disposition**: **Molten by default.** Auto-swap to Mage when `mana_pct < 30 sustained for 30s`. Auto-swap to Frost when ≥2 melee within 8y AND bot.hp<60.

### 4.8. Frostfire Bolt level — 56 (confirmed)

### 4.9. Counterspell auto-fire — selective with rotation discipline

**Spec disposition**: Auto only when interrupt-rotation-position OR no_other_interrupter_off_cd_in_2s.

### 4.10. Polymorph in 5-man — auto-CC or command-only?

**Spec disposition**: Auto only on cc-marked target (moon icon) AND humanoid/beast/critter AND no recent damage. Disabled in raid.

---

## 5. Items NOT addressed by ANY guide (pure author judgment)

| Spec item | Value chosen | Why |
|---|---|---|
| Self-emergency Ice Block | self.hp<20 AND no_other_save_ready | Bot self-preservation. |
| Mage Ward maintenance | Recast every CD (always-on) | Guides "use when needed." |
| Mana Shield trigger | MW on CD AND magic dmg AND self.hp<50 | Mana drain brutal. |
| Frost Nova trigger | melee_in_8y >= 2 AND self.hp<60 OR need_to_kite | Guides "kiting." |
| Cone of Cold trigger | enemies_in_12y_cone >= 2 AND off_CD | Guides "AoE." |
| Counterspell auto | target.casting AND remaining>0.5s AND (rot_pos OR no_other) | See §4.9. |
| Polymorph auto | cc_marker == "poly" AND humanoid/beast/critter AND no_dmg_3s AND party<6 | See §4.10. |
| Pet Freeze trigger | enemies_in_8y_target>=1 AND mage.FoF<2 AND off_CD | Guides "use for FoF." |
| Mirror Image pre-pull | T-2s | Bot-side timing. |
| Volcanic Potion | T-2s + 1-min mark if CDs aligned | Bot-side timing. |
| Mana conserve threshold | self.mana<50 | Cross-class default. |
| Mana crisis threshold | self.mana<25 | Cross-class default. |
| Drink threshold | 30%/95% | Cross-class default. |
| Evocation trigger | self.mana<40 AND no_emergency AND not_meleed | Channel — interruptible. |
| Mana Gem trigger | self.mana<60 AND charges>0 | No guide gave threshold. |
| AoE-mode entry | enemies_within_15y_target >= 3 | Cross-class default. |
| AoE-mode exit | <3 sustained for 5s | Hysteresis. |
| Ring of Frost | manual command only | Long cast, breaks on dmg. |
| Spellsteal | manual only | Encounter-specific. |
| Time Warp ownership | true unless shaman/hunter present | Manual override. |
| Pet re-summon | pet.dead AND (not_combat OR fight_remaining>30s) | Mana cost gating. |
| Threat dump (Invisibility) | self.threat>90% AND not_main_threat | Bot-side. |

---

## 6. Author-judgment items — leveling-specific

```yaml
# Spell unlock breakpoints (sourced from Wowhead Cata Classic / Warcraft Wiki / G4)
spell_unlocks:
  FROSTBOLT:           {level: 7,  spell_id: 116}
  FROST_NOVA:          {level: 8,  spell_id: 122}
  COUNTERSPELL:        {level: 9,  spell_id: 2139}
  WATER_ELEMENTAL:     {level: 10, spell_id: 31687}    # PERMANENT pet for Frost spec
  EVOCATION:           {level: 12, spell_id: 12051}
  POLYMORPH:           {level: 14, spell_id: 118}
  CONE_OF_COLD:        {level: 18, spell_id: 120}
  SLOW_FALL:           {level: 22, spell_id: 130}
  ICE_LANCE:           {level: 28, spell_id: 30455}
  CONJURE_MANA_GEM:    {level: 28, spell_id: 759}
  MANA_SHIELD:         {level: 30, spell_id: 1463}
  ICE_BLOCK:           {level: 30, spell_id: 45438}
  MOLTEN_ARMOR:        {level: 34, spell_id: 30482}
  MAGE_WARD:           {level: 36, spell_id: 543}
  MIRROR_IMAGE:        {level: 50, spell_id: 55342}
  BLIZZARD:            {level: 52, spell_id: 10}
  FROST_ARMOR:         {level: 54, spell_id: 7302}
  FROSTFIRE_BOLT:      {level: 56, spell_id: 44614}
  ARCANE_BRILLIANCE:   {level: 58, spell_id: 1459}
  DEEP_FREEZE:         {level: 60, spell_id: 44572}    # 31-Frost talent capstone OR baseline at 60
  MAGE_ARMOR:          {level: 68, spell_id: 6117}
  SPELLSTEAL:          {level: 70, spell_id: 30449}
  FLAME_ORB:           {level: 81, spell_id: 82731}    # converted to Frostfire Orb via talent
  RING_OF_FROST:       {level: 83, spell_id: 113724}
  TIME_WARP:           {level: 85, spell_id: 80353}
  # Talent-gated:
  SHATTER:                {requires_talent: true, talent_tier: 1, tree: frost}
  PIERCING_ICE:           {requires_talent: true, talent_tier: 1, tree: frost}
  IMPROVED_FROSTBOLT:     {requires_talent: true, talent_tier: 1, tree: frost}
  EARLY_FROST:            {requires_talent: true, talent_tier: 1, tree: frost}    # SKIPPED at 85
  ICE_FLOES:              {requires_talent: true, talent_tier: 2, tree: frost}
  FINGERS_OF_FROST:       {requires_talent: true, talent_tier: 3, tree: frost}    # 7/14/20%
  ICY_VEINS:              {requires_talent: true, talent_tier: 3, tree: frost}
  COLD_SNAP:              {requires_talent: true, talent_tier: 4, tree: frost}
  BRAIN_FREEZE:           {requires_talent: true, talent_tier: 4, tree: frost}    # 5/10/15%
  IMPROVED_CONE_OF_COLD:  {requires_talent: true, talent_tier: 5, tree: frost}
  FROSTFIRE_ORB:          {requires_talent: true, talent_tier: 6, tree: frost}
  IMPROVED_COUNTERSPELL:  {requires_talent: true, talent_tier: 2, tree: arcane}    # 2/4s silence
  IGNITE:                 {requires_talent: true, talent_tier: 1, tree: fire}      # +40% Fire dmg as DoT — applies to FFB
  MASTER_OF_ELEMENTS:     {requires_talent: true, talent_tier: 2, tree: fire}
  CRITICAL_MASS:          {requires_talent: true, talent_tier: 3, tree: fire}
```

Mana floors / leveling brackets: same cross-class convention as healers (0 pre-60; 55/30 BC; 45/25 WotLK; 50/25 Cata).

---

## 7. Suggested next research passes

1. 2-Arcane talent allocation telemetry (Improved Counterspell vs Netherwind partial-rank).
2. Water Elemental Freeze cadence — measure FoF generation efficiency.
3. Brain Freeze hold-vs-burn timing.
4. Cold Snap alignment with Bloodlust window vs Icy Veins.
5. Counterspell auto-fire predicate (rotation-position coordination).
6. AoE-mode threshold tuning.
7. Major glyph swaps (encounter-specific).
8. Armor-mode auto-swap thresholds.
9. Volcanic Potion re-pot timing.
10. Polymorph re-CC cadence.
11. Frost vs Fire spec sibling for max-level raid.
12. Pet leashing / re-summon tuning.

---

## 8. Source-coverage gaps for DBC verification

- Brain Freeze 15% at 3/3 — DBC Talent.dbc rank scaling.
- Fingers of Frost 20% at 3/3 — Talent.dbc.
- Shatter ×3 multiplier — Talent.dbc + spell mechanic verify.
- Frostfire Bolt periodic damage component — DBC EffectAura.
- Mage Armor mana regen % — Spell.dbc.
- Molten Armor crit % — Spell.dbc.
- Water Elemental Freeze CD — Spell.dbc.
- Water Elemental Freeze AoE radius (8y assumed) — Spell.dbc.
- Deep Freeze damage formula — Spell.dbc + spellpower coefficient.
- Cold Snap exact reset list — Spell.dbc + cooldown-category data.
- Counterspell silence at 2/2 Improved CS — Talent.dbc.
- Polymorph PVE duration (50s assumed) — Spell.dbc.
- Time Warp Temporal Displacement (10 min) — Spell.dbc.
- Frostfire Orb FoF proc capability — Spell.dbc + talent effect.
- Frostbolt slow effect — Spell.dbc.
- Glyph of Frostbolt +5% crit on shattered — Glyph effect interaction.
- Glyph of Deep Freeze +20% (additive vs multiplicative w/ Frostburn).
- **Critical: Ignite procs from Frostfire Bolt crits** — Talent.dbc effect proc-flag verification (determines if 8-Fire allocation is correct).

---

## 9. Notes on prompt deviations

1. **Polymorph at level 8** in prompt — **incorrect, level 14**.
2. **Counterspell at 22** — **incorrect, level 9**.
3. **Ice Lance at 66** — **incorrect, level 28**.
4. **Water Elemental "lvl 60 talent"** — **incorrect, permanent at 10 from Frost spec selection**.
5. **Polymorph "Sheep"** — confirmed.
6. **Brain Freeze instant + free FFB** — confirmed (15s buff).
7. **Cold Snap 8 min CD** — confirmed.
8. **Mastery: Frostburn 5% base + 2.5%/point** — confirmed.
9. **Frost as strongest leveling spec** — confirmed by G4.
10. **AoE: Blizzard + Cone of Cold + Flamestrike (sustained 4+)** — confirmed.

End of survey.

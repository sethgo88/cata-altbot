# Balance Druid 4.3.4 — Multi-Guide Strategy Survey

Audit trail for the strategy decisions that will be encoded in `docs/specs/balance-druid.md`. The spec doc is the source of truth for the bot.

Survey conducted 2026-05-02 by bot author. Strategy/threshold content reconciled across the strategy-bearing guides below. This is the **fifth ranged-DPS survey in the project**. Like Shadow Priest, **no spec-selection reconciliation is needed** — Balance is the only ranged-DPS Druid spec; Feral is melee/tank, Resto is healer (already documented).

---

## 1. Spec selection — Balance (no alternatives)

| Source | Balance Druid |
|---|---|
| Icy Veins Cata Classic Tier List | **A-tier** (DS 4.3) |
| wowtbc.gg Phase 4 (top-10% WCL) | rank #5–7 (high) |
| Wowhead Cata Tier 13 | A/A- tier |

**Spec disposition: Balance.** Strong sustained AoE/cleave + competitive single-target. Mechanic identity is the **Eclipse bar**. Utility: Innervate, Solar Beam, Typhoon, Force of Nature, Faerie Fire, Rebirth, Mark of the Wild.

---

## 2. Sources

| # | Guide | URL | Confidence |
|---|---|---|---|
| G1 | Icy Veins Balance Rotation/CDs | https://www.icy-veins.com/cataclysm-classic/balance-druid-pve-rotation-cooldowns-abilities | High |
| G2 | Icy Veins Balance Talents/Glyphs | https://www.icy-veins.com/cataclysm-classic/balance-druid-pve-spec-builds-talents-glyphs | High |
| G3 | Icy Veins Balance Stat Priority | https://www.icy-veins.com/cataclysm-classic/balance-druid-pve-stat-priority | High |
| G4 | Wowhead Cata Classic Balance | https://www.wowhead.com/cata/guide/classes/druid/balance/dps-overview-pve | Medium-High |
| G5 | wowtbc.gg Balance | https://wowtbc.gg/cata/class-guides/balance-druid/ | Medium |
| G6 | Warcraft Tavern Balance | https://www.warcrafttavern.com/cataclysm/guides/pve-balance-druid/ | Medium-High |
| G7 | Overgear Balance | https://overgear.com/guides/cataclysm-classic/balance-druid-guide/ | Medium |
| G8 | gotwarcraft Balance 4.3 | https://gotwarcraft.com/guides/classguides/balancedruid.php | Medium |
| G15 | Warcraft Wiki per-spell | https://warcraft.wiki.gg/wiki/<spell> | High |
| G16 | Wowhead Cata per-spell | https://www.wowhead.com/cata/spell=<id> | High |

**Caveats:** Several Cata-Classic-era guide pages 403'd; reconstructed via search-engine cache + Warcraft Wiki cross-reference.

---

## 3. Strong consensus

| Item | Consensus | Spec disposition |
|---|---|---|
| **Balance A-tier in DS 4.3.** | 4/4 | Documented. |
| **Spec utility:** Innervate, Solar Beam, Typhoon, Force of Nature, Faerie Fire, Rebirth, Mark of the Wild, Cyclone, Hibernate, Soothe. | 5/5 | Exposed. |
| **Moonkin Form** — required form, +15% magic damage, +10% Spell Crit aura (Moonkin Aura). | 5/5 | Always-on. |
| **Eclipse bar** — -100 (Lunar) to +100 (Solar). Wrath +13 toward Solar; Starfire -20 toward Lunar. Reaching extreme triggers 15s Eclipse buff. | 5/5 | Tracked -100..+100 + sign of last extremum. |
| **Lunar Eclipse** — +30% Arcane direct damage (Starfire/Moonfire/Starsurge). | 5/5 | Cast Starfire while active. |
| **Solar Eclipse** — +30% Nature direct damage (Wrath/Insect Swarm/Hurricane/Sunfire). | 5/5 | Cast Wrath while active. |
| **Eclipse swing direction** — bar moves one direction at a time; flip only after Eclipse consumed. | 4/4 | Encoded. |
| **Starsurge** — 2s cast (instant via Shooting Stars), 15s CD, +15 Eclipse energy in current direction. | 4/4 | Cast on CD; never delay. |
| **Shooting Stars** (talent) — 2/4% per Moonfire/Sunfire tick crit; instant Starsurge proc. | 4/4 | If proc active, Starsurge regardless of bar. |
| **Moonfire** — instant 18s DoT, Arcane school. Apply during Lunar Eclipse for snapshot. | 4/5 | Lunar-Eclipse refresh window. |
| **Sunfire** — talent-conditional; Solar Eclipse converts Moonfire→Sunfire (Nature DoT, 18s). | 4/5 | Solar-Eclipse refresh window. |
| **Moonfire ↔ Sunfire mutual exclusion** — only one active per target. | 4/4 | Track `dot_id`. |
| **Insect Swarm** — instant 14s Nature DoT. NO -hit debuff in 4.3.4 (legacy from Wrath). | 4/5 | Maintain on primary. |
| **Wrath** — 2s Nature, +13 Solar (+1 with Euphoria). | 5/5 | Solar-filler. |
| **Starfire** — 2.7s Arcane, -20 Lunar (-1 with Euphoria). | 5/5 | Lunar-filler. |
| **Nature's Grace** (talent) — Eclipse trigger procs +15% haste 15s. | 4/4 | Implicit. |
| **Euphoria** (talent) — +1 Eclipse energy on non-Eclipse Wrath/Starfire; +12% mana on Eclipse trigger. | 4/4 | Mandatory. |
| **Lunar Shower** (talent) — Moonfire/Sunfire stacking damage buff (max 3, +15%/+30%/+45%). | 4/4 | Movement-DPS rule. |
| **Hurricane** — 10s channel, 8% mana/sec, Nature AoE. Solar-favored. | 4/4 | AoE channel. |
| **Force of Nature** — 3 Treants, 30s, 3min CD. | 5/5 | Cast on CD. |
| **Innervate** — 3min CD, 20% mana over 10s. Glyph: 50% to caster when cast on others. | 4/4 | Self-cast on mana_pct < 50. |
| **Typhoon** — 20s CD, knockback. Glyph removes knockback. | 4/4 | Manual command-only. |
| **Solar Beam** — 1min CD, 10y AoE silence, 10s. Lvl 85 talent-tier 7. | 4/4 | Auto-silence + manual command. |
| **Faerie Fire** — -12% armor, instant, 6s CD. | 3/4 | Apply if armor debuff missing. |
| **Mastery: Total Eclipse** — +2%/point to Eclipse damage modifier (16% baseline). | 4/4 | Reforge target. |
| **Talent build — 31/2/8** (31 Balance + Feral 2 Furor + Resto 8 Heart of the Wild path). | 4/5 | Encoded. |
| **Stat priority** — Int > Hit (17%/1742) > Haste > Mastery > Crit. | 4/5 | Encoded. |
| **Spell hit cap 17% / 1742 at L85.** | 5/5 | Hard cap. |
| **Balance of Power** — 50%/100% Spirit→Hit. | 5/5 | Pre-condition for spirit reforge. |
| **Glyphs — Prime: Wrath + Insect Swarm + Starfire.** | 5/5 | Default. |
| **Glyphs — Major: Starfall + Innervate + Typhoon.** | 4/5 | Default. |
| **Glyphs — Minor: Mark of the Wild + Unburdened Rebirth + Aquatic Form.** | 3/3 | Default. |
| **Pre-pot Volcanic Potion** — +1200 Int 25s. T-2s + 1-min mark. | 4/4 | Encoded. |
| **Opener:** pre-pot T-2s → Insect Swarm → Moonfire → Wrath spam to Solar → Starsurge → Sunfire → Wrath until Solar runs out → Starfire spam to Lunar → Starsurge → Moonfire → Starfire until Lunar runs out → repeat. | 4/4 | Encoded. |
| **Starfall** — 90s CD (60s with Glyph), 10s passive AoE aura, 20 stars. | 5/5 | Cast on CD. |
| **Wild Mushroom + Detonate** — 3-charge AoE (manual placement). | 3/4 | Manual command-only. |
| **Mana cascade** — Innervate self → Replenishment passive → Euphoria 12% on Eclipse → Mana Tide. | 4/4 | Encoded. |

---

## 4. Disagreements

### 4.1. Eclipse generation values (Wrath +13, Starfire -20 baseline)

**Spec disposition: Wrath = +13, Starfire = -20**, +1 each with Euphoria 2/2. Tagged DBC verify.

### 4.2. Starsurge at-zero direction

**Spec disposition: defaults Lunar at zero/no-memory.**

### 4.3. Shooting Stars proc — per-tick crit

**Spec disposition: 2/4% per rank per Moonfire/Sunfire tick crit.**

### 4.4. Lunar Shower stack count

**Spec disposition: stacks to 3** (+15%/+30%/+45%).

### 4.5. Moonkin Form mana drain

**Spec disposition: shape-shift cost ~13% base mana once at session start.**

### 4.6. Insect Swarm hit debuff — REMOVED in 4.3.4

**Spec disposition: pure DoT, no -hit debuff.**

### 4.7. Talent split — 31/2/8 default

**Spec disposition: 31/2/8** (Heart of the Wild path); 31/0/10 alt for Genesis-heavy.

### 4.8. Mastery vs Haste

**Spec disposition: Hit > Haste > Mastery > Crit** for single-target.

### 4.9. Haste breakpoints — DoT extra ticks

**Spec disposition:**
- Insect Swarm 8th tick at 12.5% haste
- Moonfire 5th tick at 25% haste
- Sunfire 5th tick at 25% haste
- Tagged DBC verify.

### 4.10. Glyph third Prime — Starfire (default)

**Spec disposition: Glyph of Starfire** (extends Moonfire +9s under Lunar).

### 4.11. Force of Nature snapshot

**Spec disposition: NOT snapshot** (safer assumption).

### 4.12. Wild Mushroom in single-target

**Spec disposition: NOT in single-target rotation; AoE-only via manual command.**

---

## 5. Items NOT addressed (author judgment)

| Spec item | Value | Why |
|---|---|---|
| Eclipse target sign at start | Solar first | Wrath pre-cast pushes Solar; Starfire slower. |
| DoT refresh threshold | ≤2s remaining AND Eclipse buff active | Snapshot benefit. |
| DoT non-Eclipse refresh | ≤0.5s remaining | Avoid losing DoT entirely. |
| Starsurge cast | proc OR CD ready, regardless of Eclipse | Free instant on proc. |
| Shooting Stars priority | Starsurge instant > maintain DoTs > filler | Free cast. |
| Mid-Eclipse filler | Wrath if Solar, Starfire if Lunar | Maintain bonus. |
| Pre-Eclipse filler | direction-aware | Bar moving toward target. |
| Movement DPS | Moonfire/Sunfire (Lunar Shower) + Starsurge if proc | Instant only. |
| Mushroom AoE threshold | enemies_within_8y >= 5 | Detonate + Hurricane chain. |
| Hurricane AoE threshold | enemies_within_8y >= 4 AND Solar Eclipse | Best AoE during Solar. |
| AoE multi-DoT | enemies >= 3 AND living >= 10s | DoT GCD investment. |
| Innervate self-trigger | mana_pct < 40 | Mana CD. |
| Innervate manual on healer | command-driven | Better target most fights. |
| Solar Beam | enemy is casting AND CD ready | Auto-silence on caster. |
| Typhoon | NEVER auto in raid | Manual only. |
| Faerie Fire | apply if armor debuff missing | Raid debuff. |
| Force of Nature | cast on CD | DPS pet. |
| Starfall | cast on CD | Free DPS. |
| Mark of the Wild | maintain on raid | Buff. |
| Rebirth in combat | manual command-only | Not auto. |
| Pre-pot timing | T-2s + 1-min mark | Bot-side. |
| Hibernate/Cyclone | manual only | Utility. |
| Soothe | manual on enrage dispel | Utility. |

---

## 6. Author-judgment items — leveling

```yaml
spell_unlocks:
  WRATH:                   {level: 1,  spell_id: 5176}
  MARK_OF_THE_WILD:        {level: 1,  spell_id: 1126}
  REJUVENATION:            {level: 4,  spell_id: 774}
  MOONFIRE:                {level: 4,  spell_id: 8921}
  HEALING_TOUCH:           {level: 6,  spell_id: 5185}
  ENTANGLING_ROOTS:        {level: 8,  spell_id: 339}
  STARFIRE:                {level: 10, spell_id: 2912}
  REGROWTH:                {level: 12, spell_id: 8936}
  FAERIE_FIRE:             {level: 14, spell_id: 770}
  REBIRTH:                 {level: 18, spell_id: 20484}
  REMOVE_CORRUPTION:       {level: 20, spell_id: 2782}
  HIBERNATE:               {level: 20, spell_id: 2637}
  THORNS:                  {level: 22, spell_id: 467}
  CYCLONE:                 {level: 24, spell_id: 33786}
  INNERVATE:               {level: 28, spell_id: 29166}
  INSECT_SWARM:            {level: 30, spell_id: 5570}
  HURRICANE:               {level: 42, spell_id: 16914}
  TRANQUILITY:             {level: 60, spell_id: 740}
  STARFALL:                {level: 60, spell_id: 48505}
  TYPHOON:                 {level: 60, spell_id: 61384}
  STARSURGE:               {level: 78, spell_id: 78674}
  SOLAR_BEAM:              {level: 81, spell_id: 78675}
  WILD_MUSHROOM:           {level: 85, spell_id: 88747}
  WILD_MUSHROOM_DETONATE:  {level: 85, spell_id: 88751}
  # Talent-gated (Balance):
  MOONKIN_FORM:            {requires_talent: true, spell_id: 24858}
  SUNFIRE:                 {requires_talent: true, spell_id: 93402}
  EUPHORIA:                {requires_talent: true}
  NATURES_GRACE:           {requires_talent: true}
  SHOOTING_STARS:          {requires_talent: true}
  LUNAR_SHOWER:            {requires_talent: true}
  FORCE_OF_NATURE:         {requires_talent: true, spell_id: 33831}
  BALANCE_OF_POWER:        {requires_talent: true}
  EARTH_AND_MOON:          {requires_talent: true}
  STARLIGHT_WRATH:         {requires_talent: true}
  GENESIS:                 {requires_talent: true}
  MOONFURY:                {requires_talent: true}
  GALES_OF_WIND:           {requires_talent: true}
  # Cross-tree:
  FUROR:                   {requires_talent: true, tree: feral}
  HEART_OF_THE_WILD:       {requires_talent: true, tree: feral}
  BLESSING_OF_THE_GROVE:   {requires_talent: true, tree: restoration}
  # Mastery / passives:
  MASTERY_TOTAL_ECLIPSE:   {passive_mastery: true, base_pct: 16, per_point_pct: 2}
  ECLIPSE_BAR:             {passive_resource: true, range: [-100, 100], default: 0}
```

**Druid is mana-only.** Mana cascade: Euphoria 12% on Eclipse → Innervate self → Replenishment passive → Mana Tide.

**Pre-Moonkin leveling** (under L20 when Moonkin talent locked): Wrath + Moonfire only. Below L78 (no Starsurge), 2-spell Wrath/Starfire alternation.

---

## 7. Suggested next research passes

1. Eclipse energy values (Wrath +13/+15; Starfire -20/-25).
2. Euphoria mana % on Eclipse (10% vs 12%).
3. Shooting Stars trigger (direct cast vs tick crit).
4. Lunar Shower stacks (2 vs 3).
5. Sunfire spell ID + cast mechanic.
6. Starsurge at-zero direction logic.
7. Force of Nature Eclipse snapshot.
8. Haste breakpoints at L85 raid-buffed.
9. Mastery Total Eclipse base % + scaling.
10. T13 set bonuses.
11. Glyph of Starfire Moonfire-extension cap.
12. Wild Mushroom: Detonate damage formula.
13. Mark of the Wild Cata stat values.
14. Nature's Grace haste % + duration.
15. Starfall random target priority.

---

## 8. DBC verification

- Wrath cost/cast/Eclipse-energy. Spell.dbc 5176.
- Starfire cost/cast/Eclipse-energy. Spell.dbc 2912.
- Starsurge 2s cast / 15s CD / +15 Eclipse. Spell.dbc 78674.
- Moonfire instant / 18s DoT. Spell.dbc 8921.
- Sunfire 93402.
- Insect Swarm 14s DoT / no hit debuff. Spell.dbc 5570.
- Hurricane 10s channel. Spell.dbc 16914.
- Force of Nature 3 Treants / 30s / 3min. Spell.dbc 33831.
- Innervate 20% mana / 10s / 3min. Spell.dbc 29166.
- Typhoon 20s CD. Spell.dbc 61384.
- Solar Beam 1min CD. Spell.dbc 78675.
- Faerie Fire instant / -12%. Spell.dbc 770.
- Starfall 90s CD / 10s. Spell.dbc 48505.
- Wild Mushroom 88747 / Detonate 88751.
- Moonkin Form +15% magic / +10% crit aura. Spell.dbc 24858.
- Eclipse Lunar buff +30% Arcane. Spell.dbc 48518.
- Eclipse Solar buff +30% Nature. Spell.dbc 48517.
- Mastery Total Eclipse base + per-point. Mastery DBC.
- Euphoria rank effect. Talent.dbc.
- Nature's Grace rank effect. Talent.dbc.
- Shooting Stars rank effect. Talent.dbc.
- Lunar Shower rank effect. Talent.dbc.
- Balance of Power rank effect. Talent.dbc.
- Earth and Moon -8% spell damage debuff. Talent.dbc.
- Moonfury rank effect. Talent.dbc.
- Genesis rank effect. Talent.dbc.
- Furor rank effect (Feral). Talent.dbc.
- Heart of the Wild rank effect (Feral). Talent.dbc.
- Glyphs (Wrath, Insect Swarm, Starfire, Starfall, Innervate, Typhoon). Glyph.dbc.
- T13 2P/4P bonuses. Item set DBC.
- Volcanic Potion +1200 Int. Item DBC.

---

## 9. Notes on prompt deviations

1. **Lunar Eclipse +25-30%** — corrected to **+30%**.
2. **Solar Eclipse +25-30%** — corrected to **+30%**.
3. **Starsurge generates BOTH directions** — corrected: +15 in current direction.
4. **Insect Swarm hit debuff** — REMOVED in 4.3.4.
5. **Lunar Shower 3-stack** — confirmed.
6. **31 Balance + 10 Resto/Feral** — refined to **31/2/8** with Heart of the Wild path.
7. **Moonkin Form 30+ talent** — talent-gated, first reachable around L20.
8. **Insect Swarm 8** — corrected to **L30 baseline** in Cata.
9. **Solar Beam 81+** — corrected to **L85 talent**.
10. **Sunfire** — talent-replaces Moonfire while in Solar Eclipse.
11. **Starfall** added (Balance major DPS CD).
12. **Wild Mushroom + Detonate** added (Cata-new mechanic).

End of survey.

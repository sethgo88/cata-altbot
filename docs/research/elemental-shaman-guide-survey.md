# Elemental Shaman 4.3.4 — Multi-Guide Strategy Survey

Audit trail for the strategy decisions that will be encoded in `docs/specs/elemental-shaman.md`. The spec doc is the source of truth for the bot.

Survey conducted 2026-05-02. Strategy/threshold content reconciled across the strategy-bearing guides below. This is the **sixth and final ranged-DPS survey in the project**. Like Shadow Priest and Balance Druid, **no spec-selection reconciliation is needed** — Elemental is the only ranged-caster Shaman spec; Enhancement is melee, Restoration is healer (already documented).

---

## 1. Spec selection — Elemental (no alternatives)

| Source | Elemental Shaman |
|---|---|
| Icy Veins Cata Classic Tier List | **A-tier** (DS 4.3) |
| wowtbc.gg Phase 4 (top-10% WCL) | rank #6–8 |
| Wowhead Cata Tier 13 | A-tier |
| Warcraft Tavern DS rankings | A-tier (cleave/AoE + Bloodlust) |

**Spec disposition: Elemental.** Strong sustained AoE/cleave (Earthquake + Magma + Chain Lightning + Fire Elemental) + competitive single-target. Mechanic identity is **Lightning Shield charge stack consumed via Fulmination Earth Shock**. Bloodlust/Heroism brings massive group utility on top of the totem buff suite.

---

## 2. Sources

| # | Guide | URL | Confidence |
|---|---|---|---|
| G1 | Icy Veins Elemental Rotation/CDs | https://www.icy-veins.com/cataclysm-classic/elemental-shaman-pve-rotation-cooldowns-abilities | High |
| G2 | Icy Veins Elemental Talents/Glyphs | https://www.icy-veins.com/cataclysm-classic/elemental-shaman-pve-spec-builds-talents-glyphs | High |
| G3 | Icy Veins Elemental Stat Priority | https://www.icy-veins.com/cataclysm-classic/elemental-shaman-pve-stat-priority | High |
| G4 | Wowhead Cata Classic Elemental | https://www.wowhead.com/cata/guide/classes/shaman/elemental/dps-overview-pve | Medium-High |
| G5 | wowtbc.gg Elemental | https://wowtbc.gg/cata/class-guides/elemental-shaman/ | Medium |
| G6 | Warcraft Tavern Elemental | https://www.warcrafttavern.com/cataclysm/guides/pve-elemental-shaman/ | Medium-High |
| G7 | Method Cata Elemental | https://www.method.gg/guides/cataclysm/elemental-shaman | Medium |
| G8 | gotwarcraft Elemental 4.3 | https://gotwarcraft.com/guides/classguides/elementalshaman.php | Medium |
| G15 | Warcraft Wiki per-spell | https://warcraft.wiki.gg/wiki/<spell> | High |
| G16 | Wowhead Cata per-spell | https://www.wowhead.com/cata/spell=<id> | High |

---

## 3. Strong consensus

| Item | Consensus | Spec disposition |
|---|---|---|
| **Elemental A-tier in DS 4.3.** | 4/4 | Documented. |
| **Spec utility:** Bloodlust/Heroism, Wind Shear, Hex, Purge, Cleansing/Tremor Totem, Wrath of Air, Mana Spring, Fire/Earth Elemental Totems, Reincarnation. | 5/5 | Exposed. |
| **Lightning Bolt** — 2.5s cast, primary single-target filler. | 5/5 | Default filler. |
| **Lava Burst** — 2s cast, 8s CD, **auto-crit when target has Flame Shock**. Top per-cast damage. | 5/5 | Cast on CD whenever FS up. |
| **Flame Shock** — instant Fire DoT, 18s+, shared 6s shock CD. **Lava Burst depends on this DoT** — maintenance mandatory. | 5/5 | Maintain at all times. |
| **Earth Shock** — instant, shared 6s shock CD. **Primary purpose: consume LS charges via Fulmination at 7+.** | 5/5 | Press at 7+ LS charges. |
| **Frost Shock** — instant slow. Niche. | 4/5 | Manual / kiting only. |
| **Chain Lightning** — 2s cast, 3-target, ~3s CD talented, AoE Nature. **Generates 1 LS charge per hit.** | 5/5 | AoE filler at 3+ targets. |
| **Thunderstorm** — 45s CD, AoE damage + 20y knockback + 8% mana. | 5/5 | Mana CD. |
| **Earthquake** — 2.5s cast, 10s CD, ground AoE channel. | 5/5 | Cast on CD at 4+ stacked. |
| **Lightning Shield** — passive, 3 base / 9 max charges with Fulmination. Procs damage on melee hit. | 5/5 | Always-on. |
| **Fulmination** — Earth Shock at 7+ charges expends all above 1. | 5/5 | Earth Shock priority gate. |
| **Elemental Mastery** — 2-min CD (talented from 3min), instant cast next + 20% haste 30s. | 4/5 | Sync with Lava Burst. |
| **Fire Elemental Totem** — 5min CD, 2min duration. | 5/5 | Cast on CD; pre-pull. |
| **Earth Elemental Totem** — 10min CD, 2min Earth pet. | 4/4 | Manual / emergency. |
| **Mastery: Elemental Overload** — chance for LB/LvB/CL to fire 2nd "overload" copy at 75% damage. | 4/5 | Reforge target. |
| **Wind Shear** — 6s CD interrupt. | 5/5 | Auto-interrupt. |
| **Bloodlust / Heroism** — 5min CD, 30% raid haste 40s, applies Sated. | 5/5 | Pull-time CD by default. |
| **Hex** — 45s CD, 1.5s cast, 1min PvE CC. | 4/4 | Manual command-only. |
| **Purge** — instant, removes 1 magic buff. | 4/4 | Manual / situational. |
| **Searing Totem** — 1min duration, single-target damage. | 5/5 | Single-target totem. |
| **Magma Totem** — ~21s duration, AoE pulses (8y). | 5/5 | 3+ targets. |
| **Wrath of Air Totem** — +5% raid spell haste aura. | 5/5 | Always-on Air totem. |
| **Mana Spring Totem** — Water totem regen aura. | 4/5 | Default Water totem. |
| **Mana Tide Totem** — **Resto-only** in Cata 4.3.4. | 3/5 | NOT in Elemental. |
| **Stoneskin / SoE Totem** — Earth-school passive buff. | 4/4 | Drop on pull. |
| **Cleansing / Tremor Totem** — manual swap when needed. | 3/4 | Encounter-conditional. |
| **Talent build 31/2/8** (Elemental + Enh + Resto). | 3/5 | Default. |
| **Stat priority** — Int > Hit (17%/1742) > Haste > Mastery > Crit. | 4/5 | Encoded. |
| **Spell hit cap 17% / 1742 at L85.** | 5/5 | Hard cap. |
| **Elemental Precision** — Spirit→Hit at 50/100%. | 5/5 | Pre-condition for spirit reforge. |
| **Glyphs — Prime: Lightning Bolt + Lava Burst + Flame Shock.** | 5/5 | Default. |
| **Glyphs — Major: Stoneclaw + Thunderstorm + Fire Elemental Totem.** | 4/5 | Default. |
| **Glyphs — Minor: Astral Recall + Renewed Life + Thunderstorm.** | 3/3 | Default. |
| **Pre-pot Volcanic Potion** — +1200 Int 25s. T-2s + 1-min mark. | 4/4 | Encoded. |
| **Opener:** pre-pot T-2s → totems pre-pull → Fire Elemental on pull → FS → LvB → LB filler → ES at 7+ charges → Bloodlust → EM → continue priority. | 4/4 | Encoded. |
| **Lava Surge** — Flame Shock ticks chance to reset Lava Burst CD + instant LvB. | 5/5 | Highest priority on proc. |
| **Spiritwalker's Grace** — 2min CD, 15s, cast while moving. | 5/5 | Movement-DPS CD. |

---

## 4. Disagreements

### 4.1. Talent split — 31/2/8 vs 31/0/10

**Spec disposition: 31/2/8** default; 31/0/10 alt for mana-stress.

### 4.2. Stat priority — Haste > Mastery default

**Spec disposition: Haste > Mastery > Crit** (re-evaluate with T13 4P out of scope for v1).

### 4.3. Haste soft cap

**Spec disposition: no hard cap** — full Haste reforge. DBC verify.

### 4.4. Lava Burst priority vs Fulmination

**Spec disposition: Lava Burst (off-CD AND FS up) > Fulmination ES (≥7 charges) > FS refresh > Earthquake > LB filler.** ES at 9 charges mandatory (cap-protect).

### 4.5. Flame Shock refresh

**Spec disposition: refresh ≤3s remaining AND target lives ≥6s.**

### 4.6. Earthquake threshold

**Spec disposition: 4+ stacked AND living ≥10s.**

### 4.7. Chain Lightning vs Lightning Bolt threshold

**Spec disposition: Chain Lightning at 3+ targets.**

### 4.8. Searing vs Magma Totem swap

**Spec disposition: Searing single-target; Magma at 3+ enemies in 8y AND living ≥15s.**

### 4.9. Fire Elemental Totem auto-on-CD

**Spec disposition: cast on CD** by default; manual override available.

### 4.10. Bloodlust on pull

**Spec disposition: pull-time** (T+0); togglable via chat command.

### 4.11. Elemental Mastery sync

**Spec disposition: cast on CD when Lava Burst available.**

### 4.12. Glyph third Prime — Flame Shock (default)

**Spec disposition: Glyph of Flame Shock** for GCD efficiency.

### 4.13. Spiritwalker's Grace usage

**Spec disposition: cast on CD when movement is required mid-Lightning-Bolt.**

---

## 5. Items NOT addressed (author judgment)

| Spec item | Value | Why |
|---|---|---|
| Lightning Shield maintenance | refresh if missing OR <10s remaining | Always-on. |
| Pre-pull totem set | Searing + Wrath of Air + Stoneskin/SoE + Mana Spring | Standard 4-totem layout. |
| Fire Elemental pre-pull | T-15s | 2min covers Bloodlust + opener. |
| Earth Elemental | manual command-only | Tank emergency. |
| ES at 9 charges | mandatory (cap-protect) | Avoid overflow. |
| Lava Surge proc | always cast LvB immediately | Free instant. |
| AoE rotation | Earthquake → Magma refresh → Chain Lightning → FS on focus | 4+ stacked. |
| Cleave (2 targets) | maintain FS on both → LvB on focus → LB | Single-target + extra FS. |
| Hex | manual command-only | CC. |
| Purge | manual / off enemy buff | Situational. |
| Wind Shear | auto-interrupt enemy casts ≤300ms before completion | Reactive. |
| Tremor / Cleansing | manual swap on encounter need | Replaces Stoneskin / Mana Spring. |
| Reincarnation | passive 30min CD | Out of scope. |
| Mana cascade | Thunderstorm → Mana Tide raid → potion → SWG mid-fight | Encoded. |
| Movement DPS | FS refresh + ES + SWG | Instants only. |
| Pre-pot timing | T-2s + 1-min mark | Bot-side. |
| Bloodlust on pull | yes (default) | Manual override. |
| Elemental Mastery sync | with Lava Burst | CD sync. |
| Mastery reforge | secondary after Hit cap | Standard. |
| Crit reforge AWAY | yes | Lowest stat weight. |

---

## 6. Author-judgment items — leveling

```yaml
spell_unlocks:
  LIGHTNING_BOLT:           {level: 1,  spell_id: 403}
  EARTH_SHOCK:              {level: 4,  spell_id: 8042}
  HEALING_WAVE:             {level: 1,  spell_id: 331}
  STONESKIN_TOTEM:          {level: 4,  spell_id: 8071}
  HEALING_SURGE:            {level: 7,  spell_id: 8004}
  LIGHTNING_SHIELD:         {level: 8,  spell_id: 324}
  FLAME_SHOCK:              {level: 10, spell_id: 8050}
  SEARING_TOTEM:            {level: 10, spell_id: 3599}
  GHOST_WOLF:               {level: 16, spell_id: 2645}
  STRENGTH_OF_EARTH_TOTEM:  {level: 16, spell_id: 8075}
  WIND_SHEAR:               {level: 16, spell_id: 57994}
  TREMOR_TOTEM:             {level: 18, spell_id: 8143}
  FROST_SHOCK:              {level: 18, spell_id: 8056}
  PURGE:                    {level: 22, spell_id: 370}
  MAGMA_TOTEM:              {level: 26, spell_id: 8190}
  MANA_SPRING_TOTEM:        {level: 26, spell_id: 5675}
  CLEANSING_TOTEM:          {level: 26, spell_id: 8170}
  CHAIN_LIGHTNING:          {level: 28, spell_id: 421}
  ASTRAL_RECALL:            {level: 30, spell_id: 556}
  REINCARNATION:            {level: 30, spell_id: 20608}
  HEX:                      {level: 40, spell_id: 51514}
  WRATH_OF_AIR_TOTEM:       {level: 42, spell_id: 3738}
  FIRE_ELEMENTAL_TOTEM:     {level: 56, spell_id: 2894}
  EARTH_ELEMENTAL_TOTEM:    {level: 56, spell_id: 2062}
  THUNDERSTORM:             {level: 60, spell_id: 51490, requires_spec: elemental}
  EARTHQUAKE:               {level: 60, spell_id: 61882, requires_talent: true}
  BLOODLUST:                {level: 70, spell_id: 2825}
  HEROISM:                  {level: 70, spell_id: 32182}
  SPIRITWALKERS_GRACE:      {level: 85, spell_id: 79206}
  # Talent-gated (Elemental):
  LAVA_BURST:               {requires_talent: true, spell_id: 51505}
  ELEMENTAL_MASTERY:        {requires_talent: true, spell_id: 16166}
  FULMINATION:              {requires_talent: true}
  LAVA_SURGE:               {requires_talent: true}
  ELEMENTAL_FOCUS:          {requires_talent: true}
  CONVECTION:               {requires_talent: true}
  CONCUSSION:               {requires_talent: true}
  CALL_OF_FLAME:             {requires_talent: true}
  ELEMENTAL_PRECISION:      {requires_talent: true}
  ELEMENTAL_OATH:           {requires_talent: true}
  STORM_EARTH_AND_FIRE:     {requires_talent: true}
  TOTEMIC_WRATH:            {requires_talent: true}
  ROLLING_THUNDER:          {requires_talent: true}
  REVERBERATION:            {requires_talent: true}
  ELEMENTAL_REACH:          {requires_talent: true}
  IMPROVED_FIRE_NOVA:       {requires_talent: true}
  # Mastery / passives:
  MASTERY_ELEMENTAL_OVERLOAD: {passive_mastery: true, base_pct: 16, per_point_pct: 2}
  LIGHTNING_SHIELD_CHARGES:   {passive_resource: true, base_max: 3, fulmination_max: 9}
```

**Shaman is mana-only.** Mana cascade: Thunderstorm 8% on 45s CD → potion → Mana Spring Totem passive → group's Replenishment / Mana Tide (Resto Shaman if present).

**Pre-Lava-Burst** (under L34): LB + FS + ES rotation. Below L28 (no Chain Lightning), 2-spell LB/FS rotation with ES on shock-CD.

**Pre-Fulmination** (under ~L40): ES used on shock CD as direct damage. Once Fulmination talented, ES gated by ≥7 LS charges.

---

## 7. Suggested next research passes

1. Lava Burst exact cast (1.5s vs 2.0s) under 5/5 talent reduction.
2. Flame Shock duration with/without Glyph (24s vs 27s vs 30s).
3. Earth Shock Fulmination damage formula (per-charge multiplier).
4. Lightning Shield charge generation rate (LB 100% / CL per-hit / Rolling Thunder talent).
5. Lava Surge proc rate per Flame Shock tick.
6. Elemental Mastery duration + CD reduction talents.
7. Earthquake damage formula (caster SP coefficient).
8. Mastery Elemental Overload base + per-point scaling.
9. Haste soft caps for FS 7-tick + LB cast time.
10. T13 set bonuses.
11. Glyph of Flame Shock duration extension exact value.
12. Glyph of Lightning Bolt damage % + range bonus.
13. Glyph of Lava Burst spell power coefficient.
14. Glyph of Fire Elemental Totem duration extension.
15. Wrath of Air +5% spell haste exact aura.
16. Spiritwalker's Grace duration / CD with talents.
17. Volcanic Potion +1200 Int duration.

---

## 8. DBC verification

- LB / 2.5s. Spell.dbc 403.
- LvB / 2s / 8s CD. Spell.dbc 51505.
- ES instant / 6s shock-CD. Spell.dbc 8042.
- FS instant / 18-24s DoT. Spell.dbc 8050.
- Frost Shock instant / slow. Spell.dbc 8056.
- CL 2s / 3-target / 6s CD. Spell.dbc 421.
- Thunderstorm 45s CD. Spell.dbc 51490.
- Earthquake 2.5s / 10s CD / 10s channel. Spell.dbc 61882.
- Lightning Shield buff / 3 base / 9 max. Spell.dbc 324.
- Elemental Mastery 3min CD / instant / +20% haste 30s. Spell.dbc 16166.
- Fire Elemental Totem 5min / 2min. Spell.dbc 2894.
- Earth Elemental Totem 10min / 2min. Spell.dbc 2062.
- Bloodlust/Heroism 5min / 30% / 40s. Spell.dbc 2825 / 32182.
- Wind Shear 6s CD / 2s lockout. Spell.dbc 57994.
- Hex 45s / 1.5s / 1min PvE. Spell.dbc 51514.
- Purge instant / 1 magic. Spell.dbc 370.
- Searing Totem 1min / Fire shots. Spell.dbc 3599.
- Magma Totem ~21s / AoE pulse. Spell.dbc 8190.
- Wrath of Air +5% spell haste. Spell.dbc 3738.
- Mana Spring 5675 / Stoneskin 8071 / SoE 8075 / Tremor 8143 / Cleansing 8170.
- Spiritwalker's Grace 2min / 15s. Spell.dbc 79206.
- Reincarnation 30min. Spell.dbc 20608.
- Mastery Elemental Overload base + per-point. Mastery DBC.
- Fulmination talent. Talent.dbc.
- Lava Surge talent proc rate. Talent.dbc.
- Rolling Thunder (LS gen). Talent.dbc.
- Reverberation (shock CD reduction). Talent.dbc.
- Elemental Precision (Spirit→Hit). Talent.dbc.
- Elemental Focus (Clearcasting). Talent.dbc.
- Storm Earth and Fire / Totemic Wrath / Improved Fire Nova. Talent.dbc.
- Glyphs (LB, LvB, FS, Stoneclaw, Thunderstorm, Fire Elemental). Glyph.dbc.
- T13 2P/4P. Item set DBC.
- Volcanic Potion +1200 Int. Item DBC.

---

## 9. Notes on prompt deviations

1. **Earth Shock primary purpose** — clarified: in 4.3.4 Fulmination dominates; ES is gated by LS charges, not used as damage filler.
2. **Lava Lash / Stormstrike in prompt** — Enhancement-only; excluded from Elemental.
3. **Thunderstorm L60+** — corrected to **L60 + Elemental spec required**.
4. **Earthquake L60+** — corrected to **L60 + Elemental talent (31-pt)**.
5. **Lava Burst L34** — corrected to **talent-gated**, effective ~L34.
6. **Mana Tide Totem** — Resto-only in 4.3.4; excluded from Elemental.
7. **Mastery Elemental Overload at 75% damage** — confirmed.
8. **Glyph layout** — Prime LB/LvB/FS confirmed; Major Stoneclaw + Thunderstorm + Fire Elemental Totem default.
9. **Lava Surge proc** — Cata-key Elemental mechanic.
10. **Spiritwalker's Grace** — Cata-new movement-DPS CD.
11. **Rolling Thunder / Fulmination** — 4.3.4 shifted ES to charge-dump.
12. **Totemic Wrath** — replaces Flametongue in Cata; +10% SP raid aura.
13. **No spec-selection reconciliation needed** — Elemental is the only ranged-caster Shaman spec.

End of survey.

# Combat Rogue 4.3.4 — Multi-Guide Strategy Survey

Audit trail for `docs/specs/combat-rogue.md`. Survey conducted 2026-05-02. Strategy/threshold content reconciled across the strategy-bearing guides below. This is the **first melee-DPS survey in the project** (after 5 healers + 6 ranged DPS specs). Rogue has 3 viable PVE specs (Assassination / Combat / Subtlety) — full spec-selection reconciliation in §1 before single-spec content.

---

## 1. Spec selection — Combat (vs Assassination, Subtlety)

| Source | Assassination | Combat | Subtlety |
|---|---|---|---|
| Icy Veins Cata Tier List (DS 4.3) | **S-tier** ST | **A-tier** multi-target | A-tier raid utility |
| wowtbc.gg Phase 4 (top-10% WCL) | rank #1–3 ST | rank #2–4 cleave | rank #6–10 |
| Wowhead Cata Tier 13 | S-tier ST | A/S-tier cleave | A-tier |
| Warcraft Tavern DS rankings | top ST | top cleave | viable; gimmick on Spine |

**Spec disposition: Combat.**
1. **Kick interrupt** — 10s CD, off-GCD; **shortest melee interrupt in 4.3.4**. Bot value huge for 5-man / heroic.
2. **Blade Flurry cleave** — toggle, 2nd target hit for 35% MH weapon damage; uniquely strong in 5-mans.
3. **Weapon-flexible** — accepts swords/maces/axes/fist (no daggers required).
4. **Killing Spree + Adrenaline Rush** — sustained 15s burst window; mechanical priority easy to encode.

Assassination (Mut/Envenom) and Subtlety (Hemo/Backstab) are out of scope for v1. Future sibling spec docs.

---

## 2. Sources

| # | Guide | URL | Confidence |
|---|---|---|---|
| G1 | Icy Veins Combat Rotation/CDs | https://www.icy-veins.com/cataclysm-classic/combat-rogue-pve-rotation-cooldowns-abilities | High |
| G2 | Icy Veins Combat Talents/Glyphs | https://www.icy-veins.com/cataclysm-classic/combat-rogue-pve-spec-builds-talents-glyphs | High |
| G3 | Icy Veins Combat Stat Priority | https://www.icy-veins.com/cataclysm-classic/combat-rogue-pve-stat-priority | High |
| G4 | Wowhead Cata Combat | https://www.wowhead.com/cata/guide/classes/rogue/combat/dps-overview-pve | Medium-High |
| G5 | wowtbc.gg Combat | https://wowtbc.gg/cata/class-guides/combat-rogue/ | Medium |
| G6 | Warcraft Tavern Combat | https://www.warcrafttavern.com/cataclysm/guides/pve-combat-rogue/ | Medium-High |
| G7 | Method Cata Combat | https://www.method.gg/guides/cataclysm/combat-rogue | Medium |
| G8 | Noxxic Combat 4.3 | https://www.noxxic.com/wow/pve/rogue/combat/ | Medium |
| G15 | Warcraft Wiki per-spell | https://warcraft.wiki.gg/wiki/<spell> | High |
| G16 | Wowhead Cata per-spell | https://www.wowhead.com/cata/spell=<id> | High |

---

## 3. Strong consensus

| Item | Consensus | Spec disposition |
|---|---|---|
| **Combat A-tier in DS 4.3 cleave / B+ ST.** | 5/5 | Documented. |
| **Class identity:** Energy (100 max, 10/sec base) + target-bound Combo Points (max 5 per target). | 6/6 | Encoded. |
| **Sinister Strike** — 40 Energy, 1 CP. | 6/6 | Default builder. |
| **Revealing Strike** — 40 Energy, 1 CP, 15s debuff +35% next finisher damage. | 6/6 | Maintain pre-finisher. |
| **Eviscerate** — 35 Energy, ST finisher. | 6/6 | Default at 5 CP. |
| **Slice and Dice** — 25 Energy, finisher, +40% melee AS. **Maintain 100% uptime.** | 6/6 | Always-on. |
| **Rupture** — finisher, bleed. Skip in Combat (Eviscerate dominates ST). | 4/6 | Skipped. |
| **Killing Spree** — 2min CD, off-GCD, 5 hits teleport. | 6/6 | On CD. |
| **Adrenaline Rush** — 3min CD, off-GCD, +Energy regen + AS 15s. | 6/6 | On CD. |
| **Blade Flurry** — toggle, 35% OH cleave to 2nd target, -20% Energy regen. | 6/6 | ON 2+, OFF 1. |
| **Bandit's Guile** — Combat passive: SS/RvS stack Insight (Shallow→Moderate→Deep), +10/20/30% damage. | 6/6 | Passive — track. |
| **Restless Blades** — Combat passive: CP spent reduces AR/KS/Redirect/Sprint CD by 2s/CP. | 6/6 | Mechanical. |
| **Mastery: Main Gauche** — chance for OH strike on auto-attack; Combat-specific. | 6/6 | Reforge target. |
| **Kick** — 10s CD, off-GCD, 5s lockout. | 6/6 | Auto-interrupt. |
| **Sprint** — 1min CD, +70% MS 8s. | 6/6 | Movement CD. |
| **Vanish** — 2min CD (talented to ~1.5min). | 6/6 | Wipe-recovery. |
| **Cloak of Shadows** — 1min CD, 5s magic immunity + dispel. | 6/6 | Defensive. |
| **Evasion** — 2min CD, 50% dodge 15s. | 6/6 | Defensive. |
| **Combat Readiness** — 2min CD, stacking parry/dodge. | 5/6 | Defensive. |
| **Feint** — 20 Energy, -50% AoE damage + threat. | 6/6 | AoE survival. |
| **Tricks of the Trade** — 30s CD, +15% damage + threat redirect. | 6/6 | Tank pre-pull + on CD. |
| **Stealth** — OoC permanent. | 6/6 | Pre-pull. |
| **Garrote / Cheap Shot** — Stealth openers. | 5/6 | Manual. |
| **Ambush** — Stealth-only; weak in Combat (no dagger). | 4/6 | Skip. |
| **Kidney Shot** — finisher, stun. | 6/6 | Manual / interrupt fallback. |
| **Sap / Blind / Gouge / Smoke Bomb** — utility CC. | 4/6 | Manual. |
| **Redirect** — 1min CD (talented), transfers CPs. | 5/6 | Target swap. |
| **Recuperate** — finisher self-HoT. Solo/leveling only. | 5/6 | Manual. |
| **Expose Armor** — finisher, -12% armor. Skip if Sunder/FF in group. | 5/6 | Composition-conditional. |
| **Fan of Knives** — 35 Energy, AoE 8y. | 6/6 | AoE builder. |
| **Crimson Tempest** — finisher, AoE bleed. | 5/6 | 5+ targets. |
| **Poisons** — MH Instant + OH Deadly default. | 6/6 | Default. |
| **Hit cap** — 8% / 961 melee + 17% / 1742 spell-hit (poisons). | 6/6 | Hard caps. |
| **Expertise cap** — 26 / ~781 rating. | 6/6 | Hard cap. |
| **Stat priority** — Agility > Hit (961) > Expertise (26) > Mastery > Haste > Crit. | 5/6 | Encoded. |
| **Talent build — 31 Combat / 5 Sub / 5 Assn.** | 4/6 | Default split. |
| **Glyphs — Prime: Sinister Strike + Revealing Strike + Adrenaline Rush.** | 4/5 | Default. |
| **Glyphs — Major: Killing Spree + Blade Flurry + Feint.** | 4/5 | Default. |
| **Glyphs — Minor: Safe Fall + Blurred Speed + Poisons.** | 3/3 | Default. |
| **Pre-pot Tolvir Potion** — +1200 Agility 25s. T-2s + 1-min mark. | 4/4 | Encoded. |
| **Opener:** pre-pot T-2s → Stealth → Garrote (caster) or open from front → SnD → AR + KS → SS build → RvS → Eviscerate at 5 CP. | 4/4 | Encoded. |
| **Tricks of the Trade on tank pre-pull** + on CD. | 5/5 | Encoded. |

---

## 4. Disagreements

### 4.1. Talent split — 5/0/31 vs 3/2/31 vs 31/5/5

**Spec disposition: 31 Combat / 5 Subtlety (Nightstalker + Elusiveness) / 5 Assassination (Improved Recuperate + Lethality).**

### 4.2. Stat priority Mastery vs Haste

**Spec disposition: Agi > Hit-cap > Expertise-cap > Mastery > Haste > Crit.** Mastery wins for Combat (Main Gauche scales with OH AP).

### 4.3. Eviscerate vs Rupture

**Spec disposition: skip Rupture in default Combat priority.** Eviscerate dominates ST.

### 4.4. SnD refresh threshold

**Spec disposition: refresh ≤3s remaining at 4-5 CP (or 1 CP if SnD <3s and no other CPs).**

### 4.5. AR + KS sync

**Spec disposition: pair AR + KS on first pull.** Subsequent uses on independent CDs.

### 4.6. KS during Blade Flurry

**Spec disposition: leave Blade Flurry ON during KS when 2+ targets.** Cleave value > ST clarity.

### 4.7. RvS refresh

**Spec disposition: refresh RvS when ≤3s remaining AND about to finisher.**

### 4.8. Blade Flurry threshold

**Spec disposition: BF ON at 2+ targets within 8y AND both living ≥10s.**

### 4.9. AoE rotation

**Spec disposition: 4+ targets → Fan of Knives builder + Crimson Tempest finisher. 2-3 targets → Sinister Strike + BF + Eviscerate.**

### 4.10. Tricks of the Trade target

**Spec disposition: tank pre-pull (T-6s); top-DPS on CD if tank threat safe.**

### 4.11. Energy pooling

**Spec disposition: no pooling default.** Pool only if AR ≤3s from press.

### 4.12. Glyph third Prime — SS vs Eviscerate

**Spec disposition: Glyph of Sinister Strike** (+20% extra-CP chance).

### 4.13. Recuperate

**Spec disposition: skip in raid; manual command for solo/leveling.**

### 4.14. Expose Armor

**Spec disposition: only if no Warrior/DK/Druid armor-debuff source in group.** Default OFF.

---

## 5. Items NOT addressed (author judgment)

| Spec item | Value | Why |
|---|---|---|
| SnD maintenance | refresh if <3s remaining | Always-on. |
| Pre-pull stealth | ON 30s pre-pull | Free Garrote/Cheap Shot. |
| Tricks of the Trade on pull | T-6s tank | Threat protection. |
| AR usage | on CD, sync first cast with KS | Pull burst. |
| KS usage | on CD when 2+ targets in 10y | Cleave utilization. |
| Blade Flurry toggle | ON 2+, OFF 1 | Energy tradeoff. |
| Bandit's Guile depth | finish in Deep window if possible | Soft priority. |
| RvS | refresh ≤3s + finisher imminent | Standard. |
| Eviscerate at 5 CP | always | Max value. |
| 4-CP Eviscerate | only if SnD/RvS expiring this GCD | Edge case. |
| Energy pool | no pooling default | Avoid cap. |
| Energy cap (≥95) | SS or finisher mandatory | Avoid waste. |
| Kick auto-interrupt | enemy cast ≤300ms before completion + melee range | Reactive. |
| Vanish | manual / wipe | Manual. |
| Cloak | reactive on magic damage spike | Defensive. |
| Evasion | reactive on physical spike | Defensive. |
| Combat Readiness | reactive on physical-heavy fight | Defensive. |
| Feint | press during AoE-damage AND Energy ≥60 + SnD safe | AoE survival. |
| Sprint | movement requirement | Movement. |
| Garrote opener | vs casters + silence talent | Stealth-aware. |
| Cheap Shot opener | vs melee trash | Stealth-aware. |
| Sap/Blind/Kidney Shot/Gouge | manual | CC/utility. |
| Smoke Bomb | manual (talent-gated, Sub) | Niche. |
| Redirect | target swap if 3+ CP on dying target | Encoded. |
| Recuperate | manual / solo | Manual. |
| Expose Armor | composition-conditional | Group check. |
| Crimson Tempest | 5+ targets AoE finisher | AoE. |
| Fan of Knives | 4+ targets AoE builder | AoE. |
| Poison setup | MH Instant + OH Deadly default | Per-encounter override. |
| Pre-pot timing | T-2s + 1-min mark | Bot-side. |
| Stat reforge | Crit → Mastery; Haste keep | Standard. |
| Hit cap | 961 melee + 1742 poison-hit | Hard. |
| Expertise cap | 26 / ~781 | Hard. |

---

## 6. Author-judgment items — leveling

```yaml
spell_unlocks:
  STEALTH:                 {level: 1,  spell_id: 1784}
  SINISTER_STRIKE:         {level: 1,  spell_id: 1752}
  EVISCERATE:              {level: 4,  spell_id: 2098}
  GOUGE:                   {level: 6,  spell_id: 1776}
  SPRINT:                  {level: 7,  spell_id: 2983}
  BACKSTAB:                {level: 8,  spell_id: 53}
  PICK_POCKET:             {level: 10, spell_id: 921}
  KICK:                    {level: 12, spell_id: 1766}
  SAP:                     {level: 14, spell_id: 6770}
  SLICE_AND_DICE:          {level: 14, spell_id: 5171}
  RECUPERATE:              {level: 14, spell_id: 73651}
  AMBUSH:                  {level: 16, spell_id: 8676}
  VANISH:                  {level: 18, spell_id: 1856}
  CHEAP_SHOT:              {level: 18, spell_id: 1833}
  EVASION:                 {level: 20, spell_id: 5277}
  RUPTURE:                 {level: 20, spell_id: 1943}
  POISONS:                 {level: 20, spell_id: 2842}
  GARROTE:                 {level: 22, spell_id: 703}
  DISTRACT:                {level: 22, spell_id: 1725}
  TRICKS_OF_THE_TRADE:     {level: 24, spell_id: 57934}
  PICK_LOCK:               {level: 24, spell_id: 1804}
  BLIND:                   {level: 26, spell_id: 2094}
  KIDNEY_SHOT:             {level: 30, spell_id: 408}
  EXPOSE_ARMOR:            {level: 32, spell_id: 8647}
  FEINT:                   {level: 32, spell_id: 1966}
  SHIV:                    {level: 36, spell_id: 5938}
  ADRENALINE_RUSH:         {level: 36, spell_id: 13750, requires_spec: combat}
  FAN_OF_KNIVES:           {level: 36, spell_id: 51723}
  BLADE_FLURRY:            {level: 40, spell_id: 13877, requires_spec: combat}
  REDIRECT:                {level: 50, spell_id: 73981}
  REVEALING_STRIKE:        {level: 50, spell_id: 84617, requires_spec: combat}
  CLOAK_OF_SHADOWS:        {level: 56, spell_id: 31224}
  CRIMSON_TEMPEST:         {level: 64, spell_id: 121411}
  COMBAT_READINESS:        {level: 78, spell_id: 74001}
  KILLING_SPREE:           {level: 81, spell_id: 51690, requires_spec: combat}
  SMOKE_BOMB:              {level: 78, spell_id: 76577, requires_talent: true}
  # Talent-gated (Combat):
  IMPROVED_SINISTER_STRIKE: {requires_talent: true}
  PRECISION:               {requires_talent: true}
  IMPROVED_SLICE_AND_DICE: {requires_talent: true}
  IMPROVED_KICK:           {requires_talent: true}
  COMBAT_POTENCY:          {requires_talent: true}
  LIGHTNING_REFLEXES:      {requires_talent: true}
  ARMORED_TO_THE_TEETH:    {requires_talent: true}
  RESTLESS_BLADES:         {requires_talent: true}
  BANDITS_GUILE:           {requires_talent: true}
  SAVAGE_COMBAT:           {requires_talent: true}
  VITALITY:                {requires_talent: true}
  AGGRESSION:              {requires_talent: true}
  IMPROVED_EVISCERATE:     {requires_talent: true}
  # Subtlety bonus:
  NIGHTSTALKER:            {requires_talent: true}
  ELUSIVENESS:             {requires_talent: true}
  # Assassination bonus:
  IMPROVED_RECUPERATE:     {requires_talent: true}
  LETHALITY:               {requires_talent: true}
  # Mastery / passives:
  MASTERY_MAIN_GAUCHE:     {passive_mastery: true, base_pct: 16, per_point_pct: 2}
  COMBO_POINTS:            {passive_resource: true, max: 5, target_bound: true}
  ENERGY:                  {passive_resource: true, max: 100, regen: 10_per_sec}
```

**Rogue is energy-only** — no mana. Energy regen base 10/sec, accelerated by Adrenaline Rush.

**Pre-SnD (under L14):** SS → Eviscerate at 4-5 CP, no SnD.
**Pre-AR/KS/BF (under L36/40/81):** Combat plays as basic SS + RvS + Eviscerate + SnD priority. Bandit's Guile (talent-gated) carries the rotation. Below L50 (no RvS), RvS skipped. Below L40 (no BF), single-target only.

---

## 7. Suggested next research passes

1. SS Energy reductions (Improved Sinister Strike).
2. RvS +35% finisher damage exact aura.
3. SnD +40% AS exact value.
4. Eviscerate damage formula per CP.
5. KS damage per hit + teleport range + total hits.
6. AR Energy regen multiplier (+100% vs +200%).
7. BF OH 35% scaling vs Mastery.
8. Bandit's Guile Insight values.
9. Restless Blades CP→CD reduction (2s/CP confirm).
10. Mastery Main Gauche base + per-point.
11. Kick CD vs Improved Kick.
12. Vanish CD vs Elusiveness.
13. Cloak magic-immunity coverage.
14. Combat Readiness stack mechanics.
15. Tricks of the Trade +15% aura + redirect.
16. Poison proc rates per weapon speed.
17. Glyph values (SS, RvS, AR, KS, BF, Feint, Eviscerate).
18. Tolvir Potion +1200 Agility duration.
19. T13 2P/4P set bonuses (Combat).

---

## 8. DBC verification

(All abilities cross-referenced; spell IDs listed in §6.)

- SS 40 Energy / 1 CP. Spell.dbc 1752.
- RvS 40 Energy / 15s debuff +35%. Spell.dbc 84617.
- Eviscerate 35 Energy / finisher. Spell.dbc 2098.
- SnD 25 Energy / +40% AS. Spell.dbc 5171.
- KS 2min CD / 5 hits / off-GCD. Spell.dbc 51690.
- AR 3min CD / 15s / off-GCD. Spell.dbc 13750.
- BF toggle / 35% OH / -20% regen. Spell.dbc 13877.
- Kick 10s CD / off-GCD / 5s lockout. Spell.dbc 1766.
- Sprint 1min / +70% / 8s. Spell.dbc 2983.
- Vanish 2min / 3s restealth. Spell.dbc 1856.
- Cloak 1min / 5s. Spell.dbc 31224.
- Evasion 2min / 50% dodge / 15s. Spell.dbc 5277.
- Combat Readiness 2min / 10s. Spell.dbc 74001.
- Feint 20 Energy / -50% AoE / 6s. Spell.dbc 1966.
- ToT 30s / +15% / redirect. Spell.dbc 57934.
- Stealth toggle. Spell.dbc 1784.
- Garrote / Cheap Shot / Ambush — Stealth-only. Spell.dbc 703 / 1833 / 8676.
- Kidney Shot finisher / stun. Spell.dbc 408.
- Gouge front incap / 4s. Spell.dbc 1776.
- Blind 2min / 10s. Spell.dbc 2094.
- Sap Stealth / 1min CC. Spell.dbc 6770.
- Smoke Bomb 3min / 5s area. Spell.dbc 76577.
- Redirect 1min CD. Spell.dbc 73981.
- Recuperate 30 Energy finisher. Spell.dbc 73651.
- Expose Armor finisher / -12% armor / 30s. Spell.dbc 8647.
- Fan of Knives 35 Energy / 8y AoE. Spell.dbc 51723.
- Crimson Tempest finisher AoE bleed. Spell.dbc 121411.
- Shiv OH instant + poison. Spell.dbc 5938.
- Mastery Main Gauche. Mastery DBC.
- Bandit's Guile / Restless Blades / Vitality. Talent.dbc.
- Combat talents (Improved SS / Imp SnD / Imp Kick / Combat Potency / Lightning Reflexes / Aggression / Savage Combat). Talent.dbc.
- Sub bonus (Nightstalker / Elusiveness). Talent.dbc.
- Assn bonus (Improved Recuperate / Lethality). Talent.dbc.
- Glyphs (SS, RvS, AR, KS, BF, Feint, Eviscerate). Glyph.dbc.
- T13 2P/4P (Combat). Item set DBC.
- Tolvir Potion. Item DBC.
- Poisons (Instant/Wound/Crippling/Mind-numbing/Deadly). Spell.dbc + Item.dbc.

---

## 9. Notes on prompt deviations

1. **Spec selection** — full §1 across 3 specs added per project methodology.
2. **Kick "shortest interrupt"** — refined to shortest *melee* interrupt (Wind Shear shaman-only).
3. **Stat priority** — Mastery > Haste > Crit confirmed (prompt left ordering open).
4. **Combo Points target-bound** — confirmed; Redirect handles target swaps.
5. **Bandit's Guile** — SS + RvS both stack Insight.
6. **Restless Blades** — affects AR/KS/Redirect/Sprint (not Vanish/Evasion).
7. **Expose Armor** — composition-conditional; default OFF.
8. **Recuperate** — Cata-baseline finisher (L14). Solo only for raid bot.
9. **Crimson Tempest** — Cata-baseline AoE finisher (L64).
10. **Combat Readiness L78** — Cata-new defensive baseline.
11. **Smoke Bomb** — Sub talent-gated; Combat skip.
12. **KS during BF** — disposition: BF stays ON.
13. **AR + KS pairing** — first-pull synergy yes; subsequent independent.
14. **Talent split** — 5pts each Sub + Assn (overridable).
15. **Energy pooling** — none in default priority.
16. **Hit cap 961 / Expertise 26** — confirmed L85.
17. **Poison setup** — MH Instant + OH Deadly default.
18. **Pre-pot Tolvir** — +1200 Agility 25s. T-2s + 1-min mark.
19. **Tricks of the Trade pre-pull** — T-6s tank.

End of survey.

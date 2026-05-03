# Retribution Paladin 4.3.4 — Multi-Guide Strategy Survey

Audit trail for `docs/specs/retribution-paladin.md`. Survey conducted 2026-05-02. This is the **third melee-DPS survey** (after Combat Rogue and Fury Warrior). Paladin has 3 specs (Holy / Protection / Retribution); Holy is healer (already covered), Protection is tank (deferred). Ret is the only Paladin DPS spec — **no spec-selection reconciliation needed**.

Cross-reference: Holy Power mechanics, Seals, Hand-of-X utility, Blessings, and most baseline paladin abilities are already documented in `docs/specs/holy-paladin.md`. This survey covers Ret-specific: Templar's Verdict, Inquisition, Mastery: Hand of Light, Zealotry, Guardian of Ancient Kings (Ret variant), 2H weapon expectations, Seal of Truth + Censure, Hammer of Wrath in Ret, Ret-specific glyph/talent/stat priorities.

---

## 1. Spec selection

**Spec disposition: Retribution.** Only Paladin DPS spec; no comparison required. 2H melee plate DPS with Holy Power as a secondary resource layered on top of mana — physical auto-attacks + 5-stack Censure DoT + Holy Power finishers.

---

## 2. Sources

| # | Guide | URL | Confidence |
|---|---|---|---|
| G1 | Icy Veins Ret Rotation/CDs | https://www.icy-veins.com/cataclysm-classic/retribution-paladin-pve-rotation-cooldowns-abilities | High |
| G2 | Icy Veins Ret Talents/Glyphs | https://www.icy-veins.com/cataclysm-classic/retribution-paladin-pve-spec-builds-talents-glyphs | High |
| G3 | Icy Veins Ret Stat Priority | https://www.icy-veins.com/cataclysm-classic/retribution-paladin-pve-stat-priority | High |
| G4 | Wowhead Cata Ret | https://www.wowhead.com/cata/guide/classes/paladin/retribution/dps-overview-pve | Medium-High |
| G5 | wowtbc.gg Ret | https://wowtbc.gg/cata/class-guides/retribution-paladin/ | Medium |
| G6 | Warcraft Tavern Ret | https://www.warcrafttavern.com/cataclysm/guides/pve-retribution-paladin/ | Medium-High |
| G7 | Method Cata Ret | https://www.method.gg/guides/cataclysm/retribution-paladin | Medium |
| G8 | Noxxic Ret 4.3 | https://www.noxxic.com/wow/pve/paladin/retribution/ | Medium |
| G15 | Warcraft Wiki per-spell | https://warcraft.wiki.gg/wiki/<spell> | High |
| G16 | Wowhead Cata per-spell | https://www.wowhead.com/cata/spell=<id> | High |

---

## 3. Strong consensus

| Item | Consensus | Spec disposition |
|---|---|---|
| **Ret resource** — mana + Holy Power 0-3. | 6/6 | Encoded. |
| **2H weapon required** — Two-Handed Weapon Specialization. | 6/6 | Hard requirement. |
| **Plate armor** — Plate Specialization +5% Strength. | 6/6 | Encoded. |
| **Seal of Truth** — primary Ret seal; on-hit Censure 5-stack. | 6/6 | Default seal. |
| **Censure** — 5-stack DoT auto-applied by Seal of Truth; refreshes per swing at 5. | 6/6 | Maintained. |
| **Crusader Strike** — 4.5s CD instant, generates 1 HP. | 6/6 | On CD. |
| **Templar's Verdict** — HP finisher; scales with HP charges (1/2/3). | 6/6 | Spend at 3 HP. |
| **Inquisition** — self-buff +30% Holy; duration scales with HP (4s × HP, max 12s). | 6/6 | Maintain. |
| **Judgement** — instant 30y; refreshes JotP haste; 8s CD. | 6/6 | On CD. |
| **Exorcism** — 15s CD ranged Holy; instant via Art of War proc. | 6/6 | Instant only. |
| **Hammer of Wrath** — usable <20% HP OR AW active OR Zealotry active (Sanctified Wrath). | 6/6 | Conditional. |
| **Divine Storm** — 8y AoE physical HP-spender; Ret talent. | 5/6 | AoE. |
| **The Art of War** — talent, melee crit = free instant Exorcism. | 6/6 | Reactive. |
| **Avenging Wrath** — 3min CD (1.5min with Sanctified Wrath), 20s, +20% damage. | 6/6 | On CD. |
| **Zealotry** — 31-pt capstone, 2min CD, 20s, max HP for duration. | 5/6 | On CD. |
| **Guardian of Ancient Kings (Ret)** — 5min CD, 30s, summon damage CD. | 5/6 | Major DPS CD. |
| **Mastery: Hand of Light** — % Holy damage on TV/DS/CS/HotR. | 6/6 | Reforge target. |
| **Word of Glory** — defensive HP self-heal off-rotation. | 5/6 | Defensive only. |
| **Talent build — 31/7/3** (Ret + Prot + Holy). | 6/6 | Default. |
| **Hit cap** — 961 (8% melee). | 6/6 | Hard. |
| **Expertise cap** — 26 / ~781. | 6/6 | Hard. |
| **Stat priority** — Str > Hit (961) > Exp (26) > Mastery > Crit > Haste. | 5/6 | Encoded. |
| **Glyphs Prime** — Templar's Verdict + Crusader Strike + Exorcism. | 6/6 | Default. |
| **Glyphs Major** — Ascetic Crusader + Hammer of Justice + Salvation. | 3/6 | Default. |
| **Buff: Blessing of Might** — single-buff raid AP + MP5. | 6/6 | Pre-pull + maintain. |
| **Buff: Blessing of Kings** — +5% all stats (overlaps Mark of the Wild). | 6/6 | Conditional. |
| **Aura: Retribution Aura** — passive Holy damage to attackers. | 5/6 | Default. |
| **Pre-pot Golemblood** — +1200 Strength 25s. T-2s + 1-min mark. | 4/4 | Encoded. |
| **Opener** — Inq (1 HP from CS) → AW + Zealotry + GoAK → CS → TV/Inq → Exo → Judge → HoW → fillers. | 4/4 | Encoded. |
| **Rebuke** — 15s CD, 4s lockout, off-GCD interrupt. | 6/6 | Auto-interrupt. |
| **Hammer of Justice** — 1min CD, 6s stun. | 6/6 | CC. |
| **Repentance** — talent CC, 6s incapacitate. | 6/6 | Talent CC. |
| **Hand of Reckoning** — Ret-baseline taunt analog. | 4/6 | Off-tank. |
| **Hand of Freedom / Protection / Salvation** — utility. | 6/6 | Utility. |
| **Lay on Hands** — 10min CD full HP heal. | 6/6 | Emergency. |
| **Divine Shield** — 5min CD full immunity, Forbearance. | 6/6 | Last-resort. |
| **Divine Protection** — 1min CD -20% magic. | 6/6 | Personal CD. |

---

## 4. Disagreements

### 4.1. Talent split — 31/7/3 vs 31/5/5

**Spec disposition: 31/7/3.** Prot 7 = Toughness 3 + Improved Judgement 2 + Pursuit of Justice 2. Holy 3 = Communion 2 + Divinity 1.

### 4.2. Stat Mastery vs Crit vs Haste

**Spec disposition: Mastery > Crit > Haste.** Hand of Light multiplicative on TV/DS/CS.

### 4.3. Inquisition refresh

**Spec disposition: refresh when remaining < 4s AND 3 HP.** Always cast at 3 HP.

### 4.4. TV vs Inquisition spend priority

**Spec disposition: Inquisition refresh > TV.** If Inq <4s, refresh; else TV at 3 HP.

### 4.5. Exorcism instant-only

**Spec disposition: Art of War proc only.** Skip hardcast.

### 4.6. Major Glyphs

**Spec disposition: Ascetic Crusader + Hammer of Justice + Salvation.** Utility-tier; encounter-tunable.

### 4.7. Minor Glyphs

**Spec disposition: Insight + Truth + Blessings.**

### 4.8. Seal swap

**Spec disposition: 1-3 enemies = Truth; 4+ = Righteousness.** Swap on threshold change.

### 4.9. Hammer of Wrath outside execute

**Spec disposition: AW OR Zealotry active OR <20% HP.** Highest priority when usable (rare instant ranged spell).

### 4.10. ST rotation order

**Spec disposition:**
1. Inquisition refresh (<4s + 3 HP)
2. Templar's Verdict (3 HP + Inq up)
3. Crusader Strike (on CD)
4. Hammer of Wrath (when usable)
5. Exorcism (Art of War proc only)
6. Judgement (on CD)
7. Holy Wrath (filler)
8. Consecration (AoE only)

### 4.11. Divine Storm vs TV at AoE

**Spec disposition: 4+ enemies → DS replaces TV; 2-3 → TV.**

### 4.12. Consecration usage

**Spec disposition: AoE 4+ enemies AND mana > 50%.**

### 4.13. Holy Wrath as filler

**Spec disposition: cast on CD when no higher priority AND AoE-applicable enemy types in front.**

### 4.14. Word of Glory DPS use

**Spec disposition: heal-only (defensive).** Self HP < 50% AND 3 HP AND Inq > 8s.

### 4.15. Pre-pot + combat-pot

**Spec disposition: Golemblood T-2s + Golemblood at 1-min mark when AW + Zealotry + GoAK align.**

### 4.16. AW / Zealotry / GoAK alignment

**Spec disposition: stack all three on first pull; drift after.**

### 4.17. Sanctified Wrath required

**Spec disposition: YES.** Ret talent, halves AW CD + enables HoW during AW.

### 4.18. Communion

**Spec disposition: take in Holy 3-pt budget.** Raid +2% dmg/heal + 1% mana/min.

---

## 5. Items NOT addressed (author judgment)

| Spec item | Value | Why |
|---|---|---|
| Seal of Truth maintenance | always; cast on log-in / after Forbearance / Divine Shield drop | Always-on. |
| Censure stack mgmt | passive | Auto. |
| Inquisition maintenance | refresh <4s + 3 HP | Always-on. |
| Crusader Strike | on CD (4.5s) | Default. |
| Judgement | on CD (8s); maintains JotP | Default. |
| Templar's Verdict | 3 HP AND Inq > 4s | Default. |
| Divine Storm | 4+ enemies AND 3 HP AND Inq > 4s | AoE. |
| Hammer of Wrath | <20% HP OR AW OR Zealotry active | Conditional. |
| Exorcism | Art of War proc only | Instant. |
| Holy Wrath | filler when no higher priority AND mana > 30% | Filler. |
| Consecration | AoE 4+ AND mana > 50% | Mana-gated. |
| Avenging Wrath | on CD (1.5min Sanctified Wrath) | CD. |
| Zealotry | on CD (2min); align first w/ AW | CD. |
| Guardian of Ancient Kings (Ret) | on CD (5min); align first w/ AW | CD. |
| Word of Glory | self HP < 50% AND 3 HP AND Inq > 8s | Defensive. |
| Lay on Hands | self HP < 15% (10min CD) | Emergency. |
| Divine Shield | self HP < 10% AND no other CD | Last-resort. |
| Divine Protection | self taking magic + HP < 60% | Magic CD. |
| Hand of Freedom | snared/rooted | Utility. |
| Hand of Protection | physical-damage spike + ally HP < 25% | Utility. |
| Hand of Salvation | high-threat ally w/o other redirect | Utility. |
| Hand of Reckoning | tank dropped / off-tank pickup | Utility. |
| Repentance | CC humanoid/dragonkin/giant/undead/demon | CC. |
| Hammer of Justice | interrupt-stun / add-stun | CC. |
| Rebuke | enemy cast ≤300ms before completion + melee range | Reactive. |
| Blessing of Might | pre-pull + maintain (5min) | Buff. |
| Blessing of Kings | conditional MotW check | Buff. |
| Retribution Aura | default | Aura. |
| Pre-pot Golemblood | T-2s + 1-min mark | Bot-side. |
| Stat reforge | Crit/Haste → Mastery; Hit/Exp keep | Standard. |

---

## 6. Author-judgment items — leveling

```yaml
spell_unlocks:
  CRUSADER_STRIKE:           {level: 1,  spell_id: 35395}
  HAMMER_OF_JUSTICE:         {level: 1,  spell_id: 853}
  RIGHTEOUS_FURY:            {level: 1,  spell_id: 25780}
  SEAL_OF_RIGHTEOUSNESS:     {level: 3,  spell_id: 20154}
  JUDGEMENT:                 {level: 4,  spell_id: 20271}
  DEVOTION_AURA:             {level: 8,  spell_id: 465}
  WORD_OF_GLORY:             {level: 9,  spell_id: 85673}
  REDEMPTION:                {level: 12, spell_id: 7328}
  DIVINE_PROTECTION:         {level: 12, spell_id: 498}
  HAND_OF_RECKONING:         {level: 14, spell_id: 62124}
  LAY_ON_HANDS:              {level: 16, spell_id: 633}
  PURIFY:                    {level: 18, spell_id: 1152}
  DIVINE_SHIELD:             {level: 18, spell_id: 642}
  BLESSING_OF_KINGS:         {level: 20, spell_id: 20217}
  HAND_OF_FREEDOM:           {level: 22, spell_id: 1044}
  HAND_OF_SALVATION:         {level: 22, spell_id: 1038}
  CONSECRATION:              {level: 24, spell_id: 26573}
  SEAL_OF_INSIGHT:           {level: 26, spell_id: 20165}
  HAND_OF_PROTECTION:        {level: 28, spell_id: 1022}
  RETRIBUTION_AURA:          {level: 30, spell_id: 7294}
  AVENGING_WRATH:            {level: 30, spell_id: 31884, requires_spec: ret}
  TEMPLARS_VERDICT:          {level: 30, spell_id: 85256, requires_spec: ret}
  INQUISITION:               {level: 30, spell_id: 84963, requires_spec: ret}
  RESISTANCE_AURA:           {level: 30, spell_id: 19891}
  SEAL_OF_TRUTH:             {level: 30, spell_id: 31801, requires_spec: ret}
  DIVINE_STORM:              {level: 30, spell_id: 53385, requires_spec: ret, requires_talent: true}
  ZEALOTRY:                  {level: 30, spell_id: 85696, requires_spec: ret, requires_talent: true}
  EXORCISM:                  {level: 32, spell_id: 879}
  HAMMER_OF_WRATH:           {level: 36, spell_id: 24275}
  BLESSING_OF_MIGHT:         {level: 40, spell_id: 19740}
  CONCENTRATION_AURA:        {level: 50, spell_id: 19746}
  HOLY_WRATH:                {level: 50, spell_id: 2812}
  REBUKE:                    {level: 54, spell_id: 96231}
  CRUSADER_AURA:             {level: 62, spell_id: 32223}
  GUARDIAN_OF_ANCIENT_KINGS_RET: {level: 85, spell_id: 86698, requires_spec: ret}
  REPENTANCE:                {requires_talent: true, spell_id: 20066}
  # Talent-gated (Ret):
  THE_ART_OF_WAR:            {requires_talent: true}
  SANCTIFIED_WRATH:          {requires_talent: true}
  TWO_HANDED_SPECIALIZATION: {requires_talent: true}
  RULE_OF_LAW:               {requires_talent: true}
  CRUSADE:                   {requires_talent: true}
  PARAGON_OF_VIRTUE:         {requires_talent: true}
  IMPROVED_JUDGEMENT:        {requires_talent: true}
  PURSUIT_OF_JUSTICE:        {requires_talent: true}
  TOUGHNESS:                 {requires_talent: true}
  COMMUNION:                 {requires_talent: true}
  DIVINITY:                  {requires_talent: true}
  # Mastery / passives:
  MASTERY_HAND_OF_LIGHT:     {passive_mastery: true, base_pct: 16.8, per_point_pct: 2.1}
  PLATE_SPECIALIZATION:      {passive: true, requires_all_plate: true, str_pct: 5}
  HOLY_POWER:                {passive_resource: true, max: 3}
```

**Paladin is mana + Holy Power dual-resource.** Mana auto-regens; HP generated by:
- Crusader Strike (1 HP)
- Hammer of Wrath crit + Sacred Duty / Tower of Light
- Zealotry: each generator grants 3 HP for 20s

**Pre-Templar's Verdict / Inquisition (under L30):** auto-attack + CS + Seal of Righteousness; no HP-spender.
**Pre-Avenging Wrath (under L30):** no DPS CD.
**Pre-Hammer of Wrath (under L36):** no execute.
**Pre-Zealotry (under L30 capstone):** burst window = AW only.
**Pre-Sanctified Wrath:** HoW only at <20% HP.
**Pre-Mastery (under L80):** Hand of Light scaling kicks in at L80+.

---

## 7. Suggested next research passes

1. Holy Power generation on Communion (HP on Judgement?).
2. Templar's Verdict damage formula per HP-stack.
3. Inquisition exact duration formula (4s × HP).
4. Sanctified Wrath AW-CD reduction (50%) AND HoW-during-AW.
5. Hammer of Wrath base damage + AP/SP scaling.
6. Censure tick rate + 5-stack damage formula.
7. Seal of Truth on-hit damage formula.
8. Mastery Hand of Light formula.
9. Two-Handed Specialization +20% damage.
10. Plate Specialization +5% Strength.
11. Art of War proc rate + 15s window.
12. Zealotry HP-generation effect.
13. Guardian of Ancient Kings Ret stacks formula.
14. Avenging Wrath +20% damage scope.
15. Divine Storm radius (8y) + target cap.
16. Exorcism cast time (1.5s vs 2.0s) + Holy school.
17. Holy Wrath AoE radius + target cap.
18. Consecration mana / radius / duration.
19. T13 2P/4P (Ret).
20. Judgements of the Pure haste % + 60s.
21. Glyph of TV (+15%).
22. Glyph of CS (+5% crit).
23. Glyph of Exorcism.
24. Hand of Reckoning taunt-status mechanics.
25. Repentance break-on-damage + duration on PvE bosses.
26. Forbearance shared CD list (LoH / Divine Shield / HoP).
27. Golemblood Potion duration.
28. Word of Glory + Eternal Glory glyph.

---

## 8. DBC verification

(Spell IDs in §6.)

- Crusader Strike 4.5s / 1 HP. Spell.dbc 35395.
- Templar's Verdict HP-spender. Spell.dbc 85256.
- Inquisition self-buff. Spell.dbc 84963.
- Judgement 8s / 30y. Spell.dbc 20271.
- Exorcism 15s / Holy / instant on AoW. Spell.dbc 879.
- Hammer of Wrath 6s / ranged / <20% HP gate. Spell.dbc 24275.
- Divine Storm 4.5s / 8y AoE. Spell.dbc 53385.
- Avenging Wrath 3min / 20s / +20%. Spell.dbc 31884.
- Zealotry 2min / 20s / capstone. Spell.dbc 85696.
- Guardian of Ancient Kings Ret 5min / 30s. Spell.dbc 86698.
- Seal of Truth + Censure. Spell.dbc 31801.
- Word of Glory HP-spender heal. Spell.dbc 85673.
- Lay on Hands 10min / full HP. Spell.dbc 633.
- Divine Shield 5min / 8s / Forbearance. Spell.dbc 642.
- Divine Protection 1min / 10s / -20% magic. Spell.dbc 498.
- Hand of Freedom 25s. Spell.dbc 1044.
- Hand of Protection 5min / Forbearance. Spell.dbc 1022.
- Hand of Salvation 2min / -20% threat. Spell.dbc 1038.
- Hand of Reckoning 8s / ranged threat. Spell.dbc 62124.
- Hammer of Justice 1min / 6s stun. Spell.dbc 853.
- Rebuke 15s / off-GCD / 4s lockout. Spell.dbc 96231.
- Repentance 1min / 6s incapacitate / talent. Spell.dbc 20066.
- Blessing of Might 5min. Spell.dbc 19740.
- Blessing of Kings 5min. Spell.dbc 20217.
- Retribution Aura passive. Spell.dbc 7294.
- Holy Wrath 15s / AoE / Holy. Spell.dbc 2812.
- Consecration mana / 8s ground AoE. Spell.dbc 26573.
- Two-Handed Specialization talent. Talent.dbc.
- Sanctified Wrath talent. Talent.dbc.
- The Art of War talent. Talent.dbc.
- Communion talent. Talent.dbc.
- Mastery Hand of Light. Mastery DBC.
- Glyphs (TV, CS, Exo, Ascetic Crusader, Hammer of Justice, Salvation, Insight, Truth, Blessings). Glyph.dbc.
- T13 2P/4P (Ret). Item set DBC.
- Golemblood Potion. Item DBC.

---

## 9. Notes on prompt deviations

1. **No spec-selection** — Ret is the only Paladin DPS spec.
2. **2H weapon hard requirement** — Two-Handed Specialization talent.
3. **Plate Specialization +5% Strength** — class passive.
4. **Holy Power max 3 charges** — Cata-new.
5. **Templar's Verdict HP-stack scaling** — 1/2/3 charge multipliers.
6. **Inquisition duration scales with HP** (4/8/12s).
7. **Exorcism instant-only** — Art of War proc.
8. **Hammer of Wrath conditions** — <20% HP OR AW OR Zealotry.
9. **Sanctified Wrath** — Ret-tree talent; halves AW CD + enables HoW during AW.
10. **Communion** — Holy 3-pt; raid +2% dmg/heal + mana-regen.
11. **Censure** — DoT auto-applied by Seal of Truth.
12. **Mastery Hand of Light** — multiplicative on TV/DS/CS/HotR.
13. **AW +20%** — affects all damage sources.
14. **Zealotry effect** — UNVERIFIED whether grants 3 HP per generator OR sets HP to 3.
15. **Guardian of Ancient Kings Ret L85** — Ret-only damage variant.
16. **Hand of Reckoning** — Ret-baseline taunt for ad pickup.
17. **Forbearance** — shared 1-min CD on LoH/DS/HoP.
18. **Pre-pot Golemblood** — Strength pot.
19. **Hit cap 8% / Expertise 26** — same as all melee.
20. **Stat priority Mastery > Crit > Haste** — disposition.
21. **Talent split 31/7/3** — default.
22. **Two-Handed Specialization** — Ret talent; +20% 2H damage.
23. **Seal swap heuristic** — 1-3 = Truth, 4+ = Righteousness.
24. **Crusader Strike L1** — Cata-Ret unlocks at L1.
25. **Templar's Verdict / Inquisition / Sanctified Wrath / Zealotry / Two-Handed Specialization** — Ret spec-gated.
26. **Hammer of Wrath L36** — confirmed.
27. **Avenging Wrath L30 (Ret)** — Ret-spec L30 reward.
28. **Divine Storm L30 (Ret talent)** — talent-gated.
29. **Word of Glory L9** — baseline Holy Power introduction.
30. **Lay on Hands 10-min CD** — Cata.
31. **Divine Protection** — Cata: -20% magic; Glyph removes magic-only.
32. **AW + Zealotry + GoAK alignment** — first-pull stack.

End of survey.

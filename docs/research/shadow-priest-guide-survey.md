# Shadow Priest 4.3.4 — Multi-Guide Strategy Survey

Audit trail for the strategy decisions that will be encoded in `docs/specs/shadow-priest.md`. The spec doc is the source of truth for the bot.

Survey conducted 2026-05-02. Strategy/threshold content reconciled across the strategy-bearing guides below. This is the **fourth ranged-DPS survey in the project**. Unlike prior ranged-DPS docs, **no spec-selection reconciliation is needed** — Shadow is the only PVE-DPS-viable Priest spec; Holy/Disc are healers.

---

## 1. Spec selection — Shadow (no alternatives)

| Source | Shadow Priest |
|---|---|
| Icy Veins Cata Classic Tier List | **B-tier (mid)** |
| wowtbc.gg Phase 4 (top-10% WCL) | rank #14 |
| Wowhead Cata Tier 13 | B-tier |

**Spec disposition: Shadow.** Mid-tier sustained DPS built around DoT maintenance + Shadow Orb procs + Evangelism/Archangel cooldown burst. Strong utility (Vampiric Embrace raid-heal, Dispersion mana/defensive, Hymn of Hope raid mana, Mass Dispel).

---

## 2. Sources

| # | Guide | URL | Confidence |
|---|---|---|---|
| G1 | Icy Veins Shadow Rotation/CDs | https://www.icy-veins.com/cataclysm-classic/shadow-priest-pve-rotation-cooldowns-abilities | High |
| G2 | Icy Veins Shadow Talents/Glyphs | https://www.icy-veins.com/cataclysm-classic/shadow-priest-pve-spec-builds-talents-glyphs | Medium-High |
| G3 | Icy Veins Shadow Stat Priority | https://www.icy-veins.com/cataclysm-classic/shadow-priest-pve-stat-priority | High |
| G5 | wowtbc.gg Shadow Priest | https://wowtbc.gg/cata/class-guides/shadow-priest/ | Medium |
| G6 | gotwarcraft Shadow 4.3 | https://gotwarcraft.com/guides/classguides/shadowpriest.php | High — explicit 31/10/0 |
| G7 | Overgear Shadow | https://overgear.com/guides/cataclysm-classic/shadow-priest-guide/ | Medium-High |
| G8 | Warcraft Tavern Shadow | (403'd) | Medium |
| G10 | Tales of a Priest haste | http://talesofapriest.com/cataclysm-and-haste/ | High for breakpoints |
| G11 | Engadget Spiritual Guidance haste | https://www.engadget.com/2011-05-25-spiritual-guidance-the-mechanics-of-shadow-priest-haste/ | High for tick formulas |
| G14 | Wowhead Shadowy Apparition | https://www.wowhead.com/cata/spell=78204/shadowy-apparition | Medium-High |
| G15 | Warcraft Wiki per-spell | https://warcraft.wiki.gg/wiki/<spell> | High |
| G16 | Wowhead Cata per-spell | https://www.wowhead.com/cata/spell=<id> | High |

**Caveats:** Multiple community guides 403'd; reconstructed via search. Wowhead Cata Classic guide pages blocked.

---

## 3. Strong consensus

| Item | Consensus | Spec disposition |
|---|---|---|
| **Shadow B-tier in DS.** | 4/4 | Documented. |
| **Spec utility:** Vampiric Embrace, Dispersion, Hymn of Hope, PW:Shield, Mass Dispel, Mind Soothe, Fade, PW:Fortitude. | 5/5 | Exposed. |
| **Single-target priority** — SW:Pain (apply once, Pain and Suffering refresh) > Vampiric Touch (apply + maintain) > Devouring Plague (apply + maintain, instant) > Mind Blast on CD (consumes Shadow Orbs) > SW:Death below 25% > Mind Flay filler > 5x Dark Evangelism → Archangel. | 5/5 | Encoded. |
| **Shadow Orbs** — generated 10% chance per SW:P tick AND per Mind Flay tick. Max 3. Consumed by Mind Blast/Mind Spike. | 4/5 | Tracked 0-3. |
| **Shadow Orb damage** — Mind Blast consumes ALL stacks; +10% damage per orb consumed. | 4/4 | Auto-consumed. |
| **Empowered Shadow buff** — applied when orbs consumed. 15s. +20% periodic damage (+22% with Harnessed Shadows 2/2). | 4/4 | Refresh DoTs while active for snapshot. |
| **Mastery: Shadow Orb Power** — +1.3% per mastery point to Empowered Shadow's DoT modifier. Passive. | 3/3 | Reforge target. |
| **Shadow Word: Pain** — instant, 22% mana, 18s, 3s tick. Pain and Suffering 60%/tick refresh on Mind Flay. | 5/5 | Apply once at opener. |
| **Vampiric Touch** — 1.5s cast, 16% mana, 15s, 3s tick. Mind Blast on VT-target → Replenishment (1% mana / 10s raid). | 5/5 | Refresh ≤2s remaining. |
| **Devouring Plague** — INSTANT, 25% mana, 24s, 3s tick. Improved Devouring Plague: up to 30% upfront damage. | 4/4 | Apply at opener; refresh ≤3s. |
| **Mind Blast** — 1.5s cast, 17% mana, 8s baseline (6.5s with Improved Mind Blast 3/3). | 5/5 | Cast on CD. |
| **Mind Flay** — 3s channel, 1s tick (3 ticks), 8% mana, 50% slow. | 5/5 | Default filler. |
| **Mind Sear** — 5s channel, 28% mana, 10y AoE. | 4/4 | AoE-mode at 3+. |
| **Shadow Word: Death** — instant, 12% mana, 10s CD, 40y. ≤25% HP = +200% damage. Self-damage if target survives. T13 2P removes self-damage. Glyph: 10s reset on miss. | 5/5 | Execute. |
| **Mind Spike** — 1.5s cast, 12% mana, lvl 81. **Removes self DoTs.** NOT used in DoT rotation. | 4/5 | Reserved for pre-pull / no-DoTs scenarios. |
| **Shadowform** — instant, 13% mana, lvl 40 spec. +15% Shadow damage, -15% damage taken. Always-on. | 5/5 | Maintain. |
| **Shadowfiend** — 5min CD, 15s pet, lvl 66. 3% mana/hit. Sin and Punishment: -10s per Mind Flay crit. | 5/5 | Mana CD + DPS. |
| **Dispersion** — 2min CD, 6s, +6% mana/sec, -90% damage taken. | 4/4 | Mana emergency + raid mechanic soak. |
| **Hymn of Hope** — 8s channel, 6min CD, lvl 64. 2% mana/2s to 3 lowest-mana + 15% max mana. | 3/3 | raid_mana_pct < 35% trigger. |
| **Vampiric Embrace** — toggle, no CD. 6% caster + 3% raid heal from Shadow damage. | 3/3 | Toggle ON. |
| **Power Word: Shield** — 3s CD, 34% mana, Weakened Soul. | 4/4 | Self-cast on incoming damage. |
| **Inner Fire vs Inner Will** — exclusive. Inner Fire: +SP. Inner Will: -15% instant cost + movement. | 4/4 | Default Inner Fire. |
| **Talent build — 31/10/0** (31 Shadow + 10 Discipline + 0 Holy). | 5/5 | Encoded. |
| **Stat priority** — Int > Hit (17% / 1742) > Haste > Mastery > Crit. | 4/5 | Encoded. |
| **Spell hit cap** — 17% / 1742 at L85. | 5/5 | Hard cap. |
| **Twisted Faith** — 100% Spirit→Hit + 4% Shadow damage. | 5/5 | Pre-condition for spirit reforge. |
| **Pain and Suffering** — 30%/60% per Mind Flay tick to refresh SW:P + reduces SW:Death self-damage 20%/40%. | 5/5 | "Cast SW:P once, never refresh manually." |
| **Sin and Punishment** — 5s/10s Shadowfiend CD reduction per Mind Flay crit. | 5/5 | Implicit benefit. |
| **Masochism** — 2%/4% mana on ≥10% HP self-damage (incl. SW:Death backlash). | 4/4 | Permits pre-execute SW:Death for mana. |
| **Improved Devouring Plague** — 5%/10% upfront, ×3 ticks = up to 30% upfront. | 4/4 | Strong opener bursts. |
| **Improved Mind Blast 3/3** — -1.5s CD (8s → 6.5s). | 4/4 | Mandatory. |
| **Veiled Shadows** — -30s/-60s Shadowfiend CD + 5/+10s Fade duration. | 4/4 | Mandatory. |
| **Darkness** — +1%/+2%/+3% spell haste. | 4/4 | Mandatory. |
| **Improved SW:Pain** — +5%/+10% damage. | 4/4 | Mandatory. |
| **Shadowy Apparition** — 4%/8%/12% per SW:P tick to summon chasing shadow. Max 4 active. | 4/4 | Passive. |
| **Vampiric Touch (talent)** — 1pt unlocks. | 4/4 | Mandatory. |
| **Dispersion (talent)** — 1pt unlocks. | 4/4 | Mandatory. |
| **Phantasm / Inner Sanctum** — defensive flex. | 3/3 | Choose Inner Sanctum (-4% spell damage). |
| **Dark Evangelism** — applied per Mind Flay tick (1 stack/tick). +2% per stack. Max 5. +10% DoT damage at 5. | 4/4 | Auto-stacked. |
| **Archangel** — 1.5min CD, consumes Evangelism stacks. +5% mana, +4%/stack damage to MF/MS/MB/SW:D, 18s. | 4/4 | Cast at 5 stacks. |
| **Glyphs — Prime: SW:Pain + Mind Flay + SW:Death.** | 5/5 | Default. |
| **Glyphs — Major: Spirit Tap + Inner Fire + Fade.** | 4/5 | Default. |
| **Glyphs — Minor: Fading + Levitate + Shadowfiend.** | 3/3 | Default. |
| **Pre-pot — Volcanic Potion** — +1200 Int 25s. T-2s + 1-min mark. | 3/3 | Encoded. |
| **Opener:** pre-pot T-2s → cast Vampiric Touch landing T=0 → SW:P → DP → Mind Blast → Mind Flay (build Evangelism + orbs) → Mind Blast → Archangel at 5 stacks → continue priority. | 4/4 | Encoded. |
| **AoE:** SW:P + VT on primary → Mind Sear channel → Pain and Suffering refreshes SW:P. Multi-DoT 3-4 if targets live ≥10s. | 3/4 | AoE-mode `enemies_within_10y >= 3`. |
| **DoT clipping** — refresh ≤3s remaining (1 tick) AND Empowered Shadow active for snapshot. | 4/4 | 3s threshold. |
| **Mana cascade** — Shadowfiend on CD → Dispersion at <20% → Hymn of Hope at <35% → Archangel +5% → Replenishment via VT-MB. | 4/4 | Encoded. |

---

## 4. Disagreements

### 4.1. Talent build — 31/10/0 (Disc secondary)

**Spec disposition: 31/10/0.** Per gotwarcraft G6: 31 Shadow core + 10 Discipline (Twin Disciplines 3/3, Mental Agility 2/3, Evangelism 2/2, Archangel 1/1, Inner Sanctum 2/3 OR Phantasm 2/2).

### 4.2. Shadow Orb generation rate

**Spec disposition: 10% per tick on SW:P AND Mind Flay.** Tagged DBC verify (one source claims 18%).

### 4.3. Mind Spike role — NOT in DoT rotation

**Spec disposition: Mind Spike removes self DoTs; bot does NOT cast in standard rotation.** Reserved for pre-pull or no-DoTs targets.

### 4.4. Dark Evangelism stacks — per-tick (5 ticks → 5 stacks)

**Spec disposition: 1 stack per Mind Flay tick (3 ticks/channel; ~2 channels to cap).**

### 4.5. Shadowy Apparition movement-bonus claim

**Spec disposition: 12% base; NO movement bonus assumed.** Tagged DBC verify.

### 4.6. Stat priority Mastery vs Haste

**Spec disposition: Hit > Haste (to breakpoint) > Mastery > Crit.**

### 4.7. Harnessed Shadows mechanic

**Spec disposition: Empowered Shadow = +20% baseline + 1%/+2% per Harnessed Shadows rank → +22% at 2/2.**

### 4.8. Glyph third Prime — SW:Death (default) or Dispersion (situational)

**Spec disposition: SW:Death default; Dispersion swap for solo-mechanic adds.**

### 4.9. Mind Sear AoE thresholds

**Spec disposition:** 2 → DoT primary only; 3-4 → multi-DoT + Mind Sear; 5+ → pure Mind Sear spam.

### 4.10. SW:Death pre-execute usage

**Spec disposition: Cast SW:Death (a) HP ≤ 25%, (b) mana_pct < 30 AND Shadowfiend CD > 30s.** Masochism mana feedback.

### 4.11. T13 4P bonus — Shadow Orb gen on apparition damage

**Spec disposition: T13 2P = +55% SW:Death damage + no backlash; T13 4P = Shadow Orbs from pet/apparitions.**

---

## 5. Items NOT addressed (author judgment)

| Spec item | Value | Why |
|---|---|---|
| Mind Blast cast threshold | cast on CD | Maximize orb consumption + Empowered Shadow uptime. |
| AoE-mode entry | enemies_within_10y_of_target >= 3 | Mind Sear 10y match. |
| Pre-cast VT on opener | YES; cast 1.5s before pull | Snapshot benefit. |
| DoT refresh threshold (snapshot) | ≤3s remaining AND Empowered Shadow active | Maximize snapshot. |
| DoT refresh threshold (no buff) | ≤3s remaining | Avoid snapshot loss. |
| Self-emergency PW:Shield | self.hp_pct < 60 AND no_Weakened_Soul | Self-shield. |
| Dispersion auto | self.mana_pct < 20 OR self.hp_pct < 30 OR raid_AoE_imminent | Defensive + mana CD. |
| Hymn of Hope auto | raid.mana_avg < 35 AND combat_duration > 60s | Don't waste 6min CD early. |
| Shadowfiend auto | mana_pct < 50 AND fight_remaining > 15s | Mana + DPS. |
| Archangel timing | Evangelism stacks == 5 AND Mind Blast about to cast | Buff applies before next damage. |
| SW:Death execute | target.hp_pct ≤ 25 AND not_self_kill | Execute. |
| SW:Death mana-emergency | mana_pct < 30 AND Shadowfiend CD > 30s | Masochism feedback. |
| Mind Spike usage | NEVER in DoT rotation | DoT removal. |
| Inner Fire vs Will | Default Fire; swap Will only if movement >30% | Flat +SP > mobility. |
| Vampiric Embrace | Always ON in raid | Free raid heal. |
| Shadowform | Always maintain | -15% damage taken. |
| Pre-pot timing | T-2s + 1-min mark | Bot-side. |
| Mind Soothe | manual command-only | Utility. |
| Mass Dispel | enemy_buff_dispellable AND CD ready | Auto-purge. |
| Fade auto | self.threat_pct > 90 AND tank_alive | Threat dump. |

---

## 6. Author-judgment items — leveling

```yaml
spell_unlocks:
  POWER_WORD_FORTITUDE:    {level: 1,  spell_id: 1243}
  SMITE:                   {level: 1,  spell_id: 585}
  SHADOW_WORD_PAIN:        {level: 4,  spell_id: 589}
  POWER_WORD_SHIELD:       {level: 6,  spell_id: 17}
  MIND_BLAST:              {level: 9,  spell_id: 8092}
  MIND_FLAY:               {level: 10, spell_id: 15407}
  FADE:                    {level: 12, spell_id: 586}
  PSYCHIC_SCREAM:          {level: 14, spell_id: 8122}
  INNER_FIRE:              {level: 16, spell_id: 588}
  LEVITATE:                {level: 22, spell_id: 1706}
  DISPEL_MAGIC:            {level: 26, spell_id: 527}
  DEVOURING_PLAGUE:        {level: 28, spell_id: 2944}
  MIND_SOOTHE:             {level: 30, spell_id: 453}
  SHADOW_WORD_DEATH:       {level: 32, spell_id: 32379}
  SHADOWFORM:              {level: 40, spell_id: 15473}
  PRAYER_OF_FORTITUDE:     {level: 48, spell_id: 21562}
  HYMN_OF_HOPE:            {level: 64, spell_id: 64901}
  SHADOWFIEND:             {level: 66, spell_id: 34433}
  MASS_DISPEL:             {level: 68, spell_id: 32375}
  MIND_SEAR:               {level: 75, spell_id: 48045}
  MIND_SPIKE:              {level: 81, spell_id: 73510}
  INNER_WILL:              {level: 83, spell_id: 73413}
  # Talent-gated:
  VAMPIRIC_TOUCH:           {requires_talent: true, spell_id: 34914, tree: shadow}
  VAMPIRIC_EMBRACE:         {requires_talent: true, spell_id: 15286, tree: shadow}
  IMPROVED_DEVOURING_PLAGUE:{requires_talent: true, tree: shadow}
  PAIN_AND_SUFFERING:       {requires_talent: true, tree: shadow}
  SIN_AND_PUNISHMENT:       {requires_talent: true, tree: shadow}
  TWISTED_FAITH:            {requires_talent: true, tree: shadow}
  HARNESSED_SHADOWS:        {requires_talent: true, tree: shadow}
  MIND_MELT:                {requires_talent: true, tree: shadow}
  MASOCHISM:                {requires_talent: true, tree: shadow}
  IMPROVED_MIND_BLAST:      {requires_talent: true, tree: shadow}
  VEILED_SHADOWS:           {requires_talent: true, tree: shadow}
  IMPROVED_SHADOW_WORD_PAIN:{requires_talent: true, tree: shadow}
  DARKNESS:                 {requires_talent: true, tree: shadow}
  SHADOWY_APPARITION:       {requires_talent: true, tree: shadow}
  PHANTASM:                 {requires_talent: true, tree: shadow}
  DISPERSION:               {requires_talent: true, tree: shadow, spell_id: 47585}
  EVANGELISM:               {requires_talent: true, tree: discipline}
  ARCHANGEL:                {requires_talent: true, tree: discipline}
  TWIN_DISCIPLINES:         {requires_talent: true, tree: discipline}
  MENTAL_AGILITY:           {requires_talent: true, tree: discipline}
  INNER_SANCTUM:            {requires_talent: true, tree: discipline}
  EMPOWERED_SHADOW:         {passive_buff: true, base_pct: 20, harnessed_shadows_pct: 2}
  MASTERY_SHADOW_ORB_POWER: {passive_mastery: true, base_pct: 1.3_per_point}
```

**Priest is mana-only.** Mana cascade: Shadowfiend → Dispersion → Hymn of Hope → Archangel → Replenishment.

**Pre-Shadowform leveling** (under L40): Smite + SW:P. Once Shadowform unlocked, switch to full DoT rotation. Below L66 (no Shadowfiend), mana via Spirit Tap + Dispersion.

---

## 7. Suggested next research passes

1. Shadow Orb proc rate (10% vs 18%).
2. Empowered Shadow baseline (20% vs 22% w/ Harnessed Shadows).
3. Shadowy Apparition movement-bonus claim.
4. Pain and Suffering proc rate (60% claim).
5. Dark Evangelism stack-application (per-cast vs per-tick).
6. Mind Blast CD scaling with Improved Mind Blast (8s → 6.5s).
7. Improved Devouring Plague upfront formula.
8. Sin and Punishment Shadowfiend CD reduction stacking limit.
9. T13 set bonuses exact wording.
10. Replenishment uptime mechanic.
11. Mind Spike + Mind Melt 2-stack instant Mind Blast confirmation.

---

## 8. DBC verification

- SW:P 22%/18s/3s tick. Spell.dbc 589.
- VT 16%/15s/3s. Spell.dbc 34914.
- DP 25%/24s/3s/instant. Spell.dbc 2944.
- Mind Blast 17%/1.5s/8s baseline. Spell.dbc 8092.
- Mind Flay 8%/3s/1s tick. Spell.dbc 15407.
- Mind Spike 12%/1.5s/lvl 81. Spell.dbc 73510.
- Mind Sear 28%/5s/10y. Spell.dbc 48045.
- SW:Death 12%/10s/40y/lvl 32. Spell.dbc 32379.
- Shadowform 13%/lvl 40. Spell.dbc 15473.
- Shadowfiend 5min/15s/lvl 66. Spell.dbc 34433.
- Dispersion 2min/6s/-90% damage. Spell.dbc 47585.
- Hymn of Hope 8s/6min/lvl 64. Spell.dbc 64901.
- Shadowy Apparition 4/8/12% per rank. Talent.dbc.
- Pain and Suffering 30%/60% per MF tick. Talent.dbc.
- Twisted Faith 100% Spirit→Hit. Talent.dbc 47577.
- Archangel +4%/stack damage 18s. Spell.dbc 87151.
- Mastery Shadow Orb Power 1.3%/pt. Mastery DBC.
- Empowered Shadow buff DoT modifier. Spell.dbc.
- Improved Devouring Plague rank effect. Talent.dbc.
- Improved Mind Blast rank effect. Talent.dbc.
- Veiled Shadows rank effect. Talent.dbc.
- Sin and Punishment rank effect. Talent.dbc.
- Masochism rank effect. Talent.dbc.
- Mind Melt rank effect. Talent.dbc.
- Harnessed Shadows rank effect. Talent.dbc.
- Darkness rank effect. Talent.dbc.
- Twin Disciplines rank effect. Talent.dbc.
- Mental Agility rank effect. Talent.dbc.
- Glyph of SW:Pain (+10% crit). Glyph.dbc.
- Glyph of Mind Flay (+10%). Glyph.dbc.
- Glyph of SW:Death (10s reset on miss). Glyph.dbc.
- Glyph of Spirit Tap. Glyph.dbc.
- Inner Fire 60% armor + 532 SP at L85. Spell.dbc.
- Inner Will -15% instant cost / +10% movement. Spell.dbc.
- T13 2P/4P bonuses. Item set DBC.
- Volcanic Potion +1200 Int. Item DBC.

---

## 9. Notes on prompt deviations

1. **Mind Spike "consumes 1 of 3 stacks"** — partly correct. Mind Spike applies 30% crit-chance buff to next Mind Blast (max 3) AND with Mind Melt: -25%/-50% Mind Blast cast time per stack (max 2). Bot does NOT cast Mind Spike in DoT rotation.
2. **Mastery: Shadow Orb Power** — passive scaling on Empowered Shadow buff, NOT generation rate.
3. **Devouring Plague (instant or 1.5s cast?)** — INSTANT in Cata.
4. **Mind Blast 4-set with talent** — Improved Mind Blast 3/3 takes 8s → 6.5s. T13 4P does NOT reduce CD (it grants Shadow Orbs from pet/apparitions).
5. **Pain and Suffering** — 60%/Mind Flay tick (~93% per channel).
6. **Hit cap 17%** — confirmed for Priest spell hit.
7. **SW:Death (62?)** — corrected to **level 32**.
8. **Mind Sear (75?)** — confirmed.
9. **Mind Spike (~70?)** — corrected to **level 81**.
10. **Shadowy Apparitions / Shadow Orbs** — TWO separate mechanics. Shadow Orbs (mastery resource consumed by Mind Blast). Shadowy Apparition (talent procs from SW:P, separate visuals).
11. **Glyphs Major: Mind Soothe** — corrected to Spirit Tap (DPS-relevant).
12. **Talent build "31 + 5"** — corrected to **31/10/0** (10 Discipline secondary).
13. **Mind Bender** — does NOT exist in Cata 4.3.4. Added in MoP 5.0.4.

End of survey.

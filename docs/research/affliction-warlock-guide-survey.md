# Affliction Warlock 4.3.4 — Multi-Guide Strategy Survey

Audit trail for the strategy decisions that will be encoded in `docs/specs/affliction-warlock.md`. The spec doc is the source of truth for the bot; this doc records *which guides agreed*, *where they disagreed*, and *which thresholds were the bot author's judgment call rather than guide consensus*.

Survey conducted 2026-05-02 by the bot author. Numerical data sourced from Wowhead's Cata archive and Warcraft Wiki. Strategy/threshold content reconciled across the strategy-bearing guides below.

This is the **second ranged-DPS survey in the project** (after frost-mage). Section 1 below justifies the spec pick — Warlock has three viable specs and the meta-DPS leader (Demonology) is *not* the project's pick.

---

## 1. Spec selection — Affliction vs Demonology vs Destruction (4.3.4 PVE)

### Source consensus on raw raid-DPS ranking (Dragon Soul / Phase 4 / 4.3.4)

| Source | Affliction | Demonology | Destruction |
|---|---|---|---|
| Wowhead Cata Classic Tier 13 DPS rankings | A-tier | **S-tier (top)** | C-tier |
| Icy Veins Cata Classic DPS rankings (Feb 2025, ckmeans/WCL) | B-tier (rank #12) | **S-tier (rank #2 overall)** | B-tier (rank #15) |
| Warcraft Tavern (top-10% WCL parses, Phase 4) | rank #18 (B) | **rank #1 (S)** | rank #16 (B) |
| Aggregated community tier lists | A | **S** | C |

The raw-DPS verdict in 4.3.4 is unambiguous: **Demonology >> Affliction > Destruction**.

### Why Affliction is still the project pick

The decisive factor is that Affliction is the simplest of the three for a bot's rotation engine while still being A-tier in Dragon Soul and top-tier in multi-target — which dominates 5-man content. Demonology is genuinely better single-target raid DPS but pet-twisting and Metamorphosis-window optimization push the rotation engine into territory that's out of scope for the first Warlock spec doc.

| Criterion | Affliction | Demonology | Destruction |
|---|---|---|---|
| **Leveling viability** | **Best** (drain-tanking) | Good | Mid |
| **Multi-target dungeon trash** | **Best** (Soul Swap + Soulburn-Seed) | Good (Hellfire/Immo Aura) | Weak |
| **Pull recovery / target switch** | Soul Swap transfers DoTs in 1 GCD | Hard-cast everything | Hard-cast everything |
| **Pet AI complexity** | Felhunter (single sane pet) | Pet-twisting (Felguard ↔ Felhunter) | Imp |
| **Rotation engine complexity** | DoT-priority list (low) | Meta windows + procs (high) | Backdraft + buff maintenance (mid) |

**Spec disposition for the project: Affliction.** Acknowledged downside: rank #12-18 vs Demonology's rank #1-2 in Dragon Soul. Future option: a Demonology sibling spec doc for max-level raid optimization.

---

## 2. Sources

| # | Guide | URL | Author / community | Date / patch | Confidence |
|---|---|---|---|---|---|
| G1 | Icy Veins — Affliction Rotation, Cooldowns & Abilities | https://www.icy-veins.com/cataclysm-classic/affliction-warlock-pve-rotation-cooldowns-abilities | Icy Veins | Cata Classic | High |
| G2 | Icy Veins — Affliction Talent Builds & Glyphs | https://www.icy-veins.com/cataclysm-classic/affliction-warlock-pve-spec-builds-talents-glyphs | Icy Veins | Cata Classic | Medium |
| G3 | Icy Veins — Affliction Stat Priority | https://www.icy-veins.com/cataclysm-classic/affliction-warlock-pve-stat-priority | Icy Veins | Cata Classic | High — concrete numbers (17%/1742 hit, mastery 13% base + 1.63%/pt) |
| G4 | Icy Veins — Affliction Leveling | https://www.icy-veins.com/cataclysm-classic/affliction-warlock-leveling | Icy Veins | Cata Classic | High |
| G5 | Icy Veins — Affliction Spec Overview | https://www.icy-veins.com/cataclysm-classic/affliction-warlock-pve-guide | Icy Veins | Cata Classic | Medium |
| G7 | Icy Veins — Demonology Spec Overview (for §1) | https://www.icy-veins.com/cataclysm-classic/demonology-warlock-pve-guide | Icy Veins | Cata Classic | High |
| G8 | Icy Veins — DPS Tier List | https://www.icy-veins.com/cataclysm-classic/dps-rankings-tier-list-for-cataclysm-classic | Icy Veins | Cata Classic | High for §1 |
| G9 | wowtbc.gg — Affliction Warlock | https://wowtbc.gg/cata/class-guides/affliction-warlock/ | wowtbc.gg | Cata Classic | Medium |
| G10 | wowtbc.gg — Cata DPS Rankings | https://wowtbc.gg/cata/class-rankings/pve-rankings/ | wowtbc.gg | Cata Classic | High for §1 |
| G12 | Overgear — Affliction Cata Classic | https://overgear.com/guides/cataclysm-classic/affliction-warlock-guide/ | Overgear | Cata Classic | Medium |
| G13 | gotwarcraft.com — Affliction Warlock for Cataclysm | https://gotwarcraft.com/guides/classguides/afflictionwarrlock.php | community | Cata 4.3 | Medium-High — explicit 31/0/10 |
| G14 | gotwarcraft.com — Warlock overview + leveling | https://gotwarcraft.com/guides/classguides/warlock.php | community | Cata 4.3 | Medium |
| G15 | Wowhead — Cata Tier 13 DPS Rankings | https://www.wowhead.com/cata/guide/classes/tier-lists/dps-rankings | Wowhead | Cata Classic | §1 only |
| G16 | Warmane forum — 4.3.4 Affliction Warlock PvE Guide thread 316459 | https://forum.warmane.com/showthread.php?t=316459 | Warmane | 4.3.4 | Low (403'd) |
| G17 | MMO-Champion — Warlock Tips and Tricks 4.3 thread 1120365 | https://www.mmo-champion.com/threads/1120365 | MMO-C | 4.3 | Medium |
| G18 | Cynwise — Warlock Spell Changes in Cataclysm | https://cynwise.wordpress.com/2012/04/11/appendix-a-warlock-spell-changes-in-cataclysm/ | community blog | 4.3 era | Medium |
| G19 | Warcraft Wiki — per-spell pages | https://warcraft.wiki.gg/wiki/<spell> | wiki | Cata patch notes | High for hard mechanics |
| G20 | Wowhead Cata — per-spell pages | https://www.wowhead.com/cata/spell=<id> | Wowhead | Cata Classic | High for hard mechanics |
| G21 | Wowpedia — Bane of Havoc | https://wowpedia.fandom.com/wiki/Bane_of_Havoc_(warlock_talent) | wiki | Cata 4.0.1 | Medium (403'd direct) |
| G22 | Engadget WoW Insider — Warlock Cataclysm Study Guide Part 2 | https://www.engadget.com/2010-11-16-the-warlock-study-guide-for-cataclysm-part-2.html | community | 4.0.x | Medium |

**Caveats on coverage:**
- G16, G24, G25, G21 returned 403/404; content reconstructed from search excerpts.
- Wowhead spell pages reachable but strip spell IDs from rendered text.
- No surveyed guide gave concrete numerical thresholds for HP-emergency, mana conserve %, drink %.
- Only G13 (gotwarcraft) lists named talent picks for both trees in the 31/0/10 build.

---

## 3. Strong consensus (≥3 of the strategy-bearing guides agree)

| Item | Consensus | Spec disposition |
|---|---|---|
| **Affliction is A-tier in Dragon Soul** — strong but not best. | 5/5 | Documented in §1. |
| **Spec utility:** CotE, Soulstone, Healthstone, Banish, Fear, Death Coil. | 5/5 | All exposed to command interface. |
| **Single-target priority** — Haunt > Bane of Doom > Corruption > Unstable Affliction > CotE > Shadowflame (in melee) > Shadow Bolt filler > Drain Soul (≤25%). | 5/5 | Spec encodes priority list. |
| **Haunt** is the centerpiece. Buff+debuff (+20% all shadow DoT damage to target). 1.5s cast, 8s CD, 1 shard. | 5/5 | Tier 1 priority — refresh on CD. Wowhead spell=48181. |
| **Everlasting Affliction** — Shadow Bolt + Haunt + Drain Soul refresh Corruption. | 5/5 | Bot only re-casts Corruption if it falls off (target swap, dispel). |
| **Pandemic** — Drain Soul refreshes UA during execute. | 4/4 | At <25%, no manual UA refresh. |
| **Execute phase** — at target HP ≤25%, swap Shadow Bolt filler for Drain Soul (×2 damage). | 5/5 | Hard threshold. |
| **Bane of Doom** replaces Curse of Doom in Cata. 1-min duration, 15s ticks, 20% Ebon Imp summon proc. | 4/4 | Default Bane; swap to Bane of Agony when `ttl < 30s`. |
| **Bane vs Curse** — separate categories. One Bane + one Curse can coexist on same target. | 4/4 | Tracks Bane and Curse slots independently. |
| **Curse of the Elements** — 8% magic damage taken, 5min, instant, 10% mana. Skip if Ebon Plaguebringer/Earth and Moon/Master Poisoner present. | 4/4 | Apply-once-per-target with 4-min refresh; skip if other class provides. |
| **Pet — Felhunter** — Spell Lock + Devour Magic + best pet damage. | 5/5 | Standing pet for Aff. |
| **Mastery: Potent Afflictions** — 13% baseline + 1.63%/pt. | 4/4 | Passive — informs reforge. |
| **Stat priority** — Int > Hit (17%/1742) > Haste > Crit > Mastery. | 4/5 | Encoded; Mastery contention noted in §4. |
| **Glyphs — Prime: Haunt + Corruption + Unstable Affliction.** | 5/5 | Universal. |
| **Glyphs — Major: Life Tap + Shadow Bolt + Soul Swap.** | 5/5 | Universal. |
| **Glyphs — Minor: Drain Soul + Unending Breath + Ritual of Souls.** | 4/4 | Default. |
| **Soul Swap — Tier 4 Aff TALENT in Cataclysm**, NOT baseline (became baseline in MoP). 18% base mana, 1 GCD, 1 shard. With Glyph: COPIES instead of removes. | 4/4 | Talent-gated in spec catalog. |
| **AoE rotation:** 2 targets → Soul Swap; 3-5 → Soulburn-Seed; 6+ → Seed spam. **Bane of Havoc is NOT an Affliction tool** (Destruction Tier-6 talent). | 4/4 | AoE-mode triggered by `enemies_within_15y_of_target >= 2` with discrete branches. |
| **Soul Shards** — max 3 in combat, no regen for Aff except Drain Soul kills. | 4/4 | Bot reserves 1 shard for Soulstone. |
| **Soulburn** (45s CD, 1 shard, 15s buff) — empowers next cast (instant Soul Fire, instant Drain Life, instant pet summon, etc.). | 4/4 | Used for AoE opener (Seed of Corruption) + emergency pet revive. |
| **Demon Soul** (level 85, 2-min CD, 20s, varies by pet). Felhunter: +20% periodic shadow. | 4/4 | Fires on opener; subsequent on CD. |
| **Doomguard / Infernal** — 10-min CD, 45s pet. Snapshots SP — fire during Bloodlust + Demon Soul + potions. | 4/4 | Doomguard for single-target boss; Infernal for AoE-mode. |
| **Shadow Bolt** — filler (no DoT needs refresh AND target HP > 25%). | 5/5 | Default filler. |
| **Shadow Embrace** — Shadow Bolt + Haunt apply +5% shadow DoT debuff, stacks 3. | 4/4 | Bot prioritizes Shadow Bolt over Drain Life when SE stacks < 3. |
| **Eradication** — 6% per Corruption tick, +20% haste 10s. RNG, no rotation logic. | 4/4 | Passive. |
| **Volcanic Potion** — T-2s pre-pull + 1-min mark cooldown align. | 3/3 | Encoded. |
| **Opener sequence** — pet up + Volcanic Potion T-2s → Shadow Bolt to land at pull → CotE → Bane of Doom → Haunt → Demon Soul → UA → Corruption → Doomguard → Shadowflame → Shadow Bolt filler. | 4/4 | Encoded. |
| **DoT clipping in Cata** — refresh carries over remainder; haste adds ticks (not reduces duration); SP/haste procs DON'T update existing DoT applications. | 3/3 | Bot delays refresh ≤1.5s for haste-proc-imminent. |
| **Banish/Fear/Death Coil** — CC and self-heal utility. | 3/3 | Exposed to command interface. |
| **Life Tap** — trade HP for mana. | 4/4 | Auto-fire when `mana_pct<50 AND hp_pct>70 AND no_priority_cast`. |
| **Fel Armor** — default armor. Demon Armor situational. | 3/3 | Encoded. |
| **Drain Life** — leveling sustain only. | 3/3 | At max-level fires only as defensive. |

---

## 4. Disagreements

### 4.1. Talent build — 31/0/10 (gotwarcraft) vs 32/0/X (Icy Veins partial)

**Spec disposition: 31/0/10.** 31 Aff (capstone Haunt) + 10 Destruction (Bane CD reduction + Shadow and Flame +12% Shadow Bolt + Improved Soul Fire OR Emberstorm).

### 4.2. Pet — Felhunter vs Doomguard primary

**Spec disposition: Felhunter as standing pet; Doomguard for single-target boss; Infernal for AoE-mode.**

### 4.3. Drain Life in execute — leveling-only

**Spec disposition: Drain Life is leveling-only.** At 85, fires only as defensive (HP < 30%).

### 4.4. Bane of Havoc — Destruction only, NOT Affliction

**Spec disposition: Bane of Havoc is NOT in the Affliction rotation.** The prompt's bullet was incorrect. Affliction's multi-target tools are Soul Swap and Soulburn-Seed-of-Corruption.

### 4.5. Soul Swap — talent-gated in Cata (NOT baseline)

**Spec disposition: Talent-gated in spec catalog**; bot uses freely from talent-spend onward.

### 4.6. Demon Soul level — 85

**Spec disposition: Demon Soul fires at 85+.**

### 4.7. Stat priority Mastery position

**Spec disposition: Int > Hit > Haste > Crit > Mastery.** Mastery is "least prioritized due to minimal returns" per Icy Veins.

### 4.8. Dark Intent target priority

**Spec disposition:** Aff Lock > Boomkin > Shadow Priest > Mage > self if no priority class.

### 4.9. Major glyph — Soul Swap + Shadow Bolt + Life Tap (consensus)

### 4.10. AoE thresholds — discrete branches at 2 / 3-5 / 6+

### 4.11. Curse of the Elements — skip if other class provides

---

## 5. Items NOT addressed by ANY guide (pure author judgment)

| Spec item | Value chosen | Why |
|---|---|---|
| Self-emergency Drain Life | self.hp<30 AND mana_pct>30 | Drain-tank fallback. |
| Soulburn-Healthstone | self.hp<35 AND no_other_save AND shards>=1 | Major emergency. |
| Soulstone (combat-rez) | death of healer OR raid-leader-marked priority | Bot priority. |
| Soulshatter (threat dump) | self.threat_pct > 90 AND not_main_threat | Bot-side. |
| Curse-of-Tongues / Weakness | manual command only | Encounter-specific. |
| Banish auto | cc_marker == "banish" AND target_type in (demon, elemental) | Mirrors Mage poly. |
| Fear auto | DISABLED in raid; manual in 5-man | Fear breaks at low HP. |
| Howl of Terror | self.melee_count >= 2 AND self.hp<60 AND CD ready | Defensive AoE. |
| Death Coil | self.hp<50 AND target.humanoid_or_undead AND CD ready | Self-heal + CC. |
| Demonic Circle Teleport | placed pre-pull; teleport when melee_count>=2 OR self.hp<25 | Mobility/escape. |
| Mana conserve | self.mana<50 | Cross-class default. |
| Mana crisis | self.mana<25 | Cross-class default. |
| Drink threshold | 30%/95% | Cross-class default. |
| Life Tap auto | mana_pct<50 AND hp_pct>70 AND no_priority_cast | Sustain regen. |
| AoE-mode entry | enemies_within_15y_of_target >= 2 | See §4.10. |
| Soul Shard reservation | always reserve 1 for Soulstone | Combat-rez insurance. |
| Pet re-summon | pet.dead AND (not_combat OR fight_remaining>30s) | Mana cost gating. |
| Soulburn-Demon-Summon | pet.dead AND combat AND fight>30s AND shards>=1 | Instant emergency pet. |
| Doomguard auto | opener AND target.is_boss AND fight>1m | One-per-fight. |
| Infernal auto | AoE_mode AND enemies>=4 AND off_CD | One-per-fight. |
| Volcanic Potion timing | T-2s + 1-min mark cooldown align | Bot-side. |
| Dark Intent target | Aff Lock > Boomkin > Shadow Priest > Mage > self | Group-utility. |

---

## 6. Author-judgment items — leveling-specific

```yaml
# Spell unlock breakpoints (sourced from Wowhead Cata Classic / Warcraft Wiki / G4 / G18 / G22)
spell_unlocks:
  SHADOW_BOLT:           {level: 1,  spell_id: 686}
  CORRUPTION:            {level: 4,  spell_id: 172}        # instant in Cata
  IMP:                   {level: 1,  spell_id: 688}
  SUMMON_VOIDWALKER:     {level: 8,  spell_id: 697}
  CURSE_OF_WEAKNESS:     {level: 6,  spell_id: 702}
  LIFE_TAP:              {level: 6,  spell_id: 1454}
  HEALTH_FUNNEL:         {level: 8,  spell_id: 755}
  FEAR:                  {level: 8,  spell_id: 5782}
  DRAIN_LIFE:            {level: 9,  spell_id: 689}
  DRAIN_SOUL:            {level: 10, spell_id: 1120}       # Cata baseline at 10
  BANE_OF_AGONY:         {level: 10, spell_id: 980}        # renamed from Curse of Agony
  CREATE_HEALTHSTONE:    {level: 10, spell_id: 6201}
  UNSTABLE_AFFLICTION:   {level: 10, spell_id: 30108}      # Cata baseline at 10 if Aff-spec
  CURSE_OF_THE_ELEMENTS: {level: 12, spell_id: 1490}        # Cata: lower than pre-Cata's 32
  HOWL_OF_TERROR:        {level: 14, spell_id: 5484}
  CURSE_OF_TONGUES:      {level: 16, spell_id: 1714}
  BANISH:                {level: 18, spell_id: 710}
  SOULSTONE:             {level: 18, spell_id: 20707}
  CREATE_SOULSTONE:      {level: 18, spell_id: 693}
  SUMMON_SUCCUBUS:       {level: 20, spell_id: 712}
  DEMON_ARMOR:           {level: 20, spell_id: 687}
  ENSLAVE_DEMON:         {level: 22, spell_id: 1098}
  DEATH_COIL:            {level: 24, spell_id: 6789}
  CURSE_OF_EXHAUSTION:   {level: 26, spell_id: 18223}
  SOULBURN:              {level: 28, spell_id: 74434}       # Cata baseline at 28
  BANE_OF_DOOM:          {level: 30, spell_id: 603}        # was Curse of Doom at 60 pre-Cata
  SUMMON_FELHUNTER:      {level: 30, spell_id: 691}
  RAIN_OF_FIRE:          {level: 38, spell_id: 5740}
  HELLFIRE:              {level: 40, spell_id: 1949}
  FEL_ARMOR:             {level: 50, spell_id: 28176}
  SUMMON_INFERNAL:       {level: 50, spell_id: 1122}       # 10-min CD; Cata redesign
  SOULSHATTER:           {level: 50, spell_id: 29858}
  SEED_OF_CORRUPTION:    {level: 50, spell_id: 27243}
  SHADOWFLAME:           {level: 56, spell_id: 47897}
  SUMMON_DOOMGUARD:      {level: 60, spell_id: 18540}      # 10-min CD; Cata redesign
  DEMONIC_CIRCLE_SUMMON: {level: 60, spell_id: 48018}
  DEMONIC_CIRCLE_TELE:   {level: 60, spell_id: 48020}
  FEL_FLAME:             {level: 81, spell_id: 77799}
  DARK_INTENT:           {level: 83, spell_id: 85767}
  DEMON_SOUL:            {level: 85, spell_id: 77801}
  # Talent-gated (Affliction tree):
  HAUNT:                 {requires_talent: true, talent_tier: 7}    # 31-Aff CAPSTONE
  SOUL_SWAP:             {requires_talent: true, talent_tier: 4}
  SOULBURN_SEED_OF_CORRUPTION: {requires_talent: true, talent_tier: 6}
  EVERLASTING_AFFLICTION:{requires_talent: true, talent_tier: 6}
  PANDEMIC:              {requires_talent: true, talent_tier: 6}
  ERADICATION:           {requires_talent: true, talent_tier: 3}
  SHADOW_EMBRACE:        {requires_talent: true, talent_tier: 3}
  DOOM_AND_GLOOM:        {requires_talent: true, talent_tier: 1}
  IMPROVED_CORRUPTION:   {requires_talent: true, talent_tier: 1}
  JINX:                  {requires_talent: true, talent_tier: 2}
  IMPROVED_LIFE_TAP:     {requires_talent: true, talent_tier: 2}
  SIPHON_LIFE:           {requires_talent: true, talent_tier: 4}
  NIGHTFALL:             {requires_talent: true, talent_tier: 5}
  # Talent-gated (Destruction tree, for 31/0/10):
  SHADOW_AND_FLAME:      {requires_talent: true, talent_tier: 2, tree: destruction}    # +12% Shadow Bolt
  BANE_DESTRUCTION:      {requires_talent: true, talent_tier: 1, tree: destruction}    # -10s Bane CD per rank
  IMPROVED_SOUL_FIRE:    {requires_talent: true, talent_tier: 2, tree: destruction}
```

Mana floors / leveling brackets: same cross-class convention as healers (0 pre-60; 55/30 BC; 45/25 WotLK; 50/25 Cata).

Drain Tanking (leveling rotation, 1-60):
- Apply Corruption + Bane of Agony + UA (where talent allows)
- Filler Shadow Bolt; switch to Drain Life when self.hp<60 AND target.hp>50
- Drain Soul to finish kills (Soul Shard refund on XP-yielding kills)
- Pet: Voidwalker until ~30 (tank); switch to Felhunter from 30+

---

## 7. Suggested next research passes

1. Demonology sibling spec for max-level raid optimization.
2. Soul Shard reservation policy (Soulstone-reserve verified).
3. Dark Intent target handoff in multi-warlock raids.
4. Soul Swap glyph dependence for raid spread.
5. Doomguard alignment with Bloodlust (opener-snapshot vs deferred).
6. Demon Soul Felhunter +20% multiplicative vs additive with Mastery.
7. Bane of Agony vs Bane of Doom threshold (`ttl < 30s` cited).
8. AoE Soulburn-Seed cadence with Drain Soul shard refund.
9. Curse of the Elements skip predicate verification (8% magic modifier overlap).
10. Pet leashing / re-summon tuning.
11. Shadow Embrace stack maintenance during target switch (Soul Swap likely doesn't carry SE).
12. Drain Soul channel-break behavior.
13. Volcanic Potion vs alternative pre-pots.
14. Soul Harvest out-of-combat shard farm window.

---

## 8. Source-coverage gaps for DBC verification

- Mastery Potent Afflictions formula (13% + 1.63%/pt).
- Haunt damage + heal coefficient + 20% debuff stack rule (additive vs multiplicative with SE).
- Drain Soul ×2 damage at ≤25%.
- Drain Soul shard refund predicate (XP/honor yielding).
- Pandemic refresh predicate.
- Everlasting Affliction rank scaling (33%/66%/100%).
- Bane of Doom Ebon Imp summon rate (20%).
- Eradication 6%/proc.
- Soul Swap Tier 4 confirmed, 18% mana cost.
- Soul Swap Glyph effect (copy not transfer).
- Soulburn 45s/1 shard/15s buff.
- Demon Soul Felhunter +20% periodic.
- Doomguard / Infernal 10-min CD + 45s pet.
- CotE 8% magic modifier vs other class debuffs.
- CotE vs Bane of Doom slot independence verification.
- Shadow Embrace 5%/stack max 3.
- Shadow and Flame +12% at 3/3.
- Glyph effects (Haunt, Corruption, UA, Life Tap, Shadow Bolt, Soul Swap).
- Drain Soul 3-shard refund on XP-yielding kill.
- Soul Harvest 9s channel / 1 shard per 3s / 45s CD.
- Bane of Havoc Destruction Tier-6 confirmation.
- Soulshatter cooldown.
- Fel Flame DoT extension on Immolate/UA.
- Howl of Terror cooldown / 8s fear / 10y AoE.
- Banish PVE duration (30s assumed).

---

## 9. Notes on prompt deviations

1. **Bane of Havoc as Affliction tool** — INCORRECT. Bane of Havoc is Destruction Tier-6 talent. Aff multi-target tools are Soul Swap + Soulburn-Seed. Removed.
2. **Demon Soul (3 min)** — corrected to 2 min.
3. **Imp pet for Aff** — not viable. Imp's Demon Soul effect only buffs Destruction cast-time spells. Felhunter is the canonical Aff pet.
4. **Spirit as primary stat** — corrected. Int is primary; Spirit only via Glyph of Life Tap.
5. **Drain Soul level 28** — corrected to level 10.
6. **31 Aff + 5 secondary** — corrected to 31/0/10.
7. **Bane of Havoc multi-target** — see (1).
8. **Soul Swap as baseline** — corrected. Talent-gated in Cata (Tier 4 Affliction); became baseline in MoP 5.0.4.

End of survey.

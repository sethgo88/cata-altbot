# Marksmanship Hunter 4.3.4 — Multi-Guide Strategy Survey

Audit trail for the strategy decisions that will be encoded in `docs/specs/marksmanship-hunter.md`. The spec doc is the source of truth for the bot; this doc records *which guides agreed*, *where they disagreed*, and *which thresholds were the bot author's judgment call rather than guide consensus*.

Survey conducted 2026-05-02 by the bot author. Numerical data sourced from Wowhead's Cata archive and Warcraft Wiki. Strategy/threshold content reconciled across the strategy-bearing guides below.

This is the **third ranged-DPS survey in the project** (after frost-mage and affliction-warlock). Section 1 below justifies the spec pick — Hunter has three viable specs and the meta-DPS leader (Survival) is *not* the project's pick.

---

## 1. Spec selection — Beast Mastery vs Marksmanship vs Survival (4.3.4 PVE)

### Source consensus on raw raid-DPS ranking (Dragon Soul / Phase 4 / 4.3.4)

| Source | Beast Mastery | Marksmanship | Survival |
|---|---|---|---|
| Icy Veins Cata Classic Tier List (Feb 2025) | **D-tier (#20)** | **D-tier (#21)** | **A-tier (#5)** |
| wowtbc.gg Phase 4 (top-10% WCL parses) | rank #19 | rank #20 | **rank #11** |
| Wowhead Cata Tier 13 DPS rankings | C-tier | C-tier | **A-tier** |
| Aggregated community tier lists | C/D | C/D | **A/S** |

The verdict: **Survival >> Marksmanship ≈ Beast Mastery** in 4.3.4.

### Why Marksmanship is still the project pick

Marksmanship provides the cleanest "ranged caster-style" rotation engine for the bot — a focus-spender priority list with one cooldown-driven proc system (Master Marksman) and one execute-style window (Careful Aim, target HP > 90%). SV's rotation is built around Lock-and-Load proc cascades plus dual DoT maintenance — branching procs-on-procs that pushed Demonology Warlock out of scope.

| Criterion | BM | MM | SV |
|---|---|---|---|
| **Leveling viability** | **Best** (pet 40%+ damage) | Good | Good |
| **Multi-target dungeon trash** | Good | Mid (Multi-Shot only) | **Best** (Trap+Black Arrow) |
| **Pet AI complexity** | Pet 40%+, positioning critical | Pet sidekick | Pet sidekick |
| **Rotation engine complexity** | Mid | **Low-mid** | High (Lock-and-Load cascade) |

**Spec disposition: Marksmanship.** Acknowledged downside: rank #20-21 vs Survival's #5-11. Future option: Survival sibling spec.

---

## 2. Sources

| # | Guide | URL | Confidence |
|---|---|---|---|
| G1 | Icy Veins MM Rotation/CDs | https://www.icy-veins.com/cataclysm-classic/marksmanship-hunter-pve-rotation-cooldowns-abilities | High |
| G2 | Icy Veins MM Talents/Glyphs | https://www.icy-veins.com/cataclysm-classic/marksmanship-hunter-pve-spec-builds-talents-glyphs | Medium-High |
| G3 | Icy Veins MM Stat Priority | https://www.icy-veins.com/cataclysm-classic/marksmanship-hunter-pve-stat-priority | High (8%/961 hit) |
| G4 | Icy Veins MM Leveling | https://www.icy-veins.com/cataclysm-classic/marksmanship-hunter-leveling | Medium |
| G7 | Icy Veins SV Rotation (for §1) | https://www.icy-veins.com/cataclysm-classic/survival-hunter-pve-rotation-cooldowns-abilities | High for §1 |
| G8 | Icy Veins DPS Tier List | https://www.icy-veins.com/cataclysm-classic/dps-rankings-tier-list-for-cataclysm-classic | High for §1 |
| G9 | wowtbc.gg MM Hunter | https://wowtbc.gg/cata/class-guides/marksmanship-hunter/ | Medium |
| G10 | wowtbc.gg DPS Rankings | https://wowtbc.gg/cata/class-rankings/pve-rankings/ | High for §1 |
| G11 | Warcraft Tavern MM Talents | (403'd; reconstructed from search) | Medium |
| G12 | wowvendor MM PvE | https://wowvendor.com/.../cataclysm-classic-marksmanship-hunter-pve-guide/ | Medium |
| G13 | gotwarcraft MM Cataclysm 4.3 | https://gotwarcraft.com/guides/classguides/marksmanhunter.php | Medium-High — explicit 7/31/3 |
| G14 | Tauri-Veins MM PvE DPS | https://www.tauri-veins.com/marksmanship-hunter-wow-pve-dps-guide | Medium |
| G18 | wow-petopia Cata 4.0.6 Hunter Stats | https://forums.wow-petopia.com/viewtopic.php?t=10088 | Medium |
| G19 | Warcraft Wiki per-spell pages | https://warcraft.wiki.gg/wiki/<spell> | High for hard mechanics |
| G20 | Wowhead Cata per-spell pages | https://www.wowhead.com/cata/spell=<id> | High for hard mechanics |
| G21 | wowwiki Mastery: Wild Quiver | https://wowwiki-archive.fandom.com/wiki/Mastery:_Wild_Quiver | Medium |
| G24 | Wowhead Hunter Pet Guide | https://www.wowhead.com/cata/guide/classes/hunter/pets | Medium |

**Caveats:** Multiple sources 403'd direct fetch; pet sub-guides 404'd; Wowhead spell tooltips often strip values. Reconstructed via search where needed.

---

## 3. Strong consensus

| Item | Consensus | Spec disposition |
|---|---|---|
| **MM is C/D-tier in Dragon Soul.** | 5/5 | Documented in §1. |
| **Spec utility:** Misdirection, Feign Death, Hunter's Mark, Tranquilizing Shot, Disengage, Trueshot Aura. | 5/5 | Exposed to command interface. |
| **Single-target priority** — Serpent Sting (apply once) > Improved Steady Shot maintenance > Chimera Shot on CD > instant Aimed Shot ("Fire!" proc) > Kill Shot (≤20%) > Aimed Shot (focus dump, stationary, ≥65 focus) > Arcane Shot (focus dump, on the move) > Steady Shot (filler). | 5/5 | Encoded. |
| **Master Marksman** — Steady Shot 20/40/60% per cast applies stack (max 5, 30s). At 5 stacks → "Fire!" buff: instant + free Aimed Shot. | 5/5 | Tier 5 MM talent; bot fires Aimed Shot on "Fire!" Tier-1. |
| **Improved Steady Shot** — 2x consecutive Steady applies +15% ranged haste 8s. | 5/5 | Maintain 100% uptime. |
| **Chimera Shot** — 50 focus, 10s CD, instant; refreshes Serpent Sting + heals 5% max HP. | 5/5 | Cast on CD if SS applied. |
| **Serpent Sting** — 25 focus, 15s, instant. Apply once per target; Chimera refreshes. | 5/5 | Apply at opener. |
| **Aimed Shot** — 50 focus, 2.9s cast, 132% weapon damage. Hard-cast stationary. | 5/5 | Default focus dump when stationary AND focus ≥ 65. |
| **Kill Shot** — execute, 45y, instant, 10s CD, no focus, requires HP ≤ 20%. | 5/5 | Hard threshold. |
| **Careful Aim (2/2)** — +60% crit on Aimed/Steady/Cobra vs targets > 90% HP. | 5/5 | Bot opens with Aimed Shot spam in Careful Aim window. |
| **Piercing Shots (3/3)** — crit Aimed/Steady/Chimera applies 30% bleed over 8s. | 4/4 | Passive; informs crit valuation. |
| **Mastery: Wild Quiver** — 16.8% base + 2.1%/pt chance for extra ranged shot. | 4/4 | Passive; reforge low-priority. |
| **Rapid Fire** — 5min CD, 15s, +40% ranged haste. | 5/5 | On-CD opener. |
| **Readiness** — 3min CD, resets all Hunter CDs except itself. | 5/5 | Fire after Rapid Fire to reset RF + Chimera + Kill Shot. |
| **Cobra Shot is BM/SV-only** — MM uses Steady Shot (Improved Steady Shot trigger). | 4/4 | MM never casts Cobra Shot. |
| **Steady Shot** — 2s cast (1.5s with ISS), 9 focus generated. | 4/4 | Filler + ISS maintenance + MM stack building. |
| **Focus regen** — 4/sec base, scales with haste. Max 100. | 3/3 | Cap at 100. |
| **Pet — Cat (Ferocity, Roar of Courage 4% Str/Agi) OR Wolf (Furious Howl 320 AP).** | 4/4 | Default Cat; Wolf if no melee; Devilsaur if no Sunder. |
| **Pet talent — Ferocity** for raid PvE. | 4/4 | Fixed Ferocity. |
| **Stat priority** — Agility > Hit (8%/961) > Crit > Haste > Mastery. | 5/5 | Encoded. |
| **Hit cap** — 8% / 961 rating at L85 (5%/641 Draenei). RANGED PHYSICAL hit (NOT 17% spell). | 5/5 | Hard cap. |
| **Glyphs — Prime: Aimed Shot + Kill Shot + Rapid Fire** (single-target) OR + Arcane Shot (high-movement). | 4/5 | Default AS/KS/RF. |
| **Glyphs — Major: Disengage + Misdirection + Trap Launcher.** | 3/3 | Default. |
| **Glyphs — Minor: Feign Death + Revive Pet + Aspect of the Pack.** | 3/3 | Default. |
| **Talent build — 7/31/3** (BM Go for the Throat / One With Nature; 31 MM core; SV Hawk Eye / Improved Serpent Sting). | 4/5 | Default 7/31/3. |
| **Variable talents:** Silencing Shot (1pt; mandatory for interrupt), Trueshot Aura (1pt; skip if redundant), Marked for Death (1pt; multi-target). Pick 3 of 4. | 4/4 | Default Silencing Shot + Trueshot Aura. |
| **Aspect of the Hawk** — default damage aspect. | 4/4 | Default. |
| **Aspect of the Fox** — alternative for movement (cast Steady/Cobra while moving). | 4/4 | Swap on `must_move > 4s`. |
| **AoE rotation:** ISS maintenance → Multi-Shot dump → "Fire!" Aimed Shot → Trap Launcher + Explosive Trap if stationary 8s+. | 4/4 | AoE-mode `enemies_within_8y_of_target >= 3`. |
| **Multi-Shot** — 40 focus, instant, 8y AoE. | 4/4 | Default AoE focus dump. |
| **Trap Launcher** — instant, 1.5s GCD, 15s buff, no focus cost. Allows ranged trap deploy 40y. **Level 75 (verify vs 48).** | 3/4 | AoE pre-place. |
| **Auto Shot** — level 1, free. Hunter shoots while moving in Cata. | 4/4 | Always on. |
| **Hunter's Mark** — level 14, 5min, 0 cost in 4.0.1+, +20 RAP. | 4/4 | Apply once per priority. |
| **Misdirection** — 30s CD, 4s damage-redirect, 30s threat-fade. | 4/4 | Auto-fire on `combat_start AND target.is_boss`. |
| **Concussive Shot** — 5 focus, 5s CD, 50% slow 6s. | 3/3 | PvE on caster trash. |
| **Disengage** — 25s CD (21s talented), backwards leap. | 4/4 | Mobility/escape. |
| **Pre-pot — Potion of the Tol'vir** — +1200 Agility 25s. T-2s + 1-min mark. | 3/3 | Encoded. |
| **Opener:** pre-pot T-2s → Hunter's Mark → Misdirection (tank) → Serpent Sting → hard-cast Aimed Shot → Chimera → Rapid Fire + Call of the Wild → Aimed Shot spam Careful Aim → Readiness → second burst. | 4/4 | Encoded. |
| **Movement handling** — swap to Aspect of the Fox + Steady Shot during scripted move; Arcane Shot for off-movement focus dump. | 4/4 | Bot swaps on `must_move > 4s`. |
| **Pet** — Tame Beast lvl 10. | 4/4 | Auto-tame Cat/Wolf at 10+. |

---

## 4. Disagreements

### 4.1. Talent build — 7/31/3 vs 3/32/6 vs Icy-Veins ambiguous

**Spec disposition: 7/31/3.** Consensus across G11/G13/G14.

### 4.2. Pet — Cat vs Wolf vs Devilsaur

**Spec disposition: Cat default; Wolf if no melee; Devilsaur if no Sunder.**

### 4.3. Glyph of Rapid Fire vs Glyph of Arcane Shot (Prime)

**Spec disposition: AS + KS + RF default; auto-swap RF → Arcane Shot if movement >30% of fight.**

### 4.4. Aimed Shot vs Arcane Shot focus dump threshold

**Spec disposition: Aimed Shot when stationary AND focus ≥ 65 AND Careful Aim window OR MM stack opportunity. Arcane Shot when on move OR target HP < 90% AND focus ≥ 50.**

### 4.5. Stat Mastery position

**Spec disposition: Agi > Hit > Crit > Haste > Mastery.** Wild Quiver is "one of the weakest masteries."

### 4.6. Pet Ferocity vs Cunning

**Spec disposition: Ferocity** for raid (Call of the Wild macro with Rapid Fire).

### 4.7. Trueshot Aura vs Frenzy

**Spec disposition: Default Trueshot Aura ON.** Frenzy swap only if raid has redundant +10% RAP.

### 4.8. Multi-Shot threshold

**Spec disposition: AoE-mode at `enemies_within_8y_of_target >= 3`.**

### 4.9. Trap Launcher level — 48 vs 75

**Spec disposition: Level 75 per prompt; flagged for DBC verify.**

### 4.10. Volley REMOVED in 4.0.1

**Spec disposition: Bot must NEVER cast Volley.**

### 4.11. Improved Steady Shot stacks

**Spec disposition: Tracked via `steady_shot_consecutive_count`; refresh ISS at 2.**

---

## 5. Items NOT addressed (author judgment)

| Spec item | Value | Why |
|---|---|---|
| "Fire!" buff use threshold | use immediately if target HP > 20% | Don't waste free Aimed Shot. |
| AoE-mode entry | enemies_within_8y_of_target >= 3 | Multi-Shot 8y radius match. |
| Pre-cast Serpent Sting on opener | YES if target.distance>30y AND boss | Establish DoT before pull. |
| Self-emergency Feign Death | self.threat_pct>90 OR self.hp<25 | Threat dump. |
| Disengage | melee_count>=2 AND self.hp<60 | Escape. |
| Deterrence | self.hp<35 AND no_other_save | 5s parry-everything. |
| Tranquilizing Shot | enemy_buff_dispellable AND CD ready | Auto-purge enrage/magic. |
| Misdirection auto | combat_start AND target.is_boss AND tank_present | Threat redirect. |
| Mend Pet | pet.hp<60 AND pet.in_combat | Maintain pet uptime. |
| Roar of Sacrifice (pet talent) | self.threat>95 AND raid_AoE | Threat dump. |
| Pet positioning | melee range of target | Default. |
| Volley | NEVER cast (removed 4.0.1) | Sanity. |
| Cobra Shot | NEVER cast for MM | Doesn't trigger ISS or MM. |
| Aspect-dancing | scripted_move>4s → Hawk → Fox; on stop → Fox → Hawk | Avoid GCD churn. |
| Pre-pot timing | T-2s + 1-min mark | Bot-side. |
| Big-3 alignment | RF + CotW + pre-pot opener; Readiness 30-45s in | Bot-side. |
| Hunter is FOCUS-only | mana brackets dropped | Cross-class cleanup. |

---

## 6. Author-judgment items — leveling

```yaml
spell_unlocks:
  AUTO_SHOT:             {level: 1,  spell_id: 75}
  TAME_BEAST:            {level: 10, spell_id: 1515}
  CALL_PET:              {level: 1,  spell_id: 883}      # starter from lvl 1 in Cata
  REVIVE_PET:            {level: 10, spell_id: 982}
  SERPENT_STING:         {level: 4,  spell_id: 1978}     # 25 focus, 15s DoT
  ARCANE_SHOT:           {level: 6,  spell_id: 3044}     # 25 focus, instant
  STEADY_SHOT:           {level: 7,  spell_id: 56641}    # 9 focus generated
  CONCUSSIVE_SHOT:       {level: 8,  spell_id: 5116}
  AIMED_SHOT:            {level: 10, spell_id: 19434}    # MM-only baseline at 10
  ASPECT_OF_THE_HAWK:    {level: 12, spell_id: 13165}
  HUNTERS_MARK:          {level: 14, spell_id: 1130}     # 0 cost in 4.0.1+
  ASPECT_OF_THE_PACK:    {level: 14, spell_id: 13159}
  ASPECT_OF_THE_CHEETAH: {level: 16, spell_id: 5118}
  MULTI_SHOT:            {level: 18, spell_id: 2643}     # 8y AoE
  DETERRENCE:            {level: 20, spell_id: 19263}    # 2-min CD, 5s
  ICE_TRAP:              {level: 20, spell_id: 13809}
  RAPID_FIRE:            {level: 26, spell_id: 3045}
  FROST_TRAP:            {level: 28, spell_id: 13809}
  FEIGN_DEATH:           {level: 30, spell_id: 5384}
  DISENGAGE:             {level: 32, spell_id: 781}
  KILL_SHOT:             {level: 35, spell_id: 53351}    # ≤20% HP
  TRANQUILIZING_SHOT:    {level: 38, spell_id: 19801}
  MISDIRECTION:          {level: 38, spell_id: 34477}
  EXPLOSIVE_TRAP:        {level: 40, spell_id: 13813}
  SNAKE_TRAP:            {level: 50, spell_id: 34600}
  CHIMERA_SHOT:          {level: 51, spell_id: 53209}    # MM talent
  READINESS:             {level: 60, spell_id: 23989}
  ASPECT_OF_THE_FOX:     {level: 64, spell_id: 82661}    # cast-while-moving
  TRAP_LAUNCHER:         {level: 75, spell_id: 77769}    # verify in DBC
  COBRA_SHOT:            {level: 81, spell_id: 77767}    # NEVER USE for MM
  # Talent-gated:
  CALL_OF_THE_WILD:      {requires_pet_talent: true, pet_spec: ferocity}
  TRUESHOT_AURA:         {requires_talent: true, tree: marksmanship}
  SILENCING_SHOT:        {requires_talent: true, tree: marksmanship}
  CAREFUL_AIM:           {requires_talent: true, tree: marksmanship}    # +60% crit on >90% HP
  IMPROVED_STEADY_SHOT:  {requires_talent: true, tree: marksmanship}    # 15% haste 8s on 2x Steady
  PIERCING_SHOTS:        {requires_talent: true, tree: marksmanship}    # 30% bleed
  MASTER_MARKSMAN:       {requires_talent: true, tree: marksmanship}    # 5-stack → Fire!
  GO_FOR_THE_THROAT:     {requires_talent: true, tree: beast_mastery}
  ONE_WITH_NATURE:       {requires_talent: true, tree: beast_mastery}
  HAWK_EYE:              {requires_talent: true, tree: survival}        # +6y range
  IMPROVED_SERPENT_STING:{requires_talent: true, tree: survival}
  WILD_QUIVER:           {passive_mastery: true, base_pct: 16.8, per_point: 2.1}
```

**Hunter is FOCUS-only since 4.0.1** — mana brackets do NOT apply. In-combat focus management:
- focus_pct < 25 → Steady Shot only (refill)
- focus_pct ≥ 50 AND < 65 AND not Careful Aim → Arcane Shot
- focus_pct ≥ 65 OR Careful Aim → Aimed Shot

---

## 7. Suggested next research passes

1. Survival sibling spec for max-level raid optimization.
2. Trap Launcher level confirmation (48 vs 75).
3. Master Marksman proc rate per Steady Shot per rank.
4. "Fire!" damage modifier (+50% claimed, tooltip silent).
5. Cobra/Steady interleave for BM/SV docs.
6. Wild Quiver proc on auto-shot vs special shots.
7. ISS buff consume-vs-refresh.
8. Ferocity Call of the Wild macro with Rapid Fire.
9. Devilsaur Terrifying Roar uptime.
10. Pet hit rating coverage from Hunter's hit rating.
11. Misdirection glyph cooldown reset rule.
12. Aimed Shot cast time scaling (additive vs multiplicative).
13. Readiness reset of Rapid Fire (full reset confirm).
14. Pre-pot snapshot for Serpent Sting / Piercing Shots.
15. Aspect of the Fox party-wide vs self-only regen.

---

## 8. DBC verification

- Master Marksman 20/40/60% proc rate / rank.
- "Fire!" damage modifier (+50%).
- "Ready, Set, Aim..." 30s stack duration / 5-stack max.
- Improved Steady Shot 8s buff / 15% haste.
- Wild Quiver 16.8% + 2.1%/pt formula.
- Aimed Shot 132% weapon damage + RAP coefficient.
- Aimed Shot! (instant) damage parity.
- Steady Shot 9 focus generation.
- Cobra Shot 9 focus + Serpent Sting +6s.
- Kill Shot ≤20% threshold.
- Glyph of Kill Shot 6s second-cast.
- Chimera Shot 5% max HP heal.
- Serpent Sting 25 focus.
- Rapid Fire 40% haste.
- Readiness full-reset confirm.
- Trap Launcher level (48 vs 75).
- Disengage 25s/21s/16s with talent + glyph.
- Misdirection 4s redirect / 30s fade.
- Hunter's Mark 0 cost / 5min / 20 RAP.
- Careful Aim >90% threshold.
- Piercing Shots 30% bleed 8s refresh-or-stack.
- Aspect of the Hawk RAP value.
- Aspect of the Fox party-wide vs self.
- Trueshot Aura 10% RAP.
- Ferocity Call of the Wild 10% AP / 20s / 5min.
- Pet hit rating coverage.
- Volley REMOVED confirmation.
- Multi-Shot 40 focus / 8y / 85% bonus weapon.
- Glyph effects (AS, KS, RF, Arcane, Steady, Chimera, Disengage, Misdirection, Trap Launcher, Feign Death, Revive Pet).

---

## 9. Notes on prompt deviations

1. **Steady vs Cobra** — RESOLVED: Steady for MM. Cobra is BM/SV-only.
2. **Bestial Wrath equivalent for MM?** — None. MM's burst is Rapid Fire + Readiness + Call of the Wild.
3. **Hit cap (17% spell?)** — corrected to 8%/961 ranged physical.
4. **Master Marksman +50% damage** — instant + free confirmed; +50% modifier flagged DBC.
5. **31 MM + 5 secondary** — corrected to 7/31/3.
6. **Trap Launcher level 75** — used; flagged DBC.
7. **Cobra Shot for MM** — EXCLUDED.
8. **Aspect of the Fox** — added (movement aspect).
9. **Volley** — REMOVED 4.0.1; bot must never cast.
10. **Hunter resource is FOCUS** — mana brackets dropped.
11. **Minimum range** — confirmed REMOVED in 4.0.1.

End of survey.

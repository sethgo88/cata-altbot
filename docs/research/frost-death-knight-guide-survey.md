# Frost Death Knight 4.3.4 — Multi-Guide Strategy Survey

Audit trail for `docs/specs/frost-death-knight.md`. Survey conducted 2026-05-02. This is the **fourth melee-DPS survey** (after Combat Rogue, Fury Warrior, Retribution Paladin). DK has 3 specs (Blood / Frost / Unholy); Blood is tank (deferred). DPS spec-selection is **Frost vs Unholy** — both viable, both seen in Dragon Soul; backlog locks **Frost** for primary implementation.

Cross-reference: Death Knights have **no class peer already documented** — this is the first DK doc in the project. Resource model (Runes + Runic Power), disease system (Frost Fever / Blood Plague), and presence system (Blood / Frost / Unholy Presence) are all new and must be encoded fresh. Cataclysm 4.0 reworked the rune system from "10 rune cooldowns" (Wrath) to "6-rune pool with 10s individual recharge" — Wrath-era guides are misleading.

---

## 1. Spec selection

**Spec disposition: Frost (2H).** Both Frost and Unholy clear Dragon Soul at competitive numbers (within ~5% on most fights, with fight-specific swings). Frost is chosen because:

1. **Bot-control simplicity.** Frost's priority list has fewer interlocking timers — Unholy juggles Festering Strike disease-pandemic windows, ghoul pet control, Sudden Doom procs, Dark Transformation timing, and Unholy Frenzy alignment; Frost mostly tracks Killing Machine + Rime procs over a flat Obliterate/Frost-Strike/Howling-Blast priority.
2. **No pet management.** Frost's only pet is a 1-min temporary Ghoul (Raise Dead, off the GCD); Unholy's permanent ghoul is a meaningful chunk of damage and requires pet-AI plumbing.
3. **2H vs DW choice is consensus 2H.** Threat of Thassarian (talent) makes DW Frost theoretically competitive, but 2H is the consensus pick on 4.3.4 due to Might of the Frozen Wastes's +20% 2H damage and weapon-quality scaling on Dragon Soul-tier 2H weapons. **2H Frost** is the disposition.

Within Frost: **2H Frost (Might of the Frozen Wastes)** over **DW Frost (Threat of Thassarian)**. The two builds share ~90% of the priority; the DW variant only changes weapon-spec talent + glyph (Glyph of Frost Strike for DW vs Glyph of Howling Blast for 2H is encounter-tunable, not spec-defining).

---

## 2. Sources

| # | Guide | URL | Confidence |
|---|---|---|---|
| G1 | Icy Veins Frost DK Rotation/CDs | https://www.icy-veins.com/cataclysm-classic/frost-death-knight-pve-rotation-cooldowns-abilities | High |
| G2 | Icy Veins Frost DK Talents/Glyphs | https://www.icy-veins.com/cataclysm-classic/frost-death-knight-pve-spec-builds-talents-glyphs | High |
| G3 | Icy Veins Frost DK Stat Priority | https://www.icy-veins.com/cataclysm-classic/frost-death-knight-pve-stat-priority | High |
| G4 | Wowhead Cata Frost DK | https://www.wowhead.com/cata/guide/classes/death-knight/frost/dps-overview-pve | Medium-High |
| G5 | wowtbc.gg Frost DK | https://wowtbc.gg/cata/class-guides/frost-death-knight/ | Medium |
| G6 | Warcraft Tavern Frost DK | https://www.warcrafttavern.com/cataclysm/guides/pve-frost-death-knight/ | Medium-High |
| G7 | Method Cata Frost DK | https://www.method.gg/guides/cataclysm/frost-death-knight | Medium |
| G8 | Noxxic Frost DK 4.3 | https://www.noxxic.com/wow/pve/death-knight/frost/ | Medium |
| G9 | (Spec selection) Icy Veins Unholy DK | https://www.icy-veins.com/cataclysm-classic/unholy-death-knight-pve-rotation-cooldowns-abilities | High (cross-ref only) |
| G10 | (Spec selection) Method Cata DK class hub | https://www.method.gg/guides/cataclysm/death-knight | Medium |
| G15 | Warcraft Wiki per-spell | https://warcraft.wiki.gg/wiki/<spell> | High |
| G16 | Wowhead Cata per-spell | https://www.wowhead.com/cata/spell=<id> | High |

---

## 3. Strong consensus

| Item | Consensus | Spec disposition |
|---|---|---|
| **DK resource model** — 6 runes (2 Blood / 2 Frost / 2 Unholy) + 0–100 Runic Power. | 8/8 | Encoded. |
| **Rune recharge** — each rune regenerates independently in ~10s (modified by Haste + Runic Focus passive). | 8/8 | Encoded. |
| **Death Runes** — runes converted to "any color" via talents/abilities; consume as Blood/Frost/Unholy. | 8/8 | Encoded. |
| **Frost Presence** — DPS presence; +10% Stamina, +60% threat reduction, +5% physical damage on melee crit (Cata change). | 8/8 | Default presence. |
| **2H weapon (Frost build)** — Might of the Frozen Wastes +20% melee dmg with 2H. | 7/8 | Hard requirement. |
| **Plate Specialization** — +5% Strength when fully plate-armored. | 8/8 | Encoded. |
| **Strength = primary stat** — converts to AP at 2:1 (DK Strength = AP via Forceful Deflection / class scaling). | 8/8 | Encoded. |
| **Obliterate** — primary builder; 1 Frost + 1 Unholy rune; bonus damage with diseases up. | 8/8 | High priority. |
| **Frost Strike** — Runic Power spender; 32 RP; instant. | 8/8 | Primary RP dump. |
| **Howling Blast** — 1 Frost rune; AoE; applies Frost Fever; primary disease applicator in Frost. | 8/8 | Maintained. |
| **Frost Fever** — disease applied by Howling Blast / Icy Touch; 21s baseline duration. | 8/8 | Maintained. |
| **Blood Plague** — disease applied by Plague Strike; 21s baseline duration; in Frost, applied via Outbreak only. | 8/8 | Maintained. |
| **Outbreak** — Cata-new (L81); instant, no rune cost, applies BOTH diseases; 1min CD. | 8/8 | Pull-opener. |
| **Killing Machine (KM)** — Frost talent; melee/ranged auto-attack proc → next Frost Strike OR Obliterate auto-crits. | 8/8 | Reactive priority. |
| **Rime** — Frost talent; Obliterate proc → next Howling Blast costs no rune AND no RP cost. | 8/8 | Reactive priority. |
| **Pillar of Frost** — 1min CD, Frost talent, +20% Strength for 20s. | 8/8 | Major DPS CD. |
| **Empower Rune Weapon** — 5min CD, instantly refreshes all 6 runes + grants 25 RP. | 8/8 | Major CD. |
| **Mind Freeze** — 10s CD, off-GCD, 4s lockout interrupt. | 8/8 | Auto-interrupt. |
| **Death Grip** — 25s CD, 30y ranged pull / forced taunt. | 8/8 | Utility / pull. |
| **Anti-Magic Shell (AMS)** — 45s CD, 5s, absorbs 75% magic + grants RP from absorbed. | 8/8 | Defensive + RP gen. |
| **Anti-Magic Zone (AMZ)** — 2min CD, raid AoE magic absorb (talent-gated; baseline in Cata). | 6/8 | Raid utility. |
| **Icebound Fortitude** — 2min CD (1min glyphed), -20% damage taken, 12s. | 8/8 | Defensive CD. |
| **Death Strike** — 1 Frost + 1 Unholy rune; self-heal (Cata: heals based on damage taken); off-rotation in Frost DPS. | 8/8 | Defensive / off-rotation. |
| **Horn of Winter** — 2min raid buff; +Strength + Agility; also generates 10 RP when cast. | 8/8 | Buff + RP filler. |
| **Death's Advance** — Cata-new passive; -50% movement-impair + +10% movement (talent-modified). | 7/8 | Passive movement. |
| **Mastery: Frozen Heart** — increases all Frost-school damage (Obliterate physical + Frost portion, Frost Strike, Howling Blast, Frost Fever, Icy Touch). | 8/8 | Reforge target. |
| **Talent build** — 31 Frost / 7 Blood / 3 Unholy (variants on filler points). | 7/8 | Default. |
| **Hit cap** — 8% melee (961 rating @ L85). | 8/8 | Hard. |
| **Spell Hit cap** — 17% (1742 rating) for spells but DK rarely reforges to spell hit; 8% melee gives ~2.5% spell hit. **Diseases / Howling Blast / Frost Strike are treated as melee abilities and use melee hit table.** | 6/8 | Melee hit table. |
| **Expertise cap** — 26 (~781 rating). | 8/8 | Hard. |
| **Stat priority** — Hit (8%) → Expertise (26) → Strength → Mastery > Crit > Haste. | 7/8 | Encoded. |
| **Glyphs Prime** — Obliterate + Frost Strike + Howling Blast (2H build). | 7/8 | Default 2H. |
| **Glyphs Major** — Pillar of Frost + Anti-Magic Shell + Death's Advance (or Death Grip). | 6/8 | Default. |
| **Raise Dead** — 3min CD baseline (1min glyphed); summons temporary ghoul; off-GCD. | 8/8 | On CD. |
| **Death Coil** — 32 RP; primary RP dump for Unholy; **NOT used in Frost rotation** (Frost Strike is better). | 8/8 | Skip in Frost. |
| **Path of Frost** — utility water-walking; out-of-combat. | 8/8 | Utility. |
| **Strangulate** — 2min CD ranged silence (5s); off-GCD. | 8/8 | Ranged interrupt. |
| **Chains of Ice** — 1 Frost rune; ranged snare (slow); kiting. | 7/8 | Utility. |
| **Hungering Cold** — talent; 1min CD, 10s AoE freeze (breaks on damage). | 5/8 | Talent CC. |
| **Pre-pot Golemblood** — +1200 Strength 25s; T-2s + 1-min mark. | 6/8 | Encoded. |
| **Opener** — Horn of Winter pre-pull → Outbreak → Plague Strike (skip if Outbreak up) → Pillar of Frost + Empower Rune Weapon + Raise Dead + Golemblood → Howling Blast (Rime) → Obliterate ×2 → Frost Strike → Howling Blast → ... | 5/8 | Encoded. |

---

## 4. Disagreements

### 4.1. 2H Frost vs DW Frost

**Spec disposition: 2H Frost (Might of the Frozen Wastes).** DW Frost via Threat of Thassarian is within ~3% on patchwerk-style fights but loses on cleave (off-hand Howling Blast / Frost Strike diminishment) and is more gear-dependent (needs two high-iLvl 1H weapons). 2H scales cleanly with the 2H weapon drops in Dragon Soul. Bot-control: 2H removes off-hand-weapon equip-state checks.

### 4.2. Talent split — 31/7/3 vs 31/5/5 vs 31/8/2

**Spec disposition: 31/7/3 (Frost / Blood / Unholy).**
- Frost 31: full tree including Killing Machine, Rime, Pillar of Frost, Might of the Frozen Wastes, Threat of Thassarian (taken regardless because it grants free Death Strike off-hand benefit + RP from Frost Strike), Howling Blast capstone access path, Brittle Bones, Endless Winter, Chill of the Grave, Annihilation, Merciless Combat.
- Blood 7: **Butchery 2** (RP on auto-attack) + **Scent of Blood 3** (RP on damage taken — even 1pt is fine on PvE) + **Improved Blood Tap 2** OR **Toughness 2** (Toughness preferred for raid survivability).
- Unholy 3: **Virulence 3** (+9% disease damage + +9% disease hit, helps soft-cap spell hit on diseases AND Howling Blast).

### 4.3. Killing Machine: spend on Frost Strike or Obliterate?

**Spec disposition: spend on Frost Strike unless Obliterate has 0 risk of overcapping a Killing Machine + Rune drift.** Frost Strike is RP-only (no rune cost), so consuming KM with Frost Strike doesn't deplete the Frost-rune pool that Obliterate needs. If both Frost runes are also off cooldown, Obliterate can take it to compress damage into the Pillar of Frost window. Default: **Frost Strike consumes KM.**

### 4.4. Rime consumption priority

**Spec disposition: consume Rime as soon as it procs IF Frost Fever is the only disease that needs refresh OR Frost Fever is about to fall AND Howling Blast in Frost spec applies it.** Rime Howling Blast is free + refreshes Frost Fever. Don't sit on Rime — proc overwrites if a second Obliterate procs another Rime.

### 4.5. Disease pandemic / refresh window

**Spec disposition: refresh diseases when remaining duration ≤ 3s.** Cata supports "snapshotting" — diseases inherit current Strength/Mastery on apply, so refreshing during Pillar of Frost is a measurable gain. If Pillar of Frost is up and disease has ≤8s remaining, refresh now to snapshot the buff. Otherwise the 3s pandemic window keeps RP/rune economy clean.

### 4.6. Howling Blast spam vs Obliterate priority

**Spec disposition: Single-target — Obliterate > Howling Blast (Rime) > Frost Strike > Howling Blast (no Rime, only to refresh Frost Fever or dump Frost runes that would overcap).** AoE 3+ targets — Howling Blast > Blood Boil (DnD-buffed) > Obliterate-on-primary > Frost Strike. Howling Blast becomes higher priority than Obliterate at 3+ targets due to Glyph of Howling Blast (+disease hit on full radius).

### 4.7. Howling Blast spec-glyph interaction

**Spec disposition: Glyph of Howling Blast = Prime slot.** Causes Howling Blast to also apply Frost Fever (it does this baseline) AND in Cata 4.x the glyph adds critical-strike chance on Howling Blast. **UNVERIFIED** whether it's +20% crit or +damage; both are claimed across guides. Encode as "glyph slot, effect TBD via DBC."

### 4.8. Empower Rune Weapon timing

**Spec disposition: align with Pillar of Frost on first use; thereafter use on CD whenever it would not waste >2 runes (i.e. cast when at least 4 runes are on cooldown).** Empower Rune Weapon is also a "panic button" if RP is empty and runes are full — though Frost rarely starves.

### 4.9. Pillar of Frost alignment

**Spec disposition: on CD (1 min); first use stacked with Empower Rune Weapon + Raise Dead + Golemblood at the pull.** Subsequent uses drift with cooldown; align with on-use trinkets and Bloodlust where possible (Bloodlust at pull is standard; subsequent Pillar drifts off Bloodlust).

### 4.10. Raise Dead in Frost rotation

**Spec disposition: on CD (1min with Glyph of Raise Dead) as DPS contribution.** The temporary ghoul deals modest damage. Glyph of Raise Dead is **NOT a default Major glyph** in our build (we prioritize Pillar of Frost / AMS / Death's Advance), so use is on the **3min** CD baseline aligned with Pillar of Frost.

### 4.11. Horn of Winter usage

**Spec disposition: pre-pull cast (raid buff lasts 2 min) AND on CD as RP filler when no other action available.** Horn of Winter is off-GCD only with **Forceful Deflection** talent (it's NOT off-GCD baseline — it's on-GCD, 1.5s, generating 10 RP). Cast as filler when no rune is available AND you're below ~80 RP. Avoid casting when Frost Strike is castable (Frost Strike beats Horn for damage).

### 4.12. Death Strike in Frost DPS

**Spec disposition: emergency self-heal only.** Frost DPS does NOT use Death Strike for damage — Obliterate scales better with Frost spec talents. Bot may cast Death Strike when self HP < 35% AND 1 Frost + 1 Unholy runes available, to apply Blood Shield + heal-from-damage-taken (~5–10% HP).

### 4.13. Anti-Magic Shell rotational use

**Spec disposition: defensive primary, RP-filler secondary.** AMS absorbs magic damage AND converts absorbed magic to Runic Power (capped at 50 RP per cast). Cast pre-emptively before known incoming magic damage (Hagara lightning storm, Yor'sahj Black Slime, etc.). On bosses with predictable raid-wide magic ticks, use on CD for the RP gain.

### 4.14. Hungering Cold

**Spec disposition: skip in single-target raid; talent-flexible for AoE / trash.** Single-target boss fights don't need Hungering Cold; the 5 talent points are better spent in Blood (Toughness) or Unholy (Virulence). Encode as off by default; flag for AoE-trash builds.

### 4.15. Glyph of Death Grip vs Glyph of Death's Advance

**Spec disposition: Death's Advance (default).** Death's Advance reduces the cooldown reduction on the Death's Advance passive, granting more frequent move-impair immunity windows — useful on every fight. Glyph of Death Grip removes the cooldown when targeted enemy dies — situational, only matters on heavy add-spawn fights.

### 4.16. Major Glyphs split

**Spec disposition: Pillar of Frost + Anti-Magic Shell + Death's Advance.**
- Pillar of Frost: prevents knockback during Pillar (stat preservation).
- Anti-Magic Shell: +2s duration on AMS.
- Death's Advance: lower CD on the passive.
**Alternatives** (encounter-tunable): Glyph of Icebound Fortitude (-50% CD), Glyph of Mind Freeze (-1s CD), Glyph of Resilient Grip (Death Grip refund on immune target).

### 4.17. Minor Glyphs

**Spec disposition: Path of Frost + Death's Embrace + Resilient Grip.** Cosmetic / utility — bot-side can ignore.

### 4.18. Stat: Mastery vs Crit vs Haste

**Spec disposition: Mastery > Crit > Haste.** Mastery: Frozen Heart scales every Frost-school hit (Frost Strike, Howling Blast, Frost Fever, Frost portion of Obliterate). Crit synergizes with Killing Machine ICD (KM is melee/ranged auto-attack; crit does NOT proc KM directly but Annihilation gives crit-bonus on Frost Strike/Obliterate). Haste contributes rune-recharge AND auto-attack speed but is weakest at current iLvls. **Reforge order: Haste → Mastery; Crit → Mastery once Mastery>Crit threshold reached.**

### 4.19. Pre-pot + combat-pot

**Spec disposition: Golemblood T-2s pre-pull AND second Golemblood at the 1-min mark when Pillar of Frost + Empower Rune Weapon align.** Volcanic Potion is NOT used (Frost is melee physical; Volcanic is intellect — irrelevant).

### 4.20. Opener sequence

**Spec disposition (ST):**
1. **T-15s** — Horn of Winter (pre-buff).
2. **T-2s** — Golemblood Potion.
3. **T-0** — Outbreak (instant, both diseases up immediately).
4. **T-0+GCD** — Pillar of Frost + Empower Rune Weapon (both off-GCD) + Raise Dead (off-GCD).
5. **T+~1.5s** — Howling Blast (free Rime if Obliterate hasn't procced one — typically NOT yet; this Howling Blast is paid, and is a cost-of-business if no Rime; in raid, often skip this slot in favor of straight Obliterate).
6. **T+~3s** — Obliterate (1 Frost + 1 Unholy rune).
7. **T+~4.5s** — Obliterate (consumes the second Frost + Unholy).
8. **T+~6s** — Frost Strike (KM proc likely from auto-attack crit by this point).
9. **T+~7.5s** — Howling Blast (Rime proc from one of the Obliterates, free).
10. **Continue priority** — Obliterate as runes come up → Frost Strike with KM → Frost Strike to dump RP at >85 → Howling Blast on Rime → maintain diseases.

### 4.21. ST rotation order

**Spec disposition (priority list):**
1. **Diseases falling within 3s** — Outbreak (if off CD) OR Howling Blast (Frost Fever) + Plague Strike (Blood Plague, if Outbreak on CD). **Note:** in Frost, Plague Strike is rare — usually Outbreak handles both, and Howling Blast keeps Frost Fever up; Blood Plague refreshes via Outbreak only.
2. **Killing Machine proc up** — Frost Strike (default) OR Obliterate (if all Frost runes available AND Pillar of Frost active).
3. **Rime proc up** — Howling Blast (free).
4. **Obliterate** — 1 Frost + 1 Unholy rune available.
5. **Frost Strike** — at ≥ 88 RP (cap at 100; spend before overcap).
6. **Howling Blast** (no Rime) — if both Frost runes are up AND Obliterate isn't usable (Unholy runes on CD).
7. **Frost Strike** — at any RP ≥ 32 (default RP dump).
8. **Horn of Winter** — filler when no rune AND RP < 50.

### 4.22. AoE rotation (3+ enemies)

**Spec disposition:**
1. Diseases on primary (Outbreak) → spread via Pestilence (1 Blood rune; spreads to all infected within 10y of primary target).
2. **Howling Blast** — primary AoE generator; hits all in 10y.
3. **Blood Boil** — 1 Blood rune; AoE damage on diseased targets; replaces Pestilence after spread is done.
4. **Frost Strike** — RP dump (single-target only — Frost Strike does not cleave).
5. **Death and Decay (DnD)** — 1 Blood + 1 Frost + 1 Unholy rune; ground AoE 30s CD; high priority on stationary AoE pulls; **buffs Blood Boil damage on targets standing in DnD via talent (Morbidity / Crimson Scourge — Crimson Scourge is Blood-spec-only; Frost gets DnD as raw AoE).**

### 4.23. Cleave 2 enemies

**Spec disposition: ST priority on primary; let Howling Blast / Blood Boil / DnD provide passive AoE.** Don't switch to AoE-mode rotation until 3+ targets.

### 4.24. Multi-target threat / off-tank

**Spec disposition: Death Grip on adds + Dark Command (taunt analog) only if specced for tanking — Frost is NOT a tank spec.** Frost DK has no taunt (Death Grip is a single-target forced taunt-pull, but doesn't sustain threat); off-tanking is Blood-spec territory. Frost's role on adds is high-AoE damage via Howling Blast.

### 4.25. Movement / disengage

**Spec disposition: Death's Advance passive + Path of Frost out-of-combat + Death Grip for gap-close.** No gap-closer beyond Death Grip; on movement-heavy fights, instant-cast Howling Blast / Frost Strike / Outbreak are the workhorses; pre-cast Frost Strike before required movement.

### 4.26. Self-heal / survival

**Spec disposition: Death Strike emergency only (HP < 35%); Death Pact (talent — Unholy-tree, NOT taken in Frost); Lichborne (talent — converts to Undead, allows self-target heal; Frost takes Lichborne if available in build but it's typically Unholy-tree).** Frost has fewer self-heal options than Blood/Unholy; relies on healers.

### 4.27. T13 set bonuses (Frost)

**Spec disposition: 4-piece T13 = priority set goal.**
- **2P:** Obliterate / Scourge Strike critical strikes have a chance to grant +X RP. (Frost: Obliterate; Unholy: Scourge Strike.)
- **4P:** Pillar of Frost / Unholy Frenzy duration extended OR cooldown reduced — **UNVERIFIED** which exactly; both have been reported.
Goal: 4P T13 Frost set; downgrade to 2P if better off-set tier-bonus stats are blocking gear.

### 4.28. Trinket pairing

**Spec disposition: any on-use Strength trinket aligned with Pillar of Frost.** Wrath of Unchaining, Vessel of Acceleration (passive), etc. Bot-side: trinket-firing logic should align with Pillar of Frost on-use.

### 4.29. Bloodlust / Heroism / Time Warp alignment

**Spec disposition: pre-pull cast by raid (default); Frost benefits significantly from Haste during Bloodlust (rune recharge speeds up).** No spec-side action — bot consumes whatever buff timing the raid provides.

---

## 5. Items NOT addressed (author judgment)

| Spec item | Value | Why |
|---|---|---|
| Frost Presence | always; cast on log-in / after death | Always-on. |
| Disease maintenance | refresh ≤ 3s remaining | Always-on. |
| Outbreak | on CD (1min); pre-pull + on-CD reapply | Default. |
| Howling Blast | Rime proc OR disease refresh OR AoE 3+ | Conditional. |
| Plague Strike | rare in Frost — only when Outbreak on CD AND Blood Plague < 3s | Conditional. |
| Obliterate | 1F + 1U runes available AND no higher-priority proc | Default. |
| Frost Strike | Killing Machine OR ≥ 88 RP OR ≥ 32 RP filler | Layered priority. |
| Killing Machine | Frost Strike default; Obliterate if Pillar of Frost + 2F runes ready | Reactive. |
| Rime | Howling Blast immediately | Reactive. |
| Pillar of Frost | on CD (1min); first use stacked w/ EmpRW + Raise Dead + Golemblood | CD. |
| Empower Rune Weapon | first use w/ Pillar; subsequent on CD when ≥ 4 runes off CD | CD. |
| Raise Dead | on CD (3min default; 1min glyphed) | CD. |
| Horn of Winter | pre-pull + filler (no rune, RP < 50) | Buff + filler. |
| Death Strike | self HP < 35% AND 1F + 1U runes | Defensive. |
| Anti-Magic Shell | pre-empt known magic damage OR on CD for RP gain | Defensive + RP. |
| Anti-Magic Zone | raid-magic burst window (pre-cast) | Raid utility. |
| Icebound Fortitude | self HP < 40% AND no other CD up | Defensive CD. |
| Lichborne | self-CC break (charm / fear / sleep) | Reactive. |
| Death Pact | NOT taken in Frost (Unholy talent) | N/A. |
| Mind Freeze | enemy cast ≤ 300ms before completion + melee range | Reactive. |
| Strangulate | enemy cast at ranged (>5y) + melee CD up | Reactive. |
| Death Grip | gap-close OR force-pull add OR threat reset | Utility. |
| Chains of Ice | enemy fleeing OR add-snare | Utility. |
| Hungering Cold | NOT taken in default Frost build | N/A. |
| Pestilence | AoE 3+ AND diseases on primary | AoE. |
| Blood Boil | AoE 3+ AND diseases on targets | AoE. |
| Death and Decay | AoE 3+ AND stationary (or large radius worth it) | AoE. |
| Death Coil | NOT used in Frost (RP goes to Frost Strike) | N/A. |
| Path of Frost | utility / out-of-combat | Utility. |
| Pre-pot Golemblood | T-2s + 1-min mark | Bot-side. |
| Stat reforge | Crit/Haste → Mastery; Hit/Exp keep | Standard. |
| Glyph of Howling Blast | Prime slot — disease-application crit | Default. |
| Glyph of Obliterate | Prime slot — +20% Obliterate damage | Default. |
| Glyph of Frost Strike | Prime slot — -8 RP cost | Default. |
| Glyph of Pillar of Frost | Major — knockback immunity | Default. |
| Glyph of Anti-Magic Shell | Major — +2s duration | Default. |
| Glyph of Death's Advance | Major — passive CD reduction | Default. |
| Glyph of Path of Frost | Minor — fall-damage immunity (water-walking) | Cosmetic. |
| Glyph of Death's Embrace | Minor — cosmetic | Cosmetic. |
| Glyph of Resilient Grip | Minor — Death Grip CD refund on immune | Utility. |

---

## 6. Author-judgment items — leveling

```yaml
spell_unlocks:
  # Class baselines (DK starts at L55 from DK starter zone)
  DEATH_GRIP:                {level: 55, spell_id: 49576}
  ICY_TOUCH:                 {level: 55, spell_id: 45477}
  PLAGUE_STRIKE:             {level: 55, spell_id: 45462}
  DEATH_COIL:                {level: 55, spell_id: 47541}
  DEATH_STRIKE:              {level: 56, spell_id: 49998}
  RAISE_DEAD:                {level: 56, spell_id: 46584}
  RUNE_STRIKE:               {level: 56, spell_id: 56815}
  STRANGULATE:               {level: 56, spell_id: 47476}
  BLOOD_BOIL:                {level: 58, spell_id: 48721}
  BLOOD_PRESENCE:            {level: 58, spell_id: 48263}
  ANTI_MAGIC_SHELL:          {level: 58, spell_id: 48707}
  HORN_OF_WINTER:            {level: 60, spell_id: 57330}
  CHAINS_OF_ICE:             {level: 60, spell_id: 45524}
  ARMY_OF_THE_DEAD:          {level: 60, spell_id: 42650}
  PATH_OF_FROST:             {level: 60, spell_id: 3714}
  MIND_FREEZE:               {level: 62, spell_id: 47528}
  ICEBOUND_FORTITUDE:        {level: 62, spell_id: 48792}
  DEATH_AND_DECAY:           {level: 64, spell_id: 43265}
  RAISE_ALLY:                {level: 64, spell_id: 61999}
  PESTILENCE:                {level: 66, spell_id: 50842}
  RUNE_TAP:                  {level: 66, spell_id: 48982}
  UNHOLY_PRESENCE:            {level: 66, spell_id: 48265}
  FROST_PRESENCE:            {level: 68, spell_id: 48266}
  ANTI_MAGIC_ZONE:           {level: 70, spell_id: 51052}
  DARK_COMMAND:              {level: 72, spell_id: 56222}
  DEATH_GATE:                {level: 75, spell_id: 50977}
  # Cataclysm-new (L80–L85)
  OUTBREAK:                  {level: 81, spell_id: 77575}
  NECROTIC_STRIKE:           {level: 81, spell_id: 73975}
  DARK_SIMULACRUM:           {level: 85, spell_id: 77606}
  # Frost-spec talented abilities (level reqs are talent-tier, not character level)
  HOWLING_BLAST:             {requires_talent: true, requires_spec: frost, spell_id: 49184}
  OBLITERATE:                {requires_talent: true, requires_spec: frost, spell_id: 49020}
  FROST_STRIKE:              {requires_talent: true, requires_spec: frost, spell_id: 49143}
  PILLAR_OF_FROST:           {requires_talent: true, requires_spec: frost, spell_id: 51271}
  EMPOWER_RUNE_WEAPON:       {requires_talent: true, spell_id: 47568}
  KILLING_MACHINE:           {requires_talent: true, requires_spec: frost, passive_proc: true}
  RIME:                      {requires_talent: true, requires_spec: frost, passive_proc: true}
  THREAT_OF_THASSARIAN:      {requires_talent: true, requires_spec: frost}
  MIGHT_OF_THE_FROZEN_WASTES:{requires_talent: true, requires_spec: frost}
  ANNIHILATION:              {requires_talent: true, requires_spec: frost}
  MERCILESS_COMBAT:          {requires_talent: true, requires_spec: frost}
  CHILL_OF_THE_GRAVE:        {requires_talent: true, requires_spec: frost}
  ENDLESS_WINTER:            {requires_talent: true, requires_spec: frost}
  BRITTLE_BONES:             {requires_talent: true, requires_spec: frost}
  IMPROVED_ICY_TALONS:       {requires_talent: true, requires_spec: frost}
  DEATHS_ADVANCE:            {requires_talent: true}
  HUNGERING_COLD:            {requires_talent: true, requires_spec: frost}
  LICHBORNE:                 {requires_talent: true}
  # Cross-tree talents commonly taken
  BUTCHERY:                  {requires_talent: true, tree: blood}
  SCENT_OF_BLOOD:            {requires_talent: true, tree: blood}
  TOUGHNESS:                 {requires_talent: true, tree: blood}
  IMPROVED_BLOOD_TAP:        {requires_talent: true, tree: blood}
  VIRULENCE:                 {requires_talent: true, tree: unholy}
  # Mastery / passives
  MASTERY_FROZEN_HEART:      {passive_mastery: true, base_pct: 16.0, per_point_pct: 2.0, requires_spec: frost}
  PLATE_SPECIALIZATION:      {passive: true, requires_all_plate: true, str_pct: 5}
  RUNIC_FOCUS:               {passive: true, runic_power_max: 100}
  # Disease durations (baseline; modified by Epidemic talent — Unholy)
  FROST_FEVER_DURATION:      {seconds: 21}
  BLOOD_PLAGUE_DURATION:     {seconds: 21}
```

**Death Knight is a hero class — starts at L55.** Resource model is dual: **Runes (6, color-coded)** and **Runic Power (0–100)**. There is no leveling progression below L55 to encode; all "pre-X" notes start at L55+.

**Pre-Outbreak (under L81):** disease application via Icy Touch (Frost Fever) + Plague Strike (Blood Plague), each costing rune; less efficient. Rotation: Icy Touch → Plague Strike → Howling Blast (if Frost) → Obliterate as runes come up.

**Pre-Howling Blast / Obliterate / Frost Strike / Pillar of Frost (until Frost spec-talent investment):** baseline DK rotation of Icy Touch + Plague Strike + Death Strike + Death Coil. Frost spec talents unlock via talent tree, not by character level — once spec is chosen and points invested, all 4 abilities are available immediately.

**Pre-Necrotic Strike (under L81):** no anti-heal pressure (PvP-relevant; PvE skip).

**Pre-Dark Simulacrum (under L85):** no spell-mirror utility.

**Pre-Frost Presence (under L68):** use Blood Presence (tank-default since L58) for leveling tankiness.

**Pre-Mastery (under L80):** no Frozen Heart scaling — Frost damage flat-rate.

**Pre-Mind Freeze (under L62):** rare; DK-starter zone Strangulate available at L56 covers interrupts. Strangulate is 5s silence at 30y range; substitutes pre-L62.

**Pre-Anti-Magic Shell (under L58):** none — no magic absorb. Use Icebound Fortitude (L62) once available; otherwise rely on Death Strike heal.

---

## 7. Suggested next research passes

1. Killing Machine ICD / proc rate per-weapon-speed (auto-attack only? or includes melee specials?).
2. Rime proc rate from Obliterate (15% per hit?).
3. Howling Blast Glyph of Howling Blast — exact effect: +20% crit OR +50% disease damage on Rime procs.
4. Obliterate damage formula (weapon damage × multiplier) + disease bonus (15% per disease?).
5. Frost Strike RP cost (32 baseline; -8 with glyph = 24).
6. Pillar of Frost +20% Strength stack-with-Strength-buffs scaling (additive vs multiplicative with Horn / Plate Spec).
7. Empower Rune Weapon — does the 25 RP overcap to 100 if currently > 75?
8. Frost Fever Cataclysm-specific debuff effects (-14% melee haste on target?).
9. Blood Plague — does it tick faster than Frost Fever or same rate?
10. Outbreak full effect — applies both diseases at full duration, no rune cost, 1min CD; stacks with talent **Epidemic** only in Unholy tree.
11. Mastery Frozen Heart formula — base 16% + 2% per Mastery rating tier (similar to other Cata masteries).
12. Threat of Thassarian — what does it do for 2H Frost? (DW: off-hand auto-attack with Frost Strike / Obliterate / Death Strike / Rune Strike. 2H: dead talent or grants Frost Strike to crit-bonus?)
13. Might of the Frozen Wastes — +20% melee dmg with 2H + 4% Strength + 45% chance to reset Howling Blast CD on Obliterate (?).
14. Annihilation — Frost Strike + Obliterate damage / RP bonuses.
15. Merciless Combat — +20% damage on Obliterate / Frost Strike vs targets < 35% HP.
16. Chill of the Grave — RP-gain talent on Frost Strike / Obliterate.
17. Endless Winter — +Strength + Howling Blast cost reduction.
18. Brittle Bones — +4% physical damage debuff on diseased targets.
19. Improved Icy Talons — +20% melee haste raid buff (overlaps Wrath of Air / Hunting Party).
20. Hungering Cold — duration / radius / breaks on damage.
21. Lichborne — duration / utility (self-CC break).
22. Death's Advance passive — 50% movement-impair reduction + 15% movement bonus.
23. Anti-Magic Shell — RP-gain cap (50 RP per cast?).
24. Anti-Magic Zone — radius / total absorb / talent-vs-baseline.
25. Icebound Fortitude — base 12s / -20% damage / glyph -50% CD.
26. Mind Freeze — 4s lockout + 10s CD off-GCD.
27. Strangulate — 5s silence / 2min CD off-GCD.
28. Death Grip — 25s CD / 30y range / forced-taunt magnitude.
29. Pestilence — disease spread radius (10y) + does it refresh full duration on primary?
30. Blood Boil — diseased-target damage bonus + non-disease damage.
31. Death and Decay — radius (10y) + 30s CD + tick rate.
32. Horn of Winter — 2min raid buff + 10 RP / off-GCD with Forceful Deflection talent.
33. Death Pact — Unholy tree only; ghoul-sacrifice self-heal.
34. Death Strike — Cata heal-from-damage-taken formula (% of damage taken in last 5s, min 7%).
35. Forceful Deflection (passive) — Strength → Parry rating conversion.
36. Runic Focus passive — Runic Power max + decay rate (0 OOC?).
37. T13 2P/4P (Frost) — exact bonuses.
38. Glyph of Raise Dead — -2min CD.
39. Glyph of Death Coil — refund / reduced cost.
40. Death Coil in Frost — does it ever beat Frost Strike? (No — but verify edge cases.)
41. Bone Shield (Unholy) — talent / cooldown / charge count (NOT taken in Frost).
42. Crimson Scourge (Blood) — talent (NOT taken in Frost).

---

## 8. DBC verification

(Spell IDs in §6.)

- Death Grip 25s / 30y / forced-taunt. Spell.dbc 49576.
- Icy Touch RP gen + Frost Fever. Spell.dbc 45477.
- Plague Strike + Blood Plague. Spell.dbc 45462.
- Death Coil RP-spender. Spell.dbc 47541. **NOT used in Frost rotation — kept for level reference only.**
- Death Strike heal-from-damage-taken. Spell.dbc 49998.
- Raise Dead 3min / 1min CD glyphed. Spell.dbc 46584.
- Strangulate 5s silence / 2min. Spell.dbc 47476.
- Blood Boil AoE diseased dmg. Spell.dbc 48721.
- Anti-Magic Shell 5s / 75% magic absorb / RP gain. Spell.dbc 48707.
- Horn of Winter raid buff + 10 RP. Spell.dbc 57330.
- Chains of Ice ranged snare. Spell.dbc 45524.
- Army of the Dead 10min CD raid CD. Spell.dbc 42650.
- Path of Frost water-walking. Spell.dbc 3714.
- Mind Freeze 10s / 4s lockout / off-GCD. Spell.dbc 47528.
- Icebound Fortitude 2min / 12s / -20% dmg. Spell.dbc 48792.
- Death and Decay 30s CD / 10s ground AoE. Spell.dbc 43265.
- Pestilence disease-spread. Spell.dbc 50842.
- Frost Presence threat reduction + stamina. Spell.dbc 48266.
- Anti-Magic Zone raid magic absorb. Spell.dbc 51052.
- Dark Command threat redirect. Spell.dbc 56222.
- Outbreak 1min / no rune / both diseases. Spell.dbc 77575.
- Necrotic Strike anti-heal. Spell.dbc 73975.
- Dark Simulacrum spell-mirror. Spell.dbc 77606.
- **Howling Blast — Frost talent, AoE, applies Frost Fever. Spell.dbc 49184.**
- **Obliterate — Frost talent, 1F+1U rune, disease bonus. Spell.dbc 49020.**
- **Frost Strike — Frost talent, 32 RP, dual-wield with ToT. Spell.dbc 49143.**
- **Pillar of Frost — Frost talent, 1min / 20s / +20% Str. Spell.dbc 51271.**
- Empower Rune Weapon 5min / refresh runes + 25 RP. Spell.dbc 47568.
- Killing Machine — Frost talent, passive proc. Talent.dbc.
- Rime — Frost talent, passive proc. Talent.dbc.
- Threat of Thassarian — Frost talent (DW). Talent.dbc.
- Might of the Frozen Wastes — Frost talent (2H). Talent.dbc.
- Annihilation — Frost talent. Talent.dbc.
- Merciless Combat — Frost talent. Talent.dbc.
- Chill of the Grave — Frost talent. Talent.dbc.
- Endless Winter — Frost talent. Talent.dbc.
- Brittle Bones — Frost talent. Talent.dbc.
- Improved Icy Talons — Frost talent (raid haste buff). Talent.dbc.
- Hungering Cold — Frost talent (1min / 10s AoE freeze). Talent.dbc.
- Lichborne — DK talent (cross-tree). Talent.dbc.
- Death's Advance — talent, passive movement. Talent.dbc.
- Butchery — Blood talent (RP-on-attack). Talent.dbc.
- Scent of Blood — Blood talent (RP-on-damage-taken). Talent.dbc.
- Toughness — Blood talent (armor / -movement-impair). Talent.dbc.
- Improved Blood Tap — Blood talent. Talent.dbc.
- Virulence — Unholy talent (+disease damage + spell hit). Talent.dbc.
- Mastery Frozen Heart — Frost. Mastery DBC.
- Plate Specialization — class passive. Spell.dbc.
- Glyphs (Obliterate, Frost Strike, Howling Blast, Pillar of Frost, AMS, Death's Advance, Path of Frost, Death's Embrace, Resilient Grip, Raise Dead, Death Grip, Icebound Fortitude, Mind Freeze). Glyph.dbc.
- T13 Frost 2P/4P. Item set DBC.
- Golemblood Potion. Item DBC.
- Frost Fever debuff. Spell.dbc.
- Blood Plague debuff. Spell.dbc.
- Forceful Deflection passive (Str→Parry). Spell.dbc.
- Runic Focus passive (RP max). Spell.dbc.

---

## 9. Notes on prompt deviations

1. **Spec selection: Frost (2H).** Per backlog directive; verified Frost vs Unholy parity for Dragon Soul.
2. **Rune system Cata-rework** — 6-rune pool with independent 10s recharge, NOT Wrath's 10-CD-slot system. Older guides describe Wrath behavior; Cata is consolidated.
3. **Death Runes** — runes converted via Blood Tap / Death Rune talents; consume as any color. Frost spec converts Blood runes to Death runes via **Blood of the North** (Cata: this is now an Unholy talent named "Blood of the North"; Frost trees this differently; **UNVERIFIED whether Frost retains Blood-of-the-North-equivalent in Cata**) — affects whether Frost has a 0/2/2 or 0/2/4 effective rune layout.
4. **Outbreak (Cata-new L81)** — 1min CD, applies both diseases, no rune cost. **Replaces Icy Touch+Plague Strike disease application** for boss pulls.
5. **Pillar of Frost** — Frost-spec 1min CD, +20% Strength 20s. Major DPS CD #1.
6. **Empower Rune Weapon** — 5min CD, refreshes all runes + 25 RP. Major DPS CD #2 (long-CD).
7. **Killing Machine + Rime** — proc-based reactive layer. Bot must track these as event-driven flags, not timers.
8. **Mastery Frozen Heart** — Frost-only, scales Frost-school damage (Frost Strike, Howling Blast, Frost Fever, partial Obliterate Frost-portion).
9. **Stat priority Hit/Exp first** — same melee rule as all melee specs; hard cap 8% / 26.
10. **Glyph of Howling Blast effect — UNVERIFIED.** Some guides say +20% crit on Howling Blast; others say diseased-target bonus. Flag for DBC verification.
11. **2H Frost via Might of the Frozen Wastes** — +20% melee damage with 2H. Hard 2H requirement for this build.
12. **Threat of Thassarian** — taken in 2H build for the Frost Strike RP-gain side effect (passive bonus); DW build uses it for off-hand spec-strike copies.
13. **Plague Strike rare in Frost** — only used when Outbreak on CD AND Blood Plague < 3s; usually Outbreak handles all disease application.
14. **Frost Strike preferred RP dump** — Death Coil is Unholy-spec's RP dump; Frost ignores Death Coil.
15. **Frost Presence default** — DPS presence; Blood Presence reserved for tanking (not Frost-DPS).
16. **Anti-Magic Shell dual-purpose** — defensive AND RP-generation; pre-empt magic damage when known incoming.
17. **Death Strike emergency-only in Frost** — Cata heal-from-damage-taken makes it useful at HP < 35%, but NOT a rotational ability.
18. **Hungering Cold off by default** — 5 talent points better spent in Toughness / Virulence on raid-PvE.
19. **Death's Advance passive** — Cata-new movement passive (-50% snare reduction); take 1pt minimum.
20. **Talent split 31/7/3** — default; alternates 31/5/5 reach Lichborne (cross-tree) for self-CC-break utility, encounter-tunable.
21. **DK starts L55** — hero class; no L1–L54 progression.
22. **No L1 spec choice** — all DKs go through DK-starter zone L55–L58 in Blood Presence by default; spec chosen at L58 trainer.
23. **Disease snapshot on apply** — Cata: diseases inherit Strength/Mastery on apply, so refreshing during Pillar of Frost is a measurable gain. Bot logic: if Pillar of Frost up AND disease ≤ 8s remaining, refresh now.
24. **Pestilence behavior Cata 4.x** — spreads diseases AND refreshes them on primary target (full duration). Useful in AoE for disease maintenance + spread in one cast.
25. **Death Coil not in Frost rotation** — skip; it costs the same RP as Frost Strike but Frost Strike's Frost-school damage benefits from Mastery.
26. **Blood-tree talent investment in Frost** — Toughness 2 (default) OR Butchery 2 + Scent of Blood 1 (if RP-starvation observed in raid logs). Encounter-tunable.
27. **Unholy-tree Virulence 3** — flat +9% disease damage, mandatory because Blood Plague + Frost Fever are non-trivial damage in Frost with full rune economy.
28. **Pre-pot Golemblood + 1-min mark second pot** — same as all Strength-melee specs.
29. **Bot-side note: Mind Freeze + Strangulate dual-interrupt-set** — Mind Freeze for melee-range, Strangulate for ranged casters; bot should auto-pick based on target distance.
30. **Bot-side note: Death Grip add-pull / gap-close** — distinct from interrupt or threat use; encode as a separate utility action with target-priority gating.
31. **Bot-side note: Anti-Magic Shell auto-cast on incoming magic damage event** — pre-emptive cast logic requires encounter-aware threat/cast detection; default to "self HP < 80% AND last incoming hit was magic school" heuristic.
32. **Bot-side note: Runic Power overcap detection** — Frost Strike fires when RP ≥ 88 (preventing overcap on next 12-RP gain from Obliterate), BEFORE Obliterate priority slot.
33. **Bot-side note: Killing Machine consumption** — must check both Frost Strike and Obliterate runes/RP availability AND Pillar of Frost active to decide which spell consumes the proc.
34. **Bot-side note: Rime consumption** — single-cast, immediate; no rune/RP gating because Rime makes Howling Blast free.
35. **Bot-side note: Empower Rune Weapon "panic button" condition** — runes ≤ 1 AND Pillar of Frost active AND incoming GCD has no other valid action; rare but real edge case in long-fight rotation.
36. **No taunt in Frost** — Death Grip is single-target forced-taunt-pull but not a sustained-threat tool; off-tank duty defaults to Blood-spec DKs.
37. **No gap-closer beyond Death Grip** — frequent-movement fights (Spine of Deathwing's roll, Blackhorn's adds) benefit from Frost's instant-cast-heavy kit; bot should pre-cast Frost Strike before required movement.

End of survey.

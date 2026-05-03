# Feral Cat Druid 4.3.4 — Multi-Guide Strategy Survey

Audit trail for `docs/specs/feral-cat-druid.md`. Survey conducted 2026-05-02. This is the **sixth and final melee-DPS survey** (after Combat Rogue, Fury Warrior, Retribution Paladin, Frost Death Knight, Enhancement Shaman). Feral Cat is the **DPS half of the Druid Feral talent tree**; the **Bear/Guardian half (tank)** is **DEFERRED** to the tank phase per the role-ordering rule (healer → ranged → melee → tank). Druid has four roles total: **Balance** (ranged caster — already documented in `docs/specs/balance-druid.md`), **Restoration** (healer — already documented in `docs/specs/resto-druid.md`), **Feral Cat** (this doc), and **Feral Bear/Guardian** (tank — deferred). With Feral Cat documented, all four melee DPS Cata specs are covered (Rogue Combat, Warrior Fury, Paladin Ret, DK Frost, Shaman Enhancement, Druid Feral-Cat).

Cross-reference: Cata 4.3.4 Druids share the **shapeshift mechanic**, **Mark of the Wild raid buff**, **Innervate**, **Rebirth**, **Faerie Fire / Faerie Fire (Feral)**, **Hibernate**, **Soothe**, **Remove Curse / Cure Poison (Resto-form)**, and the **Tranquility** ultimate with Balance and Restoration. Those mechanics are reused as encoded in the prior two specs; this survey focuses on the Feral-Cat-specific shape: **Cat Form melee combat with Energy + Combo Points (Rogue-like resource model)**, **Mangle / Shred / Rake / Rip / Ferocious Bite / Savage Roar priority**, **Tiger's Fury and Berserk damage windows**, **Bleed-snapshot/pandemic mechanics post-Cata-4.0 rework**, **Predatory Swiftness instant-cast proc bridge to Resto utility**, and **Stampede + King of the Jungle**.

Feral Cat is unique among Cata 4.3.4 melee DPS specs because it is the **only DPS spec that shares its talent tree with a tank role** (Bear/Guardian). The shared tree means talent-investment selection determines role — Cat-DPS picks specific talents (King of the Jungle, Predatory Strikes, Stampede, Furor) while skipping bear-tank talents (Pulverize, Brambles, Thick Hide, Natural Reaction). At Mists 5.0+ the spec was split into Feral (Cat-only) and Guardian (Bear-only), but **at 4.3.4 it remains one shared tree**.

---

## 1. Spec selection

**Spec disposition: Feral Cat (DPS half of shared Feral talent tree).** Druid has four role-functions in Cata 4.3.4: Balance (ranged caster, covered separately), Restoration (healer, covered separately), Feral Cat (this doc — melee DPS), Feral Bear / Guardian (tank — DEFERRED to tank phase). Within Druid melee, Cat is the only DPS option — Bear is exclusively tank. There is no Cat-vs-other-DPS-spec contest within Druid melee.

**Shared talent tree note.** The Feral tree at 4.3.4 contains both Cat and Bear talents intermixed. Cat-DPS spec disposition means the bot takes the Cat-favored talents (Predatory Strikes 3/3 for AP, King of the Jungle 3/3 for Berserk damage, Stampede 2/2 for post-Tiger's-Fury Ravage, Furor 3/3 for Energy gen, Primal Fury 2/2 for crit→CP, Endless Carnage 2/2 for SR/Rake duration, Berserk 1/1, Predatory Swiftness 3/3) and **skips** Bear-tank talents (Pulverize, Brambles, Thick Hide, Natural Reaction). The bear-form abilities (Mangle Bear, Maul, Lacerate, Pulverize, Frenzied Regeneration) remain on the spellbook but are not used in cat-form DPS rotation. Stance-dance to bear is largely dead in Cata Cat PvE — used only for utility (Bash interrupt fallback if Skull Bash is on CD; Frenzied Regeneration emergency self-heal in extreme cases). The bot does NOT routinely swap forms.

**Cat Form prerequisite.** Cat Form must be active for Cat DPS. Cat Form is L20 baseline Druid (not spec-locked); the Druid baseline shapeshift form is unlocked early. All cat-form melee abilities (Mangle Cat, Shred, Rake, Rip, Ferocious Bite, Savage Roar, Tiger's Fury, Berserk-cat, Pounce, Ravage) gate on Cat Form active.

**Resource model.** Feral Cat uses **Energy (0–100) + Combo Points (0–5 on target)** — identical resource shape to Rogue. Energy regenerates at 10/sec base (faster with haste). Combo Points accumulate on a target; finishers consume them. Mana is reserved for out-of-cat-form spells only (Healing Touch via Predatory Swiftness, Rebirth, Mark of the Wild) and is irrelevant to the DPS rotation.

**Raid viability (Dragon Soul tier):** Feral Cat clears all of Dragon Soul comfortably; consensus tier rating across guides is **A-tier** (top-half of melee DPS on simulated DPS in 4.3.4). Strong fights for Cat include Spine of Deathwing tendons (sustained ST with bleed-favored mastery), Madness of Deathwing tentacles (bleed targets benefit from Mastery: Razor Claws), Warmaster Blackhorn ground phase (high uptime ST). Weak fights include Yor'sahj slime adds (low-uptime cleave; bleeds don't propagate), Hagara phase 1 lightning (movement-heavy phases compress builder cycles).

**Cat positional requirement.** Shred requires the player to be **behind** the target. This is the spec's most distinctive bot-control challenge — positional checks (target-facing detection) gate Shred vs Mangle priority. When behind target, Shred is the primary builder; when in front, Mangle Cat is the fallback. Bot positioning logic must prefer rear-arc engagement.

---

## 2. Sources

| # | Guide | URL | Confidence |
|---|---|---|---|
| G1 | Icy Veins Feral Cat Druid Rotation/CDs | https://www.icy-veins.com/cataclysm-classic/feral-druid-pve-rotation-cooldowns-abilities | High |
| G2 | Icy Veins Feral Cat Druid Talents/Glyphs | https://www.icy-veins.com/cataclysm-classic/feral-druid-pve-spec-builds-talents-glyphs | High |
| G3 | Icy Veins Feral Cat Druid Stat Priority | https://www.icy-veins.com/cataclysm-classic/feral-druid-pve-stat-priority | High |
| G4 | Wowhead Cata Feral Cat Druid | https://www.wowhead.com/cata/guide/classes/druid/feral/dps-overview-pve-cat | Medium-High |
| G5 | wowtbc.gg Feral Cat Druid | https://wowtbc.gg/cata/class-guides/feral-cat-druid/ | Medium |
| G6 | Warcraft Tavern Feral Cat Druid | https://www.warcrafttavern.com/cataclysm/guides/pve-feral-cat-druid/ | Medium-High |
| G7 | Method Cata Feral Cat Druid | https://www.method.gg/guides/cataclysm/feral-druid | Medium |
| G8 | Noxxic Feral Cat Druid 4.3 | https://www.noxxic.com/wow/pve/druid/feral-cat/ | Medium |
| G9 | (Cross-ref) `docs/specs/balance-druid.md` (Mark of the Wild / Innervate / Faerie Fire shared) | local | Authoritative on shared mechanics |
| G10 | (Cross-ref) `docs/specs/resto-druid.md` (Rebirth / Healing Touch / Tranquility shared) | local | Authoritative on shared mechanics |
| G11 | Fluiddruid forums (Feral Cat community) | https://www.fluiddruid.net/forum/ | Medium-High (community deep-dives) |
| G15 | Warcraft Wiki per-spell | https://warcraft.wiki.gg/wiki/<spell> | High |
| G16 | Wowhead Cata per-spell | https://www.wowhead.com/cata/spell=<id> | High |

Notes: Fluiddruid (G11) is a long-running Feral community forum with deep mechanical analysis (snapshot rules, optimal Rip refresh windows, Bloodletting glyph math). Where Icy Veins / Wowhead diverge, Fluiddruid threads are the tiebreaker for Cat-rotation specifics.

---

## 3. Strong consensus

| Item | Consensus | Spec disposition |
|---|---|---|
| **Cat Form active for DPS** — Cat Form L20 baseline; all cat melee abilities gated on Cat Form. | 8/8 | Hard requirement. |
| **Agility = primary stat** — Cat scales melee AP and crit from Agility (not Strength). | 8/8 | Encoded. |
| **Energy + Combo Points resource model** — Energy 0–100, regen 10/sec base; Combo Points 0–5 on target. | 8/8 | Rogue-like. |
| **Mangle Cat** — primary front-arc builder; applies +30% bleed-damage debuff to target (60s); 0 CD, costs 35 Energy; melee strike. | 8/8 | Front-arc builder. |
| **Shred** — primary rear-arc builder; **requires being behind target**; costs 40 Energy; +damage vs bleeding target. | 8/8 | Rear-arc builder, primary. |
| **Rake** — Bleed DoT builder; 9s base duration; ticks every 3s; costs 35 Energy; refresh on pandemic window. | 8/8 | Maintained DoT builder. |
| **Rip** — primary 5-CP finisher; 16s base bleed DoT; ticks every 2s; costs 30 Energy. | 8/8 | Primary finisher. |
| **Ferocious Bite** — alternate finisher; consumes CP + extra Energy (up to +25 Energy = +100% damage); execute use at < 25% target HP. | 8/8 | Sub-25% execute / Rip-extension via glyph. |
| **Savage Roar** — self-buff +30% damage in cat form; 12s + 6s/CP duration (24s @ 1CP up to 42s @ 5CP per Glyph of Savage Roar — verify). | 8/8 | Mandatory uptime self-buff. |
| **Tiger's Fury** — 30s CD; restores 60 Energy + +15% damage 6s; off-GCD; cannot be used during Berserk. | 8/8 | On CD when Energy ≤ 35. |
| **Berserk (Cat)** — 3min CD; 15s; reduces all ability Energy cost by 50% AND Mangle Cat 0-CD (already 0; really means "Mangle ignores its global CD" — Cata altered Berserk effect, **UNVERIFIED** specifics); +Energy regen via talent King of the Jungle. | 7/8 | Major DPS CD. |
| **King of the Jungle (talent)** — Tiger's Fury grants +15% damage 6s; Berserk gives +50% Energy regen during effect (talent-driven scaling). | 7/8 | Default 3/3. |
| **Predatory Strikes (talent)** — +AP based on weapon damage in feral forms; baseline scaling. | 8/8 | Default 3/3. |
| **Predatory Swiftness (talent)** — finishers grant chance for next Healing Touch / Rebirth / Entangling Roots / Cyclone to be instant cast; 3/3 = 100% proc on 5-CP finisher. | 8/8 | Default 3/3. |
| **Stampede (talent)** — Tiger's Fury enables Ravage usable without stealth for 8s (one cast). | 8/8 | Default 2/2. |
| **Furor (talent)** — Faerie Fire (Feral) restores Energy; +10 Energy on shapeshift to Cat; +Mana for caster-form. | 7/8 | Default 3/3 (some builds 2/3). |
| **Endless Carnage (talent)** — extends Savage Roar duration +4s/+8s and Rake duration +3s/+6s. | 8/8 | Default 2/2. |
| **Primal Fury (talent)** — Cat: critical strikes from Mangle/Shred/Ravage award an extra Combo Point; Bear: +rage on crit. | 8/8 | Default 2/2; main CP-gen. |
| **Feral Aggression (talent)** — Mangle damage +5%/+10%/+15% AND Ferocious Bite damage +X%/Maim duration +X% (verify scaling). | 7/8 | Default 3/3. |
| **Berserk talent (capstone)** — unlocks Berserk ability; 1/1. | 8/8 | Default 1/1. |
| **Mangle Bleed bonus** — Mangle Cat (and Mangle Bear, and Trauma talent — N/A here) applies a debuff giving +30% damage from all bleeds to target for 60s; required for Rip / Rake / Lacerate damage to be uncrippled. | 8/8 | Maintained. |
| **Mastery: Razor Claws** — Feral Druid mastery; increases bleed damage by X% (8% baseline + 3.13% per mastery point — **UNVERIFIED** exact formula). | 8/8 | Reforge target. |
| **Bleeds do NOT snapshot in Cata 4.0+** — Rip / Rake refresh inherits CURRENT stats, not buff state at apply time. Tiger's Fury / Berserk windows still favored for refresh because they boost ALL damage during the buff window (DoT ticks during Tiger's Fury benefit from +15%). | 7/8 | Encoded. |
| **Pandemic refresh window** — refresh DoT within last ~30% (~5s on Rip, ~3s on Rake) inherits remaining duration to cap; refreshing earlier wastes duration. | 7/8 | Encoded. |
| **Faerie Fire (Feral)** — 6s CD instant; armor debuff -12% (raid-equivalent to Sunder/Expose Armor); off-GCD with talent (Furor); +Energy regen. | 7/8 | Always-on debuff; on CD. |
| **Skull Bash (Cata-new L60)** — charge interrupt; 13–25y range; 1min CD; Cat or Bear form. | 8/8 | Auto-interrupt. |
| **Stampeding Roar (Cata-new L83)** — raid speed buff +60% movement 8s; 3min CD; Cat or Bear form. | 7/8 | Movement utility. |
| **Maim** — stun finisher; 1–5s stun based on CP spent; 10s CD; Cat-form. | 7/8 | CC utility. |
| **Pounce** — stealth-only opener; 4s stun + builder; awards 1 CP. | 8/8 | Opener only. |
| **Ravage** — stealth-only or Stampede-enabled; high-damage opener strike; 5+ CP awarded? **UNVERIFIED** (most sources say 1 CP, +large damage from positional behind). | 7/8 | Opener / Stampede-Tiger's-Fury proc. |
| **Cower** — threat reduction; rarely used in Cat (raid tanks hold threat). | 6/8 | Niche. |
| **Bash** — bear-form 5s stun; 35s CD; rarely used in Cat (Skull Bash supersedes). | 6/8 | Stance-dance utility, niche. |
| **Talent split — 31 Feral / 3 Balance / 7 Resto** OR **31 Feral / 7 Balance / 3 Resto** — main filler-point variations. | 6/8 | Default 31/3/7 for Furor (Resto-tree). |
| **Hit cap (melee special)** — 8% (~961 rating @ 85). Cat's melee abilities use melee-special-hit cap. | 8/8 | Hard cap. |
| **Expertise cap** — 26 (~781 rating @ 85) to dodge cap; matters for Shred (rear-arc strikes can still be dodged by some bosses). | 7/8 | Hard cap. |
| **Stat priority** — Agility → Hit (8%) → Expertise (26) → Mastery → Crit → Haste (consensus, with caveats — see disagreements). | 6/8 | See §4.3. |
| **Glyphs Prime** — Bloodletting + Tiger's Fury + Rip (consensus default). | 7/8 | Default. |
| **Glyphs Major** — Berserk + Stampeding Roar + Pounce OR Maim. | 6/8 | Default. |
| **Glyphs Minor** — Mark of the Wild + Aquatic Form + Travel Form OR Unburdened Rebirth (cosmetic). | 7/8 | Cosmetic. |
| **Pre-pot Tolvir Potion** — +1200 Agility 25s; T-2s + 1-min mark. | 7/8 | Encoded. |
| **Opener** — Stealth → Pounce (or Ravage with Stampede) → Mangle/Shred → Tiger's Fury → Mangle/Shred to 5 CP → Savage Roar → Mangle/Shred to 5 CP → Rake → Rip → Berserk → priority. | 5/8 | Encoded; variants per §4.X. |
| **Bloodlust raid alignment** — pre-pull Bloodlust stacks with Berserk + Tiger's Fury + Tolvir Potion for opener burst. | 7/8 | Raid CD. |
| **Mark of the Wild raid buff** — pre-cast all stats / Resistance buff; 1hr; Druid-baseline. | 8/8 | Pre-pull. |
| **Tranquility** — Cata change: now 8min CD raid heal (not Resto-only); Feral can cast in caster form for emergency raid healing. | 7/8 | Emergency utility. |
| **Innervate** — 3min CD mana CD; cast on a healer or self (out of form); Druid-baseline. | 8/8 | Utility. |
| **Rebirth** — 10min CD combat-res; Druid-baseline; cast in caster form (or via Predatory Swiftness instant). | 8/8 | Utility. |
| **Mangle armor debuff** — Mangle Cat applies a -12% armor debuff (raid-equivalent to Sunder/Expose Armor) — **WAIT — this is NOT consensus**. Mangle's bleed-damage debuff is consensus; Mangle as armor-debuff source is NOT. Faerie Fire (Feral) is the armor-debuff source. **UNVERIFIED** — verify Mangle Cat does NOT apply armor-debuff, only bleed-damage debuff. | UNVERIFIED | See disagreements §4.X. |

---

## 4. Disagreements

### 4.1. Talent split — 31/3/7 vs 31/7/3 vs 31/5/5

**Spec disposition: 31 Feral / 3 Balance / 7 Resto.**

- **Feral 31:** all core (Predatory Strikes 3/3, Feral Aggression 3/3, Stampede 2/2, Primal Fury 2/2, Furor — wait, **Furor is Resto-tree at 4.3.4** — verify; Endless Carnage 2/2, King of the Jungle 3/3, Predatory Swiftness 3/3, Berserk capstone, Blood in the Water (sub-25% Ferocious Bite refreshes Rip — talent), Rend and Tear (+Maim/Ferocious-Bite vs bleeding), Nurturing Instinct).
- **Balance 3:** **Furor 3/3** (+10% mana, +Energy on shapeshift) — actually, Furor location varies between expansions; at 4.3.4 it's in the Resto tree. **UNVERIFIED — verify Furor tree placement.**
- **Resto 7:** **Furor 3/3** + **Naturalist 2/2** (+damage in cat form) + **Natural Shapeshifter 2/2** (-mana cost on shapeshift) OR **Heart of the Wild 3/3** (Stamina + Intellect + AP scaling).

Variant: **31/7/3** invests Balance more deeply for Genesis (+5% periodic damage — boosts Rake/Rip ticks). G7 (Method) prefers 31/7/3 with Genesis 3/3 over Resto-tree depth.

G6 / G8 default: 31/3/7 (Resto-side Furor + Naturalist).
G1 / G3 / G4: 31/3/7.
G7: 31/7/3.

**Disposition: 31/3/7 (Resto-side filler).** Furor + Naturalist gives more raid DPS than Genesis on simulated parses. **Verify Furor tree placement at 4.3.4.**

### 4.2. Stat priority — Mastery vs Haste vs Crit ordering

**Spec disposition: Agility → 8% Hit → 26 Expertise → Mastery → Crit → Haste.**

- G1 (Icy Veins): Mastery > Crit > Haste.
- G3 (Icy Veins stats page): Mastery > Crit > Haste; notes Haste closes the gap at very high gear levels because faster auto-attacks generate Energy via Omen of Clarity (Resto-tree clearcasting) procs.
- G4 (Wowhead): Mastery > Haste > Crit (minority — places Haste above Crit).
- G6 (Warcraft Tavern): Mastery > Crit > Haste.
- G8 (Noxxic): Crit > Mastery > Haste (rare ordering — places Crit above Mastery; based on Primal Fury CP-gen value).
- G11 (Fluiddruid forums): Mastery > Crit ≈ Haste (close at high gear levels).

**Disposition: Mastery > Crit > Haste (4/6 majority).** Bot-side stat-weight reforge target: Mastery primary, Crit secondary. Mastery: Razor Claws scales bleed damage which is ~50% of total Cat damage (Rake + Rip ticks). Crit feeds Primal Fury (extra CP on Mangle/Shred/Ravage crit) which compresses CP-build cycles. Haste affects Energy regen (through faster auto-attack proc rate, not directly) and is the weakest stat.

### 4.3. Hit cap — 8% melee or pursue spell hit?

**Spec disposition: target 8% melee hit; ignore spell hit.**

- All sources: 8% melee-special hit (~961 rating @ 85) is the hard cap.
- Spell hit is irrelevant for Feral Cat in DPS rotation. Healing Touch / Rebirth (caster form) and Entangling Roots / Cyclone (PvP) require spell hit, but Cat DPS rotation never casts a damage spell.

**Encoded: 8% melee hit cap; spell hit floats at whatever passive sources provide.**

### 4.4. Expertise cap — 26 or stop at boss-attack-table-cap?

**Spec disposition: 26 expertise (dodge cap).**

Cat's primary builder Shred is a rear-arc strike that **cannot be parried** but **can still be dodged** by some bosses (depends on encounter coding — Cata generally treats rear-arc as no-parry but dodgeable). Reaching 26 expertise eliminates dodge from the attack table for Shred.

- All sources: 26 expertise hard cap.

**Encoded: 26 expertise hard cap.**

### 4.5. Mangle vs Shred — positional priority

**Spec disposition: Shred when behind target; Mangle Cat when in front.**

This is the spec's defining positional rule. The bot must check `target_facing == AWAY_FROM_PLAYER` before each builder cast:
- If behind (rear-arc), cast Shred (40 Energy, +damage vs bleed, no parry, dodgeable).
- If in front, cast Mangle Cat (35 Energy, omnidirectional, applies bleed-damage debuff).

Mangle Cat must STILL be cast at least once at the start of the fight to apply the +30% bleed-damage debuff on target (lasts 60s, refresh by Mangle re-cast or Trauma talent — Trauma is Warrior, N/A here; Druid bleeds keep the debuff up via Mangle on cooldown when needed).

- G1 / G3 / G4 / G6 / G7 / G11: Shred behind, Mangle in front. Open with Mangle to apply bleed-debuff if no other source.
- G5 / G8: same.

**Encoded: Shred rear-arc, Mangle Cat front-arc; pull-opener Mangle Cat (or Shred behind) to apply +30% bleed debuff.**

### 4.6. Mangle bleed-debuff source — Mangle Cat only or all forms?

**Spec disposition: Mangle Cat AND Mangle Bear both apply the +30% bleed-damage debuff.** Trauma (Warrior talent) and Stampede (Hunter pet) and Hemorrhage (Rogue Sub) provide equivalent debuffs from other classes. Druid bleed-debuff uptime is provided by Mangle on the Cat's primary target.

- G1 / G3 / G4: Mangle Cat applies bleed-damage debuff. Refresh on 60s timer.
- G6 / G7: same.
- G11 (Fluiddruid): notes Mangle is uniquely the Druid source; Trauma / Hemorrhage from other classes also provide it. Bot should NOT re-apply Mangle if external bleed-debuff source is already active (saves Energy on Shred cycles).

**Encoded: maintain bleed-damage debuff on target. If no external source (Trauma Warrior, Hemorrhage Rogue), Mangle Cat must refresh every 60s; otherwise skip Mangle and stay on Shred.**

### 4.7. Rake refresh — pandemic or hard-overwrite?

**Spec disposition: refresh Rake within pandemic window (~3s remaining); never hard-overwrite.**

Rake is a Bleed DoT (9s base, 15s with Endless Carnage 2/2). Refreshing earlier than the pandemic window (~3s remaining) wastes duration; refreshing later (after expiration) loses ticks.

- G1 / G3 / G4 / G6 / G11: pandemic window 3s remaining or less.
- G7: pandemic window 30% of duration remaining (~4.5s on 15s base).
- G5: refresh on expire; minority — loses pandemic value.

**Disposition: refresh within last 3s of remaining duration.** Bot-side: track Rake duration on target; refresh when duration ≤ 3s.

### 4.8. Rip refresh — 5 CP requirement and pandemic window

**Spec disposition: refresh Rip ONLY at 5 CP; pandemic window is last ~5s of remaining Rip duration.**

Rip is the highest-damage finisher and must always be applied at 5 CP (lower CP yields proportional damage and is a DPS loss). Pandemic refresh applies — refreshing within the last ~30% of duration extends the new duration without losing ticks.

- G1 / G3 / G4 / G6 / G7 / G11: 5 CP only; pandemic 5s.
- G8: 5 CP only.
- G5: 5 CP only.

**Disposition: refresh Rip at 5 CP when Rip duration ≤ 5s.**

### 4.8b. Rip via Bloodletting glyph extension

**Spec disposition: Glyph of Bloodletting extends Rip by 2s per Shred cast (cap at 6 extra seconds = +3 Shreds).**

This is the spec's core glyph. Each Shred cast on a target with Rip active extends Rip by 2s, up to 6 additional seconds (3 Shreds). Bot-side: Shred during Rip-active window contributes both damage and Rip-extension; the bot does NOT need to manually re-apply Rip if Shreds are flowing during Rip's life.

- All sources: Bloodletting glyph mandatory; extends Rip via Shred.

**Encoded: Glyph of Bloodletting Prime slot. Track Rip-extension accumulated from Shreds.**

### 4.9. Savage Roar maintenance — when to refresh?

**Spec disposition: maintain Savage Roar uptime ~100%; refresh at 1 CP if Roar < 5s remaining OR at 5 CP otherwise.**

Savage Roar is +30% damage in cat form; mandatory uptime. Duration scales with CP spent (12s base + 6s/CP, with Endless Carnage 2/2 adding +8s).

- 0 CP: 12s
- 1 CP: 18s
- 5 CP: 42s
- (with Endless Carnage 2/2: +8s on all)

Bot-side maintenance rule:
- If Roar duration < 5s AND have ≥ 1 CP, refresh at 1 CP (don't waste 5 CP on Roar — Rip is higher value at 5 CP).
- If Roar duration ≥ 5s AND Rip is healthy AND we've built 5 CP, sometimes refresh Roar at 5 CP to maximize duration before next Rip.

- G1: refresh at 1 CP under 5s remaining.
- G3: refresh at 1 CP under 5s remaining.
- G7: 5 CP refresh if Rip is healthy and CP overflowing (rare).
- G11 (Fluiddruid): primary refresh at 1 CP < 5s; allow 5 CP refresh on long-uptime fights when Rip pandemic doesn't need a 5-CP cycle.

**Disposition: 1 CP refresh under 5s remaining (default). Bot tracks Roar duration as event-driven.**

### 4.10. Tiger's Fury usage — on CD or save for low Energy?

**Spec disposition: cast Tiger's Fury when Energy ≤ 35 AND TF off CD.**

Tiger's Fury restores 60 Energy. Casting at full Energy (capped) wastes the +Energy effect.

- G1 / G3 / G4 / G6 / G7: cast at Energy ≤ 35 (so the +60 doesn't cap).
- G11: cast at Energy ≤ 30; cast immediately if Berserk is about to come up (need TF in CD-window for damage stacking).
- G5: on CD.

**Disposition: cast Tiger's Fury when Energy ≤ 35.** Exception: cast Tiger's Fury immediately before Berserk (TF + Berserk damage windows stack the +15% damage during Berserk). Bot-side: track Energy threshold AND Berserk-CD upcoming.

### 4.11. Berserk usage — solo or stack with Tiger's Fury?

**Spec disposition: stack Berserk with Tiger's Fury for 24% damage window (Tiger's Fury +15% × Berserk-period buffs).**

Berserk gives 15s of doubled Energy regen and 50% reduced Energy cost. Stacking with Tiger's Fury (+15% damage 6s) overlaps for the first 6s of Berserk for compounded damage.

- G1 / G3 / G4: stack Berserk with Tiger's Fury.
- G6 / G7 / G11: stack with Bloodlust + Tolvir Potion at the pull when possible; on subsequent Berserk casts (at 3min, 6min), stack only with Tiger's Fury.

**Disposition: pull-Berserk = Tiger's Fury + Tolvir + Bloodlust + trinket on-use. Subsequent Berserks = Tiger's Fury alignment only (Bloodlust drifts).**

### 4.12. Pounce vs Ravage (with Stampede) — opener

**Spec disposition: Ravage opener if Stampede 2/2 talented (Tiger's Fury enables Ravage without stealth requirement); Pounce-from-stealth opener otherwise.**

- Pounce: stealth-only stun + builder; awards 1 CP.
- Ravage: stealth-only or Stampede-enabled; high-damage strike; awards 1 CP. Damage exceeds Pounce.
- Stampede 2/2: Tiger's Fury enables a single Ravage cast without stealth.

Opener variants:
1. **Stealth-Pounce → Tiger's Fury → Ravage (Stampede) → Mangle/Shred to 5 CP → Savage Roar → Berserk → Mangle/Shred to 5 CP → Rake → Rip.** Top-DPS opener; uses both Pounce (stun + 1 CP from stealth) AND Ravage (Stampede-proc from TF).
2. **Stealth-Ravage → Mangle/Shred...** (no Pounce; sacrifices the stealth stun for direct Ravage damage).
3. **Direct engagement (no stealth) → Mangle Cat → Tiger's Fury → Ravage (Stampede) → ...**

- G1 / G3 / G7 / G11: variant 1 (Pounce → TF → Ravage → ...).
- G4 / G6: variant 2 or 3 (sacrifice stealth for direct Ravage or skip stealth entirely).

**Disposition: variant 1 (Pounce → TF → Ravage → Mangle/Shred build → Savage Roar at 1 CP → continue building → Berserk → Rake → Rip at 5 CP).**

### 4.13. Ferocious Bite usage — execute only or Rip-extension via Glyph of Ferocious Bite?

**Spec disposition: Ferocious Bite < 25% target HP for execute; otherwise saved.**

Ferocious Bite consumes 5 CP + up to +25 Energy for +100% damage. At target HP < 25%, **Blood in the Water** talent (3/3) makes Ferocious Bite refresh Rip duration to maximum. This makes Ferocious Bite the execute-phase finisher (extends Rip without burning a Rip cast).

- G1 / G3 / G4 / G6 / G7 / G11: Ferocious Bite at target < 25% HP for Blood-in-the-Water Rip refresh.
- G8: Ferocious Bite anytime above 25% HP if Rip is healthy and Savage Roar is healthy and CP cap overflowing — minority view; consensus rejects this on simulation grounds (Rip > Ferocious Bite per CP).

**Disposition: Ferocious Bite at target HP < 25% AND 5 CP AND Rip-active. At HP > 25%, prioritize Rip refresh (5 CP) over Ferocious Bite.** Below 25%, Ferocious Bite spam is the execute rotation (each Ferocious Bite refreshes Rip via Blood in the Water; replaces direct Rip casts).

### 4.14. Predatory Swiftness usage — instant Healing Touch self-heal or Rebirth or Entangling Roots?

**Spec disposition: instant Healing Touch self-heal at HP ≤ 60%; instant Rebirth on dead raid member.**

Predatory Swiftness 3/3 grants 100% chance for next Healing Touch / Rebirth / Entangling Roots / Cyclone to be instant cast and 0 mana cost on a 5-CP finisher.

- Use 1: Healing Touch self-heal during Cat DPS rotation (instant, no shapeshift required). ~30% HP heal.
- Use 2: Rebirth on dead raid member (combat-res) without breaking cat form.
- Use 3: Entangling Roots on adds (CC).
- Use 4: Cyclone on caster-add (PvP-tier CC; rare in PvE).

- G1 / G3 / G4 / G6 / G7 / G11: instant Healing Touch self-heal as default consumption.
- G7: also recommends instant Rebirth; saves a global on combat-res.

**Disposition: consume Predatory Swiftness on Healing Touch when HP ≤ 60%; otherwise let proc fall (no penalty for not consuming).**

### 4.15. Rake DoT during Berserk vs Tiger's Fury

**Spec disposition: refresh Rake just before/during Tiger's Fury OR Berserk window for the +15% / +Energy stacking.**

Cata 4.0+ removed snapshotting on Rake (refreshes inherit current stats). However, Rake **ticks** during Tiger's Fury / Berserk benefit from the active +15% damage modifier on the tick (because Tiger's Fury buffs ALL damage during the buff window, not snapshotted at apply).

This means refreshing Rake to align with Tiger's Fury / Berserk is value-positive because the ticks during the window are buffed.

- G1 / G3 / G4 / G6 / G11: refresh Rake at start of Tiger's Fury / Berserk windows when CP and Energy allow.
- G7: refresh Rake on pandemic regardless of CD windows (simpler; small DPS loss).
- G5: same as G7.

**Disposition: prefer Rake refresh aligned with Tiger's Fury window when possible; fall back to pandemic refresh.**

### 4.16. Glyph priority — Bloodletting + Tiger's Fury + Rip OR substitutes?

**Spec disposition (Prime):**
- **Glyph of Bloodletting** — Shred extends Rip by 2s (cap 6s). MANDATORY.
- **Glyph of Tiger's Fury** — reduces Tiger's Fury CD by 3s (30s → 27s OR similar — verify). MANDATORY.
- **Glyph of Rip** — adds 4s to base Rip duration (16s → 20s) OR +damage on Rip ticks — verify.

Variants:
- **Glyph of Berserk** — Berserk duration +5s (15s → 20s).
- **Glyph of Mangle** — +Mangle Cat damage (rare in default; Shred is the primary builder).
- **Glyph of Shred** — +Shred damage OR similar — verify; rarely defaulted.
- **Glyph of Ferocious Bite** — Ferocious Bite damage scales further with leftover Energy — verify.

**Disposition: Bloodletting + Tiger's Fury + Rip default Prime trio.** Berserk Prime swap on long-fight encounters; Mangle / Shred / Ferocious Bite alternates rare in Cata.

### 4.17. Major glyphs — Berserk vs Stampeding Roar vs Pounce vs Maim

**Spec disposition (Major):**
- **Glyph of Stampeding Roar** — increases Stampeding Roar radius (15y → 30y) — utility raid tool.
- **Glyph of Pounce** — increases Pounce stun duration (4s → 7s) — opener stun extension.
- **Glyph of Maim** — Maim 1s longer per CP — CC duration.
- **Glyph of Frenzied Regeneration** — converts Frenzied Regeneration HoT to instant heal — bear stance-dance defensive (rarely used in Cat).
- **Glyph of Rebirth** — Rebirth target 100% HP after res — utility.

**Disposition: Stampeding Roar + Pounce + Rebirth (default raid). Maim swaps in if encounter has CC-target adds.**

### 4.18. Minor glyphs

**Spec disposition (Minor):**
- **Glyph of Mark of the Wild** — reduces mana cost to cast (cosmetic).
- **Glyph of Aquatic Form** — +50% swim speed (cosmetic).
- **Glyph of Travel Form** — Travel Form usable indoors (utility).
- **Glyph of Unburdened Rebirth** — Rebirth no reagent (utility).
- **Glyph of Dash** — -2min CD on Dash.
- **Glyph of Challenging Roar** — reduces threat reduction CD.

**Disposition: Mark of the Wild + Travel Form + Unburdened Rebirth (raid-friendly).**

### 4.19. AoE rotation — 3+ enemies

**Spec disposition: Swipe (Cat) on 3+ enemies; maintain bleeds on highest-priority target only; no AoE bleed-spread.**

- **Swipe (Cat)** — 50 Energy AoE strike; 8y radius; awards no CP.
- Rotation for AoE: Mangle on primary (bleed debuff) → Rake on primary → Swipe spam → Tiger's Fury for Energy → Rip on primary (5 CP via Mangle/Shred not Swipe; Swipe doesn't generate CP).
- **Thrash (Cat)** — Cata-new L81 AoE bleed; 8y radius; 6s duration; awards 0 CP. Cat counterpart of Bear's Thrash. — verify presence in Cat form at 4.3.4.

- G1 / G3 / G6 / G7: Swipe spam with Mangle on primary; bleeds maintained on primary only.
- G4: Thrash + Swipe rotation on AoE if Thrash available in Cat form.
- G11: sustained AoE = Swipe + Thrash + bleeds-on-primary.

**Disposition: AoE 3+ → Swipe spam + maintain bleeds on primary + Thrash if available.** Cleave 2 enemies → ST priority on primary; let Swipe out-DPS Mangle only at 3+ targets.

### 4.20. Skull Bash — interrupt and gap-closer

**Spec disposition: 60s CD; charge interrupt; 13–25y range; usable in Cat or Bear form.**

Skull Bash is Cata-new L60. It both interrupts a cast AND charges the bot to the target (gap-closer). 1min CD.

- G1 / G3 / G4 / G6 / G7: auto-cast on enemy cast ≤ 300ms before completion + bot is in melee or charge range.
- G11: same; also usable as proactive gap-closer (charge into melee even without an interrupt target — this is a DPS gain on movement-heavy fights).

**Disposition: Skull Bash on enemy cast ≤ 300ms before completion; secondary use as gap-closer when bot is at range and target is at 13–25y.**

### 4.21. Stampeding Roar — when to cast?

**Spec disposition: 3min CD; raid speed buff +60% movement 8s; cast on movement-heavy phases.**

- G1 / G3 / G4 / G6 / G7: encounter-tunable; cast on phase transitions (Hagara phase 2 ice tomb run, Madness platform jump, etc.).
- G11: same; with Glyph of Stampeding Roar (radius 15y → 30y), it's a raid-wide CD.

**Bot-side: encounter-script-driven; default = on phase transition or on raid-leader macro trigger.**

### 4.22. Faerie Fire (Feral) — armor debuff + Energy

**Spec disposition: 6s CD; instant; off-GCD with Furor talent; -12% armor debuff; restores 8 Energy via Furor talent.**

Faerie Fire (Feral) is the armor-debuff source for Cat (NOT Mangle, which only applies the bleed-damage debuff). Equivalent to Sunder Armor / Expose Armor / Curse of Weakness in raid-debuff slot.

- G1 / G3 / G4 / G6 / G7 / G11: cast on pull and refresh as needed (debuff lasts 5min; pull-cast is sufficient unless a class with a different stack-source lapses).

**Disposition: cast Faerie Fire (Feral) on pull; refresh on debuff expire (rare; 5min duration). Furor talent provides +Energy on cast — bot can use Faerie Fire as an Energy-tap when low.**

### 4.23. Tranquility — emergency raid heal usage

**Spec disposition: 8min CD; raid AoE heal channel 8s; usable in caster form.**

Cata change: Tranquility was made baseline (not Resto-only) and CD reduced. Feral can break form and cast Tranquility for emergency raid healing. Significant DPS loss (10s in caster form including form-swap) but raid-saving in some cases.

- G1 / G3 / G4 / G6 / G7: emergency only; do not cast in normal rotation.
- G11: same; raid-leader-coordinated only.

**Disposition: Tranquility on raid-leader trigger only; not auto-cast in rotation.**

### 4.24. Innervate — self or healer?

**Spec disposition: 3min CD; cast on healer if low mana OR self if soloing.**

Innervate gives 20% mana over 10s. Cat's mana use is minimal (only out-of-form spells). Default raid use: cast on the lowest-mana healer (or designated healer per raid macro).

- All sources: cast on healer in raid; self in solo.

**Disposition: cast on healer in raid context; self if soloing.**

### 4.25. Rebirth — combat-res priority

**Spec disposition: 10min CD combat-res; cast in caster form (or via Predatory Swiftness instant).**

Rebirth is a 10min CD combat-res. Druid + DK + Warlock all have a combat-res; raid pool is 1 (10-man) / 3 (25-man) total raid uses per encounter.

- All sources: cast on raid-leader trigger for designated dead member.

**Disposition: raid-leader-coordinated; auto-cast off via Predatory Swiftness proc when triggered.**

### 4.26. Survival — Barkskin / Frenzied Regeneration / Predatory Swiftness Healing Touch / Survival Instincts

**Spec disposition (defensive layering):**
1. **Barkskin** — 1min CD; -20% damage 12s; instant; Cat or any form.
2. **Survival Instincts** — 3min CD; -50% damage 12s; talented; Cat or Bear form.
3. **Predatory Swiftness Healing Touch** — instant ~30% HP heal; consumes a finisher proc.
4. **Frenzied Regeneration (Bear-form)** — stance-dance to bear; HoT heal; significant DPS loss to swap form. Emergency only.
5. **Tranquility** — raid-wide; emergency only.

- G1 / G3 / G4 / G6: Barkskin on incoming damage windows; Survival Instincts on emergency.
- G7 / G11: same; Predatory Swiftness Healing Touch as constant-throughput baseline.

**Disposition: Barkskin reactive; Survival Instincts < 30% HP; Predatory Swiftness Healing Touch at HP ≤ 60% with proc available.**

### 4.27. Hibernate / Soothe / Cyclone — CC

**Spec disposition: PvE-niche CC tools.**
- **Hibernate** — 30s CD; sleeps Beast / Dragonkin targets; 40s duration; breaks on damage. Used on caster-add encounters with Beast-type adds.
- **Soothe** — 10s CD; calms enraged targets; removes enrage buff from boss/add.
- **Cyclone** — 0 CD (DR-shared); 6s incapacitate; PvP-tier; rare PvE use.
- **Entangling Roots** — 0 CD; 30s root; usable on adds (Predatory Swiftness instant for free).

- G1 / G3 / G4: Hibernate on Beast adds (rare in Dragon Soul); Soothe on Enraged-mechanic adds; Roots on adds.
- G6 / G7: same.

**Disposition: encounter-driven; bot logs Hibernate / Soothe / Cyclone / Roots as available CC tools.**

### 4.28. Pre-pot + combat-pot

**Spec disposition: Tolvir Potion T-2s pre-pull AND second Tolvir at 1-min mark when Berserk + Tiger's Fury align.** Tolvir grants +1200 Agility 25s. Volcanic Potion (intellect) is NOT used — Cat scales Agility.

### 4.29. Bloodlust / Heroism / Time Warp alignment

**Spec disposition: pre-pull cast by raid (default).** Cat benefits from haste during Bloodlust because Energy regen scales with haste (haste makes Energy-per-second go up). Stack Berserk + Tiger's Fury + Tolvir Potion + trinket inside Bloodlust window.

### 4.30. T13 set bonuses

**Spec disposition: 4-piece T13 = priority set goal.**
- **2P:** Mangle / Shred chance to grant +Energy on cast OR +damage on Rake — **UNVERIFIED**.
- **4P:** Tiger's Fury cooldown reduction OR Berserk extension — **UNVERIFIED**.

Goal: 4P T13; downgrade to 2P if better off-set tier-bonus stats are blocking gear.

### 4.31. Trinket pairing

**Spec disposition: any on-use Agility trinket aligned with Berserk + Tiger's Fury.** Vessel of Acceleration (passive crit), Kiroptyric Sigil (Agility on-use), Bone-Link Fetish (DS heroic Agility on-use), etc. Bot-side: trinket-firing logic should align with Berserk on-use.

### 4.32. Mark of the Wild — pre-buff

**Spec disposition: cast on raid pre-pull; 1hr buff; Druid-baseline.** Replaces Blessing of Kings / Embrace of the Shale Spider in raid-buff slot (or stacks; verify additive vs replacement). All Druid specs cast Mark of the Wild as the primary raid pre-buff.

### 4.33. Cleanse poisons / Remove Curse

**Spec disposition: Cure Poison (Resto) — Cat-form CANNOT cast. Bot must shapeshift to caster form to cleanse.** Out of form, Cat is significantly weakened (no melee). Default: skip cleanse-on-self in Cat-form rotation; rely on healers.

Exception: emergency dispel where the cost of NOT cleansing exceeds the form-swap DPS loss (e.g., poison that will kill the bot in 5s).

### 4.34. Stance-dance — when to swap to Bear?

**Spec disposition: rarely.**
- Frenzied Regeneration (Bear-form HoT) — emergency self-heal beyond Healing Touch.
- Bash interrupt — only if Skull Bash on CD AND interrupt is required. Rare.
- Bear-form survival (e.g., Hagara phase 1 lightning) — Bear has more armor + Stamina; some encounters favor bear-form for survival in phases.

**Disposition: skip stance-dance in default rotation. Encounter-script can trigger Bear-form for survival phases.**

### 4.35. Bleed snapshot rule (Cata 4.0 change)

**Spec disposition: bleeds NO LONGER snapshot in Cata 4.0+.**

Pre-Cata, Rip / Rake snapshotted the buff state at apply (Tiger's Fury active when Rip applied = Rip ticks for full duration with +15% damage). Cata 4.0 changed this: Rip / Rake refresh inherits CURRENT stats (not buff-state-at-apply); ticks during a buff window get +damage on the tick.

Implication: aligning Rip refresh with Tiger's Fury window is still valuable because the 8 ticks during the 6s Tiger's Fury overlap get +15%. But not as strong as pre-Cata snapshot. Bot-side: track buff state on tick, not on apply.

- G1 / G3 / G4 / G6 / G7 / G11: Cata 4.0 NO snapshot on bleeds; tick-based stat inheritance.

**Encoded: bleed ticks inherit current stats per-tick; align Tiger's Fury / Berserk windows for value-positive Rip refresh but no full-duration snapshot.**

### 4.36. CP-overflow rule

**Spec disposition: never let a builder push CP over 5.**

Building from 4 CP with Mangle/Shred when Primal Fury could crit-proc to 6 CP wastes the 6th CP. Bot-side: at 4 CP, prefer a finisher (Rip / Savage Roar / Ferocious Bite) BEFORE another builder, especially with high crit chance.

This is a soft rule — at 4 CP if all finishers are healthy (Roar > 5s, Rip > 5s, Bite irrelevant > 25%), continue building to 5 CP.

**Disposition: at 4 CP, finisher-priority first; if no finisher needs casting, build to 5 CP.**

### 4.37. Energy-pool management

**Spec disposition: stay below 100 Energy at all times to avoid capping.**

Energy regenerates at 10/sec base. Capping at 100 wastes regen. Bot-side rule:
- If Energy ≥ 90 AND Tiger's Fury off CD AND can cast Tiger's Fury, hold (don't waste TF restore).
- If Energy ≥ 60 AND a builder is available, cast builder.
- If Energy ≤ 35 AND Tiger's Fury off CD, cast Tiger's Fury for +60 Energy.
- During Berserk: -50% Energy cost, so lower threshold for Tiger's Fury (Energy ≤ 20 OR similar).

### 4.38. Shred without behind-target check — fallback to Mangle?

**Spec disposition: bot must check facing before each Shred cast; if cannot get behind, use Mangle Cat as fallback builder.**

Bot positioning logic: prefer rear-arc engagement; if positioning is blocked (boss with rear-arc cleave like Magmaw, or movement-locked add), Mangle Cat is the fallback. Mangle Cat is ~20% lower damage than Shred but omnidirectional.

---

## 5. Items NOT addressed (author judgment)

| Spec item | Value | Why |
|---|---|---|
| Cat Form | active throughout DPS rotation | Hard requirement. |
| Mark of the Wild | pre-pull; 1hr buff | Pre-buff. |
| Faerie Fire (Feral) | pull-cast; refresh on debuff expire (5min); Energy-tap on low | Always-on. |
| Mangle Cat | front-arc builder OR pull-opener for bleed-debuff | Conditional builder. |
| Shred | rear-arc primary builder | Primary builder. |
| Rake | refresh ≤ 3s remaining | DoT maintenance. |
| Rip | 5 CP refresh; pandemic ≤ 5s remaining; primary finisher | Finisher. |
| Ferocious Bite | target HP < 25%; 5 CP; Blood-in-the-Water Rip refresh | Execute. |
| Savage Roar | 1 CP refresh < 5s remaining; 100% uptime | Self-buff. |
| Tiger's Fury | Energy ≤ 35; off-GCD; +60 Energy + +15% damage 6s | Energy + buff. |
| Berserk | 3min CD; stack with Tiger's Fury + Bloodlust + Tolvir | Major DPS CD. |
| Pounce | stealth-only; opener stun + 1 CP | Opener. |
| Ravage | stealth or Stampede-from-TF; high-damage builder | Opener / Stampede proc. |
| Maim | CC stun finisher | Niche utility. |
| Skull Bash | 1min CD interrupt + gap-closer | Reactive interrupt. |
| Stampeding Roar | 3min CD raid speed CD | Encounter-driven. |
| Swipe (Cat) | AoE 3+; 50 Energy; 8y | Conditional AoE. |
| Thrash (Cat L81) | AoE bleed; verify Cat form availability at 4.3.4 | Conditional AoE. |
| Predatory Swiftness Healing Touch | HP ≤ 60% with proc available | Defensive. |
| Predatory Swiftness Rebirth | dead raid member; raid-leader trigger | Combat-res. |
| Predatory Swiftness Entangling Roots | add CC | Niche. |
| Barkskin | 1min CD reactive defensive | Reactive defensive. |
| Survival Instincts | 3min CD emergency defensive (HP < 30%) | Emergency defensive. |
| Frenzied Regeneration | Bear-form HoT; emergency only | Emergency stance-dance. |
| Tranquility | 8min CD raid heal; raid-leader trigger | Emergency utility. |
| Innervate | 3min CD; cast on healer in raid | Utility. |
| Rebirth | 10min CD combat-res; raid-leader trigger | Utility. |
| Hibernate | Beast/Dragonkin CC | Niche. |
| Soothe | enrage cleanse | Niche. |
| Cyclone | rare PvE CC | Niche. |
| Bash (Bear-form) | Skull Bash supersedes; rare | Niche. |
| Pre-pot Tolvir | T-2s + 1-min mark | Bot-side. |
| Stat reforge | Crit/Haste → Mastery; Hit/Exp keep | Standard. |
| Glyph of Bloodletting | Prime — Shred extends Rip | Default. |
| Glyph of Tiger's Fury | Prime — -CD on Tiger's Fury | Default. |
| Glyph of Rip | Prime — +Rip duration or damage | Default. |
| Glyph of Berserk | Prime alt — +Berserk duration | Encounter-tunable. |
| Glyph of Stampeding Roar | Major — +radius | Default. |
| Glyph of Pounce | Major — +stun duration | Default. |
| Glyph of Rebirth | Major — +HP on res | Default. |
| Glyph of Mark of the Wild | Minor — -mana | Cosmetic. |
| Glyph of Travel Form | Minor — usable indoors | Utility. |
| Glyph of Unburdened Rebirth | Minor — no reagent | Utility. |
| Cower | rare threat-drop | Niche. |
| Prowl (Stealth) | opener positioning | Opener. |
| Dash | Cat-form sprint; movement | Utility. |
| Travel Form | out-of-combat travel | Utility. |
| Aquatic Form | water travel | Utility. |
| Flight Form / Swift Flight Form | flying | Utility. |

---

## 6. Author-judgment items — leveling

```yaml
spell_unlocks:
  # Class baselines (Druid starts L1; relevant abilities for Feral Cat leveling)
  WRATH:                     {level: 1, spell_id: 5176}
  REJUVENATION:              {level: 3, spell_id: 774}
  HEALING_TOUCH:             {level: 1, spell_id: 5185}
  MOONFIRE:                  {level: 6, spell_id: 8921}
  MARK_OF_THE_WILD:          {level: 1, spell_id: 1126}
  THORNS:                    {level: 6, spell_id: 467}
  BEAR_FORM:                 {level: 8, spell_id: 5487}
  CAT_FORM:                  {level: 20, spell_id: 768}
  AQUATIC_FORM:              {level: 16, spell_id: 1066}
  TRAVEL_FORM:               {level: 16, spell_id: 783}
  FLIGHT_FORM:               {level: 60, spell_id: 33943}
  SWIFT_FLIGHT_FORM:         {level: 71, spell_id: 40120}
  TREE_OF_LIFE:              {level: 80, spell_id: 33891}  # Resto talent — N/A here
  REGROWTH:                  {level: 12, spell_id: 8936}
  REBIRTH:                   {level: 32, spell_id: 20484}
  INNERVATE:                 {level: 38, spell_id: 29166}
  HIBERNATE:                 {level: 18, spell_id: 2637}
  ENTANGLING_ROOTS:          {level: 8, spell_id: 339}
  CYCLONE:                   {requires_talent: true, tree: balance, spell_id: 33786}
  CURE_POISON:               {level: 14, spell_id: 8946}
  REMOVE_CURSE:              {level: 24, spell_id: 2782}
  TRANQUILITY:               {level: 70, spell_id: 740}
  STARFIRE:                  {level: 10, spell_id: 2912}
  SOOTHE:                    {level: 12, spell_id: 2908}
  BARKSKIN:                  {level: 44, spell_id: 22812}
  HURRICANE:                 {requires_talent: true, tree: balance, spell_id: 16914}
  REVIVE:                    {level: 12, spell_id: 50769}
  # Cat-form abilities (gated on Cat Form L20)
  CLAW:                      {level: 20, spell_id: 1082}      # Replaced by Mangle Cat
  MANGLE_CAT:                {level: 30, requires_spec: feral, spell_id: 33876}
  SHRED:                     {level: 22, spell_id: 5221}
  RAKE:                      {level: 26, spell_id: 1822}
  RIP:                       {level: 20, spell_id: 1079}
  FEROCIOUS_BITE:            {level: 32, spell_id: 22568}
  SAVAGE_ROAR:               {level: 24, spell_id: 52610}
  TIGERS_FURY:               {level: 18, spell_id: 5217}
  POUNCE:                    {level: 36, spell_id: 9005}
  RAVAGE:                    {level: 32, spell_id: 6785}
  MAIM:                      {level: 40, spell_id: 22570}
  COWER:                     {level: 20, spell_id: 8998}
  PROWL:                     {level: 20, spell_id: 5215}
  DASH:                      {level: 20, spell_id: 1850}
  SWIPE_CAT:                 {level: 30, requires_spec: feral, spell_id: 62078}
  THRASH_CAT:                {level: 81, requires_spec: feral, spell_id: 77758}  # Cata-new; verify cat-form
  # Bear-form abilities (gated on Bear Form L8)
  GROWL:                     {level: 8, spell_id: 6795}
  MAUL:                      {level: 8, spell_id: 6807}
  DEMORALIZING_ROAR:         {level: 8, spell_id: 99}
  CHALLENGING_ROAR:          {level: 16, spell_id: 5209}
  BASH:                      {level: 14, spell_id: 5211}
  ENRAGE:                    {level: 14, spell_id: 5229}
  FRENZIED_REGENERATION:     {level: 36, spell_id: 22842}
  MANGLE_BEAR:               {level: 30, requires_spec: feral, spell_id: 33878}
  LACERATE:                  {level: 65, spell_id: 33745}
  PULVERIZE:                 {requires_talent: true, requires_spec: feral_bear, spell_id: 80313}
  # Cataclysm-new (L60–L85)
  SKULL_BASH:                {level: 60, spell_id: 80964}  # Cat: 80964; Bear: 80965
  STAMPEDING_ROAR:           {level: 83, spell_id: 77761}  # Bear: 77761; Cat: 77764
  WILD_MUSHROOM:             {requires_talent: true, tree: balance, spell_id: 88747}  # Boomkin only
  EFFLORESCENCE:             {requires_talent: true, tree: restoration, spell_id: 81262}  # Resto only
  # Feral-spec talented abilities (level reqs are talent-tier, not character level)
  BERSERK:                   {requires_talent: true, requires_spec: feral, spell_id: 50334}
  PREDATORY_STRIKES:         {requires_talent: true, requires_spec: feral, passive: true}
  KING_OF_THE_JUNGLE:        {requires_talent: true, requires_spec: feral, passive: true}
  STAMPEDE:                  {requires_talent: true, requires_spec: feral, passive: true}
  PREDATORY_SWIFTNESS:       {requires_talent: true, requires_spec: feral, passive_proc: true}
  FERAL_AGGRESSION:          {requires_talent: true, requires_spec: feral, passive: true}
  PRIMAL_FURY:               {requires_talent: true, requires_spec: feral, passive_proc: true}
  ENDLESS_CARNAGE:           {requires_talent: true, requires_spec: feral, passive: true}
  SURVIVAL_INSTINCTS:        {requires_talent: true, requires_spec: feral, spell_id: 61336}
  BLOOD_IN_THE_WATER:        {requires_talent: true, requires_spec: feral, passive_proc: true}
  REND_AND_TEAR:             {requires_talent: true, requires_spec: feral, passive: true}
  NURTURING_INSTINCT:        {requires_talent: true, requires_spec: feral, passive: true}
  INFECTED_WOUNDS:           {requires_talent: true, requires_spec: feral, passive_proc: true}
  FURY_SWIPES:               {requires_talent: true, requires_spec: feral, passive_proc: true}
  # Cross-tree talents commonly taken
  FUROR:                     {requires_talent: true, tree: restoration, passive: true}  # Verify tree
  NATURALIST:                {requires_talent: true, tree: restoration, passive: true}
  NATURAL_SHAPESHIFTER:      {requires_talent: true, tree: restoration, passive: true}
  HEART_OF_THE_WILD:         {requires_talent: true, tree: restoration, passive: true}
  GENESIS:                   {requires_talent: true, tree: balance, passive: true}
  # Mastery / passives
  MASTERY_RAZOR_CLAWS:       {passive_mastery: true, base_pct: 8.0, per_point_pct: 3.13, requires_spec: feral}
  # Druid baseline (caster-form)
  FAERIE_FIRE:               {level: 18, spell_id: 770}
  FAERIE_FIRE_FERAL:         {level: 18, requires_form: feral, spell_id: 16857}
  STARFALL:                  {requires_talent: true, tree: balance, spell_id: 48505}
```

**Druid is a base class — starts at L1.**

**Pre-Cat Form (under L20):** Druid leveling pre-L20 is bear-form melee + caster-form Wrath/Moonfire. Bear Form is L8. Bot rotation L1–L19: caster-form Wrath / Moonfire ranged + Bear Form melee for close combat. No Cat-form rotation possible.

**Pre-Mangle Cat (under L30):** primary cat-form builder is **Claw** (L20 Druid baseline). Claw replaces Mangle in priority for L20–L29. Rotation L20–L29: Cat Form → Claw → Rake (L26+) → Rip (L20+) at 5 CP → Tiger's Fury (L18+).

**Pre-Shred (under L22):** Claw is the only cat-form builder. Front-arc / rear-arc positioning irrelevant.

**Pre-Rake (under L26):** no Bleed DoT builder; Rip (L20+) is the only Bleed available; Claw + Rip rotation only.

**Pre-Savage Roar (under L24):** no +30% damage self-buff; rotation is straight builder + finisher.

**Pre-Berserk (talent invest required):** no Berserk DPS CD; Tiger's Fury is the only damage CD.

**Pre-Predatory Swiftness (talent invest required):** no instant Healing Touch; Healing Touch requires caster-form swap (DPS loss).

**Pre-Stampede (talent invest required):** no Tiger's-Fury-Ravage proc; Ravage (L32+) is stealth-only.

**Pre-King of the Jungle (talent invest required):** Tiger's Fury / Berserk damage windows scale at base (no +15% damage from talent).

**Pre-Skull Bash (under L60):** no charge interrupt; interrupt option is Bash (Bear-form, L14+; stance-dance required).

**Pre-Stampeding Roar (under L83):** no raid speed CD; Dash (Cat-form L20) provides self-only +70% speed.

**Pre-Mastery (under L80):** no Razor Claws scaling — bleed damage at flat rate.

**Pre-Faerie Fire (Feral) (under L18):** no armor debuff; no Energy-tap.

**Pre-Tiger's Fury (under L18):** no Energy CD; rely on natural regen.

**Pre-Pounce (under L36):** no stealth opener stun; Prowl + Ravage (L32+) is the alternative; pre-L32, no opener at all.

**Pre-Predatory Strikes / Heart of the Wild (talent invest required):** AP scaling from weapon damage is at baseline only; no +AP boost.

**Pre-Tranquility (under L70):** no raid heal CD.

**Pre-Rebirth (under L32):** no combat-res; rely on Soulstone (Warlock) or Reincarnation (Shaman).

**Pre-Innervate (under L38):** no mana CD.

**Pre-Hibernate (under L18):** no Beast/Dragonkin CC.

**Pre-Barkskin (under L44):** no defensive CD; only Bear Form's higher armor pool for survival.

**Druid leveling rotation L1–L19 (caster + bear-form):**
- Caster: Wrath spam at range; Moonfire DoT; Healing Touch self-heal at low HP.
- Bear-form L8+: Maul on Rage; Demoralizing Roar; emergency Bear-form survival.

**Druid leveling rotation L20–L29 (Cat-form available):**
- Pull: Wrath at range OR Cat Form → Prowl (L20+) → Claw → continue.
- Combat: Claw spam + Rip (L20+) at 5 CP + Tiger's Fury (L18+) on CD.
- L24+: Savage Roar maintained at 1+ CP.
- L26+: Rake + Claw + Rip rotation.

**Druid leveling rotation L30+ (Feral spec invest):**
- Mangle Cat (L30) replaces Claw as front-arc builder.
- Shred (L22) becomes available as rear-arc (positional).
- L32+: Ferocious Bite + Ravage opener.
- L36+: Pounce stealth opener.
- L40+: Maim CC stun.
- L60+: Skull Bash interrupt + gap-closer.
- L83+: Stampeding Roar.

---

## 7. Suggested next research passes

1. Mangle Cat — bleed-damage debuff +30% to target; 60s duration; stacks with Trauma (Warrior) / Hemorrhage (Rogue Sub) — verify single-source-only or multiple-debuff.
2. Mangle Cat — does Mangle Cat apply armor-debuff (-12%)? Or only Faerie Fire (Feral) provides armor-debuff? Verify.
3. Shred — rear-arc-only requirement; +damage vs bleeding target — verify damage formula.
4. Rake — 9s base duration; tick every 3s; Endless Carnage 2/2 = 15s duration. Verify per-tick damage formula.
5. Rip — 16s base duration; tick every 2s = 8 ticks; 5-CP only. Verify damage formula.
6. Ferocious Bite — 5 CP base + up to +25 Energy for +100% damage. Blood in the Water 3/3 — under 25% HP, Ferocious Bite refreshes Rip.
7. Savage Roar — 12s base + 6s/CP duration; +30% damage in cat form; Endless Carnage adds +8s.
8. Tiger's Fury — 30s CD; +60 Energy; +15% damage 6s; off-GCD; cannot use during Berserk; King of the Jungle scales the +damage and adds +Energy regen?
9. Berserk (Cat) — 3min CD; 15s; -50% Energy cost; +Energy regen via King of the Jungle.
10. Pounce — stealth-only stun + 1 CP. 4s stun. 35 Energy.
11. Ravage — stealth or Stampede-enabled; +1 CP; high damage; 60 Energy.
12. Stampede — Tiger's Fury enables Ravage without stealth for 8s.
13. Predatory Swiftness — finisher chance for instant Healing Touch / Rebirth / Entangling Roots / Cyclone. 3/3 = 100% on 5-CP finisher.
14. Predatory Strikes — +AP based on weapon damage in feral forms.
15. King of the Jungle — Tiger's Fury and Berserk effects boosted.
16. Furor — +Energy on Cat-form shapeshift; +Mana for caster; Faerie Fire (Feral) +Energy on cast.
17. Endless Carnage — +Savage Roar duration (4s/8s); +Rake duration (3s/6s).
18. Primal Fury — Cat: crit on Mangle/Shred/Ravage = +1 CP; Bear: +rage on crit.
19. Feral Aggression — +Mangle damage + +Ferocious Bite damage / Maim duration.
20. Blood in the Water — < 25% HP, Ferocious Bite refreshes Rip; how often (every Bite or chance)?
21. Rend and Tear — +Maim and Ferocious Bite damage vs bleeding targets.
22. Infected Wounds — Mangle / Shred apply slow debuff to target (PvP utility).
23. Fury Swipes — chance on auto-attack to deal extra damage in cat or bear form.
24. Nurturing Instinct — Agility increases healing done; Stamina-from-Agility scaling.
25. Heart of the Wild (Resto-tree) — +Stamina + +Intellect + AP scaling — formula at 4.3.4.
26. Naturalist (Resto-tree) — +damage in cat/bear form (% boost).
27. Natural Shapeshifter (Resto-tree) — -mana cost on shapeshift.
28. Genesis (Balance-tree) — +5% periodic damage to all DoTs; affects Rake/Rip ticks.
29. Mastery: Razor Claws — 8% baseline + 3.13% per mastery point — verify formula.
30. Glyph of Bloodletting — Shred extends Rip 2s / cap 6s — verify.
31. Glyph of Tiger's Fury — -CD on Tiger's Fury (3s? 5s?).
32. Glyph of Rip — +duration (4s) OR +damage on ticks — verify.
33. Glyph of Berserk — +duration (5s) on Berserk.
34. Glyph of Stampeding Roar — +radius (15y → 30y).
35. Glyph of Pounce — +stun duration (4s → 7s).
36. Glyph of Maim — +duration per CP (1s).
37. Glyph of Ferocious Bite — extra Energy converts to extra damage / Rip extension.
38. Glyph of Mangle — +Mangle damage (% boost; rare).
39. Glyph of Shred — +Shred damage (% boost; rare).
40. Faerie Fire (Feral) — 6s CD; off-GCD with Furor; -12% armor; +8 Energy via Furor — verify.
41. Skull Bash (Cat-form) — 60s CD; charge interrupt; 13–25y range; -10% damage taken on next spell — verify.
42. Skull Bash (Bear-form) — same but Bear-form variant.
43. Stampeding Roar — 3min CD; +60% movement speed 8s; raid radius 15y / 30y with glyph.
44. Survival Instincts — 3min CD; -50% damage taken 12s; talented in Cat / Bear.
45. Barkskin — 1min CD; -20% damage taken 12s; instant; any form.
46. Tranquility (Cata change) — 8min CD; was Resto-only pre-Cata; raid AoE heal.
47. Innervate — 3min CD; +20% mana regen 10s; cast on healer.
48. Rebirth — 10min CD combat-res; consumes 1 raid combat-res charge.
49. T13 Feral 2P/4P — exact bonuses (UNVERIFIED in survey).
50. Cat Form damage scaling — base AP from weapon damage (Predatory Strikes); +Mastery bleed scaling; +King of the Jungle in TF/Berserk.
51. Energy regen — 10/sec base; +Haste scaling formula (haste increases auto-attack rate which indirectly increases Energy via clearcasting / no — Energy regen is haste-scaled directly).
52. Haste scaling for Energy regen — 1% haste = 1% faster Energy? OR auto-attack-based?
53. Combo Point retention — CP fall off if target changes? CP carry on target-swap?
54. Cower — threat reduction; rare use; verify reduction %.
55. Tolvir Potion — +1200 Agility 25s; 1min mark for second pot.
56. Bone-Link Fetish trinket — DS heroic Agility on-use; align with Berserk.
57. Vessel of Acceleration trinket — passive crit; pairs with Mastery > Crit > Haste priority.
58. Auto-attack contribution to total DPS — % share of damage from auto-attacks vs abilities.
59. Bleed snapshot rule (Cata 4.0 change) — confirm tick-based stat inheritance, not apply-snapshot.
60. Pandemic refresh window — last 30% of duration (5s on Rip; 3s on Rake) verified across guides.
61. Mangle bleed-debuff application — applies on cast; refreshes on Mangle re-cast; not extended by Shred.
62. Trauma (Warrior Arms) and Hemorrhage (Rogue Sub) — equivalent bleed-damage debuff sources; bot should detect external source and skip Mangle if active.
63. Savage Roar — Glyph of Savage Roar exists? +duration?
64. Berserk during Tiger's Fury — does Berserk CD prevent Tiger's Fury cast? OR Tiger's Fury is grayed during Berserk-effect? Verify ordering rule.
65. Predatory Swiftness proc — does proc consume on first eligible spell (Healing Touch, Rebirth, etc.) or only when manually fired? Verify proc-consumption mechanics.
66. Faerie Fire armor-debuff stacking — does Sunder Armor (Warrior) and Faerie Fire (Feral) provide redundant -armor or distinct stacks?
67. Cat Form swim-speed / movement speed — base; Dash effects.
68. Out-of-form spells affected by Predatory Swiftness — Healing Touch / Rebirth / Entangling Roots / Cyclone confirmed; what about Hibernate / Soothe / Cure Poison?
69. Thrash (Cat-form) — Cata 4.3.4 has Thrash in Cat? OR Bear-only? Verify.
70. Maim scaling — 5 CP = 5s stun? or 1s/CP? Verify.
71. Cower threat-drop value — 25%? 50%?
72. Hibernate breaks-on-damage — verify standard CC-break rule.
73. Cyclone diminishing returns — DR-shared with Hex / Polymorph / Fear?
74. Soothe usage — removes ALL enrage buffs or one stack? Verify.

---

## 8. DBC verification

(Spell IDs in §6.)

- Wrath — baseline. Spell.dbc 5176.
- Healing Touch — baseline. Spell.dbc 5185.
- Mark of the Wild — baseline raid buff. Spell.dbc 1126.
- Moonfire — baseline. Spell.dbc 8921.
- Rejuvenation — baseline. Spell.dbc 774.
- Regrowth — baseline. Spell.dbc 8936.
- Bear Form — baseline. Spell.dbc 5487.
- Cat Form — baseline. Spell.dbc 768.
- Travel Form — baseline. Spell.dbc 783.
- Aquatic Form — baseline. Spell.dbc 1066.
- Flight Form — Druid flying. Spell.dbc 33943.
- Swift Flight Form — Druid flying. Spell.dbc 40120.
- Faerie Fire — baseline caster-form. Spell.dbc 770.
- Faerie Fire (Feral) — feral-form variant. Spell.dbc 16857.
- Hibernate — baseline CC. Spell.dbc 2637.
- Entangling Roots — baseline. Spell.dbc 339.
- Cyclone — Balance/Resto talent CC. Spell.dbc 33786.
- Cure Poison — baseline. Spell.dbc 8946.
- Remove Curse — baseline. Spell.dbc 2782.
- Soothe — baseline enrage cleanse. Spell.dbc 2908.
- Tranquility — baseline raid heal. Spell.dbc 740.
- Rebirth — baseline combat-res. Spell.dbc 20484.
- Innervate — baseline mana CD. Spell.dbc 29166.
- Barkskin — baseline defensive. Spell.dbc 22812.
- Revive — out-of-combat res. Spell.dbc 50769.
- Thorns — baseline retaliation. Spell.dbc 467.
- **Cat-form abilities:**
- Claw — Cat-form L20 baseline builder. Spell.dbc 1082.
- **Mangle Cat — feral-spec L30; +30% bleed-damage debuff. Spell.dbc 33876.**
- **Shred — rear-arc primary builder. Spell.dbc 5221.**
- **Rake — Bleed DoT builder. Spell.dbc 1822.**
- **Rip — primary 5-CP finisher Bleed. Spell.dbc 1079.**
- **Ferocious Bite — execute finisher. Spell.dbc 22568.**
- **Savage Roar — +30% damage self-buff. Spell.dbc 52610.**
- **Tiger's Fury — Energy CD. Spell.dbc 5217.**
- **Pounce — stealth opener stun + builder. Spell.dbc 9005.**
- **Ravage — stealth/Stampede high-damage builder. Spell.dbc 6785.**
- **Maim — CC stun finisher. Spell.dbc 22570.**
- Cower — Cat-form threat-drop. Spell.dbc 8998.
- Prowl — Cat-form stealth. Spell.dbc 5215.
- Dash — Cat-form sprint. Spell.dbc 1850.
- Swipe (Cat) — feral-spec L30 AoE strike. Spell.dbc 62078.
- Thrash (Cat) — Cata-new L81 AoE bleed; verify Cat-form availability. Spell.dbc 77758.
- **Bear-form abilities:**
- Growl — Bear taunt. Spell.dbc 6795.
- Maul — Bear strike. Spell.dbc 6807.
- Demoralizing Roar — AoE -damage debuff. Spell.dbc 99.
- Challenging Roar — AoE taunt. Spell.dbc 5209.
- Bash — Bear stun. Spell.dbc 5211.
- Enrage — Bear self-buff. Spell.dbc 5229.
- Frenzied Regeneration — Bear HoT. Spell.dbc 22842.
- Mangle Bear — feral-spec L30; bear-form Mangle. Spell.dbc 33878.
- Lacerate — Bear Bleed builder. Spell.dbc 33745.
- Pulvarize — feral-bear talent. Spell.dbc 80313.
- **Cataclysm-new (L60–L85):**
- **Skull Bash (Cat) — charge interrupt. Spell.dbc 80964.**
- Skull Bash (Bear) — bear variant. Spell.dbc 80965.
- **Stampeding Roar (Bear) — raid speed CD. Spell.dbc 77761.**
- **Stampeding Roar (Cat) — Cat variant. Spell.dbc 77764.**
- **Feral talents:**
- **Berserk (Cat/Bear capstone). Spell.dbc 50334.**
- **Survival Instincts. Spell.dbc 61336.**
- Predatory Strikes — Feral talent. Talent.dbc.
- King of the Jungle — Feral talent. Talent.dbc.
- Predatory Swiftness — Feral talent. Talent.dbc.
- Stampede — Feral talent. Talent.dbc.
- Feral Aggression — Feral talent. Talent.dbc.
- Endless Carnage — Feral talent. Talent.dbc.
- Primal Fury — Feral talent. Talent.dbc.
- Blood in the Water — Feral talent. Talent.dbc.
- Rend and Tear — Feral talent. Talent.dbc.
- Nurturing Instinct — Feral talent. Talent.dbc.
- Infected Wounds — Feral talent. Talent.dbc.
- Fury Swipes — Feral talent. Talent.dbc.
- **Cross-tree talents:**
- Furor — tree placement varies by expansion; verify 4.3.4 placement (Resto?). Talent.dbc.
- Naturalist — Resto talent. Talent.dbc.
- Natural Shapeshifter — Resto talent. Talent.dbc.
- Heart of the Wild — Resto talent. Talent.dbc.
- Genesis — Balance talent. Talent.dbc.
- **Mastery:**
- Mastery: Razor Claws — Feral mastery. Mastery DBC.
- **Glyphs:**
- Glyph of Bloodletting (Prime). Glyph.dbc.
- Glyph of Tiger's Fury (Prime). Glyph.dbc.
- Glyph of Rip (Prime). Glyph.dbc.
- Glyph of Berserk (Prime). Glyph.dbc.
- Glyph of Mangle (Prime). Glyph.dbc.
- Glyph of Shred (Prime). Glyph.dbc.
- Glyph of Ferocious Bite (Prime). Glyph.dbc.
- Glyph of Stampeding Roar (Major). Glyph.dbc.
- Glyph of Pounce (Major). Glyph.dbc.
- Glyph of Maim (Major). Glyph.dbc.
- Glyph of Rebirth (Major). Glyph.dbc.
- Glyph of Frenzied Regeneration (Major). Glyph.dbc.
- Glyph of Mark of the Wild (Minor). Glyph.dbc.
- Glyph of Aquatic Form (Minor). Glyph.dbc.
- Glyph of Travel Form (Minor). Glyph.dbc.
- Glyph of Unburdened Rebirth (Minor). Glyph.dbc.
- Glyph of Dash (Minor). Glyph.dbc.
- Glyph of Challenging Roar (Minor). Glyph.dbc.
- T13 Feral 2P/4P. Item set DBC.
- Tolvir Potion. Item DBC.
- Mangle bleed-damage debuff. Spell.dbc.
- Tiger's Fury +damage buff. Spell.dbc.
- Berserk-period Energy buff. Spell.dbc.
- Savage Roar +damage buff. Spell.dbc.
- Stampede-from-Tiger's-Fury proc. Spell.dbc.
- Predatory Swiftness proc buff. Spell.dbc.
- Primal Fury crit→CP proc. Spell.dbc.
- Rake DoT debuff. Spell.dbc.
- Rip Bleed debuff. Spell.dbc.
- Faerie Fire -armor debuff. Spell.dbc.

---

## 9. Notes on prompt deviations

1. **Spec selection: Feral Cat (DPS half of shared Feral talent tree).** Bear/Guardian DEFERRED to tank phase per role-ordering rule (healer → ranged → melee → tank).
2. **Shared Feral tree at 4.3.4.** Cat and Bear share one talent tree; Cat-DPS picks Cat-favored talents (Predatory Strikes, King of the Jungle, Predatory Swiftness, Stampede, Furor, Endless Carnage, Primal Fury, Feral Aggression, Blood in the Water) and skips Bear-tank talents (Pulverize, Brambles, Thick Hide, Natural Reaction). Spec split into Feral-Cat-only / Guardian-Bear-only happened in Mists 5.0 — NOT in 4.3.4.
3. **Cat Form prerequisite.** All cat-form abilities gated on Cat Form active (L20 Druid baseline). Bot must enter Cat Form before DPS rotation; out-of-form spells (Healing Touch via Predatory Swiftness, Rebirth, Mark of the Wild) do NOT require form-swap if Predatory Swiftness proc is active.
4. **Energy + Combo Points resource model.** Identical to Rogue: Energy 0–100 regen 10/sec base; CP 0–5 on target. Bot must track both as event-driven flags.
5. **Agility primary stat (NOT Strength).** Feral Cat scales melee AP and crit from Agility — distinct from Warrior / DK / Paladin which scale Strength. Bot stat-comparison logic must distinguish.
6. **Mastery: Razor Claws as primary stat #1.** Bleed damage scales with Mastery (~50% of Cat's total damage is from Rake + Rip ticks). Bot reforge target: Mastery first.
7. **Positional Shred vs Mangle Cat.** Shred requires being behind target; Mangle Cat is the front-arc fallback. Bot positioning logic: prefer rear-arc engagement; fallback to Mangle Cat if blocked.
8. **Mangle bleed-damage debuff (+30%).** Mangle Cat applies +30% bleed-damage debuff to target for 60s. Bot must apply Mangle on pull (or after target-swap) to ensure Rake/Rip damage is uncrippled. Trauma (Warrior) / Hemorrhage (Rogue Sub) provide equivalent debuff from other classes — bot should detect external source and skip Mangle if active.
9. **Faerie Fire (Feral) is the armor-debuff source — NOT Mangle.** Cata 4.x assigns -12% armor debuff to Faerie Fire (Feral), not Mangle. Bot must apply Faerie Fire (Feral) on pull as raid-debuff slot (equivalent to Sunder Armor / Expose Armor / Curse of Weakness).
10. **Bleed snapshotting removed in Cata 4.0.** Rip / Rake refresh inherits CURRENT stats per-tick (NOT apply-snapshot). Aligning Rip refresh with Tiger's Fury / Berserk windows still has value because the ticks during the buff window get +damage, but no full-duration snapshot.
11. **Pandemic refresh — Rip 5s, Rake 3s.** Refresh DoT within last ~30% of duration to inherit remaining duration to cap; refreshing earlier wastes duration.
12. **Savage Roar +30% damage self-buff — 100% uptime mandatory.** Refresh at 1 CP if Roar < 5s remaining (don't waste 5 CP on Roar — Rip is higher value at 5 CP).
13. **Rip 5-CP-only finisher.** Never cast Rip below 5 CP; lower CP is a DPS loss.
14. **Glyph of Bloodletting — Shred extends Rip by 2s (cap 6s).** Mandatory Prime glyph. Bot must track Rip-extension accumulated from Shreds during Rip's life.
15. **Ferocious Bite + Blood in the Water execute.** At target HP < 25%, Ferocious Bite refreshes Rip via Blood in the Water 3/3. Execute rotation is Ferocious Bite spam (each cast refreshes Rip + deals direct damage).
16. **Tiger's Fury Energy gate.** Cast at Energy ≤ 35 to avoid capping the +60 Energy restore. Pull-Tiger's Fury aligned with Berserk for stacked damage window.
17. **Berserk pull-stack (Berserk + Tiger's Fury + Tolvir + Bloodlust + trinket).** First Berserk on pull stacks all major DPS modifiers. Subsequent Berserks (3min, 6min) stack only with Tiger's Fury; Bloodlust drifts.
18. **Stampede-Ravage from Tiger's Fury.** Stampede 2/2 enables Ravage usable without stealth for 8s after Tiger's Fury. Opener: Pounce (stealth) → Tiger's Fury → Ravage (Stampede-proc) → Mangle/Shred build.
19. **Predatory Swiftness instant Healing Touch self-heal.** 5-CP finisher procs 100% chance for next Healing Touch / Rebirth / Entangling Roots / Cyclone to be instant. Bot consumes on Healing Touch at HP ≤ 60%.
20. **Predatory Swiftness instant Rebirth.** Combat-res without breaking cat form (still requires raid-leader trigger).
21. **Skull Bash (Cata-new L60).** Charge interrupt; 13–25y range; 60s CD. Both interrupt AND gap-closer; usable in Cat or Bear form.
22. **Stampeding Roar (Cata-new L83).** Raid speed buff +60% movement 8s; 3min CD. Encounter-driven; raid-leader trigger.
23. **Tranquility (Cata change).** Made baseline (not Resto-only); 8min CD raid AoE heal. Feral can break form for emergency raid heal.
24. **Mark of the Wild raid pre-buff.** 1hr; pre-pull cast.
25. **Tolvir Potion (Agility) — pre-pot + 1-min mark.** NOT Volcanic Potion (intellect — irrelevant for Feral).
26. **Stat priority Mastery > Crit > Haste (consensus).** Minority view (G8 Noxxic) places Crit > Mastery; default to Mastery.
27. **Hit cap 8% melee-special.** Spell hit irrelevant — Cat DPS rotation never casts a damage spell. Predatory Swiftness Healing Touch is healing (no hit roll on friendly target).
28. **Expertise cap 26.** Shred is rear-arc no-parry but dodgeable; 26 expertise eliminates dodge.
29. **Bot-side: positional check before each Shred cast.** Front-arc → Mangle Cat fallback.
30. **Bot-side: Energy management.** Stay below 100 to avoid capping; cast Tiger's Fury at Energy ≤ 35.
31. **Bot-side: CP-overflow rule.** At 4 CP, prefer finisher over another builder if Primal Fury could crit-proc to 6 CP.
32. **Bot-side: Savage Roar maintenance.** Track Roar duration; refresh at 1 CP when Roar < 5s remaining.
33. **Bot-side: Rake pandemic refresh.** Track Rake duration; refresh ≤ 3s remaining.
34. **Bot-side: Rip refresh logic.** 5-CP only; pandemic ≤ 5s remaining; align with Tiger's Fury / Berserk window when possible.
35. **Bot-side: Mangle bleed-debuff tracking.** Detect external Trauma / Hemorrhage source; skip Mangle if active. Refresh at 60s if Druid is sole source.
36. **Bot-side: Bloodletting glyph extension tracking.** Each Shred during Rip-active extends Rip 2s (cap 6s). Bot tracks accumulated extension.
37. **Bot-side: stance-dance avoidance.** Default rotation never swaps to Bear form. Encounter-script can trigger Bear-form for emergency survival (Frenzied Regeneration) or fight-specific phases (Hagara phase 1 lightning).
38. **Bot-side: AoE switch threshold 3+ enemies.** Swipe (Cat) replaces Mangle/Shred at 3+ targets. Bleeds maintained on primary only — no AoE bleed-spread mechanic in Cata 4.3.4 Cat.
39. **Bot-side: Faerie Fire Energy-tap.** Furor talent makes Faerie Fire (Feral) an Energy-restore tool when Tiger's Fury is on CD and Energy is low.
40. **Bot-side: Predatory Swiftness proc consumption.** Proc lasts ~10s after finisher; consume on Healing Touch at HP ≤ 60% OR let proc fall (no penalty).
41. **Bot-side: Skull Bash auto-interrupt logic.** 60s CD; cast on enemy cast ≤ 300ms before completion + bot at 13–25y from target. Doubles as gap-closer when bot is at range.
42. **Bot-side: out-of-form spell handling.** Cure Poison / Remove Curse / Cyclone / Hibernate / Soothe require caster-form. Default rotation does NOT shapeshift; rely on healers for cleanse. Exception: emergency dispel where cost of NOT cleansing exceeds form-swap DPS loss.
43. **Cross-spec shared mechanics.** Mark of the Wild, Innervate, Rebirth, Tranquility, Faerie Fire (caster), Hibernate, Soothe, Cyclone, Healing Touch, Wrath, Moonfire, mana resource model are documented in `docs/specs/balance-druid.md` and `docs/specs/resto-druid.md`. Reuse encoded mechanics from those specs; do NOT re-encode.
44. **Tank role (Bear/Guardian) deferred.** Per role-ordering rule, Bear/Guardian tank documentation comes in the tank phase. Cat-DPS doc references Bear-form abilities only as stance-dance utility (Frenzied Regeneration, Bash) — not as primary rotation.
45. **Final melee-DPS spec covered.** With Feral Cat documented, all six Cata 4.3.4 melee DPS specs are covered: Combat Rogue, Fury Warrior, Retribution Paladin, Frost DK, Enhancement Shaman, Feral Cat Druid. Next research phase moves to ranged / healer / tank role coverage per project roadmap.

End of survey.

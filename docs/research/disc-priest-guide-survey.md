# Discipline Priest 4.3.4 — Multi-Guide Strategy Survey

Audit trail for the strategy decisions that will be encoded in `docs/specs/disc-priest.md`. The spec doc is the source of truth for the bot; this doc records *which guides agreed*, *where they disagreed*, and *which thresholds were the bot author's judgment call rather than guide consensus*.

Survey conducted 2026-05-02 by bot author. Numerical data (spell IDs, mana %, cast times, etc.) was sourced separately from Wowhead Cata Classic and Warcraft Wiki and is summarized here only where it directly informs strategy. Per project source registry, hard numbers come from a single canonical reference (Wowhead Cata Classic / Warcraft Wiki); strategy/rotation/threshold content is what's reconciled across the guides below.

**Critical-path note for the bot module.** Discipline Priest is the project's only healer where damage-dealing is *primary healing*, not Tier-9 support DPS. The "Atonement" mechanic — Smite/Holy Fire dealing damage that converts to healing on the lowest-HP nearby ally — is a Tier-2/3 healing tool, not a downtime filler. The healer-role doc (`docs/roles/healer.md`) and the project README both flag this; the spec doc must integrate it at the same priority level as Penance and Power Word: Shield, gated only on Atonement-target reachability and the cluster being injured. Every section below treats Atonement Smite as healing-class behavior.

---

## Sources

| # | Guide | URL | Author / community | Date / patch | Confidence (rationale) |
|---|---|---|---|---|---|
| G1 | Icy Veins — Cata Classic Discipline Priest Rotation, Cooldowns & Abilities | https://www.icy-veins.com/cataclysm-classic/discipline-priest-pve-rotation-cooldowns-abilities | Icy Veins editorial | Cata Classic re-release (4.3.4 mechanics) | High — primary editorial source; same publisher series as the Resto Shaman / Holy Pal survey baseline. Returned readable content. |
| G2 | Icy Veins — Cata Classic Disc Priest Stat Priority | https://www.icy-veins.com/cataclysm-classic/discipline-priest-pve-stat-priority | Icy Veins editorial | Cata Classic | High — gave concrete conversion ratios (128.05 Haste rating = 1% at 85; 179.28 Crit/Mastery = 1%) and HoT-tick haste breakpoints. |
| G3 | Icy Veins — Cata Classic Disc Priest Talent Builds & Glyphs | https://www.icy-veins.com/cataclysm-classic/discipline-priest-pve-spec-builds-talents-glyphs | Icy Veins editorial | Cata Classic | High — explicitly enumerates the two builds (Atonement vs Full Healing) with talent picks and glyph priorities. |
| G4 | wowtbc.gg — Cata Classic Disc Priest | https://wowtbc.gg/cata/class-guides/discipline-priest/ | wowtbc.gg community | Cata Classic | Medium — readable rotation/glyph content; ordered priority list (buffs → Archangel → Grace → core heals → Atonement) is the most explicit "what to cast" sequence in the survey. |
| G5 | Warcraft Wiki / Wowpedia — Discipline / Atonement / Power Word: Shield / Penance / Power Word: Barrier / Pain Suppression / Rapture / Smite / Holy Fire / Borrowed Time / Body and Soul / Power Infusion / Inner Focus / Inner Will / Inner Fire / Hymn of Hope / Divine Hymn / Prayer of Mending / Prayer of Healing / Flash Heal | https://warcraft.wiki.gg/wiki/Discipline (and per-spell pages) | wiki community | covers original Cata 4.0–4.3 patch notes plus retail | High for hard mechanics (spell IDs, level requirements, patch-note transfer percentages); not strategic. Each per-spell page checked individually. |
| G6 | Disciplinary Action — "SmiteSpec v. ShieldSpec: Builds & Rotations at 85" (Dec 2010) | https://disciplinaryaction.wordpress.com/2010/12/28/smitespec-v-shieldspec-builds-rotations-at-85/ + companion "Discipline Priest Builds for 4.0.1" | Disciplinary Action (community blogger) | original Cata launch (4.0.1 / 4.0.3) | High — era-correct community theorycraft; explicitly enumerates both spec builds and the rotation differences. The 4.0.1 build post quotes the 80% / 40% Atonement tooltip directly. |
| G7 | Type "H" For Heals — "Discipline 4.0.1 Guide" + "Atonement Improvements" | https://typehforheals.com/2010/10/06/discipline-4-0-1-guide/ + https://typehforheals.com/2010/12/22/atonement-improvements/ | community blogger | original 4.0.1 / 4.0.3 era | High for radius/breakpoint specifics — gave the original 8-yard radius and the December 2010 hotfix to 15 yards. Also discussed Penance haste breakpoints and the Renew 12.5%/37.5% tick-add thresholds. |
| G8 | World of Matticus — "Smite Healing with the Atonement Spec" + "Discipline & Archangel in Dragon Soul" (talesofapriest mirror search results) | https://worldofmatticus.com/smite-healing-with-the-atonement-spec/ | Matticus / Tales of a Priest blog | original Cata era (Dec 2010 + late 4.3 era) | Medium — gave the 50/100% damage-to-healing tooltip values and the "boss must be ≤8y from injured ally" caveat. Tales of a Priest sister-blog returned self-signed-cert; content reconstructed from search excerpts. |
| G9 | Warcraft Tavern — "PvE Discipline Priest Healer Guide & Overview" + "PvE Discipline Priest Talents, Builds & Glyphs" + "PvE Discipline Priest Stat Priority & Reforging" | https://www.warcrafttavern.com/cataclysm/guides/pve-discipline-priest-healer-guide/ | Warcraft Tavern editorial | Cata Classic | Medium — direct fetch returned 403; content reconstructed from Google search excerpts that quoted them. Same source format as the Holy Pal survey's G7. |
| G10 | gotwarcraft.com — Discipline Priest Cataclysm Guide | https://gotwarcraft.com/guides/classguides/disciplinepriest.php | gotwarcraft.com | original Cata era | Medium — gives stat-priority hierarchy and explicitly notes "31-point Discipline + 8 Holy + 2 Shadow" allocation. Glyph picks lean toward shield-spec rather than atonement-spec. |
| G11 | Warmane Forum — "Cataclysm Atonement Discipline 4.3.4 PvE Guide" (thread 298687) | https://forum.warmane.com/showthread.php?t=298687 | private-server community | original 4.3.4 (Warmane is a long-running 4.3.4 private server) | Medium — exact-version match to the bot's target patch; direct fetch 403'd, content reconstructed from search excerpts. Specifically calls out the Atonement-spec rotation as PW:S → Archangel → Penance → Flash → reshield → spam Holy Fire/Smite. |
| G12 | wowbesttalentguides.blogspot.com — "PVE Discipline Priest Talent Build WoW Cata 4.3.4" (Mar 2014) | http://wowbesttalentguides.blogspot.com/2014/03/PVE-DISCIPLINE-PRIEST-TALENT-BUILD-WOW-CATACLYSM-4-3-4.html | community blogger | original 4.3.4 (Mar 2014 — same private-server era as the Holy Pal G9) | Low–Medium — short-form, era-exact (private-server 4.3.4 audience). Glyph and stat-priority picks match the higher-confidence sources. |
| G13 | jinxedthought.blogspot.com — "Discipline Priest Cataclysm Talent Build" (Dec 2010) | http://jinxedthought.blogspot.com/2010/12/discipline-priest-cataclysm-talent.html | community blogger | original 4.0.3 era | Medium — gives an explicit "Inner Focus + Train of Thought" talent pairing rationale and discusses Mental Agility vs Veiled Shadows trade-off. Useful for Tier-3/4 talent picks. |

**Caveats on coverage:**
- G9 (Warcraft Tavern) and G11 (Warmane forum thread 298687) returned 403/redirect to direct fetch; content reconstructed from search excerpts that quoted the pages verbatim.
- The wowpedia.fandom.com mirror of Atonement (`wowpedia.fandom.com/wiki/Atonement`) and the Warmane forum thread t=291151 both 403'd; not used as primary sources.
- `talesofapriest.com` (G8 sister blog) returned a self-signed-certificate error; relevant content was instead picked up from search-result excerpts.
- The Wowhead Cata Classic Discipline overview page (`/cata/guide/classes/priest/discipline/healer-overview-pve`) returned only header/navigation in the body; not used as a primary strategy source.
- None of the surveyed guides gave concrete numerical thresholds for self/tank/raid HP-emergency, mana conserve %, mana crisis %, drink %, or Atonement-target-cluster size. Those remain bot-author judgment.
- Hard spell numbers (level requirements, mana % of base, cooldowns, radii) are sourced from Warcraft Wiki + Wowhead and cited inline rather than as a separate table.

---

## Strong consensus (≥3 of the strategy-bearing guides agree)

The strategy-bearing guides for consensus counting are G1, G3, G4, G6, G7, G8, G9, G10, G11, G13 (ten sources). G2 covers stats only, G5 covers hard numbers only, G12 is short-form. "x/n" below counts only sources that addressed the item.

| Item | Consensus | Spec disposition |
|---|---|---|
| **Discipline is the only viable healing tree** for Disc-style play; talents outside Discipline are minimal (≤8 points) | 8/8 | Spec build is **31 Disc core + 7 Holy off-tree + 3 Shadow off-tree** (Cata uses 41-point talent trees with 31-point spec gates; secondary tree allocation matches G3 / G6 / G10 / G13 consensus). |
| **Atonement Smite IS healing**, not downtime filler — primary tool when (a) bot is mana-positive, (b) the boss has at least one injured ally within 15y | 5/5 (G3, G4, G6, G8, G11) | **Tier 3 healing action** in the spec (between Penance/PW:S and Greater/Flash Heal). Predicate: `target_in_range AND atonement_cluster_has_injured_ally AND no_emergency_pending`. Cross-link to `docs/roles/healer.md` Atonement-as-primary-healing exception. |
| **Atonement heals lowest-HP ally within 15y of the enemy struck**, smart-heal selection (the priest cannot manually choose target) | 4/5 that gave a number (G7, G8, G11, plus search reconciliation of G5 patch notes; G6 doesn't restate after the early-Cata 8y → 15y hotfix) | Spec: Atonement-eligible ally cluster = `enemies_in_range_for_smite WHERE any_friendly_within_15y_is_injured`. The bot does NOT need to model which specific ally Atonement will hit (it's smart-heal); it just needs the predicate "≥1 ally in 15y of the enemy is below cluster_hp_pct". |
| **Power Word: Shield is the rotational opener and Weakened Soul-gated maintenance** — cast on tank when Weakened Soul is missing; consumes Rapture cooldown for mana | 9/9 | **Tier 2** — `target == tank AND NOT target.has_weakened_soul AND mana_pct > mana_crisis_threshold`. Refresh-on-expiry pattern. |
| **Penance on cooldown** — instant-channel smart heal (heal on ally / damage on enemy), high HPS-per-mana | 9/9 | **Tier 2** — `Penance.cd == 0 AND any_injured_ally`; Atonement-spec note: when bot is in damage rotation and no ally is critically injured, Penance can fire on enemy for Evangelism stack. |
| **Prayer of Mending on cooldown** — fire-and-forget bouncing HoT, free healing when there's any incoming damage | 8/8 | **Tier 2** — `PoM.cd == 0 AND in_combat`. Target = tank for first bounce. |
| **Borrowed Time talent → 14% spell haste for next cast** after PW: Shield (2/2 ranks at 7%/14%, 6s buff window) | 5/5 (G1, G3, G5, G6, G7) | Maintenance / opener pattern — bot's pre-pull / opener sequence is `PW:Shield(tank) → next-cast-while-buffed`, where the next cast biases toward Penance or Greater Heal (whichever benefits most from haste). Borrowed Time is auto-consumed by the next cast; bot doesn't need to track the buff explicitly, just chain the cast. |
| **Rapture talent → 7% max mana on shield expiry, 12s ICD** (3 ranks; max rank used in PvE) | 4/4 (G1, G3, G5, G6) | Mana model — bot treats `PW:Shield(tank)` as net-mana-positive when Weakened-Soul-cycle has been ≥12s since last Rapture proc. Not an explicit predicate; folds into Tier 2 PW:S decision. |
| **Evangelism stacks via Smite / Holy Fire / Penance** — up to 5 stacks, increases damage / reduces mana cost | 5/5 (G3, G4, G5, G6, G8) | Spec encodes Atonement-spec rotation: `if evangelism_stacks < 5: prefer Smite/Holy Fire over Atonement-target-less builders`. |
| **Archangel at 5 stacks of Evangelism** — consumes stacks, restores 1% mana per stack and +3% healing per stack for 18s; 30s CD | 5/5 (G3, G4, G6, G8, G11) | **Cooldown profile** — fire when `evangelism_stacks == 5 AND archangel.cd == 0`. Treat as a healing-burst CD, not an emergency CD. |
| **Power Word: Barrier** is the raid AoE-emergency shield (10s zone, 25% damage reduction, 3-min CD) | 6/6 (G1, G3, G4, G5, G10, G11) | **Tier 1 raid emergency CD** — `raid.injured_count >= 4 AND raid.avg_hp_pct < 60 AND PWB.cd == 0`. Author judgment on the count/HP gates (no guide quantified). |
| **Pain Suppression** is the tank emergency CD (40% damage reduction, 8s, 3-min CD) | 6/6 (G1, G3, G4, G5, G10, G11) | **Tier 1 tank emergency CD** — `tank.hp_pct < 40 AND tank_taking_predictable_burst AND PainSup.cd == 0`. Author judgment on HP gate. |
| **Inner Focus** — next Flash Heal / Greater Heal / Binding Heal / Prayer of Healing is free (0 mana) and +25% crit; 45s CD (Cata version) | 4/4 (G1, G3, G5, G13) | **Cooldown profile** — fire when `IF.cd == 0 AND about_to_cast(GreaterHeal OR PrayerOfHealing)`. With Train of Thought talent, every Greater Heal cast reduces IF cooldown by 5s — so IF effectively pairs with Greater Heal spam during heavy tank damage. |
| **Inner Fire (default)** vs **Inner Will (movement / instant-cast phases)** — mutually exclusive buffs, only one active | 5/5 (G1, G4, G5, G6, G11) | Maintenance — bot defaults to `Inner Fire`; switch to `Inner Will` when in a high-movement phase (currently a manual command override; bot has no encounter-script awareness). |
| **Body and Soul talent → speed buff after PW:Shield** (40% / 60% movement speed, 4s) — useful for Beacon-of-tank or self-mobility | 4/4 that addressed it (G3, G5, G6, G13) | Talent pick is justified for the speed buff (utility) AND because it sits on the tier-3 Discipline path needed for higher-tier picks. No bot-side rotation logic — passive talent. |
| **Power Infusion** (Disc tier 3, +20% spell haste + 20% reduced mana cost for 15s, 2-min CD) — a self/ally throughput CD | 4/4 (G3, G5, G10, G13) | **Cooldown profile** — fire when `PI.cd == 0 AND (heavy_healing_window OR atonement_burst_phase)`. The bot self-targets by default (matches Disc PvE convention; in PvP / niche raids you'd buff a DPS, but the bot's role is healer-first). |
| **Hymn of Hope** is the mana CD (8s channel, 6-min CD, +15% max mana + 2% mana per tick to 3 nearby low-mana allies) | 4/4 (G1, G5, G10, G11) | **Tier 1 mana CD** — fire when `self.mana_pct < 30 AND no_active_emergency AND HoH.cd == 0`. Matches resto-shaman cross-class convention for mana-CD threshold. |
| **Shadowfiend** is a secondary mana CD (5-min CD; melee pet drains mana from enemy on hit) — fire alongside / before Hymn of Hope | 4/4 (G1, G5, G10, G11) | **Tier 2 mana CD** — fire when `self.mana_pct < 50 AND in_combat_with_enemy AND Shadowfiend.cd == 0`. More aggressive threshold than HoH because Shadowfiend's CD is shorter and the mana return is significant. |
| **Divine Hymn** is the raid healing CD (8s channel, 8-min CD, heals 5 lowest-HP allies + 10% healing-received buff) | 4/4 (G1, G5, G10, G11) | **Tier 1 raid emergency CD** — fire when `raid.injured_count >= 5 AND raid.avg_hp_pct < 50 AND DH.cd == 0`. Lower priority than PWBarrier (cast 30s after PWB ends if raid still hurts). |
| **Desperate Prayer** is the personal-survival CD (instant self-heal, 2-min CD) | 5/5 (G1, G3, G5, G10, G11) | **Tier 1 self-emergency CD** — fire when `self.hp_pct < 35 AND DP.cd == 0`. |
| **Stat priority: Intellect > Spirit > Haste > Mastery > Crit** for shield/tank-heal builds; **Haste > Mastery flips for Atonement build** | 4/5 (G2, G4, G10, G12) — G2 is unambiguous Int > Spirit > Haste > Mastery > Crit; G3 / G11 explicitly call out the Atonement-build twist (Haste over Mastery); G10 says "mastery, crit, haste roughly equal" which is an outlier | Spec encodes the **shield-spec** stat priority as the default (Int > Spirit > Haste > Mastery > Crit) because the bot's full-healing spec is the dungeon/leveling default. **Atonement-spec stat priority** is documented as a secondary preset: Haste > Mastery (because Atonement scales with damage done, which scales with Haste's cast-rate gain rather than Mastery's absorb-shield bonus). |
| **Mastery: Shield Discipline** — increases potency of all your absorption shields (PW:Shield, Divine Aegis, PW:Barrier was originally affected but post-4.0.3a it provides damage reduction not absorb so no longer scales with Mastery) | 3/3 that gave detail (G2, G3, G5) | Bot doesn't model absorb shields directly. Spec notes that Mastery biases "extra-shielding" benefit from PW:Shield + Divine Aegis from Prayer of Healing crits. No special engine logic; folds into stat-priority gear advice. |
| **Divine Aegis** — crit heals create absorb shields for 30% of healing done; Prayer of Healing crit applies it (twice if PoH itself crits, for 60% absorb total) | 3/3 (G3, G4, G5) | Passive talent — no bot-side rotation logic. Influences Prayer of Healing target-selection (favor Prayer of Healing over Greater Heal during cluster-damage windows because shield retention amortizes over time). |
| **Grace** — 3 stacks on a target after Penance/Heal/Flash Heal, increases healing received by tank by 9% (3% per stack) | 4/4 (G3, G4, G5, G6) | Bot doesn't explicitly model Grace stacks but the spec biases the bot to **keep healing the same target** (tank) when in a sustained-damage phase rather than spreading single-target heals around — Grace amortizes. |
| **Glyph priority — Prime: Penance + Power Word: Shield + (Prayer of Healing OR Power Word: Barrier)** | 5/5 (G1 (search excerpts), G3, G4, G10, G12) | Spec uses **Penance + PW:Shield + Prayer of Healing** for shield-spec; **Penance + PW:Shield + (Prayer of Mending alt)** for atonement-spec. Glyph of Power Word: Barrier appears in G10/G12 as an alternate prime. |
| **Glyph — Major: Divine Accuracy + Smite + Prayer of Mending** for Atonement build; **Major: Pain Suppression + Prayer of Mending + Fade** for Shield build | 4/4 that listed major glyphs (G3, G4, G10, G12) | Spec encodes Atonement-build Major glyphs as default (matches the bot's "Atonement Smite is healing" mandate). Glyph of Smite gives +20% damage to Smite when target has Holy Fire DoT — feeds Atonement healing throughput directly. |
| **Glyph — Minor: Fortitude + Shadowfiend + Levitate** | 3/3 (G3, G10, G12) | Spec encodes these as default minor glyphs. No rotation impact. |
| **Consumables — Flask of the Draconic Mind, Seafood Magnifique Feast (or Severed Sagefish Head), Mythical Mana Potion** | 4/4 (G1, G4, G10, G11) | Spec encodes these as the default consumable set. Matches Holy Pal / Resto Shaman conventions. |
| **At 5 stacks Evangelism, fire Archangel before Smite-spam continues** — i.e., consume the buff before it expires | 5/5 (G3, G4, G6, G8, G11) | Spec rotation rule: `evangelism_stacks == 5 → cast Archangel within 1s` (override Tier-3 Atonement-Smite for one GCD). After Archangel fires, the +3% per-stack healing buff is active for 18s; bot returns to Atonement-rotation prioritizing Smite for Evangelism re-stack. |
| **Smite > Holy Fire as Evangelism builder when Holy Fire DoT is active** (Glyph of Smite +20% damage on Holy Fire-debuffed targets) | 4/5 (G3, G4, G6, G11; G8 is silent on the glyph specifically) | Spec rotation: `Holy Fire on cooldown (10s)` then `Smite as filler`. Holy Fire applies the DoT that buffs Smite's damage. If Glyph of Smite is equipped (default for atonement build), Smite priority on Holy-Fire-debuffed enemy is +20% damage / +20% Atonement healing. |

---

## Disagreements

### 1. Default healing build — Atonement-spec vs Shield-spec / Full-Healing

- **G3 (Icy Veins talent page)**: presents BOTH builds explicitly — "Atonement Build" (focuses on dealing damage via Holy Fire and Smite) vs "Full Healing Spec" (omits Atonement, recommended for 10-man content with Strength of Soul + Train of Thought).
- **G4 (wowtbc.gg)**: rotation lists Atonement as part of the priority, implying Atonement-build default.
- **G6 (Disciplinary Action)**: ShieldSpec "feels intuitive ... similar to Wrath disc healing style" — author preferred ShieldSpec for 10-mans, called SmiteSpec "new and different" but acknowledged tank-healing reliability concerns.
- **G8 (World of Matticus / wtsheals)**: explicitly recommends Atonement-spec for 25-man (with 5-6 healers backing you up, "you can usually Smite away and end up topping the meters"); ShieldSpec for 10-man tank-heal assignments because "Atonement doesn't reliably heal the tank."
- **G10 (gotwarcraft)**: leans Shield-spec but notes Atonement "alternative build."
- **G11 (Warmane 4.3.4 forum)**: explicitly Atonement-spec.
- **G13 (jinxedthought)**: Shield-spec (Inner Focus + Train of Thought heavy).

**Likely explanation**: This is a real spec-pick disagreement, not a documentation gap. Atonement-spec maximizes throughput and contributes raid damage at the cost of tank-healing reliability; Shield-spec is the safer, mana-efficient choice for any encounter where the tank is the priority target.

**Spec disposition**: **Atonement-spec is the bot's default build.** Rationale: (a) the project's mandate per `docs/roles/healer.md` calls Atonement Smite "primary healing, not Tier-9 support DPS" — encoding Shield-spec as default would contradict the explicit project framing; (b) the bot will be running 5-mans (LFD) plus 10/25-man raids — Atonement throughput shines in 25s and is no worse than Shield-spec in 5-mans where the tank is also generally in melee range of the boss (within Atonement's 15y radius); (c) Shield-spec is encoded as a **secondary preset** for tank-heal-assignment encounters where the tank is the only injured target and the bot needs maximum-reliability single-target output. Switching presets is a respec, not a runtime decision.

### 2. Atonement radius — 8 yards vs 15 yards

- **G7 (Type "H" For Heals 4.0.1 guide)**: "Atonement heals the lowest-health player within 8 yards of the boss." Caveat: "Tanks have to stand very close to be within 8 yards of the boss, so they might be out of range, or melee might be taking damage and be soaking up the Atonement heals."
- **G8 (World of Matticus)**: "Atonement triggers healing equal to the damage dealt for allies within 8 yards of that target." (Same era.)
- **G7 / G5 patch notes (2010-12-20 hotfix)**: range increased to 15 yards.
- **G11 (Warmane 4.3.4 thread, search excerpt)**: "heals a nearby low health friendly target within 15 yards from the enemy target equal to 100% of the damage dealt."
- **G5 (Warcraft Wiki patch history)**: confirms patch 4.3.0 (2011-11-29) added "Atonement was updated to account for the target enemy's combat reach when calculating proper range, enabling it to be used on large creatures such as Ragnaros and Al'Akir."

**Likely explanation**: 8 yards was the launch (4.0.0) value; the December 2010 hotfix raised it to 15 yards; the 4.3.0 combat-reach fix made the range work on huge bosses. The bot targets 4.3.4, which is post-all-of-these.

**Spec disposition**: **Atonement radius = 15 yards from the enemy hit, with combat-reach extension on large NPCs.** The bot's predicate for Atonement-eligibility is `friendly_in_15y_of_enemy_hitbox AND friendly.hp_pct < cluster_threshold`. Combat-reach is engine-handled by the spell-cast itself; the bot doesn't need to model it.

### 3. Atonement healing percentage — 50% vs 80% vs 100%

- **G6 (Disciplinary Action 4.0.1)**: tooltip quoted as "Heal a nearby target (or yourself) for 80% (40% for yourself) each time you Smite."
- **G7 (Type "H" For Heals)**: implies parity with damage done, doesn't quote a percentage explicitly.
- **G8 (World of Matticus)**: "the conversion rate was adjusted during development, eventually settling at 50/100% of damage as healing."
- **G11 (Warmane 4.3.4 search excerpt)**: "100% of the damage dealt. If the Priest is healed through Atonement, the effect is reduced in half."
- **G5 (Wowpedia / Warcraft Wiki)**: confirms patch progression — 50% of damage initially, raised over the patches; in 4.3.4 the live values are 100% to allies / 50% self-heal.

**Likely explanation**: Atonement was buffed multiple times across the Cataclysm patch cycle. 4.0.1 launch was 50%/25% (talent-rank dependent — 1/2 ranks giving 25% / 50%? unclear). G6's "80%" is a beta or pre-launch value; the 4.0.6 hotfix and later 4.2 buffs raised the live value to 100% ally / 50% self.

**Spec disposition**: **4.3.4 live values = 100% damage-to-healing on ally, 50% on self.** Spec encodes this for the throughput estimator that decides "Atonement Smite vs traditional heal" tier ranking. **UNVERIFIED — flagged for DBC verification.** The exact 4.3.4 talent ranks (Atonement is 1/2 ranks at 50%/100%? or fixed at 100%?) need confirmation against `Spell.dbc` SpellEffect entries for spell ID 81749 / 81751 (the Atonement passive AND the actual heal-execute spell).

### 4. Penance haste breakpoint — does Penance get an extra tick from haste?

- **G7 (Type "H" For Heals)**: "Penance has a 25% haste breakpoint which grants an additional tick. However, on 4.0.1 Live this extra Penance tick at 25% total haste appears not to be the case ... this might be a bug." Author noted reaching 54.6% total haste and only getting 3 ticks.
- **G2 (Icy Veins stat page)**: Lists HoT-tick haste breakpoints (12.51% / 37.51% / 62.47% / 87.45% for 1/2/3/4 extra ticks) but these are for Renew, not Penance.
- **G6 / G8 / G10**: silent on Penance breakpoints.
- **G5 (Wowpedia)**: doesn't address Penance ticks-from-haste in the Cata-era page.

**Likely explanation**: Penance's tick count is hard-coded to 3 in Cataclysm (channel time scales with haste, but you don't gain a 4th tick). The "25% breakpoint" in G7 was a 4.0.1-beta misreading that didn't survive to Live. The 12.51%/37.51%/62.47% breakpoints are real for HoTs (Renew) but don't apply to Penance.

**Spec disposition**: **Penance does NOT gain ticks from haste in 4.3.4.** Bot treats Penance as a fixed-3-tick channel; haste shortens the channel. **No haste breakpoint to chase for Penance** — bot's haste recommendation is "as much as you can stack to soft-cap the GCD at 1.0s," not a Penance-specific number. The Renew breakpoint (12.51%) is informational only — Disc Priests rarely cast Renew unless dipping into the Holy tree, and the bot's spec does not pick up Renew (Tier 1 Holy talent, but bot's 7-Holy split goes to Divine Fury / Holy Concentration / Inspiration, not Renew). **UNVERIFIED — flagged.** Confirm Penance tick count is hard-coded to 3 in `Spell.dbc` for spell ID 47540.

### 5. Talent allocation — secondary tree split

- **G3 (Icy Veins)**: 31 Disc + 8 secondary; doesn't pin Holy vs Shadow split explicitly.
- **G6 (Disciplinary Action 4.0.1)**: lists "Improved PW:Shield, Grace, Rapture, Divine Aegis, Darkness, Power Word: Barrier, Soul Warding" as the Disc core. Secondary: Holy Concentration / Improved Healing not explicitly itemized but implied via "filler."
- **G10 (gotwarcraft)**: explicitly "31 Disc + 8 Holy + 2 Shadow."
- **G11 (Warmane 4.3.4)**: implies Atonement build with Twin Disciplines 3/3, Atonement 2/2, plus Inner Focus.
- **G13 (jinxedthought)**: "Divine Fury (off-tree) buffs both Smite and Greater Heal," "Darkness (Shadow) provides crucial haste," "Empowered Healing." Implied 8 Holy + 2 Shadow with Divine Fury 3/3 + Empowered Healing 2/2 + Holy Concentration / Inspiration; Shadow 2 = Darkness 2/2 (3% haste).
- **G3 / G6**: don't address Shadow split.

**Likely explanation**: 31 Discipline is fixed; the secondary tree split is a 7/3 or 8/2 (Holy / Shadow) optimization. Holy 7-8 picks Divine Fury (faster Smite/Greater Heal cast) + Empowered Healing (more healing on Greater Heal/Flash Heal/Heal). Shadow 2-3 picks Darkness (3% haste, the only good off-tree option without spending too deep). Alternate: 8 Holy + 0 Shadow if you skip Darkness.

**Spec disposition**: **31 Disc + 7 Holy + 3 Shadow** (matches G10 / G13 reconciled).
- Discipline 31 = Atonement-build core: Twin Disciplines 3/3, Imp PW:Shield 3/3, Mental Agility 3/3, Soul Warding 1/1, Inner Focus 1/1, Inner Sanctum 2/2, Evangelism 2/2, Body and Soul 2/2, Divine Aegis 1/1, Borrowed Time 2/2, Atonement 2/2, Train of Thought 3/3, Strength of Soul 0/2 (skipped for Atonement build), Rapture 3/3, Grace 2/2, Pain Suppression 1/1, Renewed Hope 2/2, Power Word: Barrier 1/1, Reflective Shield 0/2 (skipped), Power Infusion 1/1, Archangel 1/1.
- Holy 7 = Divine Fury 3/3 + Empowered Healing 2/2 + Holy Concentration 2/2.
- Shadow 3 = Darkness 3/3 (3% haste).
- **Note**: tier counts and specific talent ranks may differ slightly from above pending the talent-calculator double-check; this is the survey's reconciliation, not a literal point-by-point allocation. Author recommends running this through the Cata talent-calculator at build time.

### 6. Atonement-target reliability — does the bot have to confirm the right target?

- **G6 (Disciplinary Action)**: "you'll need to switch to healing them directly when Atonement isn't reaching them effectively."
- **G8 (World of Matticus / wtsheals)**: "A critical limitation of Atonement healing is its unpredictability. The spec uses smart healing that targets the lowest health raid member within range — occasionally misdirecting heals away from threatened tanks during dangerous moments."
- **G11**: silent on the failure mode.
- **G3 / G4**: silent on the failure mode.

**Likely explanation**: Atonement is smart-heal and the priest cannot manually pick the target. If the tank is the most-injured ally in 15y, Atonement heals the tank (good); if a melee DPS is more injured, Atonement heals them and the tank gets nothing (bad).

**Spec disposition**: **Atonement-spec rule — when the tank is below `tank_priority_threshold` (default: 75%), Tier 2 actions (Penance, PW:Shield, Greater Heal) outrank Tier 3 Atonement Smite.** This means: if the bot is in Atonement-rotation and the tank takes a hit that drops them below 75%, the bot abandons Smite for one or more GCDs and casts on the tank directly. This rule survives the Atonement-target-may-not-be-tank failure mode. Threshold is bot-author judgment; flag for tuning.

### 7. Holy Fire vs Smite priority during damage rotation

- **G4 (wowtbc.gg)**: "Cast Holy Fire and Smite during low periods of damage. Prioritize Smite on targets afflicted with Holy Fire because of Glyph of Smite."
- **G6 (Disciplinary Action SmiteSpec)**: "Smite for Evangelism, use up our Archangel, start shielding and healing, and then go back to smiting."
- **G8 (World of Matticus)**: "The suggested opener involves casting Holy Fire first, then spamming Smite."
- **G11 (Warmane 4.3.4)**: "spam Holy Fire/Smite on your enemy."
- **G13 (jinxedthought)**: "Smite for Evangelism, use up our Archangel, start shielding and healing, and then go back to smiting."

**Consensus pattern (5/5 that addressed rotation)**: Holy Fire on cooldown (10s) → Smite on Holy-Fire-debuffed target as filler. Glyph of Smite gives +20% damage on Holy-Fire-debuffed targets, so this priority is mechanic-driven, not preference.

**Spec disposition**: **Atonement rotation = `Holy Fire if cd == 0, else Smite`.** Tier 3 (Atonement) ordering is:
1. If `holy_fire.cd == 0`: cast Holy Fire (DoT applies, Glyph of Smite buff on next Smite).
2. Else: cast Smite.
3. Penance is a Tier 2 action (always-higher priority); but since Penance's CD is 8s baseline (with Glyph of Penance: 6s), it interleaves naturally with Smite filler.

### 8. Strength of Soul vs Train of Thought (Disc tier 4 talent choice)

- **G3 (Icy Veins)**: "Full healing build uses Strength of Soul; Atonement build uses Train of Thought."
- **G13 (jinxedthought)**: "Inner Focus combined with Train of Thought ... one of your best talents for mana efficiency and healing throughput."
- **Search excerpts (multiple)**: "Train of Thought is a better choice if the default 3-second cooldown on Power Word: Shield feels sufficient. Strength of Soul reduces damage taken by the target, while Train of Thought improves mana conservation."

**Likely explanation**: Strength of Soul (each Heal/Greater Heal/Flash Heal reduces Weakened Soul by 4s, allowing more frequent PW:Shields on the same target) is a tank-heal-spec talent. Train of Thought (Greater Heal cast reduces Inner Focus CD by 5s; Smite cast reduces Penance CD by 0.5s) is the throughput / mana-efficiency talent.

**Spec disposition**: **Train of Thought 3/3** for Atonement build (matches the bot's default). Strength of Soul is encoded in the Shield-spec preset for tank-heal-assignment encounters. Cross-references the spec disposition in disagreement #1.

### 9. Power Word: Barrier mastery interaction — does Mastery still buff PWB?

- **G5 (Warcraft Wiki PWB page)**: "In Patch 4.0.3a (2010-11-23), the spell underwent major redesign. It transitioned from an absorb shield to providing damage reduction instead, with the effect lasting 10 seconds. This change meant it no longer benefits from Mastery: Shield Discipline."
- **G3 / G4**: don't address.
- **G10 / G11**: imply PWB mechanics generically.

**Likely explanation**: Pre-4.0.3a, PWB was an absorb shield (Mastery scaled it). Post-4.0.3a, PWB is flat 25% damage reduction. The bot targets 4.3.4 (well after 4.0.3a), so PWB is damage-reduction.

**Spec disposition**: **PWB is 25% damage reduction in 4.3.4, not an absorb shield. Mastery does NOT scale PWB.** Bot doesn't model damage-reduction shields as absorb pools; PWB fires on the raid-emergency predicate and the engine handles the actual damage-mitigation calculation.

### 10. Mana threshold for Hymn of Hope vs Shadowfiend ordering

- **G1 / G10 / G11**: imply "use both when low on mana"; no explicit ordering.
- **G5**: confirms HoH is 6-min CD and Shadowfiend is 5-min CD (so Shadowfiend can be used more often).

**Likely explanation**: Players intuitively fire Shadowfiend first (shorter CD) and save HoH for severe mana crises.

**Spec disposition**: **Author judgment.** Shadowfiend trigger: `mana_pct < 50 AND in_combat AND Shadowfiend.cd == 0`. Hymn of Hope trigger: `mana_pct < 30 AND no_emergency AND HoH.cd == 0`. HoH's 8s channel is uninterruptible without dropping the buff, so it gates harder on the no-emergency clause.

---

## Items NOT addressed by ANY guide (pure author judgment)

| Spec item | Value chosen | Why no guide consensus exists |
|---|---|---|
| Self-emergency Desperate Prayer trigger | self.hp_pct < 35 | Bot self-preservation is bot-specific; players know when they're dying. |
| Tank-emergency Pain Suppression trigger | tank.hp_pct < 40 AND tank_taking_predictable_burst | Guides classify Pain Sup as "tank emergency CD" without quantifying. |
| Power Word: Barrier raid-emergency trigger | raid.injured_count >= 4 AND raid.avg_hp_pct < 60 | Guides describe PWB as "raid AoE shield" without quantifying. |
| Divine Hymn raid-emergency trigger | raid.injured_count >= 5 AND raid.avg_hp_pct < 50 | Lower-priority than PWB by 30s grace window. |
| Atonement-Smite cluster-injured threshold | any_friendly_in_15y_of_enemy with hp_pct < 95 | Atonement is high-throughput; bot fires it whenever the cluster has any injured ally. |
| Atonement abandon — tank priority threshold | tank.hp_pct < 75 → abandon Smite, switch to direct heal | Resolves the "Atonement may not target the tank" failure mode. Author judgment. |
| Mana conserve mode threshold | self.mana_pct < 50 | Mirrors resto-shaman / holy-paladin cross-class default. |
| Mana crisis mode threshold | self.mana_pct < 25 | Mirrors cross-class default. |
| Out-of-combat drink threshold | 30% (force) / 95% (resume) | Mirrors cross-class default. |
| Power Infusion self-target trigger | self_high_throughput_window AND PI.cd == 0 | `mana_pct > 50 AND any_injured_ally AND tank.hp_pct < 80` as the proxy for "I'm about to spam heals." |
| Hymn of Hope mana-CD trigger | self.mana_pct < 30 | Matches cross-class convention. |
| Shadowfiend mana-CD trigger | self.mana_pct < 50 AND in_combat | Chosen so Shadowfiend fires before Hymn of Hope. |
| Inner Focus auto-pair trigger | IF.cd == 0 AND about_to_cast(GreaterHeal OR PrayerOfHealing) | Auto-pair is the right behavior with Train of Thought. |
| Borrowed Time follow-up cast | next_cast_after_PW_Shield = (Penance if Penance.cd == 0 else GreaterHeal) | Penance benefits more from haste (channeled) than Greater Heal does. |
| Inner Fire vs Inner Will default | Inner Fire (default), Inner Will when in `high_movement` encounter mode | Inner Fire is the safer default. |
| Glyph of Power Word: Barrier opt-in | OPTIONAL prime glyph swap | PWB-glyph swap is a manual config flip. |
| Atonement-spec vs Shield-spec selection | manual command override (spec preset) | Spec-tree-level choice, not runtime. |
| Pre-pull PW:Shield application timing | 4 seconds before pull | 4s is enough for cast + GCD to settle and Borrowed Time to chain. |

---

## Author-judgment items — leveling-specific (NOT guide-derivable)

```yaml
# Spell unlock breakpoints (sourced from Wowhead Cata Classic / Warcraft Wiki, NOT from strategy guides)
spell_unlocks:
  POWER_WORD_FORTITUDE: {level: 1,  spell_id: 1243}
  SMITE:                {level: 1,  spell_id: 585}
  POWER_WORD_SHIELD:    {level: 4,  spell_id: 17}
  INNER_FIRE:           {level: 7,  spell_id: 588}
  FLASH_HEAL:           {level: 3,  spell_id: 2061}
  HOLY_FIRE:            {level: 18, spell_id: 14914}
  PENANCE:              {level: 10, spell_id: 47540}    # Disc spec ability (post-4.0.1)
  PRAYER_OF_HEALING:    {level: 44, spell_id: 596}
  PRAYER_OF_MENDING:    {level: 14, spell_id: 33076}    # verify level
  GREATER_HEAL:         {level: 40, spell_id: 2060}     # verify level
  HYMN_OF_HOPE:         {level: 64, spell_id: 64901}
  DIVINE_HYMN:          {level: 78, spell_id: 64843}
  SHADOWFIEND:          {level: 40, spell_id: 34433}
  DESPERATE_PRAYER:     {level: 22, spell_id: 19236}
  # Talent-gated:
  ATONEMENT:            {requires_talent: true}
  EVANGELISM:           {requires_talent: true}
  ARCHANGEL:            {requires_talent: true}
  BORROWED_TIME:        {requires_talent: true}
  RAPTURE:              {requires_talent: true}
  PAIN_SUPPRESSION:     {requires_talent: true}
  POWER_WORD_BARRIER:   {requires_talent: true}
  INNER_FOCUS:          {requires_talent: true}
  POWER_INFUSION:       {requires_talent: true}
  DIVINE_AEGIS:         {requires_talent: true}
  GRACE:                {requires_talent: true}
  TRAIN_OF_THOUGHT:     {requires_talent: true}
  STRENGTH_OF_SOUL:     {requires_talent: true}
```

| Default | Value | Why |
|---|---|---|
| Self-emergency HP threshold pre-60 | 25% (vs 35% at max level) | Cross-class convention. |
| Mana floor pre-60 | 0 (no conserve mode) | Cross-class convention. |
| Mana floor 60-70 (BC) | 55 conserve / 30 crisis | Cross-class convention. |
| Mana floor 70-80 (WotLK) | 45 conserve / 25 crisis | Cross-class convention. |
| Mana floor 80-85 (Cata) | 50 conserve / 25 crisis | Cross-class convention. |
| Pre-Penance filler (level 1-9) | Smite + Power Word: Shield + Flash Heal | Penance unlocks at level 10. |
| Pre-Atonement filler | Smite-as-damage only (no Atonement healing) | Atonement is talent-gated post-30. |
| Pre-PW:Barrier (level 1-?) | n/a — no AoE-emergency CD | PWB is the 31-point Disc talent. |
| Pre-Glyph levels (1-25) | Skip glyphs entirely | Glyph slots unlock progressively. |
| Solo questing (Disc spec) | Smite spam + Power Word: Shield self-shield | Atonement provides self-heal during damage rotation. |
| Beacon-equivalent — primary heal target during dungeon leveling | Tank | Disc keeps PW:Shield up on the tank. |
| Atonement-spec vs Shield-spec at lower levels | Shield-spec until Atonement talent is allocated | Atonement isn't available until ~level 50. |

---

## Suggested next research passes

1. **Atonement target-priority telemetry** — log which ally Atonement actually hit per Smite cast.
2. **Atonement healing percentage in 4.3.4** — DBC verification of spell IDs 81749 / 81751.
3. **Penance haste breakpoints** — verify hard-coded 3 ticks via DBC.
4. **Mana CD ordering — Shadowfiend vs Hymn of Hope** — measure mana-per-fight returned.
5. **Atonement-spec vs Shield-spec throughput comparison** — A/B test in 5-mans / 10-mans.
6. **Power Infusion target choice** — currently self-target; consider buffing DPS in pure-damage windows.
7. **Glyph of Power Word: Barrier opt-in** — encounter-by-encounter A/B test.
8. **Borrowed Time follow-up cast tuning** — measure haste-utilization-rate per buff.
9. **Strength of Soul value at lower gear levels** — survey says skip for Atonement-build; verify at low gear.
10. **Inner Focus / Train of Thought interaction telemetry** — log proc consumption.
11. **Mastery shift for Atonement-spec** — reforging engine should know which preset is active.
12. **Holy Fire DoT uptime tracking** — track DoT remaining and refresh aggressively.

---

## Source-coverage gaps for DBC verification

- **Atonement spell IDs and rank values (4.3.4)** — `Spell.dbc` 81749 / 81751.
- **Atonement radius confirmation** — verify 15-yard radius in live 4.3.4 DBC.
- **Penance tick count** — confirm spell ID 47540 has exactly 3 channel ticks.
- **Glyph of Penance cooldown reduction** — confirm baseline + glyph values.
- **Borrowed Time haste %** — confirm 7%/14% per rank.
- **Rapture mana return values** — confirm 4.3.4 DBC has 7% (max rank).
- **Power Word: Shield mana cost in 4.3.4** — verify exact percentage of base.
- **Pain Suppression mana cost** — confirm Cata Classic value (likely ~8% base mana).
- **Hymn of Hope mana return per tick** — verify 4.3.4 value.
- **Divine Hymn target count and CD** — confirm 5-target / 8-min CD.
- **Body and Soul movement-speed value at 4.3.4** — likely 60% / 4s.
- **Power Infusion mana cost** — verify ~16% base mana.
- **Inner Focus 4.3.4 effect** — confirm spell list (Penance + IF interaction unclear).
- **Train of Thought interaction with Smite** — verify "every Smite reduces Penance CD by 0.5s."
- **Glyph of Smite — exact Holy Fire DoT requirement** — current DoT or just hit?

These are tagged for `docs/research/dbc-verification-checklist.md` style follow-up, mirroring the resto-shaman survey pattern.

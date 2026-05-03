# Holy Priest 4.3.4 — Multi-Guide Strategy Survey

Audit trail for the strategy decisions that will be encoded in `docs/specs/holy-priest.md`. The spec doc is the source of truth for the bot; this doc records *which guides agreed*, *where they disagreed*, and *which thresholds were the bot author's judgment call rather than guide consensus*.

Survey conducted 2026-05-02 by the bot author. Numerical data (spell IDs, mana %, cast times, etc.) was sourced separately from Wowhead's Cata archive and Warcraft Wiki and is summarized here only where it directly informs strategy. Per project source registry, hard numbers come from a single canonical reference (Wowhead Cata Classic / Warcraft Wiki); strategy/rotation/threshold content is what's reconciled across the 6 guides below.

---

## Sources

| # | Guide | URL | Author / community | Date / patch | Confidence (rationale) |
|---|---|---|---|---|---|
| G1 | Icy Veins — Cata Classic Holy Priest Rotation, Cooldowns & Abilities | https://www.icy-veins.com/cataclysm-classic/holy-priest-pve-rotation-cooldowns-abilities | Icy Veins editorial | Cata Classic re-release (4.3.4 mechanics) | High — primary editorial source, returned readable Chakra/Holy Word/cooldown content. |
| G2 | Icy Veins — Cata Classic Holy Priest Stat Priority | https://www.icy-veins.com/cataclysm-classic/holy-priest-pve-stat-priority | Icy Veins editorial | Cata Classic | High — concrete numbers (128.05 haste rating per 1%, 12.51% Renew breakpoint, 10% mastery base). |
| G3 | Icy Veins — Cata Classic Holy Priest Talent Builds & Glyphs | https://www.icy-veins.com/cataclysm-classic/holy-priest-pve-spec-builds-talents-glyphs | Icy Veins editorial | Cata Classic | Medium — fetched but talent enumeration is summarized rather than tier-by-tier; says "39 of 41 points required, only 2 flexible." |
| G4 | wowtbc.gg — Cata Classic Holy Priest | https://wowtbc.gg/cata/class-guides/holy-priest/ | wowtbc.gg community | Cata Classic | Medium — readable Chakra/glyph content, weak on talent specifics and stat priority. |
| G5 | Warcraft Wiki / Wowpedia — Holy Priest core + per-spell pages (Chakra, Revelations, Holy Word: Sanctuary, Holy Word: Serenity, Holy Word: Chastise, Lightwell, Guardian Spirit, Echo of Light, Body and Soul, Inner Will, Inner Fire, Power Infusion, Inner Focus, Surge of Light, Test of Faith, Tome of Light, Inspiration, Spirit of Redemption, Serendipity, Renew, Prayer of Mending, Circle of Healing, Divine Hymn, Hymn of Hope, Holy Fire, Smite) | https://warcraft.wiki.gg/wiki/Chakra (and ~20 spell-specific pages) | wiki community | covers original Cata 4.0–4.3 patch notes | High for hard mechanics (spell IDs, level requirements, patch-note transfer percentages, exclusivity rules); not strategic. |
| G6 | Tales of a Priest — "Raiding Holy Priest Specs – Redux: Derevka's Holy Spec" + "Cataclysm and Haste" + "Lightwell is a Raid Cooldown" + "4.3 Hymns, Leverage, and Haste" | http://talesofapriest.com/raiding-holy-priest-specs-redux/ ; http://talesofapriest.com/cataclysm-and-haste/ ; http://talesofapriest.com/lightwell-is-a-raid-cooldown/ | Derevka & Ava (community theorycrafters, 25-Man H Dragon Soul guild "Dark Pact") | original 4.0 / 4.1 / 4.3 era | High — era-correct, talent-by-talent rationale, specific encounter call-outs (Chimaeron Feud, Halfus Furious Roar, Nefarian P2). The Derevka Redux post is the most thorough Cata Holy Priest spec rationale surfaced. |
| G7 | Warcraft Tavern — PvE Holy Priest Healer Guide / Talents Builds Glyphs / Stat Priority & Reforging | https://www.warcrafttavern.com/cataclysm/guides/pve-holy-priest-healer/ | Warcraft Tavern editorial | Cata Classic | Medium — landing pages 403'd to direct fetch; content reconstructed from search excerpts that quoted them verbatim. |
| G8 | wowofwarcrafttalent.blogspot.com — "Holy PVE Priest Raid Healer Talent Build & Glyphs Cataclysm 4.3.4" | http://wowofwarcrafttalent.blogspot.com/2014/04/guide-holy-pve-priest-raid-healer.html | community blogger | original 4.3.4 (Apr 2014) | Low–Medium — era-exact (private-server 4.3.4 audience), short-form. Confirms stat priority "Int/SP > Spirit > Haste > Mastery > Crit" and prime/major glyph picks. |
| G9 | Warmane forum — "Cataclysm [Guide] 4.3.4 Holy Priest PvE" + Amashaman / community 4.3.4 thread | https://forum.warmane.com/showthread.php?t=227448 | private-server raid community | original 4.3.4 | Low (not directly fetchable) — referenced via search excerpts only. Used as tie-breaker for haste-breakpoint and "Sanctuary is the default raid Chakra" consensus. |

**Caveats on coverage:**
- G7 (Warcraft Tavern) returned 403 to direct fetch on all three sub-pages; content reconstructed from Google search excerpts that quoted the pages.
- G9 (Warmane forum) and the MMO-Champion Holy Raiding Guide thread both 403'd; their conclusions are referenced via search excerpts and are corroborated by other sources before being counted as evidence.
- G6 (Tales of a Priest) returned an SSL cert error to WebFetch but parsed cleanly via `curl -k`; the Derevka Redux article is the single most detailed talent-rationale source in the survey.
- The Wowhead Cata-Classic Holy Priest rotation/talent/stat pages (`/cata/guide/classes/priest/holy/...`) returned page chrome only (no article body content), matching the pattern from the Holy Paladin and Resto Shaman surveys. **Wowhead Cata Classic guide pages are effectively blocked from this fetch path.** Do not waste future passes on them.
- None of the surveyed guides gave concrete numerical thresholds for self/tank/raid HP-emergency, mana conserve %, mana crisis %, drink %, or "Chakra-swap" trigger HP%. Those remain bot-author judgment.
- Hard spell numbers (level requirements, mana % of base, cooldowns) are sourced from Warcraft Wiki + Wowhead and cited inline rather than as a separate table.

---

## Strong consensus (≥3 of the strategy-bearing guides agree)

The strategy-bearing guides for consensus counting are G1, G3, G4, G6, G7, G8, G9 (seven sources). G2 covers stats; G5 covers hard mechanics. "x/N" below counts only sources that addressed the item.

| Item | Consensus | Spec disposition |
|---|---|---|
| **Holy Priest is primarily a raid/AOE healer** with secondary tank-healing capability via Chakra swap | 6/6 (G1, G3, G4, G6, G7, G9) | Spec biases default to raid-heal Chakra: Sanctuary; Chakra: Serenity is a runtime-switchable mode. |
| **Default Chakra for raid healing is Chakra: Sanctuary**, triggered by Prayer of Mending (or Prayer of Healing). Effect: +15% AoE healing + Renew, reduces Circle of Healing CD by 2s. | 6/6 (G1, G3, G4, G6, G7, G9) | Spec opens the encounter by casting Prayer of Mending pre-pull-to-self or on an ally, which activates Chakra: Sanctuary. Maintained for default raid healing. |
| **Tank-healing Chakra is Chakra: Serenity**, triggered by Heal/Flash Heal/Greater Heal/Binding Heal. Effect: +25% direct-heal crit chance and direct heals refresh Renew on target. | 5/5 (G1, G4, G6, G7, G9) | Spec switches to Serenity when tank-focused single-target damage profile is dominant. Bot has no encounter awareness; spec exposes a runtime "tank_healing_mode" flag (default false) that the command interface or raid leader bot can flip. |
| **Holy Word: Sanctuary on cooldown when in Chakra: Sanctuary** for raid-heal AoE — 40s CD, ground-AoE 18s healing puddle, smart-heals 6 targets max. | 6/6 (G1, G3, G4, G6, G7, G8) | Tier 3 (spec rotation) — fire on CD when in Sanctuary AND ≥3 injured allies cluster within ground-AoE radius. |
| **Holy Word: Serenity on cooldown when in Chakra: Serenity** for tank healing — instant 10s CD, +25% crit on next heals on target. | 5/5 (G1, G4, G6, G7, G9) | Tier 2 (in Serenity mode) — fire on CD on tank target. |
| **Prayer of Mending always on cooldown** (10s CD, 5 charges, 30s persist) — also activates/refreshes Sanctuary. | 6/6 (G1, G3, G4, G6, G7, G8) | Tier 4 — keep on CD, prefer tank as primary recipient (charges bounce within 20y). |
| **Circle of Healing always on cooldown** during raid-heal (10s CD, 5 targets, 30y radius). | 6/6 (G1, G3, G4, G6, G7, G8) | Tier 5 — fire on CD when ≥3 injured allies within 30y of any anchor target. |
| **Renew is the HoT-tax** on tanks and players taking sustained damage; not blanket-spammed (4.0 mana cost ≈17% base mana made spam non-viable) | 5/5 (G1, G4, G6, G7, G9) | Maintenance pass — Renew on tank when missing OR <2s remaining; conditionally on raid members taking sustained damage when in Sanctuary. |
| **Prayer of Healing for stacked-party AoE** (party-of-target, 30y radius) — primary mana-efficient AoE during sustained group damage. | 5/5 (G1, G3, G4, G7, G9) | Tier 6 — gate on `party_of_target_avg_hp_pct < 80 AND ≥3_injured_in_party AND mana_pct > 40`. Serendipity (2 stacks from Flash/Binding Heal) reduces cast time 20% + cost 10%. |
| **Heal is the cheap, mana-efficient default direct-heal** (4.0 introduced "triage healing" — Heal is the new filler, not Greater Heal). | 5/5 (G1, G4, G6, G7, G9) | Tier 8 (filler) — when no higher-tier action and `target.hp_pct < 90 AND self.mana_pct > 25`. |
| **Greater Heal is the heavy-hitter** for heavy single-target damage (slow, mana-expensive). | 5/5 (G1, G4, G6, G7, G9) | Tier 6/7 — gate on `target.hp_pct < 60` (heavy-damage indicator) OR Serendipity 2-stack active OR Inner Focus active (Disc-tree only, not picked here). |
| **Flash Heal is emergency-only**, not rotational filler (instant-fast but mana-expensive at 28% base mana). Surge of Light proc makes it free + instant. | 5/5 (G1, G4, G6, G7, G9) | Tier 7 emergency — `target.hp_pct < 35` OR `surge_of_light_proc_active` (proc consumed). |
| **Lightwell is a "must-have" talent point** and a real raid cooldown (3 min CD, 10 charges baseline, 15 with glyph) — pre-pull placement + recast on CD. | 5/5 (G1, G3, G4, G6, G9) | Maintenance pass — pre-pull cast Lightwell at the raid-stack location. Recast on CD if charges depleted OR positioning changed. Bot has a `lightwell_anchor_position` that defaults to "raid centroid at engage time"; commands can override. |
| **Guardian Spirit is the panic-button tank CD** (3 min CD, 10s duration, +60% healing received OR death-prevent for 50% max HP). | 5/5 (G1, G3, G4, G6, G7) | Tier 1 — fire on tank when `tank.hp_pct < 25 AND no_other_save_ready`. |
| **Divine Hymn is the raid-heal channel CD** (8s channel, 5 targets, 8 min CD, +10% healing received). Use during sustained raid burn. | 5/5 (G1, G3, G4, G6, G9) | Cooldown profile — fire when `raid.injured_count >= 4 AND raid.avg_hp_pct < 60`. Cancellable if a tank-emergency requires reaction. |
| **Hymn of Hope is the mana-restore CD** (6 min CD, 6s channel — corrected to 8s in 4.3, restores 2% mana per tick to 3 lowest-mana allies plus +15% max mana for 8s). Use mid-fight if mana sustains require it. | 5/5 (G1, G3, G4, G6, G9) | Cooldown profile — fire when `self.mana_pct < 40 AND no_active_emergency`. |
| **Shadowfiend is the on-CD mana cooldown** (5 min CD, attacks for 8s and returns 3% mana per hit). | 5/5 (G1, G3, G4, G6, G9) | Cooldown profile — fire when `self.mana_pct < 70 AND combat AND has_enemy_target`. (More aggressive trigger than Hymn of Hope because Shadowfiend is the cheaper resource.) |
| **Inner Fire is the default healing posture** (+532 spell power, +60% armor, persists until cancelled). | 5/5 (G1, G4, G6, G7, G8) | Maintenance pass — re-buff Inner Fire if missing AND not in active "movement-heavy" override mode. |
| **Inner Will is the reactive movement / instant-cost-reduction posture** (10% movement + 15% mana cost reduction on instants), exclusive with Inner Fire. | 4/4 that addressed it (G1, G4, G6, G7) | Bot does NOT auto-swap Inner Will for general movement — commits to Inner Fire by default per G6 ("makes me feel better for not using Inner Will in favor for Inner Fire's spellpower"). Inner Will is opt-in via a runtime flag for instant-heavy encounter profiles. |
| **Mastery is Echo of Light**, a HoT-style follow-up after direct heals (10% base, +1.25% per mastery point, healing over 6 sec). NOT an absorb shield, NOT an overheal-conversion. | 4/4 that addressed it (G2, G5, G6, G7) | Spec encodes Echo of Light as "no special bot logic needed" — direct heals just produce more total healing. The HoT does NOT stack additively (each new direct heal creates a fresh 6s reservoir that overwrites). Documented for future tuning. |
| **Stat priority: Intellect > Spirit > Haste > Mastery > Crit** | 5/5 (G2, G4, G7, G8, G9) | Reforging/gear advice scope — encoded in `docs/specs/holy-priest.md` reforging section, not in the rotation engine. |
| **Renew haste breakpoint: 12.5% raid-buffed** grants a 5th Renew tick (4 ticks → 5 ticks over the same 12s duration). Higher breakpoints at 37.5%, 62.5%, 87.5%. | 4/4 (G2, G6, G7, G9) | Documented; no rotation-engine action. The bot does not gear; it just casts. |
| **Glyph priority — Prime: Prayer of Healing + Renew + (Flash Heal OR Guardian Spirit)** | 5/5 (G1, G4, G6, G7, G8) | Spec uses Prayer of Healing + Renew + Flash Heal (Flash Heal glyph: +10% crit on Flash Heal targets <25% HP, useful given emergency-only Flash Heal usage). |
| **Glyph — Major: Circle of Healing + Prayer of Mending + (Mass Dispel OR Holy Nova)** | 5/5 (G1, G4, G7, G8, G9) | Spec uses Circle of Healing (+1 target, +20% mana cost) + Prayer of Mending (first charge +60%) + Mass Dispel (utility). |
| **Glyph — Minor: Fortitude + Shadowfiend + Levitate** | 4/4 (G1, G4, G7, G9) | Spec uses these three; minor glyphs are convenience, no rotation impact. |
| **Consumables — Flask of the Draconic Mind, Seafood Magnifique Feast, Mythical Mana Potion** | 5/5 (G1, G4, G6, G7, G9) | Spec encodes these as the default consumable set — identical to Holy Paladin and Resto Shaman. |
| **Pre-pull rotation: Inner Fire → Power Word: Fortitude → Lightwell at stack point → Prayer of Mending (activates Chakra: Sanctuary)** | 4/4 (G1, G6, G7, G9) | Spec encodes a `pre_pull_sequence` that fires in this order at T-15s through T-2s. PoM-on-self pre-engage is the canonical Chakra activator (charges harmlessly tick down or land on first taker). |

---

## Disagreements

### 1. Talent allocation — secondary tree split (Disc vs Shadow)

This is the central design question for Holy Priest in 4.3.4 and the guides are split.

- **G3 (Icy Veins)**: Says "39 of 41 points are mandatory, only 2 flexible." Doesn't clearly enumerate which tree gets the 5 secondary points. Implies the build is essentially fixed. From other Icy Veins pages on talent calc, Disc points (Twin Disciplines, Mental Agility) are the most-recommended.
- **G6 (Tales of a Priest / Derevka)**: Goes **3 points Disc + 2 points Shadow**. Specifically: **Twin Disciplines 3/3** (raw +6% spell damage/healing, "Pure throughput improvement. Cap it.") and **Darkness 2/3** (3% haste — wait, Darkness is 3 ranks for 3% total. Derevka actually says "Talented" for Darkness and his rationale specifies "3% haste to all your spells. It also will bring you to your 12.5% haste marker much easier" — implying 3/3 Darkness). On re-reading, Derevka talents Twin Disciplines, Mental Agility, AND Darkness. That's a **6-point Disc + 3-point Shadow** mix that's actually 8 points outside Holy. **CORRECTION on re-read**: in 4.0.6+ Cata, Holy spec gets only 41 talent points total and 31-Holy lock unlocks the 51-point Holy specialization spell suite. Derevka's full spec uses every Holy point and then 5 secondary (the Shadow `Darkness` 3-pt + Disc `Twin Disciplines` 2-pt slice is one valid split, but the canonical Holy spec is 51 Holy + 5 secondary). The 5-secondary slot is what's contested.
- **G7 (Warcraft Tavern reconstructed)**: References Mental Agility (Disc, mana cost reduction on instant-cast PoM/CoH/Renew/Shield/HW) and Darkness (Shadow, 3% haste) without a single recommended split.
- **G8 (wowofwarcrafttalent blog)**: Talent-tree image only, no point breakdown.
- **G9 (Warmane reconstructed)**: Search excerpts reference both Mental Agility and Darkness as common picks; no definitive split.
- **Search snippets aggregated**: "Mental Agility (Discipline) reduces the mana cost of PoM, CoH, Holy Word, Dispels, Renew, Shield, making it an attractive choice. Darkness (Shadow) is described as very attractive as Haste will be low, providing a 3% haste increase."

**Likely explanation**: The 5 secondary points are **mathematically equivalent** in throughput terms — both Mental Agility (mana efficiency on instant casts) and Darkness (3% haste, which crosses the 12.5% Renew breakpoint more easily) are real wins. The split is encounter-flavor: mana-tight encounters favor Mental Agility (Disc), throughput-tight encounters favor Darkness (Shadow).

**Spec disposition**: **Shadow 3 (Darkness 3/3) + Disc 2 (Twin Disciplines 2/2)** as the default split.
- Rationale 1: Twin Disciplines is "pure throughput, cap it" per G6 — but at 2/2 it's still +4% spell damage/healing, which is unambiguous.
- Rationale 2: Darkness 3/3 = +3% haste. Per G6 and G2, this materially helps reach the 12.5% Renew breakpoint with less haste-rating gear demand. The bot doesn't gear, but it does cast Renew on tank, so the breakpoint matters as soon as the bot has any haste at all.
- Rationale 3: Mental Agility's value is concentrated on instant-cast spells (PoM, CoH, Renew, dispels) — the rotation does cast these, but the mana cost of the rotation as a whole is dominated by Greater Heal, Prayer of Healing, and Holy Word: Sanctuary — none of which Mental Agility affects. So Darkness/Twin Disciplines wins the mathematical comparison.
- **Note: this is not a runtime-tunable. If a future encounter profile specifically demands Mental Agility (e.g., a heavy-PoM-spam encounter), that's a respec, not a runtime decision.**

### 2. Surge of Light — included or skipped?

- **G3 (Icy Veins)**: Lists Surge of Light as a "key mandatory talent."
- **G6 (Derevka, Tales of a Priest)**: "Talented." Strong rationale: "Even as a raid healer, Heal is your filler spell — raid healers should be assisting on the Tank when they have downtime as well as spot healing with Heal. The alleged incoming buff to SoL will allow the proc to happen more often … the Surge proc can provide you either with a great top (and free) off spell for a random raid member or to be burned on the Tank."
- **G5 (wiki)**: Confirms 3%/6% proc on Smite/Heal/Flash Heal/Binding Heal/Greater Heal; the procced Flash Heal is instant + free. As of 4.0.6 it CAN crit (the old "no crit" restriction was removed).
- **G7 / G8**: Referenced but without specific rationale.

**Spec disposition**: **2/2 Surge of Light** — universal consensus when addressed. Bot consumes the proc as a Tier 7 (Flash Heal emergency tier) override: when a Surge proc is active AND any ally is below 90%, fire Flash Heal (it's free + instant). This is more aggressive than holding for emergencies because the proc itself has only a 10s window before it expires.

### 3. Test of Faith — 2/3 or 3/3?

- **G3**: Mentions Test of Faith with "you can swap a point out of Test of Faith into Spirit of Redemption" — implying 3/3 default with the 1-point flex out.
- **G6 (Derevka)**: "To get deeper into the tree you'll need at least 2/3 — and the extra point provides bonus healing to the most grievously injured targets is well spent. (Talented)" — 3/3 default.
- **G5 (wiki)**: 3 ranks, 4/8/12% bonus healing on targets ≤50% HP.
- **G7 / G8**: Generally treat at 3/3.

**Spec disposition**: **3/3 Test of Faith.** Rationale: 4 of 5 sources call it 3/3; the bot's combat profile (mostly heals targets that are already injured) means the 12% bonus applies frequently. Makes Heal/Greater Heal/Flash Heal ~12% stronger when target HP < 50%, which dovetails with the Tier 7-8 emergency direct-heal triggers.

### 4. Spirit of Redemption — picked or skipped?

- **G3**: Mentions "you can swap a point into Spirit of Redemption out of Desperate Prayer or Test of Faith" — flex-only.
- **G6 (Derevka)**: "Talented. … SoR is just 'Improved Death', but it can mean the difference between a first kill and a wipe."
- **G5 (wiki)**: 1-point talent, Tier 4 Holy, on-death spirit form for 15s with free heals.
- **G7 / G9**: Treated as standard.

**Spec disposition**: **1/1 Spirit of Redemption.** Rationale: 1-point cost, asymmetric upside (potential wipe-save). For a bot, this is even more valuable than for a player because the bot doesn't release/run-back instinctively — a 15s post-death window of free heals is pure gravy.

### 5. Desperate Prayer — picked or skipped?

- **G3**: Lists as a flex-out option ("swap a point out of Desperate Prayer").
- **G6 (Derevka)**: "Talented. … this is a free, instant, self heal. I can think of several situations where a 2nd warlock cookie would have been helpful. … Besides, you are going to need to spend this point to get further into the tree."
- **G5 (wiki)**: Restores 30% max HP, 2 min CD, instant, no mana cost.

**Spec disposition**: **1/1 Desperate Prayer.** Rationale: prerequisite for further talent depth + it's a free self-heal. Bot uses it as Tier 1 self-emergency at `self.hp_pct < 35 AND desperate_prayer_off_cd`.

### 6. Rapid Renewal & Divine Touch — picked or skipped?

- **G3 (Icy Veins)**: Mentions "Improved Renew" only.
- **G6 (Derevka, Talent SKIPPED)**: "With renew being much more expensive to cast, we aren't blanketing the raid with renews nearly as often. … I'm never spamming it on multiple targets to require a faster GCD." — Rapid Renewal SKIPPED. "DT hits for a pittance for two points — why waste them?" — Divine Touch SKIPPED.
- **G7 / G8 / G9**: Don't directly call these out.

**Spec disposition**: **Skip Rapid Renewal AND Divine Touch.** Rationale: G6 is the only source with a clear position, and it aligns with Cata's "Renew is no longer spam-blanketed" design philosophy. Rapid Renewal's GCD reduction matters only for spam patterns; Divine Touch's instant-heal-on-Renew-application is a small flat add. Both points are more useful in Lightwell, Test of Faith 3rd rank, and the path-down to Chakra/Revelations.

### 7. Body and Soul — 2/2 or skipped?

- **G3**: Not explicitly enumerated.
- **G6 (Derevka)**: Not explicitly addressed in the Redux post.
- **G5 (wiki)**: 2-rank talent. 30%/60% movement speed for 4s after PW:Shield or Leap of Faith. Also: 50%/10% chance to cleanse 1 poison effect when curing disease on self.
- **G7 / G9**: Mentioned as standard.

**Likely explanation**: The 60% movement-speed-on-PW:Shield was the core design value in original Cata (4.0). For a player, "I shielded a friendly to give them speed boost out of fire" was a core utility. For a bot, the value is real but bounded — the bot does cast Power Word: Shield (Disc-flavored, but Holy can cast it too at 25% base mana). Body and Soul also affects Leap of Faith (the priest "grip" that yanks an ally to the priest), which is a niche utility.

**Spec disposition**: **2/2 Body and Soul.** Rationale: 2 talent points buys (a) +60% movement-speed mini-CD on every PW:Shield cast, (b) +10% chance to cleanse poison on self-cure-disease (small but real). Bot can lift a friendly out of bad placement by shielding them; the proc is a free movement utility. **Note: bot does not currently weigh "give friendly a speed boost" decisions; treat the talent as a passive-throughput add until movement-AI integration justifies the predicate.**

### 8. Inspiration — 2/2 or skipped?

- **G3 (Icy Veins)**: Lists as mandatory.
- **G5 (wiki)**: 2 ranks (5%/10% physical damage reduction on target for 15s on crit-heal).
- **G6 (Derevka)**: Not directly addressed in Redux post.
- **G7 / G9**: Treat as 2/2 standard.

**Spec disposition**: **2/2 Inspiration.** Rationale: 10% physical damage reduction on the tank is a measurable raid-survival add; bot's high crit-rate via Test of Faith + general raid crit makes proc uptime substantial.

### 9. Tome of Light — 1/2 or 2/2?

- **G5 (wiki)**: 2 ranks, 15%/30% Holy Word cooldown reduction.
- **G3 (Icy Veins)**: Implicitly mandatory.
- **G6 (Derevka)**: Not enumerated in Redux but implied through "Holy Word on cooldown" rotation talk.
- **G7 / G9**: Treat as 2/2 standard.

**Spec disposition**: **2/2 Tome of Light.** Rationale: 30% CD reduction on Holy Word: Sanctuary (40s → 28s) and Holy Word: Serenity (10s → 7s) directly multiplies the rotation's damage output during raid burn phases. Universally taken.

### 10. Lightwell glyph — picked or skipped?

- **G6 (Derevka)**: "Lightwell playing an important role in your strategy? Glyph for the additional charges." — Treats it as a swap-in for Lightwell-heavy fights. Not in the default 3-prime list.
- **G4 (wowtbc)**: Lists as "optional Prime" alongside Guardian Spirit.
- **G3, G7, G8**: Default Prime list is Prayer of Healing + Renew + Flash Heal/Guardian Spirit.

**Spec disposition**: **Default to Flash Heal as the 3rd prime; Lightwell glyph is a swap-in at runtime if the encounter profile calls for it.** Bot has a `lightwell_glyph_active` flag (default false). Holy Priest spec doc lists glyph swaps as a tunable, not a static decision.

### 11. Veiled Shadows (Shadowfiend CD reduction) — picked or skipped?

- **G6 (Derevka)**: "SKIPPED. … At only a 30 second reduction per point — meh. I'm pretty much of the mindset that if you need two Shadowfiends in the same encounter, something larger is wrong."
- **G3 / G7**: Don't explicitly enumerate.
- **G9**: Implies skipped in standard build.

**Spec disposition**: **Skip Veiled Shadows.** Rationale: G6 is the only source with a clear position. The 5 minute Shadowfiend CD is sufficient for almost all encounter durations; the points are better spent in Darkness/Twin Disciplines.

### 12. Chakra default — Sanctuary always, or fight-dependent?

- **G1, G3, G4, G7, G8, G9**: All default to Chakra: Sanctuary for raid healing.
- **G6 (Derevka)**: "I find myself pre-pull to be in Chakra: Serenity for the first 15-20 seconds of a fight to then be sure that I can swap to Chakra: Sanctuary when the AOE starts — e.g. Ascendant Council… but that might just be a pipe dream."

**Likely explanation**: G6's pre-pull-Serenity micro-tactic exploits the fact that Chakra has a 30s CD (per State of Mind talent reduction) — by entering Serenity first (e.g., on a pre-pull Heal cast), the priest can swap to Sanctuary when AoE phase actually starts. For a bot, this micro-optimization is fragile and depends on knowing "when AoE will start," which is encounter-aware logic the bot doesn't have.

**Spec disposition**: **Default Chakra: Sanctuary on engage** (cast Prayer of Mending pre-pull, locks in Sanctuary). Runtime command can switch to Serenity for tank-heal mode. **Reject G6's pre-pull-Serenity micro-tactic** on the grounds of "bot can't predict AoE start." Documented as a future-tuning candidate when encounter awareness lands.

### 13. Lightwell pre-pull strategy — always pre-place or hold for big damage?

- **G6 (Derevka, Lightwell article)**: "Many of us toss a Lightwell down right as the fight starts or just before a pull. This is a perfectly great way to use this spell, and by no means a bad usage. … However, we need to keep in mind that so many of the fights available to us right now have periods of phases that have massive amounts of raid damage. … We can use Lightwell as a raid cooldown during this time."
- **G6 (cont.)**: "Pre-emptively cast Lightwell charges should be saved for Phase 2 or dire situations in Phase 1." (Halfus-specific advice)
- **G1, G3, G7**: Pre-pull cast as default.
- **G4**: Pre-pull cast as default.
- **G9**: Pre-pull cast.

**Likely explanation**: G6 advocates for situational deployment (encounter-aware); the other 4 sources advocate for "always pre-pull, recast on CD." For a bot, encounter-aware deployment requires knowing the fight; default behavior should be the more-conservative "pre-pull + recast on CD."

**Spec disposition**: **Pre-pull Lightwell + recast on CD.** Specifically: place at `raid_centroid_at_engage_time` 4s before pull, recast every 3min when off CD if charges <= 2 (don't recast a Lightwell that still has charges; this would waste the cooldown). **Note: this is a known sub-optimal strategy for encounters with phase transitions; it's the "no-encounter-knowledge default" and will be overridden when encounter awareness lands.**

### 14. Holy Word target/placement decision

- **G1 / G7**: Holy Word: Sanctuary placement = "where most raid members cluster."
- **G6**: Doesn't directly address (focuses on rotation philosophy).
- **G5 (wiki)**: Sanctuary heals all within 10y radius for 18s. Smart-heals up to 6 most-injured.

**Spec disposition**: **Sanctuary placement target = `raid_centroid_within_40y_of_self`** at cast time. Compute centroid as the average position of all raid members within 40y; verify cluster has ≥3 members within 10y of that centroid. Falls back to "self position" if no clear cluster exists.

### 15. Mana — Hymn of Hope and Shadowfiend interleave

- **G1**: "Hymn of Hope when needing mana for downtime."
- **G6 (4.3 Hymns article — referenced via search excerpt)**: Hymn of Hope value increased in 4.3 with the haste-channel-buff change.
- **G3 / G4 / G7 / G9**: Treat as standard mana CD.

**Likely explanation**: Player guides assume the human knows when to fire mana CDs based on bar awareness. Bots need numeric thresholds.

**Spec disposition**: **Author judgment.** Spec uses:
- `Shadowfiend at self.mana_pct < 70 AND combat AND has_enemy_target` (cheaper resource, fire first)
- `Hymn of Hope at self.mana_pct < 40 AND no_active_emergency` (higher CD, save for real mana stress)
- These are ordering decisions, not consensus. Cross-link to `docs/specs/holy-priest.md` mana CD rationale.

### 16. Power Infusion — does Holy Priest have it?

- **G5 (wiki)**: "Power Infusion was a Tier 3 Discipline priest talent" — Disc-only.
- **All other guides**: Don't explicitly call out availability for Holy.

**Spec disposition**: **Holy Priest does NOT have Power Infusion in the rotation engine.** This is a wiki-confirmed mechanic — Holy spec cannot pick it up. **Note: this is a deviation from the user's prompt which listed Power Infusion as a candidate cooldown — flagging here so the spec doc clearly excludes it.**

### 17. Inner Focus — does Holy Priest have it?

- **G5 (wiki)**: "Tier 3 Discipline priest talent" — Disc-only.
- **All other guides**: Treat as a Disc-spec ability.

**Spec disposition**: **Holy Priest does NOT have Inner Focus.** Same as Power Infusion — Disc-only talent. **Note: another deviation from prompt; spec doc excludes.**

### 18. Pain Suppression — does Holy Priest have it?

- **G5 (wiki) and consensus**: Pain Suppression is a Tier 5 Disc talent (40% damage reduction on target, 8s, 3 min CD). Disc-only.

**Spec disposition**: **Holy Priest does NOT have Pain Suppression.** Confirmed in user's prompt ("no — Disc only"). Spec doc excludes.

---

## Items NOT addressed by ANY guide (pure author judgment)

These are thresholds and rules the bot author chose. None of the surveyed guides gave numeric values. Documented here so future maintenance knows to NOT cite "the guides said X" for these.

| Spec item | Value chosen | Why no guide consensus exists |
|---|---|---|
| Self-emergency Desperate Prayer trigger | self.hp_pct < 35 | Bot self-preservation is bot-specific; players know when they're dying. |
| Tank-emergency Guardian Spirit trigger | tank.hp_pct < 25 AND no_other_save_ready_in_1.5s | Guides classify GS as "panic button" without quantifying. The 25% gate balances "burn this 3-min CD" risk against "tank dies" risk. |
| Divine Hymn trigger | raid.injured_count >= 4 AND raid.avg_hp_pct < 60 | Guides describe DH as "sustained raid burn" without numeric gates. |
| Hymn of Hope trigger | self.mana_pct < 40 AND no_active_emergency | Guides describe HoH "for mana." |
| Shadowfiend trigger | self.mana_pct < 70 AND combat AND has_enemy_target | Guides describe Shadowfiend "on CD" without mana threshold. |
| Holy Word: Sanctuary cluster trigger | sanctuary_chakra_active AND injured_allies_within_10y_of_centroid >= 3 AND avg_cluster_hp_pct < 80 | Guides say "place where raid is" without numeric gates. |
| Holy Word: Serenity HP gate | serenity_chakra_active AND tank.hp_pct < 90 | Guides say "fire on CD on tank." 90% upper bound prevents pure overheal. |
| Prayer of Healing cluster trigger | party_of_target_avg_hp_pct < 80 AND injured_in_party >= 3 AND mana_pct > 40 | Guides say "for stacked party damage" without gates. |
| Circle of Healing cluster trigger | injured_allies_within_30y_of_anchor >= 3 AND avg_cluster_hp_pct < 85 | Guides say "AoE healing on CD." |
| Renew refresh threshold | tank: missing OR <2s remaining; raid member: missing AND target.hp_pct < 80 | Guides say "Renew on tank" without refresh quantification. |
| Heal filler upper-bound HP | < 90% | Guides say "Heal is the new filler." 90% lets bot top up ambient damage without overhealing. |
| Greater Heal HP gate | target.hp_pct < 60 OR Serendipity_2_stack_active OR target == tank AND target.hp_pct < 70 | Guides say "for big wounds." |
| Flash Heal emergency HP gate | target.hp_pct < 35 OR surge_of_light_proc_active | Guides say "emergency only." 35% gate aligns with Resto Shaman / Holy Pal cross-class default. |
| Mana conserve mode threshold | self.mana_pct < 50 | No guide gave a number. Mirrors resto-shaman / holy-pal cross-class consistency. |
| Mana crisis mode threshold | self.mana_pct < 25 | No guide gave a number. |
| Out-of-combat drink threshold | 30% (force) / 95% (resume) | No guide gave a number. Mirrors cross-class default. |
| Lightwell pre-pull placement | `raid_centroid_within_40y_of_self at engage_time - 4s` | No guide pinned exact placement algorithm; "where the raid stacks" is consensus rationale. |
| Lightwell recast threshold | charges <= 2 AND off_CD | No guide gave a number. |
| Power Word: Shield self/ally trigger | self.hp_pct < 50 OR ally.hp_pct < 30 AND no_weakened_soul_debuff | Holy doesn't get the strong PW:S buffs Disc does, but the spell is castable. Author-judgment trigger. |
| Body and Soul movement-speed handout | OFF (manual command override) | Bot doesn't currently model "ally needs to move out of fire." |
| Leap of Faith trigger | OFF (manual command override) | Pulls an ally to the priest; encounter-specific use. |
| Mass Dispel trigger | dispellable_debuff_count_in_raid >= 3 | No guide gave a number. |
| Chakra-swap trigger (Sanctuary → Serenity) | runtime command flag (default Sanctuary) | Bot has no encounter-script awareness; default to raid-heal mode. |
| Chakra refresh threshold (when buff is about to expire) | Chakra-buff-remaining < 10s AND in combat — recast trigger spell to refresh | Tome of Light + State of Mind talents extend Chakra duration; refresh proactively. |
| Resurrection target priority | healer-class > tank-class > caster-DPS > melee-DPS, all out-of-combat | Cross-class generic rule; not guide-derivable. |
| Shadowfiend pet positioning | leash to caster within 10y; Shadowfiend auto-attacks priest's target | Pet AI default; not in any healer guide. |

---

## Author-judgment items — leveling-specific (NOT guide-derivable)

Mirrors the resto-shaman and holy-paladin survey leveling sections. Spell unlock levels themselves are NOT guesses — they're sourced from Wowhead Cata Classic / Warcraft Wiki and listed in the spec doc, not here.

```yaml
# Spell unlock breakpoints (sourced from Wowhead Cata Classic / Warcraft Wiki, NOT from strategy guides)
spell_unlocks:
  RENEW:                {level: 4,  spell_id: 139}    # baseline (post-Cata leveling consolidation)
  POWER_WORD_SHIELD:    {level: 5,  spell_id: 17}     # baseline
  POWER_WORD_FORTITUDE: {level: 6,  spell_id: 21562}  # baseline
  INNER_FIRE:           {level: 7,  spell_id: 588}    # baseline (post-Cata; was lvl 12 pre-Cata)
  FLASH_HEAL:           {level: 3,  spell_id: 2061}   # baseline (post-4.0.1; was lvl 20 originally)
  HEAL:                 {level: 16, spell_id: 2050}   # baseline (replaces Lesser Heal)
  HOLY_FIRE:            {level: 18, spell_id: 14914}  # baseline
  RESURRECTION:         {level: 18, spell_id: 2006}   # baseline
  MIND_BLAST:           {level: 10, spell_id: 8092}   # baseline (DPS utility)
  PRAYER_OF_HEALING:    {level: 44, spell_id: 596}    # baseline
  GREATER_HEAL:         {level: 38, spell_id: 2060}   # baseline (post-Cata; was lvl 40)
  PRAYER_OF_MENDING:    {level: 68, spell_id: 33076}  # baseline (Cata broadened to all priests)
  BINDING_HEAL:         {level: 64, spell_id: 32546}  # baseline
  SHADOWFIEND:          {level: 68, spell_id: 34433}  # baseline
  HYMN_OF_HOPE:         {level: 70, spell_id: 64901}  # baseline
  MIND_SOOTHE:          {level: 30, spell_id: 453}    # baseline utility
  MASS_DISPEL:          {level: 72, spell_id: 32375}  # baseline
  LEAP_OF_FAITH:        {level: 85, spell_id: 73325}  # baseline (added in 4.0.6)
  # Holy spec abilities:
  CHAKRA:               {level: 10, spell_id: 14751}  # Holy spec choice
  HOLY_WORD_CHASTISE:   {level: 10, spell_id: 88625}  # Holy spec choice (talent-conditional via Revelations)
  CIRCLE_OF_HEALING:    {level: 39, spell_id: 34861}  # Holy spec
  GUARDIAN_SPIRIT:      {level: 70, spell_id: 47788}  # Holy spec (was talent pre-Cata; baseline Holy in Cata)
  DIVINE_HYMN:          {level: 78, spell_id: 64843}  # Holy spec
  # Talented additions:
  LIGHTWELL:            {requires_talent: true, talent_tier: 3}  # 1-point Holy talent
  REVELATIONS:          {requires_talent: true, talent_tier: 5}  # unlocks HW: Sanctuary + HW: Serenity transformations
  TOME_OF_LIGHT:        {requires_talent: true, talent_tier: 3}
  SURGE_OF_LIGHT:       {requires_talent: true, talent_tier: 4}
  TEST_OF_FAITH:        {requires_talent: true, talent_tier: 6}
  BODY_AND_SOUL:        {requires_talent: true, talent_tier: 5}
  INSPIRATION:          {requires_talent: true, talent_tier: 2}
  SPIRIT_OF_REDEMPTION: {requires_talent: true, talent_tier: 4}
  SERENDIPITY:          {requires_talent: true, talent_tier: 4}
  DESPERATE_PRAYER:     {requires_talent: true, talent_tier: 1}
  # Disc-tree talents (5-point secondary slot — 2 in Twin Disciplines):
  TWIN_DISCIPLINES:     {requires_talent: true, tree: discipline}
  # Shadow-tree talents (5-point secondary slot — 3 in Darkness):
  DARKNESS:             {requires_talent: true, tree: shadow}
```

| Default | Value | Why |
|---|---|---|
| Self-emergency HP threshold pre-60 | 25% (vs 35% at max level) | Low-level mob damage is sparse; Desperate Prayer's CD is 2 minutes. Looser gate is risk-tolerant. Mirrors holy-pal/resto-sham convention. |
| Mana floor pre-60 | 0 (no conserve mode) | Mana isn't tight pre-BC; matches cross-class convention. |
| Mana floor 60-70 (BC) | 55 conserve / 30 crisis | BC is the highest mana stress era; matches cross-class. |
| Mana floor 70-80 (WotLK) | 45 conserve / 25 crisis | WotLK regen overhauls relax the floor; matches cross-class. |
| Mana floor 80-85 (Cata) | 50 conserve / 25 crisis | Cata reverts to tighter regen; matches max-level spec. |
| Pre-Heal filler (level 1-15) | Renew at level 4 + Lesser Heal as default direct heal | No guide addresses leveling rotations at this granularity. Lesser Heal is the only direct-heal pre-16. |
| Pre-Chakra filler (level 1-9) | Renew + Lesser Heal/Heal + Smite damage downtime | Chakra unlocks at 10 with the Holy spec choice; pre-10 there's no spec, no Chakra. |
| Solo questing / leveling rotation (Holy spec) | Holy Fire (18+) → Smite spam → Shadow Word: Pain (3+) → Mind Blast (10+) | Holy is "not recommended for leveling" (G1 leveling page) but it IS the bot's mandate; Smite is the primary damage filler. Holy Fire on CD adds DoT damage. SW:Pain is base priest. Shadowfiend on CD for mana. |
| Tank-healing default during dungeon leveling | Heal (16+) + Renew (4+) maintained; PoM (68+) when available; Greater Heal (38+) for emergencies | Bot stays in Sanctuary Chakra for raid healing as soon as Chakra (10) + Revelations (talent ~level 30) are available. |
| Chakra: Sanctuary level-gate | level 10 (spec choice) AND has cast Prayer of Mending OR Prayer of Healing | Chakra is unlocked at the Holy spec selection. Without Revelations talent, Chakra: Sanctuary still gives the +15% AoE healing buff but doesn't summon Holy Word: Sanctuary. |
| Holy Word: Sanctuary level-gate | level 10 spec + Revelations talent (~level 30 talent path) | Without Revelations (Tier 5 Holy talent), Chakra states only buff existing spells; the Holy Word transformations require the talent. |
| Lightwell early — pre-30 leveling | Skip (no talent points yet) | Lightwell is Tier 3 Holy talent; talent path doesn't reach until ~level 25-30. |
| Glyphs pre-25 | Skip | Glyph slots unlock progressively; pre-25 there's nothing important to glyph. |
| Power Word: Shield in healing rotation pre-40 | Reactive use only | Holy spec doesn't have Disc's Rapture/Borrowed Time/Inspiration synergy; PW:S is just a small absorb. Use when Heal cast won't land in time. |
| Inner Fire vs Inner Will pre-10 | Inner Fire | Inner Will is exclusive with Inner Fire; bot defaults to Inner Fire for the +SP and +armor buffs. |

---

## Suggested next research passes

When tuning becomes data-driven (i.e., bots are running and we can measure outcomes):

1. **Disc 5 vs Shadow 3 + Disc 2 secondary split** — replace the heuristic-based default with sim or telemetry. Mental Agility's mana-cost reduction on instant casts vs Darkness's 3% haste is a real sim-able question.
2. **Chakra-swap predicate** — currently bot defaults to Sanctuary; needs runtime command override. Worth testing whether a "tank.hp_pct < 50 sustained for 5s" auto-swap-to-Serenity rule outperforms the static default.
3. **Lightwell auto-recast trigger refinement** — current rule "off CD AND charges <= 2" may be wrong for encounters where charges aren't being consumed (no one's clicking). Add a `lightwell_charges_remaining_avg < 5` time-window trigger.
4. **Holy Word: Sanctuary placement algorithm** — current rule is "raid centroid within 40y." Verify in raid telemetry whether the 6-target smart-heal cap is being saturated; if not, tighter clustering predicates may improve effective HPS.
5. **Renew uptime on tank vs raid members** — current rule maintains on tank only; G6's "Renew on tank, occasionally on a random DPS person" hints that selective raid-Renew-ing has real value.
6. **Surge of Light proc consumption priority** — when proc is up AND multiple targets are injured, who gets the free Flash Heal? Spec uses `lowest-HP injured ally including tank`. Worth measuring against `tank-priority` and `largest-absolute-deficit`.
7. **Pre-pull Chakra activator** — currently spec uses "PoM-on-self-pre-pull." Test whether "Prayer of Healing on the raid stack at T-2s" is better — PoH is cheaper at low spell power and also activates Sanctuary.
8. **Body and Soul utility** — bot does NOT currently use the speed-boost-on-PW:Shield as an active utility. Add encounter-script integration so the bot can shield a friendly to give them a 60% speed burst out of bad placement.
9. **Hymn of Hope mana threshold** — `mana_pct < 40` is bot-aggressive; players typically wait for `<50%` and a low-damage window. Tune from raid telemetry.
10. **Echo of Light reservoir mechanics** — guide consensus is HoT-style "no stack, refreshes on each cast." Verify the actual stacking/overwriting behavior via DBC + in-combat test; biases direct-heal cadence preferences if the reservoir is replaced (lose pending healing) vs added (gain pending healing).
11. **Glyph swaps** — Lightwell glyph (+5 charges) vs Flash Heal glyph (+10% crit on <25% HP targets) — encounter-specific tunable, not a static decision.

---

## Source-coverage gaps to revisit

These are items the spec needs to lock down BEFORE shipping the rotation engine, but no surveyed source resolved them. Author-judgment defaults are in place; flag for follow-up:

- **Echo of Light reservoir mechanic** — does a new direct-heal cast OVERWRITE the pending HoT, or ADD to it? Affects whether casting two direct heals on the same target in 6s is double-mastery or single-mastery. **DBC verification needed.**
- **Chakra duration** — base duration is ambiguous in the wiki (some sources say 60s, some imply unlimited as long as you cast spells of the matching school). The spec needs to know whether to refresh Chakra by recasting the trigger spell (PoM/Heal/PoH/Smite) or whether it just decays naturally. **DBC verification needed.**
- **State of Mind / Tome of Light interaction with Chakra duration** — does Tome of Light extend Chakra duration in addition to reducing Holy Word CD? G3 says yes implicitly; G5 says no. **DBC verification needed.**
- **Holy Word: Sanctuary radius** — guides give 10y; G5 says "10 yard radius" but Wowhead's spell-effect data may show different on-target vs from-caster radius. **DBC verification needed.**
- **Prayer of Healing target restriction** — wiki says "target's party members" — does this mean the priest's party (party-of-self) or the priest can target an ally and PoH heals THAT ally's party? In raid, parties of 5 are the unit. Spec assumes party-of-target. **DBC verification needed.**
- **Lightwell charges baseline (10 vs 15)** — wiki says 10 baseline / 15 with glyph; some search excerpts said "10 or 15." **DBC verification.**
- **Renew tick count without haste** — wiki says "every 3 sec for 12 sec" → 4 ticks. Some sources say 3 ticks. The 12.5% haste-for-5th-tick math implies 4 base ticks. **DBC verification.**
- **Inner Fire spell power amount** — wiki gives 532 SP; this may be level-scaled in 4.3.4 (some sources cite 700+ at level 85). **DBC verification.**
- **Pre-pull cast timing of Lightwell** — Lightwell has a 0.5s cast time and consumes mana; bot needs to cast it after Inner Fire and before combat starts. The exact T-X timing is bot-side.

These are tagged for `docs/research/dbc-verification-checklist.md` style follow-up — not blocking module work, but blocking final spec finalization.

---

## Notes on prompt deviations

A few items in the user's prompt do not survive the source survey and are corrected here:

1. **Power Infusion** — listed in the prompt as a Holy Priest cooldown candidate. Source consensus (G5 wiki) is unambiguous: **Power Infusion is a Tier 3 Discipline talent**, not available to Holy spec. Excluded from the spec.
2. **Inner Focus** — listed in the prompt as a Holy Priest cooldown. Source consensus (G5 wiki): **Tier 3 Discipline talent**, not available to Holy spec. Excluded.
3. **Pain Suppression** — prompt correctly noted "no — Disc only." Confirmed.
4. **Holy Shock equivalent for solo questing** — prompt suggested "Holy Fire damage cast in solo." Confirmed: **Holy Priest does NOT have Holy Shock** (that's Paladin). Solo damage rotation is Smite + Holy Fire + SW:Pain + Mind Blast + Shadowfiend.
5. **Four Chakras** — prompt mentioned "Sanctuary / Serenity / Chastise / formerly Sanctified Wrath?". Source consensus (G5 wiki + G1 + G6): **There are three Chakras in 4.3.4 — Sanctuary, Serenity, and Chastise.** No fourth. (Sanctified Wrath is a Holy Paladin talent; possible name confusion in prompt.)

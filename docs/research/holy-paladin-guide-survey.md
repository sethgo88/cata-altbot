# Holy Paladin 4.3.4 — Multi-Guide Strategy Survey

Audit trail for the strategy decisions that will be encoded in `docs/specs/holy-paladin.md`. The spec doc is the source of truth for the bot; this doc records *which guides agreed*, *where they disagreed*, and *which thresholds were the bot author's judgment call rather than guide consensus*.

Survey conducted 2026-05-02. Numerical data (spell IDs, mana %, cast times, etc.) was sourced separately from Wowhead's Cata archive and Warcraft Wiki and is summarized here only where it directly informs strategy. Per project source registry, hard numbers come from a single canonical reference (Wowhead Cata Classic / Warcraft Wiki); strategy/rotation/threshold content is what's reconciled across the 5 guides below.

---

## Sources

| # | Guide | URL | Author / community | Date / patch | Confidence (rationale) |
|---|---|---|---|---|---|
| G1 | Icy Veins — Cata Classic Holy Paladin Rotation, Cooldowns & Abilities | https://www.icy-veins.com/cataclysm-classic/holy-paladin-pve-rotation-cooldowns-abilities | Icy Veins editorial | Cata Classic re-release (4.3.4 mechanics) | High — primary editorial source; same publisher series as the Resto Shaman survey baseline. Returned readable content. |
| G2 | Icy Veins — Cata Classic Holy Paladin Stat Priority | https://www.icy-veins.com/cataclysm-classic/holy-paladin-pve-stat-priority | Icy Veins editorial | Cata Classic | High — companion page to G1, gave concrete conversion ratios and breakpoints. |
| G3 | Icy Veins — Cata Classic Holy Paladin Talent Builds & Glyphs | https://www.icy-veins.com/cataclysm-classic/holy-paladin-pve-spec-builds-talents-glyphs | Icy Veins editorial | Cata Classic | Medium — fetched but talent-tree numerics under-specified in returned text; cross-referenced via search excerpts. |
| G4 | wowtbc.gg — Cata Classic Holy Paladin | https://wowtbc.gg/cata/class-guides/holy-paladin/ | wowtbc.gg community | Cata Classic | Medium — readable rotation/glyph content, weak on stat priority and explicit talent allocations. |
| G5 | Warcraft Wiki / Wowpedia — Holy Paladin (and per-spell pages: Holy Shock, Beacon of Light, Word of Glory, Light of Dawn) | https://warcraft.wiki.gg/wiki/Holy_Paladin | wiki community | covers original Cata 4.0–4.3 patch notes | High for hard mechanics (spell IDs, level requirements, patch-note transfer percentages); not strategic. |
| G6 | Kurn's Corner — "Cataclysm Holy How-To #1: Specs & Glyphs" / "#2: Spells and Abilities" | https://kurn.info/blog/cataclysm-holy-how-to-1-specs-glyphs/ | Kurn (community blogger, established Holy Pal author) | original 4.0–4.1 era | Medium — era-correct community theorycraft; numbers (Holy Light 12% / Flash 31% / Divine 35% base mana, Beacon 100% / 50% transfer) align with later sources. |
| G7 | Warcraft Tavern — PvE Holy Paladin Rotation & Cooldowns / Healer Guide / Stat Priority | https://www.warcrafttavern.com/cataclysm/guides/pve-holy-paladin-rotation-cooldowns/ | Warcraft Tavern editorial | Cata Classic | Medium — both the rotation and overview pages 403'd to direct fetch; content reconstructed from Google search excerpts that quoted them verbatim. |
| G8 | gotwarcraft.com — Holy Paladin Cataclysm Guide | https://gotwarcraft.com/guides/classguides/holypaladin.php | gotwarcraft.com | original Cata era | Medium — only source that gave a numeric talent-tree split (33/3/5 PvE) and explicit Beacon mechanic ("each heal also heals Beacon for 50%"). |
| G9 | wowbesttalentguides.blogspot.com — "PVE Healer Holy Paladin Talent & Glyphs Guide WoW Cataclysm 4.3.4" | http://wowbesttalentguides.blogspot.com/2014/04/WOW-4-3-4-PVE-Guide-Holy-Paladin-Talent-Build-Glyphs-Cataclysm.html | community blogger | original 4.3.4 (Apr 2014) | Low–Medium — era-exact (private-server 4.3.4 audience), short-form, but matches the higher-confidence sources on glyph picks and stat priority "Int > Spirit > Haste > Crit > Hit > Mastery." |

**Caveats on coverage:**
- G7 (Warcraft Tavern) and the Wowhead Cata-Classic Holy Paladin rotation page (`/cata/guide/classes/paladin/holy/healer-rotation-cooldowns-abilities-pve`) returned 403/redirect to direct fetch; content reconstructed from search excerpts.
- The Warmane forum guide (`forum.warmane.com/showthread.php?t=325551`) and MMO-Champion thread (`mmo-champion.com/threads/854320`) both 403'd; not used as primary sources.
- None of the surveyed guides gave concrete numerical thresholds for self/tank/raid HP-emergency, mana conserve %, mana crisis %, drink %, or Holy-Power-spend HP gates. Those remain bot-author judgment.
- Hard spell numbers (level requirements, mana % of base, cooldowns) are sourced from Warcraft Wiki + Wowhead and cited inline in the survey rather than as a separate table.

---

## Strong consensus (≥3 of the strategy-bearing guides agree)

The strategy-bearing guides for consensus counting are G1, G3, G4, G6, G7, G8 (six sources). G2 and G5 cover stats / hard numbers respectively; G9 is era-correct but short. "x/6" below counts only sources that addressed the item.

| Item | Consensus | Spec disposition |
|---|---|---|
| Holy is the **only viable healing tree**; talents outside Holy are minimal | 6/6 | Spec build is 51 Holy + 5 secondary (Prot or Ret). |
| **Beacon of Light goes on the tank** by default (primary heal target) | 6/6 | Maintenance pass — Beacon on tank, refresh ≥30s before expiry (5 min duration, see Pending Verification). |
| **Holy Shock on cooldown** is the rotation's heartbeat | 6/6 | Tier 2 — cast on CD; targets the lowest-HP tank-or-ally needing a heal, falls back to self/Beacon target for HP generation. |
| **Word of Glory consumes Holy Power at 3 charges** (hold for 3, don't dump at 1–2 except emergencies) | 5/5 (G1, G4, G6, G7, G8) | Tier 4 — gate WoG behind `holy_power == 3` for non-emergency use; emergency-spend at HP ≥1 if tank.hp <30%. |
| **Light of Dawn is the AOE-spend** for Holy Power when 3+ allies need healing; WoG is the single-target spend | 5/5 (G1, G4, G6, G7, G8) | Tier 5 — LoD when `injured_allies_in_30y_cone >= 3 AND holy_power == 3`. WoG wins when the tank/single target is the priority. |
| **Daybreak proc → free extra Holy Shock** (use immediately) | 4/4 (G1, G4, G6, G9) | Tier 2 special-case — if Daybreak buff active, fire Holy Shock even if "on cooldown" (Daybreak skips the CD trigger). |
| **Infusion of Light proc → reduces next big-heal cast time**; consume on Divine Light or Holy Radiance, NOT on Flash of Light | 4/4 (G1, G4, G6, G7) | Tier 6 — Divine Light gate `infusion_of_light_active` AND `target.hp_pct < 70`. |
| **Divine Plea is a regen CD used in low-damage windows**; cancel if a heavy phase starts (50% healing reduction is too costly to ride out) | 5/5 (G1, G4, G6, G7, G8) | Cooldown profile — Divine Plea on CD when `self.mana_pct < 80 AND no_active_emergency`; spec auto-cancels (request-cancel) if `tank.hp_pct < 50` while buff is up. |
| **Hand of Sacrifice on tank during heavy hits** (transfers 30% damage for 12s, can self-kill the paladin) | 4/4 (G1, G4, G6, G7) | Tier 1 (tank emergency CD) — predicate `tank.hp_pct < 50 AND self.hp_pct > 70 AND tank_taking_predictable_burst`. |
| **Lay on Hands is the panic-button**, not rotational | 4/4 (G1, G4, G6, G8) | Tier 1 — `tank.hp_pct < 15 OR self.hp_pct < 15` AND no other CD will save the target in time. |
| **Aura Mastery is encounter-triggered** (magic-damage-aura phase), not generic | 4/4 (G1, G4, G6, G8) | Cooldown profile — gated on `incoming_aoe_magic_damage_phase` flag; bot has no encounter-script awareness today, so spec is "aura_mastery_on_request" with default-off. |
| **Guardian of Ancient Kings (Holy variant) is a healing CD**, ideally stacked with Avenging Wrath | 4/4 (G1, G4, G6, G8) | Tier 1 — fire alongside Avenging Wrath when `raid.injured_count >= 4 OR tank.hp_pct < 40`. |
| **Avenging Wrath is the +20% healing burst CD**, used during heavy-damage phases | 5/5 (G1, G4, G6, G7, G8) | Cooldown profile — fire when `raid.injured_count >= 3 OR tank.hp_pct < 50`. |
| **Holy Light is the cheap, mana-efficient default cast** | 5/5 (G1, G4, G6, G7, G8) | Tier 8 (filler) — when no higher-tier action and `target.hp_pct < 90 AND self.mana_pct > 25`. |
| **Divine Light is the heavy-hitter** for heavy single-target damage | 5/5 (G1, G4, G6, G7, G8) | Tier 6 — gate on `target.hp_pct < 65` (heavy-damage indicator) OR `infusion_of_light_active`. |
| **Flash of Light is emergency-only**, not rotational filler (mana-expensive instant-fast heal) | 5/5 (G1, G4, G6, G7, G8) | Tier 7 emergency — `target.hp_pct < 35 AND holy_shock_on_cd AND no_HoP_charges`. |
| **Holy Radiance for stacked-melee/cluster healing** (3+ targets) | 5/5 (G1, G4, G6, G7, G8) | Tier 5 — when `injured_allies_in_10y_around_friendly_anchor >= 3`. |
| **Judgement every 60s for Judgements of the Pure (haste buff)** | 4/4 (G1, G4, G6, G8) | Maintenance pass — refresh JotP when buff < 10s remaining. |
| **Seal of Insight maintained at all times** (mana regen on melee + healing bonus) | 5/5 (G1, G4, G6, G7, G8) | Maintenance pass — re-seal if `seal_of_insight not active`. |
| **Crusader Strike on cooldown for emergency Holy Power** if in melee range | 4/4 (G1, G4, G6, G8) | Tier 9 (low-priority HP gen) — only when `holy_power < 3 AND in_melee_range AND mana_pct > 40`; not the default HP source (Holy Shock is). |
| **Stat priority: Intellect > Spirit > Haste > Mastery ≈ Crit** | 5/5 (G2, G4, G6, G8, G9) | Reforging/gear advice scope — encoded in `docs/specs/holy-paladin.md` reforging section, not in the rotation engine. |
| **Mastery (Illuminated Healing) is an absorb shield** that scales with Mastery rating; does NOT apply to Beacon-transfer healing | 3/3 that addressed it (G2, G4, G5) | Disposition: bot doesn't model absorb shields directly; spec notes that mana-efficient direct heals (Holy Light especially) feed Mastery shields, which biases ambient-damage filler decisions toward direct heals over Beacon-bounce-only logic. No special engine logic. |
| **Beacon of Light transfers 100% from Holy Light, 50% from all other heals (Word of Glory, Holy Shock, Flash of Light, Divine Light, Light of Dawn)** as of patch 4.3 | 3/3 that gave numbers (G5, G6, G8) | Spec encodes Beacon-aware target selection: when Beacon is on tank and the spell is Holy Light, the bot biases Holy Light targets toward injured non-tank allies (because the tank gets 100% transfer back). |
| **Glyph priority — Prime: Seal of Insight + Holy Shock + (Divine Favor OR Word of Glory)** | 5/5 (G1, G4, G6, G8, G9) | Spec uses Seal of Insight + Holy Shock + Divine Favor as the prime trio (matches G1/G4 directly; G8/G9 list WoG as alternate). |
| **Glyph — Major: Divine Plea (extra mana) + Divinity + (Lay on Hands OR Light of Dawn)** | 4/4 (G1, G4, G6, G8) | Spec: Divine Plea, Divinity, Lay on Hands. |
| **Consumables — Flask of the Draconic Mind, Seafood Magnifique Feast, Mythical Mana Potion** | 5/5 (G1, G4, G6, G7, G9) | Spec encodes these as the default consumable set. |

---

## Disagreements

### 1. Talent allocation — secondary tree split (Prot vs Ret)

- **G3 (Icy Veins talent page)**: 51 Holy + 5 Prot (Divinity 3/3, Eternal Glory 2/2). Implies Divinity's healing-done bonus is the priority; Eternal Glory's WoG-refund for low Holy Power gambling on big spends.
- **G6 (Kurn's Corner)**: 31-Holy core then BRANCH — Prot variant for Divinity 3 + Eternal Glory 2 OR Ret variant for Crusade 3 (boosts Holy Shock damage 30%) + Improved Judgement 3 (Judgement range) + Pursuit of Justice 2 (movement). Notes "fights with movement need Pursuit of Justice over Eternal Glory."
- **G7 (Warcraft Tavern, reconstructed)**: implies "31/5/5" as a common pattern for PvP (G7 PvP doc) but the PvE variant is "33/3/5" — Prot 3 (Divinity) + Ret 5 (Crusade 3 + Pursuit of Justice 2).
- **G8 (gotwarcraft)**: 33/3/5 PvE — Prot 3 (Divinity), Ret 5 (Crusade + Improved Judgement).
- **G4 (wowtbc.gg)**: doesn't specify secondary-tree split clearly.

**Likely explanation**: Tier-13 (Dragon Soul) raid composition shifted preferences over the patch cycle. Crusade boosts Holy Shock damage (a DPS talent) and Holy Shock healing — but only damage scales; for pure PvE healing, Divinity's 6% healing-done is unambiguously better. Pursuit of Justice (movement) value depends on how movement-heavy the encounter is; Dragon Soul is movement-heavy, so movement was prioritized.

**Spec disposition**: **Prot 5 split — Divinity 3/3 + Eternal Glory 2/2** (G3 Icy Veins, the most-recent / most-canonical Cata Classic source). Rationale: Divinity is unambiguous +6% healing done. Eternal Glory's 30% chance to refund Holy Power on Word of Glory amplifies the bot's Tier 4 WoG spam — a small but real DPS-of-healing gain. Pursuit of Justice (movement) is irrelevant to a bot that already has perfect movement intent (no fat-fingered casts to get out of fire). **Note: the bot does not treat this as an encounter-tunable value; if a future encounter profile demands Pursuit of Justice, that's a respec, not a runtime decision.**

### 2. Tower of Radiance — 3/3 vs 2/3

- **G1 / G7 / G8**: 3/3 Tower of Radiance is the "standard" PvE pick — every Flash/Divine Light on Beacon target generates Holy Power.
- **G6 (Kurn)**: notes "2/3 or 3/3" depending on raid comp; mentions in Tier-13 Dragon Soul, Word of Glory's value declined (more AoE damage favored Light of Dawn), so some paladins moved a point out of Tower of Radiance.
- **G4 (wowtbc.gg)**: 3/3 implied (Tower of Radiance listed as a key talent).

**Likely explanation**: Late-Cata raid tier shifted spend pattern toward Light of Dawn (AoE) and away from Word of Glory (single-target), which lowered the value of Tower of Radiance's HP generation. But the talent is still better than alternatives at that tier slot.

**Spec disposition**: **3/3 Tower of Radiance**. Rationale: 4-of-5 sources agree, and the bot is built for the full leveling/dungeon/raid PVE arc — not just T13. In leveling and 5-mans, single-target Word of Glory dominates; Tower of Radiance is the mechanic that keeps HP generation flowing while the bot is healing the tank.

### 3. Conviction inclusion / point count

- **G6 (Kurn)**: Conviction 3/3 is "mandatory" (95%+ uptime, 9% multiplicative healing).
- **G7 / G8 / G9**: Conviction listed as a key talent at 3/3.
- **G1 (Icy Veins rotation page)**: doesn't mention Conviction explicitly in the rotation summary returned.
- **G3 (Icy Veins talent page)**: returned text didn't enumerate Conviction directly — Icy Veins likely includes it but the search didn't surface the line.

**Likely explanation**: G1's rotation page focuses on what the player does (cast priority), not on the static talent-tree picks. Conviction is a passive — gets listed once on the talent page and forgotten in rotation guides. The "missing" mention isn't disagreement; it's section scope.

**Spec disposition**: **Conviction 3/3.** Rationale: 4-of-5 sources call it mandatory; the one that didn't list it (G1 rotation page) wasn't surveying static talents at all. No real disagreement — flagging here only because the resto-shaman survey methodology requires noting any source-coverage gap.

### 4. Speed of Light — value for PvE

- **G1 / G6**: Speed of Light (3/3, +60% movement speed for 4s after Holy Shock-IoL or Divine Favor / Hand of Freedom) "never underestimate getting out of fire."
- **G3 / G8**: Speed of Light unmentioned as PvE-mandatory; treated as optional.
- **G9**: Mentions Speed of Light as a leveling pick.

**Likely explanation**: Mobility talent — value depends entirely on encounter scripting. For a player, "I might get caught in fire" justifies the spend. For a bot, perfect movement-AI obviates much of the value, but the talent ALSO buffs Hand of Freedom uptime which has real raid-utility value.

**Spec disposition**: **Speed of Light 2/2** (Cata version is 2 ranks). Bot doesn't need the player-mobility benefit, but Hand of Freedom is a real raid utility cast and the talent is in the path to Tower of Radiance / Light of Dawn anyway. Cost-zero decision.

### 5. Mana — Divine Plea trigger threshold

- **G1**: "during periods of lower incoming damage but do not hesitate to cancel if … too much healing to be done."
- **G4 / G7**: "during low damage windows or if you need the mana."
- **G6**: numerical hint — "use it when you can afford the 50% healing penalty for 9 seconds" (no mana % given).
- **G8 / G9**: silent on threshold.

**Likely explanation**: Player guides routinely punt on numeric mana thresholds because human players use intuition. None of the 5 surveyed guides gave a number.

**Spec disposition**: **Author judgment.** Spec uses `self.mana_pct < 80 AND no_active_emergency` for Divine Plea — i.e., fire it the moment mana isn't full and there's no fire to put out. This is more aggressive than the guides' "low-damage windows" hint, but it's the correct bot-side choice because the bot can react to a rising-emergency signal mid-channel and cancel within ~150ms of detection.

### 6. Holy Shock — heal vs damage usage during downtime

- **G6**: Holy Shock as a damage cast on enemies for solo questing / downtime (it's HP generation either way).
- **G1 / G4**: Holy Shock is a healing tool; damage variant exists but isn't called out for raid downtime.
- **G7**: silent.

**Likely explanation**: Different scope — solo/leveling guides cover the damage-cast use, raid healer guides assume an enemy is an "irrelevant target."

**Spec disposition**: **Tier 9 (Support DPS, mirroring resto-shaman pattern)** — when nothing higher-tier needs healing AND in combat AND a damageable target is available, Holy Shock can target the enemy to generate Holy Power. Required to NOT break Holy Power economy during low-damage phases. Cross-link to `docs/roles/healer.md` Support DPS section.

### 7. Beacon-bounce — does heal-on-Beacon proc Beacon back?

- **G5 (Wiki/Wowpedia)**: doesn't explicitly resolve. Patch 4.2 notes confirm "transfers 100% of Holy Light, 50% of others" but don't address self-feedback.
- **G6**: doesn't address. G8 says "each heal you cast on party or raid members will also heal the Beacon."
- **Common community knowledge**: Beacon does NOT bounce — i.e., a heal you cast directly on the Beacon target doesn't get transferred back (because the target IS the Beacon). G8's phrasing "party/raid members" hints at this exclusion but doesn't say it explicitly.

**Likely explanation**: This is a mechanic edge case that none of the surveyed strategy guides spell out. Wowhead/wiki patch notes describe the transfer in general terms and assume the reader understands "you can't double-dip on the same target."

**Spec disposition**: **UNVERIFIED — flagged for DBC verification.** Spec assumes Beacon does NOT proc on a heal cast directly on the Beacon target (single-application rule). This affects bot target-selection: when the tank is the Beacon target AND injured, the bot still casts directly on the tank rather than trying to "Beacon-bounce" through someone else. Verified mechanic match against the bot's expected target-selection — but flag to confirm via TC's `Spell.dbc` / SpellLinkedSpell table when DBC access is available. (See `docs/research/dbc-verification-checklist.md` pattern from resto-shaman.)

### 8. Holy Shock target priority — self vs ally

- **G1**: "use on cooldown to maximize Holy Power"; doesn't specify target.
- **G4**: "potent quick heal"; targets implied lowest-HP.
- **G6 (Kurn)**: hints that for HP generation, casting on anyone is fine; for the heal benefit, target the lowest HP.
- **G7 / G8**: silent on target priority.

**Likely explanation**: For a human player, target = lowest HP within range is intuitive and goes unstated.

**Spec disposition**: **Holy Shock target = lowest-HP injured ally within 20y, then tank, then self.** Rationale: lowest-HP-first matches generic healer priority. Tank-second handles the case where the tank is full but a Beacon-transfer would still be valuable. Self-last is the fallback when no allies are injured (HP generation while idle in combat).

### 9. Word of Glory cooldown — does Holy Pal have it?

- **G1 / G6**: Holy Paladin has NO cooldown on Word of Glory (the "Walk in the Light" passive removes the 20s CD that Prot/Ret have).
- **G5 (wiki)**: confirms — WoG has 20s CD baseline, but Holy spec's Walk in the Light passive removes it.
- **All other guides**: don't address explicitly.

**Likely explanation**: Not a disagreement — the absence of CD for Holy Pals is a fact that less-thorough guides simply don't restate.

**Spec disposition**: **Spec encodes Word of Glory as no-CD for Holy Paladin.** No author judgment needed; this is a wiki-confirmed mechanic. The bot can spam WoG every GCD as long as it has 3 Holy Power.

---

## Items NOT addressed by ANY guide (pure author judgment)

These are thresholds and rules the bot author chose. None of the surveyed guides gave numeric values. Documented here so future maintenance knows to NOT cite "the guides said X" for these.

| Spec item | Value chosen | Why no guide consensus exists |
|---|---|---|
| Self-emergency Lay-on-Hands trigger | self.hp_pct < 15 | Bot self-preservation is bot-specific; players know when they're dying. |
| Tank-emergency Lay-on-Hands trigger | tank.hp_pct < 15 AND no other CD ready in next 1.5s | Guides classify LoH as "panic button" without quantifying. |
| Hand of Sacrifice trigger | tank.hp_pct < 50 AND self.hp_pct > 70 AND tank_taking_predictable_burst | Guides describe HoS as "tank emergency" without quantifying. The self.hp>70 gate prevents bot self-kill (HoS transfers up to 100% of paladin HP). |
| Avenging Wrath trigger | raid.injured_count >= 3 OR tank.hp_pct < 50 | Guides describe AW as "heavy-damage burst CD" without numeric gates. |
| Guardian of Ancient Kings trigger | same as Avenging Wrath, fire together when both available | Guides recommend stacking AW + GoAK for damage windows. |
| Divine Plea trigger | self.mana_pct < 80 AND no_active_emergency, with auto-cancel if tank.hp < 50 mid-channel | Guides recommend DP "in low-damage windows"; no mana % given. |
| Divine Plea cancel trigger | tank.hp_pct < 50 OR raid.injured_count >= 4 (mid-channel) | Guides recommend cancel-on-emergency without quantifying. |
| Holy Power emergency-spend (WoG) at < 3 charges | tank.hp_pct < 30 AND holy_power >= 1 | Guides say "save for 3 charges"; the override threshold for emergency is bot-side. |
| Light of Dawn cluster trigger | injured_allies_in_30y_cone >= 3 AND avg_cluster_hp_pct < 80 AND holy_power == 3 | Guides say "use when 4+ targets injured" or "3+"; cluster-size disagreement is small but real. Spec uses 3 (more inclusive — fits 5-man content too). |
| Holy Radiance cluster trigger | injured_allies_within_10y_of_anchor >= 3 AND avg_cluster_hp_pct < 85 | Guides say "stacked melee" without count; 3 matches Light of Dawn for consistency. |
| Mana conserve mode threshold | self.mana_pct < 50 | No guide gave a number. Mirrors resto-shaman value for cross-class consistency. |
| Mana crisis mode threshold | self.mana_pct < 25 | No guide gave a number. Mirrors resto-shaman value. |
| Out-of-combat drink threshold | 30% (force) / 95% (resume) | No guide gave a number. Mirrors resto-shaman cross-class default. |
| Divine Shield self-save trigger | self.hp_pct < 20 AND not_in_freedom_required_position | Guides describe Divine Shield as "panic button"; no HP threshold given. The position gate is to prevent the bot from bubbling while it needs to stay in melee for Crusader Strike. |
| Divine Protection trigger | self.hp_pct < 50 (incoming-damage prediction unsupported, so reactive) | Guides describe DPro as a personal-mitigation CD; no HP threshold given. |
| Aura Mastery default | OFF (manual command override) | Guides say "use during magic-damage phase"; bot has no encounter awareness today. |
| Beacon of Light refresh threshold | duration_remaining < 30s | 5-min duration is generous; 30s buffer prevents accidental drop. No guide gave a refresh number. |
| Judgement (JotP) refresh threshold | jotp_buff_remaining < 10s | 60s buff duration; 10s buffer. No guide gave a number. |
| Crusader Strike emergency HP-gen gate | holy_power < 3 AND in_melee_range AND mana_pct > 40 | Guides recommend CS for HP gen "if in melee"; no mana floor given. The 40% gate prevents wasted mana when Holy Shock will refresh in <6s anyway. |
| Holy Shock target priority hierarchy | lowest-HP injured ally < 20y → tank → self → enemy (Tier 9 support DPS) | Guides don't explicitly enumerate; spec mirrors generic healer pattern. |
| Tier-9 enemy-target Holy Shock gate | combat AND no_higher_priority AND enemy_target_in_20y AND mana_pct > 30 | Guides treat solo / raid-downtime variants separately; bot-side support-DPS pattern unifies. |
| Pre-pull Beacon application timing | 4 seconds before pull | No guide pinned the timing; 4s is enough for the cast + GCD to settle. |
| Hand of Salvation / Hand of Protection / Hand of Freedom defaults | OFF (manual command override) | These are utility CDs that depend on raid context; no guide gave a "fire automatically when X" rule. |

---

## Author-judgment items — leveling-specific (NOT guide-derivable)

Mirrors the resto-shaman survey's leveling section. Spell unlock levels themselves are NOT guesses — they're sourced from Wowhead Cata Classic / Warcraft Wiki and listed in the spec doc, not here.

```yaml
# Spell unlock breakpoints (sourced from Wowhead Cata Classic / Warcraft Wiki, NOT from strategy guides)
spell_unlocks:
  HOLY_LIGHT:           {level: 1,  spell_id: 635}    # baseline
  CRUSADER_STRIKE:      {level: 1,  spell_id: 35395}  # baseline
  JUDGEMENT:            {level: 4,  spell_id: 20271}  # baseline
  WORD_OF_GLORY:        {level: 7,  spell_id: 85673}  # baseline (all paladin specs)
  SEAL_OF_INSIGHT:      {level: 10, spell_id: 20165}  # baseline post-Cata
  HOLY_SHOCK:           {level: 10, spell_id: 20473}  # Holy spec
  FLASH_OF_LIGHT:       {level: 20, spell_id: 19750}  # baseline
  LAY_ON_HANDS:         {level: 24, spell_id: 633}    # baseline
  AVENGING_WRATH:       {level: 30, spell_id: 31884}  # Holy spec at lvl 30 (talent in pre-Cata)
  AURA_MASTERY:         {level: 36, spell_id: 31821}  # Holy spec
  BEACON_OF_LIGHT:      {level: 39, spell_id: 53563}  # Holy spec  (G5 confirmed)
  HAND_OF_SACRIFICE:    {level: 40, spell_id: 6940}   # baseline
  DIVINE_LIGHT:         {level: 64, spell_id: 82326}  # Holy spec
  DIVINE_PLEA:          {level: 68, spell_id: 54428}  # baseline
  HOLY_RADIANCE:        {level: 83, spell_id: 82327}  # Holy spec
  LIGHT_OF_DAWN:        {level: 85, spell_id: 85222}  # Holy spec (Light of Dawn talent gates on 31-Holy)
  GUARDIAN_OF_ANCIENT_KINGS: {level: 85, spell_id: 86150}  # Holy spec final ability
  # Talented additions:
  TOWER_OF_RADIANCE:    {requires_talent: true}
  DAYBREAK:             {requires_talent: true}
  INFUSION_OF_LIGHT:    {requires_talent: true}
  CONVICTION:           {requires_talent: true}
```

| Default | Value | Why |
|---|---|---|
| Self-emergency HP threshold pre-60 | 25% (vs 15% at max level) | Low-level mob damage is sparse; LoH's CD is 8 minutes — don't waste it on minor scares. The looser 25% gate is risk-tolerant. |
| Mana floor pre-60 | 0 (no conserve mode) | Mana isn't tight pre-BC; matches resto-shaman convention. |
| Mana floor 60-70 (BC) | 55 conserve / 30 crisis | BC is the highest mana stress era; matches resto-shaman convention. |
| Mana floor 70-80 (WotLK) | 45 conserve / 25 crisis | WotLK regen overhauls relax the floor; matches resto-shaman. |
| Mana floor 80-85 (Cata) | 50 conserve / 25 crisis | Cata reverts to tighter regen; matches max-level spec. |
| Pre-Holy-Shock filler | Holy Light spam | Pre-level-10 there is no Holy Power; Holy Light is the only heal. |
| Holy-Power-spender pre-WoG (level 1-6) | n/a — bot pools but cannot spend | WoG unlocks at 7. HP just buffers. |
| Solo questing (Holy spec) | Holy Shock damage cast + Crusader Strike + Word of Glory self-heal | Holy is a viable (slow) solo spec; matches G6's "Holy Shock for damage" advice. Bot stays Holy throughout (no Ret respec) since the bot's mandate is dungeon healing. |
| Beacon target during dungeon leveling | Tank | Same as max-level. |
| Light of Dawn pre-T13 cluster trigger | injured_allies_in_30y_cone >= 3 (same as max level) | No reason to vary by level. |
| Glyphs pre-25 | Skip | Glyph slots unlock progressively; pre-25 there's nothing important to glyph. |

---

## Suggested next research passes

When tuning becomes data-driven (i.e., bots are running and we can measure outcomes):

1. **Holy Power emergency-spend threshold** — replace the `tank.hp_pct < 30 AND holy_power >= 1` rule with a value learned from in-combat survival data. Guides agreed "save for 3" but didn't quantify the override.
2. **Light of Dawn cluster size (3 vs 4)** — guides split between "3+" and "4+" injured allies as the trigger. 3 is more inclusive for 5-mans. Verify in raid telemetry whether 4 increases throughput at the cost of latency.
3. **Divine Plea timing** — `mana_pct < 80` is bot-aggressive vs the guides' "low-damage window" hint. Measure DP's actual cast count per fight vs ideal.
4. **Beacon-bounce DBC verification** — confirm via `Spell.dbc` that a heal cast directly on the Beacon target does NOT proc Beacon-back (would be a weird but possible mechanic). Affects target-selection logic.
5. **Speed of Light value re-test** — bot's perfect movement-AI may make this talent fully wasted. Try a build with the 2 points moved to Crusade or Pursuit of Justice and compare HPS.
6. **Tower of Radiance 3/3 vs 2/3** — late-Cata Dragon Soul saw some paladins move a point out. Worth a re-test in T13 specifically; for the leveling/5-man arc, 3/3 dominates.
7. **Conviction uptime** — guides claim 95%+ at low crit; verify in low-gear leveling brackets where crit may be 5% or below.
8. **Avenging Wrath + Guardian of Ancient Kings stacking** — guides recommend stacking but don't address the "wasted on a brief damage spike" failure mode. Encode an encounter-class signal (raid burn phase vs sustained damage) once the bot has it.
9. **Holy Radiance vs Light of Dawn priority** — both are AoE heals; Holy Radiance is mana-expensive (40% of base) but covers melee cluster while Light of Dawn is HP-spend. The "which fires first" tier ordering may need separate gates for sustained-AoE vs burst-AoE phases.
10. **Aura Mastery encounter triggers** — currently OFF-by-default with manual override. Add encounter-script awareness once a generic pre-known-magic-damage-phase signal is available.

---

## Source-coverage gaps to revisit

These are items the spec needs to lock down BEFORE shipping the rotation engine, but no surveyed source resolved them. Author-judgment defaults are in place; flag for follow-up:

- **Beacon bounce on direct-Beacon-target heal** — DBC verification.
- **Walk in the Light WoG-no-CD** — confirmed from G5 wiki, but bot should verify the spell ID 85673's Holy-spec aura interaction in `Spell.dbc`.
- **Holy Power max charges in 4.3.4** — assumed 3. Confirm not 5 (5 was a MoP change). G5 + G1 + G6 all imply 3-cap for Cata.
- **Light of Dawn target count in 4.3** — G5 patch note says "5 → 6 in patch 4.3"; verify the engine's spell-effect-target-count for Light of Dawn matches 6 at runtime.
- **Holy Radiance range / radius** — guides give 10y to 20y; G5 / Wowhead authoritative is 10y radius, 20y from caster. Confirm against `Spell.dbc` AreaEffectRadius before encoding the cluster predicate.
- **Crusader Strike Holy Power generation rate at low levels** — talent prerequisite for HP gen below level 30 unclear from surveyed sources.

These are tagged for `docs/research/dbc-verification-checklist.md` style follow-up.

# Resto Shaman 4.3.4 — Multi-Guide Strategy Survey

Audit trail for the strategy decisions in `docs/specs/resto-shaman.md`. The spec doc is the source of truth for the bot; this doc records *which guides agreed*, *where they disagreed*, and *which thresholds were the bot author's judgment call rather than guide consensus*.

Survey conducted 2026-04-30. Numerical data (spell IDs, mana %, cast times, etc.) was sourced separately from Wowhead's Cata archive and is not duplicated here.

---

## Sources

| # | Guide | URL | Author / community | Date / patch |
|---|---|---|---|---|
| G1 | Icy Veins — Cata Classic Resto Shaman Rotation & Cooldowns | https://www.icy-veins.com/cataclysm-classic/restoration-shaman-pve-rotation-cooldowns-abilities | Icy Veins editorial | Cata Classic re-release (4.3.4 mechanics) |
| G2 | wowtbc.gg — Cata Classic Resto Shaman PvE Guide | https://wowtbc.gg/cata/class-guides/restoration-shaman/ | wowtbc.gg | Cata Classic |
| G3 | Wowhead — Cata Classic Resto Shaman Healer Rotation/Cooldowns | https://www.wowhead.com/cata/guide/classes/shaman/restoration/healer-rotation-cooldowns-abilities-pve | Wowhead editorial | Cata Classic |
| G4 | The Bearded Gnome — "Restoration Shaman Guide as of 4.3 Dragon Soul" | http://thebeardedgnome.blogspot.com/2012/01/restoration-shaman-guide-as-of-43.html | community blogger, Jan 2012 | original 4.3 / Dragon Soul |
| G5 | Warmane forum — "Call of the Elements" by Amashaman | https://forum.warmane.com/showthread.php?t=244195 | private-server raid community | 4.3.4 |

Caveat on coverage: G3 and G5 returned 403 to direct fetch; their content was reconstructed from search-result excerpts that quoted the pages. G4 fetched directly but is sparse on numbers. None of the 5 sources gave concrete numerical thresholds for mana conserve %, crisis %, drink %, or tank-emergency HP % — those remain bot-author judgment, not guide-derivable.

---

## Strong consensus (4-5 of 5 guides)

| Item | Consensus | Spec disposition |
|---|---|---|
| Riptide is **always on cooldown** | 5/5 | Tier 3 — keep on CD, prefer tank, fall back to lowest-HP injured ally |
| Earth Shield is **maintained on tank** (pre-pull + reactive refresh) | 4/4 that addressed it | Maintenance pass — refresh on missing OR charges ≤ 3 |
| Healing Surge is **emergency only**, not rotational filler | 5/5 | Tier 1 (self), Tier 2 (tank), Tier 7 (non-tank ally <50%) — never a default TW consumer |
| Tidal Waves should be kept up — primarily fed by Riptide | 5/5 | Tier 3 generates; Tier 8 consumes |
| Spirit Link Totem is a **major emergency CD**, not rotational | 4/4 | Tier 4 with restrictive predicate (3+ party <40%, clustered within 10y) |
| Mana Tide is a **raid-benefit CD** (not "self at X%") | 3/3 that addressed it | Cooldown profile uses self<60% as a single-bot proxy; documented as such, not as consensus |

---

## Disagreements

### Preferred Tidal Waves consumer

- **G4 (BeardedGnome, Jan 2012)**: GHW dominates ~98.54% of the time as the TW consumer.
- **G1 / G2 / G3 / G5 (modern Cata Classic)**: HW is the cheap default; GHW reserved for heavier wounds.

**Likely explanation**: G4 is original-4.3 progression-Dragon-Soul gear, where healer mana pools and intellect scaling supported GHW spam. Modern Cata Classic re-release tunes for broader audience and average gear, where HW efficiency wins.

**Spec disposition**: Sided with the modern consensus (4 guides). Tier 8 (HW filler) is the default TW consumer; Tier 6 GHW gates on mana>50% so it only fires when the bot can afford it.

### Mana Tide threshold

- **G1 (Icy Veins)**: "When group is at 80% mana or less" (group-relative).
- **G2, G3, G4, G5**: No numeric threshold given — "when raid will benefit," "stack with other CDs."

**Spec disposition**: Bot has no group-mana awareness API yet, so spec uses self.mana<60% as a single-actor proxy. Documented in the cooldown rationale that this is a heuristic, not a consensus value. If the bot ever gets group-mana telemetry, switch to the Icy Veins 80% group-mana rule.

### Nature's Swiftness pairing

- **G1 (Icy Veins)**: NS + Greater Healing Wave (single-target panic).
- **G2 (wowtbc.gg)**: NS + Healing Wave OR NS + Chain Heal (versatile).

**Spec disposition**: Tiered NS to fire alongside tank emergency (Tier 2) → empowered GHW on tank, matching Icy Veins. NS+CH alternate pairing noted in cooldown rationale as a tunable for when AoE-cluster emergencies dominate the encounter mix.

---

## Spec items that are **author judgment**, not guide consensus

These are thresholds and rules the bot author chose. None of the surveyed guides gave numeric values; they are internally consistent with the spec's overall philosophy but should not be cited as "from the guides."

| Spec item | Value chosen | Why no guide consensus exists |
|---|---|---|
| Self-save HSurge trigger | self.hp_pct < 35 | Bot self-preservation is a bot concern; player guides assume the human knows when they're dying. |
| Tank emergency HSurge trigger | tank.hp_pct < 40 | Guides classify HSurge as emergency-only without quantifying "emergency." |
| Chain Heal AoE cluster trigger | 3+ injured allies within 12y at <85% | Guides describe "when sustained group damage" without numeric gates. 12y matches the spell's jump radius. |
| Spirit Link Totem trigger | 3+ party <40% AND clustered within 10y | Guides describe SLT as emergency without quantifying. 10y matches the totem's effect radius. |
| Mana conserve mode threshold | self.mana_pct < 50 | No guide gave a number. Consistent with the principle "downshift to HW for efficiency." |
| Mana crisis mode threshold | self.mana_pct < 25 | No guide gave a number. Internally consistent with potion / mana-tide-priority logic. |
| GHW gate (require mana > 50% AND TW active AND target < 60%) | as listed | Guides describe GHW as "for heavier wounds." Specific numeric gates are bot-side. |
| HW filler upper-bound HP | < 90% | No guide gave a number. 90% lets the bot top up ambient damage without overhealing. |
| Earth Shield refresh threshold | charges ≤ 3 (of 9) | No guide gave a number. Below 3 charges is a reasonable buffer to ensure ES doesn't fully expire mid-tank-hit. |
| Out-of-combat drink threshold | 30% (force) / 95% (resume) | No guide gave a number. |
| Earth Elemental Totem trigger | raid + tank<25% + SLT on CD | No surveyed guide addressed EET for healers. Treated as last-ditch threat backup; spec author's call. |
| Nature's Swiftness HP gate for emergency | tank<30% (within Tier 2) | No guide gave a number. |

---

## Suggested next research passes

When tuning becomes data-driven (i.e., bots are running and we can measure outcomes):

1. **Mana conservation thresholds** — replace the 50% / 25% guesses with values learned from sim or in-game telemetry.
2. **Chain Heal cluster heuristic** — guide consensus is "fire on cluster"; the right cluster definition (radius, count, HP%) is encounter-dependent.
3. **HSurge emergency HP gates** — 35% (self), 40% (tank), 50% (party) are educated guesses; tune from in-combat survival data.
4. **Mana Tide group-mana variant** — once the bot can read party-mana, switch from self-mana proxy to the Icy Veins group<80% rule and compare uptime/effect.

---

# Leveling Rotation — Multi-Guide Survey (added 2026-05-01)

Audit trail for the **Leveling rotation** section added to `docs/specs/resto-shaman.md`. Strategy reconciled across 7 distinct sources; spell-unlock levels sourced from Wowhead Cata Classic (single canonical source per project convention).

## Sources (leveling rotation)

| # | Guide | URL | Author | Date/Patch | Confidence |
|---|---|---|---|---|---|
| L1 | Icy Veins — Cata Classic Resto Shaman Leveling | `icy-veins.com/cataclysm-classic/restoration-shaman-leveling` | Icy Veins staff | Cata Classic 2024-2025, 4.3.4 | High |
| L2 | Icy Veins — Cata Classic Resto Shaman Talents/Glyphs | `icy-veins.com/cataclysm-classic/restoration-shaman-pve-spec-builds-talents-glyphs` | Icy Veins staff | Cata Classic, 4.3.4 | High |
| L3 | Wowhead — Cata Classic Resto Shaman Healer Leveling Tips | `wowhead.com/cata/guide/classes/shaman/restoration/healer-leveling-tips` | Wowhead staff | Cata Classic, 4.3.4 | Medium |
| L4 | wowtbc.gg — Cata Classic Resto Shaman | `wowtbc.gg/cata/class-guides/restoration-shaman/` | Community | Cata Classic, 4.3.4 | Medium |
| L5 | guiaswow.com — Quick Resto Shaman Cataclysm | `en.guiaswow.com/shamans/quick-guide-shaman-restoration-cataclysm.html` | Guiaswow community | Original Cata era (4.x) | Medium |
| L6 | Warmane forum — Amashaman 4.3.4 Resto guide | `forum.warmane.com/showthread.php?t=244195` | Amashaman | Original 4.3.4, private server | Medium (not directly fetchable) |
| L7 | Pocket Heals blog — Resto Shaman in Cataclysm | `pocketheals.blogspot.com/2010/12/guide-to-pve-restoration-shaman-in.html` | Pocket Heals | 2010, original Cata launch | Low (era-correct but pre-4.3.4 nerfs) |

## Strong consensus (4+ guides agree)

| Topic | Consensus | Spec disposition |
|---|---|---|
| Spec for solo + dungeons | Elemental for solo questing + Resto for dungeon healing (dual-spec) | Spec doc takes the dungeon-only path: pure Resto from level 10. Bot's mandate is dungeon healing. |
| Earth Shield as level-10 spec ability | All Cata sources agree: 4.0.1 made it baseline at spec choice | `EARTH_SHIELD.required_level: 10` |
| Riptide as level-10 spec ability | Same | `RIPTIDE.required_level: 10` |
| Tidal Waves is the keystone Resto talent | Universally treated as mandatory in Resto leveling | Talent allocation order puts Tidal Waves at tier 6-7 (~level 35-44) |
| Water Shield > Lightning Shield for Resto | Once Water Shield unlocks at 20, switch | `Maintenance pass` rule — already in spec |
| Glyph priority: Riptide → Earthliving → Earth Shield (primes) | Consensus | Encoded in Glyph priorities block |

## Disagreements

| Topic | Source A | Source B | Resolution |
|---|---|---|---|
| Greater Healing Wave required level | Wowhead Cata Classic: 68 | Some legacy guides: 60 (likely MoP 5.0.4 confusion) | Treat 68 as authoritative; flagged UNCERTAIN |
| Healing Surge required level | Wowhead Cata Classic: 20 | One search snippet: 7 (confused with Healing Wave) | Treat 20 as authoritative |
| Earthliving Weapon required level | Wowhead Cata Classic: 54 | Pocket Heals 2010: 30 (pre-4.0.6 patch) | 54 is correct for 4.3.4 |
| Mana Tide / Spirit Link / Nature's Swiftness — talent vs baseline | Research agent: Resto talents at tiers 3 / 5 / 6 | Author memory: SLT is baseline level-85 in original Cata | UNCERTAIN — flagged in spec UNVERIFIED items table; verify via TC `Spell.dbc` |
| Pure-Resto leveling viability | Most guides recommend dual-spec | Bot's use case = dungeons only, so pure Resto is acceptable | Spec doc takes pure-Resto path |

## Author-judgment defaults at low levels (NOT guide-derivable)

These are spec-doc defaults the bot author chose. No leveling guide pins them down.

| Default | Value | Why |
|---|---|---|
| Self-emergency HP threshold pre-60 | 25% (vs 35% at max level) | Low-level mob damage is sparse |
| Mana floor pre-60 | 0 (no conserve mode) | Mana isn't tight pre-BC |
| Mana floor 60-70 (BC) | 55 conserve / 30 crisis | BC era is the highest mana stress |
| Mana floor 70-80 (WotLK) | 45 conserve / 25 crisis | WotLK regen overhauls relax the floor |
| Pre-Earthliving imbue choice | Rockbiter (default) | No guide pins this; Rockbiter has a small threat-reduction proc |
| Lightning Bolt filler in dungeons | Stop weaving past level 30 | No guide addresses this for low levels |
| Respec during leveling | Stay Resto throughout | No guide recommends mid-leveling respecs for dungeon-only Resto |
| Chain Heal pre-60 trigger | 3+ <80% (vs 3+ <85% at max) | Tighter at low gear levels because Chain Heal is mana-expensive |
| Greater Healing Wave required level | 68 (Wowhead Cata Classic value) | Some sources cite 60 — flag and confirm via DBC if implementing |

---

## Correction: Telluric Currents inclusion (2026-05-01)

**The original leveling-research agent's recommendation to "skip Telluric Currents (DPS talent, not for Resto healers)" was wrong.** Verified via Warcraft Wiki:

- Talent tree: **Restoration**, tier 5
- Ranks: 2 (20% / 40% of Lightning Bolt damage returned as mana)
- Effect: turns Lightning Bolt into a mana-positive cast against damaged enemies — specifically designed for healers to use offensive casts during downtime as mana sustain

**Spec-doc changes from this correction:**
- Added Tier 9 (Support DPS) to the Rotation Priority List
- Added Telluric Currents (2/2) to the talent path at levels 30-34
- Updated `support_dps_during_leveling` author-judgment default to reflect TC-enabled support DPS
- Cross-linked to `docs/roles/healer.md` Support DPS section for the cross-class generalist pattern

**Lesson for future research passes:** when an agent recommends "skip talent X," verify the talent's actual mechanic before encoding the recommendation. Talents with names that sound like DPS abilities (Telluric Currents, Magma Totem, etc.) may have specific healer-utility designs.

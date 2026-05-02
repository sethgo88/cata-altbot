# Throne of the Tides — Multi-Guide Strategy Survey

Audit trail for the strategy decisions in `docs/dungeons/throne-of-the-tides/encounter.md` and `docs/dungeons/throne-of-the-tides/healer.md`. Records which guides agreed, where they disagreed, which thresholds are bot-author judgment, and the full source list (cached for future role docs — `ranged-dps.md`, `melee-dps.md`, `tank.md` — so we don't re-research).

Survey conducted 2026-05-01. Spell IDs sourced separately from Wowhead's Cata archive (single-source per project convention) and not duplicated here.

---

## Sources (8 — exceeded the 4-5 ask)

| # | Guide | URL | Author / Community | Era / Patch | Confidence |
|---|---|---|---|---|---|
| G1 | Icy Veins — ToT Cata Classic Dungeon Guide | `icy-veins.com/cataclysm-classic/throne-of-the-tides-dungeon-guide` | Icy Veins editorial | Cata Classic re-release | High |
| G2 | Wowhead — Cata NPC pages (per boss) | `wowhead.com/cata/npc=40586/lady-nazjar`, `/40765/commander-ulthok`, `/40788/mindbender-ghursha` | Wowhead DB + community | Cata Classic + original 4.0–4.3 data | High (DB-backed) |
| G3 | Warcraft Wiki tactics pages | `warcraft.wiki.gg/wiki/Lady_Naz%27jar_(tactics)` etc. | Wowpedia / Warcraft Wiki community | 2010-2012 + later edits | High |
| G4 | Dexerto Cata Classic guide | `dexerto.com/world-of-warcraft/wow-cataclysm-classic-throne-of-the-tides-guide-boss-tactics-loot-heroic-more-2722504/` | Dexerto staff | 2024 Cata Classic | Medium |
| G5 | GuiasWoW Heroic/Normal | `en.guiaswow.com/mazmorras/guia-trono-mareas-throne-tides-heroico-normal.html` | GuiasWoW community | 4.3.4 era | Medium |
| G6 | Type "H" For Heals — healer blog | `typehforheals.com/2010/08/03/cataclysm-beta-healing-throne-of-the-tides/` | Vidyala / healer blog | Cata beta + live | High (healer specifics) |
| G7 | Green Bar Spec — resto druid blog | `greenbarspec.wordpress.com/2011/01/13/throne-of-the-tides-healing-101/` | Resto druid blogger | 4.0–4.1 live | Medium |
| G8 | GotWarcraft Heroic Boss Guide | `gotwarcraft.com/throne-of-the-tides-heroic-boss-guide/` | GotWarcraft staff | 4.x era | Medium |

These 8 guides cover both **launch-era 4.3.4** (G3/G5/G6/G7/G8) and **Cata Classic re-release** (G1/G2/G4) content. Mechanics are equivalent for the 4.3.4 server target.

---

## Strong consensus (4+ guides agree)

| Topic | Consensus | Spec disposition |
|---|---|---|
| Lady Naz'jar Shock Blast must be interrupted on heroic | 6/8 explicit, 2 implicit | encounter.md: `priority: MUST_INTERRUPT`; healer.md: tier-2 threshold raised to 50% to cushion missed interrupts |
| Fungal Spores is a Disease debuff, dispellable | 5/8 | encounter.md: `dispel_type: disease`; healer.md: top dispel priority on Naz'jar |
| Waterspout triggers at 60% / 30% HP | 7/8 (1 Spanish source said 66/33 — likely localization) | encounter.md: `[60, 30]` |
| Ulthok Dark Fissure expands in heroic, requires kiting | 4/8 explicit | encounter.md: `heroic_extra` notes; healer.md: positioning rule |
| Curse of Fatigue is a Curse, dispellable, and high priority (slowed players die in fissures) | 5/8 | encounter.md: `priority: HIGH_DISPEL`; healer.md: "fight-defining healer task" |
| Ghur'sha Absorb Magic should be purged/spellstolen — heals boss 3x absorbed | 5/8 | encounter.md: `priority: HIGHEST_DISPEL_OFFENSIVE`; healer.md: stop-casting rule for Resto Shaman (no native purge) |
| Enslaved player must be DPS'd to 50% HP — do NOT heal | 7/8 (universal in healer guides) | healer.md: `enslave_heal_trap` anti-pattern, target-filter rule |
| Ozumat P2 requires killing all 3 Faceless Sappers | 6/8 | encounter.md: phase 2 end_condition |
| Ozumat P3 Tidal Surge buff: +500% HP, +500% healing, +2000% damage | 3/8 numeric (the rest describe directionally) | encounter.md: phase 3 buff; healer.md: "your numbers are MASSIVE" framing |
| Ozumat P3 is a stacking-DoT DPS race | 8/8 | healer.md: `cooldown_hoarding_p3` anti-pattern — pop everything from phase start |

---

## Disagreements

### Add-kill priority on Lady Naz'jar Waterspout
- **G3 (Wiki) / G1 (Icy Veins)**: kill Honor Guard first, CC casters
- **G8 (GotWarcraft)**: eliminate melee add first, then casters
- **G6 / G7 (healer blogs)**: kill Tempest Witches first to protect ranged

**Likely explanation**: party composition. Honor Guard hits the tank hardest; Witches threaten the rest of the party (Chain Lightning + Lightning Surge AoE). Healer-perspective sources favor Witch-priority (their party survival concern); tank/general sources favor Honor Guard-priority (raw threat concern).

**Spec disposition**: encounter.md notes `default: CC one Witch, kill Honor Guard first; otherwise kill Witches`. Defaults to most-cited (Honor Guard) but flags it as party-comp dependent. healer.md raid-heal emphasis assumes Chain Lightning landings happen — defensive posture.

### Squeeze damage value (Ulthok)
- **G2 (Wowhead)**: 12.5k Shadow / s
- **G3 (Wiki)**: 5k Shadow / s

**Likely explanation**: normal vs heroic.

**Spec disposition**: encounter.md flags as UNCERTAIN; treats as significant single-target spike (~30-50k+ over 6s) regardless. Bot dispels not relevant (not dispellable); bot only needs to know it's "big single-target damage on a stunned ally."

### Fungal Spores tick damage (Naz'jar)
- **G1 / G6 / G7**: ~10k per 3s
- **G2**: ~15k per 3s

**Likely explanation**: normal vs heroic.

**Spec disposition**: encounter.md flags as UNCERTAIN. Bot dispels regardless of value — the value doesn't change behavior.

### Absorb Magic duration (Ghur'sha)
- **G3 (Wiki)**: 3 seconds
- **G2 (Wowhead)**: 5 seconds

**Spec disposition**: encounter.md notes `duration_ms: 3000-5000`; healer.md says "purge ASAP, don't depend on duration."

### Unrelenting Agony interruptibility (Ghur'sha)
- **G3 (Wiki)**: not interruptible (authoritative)
- some YouTube guides imply yes

**Spec disposition**: encounter.md treats as `interruptible: false` (safer default for bot). If a future test proves otherwise, change one line.

### Faceless Sapper kill technique (Ozumat P2)
- Most guides: ranged DPS kills ceiling-tethered Sappers
- Some imply melee can leap up / charge

**Spec disposition**: encounter.md notes "ceiling targets — ranged DPS responsibility." The melee-DPS doc will need to address whether melee can/should attempt to engage.

---

## Per-role consensus (must-do lists, for future role docs)

### Tank (for tank.md — last per ordering rule)
- Face Naz'jar Honor Guard / Erunak away from group (cone abilities)
- Kite Ulthok along walls; drop Dark Fissures clean (heroic)
- Kite Blight Beasts on Ozumat P2 — never cluster with party
- Pick up Behemoth on Ozumat P1, point cone outward
- Pop defensive on Shock Blast if interrupt lapses (Naz'jar)

### Healer (encoded in healer.md this iteration)
- Dispel Fungal Spores (Disease) on Naz'jar
- Dispel Curse of Fatigue on Ulthok (top priority)
- Help purge Absorb Magic on Ghur'sha (stop-casting if no native purge)
- Do NOT heal the Enslaved player
- Pre-empt Squeeze and Geyser knockback windows
- Pop CDs from start of Ozumat P3

### Melee DPS (for melee-dps.md — pre-cached for that doc's authoring)
- Interrupt Naz'jar Shock Blast (top priority, lethal-to-tank on heroic)
- Interrupt Erunak Lava Bolt
- Burn Enslaved player to 50% in P2 of Ghur'sha (target switch)
- Track fissure spawns on Ulthok — tunneling kills you
- Move with tank during Ulthok kite

### Ranged DPS (for ranged-dps.md — pre-cached for that doc's authoring)
- Interrupt Vicious Mindlasher Brain Spike (Ozumat P1) — top priority
- Primary Sapper killers in Ozumat P2 (ceiling targets)
- CC Tempest Witches on Naz'jar (Polymorph / Hex / Repentance / Trap)
- Spellsteal/purge Absorb Magic on Ghur'sha
- Backup interrupt rotation on Shock Blast

---

## UNCERTAIN items (sources disagree — confirm via DBC or testing)

Same list as in `encounter.md`'s UNCERTAIN section, kept here for the audit trail:

| Item | What's uncertain |
|---|---|
| Fungal Spores tick damage | 10k vs 15k per 3s |
| Squeeze damage | 5k/s vs 12.5k/s |
| Absorb Magic duration | 3s vs 5s |
| Unrelenting Agony interruptibility | conflicting (default: false) |
| Add-kill priority during Naz'jar Waterspout | varies by party-comp; default to Honor Guard first |
| "Beauty / pet pack" in ToTT | not present (Beauty is in Blackrock Caverns) |
| "Naga Mistral" mob name | not in current data; likely Naz'jar Tempest Witch |

---

## Author-judgment thresholds (NOT guide-derivable)

These values in `healer.md` were chosen by the bot author to encode the guides' qualitative direction as quantitative predicates. Marking them so future tuning sessions know which numbers came from the guides vs which came from judgment.

| Healer doc setting | Value | Why no guide consensus |
|---|---|---|
| Naz'jar tier-2 tank emergency raise | tank.hp_pct < 50 (from baseline 40) | Guides say "interrupt or tank dies"; specific HP threshold for the bot's reactive heal is bot-author call |
| Ulthok tier-2 tank emergency raise | tank.hp_pct < 45 | Guides describe constant pressure; no numeric value given |
| Ulthok Chain Heal tightening | 3+ <80% (from baseline 85%) | Guides describe mobility — bot author tightens to avoid greedy CH casts during fissure spawns |
| Ulthok Mana Tide trigger | self.mana_pct < 55 (from baseline 60) | Short fight; guides give no numeric mana trigger |
| Ozumat tier-2 tank emergency raise | tank.hp_pct < 50 | Behemoth Shadow Blast is huge; bot author estimates the cushion |
| Ozumat Chain Heal threshold | 2+ <85% (loosened from 3+ <85%) | Small party (5) + constant damage = baseline too greedy |
| Ozumat crisis_mode raise | mana_pct < 30 (from baseline 25) | P3 is mana-heavy; bot author shifts the floor up |
| Ozumat Spirit Link timing | "first cluster + ~5 Blight stacks" | Guides say "use during P3"; specific stack count is bot author |
| Cross-fight cooldown budget counts | as listed | Derived by combining per-fight plans; not from any single guide |

---

## Suggested next research passes

When tuning becomes data-driven (bots actually running ToTT):

1. **Fungal Spores / Squeeze / Absorb Magic exact values** — pull from server's `Spell.dbc` once cata-altbot is integrated and verified, replace UNCERTAIN with hard numbers
2. **Add-kill priority** — instrument bot logs to track per-pull survival outcomes by add-priority choice, see which actually wins
3. **Naz'jar tier-2 threshold tuning** — bot tank-survival metrics will tell us whether 50% is overcautious or correct
4. **Ozumat P3 cooldown timing** — Spirit Link timing on stack-count is the most tunable single decision; instrument to find the optimal stack count

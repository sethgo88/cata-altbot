# Leveling Dungeons — Multi-Guide Strategy Survey

Audit trail for the strategy decisions in `docs/dungeons/leveling.md`. Records which guides agreed on the era-by-era patterns, where they disagreed, and which thresholds are bot-author judgment vs guide-derivable.

Survey conducted 2026-05-01. 11 distinct sources reviewed.

---

## Sources

| # | Guide | URL | Author / Community | Date / Patch | Confidence |
|---|---|---|---|---|---|
| G1 | Wowhead Cata Classic Dungeons Overview | `wowhead.com/cata/guide/dungeons-overview` | Wowhead (Paryah) | 2022 update; Cata 4.3.4 | High (canonical dungeon list/levels) |
| G2 | Wowhead Cata Classic Leveling Tips & Routes | `wowhead.com/cata/guide/leveling-tips-tricks-routes` | Wowhead (Passion) | 2024-07-02 | High |
| G3 | Icy Veins WotLK Classic Dungeon Guides | `icy-veins.com/wotlk-classic/dungeon-guides` | Icy Veins staff | WotLK Classic era | High |
| G4 | Wowpedia / Warcraft Wiki "Dungeon Finder" | `wowpedia.fandom.com/wiki/Dungeon_Finder` + `warcraft.wiki.gg/wiki/Dungeon_Finder` | Wiki contributors | Patch-tracked; 4.3.4 entries present | High |
| G5 | Manalicious — Cataclysm Heroic Tips for Mages | `manalicious.wordpress.com/2011/01/02/cataclysm-heroic-tips-and-strategies-for-mages-part-1/` | Vidyala (cakeitso) | 2011-01-02 (live Cata) | Medium-High |
| G6 | WilfridWong — Cataclysm Heroic Quick Reference | `wilfridwong.com/articles/world-of-warcraft/cataclysm-heroic-dungeons-quick-reference-guide/` | Wilfrid Wong | 2011-04-30 (live Cata) | Medium-High |
| G7 | Wowpedia — Active Mitigation + Crowd Control | `wowpedia.fandom.com/wiki/Active_Mitigation`, `wowpedia.fandom.com/wiki/Crowd_control` | Wowpedia contributors | Patch-tracked | High |
| G8 | TheGamer — Wrath Dungeons Ranked | `thegamer.com/world-of-warcraft-classic-wow-wrath-of-the-lich-king-dungeons-ranked-best/` | Shane Martin | 2022-10-02 | Medium |
| G9 | GameRant — TBC Dungeons Ranked by Difficulty | `gamerant.com/wow-classc-burning-crusade-dungeons-ranked-hardest/` | Kristy Ambrose | 2021-06-22 | Medium |
| G10 | Epiccarry — Cataclysm Classic Leveling Guide | `epiccarry.com/blogs/wow-cataclysm-classic-leveling-guide/` | friolt | 2024 / updated 2026-03-05 | Medium |
| G11 | StarWarsUnity — Evolution of WoW Dungeon Design | `starwarsunity.net/the-evolution-of-dungeon-design-in-wow-from-vanilla-to-modern-content/` | Raymond Newman | 2025-05-19 | Medium (high-level only) |

---

## Strong consensus (4+ guides agree)

| Topic | Consensus | Spec disposition |
|---|---|---|
| Vanilla 5-mans = sprawling, low CC demand outside SM Cathedral / Scholo / Strat | G1, G2, G8, G11 + general MMO-Champion threads | Era table: vanilla `cc_required: optional (with exceptions)` |
| BC introduced **mandatory CC** and kick discipline | G7, G9, G11; Wowpedia CC | Era table: BC `cc_required: MANDATORY`, `kicks_required: MANDATORY` |
| WotLK **streamlined** dungeons; **trivialized CC** on normal; AoE pulls became standard | G3, G5, G8, G11 | Era table: WotLK `cc_required: largely optional`, `pull_discipline: loose again` |
| Cata **brought CC and interrupts back as mandatory**; healer mana matters again; tank cooldown rotation matters again | G1, G5, G6, G10 + per-dungeon Dexerto/Icy Veins | Era table: Cata `cc_required: MANDATORY`, `healer_mana_demand: CRITICAL` |
| LFD random-dungeon bonus exists at every bracket; cross-realm at 4.3.4 | G4 | LFD specifics block |
| Heirlooms stack ~45% XP | G2, G10 + multiple boost-blog sources | Leveling-only considerations |
| Hybrid quest-while-queued is the fastest leveling flow | G2, G10 + community consensus | Leveling-only considerations: bot strategy = `QueueRandomDungeon()` at instance entry, quest until pop |

---

## Disagreements

### Speed of dungeon vs quest leveling

- **Boost-blog sources** (Simpleboost, IGV, Epiccarry G10): dungeons can rival or beat questing XP/hour
- **Reddit / forum + Wowhead Passion (G2)**: questing is faster solo; hybrid is optimal

**Likely explanation**: dungeon-only viability depends on whether the bot is in a synergistic group composition AND has good gear. Solo questing has no group friction.

**Spec disposition**: doc treats dungeon-only leveling as **viable but not optimal**. Bot's value-add is "always available group" not "raw XP/hour winner." Leveling-only considerations block names this trade-off.

### Cata-Classic difficulty for normal-mode leveling dungeons

- **2024-2026 community threads** (Cata Classic re-release era): claim Cata-Classic 80-85 normals are easier than original-Cata heroics
- **Original-Cata-launch sources** (G5 Manalicious, G6 Wilfrid Wong): writing about heroics, not normals

**Likely explanation**: Cata Classic 2024 re-release had pre-tuned numbers; original launch had post-launch nerfs over time. Normal-mode 80-85 is forgiving; heroic-mode is the spike.

**Spec disposition**: doc's Cata-leveling era still describes CC/kick as mandatory because the bot will run heroics at 85 too. The per-dungeon doc (e.g., ToTT) covers heroic specifics.

### Wrath dungeon "easiness" claim

- **G3 / G8**: Wrath dungeons are streamlined and easy
- **G8 specifically calls out exceptions**: Halls of Lightning Loken, Utgarde Pinnacle Skadi gauntlet are notable difficulty spikes

**Spec disposition**: Era table notes "notable difficulty spikes (Loken, Skadi gauntlet)" as Wrath signature mechanics. Bot logic should not assume all Wrath content is trivially easy.

---

## Author-judgment items (NOT guide-derivable)

These are bot-author defaults the leveling-dungeons doc encodes. No guide pins them down.

| # | Item | Default decision | Tunable? |
|---|---|---|---|
| 1 | CC threshold by group composition | If 1+ CC class in group, mark CC target on first dangerous-caster pack of every dungeon | Yes — per-encounter override |
| 2 | Interrupt-rotation assignment with 2+ kickers | Primary = melee with shortest CD; secondary = ranged | Yes — per-encounter override |
| 3 | Vehicle-phase handling (Oculus) | Blacklist (recommended) | Yes — bot-policy flag |
| 4 | Escort NPC follow logic | Tank stays on NPC; bot follows tank as usual | Yes — needs follow-extension |
| 5 | "Skip optional bosses?" policy | Minimum-run for LFD (target end boss only); full-clear if explicit master command | Yes — bot-policy flag |
| 6 | CC-target priority among multiple CC-able mobs | caster > healer > big-melee | Yes — per-encounter override |
| 7 | Bot disengage / wipe-recovery threshold | Vanilla/WotLK: bot fights to death; BC/Cata: retreat at 20% HP if no healer attention in 2s | Yes — per-bracket override |
| 8 | Quest pickup at instance entry | Auto-pickup all available quests | Yes — bot-policy flag |
| 9 | Re-queue cadence after vote-kick | Don't re-queue for 5 minutes (let user investigate) | Yes — bot-policy flag |

---

## Open issues — bot-side gaps flagged by research

The leveling-dungeon flow exposes capabilities the cata-altbot module doesn't have yet. These are tracked in `docs/dungeons/leveling.md` "Open issues" section but documented here for the audit trail:

| Issue | Affects | Module gap |
|---|---|---|
| Vehicle phase (Oculus drake fight) | WotLK 70-80 bracket | No drake-control framework; recommend blacklisting Oculus |
| Escort NPC follow logic | Halls of Stone, CoS, Old Hillsbrad, Black Morass | `AltbotFollow.cpp` needs `FollowNPCTarget(guid)` mode |
| Dungeon quest pickup/turn-in | All eras (most leveling dungeons have quests) | Out-of-scope for combat doc tier; needs `AltbotQuestHandler.cpp` (future module) |
| Heirloom auto-equip from master's bags | All eras | Out-of-scope; needs gear-management module |
| LFD queue management (bot self-queues) | All leveling content | Out-of-scope; needs `AltbotLFG.cpp` (future module) |
| Vote-kick handling | All LFD content | Out-of-scope; bot-policy decision |

---

## Suggested next research passes

When tuning becomes data-driven (i.e., bots are running leveling dungeons and we can measure outcomes):

1. **Per-bracket tank-emergency threshold** — currently bot-author defaults (25% / 30% / 35% / 40% by era). Telemetry from real runs will show whether spike-damage tolerance matches reality.
2. **Per-bracket healer mana-floor calibration** — BC bracket assumes 55% conserve / 30% crisis. Real mana-pool ratios at BC gear levels may differ from max-level Cata gear assumptions.
3. **CC-target priority validation** — the caster > healer > big-melee default isn't based on data. Per-dungeon outcome metrics could refine.
4. **Vehicle-phase + escort NPC support** — design and build the missing module pieces, then expand the leveling.md doc with concrete bot-side handling rather than UNCERTAIN markers.
5. **LFD private-server fidelity** — verify which 4.3.4 cores faithfully implement Call to Arms, cross-realm grouping, and the JP cap.

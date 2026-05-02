# Leveling Dungeons — Cross-Class Patterns

**Bot-relevant patterns for 5-man leveling content** on a 4.3.4 server. The Dungeon Finder pools every leveling dungeon accessible to the bot's level bracket, spanning vanilla → Burning Crusade → WotLK → Cata leveling. This doc is the reference for what changes across eras, per-role progression, and the LFD-specific considerations the bot must handle.

Per-dungeon detailed walkthroughs (Throne of the Tides model) stay in `docs/dungeons/{dungeon}/...`. This doc is the level above — the shape of leveling dungeons in general.

Strategy reconciled across 11 sources — see `docs/research/leveling-dungeons-guide-survey.md` for the audit trail.

---

## Era-by-era summary

```yaml
era_summary:
  vanilla:
    level_range_on_4_3_4: 1-58
    dungeons:
      - Ragefire Chasm
      - Wailing Caverns
      - Deadmines (revamped in 4.0.3)
      - Shadowfang Keep (revamped in 4.0.3)
      - Stockades
      - Blackfathom Deeps
      - Gnomeregan
      - Razorfen Kraul
      - Razorfen Downs
      - Scarlet Monastery (Graveyard / Library / Armory / Cathedral)
      - Uldaman
      - Maraudon
      - Sunken Temple
      - Zul'Farrak
      - Blackrock Depths
      - Lower Blackrock Spire
      - Stratholme (Live / UD)
      - Scholomance
      - Dire Maul (East / North / West)
    pull_discipline: loose
    cc_required: optional (exceptions: SM Cathedral, Scholo, Strat, Sunken Temple)
    kicks_required: rare
    tank_threat_demand: low — aggro sticks easily
    healer_mana_demand: low — small HP pools, single heal often tops tank
    signature_mechanics:
      - sprawling layouts (BRD, Sunken Temple, Strat, Dire Maul)
      - many optional bosses
      - patrols (avoid double-pulling)
      - gauntlets (Strat 45-min Baron timer, Scholo gate sequence)
      - fear from undead/voidwalker mobs

  burning_crusade:
    level_range_on_4_3_4: 58-68
    dungeons:
      - Hellfire Ramparts
      - Blood Furnace
      - Slave Pens
      - Underbog
      - Mana-Tombs
      - Auchenai Crypts
      - Sethekk Halls
      - Shadow Labs
      - Steamvault
      - Shattered Halls
      - Mechanar
      - Botanica
      - Arcatraz
      - Old Hillsbrad
      - The Black Morass
    pull_discipline: TIGHT — mandatory CC pulls become standard
    cc_required: MANDATORY on most pulls
    kicks_required: MANDATORY on dangerous casters (Shadow Lab MC priests, Shattered Halls)
    tank_threat_demand: single-target focus on caster opener; AoE threat weak (only Prot Pally has Consecrate)
    healer_mana_demand: CRITICAL — Five Second Rule strictly enforced; CC break = OOM
    signature_mechanics:
      - frontal cones (Shadow Lab Murmur)
      - fears (Black Morass demon adds)
      - gauntlets (Shattered Halls)
      - event-style instances (Black Morass, Old Hillsbrad — escort/wave defense)

  wotlk:
    level_range_on_4_3_4: 68-80
    dungeons:
      - Utgarde Keep
      - The Nexus
      - Azjol-Nerub
      - Ahn'kahet
      - Drak'Tharon Keep
      - Violet Hold
      - Gundrak
      - Halls of Stone
      - Halls of Lightning
      - The Oculus                       # vehicle phase — see open issues
      - Utgarde Pinnacle
      - Culling of Stratholme            # escort phase + chase sequence
    pull_discipline: loose again — streamlined linear layouts; AoE pulls of 4-8 standard
    cc_required: largely optional on normal
    kicks_required: light — a few caster mobs matter (HoL Storm Magus, Ahn'kahet casters)
    tank_threat_demand: full AoE threat tools (Thunder Clap, Swipe, DnD); easy hold
    healer_mana_demand: easier — Replenishment baseline, mp5 talents mature
    signature_mechanics:
      - vehicle phase (Oculus end boss)
      - escort phase (Halls of Stone Brann)
      - gauntlet (Utgarde Pinnacle Skadi harpoon, Halls of Lightning Loken trash)
      - chase/run sequence (Culling of Stratholme)
      - notable difficulty spikes (Loken, Skadi gauntlet)

  cata_leveling:
    level_range_on_4_3_4: 80-85
    dungeons:
      - Throne of the Tides
      - Blackrock Caverns
      - Vortex Pinnacle
      - Stonecore
      - Lost City of the Tol'vir
      - Halls of Origination
      - Grim Batol
      - Deadmines (revamped 85 heroic — also leveling content as level 85)
      - Shadowfang Keep (revamped 85 heroic)
    pull_discipline: TIGHT again — large pulls wipe
    cc_required: MANDATORY on most non-trivial pulls
    kicks_required: MANDATORY — uninterrupted casts can one-shot tank
    tank_threat_demand: active-mitigation groundwork (Shield Block, Holy Shield, Savage Defense)
    healer_mana_demand: CRITICAL — mana discipline returns; healer regen nerfed at Cata launch
    signature_mechanics:
      - dense ground AoE telegraphs
      - heavy knockbacks (Ozruk Ground Slam, Asaad Static Cling)
      - gauntlets (HoO Setesh Halls trash, Anraphet add waves)
      - frontal cones
      - paralyze/shatter combos
```

---

## Per-role progression by level bracket

```yaml
tank_progression:
  - bracket: 1-30 (vanilla low)
    behavior: |
      Few threat tools online. Aggro sticks because mob HP/damage is low.
      Bot can skip CC entirely. Defensives barely needed.
    threshold_overrides:
      tank_emergency_hp_pct: 25     # baseline 40 — lower because spike damage is rare
      defensive_cd_use: skip mostly

  - bracket: 30-58 (vanilla mid/high)
    behavior: |
      More threat abilities online (Shield Slam at 40, Avenger's Shield at 30,
      Maul ranks). Strat/Scholo/DM punish careless pulls — tank should mark CC
      targets and use LoS pulls on patrol-heavy areas.
    threshold_overrides:
      tank_emergency_hp_pct: 30
      defensive_cd_use: reactive only

  - bracket: 58-68 (BC)
    behavior: |
      Pulls have a caster + CC + LoS opener. Tank ranged-pulls casters with
      Shoot/Avenger's Shield/Death Grip/Throw, then pivots. AoE threat weak;
      single-target focus, let DPS/CC handle adds. Positioning matters
      (avoid frontal cones around tank).
    threshold_overrides:
      tank_emergency_hp_pct: 35
      defensive_cd_use: rotate minor; major reactive

  - bracket: 68-80 (WotLK)
    behavior: |
      AoE-pull comfortably; Thunder Clap / Swipe / DnD for instant pack threat.
      Damage spike tolerance high; defensives mostly slot for "oh no" moments
      (Skadi gauntlet, Loken Lightning Nova).
      Active mitigation NOT yet a button-press discipline.
    threshold_overrides:
      tank_emergency_hp_pct: 40     # approaching baseline
      defensive_cd_use: rotate minor; major for spike events

  - bracket: 80-85 (Cata leveling)
    behavior: |
      Active-mitigation groundwork: Shield Block on cooldown, Savage Defense
      procs, Holy Shield uptime. Pulls expect proper rotation. Cooldowns
      timed against telegraphed boss damage spikes. CC + kicks coordinated.
    threshold_overrides:
      tank_emergency_hp_pct: 40     # baseline
      defensive_cd_use: full active mitigation cycle (see docs/roles/tank.md)

healer_progression:
  - bracket: 1-30
    behavior: |
      Mana rarely an issue. HP pools small; one heal tops the tank.
      Bot can spam baseline heal without thinking.
    threshold_overrides:
      conserve_mode_pct: 20         # baseline 50
      crisis_mode_pct: 10           # baseline 25

  - bracket: 30-50
    behavior: |
      Mana starts mattering as encounters lengthen (BRD, ST). Multi-heal pulls
      need triage — keep tank above 50%, top DPS only when safe.
    threshold_overrides:
      conserve_mode_pct: 35
      crisis_mode_pct: 18

  - bracket: 60-70 (BC)
    behavior: |
      Mana management is THE dominant constraint. Five Second Rule enforced.
      CC break = healer death. Bot drinks between pulls; conserves cooldowns
      (Shadowfiend, Innervate, Lay on Hands) for spike windows.
    threshold_overrides:
      conserve_mode_pct: 55         # higher than baseline
      crisis_mode_pct: 30
      drink_floor_pct: 50           # drink earlier

  - bracket: 70-80 (WotLK)
    behavior: |
      Mana regen overhauled (Replenishment, mp5 baseline). Easier to sustain
      AoE healing. Bot can play more aggressively, reactive style.
    threshold_overrides:
      conserve_mode_pct: 45
      crisis_mode_pct: 25

  - bracket: 80-85 (Cata leveling)
    behavior: |
      Full mana discipline returns. Triage healing — cheap heal for casual
      damage, big heals only when needed. Bot must stop reflex-spamming
      Greater Healing Wave / Greater Heal. CC/interrupt failure = OOM.
    threshold_overrides:
      conserve_mode_pct: 50         # baseline
      crisis_mode_pct: 25           # baseline

melee_dps_progression:
  - bracket: 1-30
    behavior: minimal interrupt duty; behind-target mostly cosmetic
  - bracket: 30-50
    behavior: rear-arc starts mattering on bosses with cleave/parry
  - bracket: 60-70 (BC)
    behavior: |
      INTERRUPT ROTATION DUTY. CC discipline (don't AoE on Sap/Poly target;
      don't hit second mob with Whirlwind/Cleave). Bot needs target whitelisting.
  - bracket: 70-80 (WotLK)
    behavior: interrupt duty lighter; AoE freely on most pulls
  - bracket: 80-85 (Cata leveling)
    behavior: full max-level discipline (see docs/roles/melee-dps.md)

ranged_dps_progression:
  - bracket: 1-30
    behavior: just nuke; positioning forgiving
  - bracket: 30-50
    behavior: spread for splash mechanics
  - bracket: 60-70 (BC)
    behavior: |
      CC DUTY: Polymorph (Mage), Freezing Trap (Hunter), Banish demons (Lock),
      Hex (Shaman — Cata add), Hibernate beasts/dragonkin (Druid).
      Backup interrupter (Counterspell, Wind Shear, Silencing Shot).
  - bracket: 70-80 (WotLK)
    behavior: CC duty light; spread for AoE telegraphs; kick backup
  - bracket: 80-85 (Cata leveling)
    behavior: full max-level discipline (see docs/roles/ranged-dps.md)
```

---

## Common mechanics catalog (bot-action lookup)

```yaml
mechanic_catalog:
  - name: healer_or_interruptible_caster_mob
    seen_in: every era; >90% of leveling pulls have at least one castable spell
    bot_response: |
      Whitelist caster mob types per dungeon. Assign 1+ kicker. If no kick
      available in group, tank pulls into LoS to force movement.

  - name: aggro_pulling_caster
    seen_in: vanilla/BC trash (RFK casters, Mana-Tombs ethereals)
    bot_response: |
      Tank uses ranged opener (Shoot, Avenger's Shield, Death Grip, Throw).
      Melee waits a global before engaging.

  - name: frontal_cone
    seen_in:
      - Shadow Lab Murmur (BC)
      - Halls of Origination Earthrager Ptah (Cata)
      - Throne of the Tides Honor Guard Arc Slash + Erunak Magma Splash + Behemoth Blight Spray (Cata)
    bot_response: |
      Bot positions behind boss for melee. Tank faces away from group.
      See docs/roles/melee-dps.md and docs/roles/tank.md cone rules.

  - name: ground_aoe_pool
    seen_in:
      - Stonecore Slabhide crystals (Cata)
      - Lost City Lockmaw poison (Cata)
      - HoO sand traps (Cata)
      - Loken Lightning Nova (WotLK)
      - Anub'arak Shadow Strike (WotLK)
    bot_response: |
      Move out on telegraph. Healer pre-cast HoT before stepping in if forced
      to clip the edge (rare).

  - name: fear_or_charm
    seen_in:
      - Vanilla SFK / Scholo (priest Mind Control)
      - BC Shadow Lab MC
      - WotLK Drak'Tharon basilisks
      - Cata Lost City Augh
    bot_response: |
      Dispels: Tremor Totem (shaman), Fear Ward (priest), Mass Dispel (priest).
      Bot maintains tracking of who's charmed and CCs them if a class is available.

  - name: pushback_or_knockback
    seen_in:
      - BC Shattered Halls Soulgrinder
      - WotLK Skadi (harpoon-toss reversed)
      - Cata Ozruk Ground Slam, Asaad Static Cling tornadoes
      - ToTT Ozumat tentacle slam
    bot_response: |
      Predict knockback path; back-step to wall when low HP. Healer pre-HoTs.

  - name: vehicle_phase
    seen_in: Oculus end boss (drake mounting) — WotLK
    bot_response: |
      UNCERTAIN — no leveling-bot framework I'm aware of handles drake controls
      cleanly. Mod-playerbots historically excluded Oculus.
      RECOMMENDED: blacklist Oculus from LFD queue OR exclude bot from drake phase
      (let players handle it). See open_issues block below.

  - name: escort_phase
    seen_in:
      - WotLK Halls of Stone Brann
      - WotLK Culling of Stratholme Arthas escort
      - BC Old Hillsbrad Thrall
      - BC Black Morass Medivh defense
    bot_response: |
      Tank stays on the escort NPC. Bot prioritizes adds over boss.
      UNCERTAIN — no clean "follow this NPC" leash framework documented in the
      module yet. AltbotFollow.cpp may need an extension to follow NPC targets.

  - name: gauntlet_section
    seen_in:
      - Strat 45-min Baron timer
      - Scholo Gandling
      - BRD Princess area
      - Halls of Origination Anraphet adds
      - Utgarde Pinnacle Skadi gauntlet
      - Shattered Halls execution event
    bot_response: |
      Sustained AoE tank threat + DPS. Healer mana-conservative; no full-mana
      resets between waves. Bot saves cooldowns for the gauntlet, not the boss.

  - name: add_spawning_boss
    seen_in:
      - Throne of Tides Lady Naz'jar
      - BFD Aku'mai
      - Maraudon Princess
      - HoO Anraphet
      - Strat Baron Rivendare
    bot_response: |
      Off-tank or DPS focus-fire adds. Main tank holds boss.
```

---

## LFD specifics (4.3.4)

```yaml
lfd_4_3_4:
  cross_realm_groups: yes (battlegroup-wide for normal + heroic)
  bracket_gating: |
    A bot CANNOT queue outside its level/expansion bracket. Random LFD pools
    dungeons within the bot's bracket only.

  random_dungeon_bonus:
    sub_85_random: bonus XP + gold (no Justice Points until 85)
    cata_normal_at_85: 140 Justice Points + bonus XP
    cata_heroic_at_85: 150 Valor Points (first 7/wk), then 70 Justice each

  vote_kick:
    cooldown_after_abuse: 2 hours
    premade_vote_threshold: 4 of 5
    pug_vote_threshold: 3 of 5

  deserter_debuff: 30-minute LFD lockout if bot leaves before final boss

  role_fill: |
    LFD fills missing role. Bot module's primary use case is the bot filling
    typically tank or healer queues to shorten pop times for human players.
    Call to Arms gives extra rewards to underrepresented role.

  re_queue_cooldown: |
    None for normal completion. Daily/weekly RDF bonus resets are separate.

  uncertain_on_private_servers: |
    4.3.4 private servers vary on faithful implementation of:
    - Call to Arms reward system
    - Cross-realm grouping
    - 7-Cata-normals-per-week JP cap
    Verify against the specific core (TC 4.3.4 / Cata-classic).
```

---

## Leveling-only considerations (don't apply at 85)

```yaml
leveling_only:
  gear_churn:
    rule: |
      Quest rewards and dungeon drops every level. Bot should auto-equip
      green/blue upgrades by ilvl + spec-stat priority.
    reforging_unlock: level 81 — irrelevant for most leveling

  heirlooms:
    xp_bonus_full_set: ~45%
    pieces_to_equip: shoulder + chest + ring (scale to 80) + helm + legs + cloak (scale to 85)
    bot_action: |
      If heirlooms are in bag/bank, auto-equip. If accessible via guild bank
      or master's storage, request from master.

  talent_progression:
    rule: |
      Talent points unlock 1/level from level 10 onward (Cata trees).
      Some signature class abilities only unlock at specific levels — bot
      rotation logic must gate by `pPlayer->getLevel()`.
    examples:
      - Riptide (Resto Shaman): level 30 baseline (Cata change from WotLK level 60 talent)
      - Death and Decay (Death Knight): level 12
      - Mind Sear (Priest): level 28
      - Mortal Strike (Arms Warrior): level 40
      - Holy Shock (Holy Paladin): level 10 (with Holy talent baseline)
      - Penance (Disc Priest): level 10 (with Disc talent baseline)
      - Healing Stream Totem (Shaman): level 20
      - Lifebloom (Druid): level 64
      - Ice Lance (Mage): level 66 baseline

  dungeon_quests:
    rule: |
      Most leveling dungeons have ≥3 associated quests (entrance NPC + interior).
      Big XP spikes at turn-in.
    bot_action: |
      Auto-pick up quests at instance entrance. Auto-turn-in at completion.
      RECOMMENDED: AltbotMgr extension to handle quest pickup/turn-in
      (out of scope for combat doc tier).

  satchel_bonus:
    rule: |
      Random Cata sub-85 dungeons award a Satchel of Helpful Goods
      (level-appropriate green + gold + flask/potion). WotLK/BC random satchels
      exist too.
    bot_action: claim satchel from LFD UI on first daily random completion

  quest_vs_dungeon_strategy:
    consensus: |
      Questing is faster XP/hour solo; dungeon-while-queued hybrid is the
      most efficient flow. Bot module's strategy:
        - QueueRandomDungeon() at instance entry
        - Quest in adjacent zone until pop
        - Enter dungeon when queue completes
    bot_value_add: |
      The bot's primary value is "always-available group" rather than "raw
      XP/hour winner."
```

---

## Open issues — bot-side gaps

These are scenarios the leveling-dungeon flow exposes that the cata-altbot module doesn't yet handle:

| Issue | Affects | Recommended approach |
|---|---|---|
| Vehicle phase (Oculus drake fight) | WotLK 70-80 bracket | Blacklist Oculus from bot LFD queue OR detect vehicle entry and pause bot AI until vehicle exit |
| Escort NPC follow logic | Halls of Stone, CoS, Old Hillsbrad, Black Morass | Extend `AltbotFollow.cpp` with `FollowNPCTarget(guid)` mode; default tank stays on escort, party follows |
| Dungeon quest pickup/turn-in | All eras (most leveling dungeons have associated quests) | Out-of-scope for `AltbotCombat.cpp`; needs `AltbotQuestHandler.cpp` (future module) |
| Heirloom auto-equip from master's bags | All eras | Out-of-scope; needs gear-management module |
| LFD queue management (bot self-queues) | All leveling content | Out-of-scope; needs `AltbotLFG.cpp` (future module) |
| Vote-kick handling (bot detects vote, accepts deserter or doesn't) | All LFD content | Out-of-scope; bot-policy decision |

These are flagged as known gaps so the doc's coverage of "what the bot needs to do during leveling dungeons" is honest about which capabilities require additional module work beyond `AltbotCombat.cpp`.

---

## Author-judgment items (not pinned by guides)

| # | Item | Default decision | Tunable? |
|---|---|---|---|
| 1 | CC threshold by group composition | If 1+ CC class in group, mark CC target on first dangerous-caster pack of every dungeon | yes — per-encounter override |
| 2 | Interrupt-rotation assignment with 2+ kickers | Primary = melee with shortest CD; secondary = ranged | yes — per-encounter override |
| 3 | Vehicle-phase handling (Oculus) | Blacklist (recommend) | yes — bot-policy flag |
| 4 | Escort NPC follow logic | Tank stays on NPC; bot follows tank as usual | yes — needs follow-extension |
| 5 | "Skip optional bosses?" policy | Minimum-run for LFD (target end boss only); full-clear if explicit master command | yes — bot-policy flag |
| 6 | CC-target priority among multiple CC-able mobs | caster > healer > big-melee | yes — per-encounter override |
| 7 | Bot disengage / wipe-recovery threshold | Vanilla/WotLK: bot fights to death; BC/Cata: retreat at 20% HP if no healer attention in 2s | yes — per-bracket override |
| 8 | Quest pickup at instance entry | Auto-pickup all available quests | yes — bot-policy flag |
| 9 | Re-queue cadence after vote-kick | Don't re-queue for 5 minutes (let user investigate) | yes — bot-policy flag |

---

## How this doc layers with the rest of the tree

```
docs/dungeons/leveling.md (this doc)
    ↑ general leveling-dungeon patterns; era-by-era summary

docs/dungeons/{specific-dungeon}/{encounter,role}.md
    ↑ per-dungeon detailed walkthroughs (Throne of the Tides model)
    referenced when bot enters that specific dungeon

docs/roles/{role}.md
    ↑ class-agnostic role patterns (apply at all levels)
    leveling.md augments with per-bracket threshold overrides

docs/specs/{spec}.md
    ↑ per-spec class data
    has its own "Leveling rotation" section (see docs/specs/resto-shaman.md
    for the first one) with spell-unlock-bracketed rotations
```

The bot's encounter handling at any leveling dungeon resolves layers in this order:

1. **Active spec doc** — base rotation, CDs, mana model
2. **`docs/dungeons/leveling.md`** — per-bracket threshold overrides for the bot's current level (this doc's `*_progression` blocks)
3. **`docs/roles/{role}.md`** — role-level patterns (triage, dispel taxonomy, etc.)
4. **`docs/dungeons/{specific-dungeon}/{role}.md`** — if a specific-dungeon doc exists for the bot's current dungeon, its overrides take final precedence

Most leveling dungeons WILL NOT have a specific-dungeon doc. The bot operates from layers 1-3 only and relies on the generic mechanic catalog above.

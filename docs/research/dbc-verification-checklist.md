# DBC Verification Checklist

**For: cata-altbot doc UNVERIFIED items.** Run this checklist when you're at the computer with the TrinityCore 4.3.4 source tree and DBC files available.

**Scope** (as of 2026-05-02): the project now has **17 spec docs** + **2 dungeon bundles**, each with its own UNVERIFIED items table. This file is no longer a single flat list — it points at the per-spec/per-dungeon tables and provides worked-example detail for the original 9 Resto Shaman items as a starter pattern.

---

## Per-spec UNVERIFIED tables — read these first

Every `docs/specs/<spec>.md` has an `## UNVERIFIED items` table at the bottom. Each row has the claim, current encoded value, and the DBC file to check. Open the spec doc, work through the table, update the table inline, remove the row when resolved.

### Healer specs (5)
- ~~`docs/specs/resto-shaman.md`~~ — ✅ all 9 items resolved 2026-05-02 (worked example below)
- `docs/specs/holy-paladin.md` — ~14 items
- `docs/specs/holy-priest.md` — ~15 items (Chakra mechanics)
- `docs/specs/disc-priest.md` — ~18 items (Atonement-Smite specifics)
- `docs/specs/resto-druid.md` — ~20 items (HoT-rolling tier mechanics)

### Ranged DPS specs (6)
- `docs/specs/frost-mage.md` — ~20 items
- `docs/specs/affliction-warlock.md` — ~26 items (Soul Swap + DoT mechanics)
- `docs/specs/marksmanship-hunter.md` — ~25 items (FOCUS-only since 4.0.1)
- `docs/specs/shadow-priest.md` — ~20 items (Shadow Orbs)
- `docs/specs/balance-druid.md` — ~20 items (Eclipse bar)
- `docs/specs/elemental-shaman.md` — ~20 items (Lightning Shield + Fulmination)

### Melee DPS specs (6)
- `docs/specs/combat-rogue.md` — ~20 items (Bandit's Guile + Blade Flurry)
- `docs/specs/fury-warrior.md` — ~24 items (Titan's Grip + Inner Rage)
- `docs/specs/retribution-paladin.md` — 28 items (mana + Holy Power)
- `docs/specs/frost-death-knight.md` — 35 items (Runes + RP + diseases — first DK doc, full resource model new)
- `docs/specs/enhancement-shaman.md` — 34 items (Maelstrom Weapon proc rates + weapon imbue PPMs)
- `docs/specs/feral-cat-druid.md` — 41 items (highest count — bleed mechanics + Cata 4.0 rework + Cat-form glyphs)

**Approximate total: ~370 UNVERIFIED items across all specs.** Many are duplicates or trivial confirmations (level gates, durations); a smaller subset (proc rates, mastery formulas, talent-rank scaling) is high-impact.

### Dungeon bundles — Phase 0 sweep complete 2026-05-10
- All 12 `docs/dungeons/*/encounter.md` files audited via `tools/dbc-extract.py --verify-encounters`.
- Output artifact: **`docs/data/dungeon-mechanic-db.csv`** (399 rows) — canonical mechanic table consumed by Phase 2 codegen.
- Final disposition (per-dungeon breakdown printed by the verifier):

  | Status | Count | Meaning |
  |---|---|---|
  | `verified` | 126 | DBC name matches AND TC script references the ID |
  | `unbound` | 109 | DBC name matches; TC script doesn't reference (bound via SQL `spell_script_names` / `creature_template_addon`, or boss script is a stub) |
  | `placeholder` | 134 | doc spell_id non-numeric (`UNVERIFIED` literal) — awaiting in-game lookup |
  | `skipped` | 18 | doc had a wrong numeric ID; corrected to `SKIPPED-AWAITING-INGAME` marker so Phase 2 codegen drops it cleanly |
  | `name-mismatch`/`not-found` | 0 | all corrected or reclassified to `skipped` |

- **Gating note for Phase 2 codegen:** the original plan said "every `avoidable` row must be `status==verified`." That's too strict — many TC bosses are empty stubs and even legitimate Cata mechanics show as `unbound`. Phase 2 should accept `verified` OR `unbound` (DBC-name-match is the authoritative check). It must reject `name-mismatch`, `not-found`, `skipped`, `placeholder`.

#### Per-dungeon corrections applied 2026-05-10

Numeric ID swaps based on DBC name match (and TC source citation where available):

| Dungeon | Mechanic | Old ID | New ID | Source |
|---|---|---|---|---|
| blackrock-caverns | Twilight Evolution | 75571 | **75732** | DBC: 75571=Wounding Strike, 75732=Twilight Evolution |
| blackrock-caverns | Crepuscular Veil | 75476 | **76189** | DBC: 75476=Dusk Shroud, 76189/76190=Crepuscular Veil |
| blackrock-caverns | Shadow Prison | 75763 | **76686** | DBC: 75763=Umbral Mending, 76686/76687=Shadow Prison |
| hour-of-twilight | Throw Knife | 103587 | **103597** | DBC: 103587=Silenced (single-digit typo) |
| hour-of-twilight | Righteous Shear | 103161 | **103149** | DBC: 103149/103151="Rigtheous Shear" [sic] |
| hour-of-twilight | Twilight Blast → Twilight Bolt | 103777 | **103777** (rename only) | DBC name is "Twilight Bolt"; doc renamed |
| end-time | Molten Mace → Molten Axe | 101836 | **101836** (rename only) | DBC name is "Molten Axe"; doc renamed |
| zul-aman | Flame Breath (Jan'alai) | 43124 | **43140** | TC `boss_janalai.cpp:47` confirms |
| zul-aman | Fire Bomb (Jan'alai) | 43137 | **42621** | TC `boss_janalai.cpp:52` confirms |
| zul-aman | Lynx Rush (Daakara) | 43151 | **43152** | DBC: 43151=Necrolord placeholder, 43152/43153=Lynx Rush |
| zul-aman | Static Charge → Static Disruption | 43622 | **(skipped)** | DBC name is "Static Disruption"; doc renamed but ID flagged for in-game confirm |

Marked `SKIPPED-AWAITING-INGAME` (DBC mismatch + TC source has empty stub):

- blackrock-caverns: Cinderbreath, Searing Lava (central plume), Call Bonecrushers
- zul-aman (Nalorakk): Brutal Swipe, Surge, Lacerating Slash, Mojo Volley, Mangle
- zul-aman (Akil'zon): Soaring Eagles
- zul-aman (Jan'alai): Hatcher's Hatch
- zul-aman (Halazzi): Frenzy, Flame Shock, Lightning Totem
- zul-aman (Hex Lord): Soul Drain
- zul-aman (Daakara): Charge (bear), Bear Claw, Cyclone (eagle), Static Disruption (eagle)

These are blocked on in-game `.lookup spell <name>` confirmation. The TC fork's ZA boss scripts are all empty stubs (`enum Spells {};`), so the source-of-truth for ZA mechanic IDs is the running server's spell DB, not the C++ source.

#### Watch-list status (from earlier checklist)

- ✅ **Spell 75763 BRC misuses** — both encounter.md entries previously using 75763 ("Umbral Mending" per DBC) are corrected: Cinderbreath → SKIPPED, Shadow Prison → 76686. The legitimate use in `docs/dungeons/grim-batol/encounter.md:805` (an actual Umbral Mending heal) remains verified.
- ✅ **Crepuscular Veil 75476** — corrected to 76189. Dispel-blacklist entry now points at the right debuff.

---

## Detailed worked example — the original 9 Resto Shaman items

The sections below are kept as-is from the original checklist. They serve as a worked example for the per-row format you'll use in the other spec docs: Spell ID + what DBC field to read + where the doc currently says it + what to update on confirm/disconfirm.

After completing these 9, follow the same pattern for the per-spec tables linked above.

---

## What "DBC verification" means

The WoW server stores spell data in binary `*.dbc` files extracted from the WoW client. `Spell.dbc` is the authoritative source for every spell's actual cooldown, cast time, mana cost, required level, etc. — your server uses these values in-game regardless of what Wowhead or any guide says.

The 9 items below are values where:
- public guides disagreed, OR
- the value couldn't be resolved from Wowhead alone, OR
- the question is "is this a baseline ability or a talent?" — which requires checking `Talent.dbc` in addition to `Spell.dbc`.

---

## Where the DBC files live (likely paths)

Check these locations on the server machine — exact path depends on how you set up TC:

```
<TC server build dir>/dbc/Spell.dbc           # most common
<TC server build dir>/dbc/Talent.dbc
<TC server build dir>/dbc/TalentTab.dbc
~/azerothcore-or-trinitycore/data/dbc/       # alternate location for some builds
<wow client install>/Data/<locale>/locale-<locale>.MPQ  # if you need to re-extract
```

If you don't see them, run TC's `mapextractor` or `dbcextractor` against your WoW 4.3.4 client to regenerate.

---

## Methodology — Spell.dbc is normalized into sub-tables in Cata 4.x

**Important** — Wrath-era guides describe `RecoveryTime`, `BaseLevel`, `ManaCost`, `Duration` as direct columns of `Spell.dbc`. **That is no longer true in Cata 4.x.** The spell row in `Spell.dbc` holds index references that point at separate sub-tables:

| Sub-table         | Spell.dbc index field      | Holds                                              |
|-------------------|----------------------------|----------------------------------------------------|
| `SpellCooldowns`  | `SpellCooldownsId`         | `RecoveryTime`, `CategoryRecoveryTime`             |
| `SpellLevels`     | `SpellLevelsId`            | `BaseLevel`, `MaxLevel`, `SpellLevel`              |
| `SpellPower`      | `SpellPowerId`             | `ManaCost`, `ManaCostPct`, `ManaCostPerLevel`      |
| `SpellDuration`   | `DurationIndex`            | `Duration_ms`, `DurationPerLevel`, `MaxDuration_ms`|

To resolve a single spell's full picture you must export all 5 DBCs to CSV and join them by index. This is what `tools/dbc-extract.py` does.

### Working extraction tool — `tools/dbc-extract.py`

The script reads the 5 CSVs exported from WDBXEditor, joins them by ID, prints answers to stdout, and writes a flat `docs/data/spell-ref.csv` with resolved per-spell values. Currently hardcoded with the original 9 Resto Shaman spell IDs in `TARGET_SPELLS`; extend that dict to verify additional spells.

**Workflow:**
1. Open `<TC server build>/dbc/enUS/Spell.dbc` in [WDBXEditor](https://github.com/WowDevTools/WDBXEditor) (Windows GUI tool)
2. Export each table to CSV:
   - `Spell.csv`
   - `SpellCooldowns.csv`
   - `SpellLevels.csv`
   - `SpellPower.csv`
   - `SpellDuration.csv`
3. Edit `DBC_DIR` in `tools/dbc-extract.py` to point at the export folder
4. Add target spell IDs to `TARGET_SPELLS` (key: spell ID, value: human name for log output)
5. Run `python tools/dbc-extract.py` — prints verification answers; appends to `docs/data/spell-ref.csv`

The output CSV is the durable artifact — refer to it from spec docs when removing UNVERIFIED markers.

### Sub-table field names in WDBXEditor exports

WDBXEditor exports sub-table columns with generic `Field01/Field02/...` names. Mapping:

```
SpellLevels:    Field01=BaseLevel    Field02=MaxLevel    Field03=SpellLevel
SpellDuration:  Field01=Duration_ms  Field02=DurationPerLevel  Field03=MaxDuration_ms
SpellPower:     Field01=ManaCost     Field02=ManaCostPct  Field03=ManaCostPerLevel  Field04=ManaPerSecond
SpellCooldowns: named correctly      (RecoveryTime, CategoryRecoveryTime)
```

The script bakes this mapping in; if the WDBXEditor schema changes, update the field references in `dbc-extract.py`.

### Fallback — in-game GM lookup

Fast for one-off single-spell questions:

```
.lookup spell <spell_name_or_id>
.lookup spell shock                  # finds all spells with "shock" in name
```

The server resolves the cross-table joins internally and reports loaded values. Slower than the script for batch verification but no setup required.

### For talent-vs-baseline questions

Cross-check `BaseLevel` and `SpellLevel` from `SpellLevels` (already in the script's output). If both are 0, also query `Talent.dbc` directly to confirm whether the spell ID appears in any talent row's `SpellRank` field. The script doesn't do this; for the original 9 Resto items it was resolved by inspection — note the pattern in `phase 3` commit if extending.

---

## The 9 items to verify ✅ RESOLVED 2026-05-02

The 9 detailed items below have been **resolved** via the dbc-extract.py workflow. Verified values are in `docs/data/spell-ref.csv` and `docs/specs/resto-shaman.md` has been updated. Sections kept as a worked example showing the per-row format you'll use when verifying additional specs.

### 1. Wind Shear cooldown

- **Spell ID**: 57994
- **What to check**: `Spell.dbc` → `RecoveryTime` (in milliseconds)
- **Doc currently says**: 6000 ms (6 sec) with comment "UNVERIFIED — Cata-Classic re-tuning shows 15s; original 4.3.4 widely cited as 6s"
- **Where**: `docs/specs/resto-shaman.md` Spell Catalog → `WIND_SHEAR.cooldown_ms`
- **What to do with result**:
  - If 6000: remove the UNVERIFIED comment, leave value
  - If 15000: change `cooldown_ms: 6000` → `cooldown_ms: 15000`, remove UNVERIFIED comment
- **Also update**: `docs/specs/resto-shaman.md` UNVERIFIED items table — remove the `WIND_SHEAR.cooldown_ms` row

### 2. Water Shield mana-per-orb

- **Spell ID**: 52127
- **What to check**: `Spell.dbc` effect rows (Effect_1, Effect_2, Effect_3) — looking for the mana-restore effect's `EffectBasePoints` (flat amount) or whether it scales as a % of base mana
- **Doc currently says**: UNCERTAIN — flagged "flat scaled value, not % base mana" but not pinned down
- **Where**: `docs/specs/resto-shaman.md` Spell Catalog → `WATER_SHIELD.notes`
- **What to do with result**: add the verified value (e.g., "758 mana per orb consumed at level 85" or whatever DBC says) to the notes; remove UNVERIFIED markers
- **Also update**: UNVERIFIED items table — remove `WATER_SHIELD.mana_per_orb` row

### 3. Resurgence per-spell mana return

- **Spell ID**: 101033 (the talent passive aura)
- **What to check**: `Spell.dbc` row 101033 effects + `spell_proc_event` table in world DB if available
- **Question**: how much mana is returned per crit-heal, per spell?
  - Healing Wave / GHW: ?
  - Healing Surge / Riptide initial: ?
  - Chain Heal: ?
- **Doc currently says**: UNCERTAIN; modern published values: HW/GHW 0.80%, HSurge/Riptide 0.48%, Chain Heal 0.20% of max mana — but flagged as possibly different in original 4.3.4
- **Where**: `docs/specs/resto-shaman.md` Proc Reactions → `RESURGENCE` block
- **What to do**: replace the modern values with whatever your DBC actually contains; remove UNVERIFIED note

### 4. Healing Stream Totem duration

- **Spell ID**: 5394 (totem cast) — also check buff aura 52042
- **What to check**: `Spell.dbc` → `Duration` (DurationIndex maps to a duration value in `SpellDuration.dbc`)
- **Doc currently says**: 60000 ms (60 sec), flagged "Cata totems went to 60s but exact value should be DBC-confirmed"
- **Where**: `docs/specs/resto-shaman.md` Spell Catalog → `HEALING_STREAM_TOTEM.duration_ms`
- **What to do**: confirm 60000 or update with actual value

### 5. Spirit Link Totem — baseline ability or Restoration talent?  ⚠️ HIGHEST IMPACT

- **Spell ID**: 98008
- **What to check**:
  1. `Talent.dbc` — does any row reference spell 98008 as `SpellRank_1` (or similar)?
  2. `Spell.dbc` row 98008 → `BaseLevel` / `SpellLevel`
- **Question**: Is SLT a Resto-tree TIER-6 talent (reachable around character level 35) or a baseline level-85 ability granted by Restoration spec?
- **Doc currently says**: research agent claimed tier-6 talent (~level 35); author memory said baseline level-85
- **Why it matters**: this changes whether the bot has SLT in its rotation from level 35-onward (talent) or only at 85 (baseline)
- **Where**: `docs/specs/resto-shaman.md` — multiple places:
  - Spell Catalog: `SPIRIT_LINK_TOTEM` entry
  - Rotation Tier 4 (currently treats SLT as a max-level cooldown)
  - Leveling rotation Section: bracket 35-44 mentions UNCERTAIN
  - UNVERIFIED items table
- **What to do with result**:
  - If TALENT (tier-6, level ~35): update Leveling rotation to include SLT in the bracket-35+ rotation; update note in UNVERIFIED table; update talent path in resto-shaman.md to flag SLT as the tier-6 capstone
  - If BASELINE LEVEL 85: confirm current treatment is correct, just remove UNVERIFIED markers

### 6. Mana Tide Totem — baseline ability or Restoration talent?

- **Spell ID**: 16190
- **What to check**: same approach as SLT — query `Talent.dbc` for spell 16190, then `Spell.dbc` for level data
- **Doc currently says**: research agent claimed tier-5 talent (~level 30); author memory said baseline
- **Where**: `docs/specs/resto-shaman.md` Spell Catalog + Cooldown Profile + Leveling rotation bracket 30-39
- **What to do**: same pattern as SLT

### 7. Nature's Swiftness — baseline ability or Restoration talent?

- **Spell ID**: 16188
- **What to check**: same approach
- **Doc currently says**: research agent claimed tier-3 talent (~level 19-20); author memory said baseline
- **Where**: `docs/specs/resto-shaman.md` Spell Catalog + Cooldown Profile + Leveling rotation bracket 20-27
- **What to do**: same pattern

### 8. Greater Healing Wave required level

- **Spell ID**: 77472
- **What to check**: `Spell.dbc` → `BaseLevel` (or `SpellLevel`)
- **Doc currently says**: 68 (Wowhead Cata Classic value), flagged "some legacy guides say 60"
- **Where**: `docs/specs/resto-shaman.md` Leveling rotation Spell Unlock Table
- **What to do with result**:
  - If 68: confirm; remove UNVERIFIED; the bracket 68-79 entry is correct
  - If 60: update spell unlock table to 60; rebracket the leveling rotation accordingly (the 60-67 bracket would gain GHW)

### 9. Earthquake required level

- **Spell ID**: 61882
- **What to check**: `Spell.dbc` → `BaseLevel`
- **Doc currently says**: UNCERTAIN — Wowhead tooltip parser failed
- **Where**: `docs/specs/resto-shaman.md` Spell Catalog → `EARTHQUAKE` and Leveling rotation Spell Unlock Table
- **What to do**: add the verified level; remove UNVERIFIED

---

## After verification — commit results back

For each item resolved:

1. Edit the spec doc (`docs/specs/resto-shaman.md`) per the "Where" + "What to do" notes above.
2. Remove the row from the UNVERIFIED items table at the bottom of the spec doc.
3. Add a brief correction note to `docs/research/resto-shaman-guide-survey.md` (similar to the Telluric Currents correction note already there) explaining: what was UNVERIFIED, what the DBC said, and which docs were updated.
4. Delete the resolved item's section from THIS file.
5. When all 9 are done, delete this entire file or rename it to `dbc-verification-checklist-completed-YYYY-MM-DD.md` for archival.

---

## Bonus DBC items (low priority)

Smaller items that would also benefit from verification but aren't blocking anything. Verify opportunistically while in the DBC for the main work.

- Earthliving Weapon proc chance % (currently 20% from Wowhead — confirm)
- Tidal Waves buff bonus values (currently -30% cast time / +30% crit — confirm)

---

## Suggested resolution order

If you don't have time to do everything in one session, prioritize by runtime-correctness impact:

1. **BRC spell ID collisions and dispel-blacklist** (75476, 75763) — runtime-correctness; bot will mis-behave if wrong.
2. ~~Resto Shaman 9 items~~ — ✅ resolved 2026-05-02.
3. **First-of-class mechanics** for specs that introduce a new resource model:
   - Frost DK Runes / Runic Power / disease IDs (first DK doc — pattern for Blood tank later)
   - Feral Cat per-target Combo Points / bleed pandemic windows / glyphs (bleed mechanics had Cata 4.0 rework)
   - Enh Shaman Maelstrom Weapon proc rate / Mental Quickness AP→SP %
4. **Mastery formulas** across all 17 specs (~17 quick lookups; informs reforge logic)
5. **Glyph effects** across all specs (low individual impact; high count)
6. **Level gates** for talented vs trainer abilities (mostly trivial confirmations)

---

## TC-fork code-blockers (also pendable this trip)

These are not DBC items but live in the same TC-fork session because they need the source tree.

### ✅ Resolved 2026-05-02 (phase 3 commit)

- ~~`RBAC_PERM_COMMAND_GM`~~ — confirmed at `server-core/src/server/game/Accounts/RBAC.h:279`, value 371. Name correct as written.
- ~~`_legacyConnectionModeEnabled`~~ — private member of `WorldSession` (`WorldSession.h:1428`); `AltbotLogin` is a member function so it has full access. No fix needed.
- ~~3 talent-aware-combat design questions~~ — all answered. See `docs/research/talent-aware-combat-design.md`. Phase 3 work can begin.

### ✅ Resolved 2026-05-02 (source tree search)

- **PlayerScript hook surface** — Searched `server-core/src/server/game/Scripting/ScriptMgr.h:616`.
  - `OnLootRoll` ❌ does not exist — no loot-roll hook in CPP TC.
  - `OnQuestAccept` ❌ does not exist — use `OnQuestStatusChange(Player*, uint32)` which fires on any status change; filter by `QUEST_STATUS_REWARDED` for reward events.
  - `OnQuestReward` ❌ does not exist — same workaround as above.
  - `OnSpellCast(Player*, Spell*, bool)` ✅ exists — use for mount detection. Alternatively poll `player->IsMounted()` in Update().
  - `OnAuraApply` ❌ does not exist — use `OnSpellCast` or poll auras each tick.
  - Full available hook list: OnPVPKill, OnCreatureKill, OnPlayerKilledByCreature, OnLevelChanged, OnFreeTalentPointsChanged, OnTalentsReset, OnMoneyChanged, OnMoneyLimit, OnGiveXP, OnReputationChange, OnDuelRequest, OnDuelStart, OnDuelEnd, OnChat (×5 overloads), OnClearEmote, OnTextEmote, **OnSpellCast**, OnLogin, OnLogout, OnCreate, OnDelete, OnFailedDelete, OnSave, OnBindToInstance, OnUpdateZone, OnMapChanged, **OnQuestStatusChange**, OnPlayerRepop.

- **`ServerScript::OnPacketReceive`** ✅ confirmed at `ScriptMgr.h:242`:
  ```cpp
  virtual void OnPacketReceive(WorldSession* /*session*/, WorldPacket& /*packet*/) { }
  ```
  Subclass `ServerScript`, override `OnPacketReceive`, filter by opcode (`CMSG_MESSAGECHAT`) to intercept addon-channel whispers. ScriptMgr callback confirmed at `ScriptMgr.h:893`.

- **`Group::GetTargetIcons()`** ❌ no such method — member is public direct access:
  ```cpp
  // Group.h:424
  ObjectGuid m_targetIcons[TARGETICONCOUNT];
  ```
  Access as `group->m_targetIcons[i]`. Skull = index 7 (`RAID_TARGET_SKULL = 7`).

- **`Player::LearnTalent` signature** ✅ confirmed at `Player.h:1590`:
  ```cpp
  bool LearnTalent(uint32 talentId, uint32 talentRank);
  ```
  `talentId` is the `Talent.dbc` row ID; `talentRank` is 0-based rank index.
  Talent/spec query API also confirmed: `GetActiveSpec()` (Player.h:1580) + `GetPrimaryTalentTree(uint8 spec)` (Player.h:1578).

- ✅ **Addon-message prefix max length + per-message size cap** — resolved 2026-05-03 via `ChatHandler.cpp::HandleAddonMessagechatOpcode`:
  - **Message**: 9-bit length field → **511 chars max**, no secondary length validation in source.
  - **Prefix**: 5-bit field but server enforces `prefix.length() > 16` → **16 chars max**.
  - Design Phase 4 chunking around 511-char message and 16-char prefix limits (not 255).

---

## Talent tree IDs hard-coded in `AltbotStrategyFactory.cpp`

Added 2026-05-03 alongside the warlock / mage / hunter strategies. These are factory-dispatch constants, not spell data, but follow the same verify-via-WDBXEditor pattern that confirmed the shaman tree IDs (261/262/263).

| Constant | Class | Tree | Encoded value | DBC field |
|---|---|---|---|---|
| `WARLOCK_TREE_AFFLICTION` | Warlock | Affliction | ~~302~~ **871** ✅ | verified 2026-05-03 |
| `WARLOCK_TREE_DEMONOLOGY` | Warlock | Demonology | ~~303~~ **867** ✅ | verified 2026-05-03 |
| `WARLOCK_TREE_DESTRUCTION` | Warlock | Destruction | ~~301~~ **865** ✅ | verified 2026-05-03 |
| `MAGE_TREE_ARCANE` | Mage | Arcane | ~~81~~ **799** ✅ | verified 2026-05-03 |
| `MAGE_TREE_FIRE` | Mage | Fire | ~~41~~ **851** ✅ | verified 2026-05-03 |
| `MAGE_TREE_FROST` | Mage | Frost | ~~61~~ **823** ✅ | verified 2026-05-03 |
| `HUNTER_TREE_BEAST` | Hunter | Beast Mastery | ~~50~~ **811** ✅ | verified 2026-05-03 |
| `HUNTER_TREE_MARKSMANSHIP` | Hunter | Marksmanship | ~~51~~ **807** ✅ | verified 2026-05-03 |
| `HUNTER_TREE_SURVIVAL` | Hunter | Survival | ~~163~~ **809** ✅ | verified 2026-05-03 |

**✅ Resolved 2026-05-03** — all 9 constants were wrong. Corrected in `src/AltbotStrategyFactory.cpp`. All original values (302/303/301, 81/41/61, 50/51/163) appear to have been placeholder/WotLK-era IDs.

---

## Spell-name match assumption (warlock / mage / hunter strategies)

`StrategyUtil::FindSpellByFamilyName` resolves each cached spell by exact match against `SpellInfo::SpellName`. This is the canonical English string from `Spell.dbc`. **Server installs running a non-enUS DBC pack will silently fail to populate the cache** — the bot will load with all spell IDs at 0 and fall back to filler-only behavior.

**Symptom:** strategy logs `cache for '<name>': ... <Spell>=0 ...` for every entry on first tick; rotation never advances past the maintenance/filler line.

**Resolution path (only if non-enUS support becomes a requirement):** swap `SpellInfo::SpellName` lookup for a SpellFamilyFlags-based lookup (TC's `flag96` per-class flag table) — locale-stable but requires baking flag values per spell.

Not currently a blocker; the project targets enUS data per the spec docs.

---

## Why this is deferred

The cata-altbot doc tree currently uses Wowhead Cata-archive values + reconciled-guide-strategy as the working baseline. None of the UNVERIFIED items prevent doc work or block the next spec/dungeon doc. They become real blockers only when:

- `AltbotCombat.cpp` references these values as compile-time constants
- The bot's level-gating logic for talent-vs-baseline abilities needs to decide whether to expose the spell at the talent-tree level or only at max level
- The bot's encounter logic references encounter spell IDs (BRC 75476 dispel-blacklist + 75763 collision are the first cases hitting this threshold)

Until either of those happens, the UNVERIFIED markers are sufficient — they tell future-you (or anyone implementing) where to double-check.

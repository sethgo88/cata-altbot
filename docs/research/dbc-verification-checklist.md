# DBC Verification Checklist

**For: cata-altbot doc UNVERIFIED items.** Run this checklist when you're at the computer with the TrinityCore 4.3.4 source tree and DBC files available.

**Scope** (as of 2026-05-02): the project now has **17 spec docs** + **2 dungeon bundles**, each with its own UNVERIFIED items table. This file is no longer a single flat list — it points at the per-spec/per-dungeon tables and provides worked-example detail for the original 9 Resto Shaman items as a starter pattern.

---

## Per-spec UNVERIFIED tables — read these first

Every `docs/specs/<spec>.md` has an `## UNVERIFIED items` table at the bottom. Each row has the claim, current encoded value, and the DBC file to check. Open the spec doc, work through the table, update the table inline, remove the row when resolved.

### Healer specs (5)
- `docs/specs/resto-shaman.md` — 9 items (the original detailed worked example below)
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

### Dungeon bundles (2)
- `docs/dungeons/throne-of-the-tides/encounter.md` — 16 mechanic spell IDs, mostly Wowhead-verified
- `docs/dungeons/blackrock-caverns/encounter.md` + `docs/research/blackrock-caverns-guide-survey.md` — 18 items including:
  - **Spell ID 75763 collision** — same ID attributed to both Karsh Cinderbreath AND Obsidius shadow puddle. **At least one is wrong; resolve early.**
  - **Crepuscular Veil 75476** — hardcoded as dispel-blacklist entry; if ID is wrong, bot will dispel the debuff and break the swap mechanic. **Runtime correctness; resolve early.**

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

## Three ways to read the values

Pick whichever is easiest for you.

### Option 1 — In-game GM lookup (simplest if your server is running)

Log in as a GM character and use chat commands:
```
.lookup spell <spell_name_or_id>
.lookup spell shock                  # finds all spells with "shock" in name
.gobject info                        # not relevant here, but useful generally
```
The server reports its loaded values for cooldown, mana cost, etc. directly from `Spell.dbc`.

### Option 2 — WDBXEditor (GUI tool)

Download [WDBXEditor](https://github.com/WowDevTools/WDBXEditor) (Windows) — opens DBC files in a spreadsheet view. Open `Spell.dbc`, search by `Id` column for each spell ID below, read the columns you need.

### Option 3 — sqlite via DBC-to-SQL conversion

If you have a script that converts DBC → SQLite, run something like:
```sql
SELECT id, recovery_time, cast_time, mana_cost, base_level, duration_index
FROM spell
WHERE id IN (57994, 52127, 101033, 5394, 98008, 16190, 16188, 77472, 61882);
```
(Field names vary by build; adjust based on your converter's schema.)

For talent-vs-baseline questions, also query `Talent.dbc`:
```sql
SELECT * FROM talent WHERE spell_rank LIKE '%98008%' OR spell_rank LIKE '%16190%' OR spell_rank LIKE '%16188%';
```

---

## The 9 items to verify

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
2. **The original 9 Resto Shaman items** (talent-vs-baseline questions in particular: SLT, MTT, NS) — gates leveling-rotation correctness from L20-L85 for one of the project's most-used specs.
3. **First-of-class mechanics** for specs that introduce a new resource model:
   - Frost DK Runes / Runic Power / disease IDs (first DK doc — pattern for Blood tank later)
   - Feral Cat per-target Combo Points / bleed pandemic windows / glyphs (bleed mechanics had Cata 4.0 rework)
   - Enh Shaman Maelstrom Weapon proc rate / Mental Quickness AP→SP %
4. **Mastery formulas** across all 17 specs (~17 quick lookups; informs reforge logic)
5. **Glyph effects** across all specs (low individual impact; high count)
6. **Level gates** for talented vs trainer abilities (mostly trivial confirmations)

---

## TC-fork code-blockers (also pendable this trip)

These are not DBC items but live in the same TC-fork session because they need the source tree:

- `RBAC_PERM_COMMAND_GM` — confirm the correct constant name on this fork (referenced in `CLAUDE.md`)
- `_legacyConnectionModeEnabled` — confirm visibility from `AltbotLogin` in `server-core/src/server/game/Handlers/CharacterHandler.cpp`
- PlayerScript hook surface: `OnLootRoll`, `OnQuestAccept`, `OnQuestReward`, mount detection (`OnSpellCast` vs `OnAuraApply`)
- `ServerScript::OnPacketReceive` for inbound addon-channel packets (or the patched `WorldSession::HandleMessagechatOpcode` route)
- `Group::GetTargetIcons()` accessor for skull-mark assist
- `Player::LearnTalent(uint32 talentId, uint32 rank)` signature on Cata fork (and `GetActiveSpec()` for dual-spec)
- Addon-message prefix max length + per-message size cap (drives chunking decisions)
- 3 talent-aware-combat design questions in `docs/research/talent-aware-combat-design.md`

---

## Why this is deferred

The cata-altbot doc tree currently uses Wowhead Cata-archive values + reconciled-guide-strategy as the working baseline. None of the UNVERIFIED items prevent doc work or block the next spec/dungeon doc. They become real blockers only when:

- `AltbotCombat.cpp` references these values as compile-time constants
- The bot's level-gating logic for talent-vs-baseline abilities needs to decide whether to expose the spell at the talent-tree level or only at max level
- The bot's encounter logic references encounter spell IDs (BRC 75476 dispel-blacklist + 75763 collision are the first cases hitting this threshold)

Until either of those happens, the UNVERIFIED markers are sufficient — they tell future-you (or anyone implementing) where to double-check.

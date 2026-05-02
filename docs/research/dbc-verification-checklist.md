# DBC Verification Checklist

**For: cata-altbot doc UNVERIFIED items.** Run this checklist when you're at the computer with the TrinityCore 4.3.4 source tree and DBC files available. Output: 9 verified values that update the spec doc + audit-trail correction notes.

This file is the single source of truth for what needs verifying. After completing it, update the affected docs and delete the resolved entries from this file.

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

## Bonus DBC items (low priority — not in the official 9)

While you're in there, these are smaller items that would also benefit from verification but aren't blocking anything:

- All 16 Cata heroic dungeon mechanic spell IDs in `docs/dungeons/throne-of-the-tides/encounter.md` — already Wowhead-verified, but DBC confirmation would be nice when convenient. Not urgent.
- Earthliving Weapon proc chance % (currently 20% from Wowhead — confirm)
- Tidal Waves buff bonus values (currently -30% cast time / +30% crit — confirm)

These can be verified opportunistically when you're already in the DBC for the main 9.

---

## Why this is deferred

The cata-altbot doc tree currently uses Wowhead Cata-archive values + reconciled-guide-strategy as the working baseline. None of the UNVERIFIED items prevent doc work or block the next spec doc (Holy Paladin). They become real blockers only when:

- `AltbotCombat.cpp` references these values as compile-time constants
- The bot's level-gating logic for SLT / MTT / NS needs to know "talent or baseline" to decide whether to expose the spell at level 35 or only at 85

Until either of those happens, the UNVERIFIED markers are sufficient — they tell future-you (or anyone implementing) where to double-check.

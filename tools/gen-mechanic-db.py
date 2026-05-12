#!/usr/bin/env python3
"""
gen-mechanic-db.py — emits src/data/MechanicDb.gen.{h,cpp} from
docs/data/dungeon-mechanic-db.csv.

Phase 2 codegen for the cata-altbot encounter pipeline. The generated table
is a sorted-by-spell_id constexpr array consumed by `EncounterMechanics::Lookup`.

Filters used by the avoidable rows (the only rows Phase 2 ships):
  - mechanic_class == "avoidable"
  - status in {"verified", "unbound"}   (DBC-name-match required;
    "unbound" rows pass because many TC bosses are empty stubs and the
    spell ID can still be authoritative per DBC.)

Re-run by hand whenever the CSV changes; the generated files are checked in.
"""
import csv
from collections import defaultdict
from pathlib import Path

REPO_ROOT = Path(__file__).resolve().parent.parent
CSV_PATH  = REPO_ROOT / "docs" / "data" / "dungeon-mechanic-db.csv"
OUT_DIR   = REPO_ROOT / "src" / "data"
OUT_H     = OUT_DIR / "MechanicDb.gen.h"
OUT_CPP   = OUT_DIR / "MechanicDb.gen.cpp"

DEFAULT_AVOID_RADIUS_Y = 6.0   # plain ground-patch fallback; per-row override wins
ACCEPTED_STATUSES      = {"verified", "unbound"}

# Map dispel_type strings from the YAML / DBC into a stable enum value.
DISPEL_ENUM = {
    "":               "None",
    "none":           "None",
    "magic":          "Magic",
    "curse":          "Curse",
    "poison":         "Poison",
    "disease":        "Disease",
    "enrage_purge":   "EnragePurge",
    "magic_offensive":"MagicOffensive",   # purge / spellsteal
}

PRIORITY_ENUM = {
    "":                          "Default",
    "MUST_INTERRUPT":            "MustInterrupt",
    "SHOULD_INTERRUPT":          "ShouldInterrupt",
    "HIGH_DISPEL":               "HighDispel",
    "SHOULD_DISPEL":             "ShouldDispel",
    "HIGHEST_DISPEL_OFFENSIVE":  "HighestDispelOffensive",
    "TANK_KITE_TRIGGER":         "Default",
    "TANK_BURST_DEFENSIVE":      "Default",
    "MELEE_POSITIONING_RULE":    "Default",
    "SELF_MANAGED_POSITIONING":  "Default",
    "KILL_ON_SIGHT":             "Default",
    "CRITICAL_DPS_SWITCH":       "Default",
    "CRITICAL":                  "Default",
}

def parse_float(val, default):
    try:
        return float(val)
    except (TypeError, ValueError):
        return default

CLASS_ENUM = {
    "avoidable": "Avoidable",
    "interrupt": "Interrupt",
    "dispel":    "Dispel",
}

def best_class(rows_for_sid):
    """Pick the dominant mechanic_class for a spell_id, preferring `avoidable`
    over `interrupt` over `dispel`. (Avoidance is the highest-priority bot
    response — stepping out of fire trumps kicking the cast.)"""
    classes = {r["mechanic_class"] for r in rows_for_sid}
    for c in ("avoidable", "interrupt", "dispel"):
        if c in classes:
            return c
    return ""

def merge_attrs(rows_for_sid, target_class):
    """Pick the most specific row attributes for this spell_id at the chosen
    class — e.g. the dispel_type and priority from the `dispel` row even if
    we tagged the entry as `avoidable`."""
    merged = dict(rows_for_sid[0])
    for r in rows_for_sid:
        if r["mechanic_class"] == target_class:
            for k in ("avoid_radius_y","dispel_type","priority","interruptible"):
                if r.get(k):
                    merged[k] = r[k]
            break
    # Also pull a dispel_type / priority from any sibling row even if we
    # didn't pick that as the class.
    for r in rows_for_sid:
        if not merged.get("dispel_type") and r.get("dispel_type"):
            merged["dispel_type"] = r["dispel_type"]
        if (not merged.get("priority") or merged["priority"] == "") and r.get("priority"):
            merged["priority"] = r["priority"]
    return merged

def main():
    if not CSV_PATH.exists():
        raise SystemExit(f"input CSV missing: {CSV_PATH}")

    with open(CSV_PATH, encoding="utf-8") as f:
        rows = list(csv.DictReader(f))

    # Phase 2/3 accept three mechanic_class values into the runtime table.
    # Group by spell_id first — a single ID can have multiple rows (e.g.
    # "interrupt" and "dispel" because it's interruptible AND has a dispel
    # type). We keep one entry per spell_id, with merged attributes.
    by_sid = defaultdict(list)
    for r in rows:
        if r["mechanic_class"] not in CLASS_ENUM:
            continue
        if r["status"] not in ACCEPTED_STATUSES:
            continue
        sid = r["spell_id"]
        if not sid or not sid.isdigit():
            continue
        by_sid[int(sid)].append(r)

    deduped = []
    for sid in sorted(by_sid):
        rows_for_sid = by_sid[sid]
        cls = best_class(rows_for_sid)
        if not cls:
            continue
        merged = merge_attrs(rows_for_sid, cls)
        merged["__class"] = cls
        deduped.append((sid, merged))

    OUT_DIR.mkdir(parents=True, exist_ok=True)

    # Generate header.
    with open(OUT_H, "w", encoding="utf-8") as f:
        f.write(_HEADER_PROLOGUE)
        f.write(f"\nstatic constexpr std::size_t kMechanicDbSize = {len(deduped)};\n")
        f.write(_HEADER_EPILOGUE)

    # Generate cpp.
    with open(OUT_CPP, "w", encoding="utf-8") as f:
        f.write(_CPP_PROLOGUE)
        for sid, r in deduped:
            name = r["spell_name_dbc"].replace('"', '\\"') or r["spell_name_doc"].replace('"', '\\"')
            radius = parse_float(r["avoid_radius_y"], DEFAULT_AVOID_RADIUS_Y)
            slug = r["dungeon_slug"] or "?"
            mob = (r["boss_or_mob"] or "?").replace('"', '\\"')
            cls = CLASS_ENUM[r["__class"]]
            dispel = DISPEL_ENUM.get((r.get("dispel_type") or "").lower(), "None")
            priority = PRIORITY_ENUM.get((r.get("priority") or "").strip(), "Default")
            f.write(f'    {{ {sid:>7}, {radius:>5.1f}f, MechanicClass::{cls}, '
                    f'DispelType::{dispel}, InterruptPriority::{priority}, '
                    f'"{slug}", "{mob}", "{name}" }},  // {r["status"]}\n')
        f.write(_CPP_EPILOGUE)

    # Tally counts per class for the operator.
    counts = {c: 0 for c in CLASS_ENUM.values()}
    for _, r in deduped:
        counts[CLASS_ENUM[r["__class"]]] += 1
    print(f"  emitted {OUT_H} ({len(deduped)} rows: " +
          ", ".join(f"{k}={v}" for k, v in counts.items()) + ")")
    print(f"  emitted {OUT_CPP}")

_HEADER_PROLOGUE = '''// AUTO-GENERATED by tools/gen-mechanic-db.py — DO NOT EDIT BY HAND.
// Source of truth: docs/data/dungeon-mechanic-db.csv.
//
// Re-run with:
//     python3 tools/gen-mechanic-db.py
//
// Sorted-by-spell_id constexpr table consumed via lower_bound by
// `EncounterMechanics::Lookup`. The runtime API lives in EncounterMechanics.h;
// nothing else should include this header.
#pragma once
#include <cstddef>
#include <cstdint>

namespace AltbotMechanicDb
{
    enum class MechanicClass : uint8_t
    {
        Avoidable = 1,
        Interrupt = 2,
        Dispel    = 3,
        Phase     = 4,
    };

    enum class DispelType : uint8_t
    {
        None             = 0,
        Magic            = 1,
        Curse            = 2,
        Poison           = 3,
        Disease          = 4,
        EnragePurge      = 5,
        MagicOffensive   = 6,
    };

    enum class InterruptPriority : uint8_t
    {
        Default                = 0,
        ShouldInterrupt        = 1,
        MustInterrupt          = 2,
        ShouldDispel           = 3,
        HighDispel             = 4,
        HighestDispelOffensive = 5,
    };

    struct MechanicEntry
    {
        uint32             spellId;
        float              avoidRadiusY;
        MechanicClass      mclass;
        DispelType         dispel;
        InterruptPriority  priority;
        char const*        dungeonSlug;
        char const*        bossOrMob;
        char const*        spellName;
    };
'''

_HEADER_EPILOGUE = '''
    // Defined in MechanicDb.gen.cpp.
    extern MechanicEntry const kMechanicDb[];
}
'''

_CPP_PROLOGUE = '''// AUTO-GENERATED by tools/gen-mechanic-db.py — DO NOT EDIT BY HAND.
#include "MechanicDb.gen.h"

namespace AltbotMechanicDb
{
    MechanicEntry const kMechanicDb[] = {
'''

_CPP_EPILOGUE = '''    };
}
'''

if __name__ == "__main__":
    main()

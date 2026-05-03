#!/usr/bin/env python3
"""
dbc-extract.py
Reads Spell.csv + sub-table CSVs exported from WDBXEditor, answers the 9
Resto Shaman DBC verification questions, and writes spell-ref.csv.

Usage:
    python dbc-extract.py

Export from WDBXEditor into DBC_DIR:
    Spell.csv, SpellCooldowns.csv, SpellLevels.csv, SpellPower.csv, SpellDuration.csv

Sub-table field mappings (WDBXEditor exports generic names):
    SpellLevels:  Field01=BaseLevel, Field02=MaxLevel, Field03=SpellLevel
    SpellDuration:Field01=Duration_ms, Field02=DurationPerLevel, Field03=MaxDuration_ms
    SpellPower:   Field01=ManaCost, Field02=ManaCostPct, Field03=ManaCostPerLevel, Field04=ManaPerSecond
    SpellCooldowns: named correctly (RecoveryTime, CategoryRecoveryTime)
"""

import csv
import os
import sys

DBC_DIR  = r"C:\cataclysm\server-build\install\dbc\enUS"
OUT_FILE = r"C:\cataclysm\cata-altbot\docs\data\spell-ref.csv"

TARGET_SPELLS = {
    57994:  "Wind Shear",
    52127:  "Water Shield",
    101033: "Resurgence",
    5394:   "Healing Stream Totem",
    98008:  "Spirit Link Totem",
    16190:  "Mana Tide Totem",
    16188:  "Nature's Swiftness",
    77472:  "Greater Healing Wave",
    61882:  "Earthquake",
}

# ── helpers ───────────────────────────────────────────────────────────────────

def load_csv(filename):
    path = os.path.join(DBC_DIR, filename)
    if not os.path.exists(path):
        print(f"  [WARN] {filename} not found -- skipping")
        return {}
    rows = {}
    with open(path, encoding="utf-8-sig", newline="") as f:
        reader = csv.DictReader(f)
        for row in reader:
            clean = {k.strip().strip('"').lower(): v.strip().strip('"') for k, v in row.items()}
            row_id = clean.get("id", "")
            if row_id:
                rows[row_id] = clean
    print(f"  [OK]   {filename} -- {len(rows)} rows")
    return rows

def get(row, key, default=""):
    return row.get(key.lower(), default)

def ms_str(val):
    try:
        ms = int(val)
        return f"{ms}ms ({ms/1000:.1f}s)" if ms else "0ms"
    except (ValueError, TypeError):
        return str(val) if val else "--"

def level_verdict(base_level, spell_level):
    try:
        lvl = max(int(base_level or 0), int(spell_level or 0))
        return f"BASELINE -- available at level {lvl}" if lvl else "UNKNOWN"
    except ValueError:
        return "UNKNOWN"

# ── main ──────────────────────────────────────────────────────────────────────

def main():
    print("=" * 60)
    print("DBC Extract -- Resto Shaman Verification")
    print("=" * 60)
    print()
    print("Loading CSVs...")
    spells    = load_csv("Spell.csv")
    cooldowns = load_csv("SpellCooldowns.csv")
    levels    = load_csv("SpellLevels.csv")
    power     = load_csv("SpellPower.csv")
    durations = load_csv("SpellDuration.csv")
    print()

    results = []
    for spell_id, spell_name in TARGET_SPELLS.items():
        sid       = str(spell_id)
        s         = spells.get(sid, {})
        if not s:
            print(f"[WARN] Spell ID {spell_id} ({spell_name}) not found in Spell.csv")
            results.append({"SpellID": sid, "Name": spell_name})
            continue

        cd  = cooldowns.get(get(s, "SpellCooldownsId"), {})
        lv  = levels.get(get(s, "SpellLevelsId"), {})
        pw  = power.get(get(s, "SpellPowerId"), {})
        dur = durations.get(get(s, "DurationIndex"), {})

        # SpellLevels: Field01=BaseLevel, Field02=MaxLevel, Field03=SpellLevel
        base_level  = get(lv, "field01")
        max_level   = get(lv, "field02")
        spell_level = get(lv, "field03")

        # SpellDuration: Field01=Duration_ms
        duration_ms = get(dur, "field01")

        # SpellPower: Field01=ManaCost, Field02=ManaCostPct
        mana_cost = get(pw, "field01")
        mana_pct  = get(pw, "field02")

        # SpellCooldowns: named correctly
        cooldown_ms     = get(cd, "RecoveryTime")
        cat_cooldown_ms = get(cd, "CategoryRecoveryTime")

        results.append({
            "SpellID":             sid,
            "Name":                spell_name,
            "SpellLevel":          spell_level,
            "BaseLevel":           base_level,
            "MaxLevel":            max_level,
            "ManaCost":            mana_cost,
            "ManaCostPct":         mana_pct,
            "Cooldown_ms":         cooldown_ms,
            "CategoryCooldown_ms": cat_cooldown_ms,
            "Duration_ms":         duration_ms,
            "Description":         get(s, "Description"),
            "AuraDescription":     get(s, "AuraDescription"),
        })

    def find(spell_id):
        return next((r for r in results if r["SpellID"] == str(spell_id)), {})

    print("=" * 60)
    print("VERIFICATION ANSWERS")
    print("=" * 60)

    r = find(57994)
    print("\n1. Wind Shear (57994) -- cooldown")
    print(f"   RecoveryTime:         {ms_str(r.get('Cooldown_ms'))}")
    print(f"   CategoryRecoveryTime: {ms_str(r.get('CategoryCooldown_ms'))}")

    r = find(52127)
    print("\n2. Water Shield (52127) -- mana per orb")
    print(f"   ManaCost:    {r.get('ManaCost') or '--'}")
    print(f"   ManaCostPct: {r.get('ManaCostPct') or '--'}")
    print(f"   AuraDesc:    {r.get('AuraDescription') or '--'}")

    r = find(101033)
    print("\n3. Resurgence (101033) -- mana return")
    print(f"   ManaCost:    {r.get('ManaCost') or '--'}")
    print(f"   ManaCostPct: {r.get('ManaCostPct') or '--'}")
    print(f"   Description: {r.get('Description') or '--'}")

    r = find(5394)
    print("\n4. Healing Stream Totem (5394) -- duration")
    print(f"   Duration: {ms_str(r.get('Duration_ms'))}")

    for n, (spell_id, label) in enumerate([(98008, "Spirit Link Totem"),
                                            (16190, "Mana Tide Totem"),
                                            (16188, "Nature's Swiftness")], start=5):
        r = find(spell_id)
        print(f"\n{n}. {label} ({spell_id}) -- baseline or talent?")
        print(f"   BaseLevel:  {r.get('BaseLevel') or '--'}")
        print(f"   SpellLevel: {r.get('SpellLevel') or '--'}")
        print(f"   Verdict:    {level_verdict(r.get('BaseLevel'), r.get('SpellLevel'))}")

    r = find(77472)
    print("\n8. Greater Healing Wave (77472) -- required level")
    print(f"   BaseLevel:  {r.get('BaseLevel') or '--'}")
    print(f"   SpellLevel: {r.get('SpellLevel') or '--'}")

    r = find(61882)
    print("\n9. Earthquake (61882) -- required level")
    print(f"   BaseLevel:  {r.get('BaseLevel') or '--'}")
    print(f"   SpellLevel: {r.get('SpellLevel') or '--'}")

    # ── write spell-ref.csv ───────────────────────────────────────────────────
    print()
    print("=" * 60)
    os.makedirs(os.path.dirname(OUT_FILE), exist_ok=True)
    fieldnames = ["SpellID","Name","SpellLevel","BaseLevel","MaxLevel",
                  "ManaCost","ManaCostPct","Cooldown_ms","CategoryCooldown_ms",
                  "Duration_ms","Description","AuraDescription"]
    with open(OUT_FILE, "w", newline="", encoding="utf-8") as f:
        writer = csv.DictWriter(f, fieldnames=fieldnames, extrasaction="ignore")
        writer.writeheader()
        writer.writerows(results)
    print(f"spell-ref.csv written -> {OUT_FILE}")

if __name__ == "__main__":
    main()

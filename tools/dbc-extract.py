#!/usr/bin/env python3
"""
dbc-extract.py — DBC verification utility for cata-altbot.

Two modes:

  (default)  resto-shaman: prints the legacy 9-item Resto Shaman verification
             report and writes docs/data/spell-ref.csv. Preserved as a worked
             example; running without args reproduces the original behavior.

  --verify-encounters
             Walks docs/dungeons/*/encounter.md, parses every spell_id with
             surrounding YAML context (ability name, cast_by, avoidable,
             interruptible, dispel_type, priority), joins against Spell.csv
             + SpellEffect.csv, cross-references TC scripts under
             ~/web/wow/src/server/scripts/<dungeon-dir>/, and emits:
               - per-row disposition (verified | name-mismatch | unbound |
                 not-found | skipped)
               - docs/data/dungeon-mechanic-db.csv — canonical mechanic table
                 consumed by Phase 2 codegen.

Sub-table field mappings (WDBXEditor exports generic names):
    SpellLevels:   Field01=BaseLevel    Field02=MaxLevel    Field03=SpellLevel
    SpellDuration: Field01=Duration_ms  Field02=DurationPerLevel  Field03=MaxDuration_ms
    SpellPower:    Field01=ManaCost     Field02=ManaCostPct  Field03=ManaCostPerLevel  Field04=ManaPerSecond
    SpellCooldowns: named correctly     (RecoveryTime, CategoryRecoveryTime)
"""

import argparse
import csv
import os
import re
import sys
from collections import defaultdict
from pathlib import Path

# ── paths ────────────────────────────────────────────────────────────────────

REPO_ROOT = Path(__file__).resolve().parent.parent
DBC_DIR   = Path(os.path.expanduser("~/web/wow/doc/dbc"))
TC_SCRIPTS_DIR = Path(os.path.expanduser("~/web/wow/src/server/scripts"))
DUNGEONS_DIR   = REPO_ROOT / "docs" / "dungeons"
OUT_SPELL_REF  = REPO_ROOT / "docs" / "data" / "spell-ref.csv"
OUT_MECHANIC_DB = REPO_ROOT / "docs" / "data" / "dungeon-mechanic-db.csv"

# slug → relative TC scripts directory.  Encounter docs use the directory name
# (kebab-case); the value here is the canonical TC source path.
DUNGEON_TC_DIRS = {
    "throne-of-the-tides":      "EasternKingdoms/ThroneOfTheTides",
    "blackrock-caverns":        "EasternKingdoms/BlackrockMountain/BlackrockCaverns",
    "the-stonecore":            "Maelstrom/Stonecore",
    "grim-batol":               "EasternKingdoms/GrimBatol",
    "halls-of-origination":     "Kalimdor/HallsOfOrigination",
    "lost-city-of-the-tolvir":  "Kalimdor/LostCityOfTheTolvir",
    "the-vortex-pinnacle":      "Kalimdor/VortexPinnacle",
    "end-time":                 "Kalimdor/CavernsOfTime/EndTime",
    "hour-of-twilight":         "Kalimdor/CavernsOfTime/HourOfTwilight",
    "well-of-eternity":         "Kalimdor/CavernsOfTime/WellOfEternity",
    "zul-aman":                 "EasternKingdoms/ZulAman",
    "zul-gurub":                "EasternKingdoms/ZulGurub",
}

# ── legacy resto-shaman verifier (preserved) ─────────────────────────────────

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

def load_csv(filename, key="ID"):
    """Load a DBC CSV keyed by the chosen column (default 'ID')."""
    path = DBC_DIR / filename
    if not path.exists():
        print(f"  [WARN] {filename} not found at {path} -- skipping")
        return {}
    rows = {}
    with open(path, encoding="utf-8-sig", newline="") as f:
        reader = csv.DictReader(f)
        for row in reader:
            clean = {k.strip().strip('"').lower(): v.strip().strip('"') for k, v in row.items()}
            row_id = clean.get(key.lower(), "")
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

def run_resto_shaman():
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
        sid = str(spell_id)
        s   = spells.get(sid, {})
        if not s:
            print(f"[WARN] Spell ID {spell_id} ({spell_name}) not found in Spell.csv")
            results.append({"SpellID": sid, "Name": spell_name})
            continue

        cd  = cooldowns.get(get(s, "SpellCooldownsId"), {})
        lv  = levels.get(get(s, "SpellLevelsId"), {})
        pw  = power.get(get(s, "SpellPowerId"), {})
        dur = durations.get(get(s, "DurationIndex"), {})

        results.append({
            "SpellID":             sid,
            "Name":                spell_name,
            "SpellLevel":          get(lv, "field03"),
            "BaseLevel":           get(lv, "field01"),
            "MaxLevel":            get(lv, "field02"),
            "ManaCost":            get(pw, "field01"),
            "ManaCostPct":         get(pw, "field02"),
            "Cooldown_ms":         get(cd, "RecoveryTime"),
            "CategoryCooldown_ms": get(cd, "CategoryRecoveryTime"),
            "Duration_ms":         get(dur, "field01"),
            "Description":         get(s, "Description"),
            "AuraDescription":     get(s, "AuraDescription"),
        })

    def find(spell_id):
        return next((r for r in results if r["SpellID"] == str(spell_id)), {})

    print("=" * 60)
    print("VERIFICATION ANSWERS")
    print("=" * 60)
    r = find(57994); print("\n1. Wind Shear (57994) -- cooldown")
    print(f"   RecoveryTime:         {ms_str(r.get('Cooldown_ms'))}")
    print(f"   CategoryRecoveryTime: {ms_str(r.get('CategoryCooldown_ms'))}")
    r = find(52127); print("\n2. Water Shield (52127) -- mana per orb")
    print(f"   ManaCost:    {r.get('ManaCost') or '--'}")
    print(f"   AuraDesc:    {r.get('AuraDescription') or '--'}")
    r = find(101033); print("\n3. Resurgence (101033) -- mana return")
    print(f"   Description: {r.get('Description') or '--'}")
    r = find(5394); print("\n4. Healing Stream Totem (5394) -- duration")
    print(f"   Duration: {ms_str(r.get('Duration_ms'))}")
    for n, (spell_id, label) in enumerate([(98008, "Spirit Link Totem"),
                                            (16190, "Mana Tide Totem"),
                                            (16188, "Nature's Swiftness")], start=5):
        r = find(spell_id)
        print(f"\n{n}. {label} ({spell_id}) -- baseline or talent?")
        print(f"   BaseLevel:  {r.get('BaseLevel') or '--'}")
        print(f"   SpellLevel: {r.get('SpellLevel') or '--'}")
        print(f"   Verdict:    {level_verdict(r.get('BaseLevel'), r.get('SpellLevel'))}")
    r = find(77472); print("\n8. Greater Healing Wave (77472) -- required level")
    print(f"   BaseLevel:  {r.get('BaseLevel') or '--'}   SpellLevel: {r.get('SpellLevel') or '--'}")
    r = find(61882); print("\n9. Earthquake (61882) -- required level")
    print(f"   BaseLevel:  {r.get('BaseLevel') or '--'}   SpellLevel: {r.get('SpellLevel') or '--'}")

    OUT_SPELL_REF.parent.mkdir(parents=True, exist_ok=True)
    fieldnames = ["SpellID","Name","SpellLevel","BaseLevel","MaxLevel",
                  "ManaCost","ManaCostPct","Cooldown_ms","CategoryCooldown_ms",
                  "Duration_ms","Description","AuraDescription"]
    with open(OUT_SPELL_REF, "w", newline="", encoding="utf-8") as f:
        writer = csv.DictWriter(f, fieldnames=fieldnames, extrasaction="ignore")
        writer.writeheader()
        writer.writerows(results)
    print(f"\nspell-ref.csv written -> {OUT_SPELL_REF}")

# ── encounter verifier ───────────────────────────────────────────────────────

# YAML keys that may be attached to a mechanic block.  Captured into the row.
ABILITY_FIELD_KEYS = {
    "spell_id", "cast_by", "cast_kind", "target", "effect", "duration_ms",
    "frequency_ms", "detonation_delay_ms", "radius", "initial_radius",
    "interruptible", "dispel_type", "avoidable", "avoidance", "priority",
    "notes", "trigger", "range",
}

# School-mask bits → friendly name (Spell.dbc SchoolMask field).
SCHOOL_MASK_NAMES = [
    (1 << 0, "physical"),
    (1 << 1, "holy"),
    (1 << 2, "fire"),
    (1 << 3, "nature"),
    (1 << 4, "frost"),
    (1 << 5, "shadow"),
    (1 << 6, "arcane"),
]

def school_mask_to_name(raw):
    try:
        m = int(raw or 0)
    except ValueError:
        return ""
    parts = [name for bit, name in SCHOOL_MASK_NAMES if m & bit]
    return "+".join(parts) if parts else "none"

def parse_int_or_none(s):
    if not s:
        return None
    try:
        return int(s)
    except (TypeError, ValueError):
        return None

# Block headers come in several flavors across the encounter docs:
#   - name: Foo                 (most common — abilities, mechanics, trash)
#   - boss_ability: Foo         (ToTT boss-mechanic blocks)
#   - mob_summon: Foo           (summon-effect blocks)
#   - buff: Foo                 (party-buff blocks; rare)
NAME_RE       = re.compile(r'^(\s*)-\s+(?:name|boss_ability|mob_summon|buff):\s+(.+?)(?:\s*#.*)?$')
MOB_RE        = re.compile(r'^(\s*)-\s+mob:\s+(.+?)(?:\s*#.*)?$')
NPC_ID_RE     = re.compile(r'^\s*npc_id:\s*(\d+)')
BOSS_RE       = re.compile(r'^boss:\s+(\S+)')
KV_RE         = re.compile(r'^\s*([a-z_][a-z_0-9]*):\s*(.+?)(?:\s*#.*)?\s*$')
SPELL_ID_RE   = re.compile(r'spell_id:\s*(\S+?)(?:\s*#.*)?$')
SPELL_ID_INLINE = re.compile(r'spell_id:\s*(\S+)')

def slug_from_dir(p):
    """`docs/dungeons/throne-of-the-tides/` → `throne-of-the-tides`."""
    return p.parent.name

def parse_encounter_md(path):
    """Yield mechanic dicts: one per `spell_id:` line found.

    Captured fields:
      file_line, dungeon_slug, ability_name, enclosing_mob, enclosing_mob_npc_id,
      boss, raw_spell_id (str, may be 'UNVERIFIED'), spell_id (int|None),
      doc_unverified (bool), doc_skipped (bool), and any ABILITY_FIELD_KEYS
      values present in the surrounding block.
    """
    slug = slug_from_dir(path)
    with open(path, encoding="utf-8") as f:
        lines = f.readlines()

    # Walk; for each spell_id line, collect context.
    for i, line in enumerate(lines):
        m = SPELL_ID_INLINE.search(line)
        if not m:
            continue
        raw_id = m.group(1).split('#')[0].strip()
        doc_unverified = "UNVERIFIED" in line
        doc_skipped    = "SKIPPED-AWAITING-INGAME" in line
        spell_id_int   = parse_int_or_none(raw_id)

        # Find enclosing ability `- name:`.
        ability_name = None
        ability_indent = None
        ability_start_idx = None
        for j in range(i - 1, -1, -1):
            mn = NAME_RE.match(lines[j])
            if mn:
                ability_name = mn.group(2).strip()
                ability_indent = len(mn.group(1))
                ability_start_idx = j
                break

        # Find enclosing `- mob:` (less-indented than the ability).
        enclosing_mob = None
        mob_npc_id = None
        if ability_indent is not None:
            for j in range(ability_start_idx - 1, -1, -1):
                mm = MOB_RE.match(lines[j])
                if mm and len(mm.group(1)) < ability_indent:
                    enclosing_mob = mm.group(2).strip()
                    # The mob's npc_id usually follows within ~5 lines.
                    for k in range(j + 1, min(j + 8, len(lines))):
                        mid = NPC_ID_RE.match(lines[k])
                        if mid:
                            mob_npc_id = parse_int_or_none(mid.group(1))
                            break
                    break

        # Find enclosing `boss: <slug>` at column 0.
        boss = None
        boss_npc_id = None
        for j in range(i - 1, -1, -1):
            mb = BOSS_RE.match(lines[j])
            if mb:
                boss = mb.group(1).strip()
                # Boss NPC ID typically on the line immediately after.
                for k in range(j + 1, min(j + 6, len(lines))):
                    mid = NPC_ID_RE.match(lines[k])
                    if mid:
                        boss_npc_id = parse_int_or_none(mid.group(1))
                        break
                break

        # Collect fields in the ability scope:
        # - lines from ability_start_idx+1 to next `- name:`/`- mob:` at the same
        #   or shallower indent (or end of file).
        fields = {}
        if ability_start_idx is not None:
            scope_end = len(lines)
            for j in range(ability_start_idx + 1, len(lines)):
                mn = NAME_RE.match(lines[j])
                mm = MOB_RE.match(lines[j])
                if mn and len(mn.group(1)) <= ability_indent:
                    scope_end = j; break
                if mm and len(mm.group(1)) <= ability_indent:
                    scope_end = j; break
            for j in range(ability_start_idx + 1, scope_end):
                mk = KV_RE.match(lines[j])
                if mk:
                    key = mk.group(1)
                    val = mk.group(2).strip().strip('"').strip("'")
                    if key in ABILITY_FIELD_KEYS and key not in fields:
                        fields[key] = val

        yield {
            "file_line":            i + 1,
            "dungeon_slug":         slug,
            "ability_name":         ability_name or "",
            "enclosing_mob":        enclosing_mob or "",
            "enclosing_mob_npc_id": mob_npc_id or "",
            "boss":                 boss or "",
            "boss_npc_id":          boss_npc_id or "",
            "raw_spell_id":         raw_id,
            "spell_id":             spell_id_int,
            "doc_unverified":       doc_unverified,
            "doc_skipped":          doc_skipped,
            "fields":               fields,
        }

def load_spell_effects():
    """Return dict: spell_id (str) → list of effect rows (dict)."""
    path = DBC_DIR / "SpellEffect.csv"
    if not path.exists():
        print(f"  [WARN] SpellEffect.csv not found at {path}")
        return {}
    out = defaultdict(list)
    with open(path, encoding="utf-8-sig", newline="") as f:
        reader = csv.DictReader(f)
        for row in reader:
            clean = {k.strip().strip('"').lower(): v.strip().strip('"') for k, v in row.items()}
            sid = clean.get("effectspellid", "")
            if sid:
                out[sid].append(clean)
    print(f"  [OK]   SpellEffect.csv -- {sum(len(v) for v in out.values())} effects "
          f"covering {len(out)} spells")
    return dict(out)

def grep_spell_id_in_dir(spell_id, tc_dir):
    """Return list of (relpath, line_no, line) where the literal spell_id appears
    as a standalone token. Cheap recursive scan; treats *.cpp/*.h files only."""
    if not tc_dir.exists():
        return []
    pat = re.compile(r'\b' + re.escape(str(spell_id)) + r'\b')
    hits = []
    for fn in tc_dir.rglob("*"):
        if not fn.is_file():
            continue
        if fn.suffix.lower() not in (".cpp", ".h", ".hpp", ".inl"):
            continue
        try:
            with open(fn, encoding="utf-8", errors="replace") as f:
                for ln, line in enumerate(f, start=1):
                    if pat.search(line):
                        hits.append((str(fn.relative_to(tc_dir)), ln, line.rstrip()))
                        if len(hits) >= 5:  # cap; we just need bound/unbound + a citation
                            return hits
        except OSError:
            continue
    return hits

def truthy(v):
    return str(v or "").strip().lower() in ("true", "yes", "1", "on")

def classify_mechanics(row):
    """Yield zero-or-more mechanic_class strings for a parsed row.

    A single ability can contribute to multiple Phase 2/3 hooks:
      - avoidable (Phase 2 ground-effect)
      - interrupt (Phase 3 interrupt list)
      - dispel    (Phase 3 dispel list)
      - phase     (boss-phase trigger; reserved)
    """
    f = row["fields"]
    classes = []
    if truthy(f.get("avoidable")) or f.get("avoidable", "").lower().startswith("partial") \
            or f.get("avoidable", "").lower().startswith("yes"):
        classes.append("avoidable")
    if truthy(f.get("interruptible")) or f.get("interruptible") == "cast_only":
        classes.append("interrupt")
    dispel = f.get("dispel_type", "").lower()
    if dispel and dispel not in ("", "none", "n/a"):
        classes.append("dispel")
    return classes

def normalize_name(s):
    return re.sub(r'[^a-z0-9]+', '', (s or "").lower())

def status_for(row, dbc_row, tc_hits):
    """Return (status, note). status ∈
       verified | name-mismatch | unbound | not-found | skipped | placeholder."""
    if row["doc_skipped"]:
        return "skipped", "doc marker SKIPPED-AWAITING-INGAME"
    if row["spell_id"] is None:
        return "placeholder", f"non-numeric spell_id ({row['raw_spell_id']!r})"
    if not dbc_row:
        return "not-found", "no row in Spell.csv"
    dbc_name = dbc_row.get("name", "")
    doc_name = row["ability_name"]
    name_ok = (normalize_name(dbc_name) == normalize_name(doc_name)
               or normalize_name(doc_name) in normalize_name(dbc_name)
               or normalize_name(dbc_name) in normalize_name(doc_name))
    bound = bool(tc_hits)
    if not name_ok and not bound:
        return "name-mismatch+unbound", f"DBC='{dbc_name}', no TC script reference"
    if not name_ok:
        return "name-mismatch", f"DBC='{dbc_name}'"
    if not bound:
        return "unbound", "DBC name matches but no TC script casts the ID"
    return "verified", "DBC + TC script confirm"

def run_verify_encounters():
    print("=" * 64)
    print("DBC Extract -- Encounter Spell Verification")
    print("=" * 64)
    print(f"DBC dir:        {DBC_DIR}")
    print(f"TC scripts dir: {TC_SCRIPTS_DIR}")
    print(f"Dungeons dir:   {DUNGEONS_DIR}")
    print()

    # 1. Load DBC.
    print("Loading CSVs...")
    spells  = load_csv("Spell.csv")
    effects = load_spell_effects()
    print()

    # 2. Walk encounter docs.
    encounter_files = sorted(DUNGEONS_DIR.glob("*/encounter.md"))
    print(f"Found {len(encounter_files)} encounter docs.")
    rows = []
    for path in encounter_files:
        for parsed in parse_encounter_md(path):
            rows.append(parsed)
    print(f"Parsed {len(rows)} mechanic entries with spell_id markers.\n")

    # 3. Cross-reference + classify.
    tc_grep_cache = {}  # (slug, spell_id) → hits

    out_csv_rows = []
    summary = defaultdict(lambda: defaultdict(int))   # slug → status → count
    for r in rows:
        slug = r["dungeon_slug"]
        sid  = r["spell_id"]
        dbc_row = spells.get(str(sid)) if sid is not None else None

        # Cross-reference: only attempt if we have a numeric ID.
        tc_subdir = DUNGEON_TC_DIRS.get(slug)
        tc_hits = []
        if sid is not None and tc_subdir:
            key = (slug, sid)
            if key not in tc_grep_cache:
                tc_grep_cache[key] = grep_spell_id_in_dir(sid, TC_SCRIPTS_DIR / tc_subdir)
            tc_hits = tc_grep_cache[key]

        status, note = status_for(r, dbc_row, tc_hits)

        # Friendly fields from DBC.
        dbc_name   = dbc_row.get("name", "") if dbc_row else ""
        school     = school_mask_to_name(dbc_row.get("schoolmask", "")) if dbc_row else ""
        attrs      = dbc_row.get("attributes", "") if dbc_row else ""

        # Effect summary: list (Effect, EffectAura) for the up-to-3 effects.
        eff_summary = ""
        if dbc_row and effects.get(str(sid)):
            triples = []
            for e in effects[str(sid)]:
                triples.append(f"E{e.get('effectindex','?')}={e.get('effect','?')}/A{e.get('effectaura','?')}")
            eff_summary = " ".join(triples)

        # Classes.
        classes = classify_mechanics(r) or ["info"]

        f = r["fields"]
        npc_entry = r["enclosing_mob_npc_id"] or r["boss_npc_id"] or ""
        priority  = f.get("priority", "")
        target    = f.get("target", "")
        avoid_radius = f.get("radius", "") or f.get("initial_radius", "")
        interruptible = "yes" if "interrupt" in classes else "no"
        dispel = (f.get("dispel_type") or "").lower()

        cite = ""
        if tc_hits:
            cite = ";".join(f"{p}:{ln}" for p, ln, _ in tc_hits[:2])

        for mclass in classes:
            out_csv_rows.append({
                "spell_id":         sid if sid is not None else "",
                "spell_name_dbc":   dbc_name,
                "spell_name_doc":   r["ability_name"],
                "dungeon_slug":     slug,
                "boss_or_mob":      r["enclosing_mob"] or r["boss"] or "",
                "source_npc_entry": npc_entry,
                "mechanic_class":   mclass,
                "interruptible":    interruptible,
                "dispel_type":      dispel,
                "target_pattern":   target,
                "priority":         priority,
                "avoid_radius_y":   avoid_radius,
                "school":           school,
                "attributes":       attrs,
                "effect_summary":   eff_summary,
                "tc_citation":      cite,
                "status":           status,
                "doc_unverified":   "yes" if r["doc_unverified"] else "no",
                "notes":            note,
                "doc_source":       f"docs/dungeons/{slug}/encounter.md:{r['file_line']}",
            })

        summary[slug][status] += 1

    # 4. Write CSV.
    OUT_MECHANIC_DB.parent.mkdir(parents=True, exist_ok=True)
    fieldnames = [
        "spell_id","spell_name_dbc","spell_name_doc","dungeon_slug","boss_or_mob",
        "source_npc_entry","mechanic_class","interruptible","dispel_type",
        "target_pattern","priority","avoid_radius_y","school","attributes",
        "effect_summary","tc_citation","status","doc_unverified","notes","doc_source",
    ]
    with open(OUT_MECHANIC_DB, "w", newline="", encoding="utf-8") as f:
        w = csv.DictWriter(f, fieldnames=fieldnames, extrasaction="ignore")
        w.writeheader()
        w.writerows(out_csv_rows)
    print(f"dungeon-mechanic-db.csv -> {OUT_MECHANIC_DB}  ({len(out_csv_rows)} rows)")

    # 5. Print summary.
    print()
    print("=" * 64)
    print("Per-dungeon disposition summary")
    print("=" * 64)
    grand_total = defaultdict(int)
    for slug in sorted(summary):
        slug_total = sum(summary[slug].values())
        statuses = ", ".join(f"{k}={v}" for k, v in sorted(summary[slug].items()))
        print(f"  {slug:<28} total={slug_total:<3}  {statuses}")
        for k, v in summary[slug].items():
            grand_total[k] += v
    print()
    print(f"  {'GRAND TOTAL':<28} total={sum(grand_total.values()):<3}  "
          + ", ".join(f"{k}={v}" for k, v in sorted(grand_total.items())))

    # 6. Highlight the known BRC offenders explicitly.
    print()
    print("=" * 64)
    print("Watch list — known-bad IDs from dbc-verification-checklist.md")
    print("=" * 64)
    for known_bad, label in [(75763, "Umbral Mending (BRC misuse)"),
                              (75476, "Dusk Shroud (BRC misuse)")]:
        for r in out_csv_rows:
            if str(r["spell_id"]) == str(known_bad):
                print(f"  {known_bad}  doc='{r['spell_name_doc']}'  dbc='{r['spell_name_dbc']}'"
                      f"  status={r['status']}  src={r['doc_source']}")

# ── entrypoint ───────────────────────────────────────────────────────────────

def main():
    global DBC_DIR, TC_SCRIPTS_DIR
    ap = argparse.ArgumentParser(description=__doc__,
                                 formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--verify-encounters", action="store_true",
                    help="Walk docs/dungeons/*/encounter.md, verify against DBC + "
                         "TC scripts, emit dungeon-mechanic-db.csv.")
    ap.add_argument("--dbc-dir",  type=Path, default=None,
                    help=f"Override DBC CSV dir (default: {DBC_DIR})")
    ap.add_argument("--tc-dir",   type=Path, default=None,
                    help=f"Override TC scripts dir (default: {TC_SCRIPTS_DIR})")
    args = ap.parse_args()

    if args.dbc_dir: DBC_DIR = args.dbc_dir
    if args.tc_dir:  TC_SCRIPTS_DIR = args.tc_dir

    if args.verify_encounters:
        run_verify_encounters()
    else:
        run_resto_shaman()

if __name__ == "__main__":
    main()

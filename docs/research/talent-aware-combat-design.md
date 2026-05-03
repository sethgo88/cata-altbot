# Talent-aware combat — design notes pending TC verification

Status: **future direction, not the current implementation**. The cata-altbot codebase
went a different route in 2026-05 — see "Current implementation" below. This file is kept
as the reference for an eventual APL/Playbook refactor; new specs landing today should
follow the strategy-class pattern, not the APL pattern described here.

## Current implementation (as of 2026-05-03)

Each spec is a hand-written C++ class in `src/strategies/{Spec}Strategy.{h,cpp}` that
inherits `AltbotStrategy` and owns its full tick: maintenance, pet, defensives, and a
tier-priority rotation. Spec dispatch happens in `AltbotStrategyFactory.cpp` keyed on
`getClass()` + `GetPrimaryTalentTree()`. Spell IDs are resolved at first tick via either:

- **Effect introspection** — RestoShamanStrategy's pattern: classify spells by their
  `SPELL_EFFECT_*` / `SPELL_AURA_*` signatures. Works when each ability has a unique
  effect shape (heals are easy: HoT vs direct vs chain vs shield).
- **Name match** — Aff Warlock / Frost Mage / MM Hunter pattern via
  `StrategyUtil::FindSpellByFamilyName(bot, SPELLFAMILY_X, "Canonical English Name")`.
  Used when effect introspection can't disambiguate (warlock DoTs all look identical
  by effect type). Trade-off: assumes enUS DBC pack — flagged in
  `docs/research/dbc-verification-checklist.md`.

Implemented specs: resto-shaman, affliction-warlock, frost-mage, marksmanship-hunter.

Future migration to the APL/Playbook model below is **not** blocking new specs — the
strategy-class pattern is the working approach.

---

The design below needs three things verified against the TrinityCore fork before we lock the data shapes and start writing code. The chat session that produced this lives on a machine without TC source access — pick this up in a session on the TC-fork machine.

---

## The plan in one paragraph

Each class gets a hand-authored **master APL** — a static table of every ability the class might use, each entry tagged with a *static gate* (level / talent learned / talent forbidden / glyph / spec) and a *dynamic condition* (resources, buffs, debuffs, target state). On bot login, talent change, level-up, glyph change, or as a 5s safety-net poll, we run a **compile** step that filters the master APL by the bot's current state and produces a small, ordered **Playbook** of just the abilities this bot can actually use, with spells resolved to their highest known rank. The per-tick combat loop walks the Playbook top-down and casts the first entry whose dynamic conditions match. The current `FindBestHealSpell` / `FindBestDamageSpell` helpers in `AltbotCombat.cpp` retire.

Why two tiers (master APL + compiled Playbook) instead of just an APL evaluated each tick: static gates are most of the predicate cost and don't change between recompile triggers, so resolving them once at compile time keeps the per-tick hot path tiny. Bonus: the Playbook is a per-bot artifact that can be dumped for "why did this bot cast X" debugging.

Why this works for the custom-server scenario the user cares about: a Warlock who specced into both Affliction and Demonology trees doesn't need a special "hybrid Aff/Demo" rotation — the compile step just sees more talent gates pass and the Playbook contains entries from both trees, sorted by priority. Same mechanism handles leveling: gates fall away as talents are spent and as level requirements are met, and the Playbook recompiles on each level-up.

---

## Data shapes (proposed, not yet committed)

These are the C++ shapes we'll use unless verification turns up a reason to change them.

### Master APL entry — authored per-class, static `constexpr` table

```cpp
struct ApGate {                    // resolved at compile time, then discarded
    uint8  minLevel;               // 0 = none
    uint32 requiredTalentSpell;    // spell granted by the talent (0 = none)
    uint32 forbidsTalentSpell;     // entry excluded if this is known (0 = none)
    uint32 requiredGlyph;          // 0 = none
    uint8  primaryTreeMin;         // e.g. 31 for "31-pt talent unlocked"; 0 = none
};

struct ApCondition {               // evaluated every tick — keep cheap
    uint8  flags;                  // bit 0: targetAlive, 1: masterInCombat,
                                   //    2: ownAuraMissing, 3: targetDebuffMissing,
                                   //    4: refreshOnly, ...
    uint32 auraSpellId;            // referenced by the flags above
    uint8  healPctBelow;           // 0 = not a heal trigger
    uint8  resourcePctMin;         // mana / rage / energy / RP floor
    uint8  comboPointsMin;         // rogue / feral
};

enum class ApRole   : uint8 { Heal, Defensive, DPS, Utility };
enum class ApTarget : uint8 { Self, Master, MasterTarget, LowestAlly, BotTarget };

struct ApEntry {
    char const* name;              // "Pyroblast (Hot Streak proc)" — debug only
    uint32      spellFamily;       // canonical/family id; resolved to highest rank known at compile
    ApRole      role;
    uint16      priority;          // lower = earlier within role
    ApGate      staticGate;
    ApCondition dynamic;
    ApTarget    targetKind;
};
```

### Compiled Playbook entry — per-bot, what the per-tick loop walks

```cpp
struct PbEntry {
    uint32      spellId;           // resolved to highest known rank
    ApRole      role;
    uint16      priority;
    ApCondition dynamic;
    ApTarget    targetKind;
    char const* debugName;         // points back into APL static data
};

class AltbotPlaybook {
public:
    static void Compile(Player* bot, AltbotPlaybook& out);
    std::vector<PbEntry> entries;       // sorted by (role, priority)
    uint64               talentHash;    // for staleness detection in the safety-net poll
};
```

---

## Where it slots into the existing module

Current state of `src/`:

- `AltbotAI` has a `Update(uint32 diff)` with `_followTimer` + `_combatTimer`, holds `WorldSession*` + master `ObjectGuid`, knows mode (Follow/Stay).
- `AltbotCombat::Update(bot, master)` is a free namespace function: heal-if-low using `FindBestHealSpell`, then DPS using `FindBestDamageSpell`. Both helpers walk `bot->GetSpellMap()` each tick and pick the highest `SpellLevel` matching effect introspection. No talent / spec / priority awareness.

Changes we'll make:

1. **`AltbotAI`** — add `std::unique_ptr<AltbotPlaybook> _playbook`, an `_initialCompileDone` flag, and a public `RecompilePlaybook()` method. The first tick where `bot->IsInWorld()` returns true performs the initial compile (the bot isn't in-world yet when the AI is constructed in `AltbotMgr::SpawnBot`, so we can't compile at construction time).

2. **`AltbotCombat::Update`** — replace the body. New body: get `bot->GetAI()->GetPlaybook()`, walk entries top-down, evaluate `dynamic` for each, resolve target via `targetKind`, cast first match, return. `FindBestHealSpell` / `FindBestDamageSpell` get deleted.

3. **`AltbotPlaybook`** — new files `src/AltbotPlaybook.{h,cpp}`. The `Compile` function dispatches on `bot->getClass()` to a per-class master APL.

4. **Per-class APL tables** — new subdirectory `src/apl/` with one file per class (`AplMage.cpp`, `AplWarlock.cpp`, ...), each exposing something like `std::span<ApEntry const> GetMageApl()`. Concrete shape TBD; want to avoid pulling in `<span>` if we can use a `(ApEntry const*, size_t)` pair to stay consistent with TC style.

---

## Recompile triggers

| Event | Hook |
|---|---|
| Initial login (player in world) | first AI tick where `bot->IsInWorld()` becomes true |
| Talent point spent / respec | `PlayerScript::OnTalentsReset` (**verify exists**) + `OnLearnSpell` filtered to talent spells |
| Level-up | `PlayerScript::OnLevelChanged` (**verify exists**) |
| Glyph change | hook glyph-apply path, OR rely on the safety-net poll |
| Manual debug | `.altbot recompile` chat command |
| Safety net | every 5s in `AltbotAI::Update`, recompute `talentHash`; recompile if drifted |

The safety-net poll means we don't need every hook on day one. Worst case is a 5s lag before a respec / glyph change reflects in the Playbook.

---

## ✅ Verification queue — DO THIS FIRST in the next session

These are the questions that need answers from the TC source tree before we commit to types and start writing code. The current chat machine doesn't have TC fork access; pick these up in a session on the machine that does.

### Q1 — `PlayerScript` hook surface for the Cata branch

**Need:** confirm or deny the existence of these `PlayerScript` virtual methods in our TC fork's `server-core/src/server/game/Scripting/ScriptMgr.h` (or wherever `PlayerScript` is declared in the fork's tree):

- `OnTalentsReset(Player*, bool noCost)` — fires when a player respecs / clears talents
- `OnLevelChanged(Player*, uint8 oldLevel)` — fires on level-up
- `OnLearnSpell(Player*, uint32 spellId)` — fires when a spell is added (covers talent spells being learned)
- Any glyph-apply hook (e.g., `OnGlyphChanged`, `OnApplyGlyph`) — bonus, not required

**If present:** we'll subclass `PlayerScript` in `AltbotLoader.cpp` (or a dedicated `AltbotPlayerScript.cpp`) and call `AltbotMgr::OnPlayerEvent` style dispatchers that locate the bot's AI by guid and call `RecompilePlaybook()`.

**If missing:** we fall back to the 5s `talentHash` safety-net poll only. The poll already covers the worst case, so this isn't a blocker — just nice-to-have.

**How to verify:** grep the TC fork. `rg 'OnTalentsReset|OnLevelChanged|OnLearnSpell' server-core/src/server/game/Scripting/` and look at `class PlayerScript` for the full virtual list.

### Q2 — Spell-rank resolution in Cata

**Need:** confirm how we should resolve "the highest rank of spell-family X that this bot knows" given Cata's mostly-rankless spell model.

Specific things to verify:
1. Does `Player::GetSpellMap()` still contain old-rank entries that we need to filter, or does Cata cleanly retain only the current rank?
2. Does `SpellInfo` expose a chain pointer (`GetFirstRankSpell()`, `GetNextRankSpell()`) we can walk to find "highest known rank"?
3. Are there spells that still genuinely have ranks in 4.3.4 (talent grants? trainer abilities at low level? rank-progression class quests?) — i.e., is the rank-walk logic still load-bearing or can we just use the canonical spell ID directly in most cases?

**How to verify:** read `server-core/src/server/game/Spells/SpellInfo.h` for the rank API, then grep for `GetFirstRankSpell` / `GetNextRankSpell` / `ChainEntry` usage. Also peek at `Player::HasSpell` and `Player::AddSpell` to see what's stored.

**Why it matters:** if Cata is fully rankless, `ApEntry::spellFamily` collapses to just `spellId` and the compile step skips rank resolution. If ranks still exist for some abilities, we need the walk. The data shape changes either way.

### Q3 — `bot->GetAI()` accessor on `Player`

**Need:** confirm whether `Player` has a hook for storing our `AltbotAI` pointer. Two options:
1. **Patch `Player`** to add an `AltbotAI* m_altbotAI` member and getter. Cleanest, but a core patch.
2. **Map lookup in `AltbotMgr`** — `AltbotMgr::FindBotAI(masterGuid, botGuid)` already exists per `AltbotMgr.h`. The combat loop would call `sAltbotMgr->FindBotAI(...)` instead of `bot->GetAI()`.

Option 2 is already wired and avoids a core patch. Lean toward that unless the per-tick lookup cost is concerning (it's a hash lookup in two unordered_maps, should be fine). Decide on this before writing `AltbotPlaybook::Compile`.

**How to verify:** confirm `AltbotMgr::FindBotAI` is implemented (signature is in `AltbotMgr.h`; check `AltbotMgr.cpp` for the body) and benchmark-not-needed since this is a 1.5s-tick combat loop, not hot.

---

## After verification — proof-of-concept plan

Once Q1 / Q2 / Q3 are answered:

1. Lock the `ApEntry` / `PbEntry` shapes (adjust based on Q2 outcome — rank handling).
2. Write `AltbotPlaybook.{h,cpp}` with `Compile()` and an empty class-dispatch switch.
3. Write **Frost Mage** as the first APL — clean test case (well-defined Cata rotation, lots of static gates from the Frost tree, decent number of dynamic conditions including Brain Freeze / Fingers of Frost procs). The `docs/specs/` work hasn't covered Frost Mage yet but `docs/research/` will have a survey by then per the existing roadmap.
4. Wire `AltbotCombat::Update` to walk the Playbook. Keep the old `FindBest*` helpers around behind a `#ifdef` until the new path is proven on the test character.
5. Add the safety-net `talentHash` poll. Defer the `PlayerScript` hooks (Q1) until the poll-based version is stable.
6. Test: log in, level up, spend a talent point, observe Playbook recompile in debug log.

Class APLs after Frost Mage follow the existing project ordering (healer → ranged → melee → tank), so Resto Shaman is the second one (already has a complete spec doc).

---

## Hand-off checklist for the TC-fork session

- [ ] Read this doc end to end.
- [ ] Run the three verification queries (Q1, Q2, Q3) against the TC fork.
- [ ] Update this doc with answers — mark each Q resolved or note blockers.
- [ ] Once Q2 is answered, lock the `ApEntry` / `PbEntry` shapes and remove the "(proposed, not yet committed)" qualifier above.
- [ ] If all three are answered cleanly, proceed with the Frost Mage POC per the plan above.
- [ ] If any answer changes the design, update this doc *first*, then proceed.

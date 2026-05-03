# Plan: Phase 6 Strategy System — Implementation (Resumed)

## Context

Phase 6 wires the first spec doc (`docs/specs/resto-shaman.md`) into a C++ strategy hierarchy.
AltbotCombat currently does generic spell scanning. This plan adds a proper `AltbotStrategy` abstraction,
a `RestoShamanStrategy` driven by the 9-tier priority list in the spec doc, and a `spec` whisper command
so the master can force a bot's spec at runtime.

**Already done this session:**
- `src/AltbotStrategy.h` — abstract base ✅
- `src/AltbotStrategyFactory.h/.cpp` — talent-tree→slug→strategy mapping ✅

**Remaining (4 tasks below):**

---

## Task 2 — Create RestoShamanStrategy

### CREATE: `src/strategies/RestoShamanStrategy.h`

Pure virtual override of `AltbotStrategy`. Key members:

```cpp
enum class Spell : uint8_t { HealingSurge, HealingWave, GreaterHealingWave, Riptide,
    ChainHeal, EarthShield, WaterShield, SpiritLinkTotem, ManaTideTotem, LightningBolt, Count };
enum class ManaMode { Normal, Conserve, Crisis };
std::array<uint32, size_t(Spell::Count)> _cache{};
bool _cacheResolved = false;
void InvalidateCache();
```

Private methods: `ResolveSpellCache`, `GetManaMode`, `TryCast`, `IsOnCooldown`,
`DoMaintenance`, `CheckCooldowns`, `Tier1`–`Tier9`.

### CREATE: `src/strategies/RestoShamanStrategy.cpp`

#### Spell cache (one-time scan, `ResolveSpellCache`)

No hardcoded IDs — classify by effect signature:

| Slot | Detection predicate |
|------|-------------------|
| Riptide | `SPELL_EFFECT_HEAL` + `SPELL_AURA_PERIODIC_HEAL` in same spell |
| ChainHeal | `SPELL_EFFECT_HEAL` + `Effects[i].ChainTargets > 0` |
| HealingSurge | direct heal, `CastTimeEntry->CastTime` ≤ 2000ms |
| GreaterHealingWave | direct heal, cast time > 2000ms, highest `SpellLevel` |
| HealingWave | direct heal, cast time > 2000ms, lower `SpellLevel` |
| EarthShield | `SPELL_EFFECT_APPLY_AURA`, nature, positive, `ExplicitTargetMask & TARGET_FLAG_UNIT_MASK` set |
| WaterShield | `SPELL_EFFECT_APPLY_AURA`, nature, positive, self-cast (unit mask NOT set) |
| LightningBolt | `SPELL_EFFECT_SCHOOL_DAMAGE`, nature, `!IsPositive()` |
| SpiritLinkTotem | `SPELL_EFFECT_SUMMON` + nature — **deferred** (DBC disambiguation needed) |
| ManaTideTotem | same — **deferred** |

#### Mana mode (spec §8)

- Normal: ≥ 50%  
- Conserve: 25–50%  
- Crisis: < 25%  
- Computed: `100.0f * GetPower(POWER_MANA) / GetMaxPower(POWER_MANA)`

#### Tank heuristic

`FindTank(master)` — party member with highest `GetMaxHealth()`. Falls back to master when solo.

#### 9-tier priority list (spec §5)

1. Bot HP < 35% → Healing Surge on self
2. Tank HP < 40% → GHW (Normal) or Healing Surge
3. Tank missing Riptide HoT → Riptide on tank
4. 3+ members < 40% → Spirit Link Totem (no-ops until totem detection lands)
5. 3+ members < 90%, not Crisis → Chain Heal on lowest HP
6. Any member < 60%, Normal mana → GHW
7. Non-tank < 50%, not Crisis → Healing Surge
8. Any member < 90% → Healing Wave on lowest
9. All ≥ 95%, Normal mana → Lightning Bolt on master's target

#### Maintenance (every tick, before tiers)

- Water Shield on self if missing
- Earth Shield on tank if missing

#### Cooldown triggers

- Mana Tide Totem at mana < 60% (no-ops until totem detection lands)

---

## Task 3 — Wire strategy into AltbotAI + AltbotCombat

### MODIFY: `src/AltbotCombat.h`

```cpp
// New signature — keep AltbotState for assist-mode logic
void Update(Player* bot, Player* master, AltbotState const& state, AltbotStrategy* strategy);
```

### MODIFY: `src/AltbotCombat.cpp`

Add `#include "AltbotStrategy.h"`. At top of `Update()` after IsInCombat guard:
```cpp
if (strategy) { strategy->Update(bot, master); return; }
// … existing generic logic unchanged …
```

### MODIFY: `src/AltbotAI.h`

Add includes: `AltbotStrategy.h`, `AltbotStrategyFactory.h`, `<memory>`, `<string>`.

New members:
```cpp
std::unique_ptr<AltbotStrategy> _strategy;
bool        _strategyResolved = false;
std::string _specOverride;
void SetSpecOverride(std::string const& spec);
```

### MODIFY: `src/AltbotAI.cpp`

`SetSpecOverride()` resets `_strategyResolved` and clears `_strategy`.

In `Update()`, before `_combatTimer` block, lazy-init the strategy:
```cpp
if (!_strategyResolved && bot->IsInWorld())
{
    _strategy = AltbotStrategyFactory::Create(bot, _specOverride);
    _strategyResolved = true;
}
```

Combat call:
```cpp
AltbotCombat::Update(bot, master, _state, _strategy.get());
```

---

## Task 4 — SetBotSpec + SQL

### CREATE: `sql/002_add_spec_override.sql`

```sql
ALTER TABLE character_altbot
  ADD COLUMN IF NOT EXISTS spec_override VARCHAR(32) NOT NULL DEFAULT ''
  COMMENT 'Empty = auto-detect from talent tree. Slug e.g. resto-shaman to force.';
```

### MODIFY: `src/AltbotMgr.h`

Add declaration: `void SetBotSpec(ObjectGuid masterGuid, ObjectGuid botGuid, std::string const& spec);`

### MODIFY: `src/AltbotMgr.cpp`

`SetBotSpec()`: calls `ai->SetSpecOverride(spec)` then persists with `UPDATE character_altbot SET spec_override = ...`.

`SpawnBot()`: after `LoadState(*ai)`, query `spec_override` from `character_altbot` and call `ai->SetSpecOverride()` if non-empty.

---

## Task 5 — spec whisper command

### MODIFY: `src/AltbotCommandTable.cpp`

New handler `CmdSetSpec`:
- arg empty/"auto" → slug `""` (revert to auto-detect)
- arg "resto"/"restoration"/"resto-shaman" → slug `"resto-shaman"`
- arg "elemental"/"ele" → slug `"elemental"`
- arg "enhance"/"enhancement" → slug `"enhancement"`
- unknown → usage error message

Calls `sAltbotMgr->SetBotSpec(...)`. Replies: `"Altbot spec set to: <display> (applies on next combat tick)."`

Add `{"spec", CmdSetSpec}` to `kCommands[]`. Update `CmdHelp`.

---

## Critical files

| File | Action |
|------|--------|
| `src/strategies/RestoShamanStrategy.h/.cpp` | CREATE |
| `sql/002_add_spec_override.sql` | CREATE |
| `src/AltbotCombat.h/.cpp` | MODIFY |
| `src/AltbotAI.h/.cpp` | MODIFY |
| `src/AltbotMgr.h/.cpp` | MODIFY |
| `src/AltbotCommandTable.cpp` | MODIFY |

CMake auto-discovers `.cpp` files — no CMakeLists changes needed.

---

## Verification

1. Apply `sql/002_add_spec_override.sql` to characters DB
2. Build: `.\build.ps1 -Target scripts` — expect clean compile
3. Log in Shaman master → whisper bot "spec resto" → confirm system message feedback
4. Enter combat → check `altbot` debug log for `RestoShamanStrategy cache` line (resolved spell IDs)
5. Verify bot casts heals in priority order via debug log
6. Whisper "spec" → reverts to auto; strategy re-resolves next combat tick
7. Enhancement Shaman alt → bot auto-detects, falls back to generic (no strategy yet)
8. `SELECT spec_override FROM character_altbot` → confirm column populated correctly

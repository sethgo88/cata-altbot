# Talent-Aware Combat Design — TC Fork Research

Answers the three open questions from Phase 3 planning. All findings are from `server-core/`.

---

## Q1: PlayerScript Hook Surface

**File:** `server-core/src/server/game/Scripting/ScriptMgr.h:616–715`

Available combat-relevant hooks on `PlayerScript`:
- `OnSpellCast(Player* player, Spell* spell, bool skipCheck)` — fires on every spell cast

**Missing hooks (not on PlayerScript in this fork):**
- `OnUpdate` — no per-player frame hook
- `OnEnterCombat` / `OnExitCombat` — no combat state change callbacks

**Combat loop design decision:**
Use `WorldScript::OnUpdate(uint32 diff)` to drive the bot tick. Inside that hook, iterate `sAltbotMgr->GetActiveBots()` and call each bot's update. Detect combat state via `player->IsInCombat()` — poll each tick and track previous state in `AltbotAI` to detect transitions.

```cpp
// Approach in AltbotLoader.cpp (WorldScript)
void OnUpdate(uint32 diff) override
{
    sAltbotMgr->UpdateAllBots(diff);
}

// In AltbotAI::Update(uint32 diff)
Player* bot = _session->GetPlayer();
if (!bot) return;

bool inCombat = bot->IsInCombat();
if (inCombat && !_wasInCombat)
    OnEnterCombat();   // internal helper
if (!inCombat && _wasInCombat)
    OnExitCombat();
_wasInCombat = inCombat;
```

---

## Q2: Cata Spell-Rank Resolution

**Files:**
- `server-core/src/server/game/Spells/SpellMgr.h:628–636` — API declarations
- `server-core/src/server/game/Spells/SpellMgr.cpp:284–301` — implementations
- `server-core/src/server/game/Entities/Player/Player.h:249,1620` — PlayerSpellMap

**Key finding:** Cataclysm retains spell rank chains via `SpellChainNode`. Individual spells are unranked from a player perspective, but the chain structure is queryable.

**Available APIs:**
```cpp
sSpellMgr->GetSpellRank(spellId)              // uint8 — 0 if no chain
sSpellMgr->GetFirstSpellInChain(spellId)      // uint32
sSpellMgr->GetLastSpellInChain(spellId)       // uint32
sSpellMgr->GetNextSpellInChain(spellId)       // uint32
sSpellMgr->GetPrevSpellInChain(spellId)       // uint32
sSpellMgr->GetSpellWithRank(spellId, rank)    // uint32
sSpellMgr->GetSpellInfo(spellId)              // SpellInfo const*
```

**Player spell iteration (rule: never hardcode spell IDs):**
```cpp
PlayerSpellMap const& spells = bot->GetSpellMap();
for (auto const& [spellId, playerSpell] : spells)
{
    if (!playerSpell.active || playerSpell.disabled)
        continue;
    SpellInfo const* info = sSpellMgr->GetSpellInfo(spellId);
    if (!info) continue;
    // evaluate info for combat use
}
```

---

## Q3: Player→AltbotAI Accessor

**Files:**
- `server-core/src/server/scripts/Custom/cata-altbot/src/AltbotMgr.h:32–35`
- `server-core/src/server/scripts/Custom/cata-altbot/src/AltbotMgr.cpp:130–147`

**Finding:** No Player member variable needed. The existing `AltbotMgr` already maintains a `masterGuid → vector<AltbotAI>` map.

**Lookup pattern:**
```cpp
AltbotAI* ai = sAltbotMgr->FindBotAI(masterGuid, botGuid);
if (!ai) return;
Player* bot = ai->GetSession()->GetPlayer();
```

For combat purposes, `AltbotAI::Update()` already holds `this` pointer and its own `_session`, so no external lookup is needed inside the bot's own update tick.

---

## Summary: Phase 3 Architecture Constraints

| Constraint | Solution |
|---|---|
| No `PlayerScript::OnEnterCombat` | Poll `player->IsInCombat()` each tick; track `_wasInCombat` in `AltbotAI` |
| No `PlayerScript::OnUpdate` | Drive ticks from `WorldScript::OnUpdate` via `sAltbotMgr->UpdateAllBots(diff)` |
| Spell IDs must not be hardcoded | Iterate `player->GetSpellMap()`, evaluate each via `SpellInfo` |
| Spell ranks exist in chain structs | Use `sSpellMgr->GetLastSpellInChain()` to get highest known rank |
| No `Player.m_botAI` member | Use `sAltbotMgr->FindBotAI()` or `this` pointer within `AltbotAI::Update` |

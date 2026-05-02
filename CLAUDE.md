# cata-altbot — Module Context

## Structure
```
cata-altbot/src/
├── AltbotMgr.h/.cpp      ← singleton, AddAltbot(), session creation
├── AltbotAI.h/.cpp       ← per-bot update loop (Phase 2+)
├── AltbotFollow.cpp      ← follow/movement (Phase 2)
├── AltbotCombat.cpp      ← spell selection, combat assist (Phase 3)
├── AltbotCommands.cpp    ← chat command parsing (Phase 4)
└── AltbotLoader.cpp      ← AddSC_AltbotLoader() registration
```

## Build Integration (how this gets compiled)
1. Junction at `server-core/src/server/scripts/Custom/cata-altbot/` → this directory
2. `CollectSourceFiles` in TC's scripts CMakeLists auto-discovers all .cpp files
3. Include path added: `server-core/src/server/scripts/CMakeLists.txt` includes `Custom/cata-altbot/src`
4. `custom_script_loader.cpp` calls `AddSC_AltbotLoader()`

## Core Patches Made
- `server-core/src/server/game/Server/WorldSession.h` line ~479 — `AltbotLogin(ObjectGuid)` declared
- `server-core/src/server/game/Handlers/CharacterHandler.cpp` line ~766 — `AltbotLogin` implemented

## AltbotLogin Flow
```
AltbotMgr::AddAltbot()
  → new WorldSession(accountId, "", nullptr, SEC_PLAYER, EXPANSION_CATACLYSM, ...)
  → botSession->AltbotLogin(botGuid)         // sets m_playerLoading, fires DB queries
  → sWorld->AddSession(botSession)           // queues session on world thread
  → [async] HandlePlayerLogin(holder)        // player loaded, added to world
```

## Pending — TC fork / private-server code access required

Items in this section can't be resolved from the altbot working directory alone — they need access to the TrinityCore source tree (and in some cases the private server's DBC / world DB). Pick these up in a session on the machine with the TC fork checked out. Each item is tagged with what kind of external access it needs.

### Compile blockers
- **[TC fork]** `RBAC_PERM_COMMAND_GM` — grep TC for the correct constant name before building. RBAC permissions are typically in `server-core/src/server/game/Misc/RBAC.h` (or similar); the actual symbol may be `rbac::RBAC_PERM_COMMAND_GM` or a different prefix on this fork.
- **[TC fork]** Verify `_legacyConnectionModeEnabled` is accessible from `AltbotLogin` in `server-core/src/server/game/Handlers/CharacterHandler.cpp` (line ~766). Member-visibility / friend-declaration check — if it's private and `AltbotLogin` can't see it, we either widen visibility or route around it.

### Design verification
- **[TC fork]** **Talent-aware combat (Phase 3 redesign)** — see `docs/research/talent-aware-combat-design.md`. Three open questions (PlayerScript hook surface, Cata spell-rank resolution, Player→AltbotAI accessor) need answers from the TC source tree before we lock data shapes and start writing. Resolve those before doing any combat-rotation work.

### Data verification (deferred — not blocking module work)
- **[TC fork + DBC]** **Resto Shaman UNVERIFIED items** — 9 spell-detail ambiguities flagged in `docs/specs/resto-shaman.md`. Step-by-step checklist lives at `docs/research/dbc-verification-checklist.md`. Needs `Spell.dbc` access (in-game GM lookup, WDBXEditor, or sqlite-converted DBC). Becomes a real blocker only when `AltbotCombat` references these as compile-time constants OR when level-gating logic needs to decide "talent vs baseline" for those 3 abilities.

## Reference Counterparts (mod-playerbots)
| Our file | Maps from |
|----------|-----------|
| AltbotMgr | PlayerbotMgr |
| AltbotAI | PlayerbotAI |
| AltbotFollow | strategy/actions/FollowMasterAction |
| AltbotCombat | strategy/actions/CastHealAction + DPS equivalents |
| AltbotCommands | PlayerbotCommandHandler + ChatHelper |

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

## Known Pending Compile Issues
- `RBAC_PERM_COMMAND_GM` — grep CPP TC for correct constant name before building
- Verify `_legacyConnectionModeEnabled` is accessible from `AltbotLogin` in CharacterHandler.cpp

## Reference Counterparts (mod-playerbots)
| Our file | Maps from |
|----------|-----------|
| AltbotMgr | PlayerbotMgr |
| AltbotAI | PlayerbotAI |
| AltbotFollow | strategy/actions/FollowMasterAction |
| AltbotCombat | strategy/actions/CastHealAction + DPS equivalents |
| AltbotCommands | PlayerbotCommandHandler + ChatHelper |

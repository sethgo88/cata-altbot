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
- `server-core/src/server/game/DungeonFinding/LFGMgr.h` near `GetTicket` — `GetPendingProposalIdForPlayer(ObjectGuid)` declared (public). Scans `ProposalsStore` so bots can auto-accept LFG proposals via `UpdateProposal`.
- `server-core/src/server/game/DungeonFinding/LFGMgr.cpp` after `GetTicket` impl — implementation of the above.

## AltbotLogin Flow
```
AltbotMgr::AddAltbot()
  → new WorldSession(accountId, "", nullptr, SEC_PLAYER, EXPANSION_CATACLYSM, ...)
  → botSession->AltbotLogin(botGuid)         // sets m_playerLoading, fires DB queries
  → sWorld->AddSession(botSession)           // queues session on world thread
  → [async] HandlePlayerLogin(holder)        // player loaded, added to world
```

## Pending — TC fork / private-server code access required

### Resolved (TC fork now accessible)
- **Resto Shaman DBC verification** — all 9 items resolved via WDBXEditor CSV export + `tools/dbc-extract.py`. Compact reference at `docs/data/spell-ref.csv`. Water Shield / Resurgence mana scaling (base=40) deferred to `spell_proc_event` — not blocking Phase 3.
- **`RBAC_PERM_COMMAND_GM`** — constant confirmed at `server-core/src/server/game/Accounts/RBAC.h:279`, value 371. Name is correct as written.
- **`_legacyConnectionModeEnabled`** — private member of `WorldSession` (`WorldSession.h:1428`); `AltbotLogin` is a `WorldSession` member function so it has full access. No fix needed.
- **Talent-aware combat design** — all 3 open questions answered. See `docs/research/talent-aware-combat-design.md`. Phase 3 work can now begin.
- **Talent / Spec API** — `GetPrimaryTalentTree(uint8 spec)` returns tree ID (Player.h:1578); `GetActiveSpec()` returns active spec index (Player.h:1580). Usage: `uint32 treeId = bot->GetPrimaryTalentTree(bot->GetActiveSpec())`. Phase 4 APIs (PlayerScript hooks, ServerScript::OnPacketReceive, Group::m_targetIcons, Player::LearnTalent) documented in `docs/porting-notes.md` Phase 4 section.

## Reference Counterparts (mod-playerbots)
| Our file | Maps from |
|----------|-----------|
| AltbotMgr | PlayerbotMgr |
| AltbotAI | PlayerbotAI |
| AltbotFollow | strategy/actions/FollowMasterAction |
| AltbotCombat | strategy/actions/CastHealAction + DPS equivalents |
| AltbotCommands | PlayerbotCommandHandler + ChatHelper |

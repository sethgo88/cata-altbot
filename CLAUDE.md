# cata-altbot Module

This is the altbot module being ported from mod-playerbots (AzerothCore 3.3.5) to CPP TrinityCore 4.3.4.

## Module Structure

```
cata-altbot/
├── src/
│   ├── AltbotMgr.h/.cpp        ← singleton manager, AddAltbot(), session creation
│   ├── AltbotAI.h/.cpp         ← per-bot update loop, master detection
│   ├── AltbotFollow.cpp        ← follow/movement logic
│   ├── AltbotCombat.cpp        ← spell selection, combat assist
│   ├── AltbotCommands.cpp      ← .altbot GM command, chat command parsing
│   └── AltbotLoader.cpp        ← AddSC_ registration entry point
├── CMakeLists.txt
└── README.md
```

## Integration Point

Scripts register via `AddSC_AltbotLoader()` called from:
`server-core/src/server/scripts/Custom/CMakeLists.txt`

## Reference Counterparts (mod-playerbots)

| Our file | Maps from |
|----------|-----------|
| AltbotMgr | PlayerbotMgr |
| AltbotAI | PlayerbotAI |
| AltbotFollow | strategy/actions/FollowMasterAction |
| AltbotCombat | strategy/actions/CastHealAction + DPS equivalents |
| AltbotCommands | PlayerbotCommandHandler + ChatHelper |

## Key Implementation Rules

- Never hardcode spell IDs — use `player->GetSpellMap()` iteration
- Bot role (healer/dps/tank) is inferred from `GetPrimaryTalentTree(GetActiveSpec())`
- Teleport bot to master if distance > 100 yards (not pathfinding — instant teleport)
- All bot commands go through `AltbotCommands.cpp` — no logic in command handlers

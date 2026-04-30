# cata-altbot

An altbot module for CPP TrinityCore (World of Warcraft 4.3.4).

An altbot loads one of your existing characters server-side as an AI companion that follows you, assists in combat, and responds to simple commands — like multiboxing without a second client.

## Status

Work in progress. See [porting-notes.md](../docs/porting-notes.md) for current progress.

## Scope

**In scope:** Follow, combat assist (heal/dps), basic commands, persistence across restarts.
**Out of scope:** Random bot generation, AH bots, BG participation, gear/talent assignment.

## Commands

| Command | Effect |
|---------|--------|
| `.altbot add <name>` | Load your character `<name>` as an altbot |
| `.altbot remove <name>` | Remove active altbot |
| `follow` (party chat) | Bot resumes following you |
| `stay` (party chat) | Bot stops moving |
| `attack` (party chat) | Bot attacks your target |
| `heal` (party chat) | Bot prioritizes healing |

## Building

See `../server-build/` for build configuration.

The module integrates into CPP TrinityCore as a custom script:
1. Copy/symlink this directory to `server-core/src/server/scripts/Custom/cata-altbot/`
2. Register `AltbotLoader.cpp` in the Custom CMakeLists
3. Build TrinityCore normally

## Reference

Ported from [mod-playerbots](https://github.com/mod-playerbots/mod-playerbots) (AzerothCore 3.3.5).
Target core: [CPP TrinityCore](https://github.com/The-Cataclysm-Preservation-Project/TrinityCore) 4.3.4.

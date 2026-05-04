# Build System

Technical reference for the CMake build configuration.

## Directory Layout

```
C:\Cataclysm\
├── server-core\            ← CMake source root (-S)
├── server-build\
│   ├── build\              ← CMake build dir (-B), gitignored
│   ├── install\            ← cmake --install output, gitignored
│   ├── build.ps1
│   ├── setup-dev.ps1
│   └── local.config.ps1    ← machine-specific, gitignored
└── cata-altbot\            ← module source (junctioned into server-core)
```

## Prerequisites

| Tool | Version | Install |
|------|---------|---------|
| Visual Studio | 2019 or 2022 | "Desktop development with C++" workload required |
| CMake | 3.20+ | cmake.org or `choco install cmake` — add to PATH |
| MySQL | 8.0 | mysql.com or `choco install mysql` |
| OpenSSL | 3.x Win64 | slproweb.com or `choco install openssl` |
| Boost | 1.74+ | `choco install boost-msvc-14.3` or set BOOST_ROOT |
| Git | any | git-scm.com |

## First-Time Setup

```powershell
# Run PowerShell as Administrator
cd C:\Cataclysm\server-build
.\setup-dev.ps1
# Edit local.config.ps1 with your paths
notepad local.config.ps1
```

## Build Commands

```powershell
cd C:\Cataclysm\server-build

# First build (or after changing CMakeLists.txt)
.\build.ps1 -Configure

# Incremental rebuild — only recompiles changed files
.\build.ps1

# Rebuild altbot module only — fastest iteration during development
.\build.ps1 -Target scripts

# Build + copy binaries to install\
.\build.ps1 -Install

# Debug build
.\build.ps1 -Config Debug -Configure

# Full clean rebuild
.\build.ps1 -Clean -Configure
```

### When to use each

| Scenario | Command |
|----------|---------|
| Edited a `.cpp` file in `cata-altbot/src/` | `.\build.ps1 -Target scripts` |
| Edited a `.h` header | `.\build.ps1 -Target scripts` |
| Added a new `.cpp` file to the module | `.\build.ps1 -Configure` (CMake must rescan glob) |
| Changed any `CMakeLists.txt` | `.\build.ps1 -Configure` |
| Ready to run/test the server | `.\build.ps1` or `.\build.ps1 -Install` |

## CMake Flags Explained

| Flag | Our Value | Why |
|------|-----------|-----|
| `SCRIPTS=static` | static | Links all scripts into worldserver.exe, no DLL loading needed |
| `TOOLS=0` | 0 | Skips map/vmap extractors — not needed unless regenerating mmaps |
| `WITH_WARNINGS=0` | 0 | TC has many warnings unrelated to our work; silence them |
| `USE_SCRIPTPCH=1` | 1 | Precompiled headers — makes incremental builds much faster |

## How cata-altbot Gets Compiled

1. `server-core/src/server/scripts/Custom/cata-altbot/` is a junction to `../cata-altbot/`
2. `GetScriptModuleList()` in CMake globs all subdirs of `src/server/scripts/` → picks up `Custom`
3. `CollectSourceFiles(Custom/)` recurses into the junction → finds our `.cpp` files
4. `scripts/CMakeLists.txt` includes `Custom/cata-altbot/src` for header resolution
5. `custom_script_loader.cpp` calls `AddSC_AltbotLoader()` at server startup

## Core Patches Tracked

These files in `server-core/` were modified — track them if you ever need to re-apply after a git pull:

| File | Change |
|------|--------|
| `src/server/game/Server/WorldSession.h` | Added `AltbotLogin(ObjectGuid)` declaration after line 478 |
| `src/server/game/Handlers/CharacterHandler.cpp` | Added `AltbotLogin` implementation before `HandleContinuePlayerLogin` |
| `src/server/scripts/CMakeLists.txt` | Added `Custom/cata-altbot/src` to `target_include_directories` |
| `dep/fmt/CMakeLists.txt` | Added `include(CheckSymbolExists)` before first `check_symbol_exists` call (missing include, CMake bug) |
| `src/server/game/Server/WorldSession.cpp` | Null-check `m_Socket[CONNECTION_TYPE_REALM]` before `CloseSocket()` in idle check; altbot sessions return true from Update() instead of false when socket is null (keeps session alive for async DB load) |
| `src/server/game/Server/WorldSession.h` | Added `_isAltbot` / `_altbotGuid` members and `IsAltbot()` / `GetAltbotGuid()` accessors for altbot session isolation |
| `src/server/game/Handlers/CharacterHandler.cpp` | `AltbotLogin()` now sets `_isAltbot = true` and `_altbotGuid = guid` before triggering login flow |
| `src/server/game/World/World.h` | Added `SessionMap m_altbotSessions` — separate bot session map keyed by character guid low |
| `src/server/game/World/World.cpp` | `AddSession_()` bypasses account-keyed kick logic and skips InitializeSession() for altbot sessions; `UpdateSessions()` iterates `m_altbotSessions` |

## After a git pull on server-core

If a TC upstream pull overwrites the patched files:
```bash
# Check if patches still apply
grep -n "AltbotLogin" server-core/src/server/game/Server/WorldSession.h
grep -n "AltbotLogin" server-core/src/server/game/Handlers/CharacterHandler.cpp
grep -n "cata-altbot" server-core/src/server/scripts/CMakeLists.txt
```
If any are missing, re-apply the patches from `docs/porting-notes.md`.

## IDE Setup (Visual Studio)

1. Copy `server-build/CMakePresets.json` to `server-core/CMakePresets.json`
2. Open Visual Studio → "Open a local folder" → select `server-core/`
3. VS will auto-detect the presets — select "release" or "debug"
4. You still need `local.config.ps1` values set as CMake cache overrides in VS settings

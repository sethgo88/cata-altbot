# How-To Guide: Cataclysm Private Server + Altbot

Step-by-step reference so you can work independently. Updated as the project evolves.

---

## 1. First-Time Machine Setup

### Install prerequisites (one time)
```powershell
# Using Chocolatey (install from chocolatey.org first if needed)
choco install cmake git mysql openssl boost-msvc-14.3 -y
# Also install Visual Studio 2022 Community with "Desktop development with C++" workload
```

### Clone repos (already done)
```
C:\Cataclysm\server-core\         ← CPP TrinityCore
C:\Cataclysm\server-refs\mod-playerbots\
C:\Cataclysm\server-refs\azerothcore-wotlk\
C:\Cataclysm\cata-altbot\         ← our module
C:\Cataclysm\server-build\        ← build scripts
```

### Run dev setup (one time, as Admin)
```powershell
cd C:\Cataclysm\server-build
.\setup-dev.ps1
notepad local.config.ps1    # fill in your MySQL and OpenSSL paths
```

---

## 2. Building the Server

### First build
```powershell
cd C:\Cataclysm\server-build
.\build.ps1 -Configure       # takes ~20-40 min first time
```

### Incremental rebuild (after code changes)
```powershell
.\build.ps1                  # usually 1-5 min
```

### Install binaries
```powershell
.\build.ps1 -Install         # copies built .exe files to server-build\install\
```

---

## 3. Setting Up the Database (First Time)

TrinityCore requires three databases: `auth`, `characters`, `world`.

### Create databases
```sql
-- Run in MySQL as root
CREATE DATABASE `auth` CHARACTER SET utf8mb4;
CREATE DATABASE `characters` CHARACTER SET utf8mb4;
CREATE DATABASE `world` CHARACTER SET utf8mb4;
CREATE USER 'trinity'@'localhost' IDENTIFIED BY 'yourpassword';
GRANT ALL ON `auth`.* TO 'trinity'@'localhost';
GRANT ALL ON `characters`.* TO 'trinity'@'localhost';
GRANT ALL ON `world`.* TO 'trinity'@'localhost';
```

### Import base data
```powershell
# Auth + characters schemas
mysql -u trinity -p auth      < server-core\sql\base\auth_database.sql
mysql -u trinity -p characters < server-core\sql\base\characters_database.sql

# World data — get the TDB (Trinity Database) for 4.3.4 from the CPP TC releases
# Import the TDB full SQL file:
mysql -u trinity -p world < TDB_full_cata_435.XXXXX.sql
```

### Apply updates
```powershell
# Run all pending update SQL files
# TC auto-applies these at startup if configured, or run manually:
mysql -u trinity -p auth      < server-core\sql\updates\auth\*.sql
mysql -u trinity -p characters < server-core\sql\updates\characters\*.sql
```

---

## 4. Configuring the Server

### Copy default configs
```powershell
cd server-build\install
cp worldserver.conf.dist  worldserver.conf
cp authserver.conf.dist   authserver.conf   # or bnetserver.conf for 4.3.4
```

### Key settings to change in worldserver.conf
```ini
DataDir = "C:/Cataclysm/server-build/install/Data"
LoginDatabaseInfo     = "127.0.0.1;3306;trinity;yourpassword;auth"
WorldDatabaseInfo     = "127.0.0.1;3306;trinity;yourpassword;world"
CharacterDatabaseInfo = "127.0.0.1;3306;trinity;yourpassword;characters"
```

---

## 5. Running the Server

```powershell
cd server-build\install

# Terminal 1 — auth server
.\authserver.exe     # or bnetserver.exe for 4.3.4

# Terminal 2 — world server
.\worldserver.exe
```

The worldserver console accepts GM commands directly (prefix with a dot for in-game commands).

---

## 6. Working on cata-altbot

### Daily workflow
```
1. Edit files in C:\Cataclysm\cata-altbot\src\
2. cd C:\Cataclysm\server-build
   .\build.ps1 -Target scripts   # fast: only recompiles altbot module
   .\build.ps1                   # full rebuild (use when testing end-to-end)
3. Test in-game with .altbot add <charname>
4. git commit and push from C:\Cataclysm\cata-altbot\
```

### When to use -Configure vs incremental

| Scenario | Command |
|----------|---------|
| Edited existing `.cpp` or `.h` | `.\build.ps1 -Target scripts` |
| Added a new `.cpp` file | `.\build.ps1 -Configure` |
| Changed any `CMakeLists.txt` | `.\build.ps1 -Configure` |
| Ready to run the server | `.\build.ps1 -Install` |

### Adding a new altbot feature
1. Read the equivalent in `server-refs/mod-playerbots/src/` first
2. Check `docs/porting-notes.md` for known API differences
3. Grep for the CPP TC equivalent API before writing code
4. Add new findings to `docs/porting-notes.md`

### After editing CMakeLists.txt or adding new .cpp files
```powershell
.\build.ps1 -Configure    # re-runs cmake to pick up new sources
```

### Fixing a compile error referencing a TC function
```powershell
# Grep the TC source for the real function name
grep -rn "FunctionName" C:/Cataclysm/server-core/src/server/game/
```

---

## 7. Connecting with the WoW Client

1. Client lives at `C:\Cataclysm\client\`
2. Edit `client\Data\enUS\realmlist.wtf`:
   ```
   set realmlist 127.0.0.1
   ```
3. Launch `client\WoW.exe`
4. Create an account via the worldserver console:
   ```
   account create myusername mypassword
   account set gmlevel myusername 3 -1
   ```

---

## 8. Git Workflow

### cata-altbot repo (module code)
```powershell
cd C:\Cataclysm\cata-altbot
git add src\AltbotMgr.cpp src\AltbotMgr.h  # stage specific files
git commit -m "Phase 2: implement follow logic"
git push
```

### server-build repo (build scripts only)
```powershell
cd C:\Cataclysm\server-build
git add build.ps1 setup-dev.ps1  # never add build/, install/, or local.config.ps1
git commit -m "Update build script"
git push
```

### Tracking server-core patches
The three patches to `server-core/` are NOT in a repo (server-core is the upstream CPP TC repo).
After any `git pull` on server-core, re-check they're still in place:
```powershell
grep -n "AltbotLogin" C:/Cataclysm/server-core/src/server/game/Server/WorldSession.h
```
If missing, ask Claude to re-apply from `docs/porting-notes.md`.

---

## 9. Common Errors

| Error | Cause | Fix |
|-------|-------|-----|
| `Cannot open include file: 'AltbotMgr.h'` | Include path not set | Verify CMakeLists.txt patch in server-core/src/server/scripts/ |
| `'RBAC_PERM_COMMAND_GM': undeclared identifier` | Wrong RBAC constant name | Grep TC source: `grep -rn "RBAC_PERM_COMMAND" server-core/src/server/game/RBAC/` |
| `WorldSession constructor: too many arguments` | Constructor signature changed | Check WorldSession.h line 478 and match our call in AltbotMgr.cpp |
| CMake can't find MySQL | Wrong path in local.config.ps1 | Fix `$MYSQL_INCLUDE` and `$MYSQL_LIBRARY` paths |
| Junction creation fails | Not running as Admin | Run PowerShell as Administrator |
| Bot doesn't appear in world | Login query holder failed | Check worldserver.log for "AltbotMgr" error lines |

# Session-lifecycle safety nets — deferred work

## Context

Found while investigating two reported bugs (worldserver crash on `.guild
invite <bot>`, and bot bags appearing empty when the user logs into the bot
character directly via the WoW client).

The bot's `WorldSession` is constructed in `AltbotMgr::SpawnBot` with
`m_Socket[CONNECTION_TYPE_REALM] = nullptr`. Several TC paths
(`WorldSession::KickPlayer`, `LogoutPlayer`'s `RemoveFromGroup` branch, the
`World::AddSession_` collision handler) are gated on `if (m_Socket[i])` and
become no-ops or partial-ops for null-socket sessions. The bug investigation
turned up four candidate safety nets; (1) and (2) are now implemented in the
module. (3) and (4) need information from the actual server-core build before
they can be written correctly.

## (1) Look up bot Player by GUID — DONE

`AltbotAI::Update` now uses `ObjectAccessor::FindConnectedPlayer(_botGuid)`
instead of trusting the cached `_botSession*`. On null lookup, sets
`_dead = true` and returns. `AltbotMgr::Update` reaps dead AIs at end of tick.

## (2) `PlayerScript::OnLogout` hook — DONE

`altbot_playerscript` in `AltbotLoader.cpp` calls
`sAltbotMgr->HandlePlayerLogout(player->GetGUID())`. Two cases:
- player is a registered master → tear down each bot via `LogoutPlayer(true)`,
  then erase the master entry from `_activeBots`.
- player is a registered bot → drop its AI from its master's list.

`HandlePlayerLogout` snapshots bot guids before iteration because
`LogoutPlayer` re-enters `OnLogout` synchronously (Case A → Case B recursion).

## (3) `WorldScript::OnShutdownInitiate` — TODO, needs info

**Goal:** save every bot's inventory/money before TC's `KickAll` runs and
session destruction starts going through the no-socket-session destructor
path. Belt-and-suspenders against (2) — if the destructor's `LogoutPlayer`
ever silently fails for null-socket bots, this catches it.

**What I need from the server-core:** the `WorldScript` class declaration in
`src/server/game/Scripting/ScriptMgr.h` (or wherever it lives in the user's
fork). Looking for which lifecycle hooks are virtual:

- `OnStartup()`
- `OnShutdownInitiate(ShutdownExitCode, ShutdownMask)` — ideal hook for us
- `OnShutdown()` — fires too late (after `KickAll`); not useful here
- `OnConfigLoad(bool reload)`
- `OnUpdate(uint32 diff)`

The exact hook name + signature varies by TC version. Confirm what's in the
fork before writing the override.

**Implementation when ready:** in `altbot_worldscript`, override the right
hook and call something like `AltbotMgr::TearDownAllBots()` which walks every
master entry in `_activeBots` and runs the same logic as
`HandlePlayerLogout` Case A on each. Reusing the existing function is fine —
just iterate keys and call it.

## (4) Same-account session-collision check — TODO, needs info

**Goal:** in `AltbotMgr::SpawnBot`, refuse to add a bot whose `accountId`
already has an active session that we'd evict. Originally framed as a
`sWorld->FindSession(botAccountId) != nullptr` check, but that is **wrong**
for this user's setup.

**Why:** the user runs *all* their bots on the same account as the master
(single-account model: master Warriorone + altbot Mageone/Hunterone/
Warlockone/Shamanone all under account M). In stock TC, `m_sessions` is keyed
by accountId (`World.cpp:369`) and `AddSession_` would evict the existing
session for that account. The fact that the user's setup works means their
TC fork has been patched to handle multi-session-per-account specifically for
bot sessions. A naive `FindSession` check would refuse every bot add.

**What I need from the server-core:**

1. `World::AddSession_` body, especially around the
   `RemoveSession(accountId)` / `delete old->second` lines (stock TC:
   `World.cpp:336-400`).
2. `WorldSession::AltbotLogin` body (the patch declared at
   `WorldSession.h:479` per CLAUDE.md, implemented in
   `CharacterHandler.cpp:766`). Either it stuffs a fake socket into
   `m_Socket[CONNECTION_TYPE_REALM]` so the standard collision logic works,
   or it bypasses the eviction path entirely.
3. The `m_sessions` declaration in `World.h` — is it still
   `std::unordered_map<uint32, WorldSession*>` keyed by accountId, or has it
   been re-keyed by GUID / made multi-valued?

**Implementation when ready:** the right check depends on what (1)–(3)
reveal:

- If the fork keys sessions by character GUID (multi-session-per-account
  natively): check `sWorld->FindSession*ByGuid*(botGuid)` — only conflicts if
  *that specific character* already has a session.
- If the fork keeps account-keyed sessions but stashes bot sessions in a
  side-map: check that side-map's collision rules.
- If the fork lets `AddSession_` skip the eviction path for null-socket
  sessions: there's nothing to add at the AltbotMgr layer — collisions can't
  happen for bots. (4) becomes a no-op and we can close it out.

**Until we have this info, do not add `(4)`.** A wrong check here would break
the user's workflow on the very first `.altbot login`.

## Other follow-ups surfaced by the same investigation

- **Bag-emptying root cause is still open.** With (1) + (2) in place, the
  use-after-free path is closed, but the user reports items sit in memory
  (visible via `AltbotInventory::ListBags`) yet never reach DB. Suspected
  root cause: `Player::Update`'s periodic `m_nextSave` save isn't firing for
  bots, and `LogoutPlayer`'s save path may also be a partial no-op for
  null-socket sessions. Plan: add explicit `bot->SaveToDB()` calls after
  loot pickups and on a periodic timer in `AltbotAI::Update`, plus on the
  master-logout path inside `HandlePlayerLogout`.

- **Guild crash on `.guild invite <bot>` is open.** Need the actual
  worldserver crash log / stack trace to pinpoint. Likely candidates:
  `Guild::SendLoginInfo` calling `LearnSpell` on a null-socket bot,
  `_BroadcastEvent(GE_JOINED)` broadcasting to the bot itself,
  `Guild::HandleMemberLogout` referencing a freed bot session, or guild
  member rows pointing at a partially-loaded bot character that crashes
  `Guild::Member::LoadFromDB` on next worldserver start.

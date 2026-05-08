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

## (3) `WorldScript::OnShutdownInitiate` — DONE

`altbot_worldscript` in `AltbotLoader.cpp` overrides
`OnShutdownInitiate(ShutdownExitCode, ShutdownMask)` (primary save point —
fires before `KickAll` and `InstanceMap::UnloadAll`) and also keeps an
`OnShutdown()` override as a late backup. Both call
`AltbotMgr::ShutdownAllBots`, which is idempotent because it clears
`_activeBots` after teardown.

`ShutdownAllBots` snapshots bot guids first (LogoutPlayer re-enters
`HandlePlayerLogout` synchronously and would otherwise invalidate the
iteration) and looks up each bot's `Player*` via `ObjectAccessor::
FindConnectedPlayer` rather than trusting cached session pointers.

## (4) Same-account session-collision check — RESOLVED, NOT NEEDED

The fork has already solved this at the core. `World::AddSession_` is
patched (in `src/server/game/World/World.cpp`) to short-circuit altbot
sessions into a separate map:

```cpp
if (s->IsAltbot())
{
    m_altbotSessions[s->GetAltbotGuid().GetCounter()] = s;
    UpdateMaxSessionCounters();
    return;
}
```

`m_altbotSessions` is keyed by character guid (low 32) instead of accountId,
so adding a bot session never evicts the master's session even when both
share an account. `WorldSession::AltbotLogin` sets the `_isAltbot` flag and
the `_altbotGuid` that this map keys on. There is no collision to defend
against at the `AltbotMgr` layer. Close the issue.

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

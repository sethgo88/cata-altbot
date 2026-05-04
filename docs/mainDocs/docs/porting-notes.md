# Porting Notes: mod-playerbots → CPP TrinityCore 4.3.4

Ongoing log of API differences found and decisions made. Update this as new diffs are discovered.

---

## Phase 1 Findings (RESOLVED)

### WorldSession Constructor
**Status:** Resolved.

AC 3.3.5 call (mod-playerbots/src/PlayerbotMgr.cpp ~line 202):
```cpp
new WorldSession(botAccountId, "", 0x0, nullptr, SEC_PLAYER,
                 EXPANSION_WRATH_OF_THE_LICH_KING, time_t(0),
                 sWorld->GetDefaultDbcLocale(), 0, false, false, 0, true);
// 13 arguments
```

CPP TC 4.3.4 constructor (WorldSession.h line 478):
```cpp
WorldSession(uint32 id, std::string&& name, std::shared_ptr<WorldSocket> sock,
             AccountTypes sec, uint8 expansion, time_t mute_time,
             LocaleConstant locale, uint32 recruiter, bool isARecruiter);
// 9 parameters
```

**Diff:** CPP TC dropped the BattlenetAccountId (3rd arg) and 4 trailing AC-specific flags.
**Our call:**
```cpp
new WorldSession(botAccountId, "", nullptr, SEC_PLAYER, EXPANSION_CATACLYSM,
                 time_t(0), sWorld->GetDefaultDbcLocale(), 0, false);
```
`EXPANSION_CATACLYSM = 3` defined in SharedDefines.h line 62.

---

### HandlePlayerLoginFromDB — Does Not Exist
**Status:** Resolved with core patch.

mod-playerbots used `WorldSession::HandlePlayerLoginFromDB(holder)` which doesn't exist in CPP TC.

**Solution:** Added `WorldSession::AltbotLogin(ObjectGuid guid)` to CPP TC core:
- Declaration: `server-core/src/server/game/Server/WorldSession.h` (after line 478)
- Implementation: `server-core/src/server/game/Handlers/CharacterHandler.cpp` (before HandleContinuePlayerLogin)

Implementation sets `m_playerLoading = guid`, `_legacyConnectionModeEnabled = true`, then calls `HandleContinuePlayerLogin()` which fires the LoginQueryHolder DB queries and eventually calls `HandlePlayerLogin(holder)`.

---

### LoginQueryHolder — Private Class
**Status:** Informational.

`LoginQueryHolder` is defined as a local class inside `CharacterHandler.cpp` — not accessible from outside. This is why we route through `AltbotLogin()` on WorldSession rather than constructing the holder ourselves.

---

### Character Lookup APIs
**Status:** Resolved.

```cpp
// Get GUID by character name
ObjectGuid guid = sCharacterCache->GetCharacterGuidByName(name);

// Get account ID by GUID
uint32 accountId = sCharacterCache->GetCharacterAccountIdByGuid(guid);

// sCharacterCache macro defined in CharacterCache.h line 70:
// #define sCharacterCache CharacterCache::instance()
```

---

### Build System — Custom Script Discovery
**Status:** Resolved.

TC's `GetScriptModuleList()` globs all subdirectories of `src/server/scripts/` — `Custom/` is auto-included with no CMakeLists.txt required. `CollectSourceFiles` recurses into junctions on Windows.

**Fix needed:** `src/server/scripts/CMakeLists.txt` `target_include_directories` patched to add `Custom/cata-altbot/src` so `#include "AltbotMgr.h"` resolves.

---

### Session Isolation (AltbotMgr — null socket, account collision)
**Status:** Resolved with core patches.

Three bugs found and fixed when running `.altbot add` for the first time:

**1. Null socket crash in WorldSession::Update()**
The idle-connection check at line ~349 dereferenced `m_Socket[CONNECTION_TYPE_REALM]` without a null guard. Altbot sessions have no socket.
Fix: added `&& m_Socket[CONNECTION_TYPE_REALM]` to the condition.

**2. Master player kicked on bot login**
`World::AddSession_()` uses account ID as the session map key. Both master and bot are on the same account, so adding the bot session kicked the master.
Fix: added `m_altbotSessions` (separate map keyed by character GUID low) to `World`. Altbot sessions bypass `AddSession_()` normal flow entirely.

**3. Bot session destroyed before character loaded**
`WorldSession::Update()` returns `false` (causing session deletion) when `m_Socket[CONNECTION_TYPE_REALM]` is null. The async DB query from `HandleContinuePlayerLogin()` hadn't completed yet on the first tick.
Fix: altbot sessions return `true` from `Update()` even with a null socket.

**4. InitializeSession() unnecessary for bots**
`InitializeSession()` sends AUTH_OK and other client handshake packets — meaningless for a bot. Removed from the altbot branch of `AddSession_()`. Character load is initiated entirely by `AltbotLogin()` → `HandleContinuePlayerLogin()`.

**Key insight:** Normal login order is InitializeSession → client sends CMSG_PLAYER_LOGIN → HandleContinuePlayerLogin. For altbots we skip the first and last steps — AltbotLogin() calls HandleContinuePlayerLogin() directly, bypassing the client round-trip entirely.

---

## Phase 2 — Pending Research

### SMSG_TIME_SYNC_REQ — bot sessions receive but never respond
**Status:** Informational — not causing disconnects in testing (5+ min confirmed stable).

The server sends periodic `SMSG_TIME_SYNC_REQ` to all players. A real client replies with `CMSG_TIME_SYNC_RESP`. The bot session never replies. Monitor for timeouts in longer sessions; if needed, auto-respond in the bot's Update() hook.

### Follow / Movement API
**Status:** TODO — research before writing AltbotFollow.cpp.

Expected (from mod-playerbots):
```cpp
bot->GetMotionMaster()->MoveFollow(master, 2.0f, M_PI);
```
Verify this API exists in CPP TC:
```bash
grep -rn "MoveFollow" server-core/src/server/game/Movement/
```

### Teleport to master (if distance > 100 yards)
Expected:
```cpp
bot->TeleportTo(master->GetMapId(), master->GetPositionX(), master->GetPositionY(),
                master->GetPositionZ(), master->GetOrientation());
```

---

## Phase 3 — Pending Research

### Talent / Spec API (4.3.4)
**Status:** TODO.

WotLK used `GetTalentPoints(tree)`. Cata uses a single specialization.
```bash
grep -rn "GetPrimaryTalentTree\|GetActiveSpec\|GetSpecialization" server-core/src/server/game/Entities/Player/
```

### Spell Selection (no hardcoded IDs)
**Decision:** Never hardcode spell IDs. 3.3.5 IDs are wrong for 4.3.4.
Use `player->GetSpellMap()` iteration, filter by spell school.


### Phase 3 resolved findings (from server-core/ search)

**RBAC header:** `server-core/src/server/game/Accounts/RBAC.h` (not `Misc/RBAC.h`). Constant `RBAC_PERM_COMMAND_GM = 371` is correct.

**WorldSession member access:** `_legacyConnectionModeEnabled` is private in `WorldSession.h:1428`. No issue — `AltbotLogin` is a `WorldSession` member function with full private access.

**Combat hook gap:** `PlayerScript` has no `OnUpdate`, `OnEnterCombat`, or `OnExitCombat`. Use `WorldScript::OnUpdate` + `player->IsInCombat()` polling. Full design in `docs/research/talent-aware-combat-design.md`.

**Spell rank chains:** Retained in `SpellChainNode`. Use `sSpellMgr->GetLastSpellInChain(id)` to get highest-rank version of a spell. Player spells iterated via `player->GetSpellMap()`.

---

## Unit / Player Method Renames

### `getPowerType()` → `GetPowerType()`
**Status:** Resolved.

AC 3.3.5 / mod-playerbots uses lowercase-g `getPowerType()`. CPP TC 4.3.4 exposes the same data as `GetPowerType()` (capital G) defined on `Unit`:
```cpp
// Unit.h ~line 803
Powers GetPowerType() const { return Powers(GetByteValue(UNIT_FIELD_BYTES_0, UNIT_BYTES_0_OFFSET_POWER_TYPE)); }
```

### `GetAttackTime()` → `GetBaseAttackTime()`
**Status:** Resolved.

AC 3.3.5 uses `unit->GetAttackTime(WeaponAttackType)`. CPP TC 4.3.4 renamed this to `GetBaseAttackTime(WeaponAttackType)`:
```cpp
// Unit.h ~line 820
uint32 GetBaseAttackTime(WeaponAttackType att) const;
```
Note: `getAttackTimer()` (lowercase, no "Base") still exists but returns the *remaining* countdown to the next swing, not the base speed. Use `GetBaseAttackTime()` when you want the weapon speed in ms.

---

## Phase 4 — Pending Research

### PlayerScript Hook Surface
**Status:** Resolved.

Available hooks relevant to altbot (from `server-core/src/server/game/Scripting/ScriptMgr.h:616`):
- `OnSpellCast(Player*, Spell*, bool)` — use for mount detection
- `OnQuestStatusChange(Player*, uint32)` — use for quest accept/reward; filter by `QuestStatus` enum value

**Missing hooks (no equivalent in CPP TC):**
- `OnLootRoll` — no hook; if needed, poll loot state
- `OnQuestAccept` / `OnQuestReward` — use `OnQuestStatusChange` instead
- `OnAuraApply` — no hook; poll `player->HasAura(id)` or use `OnSpellCast`

---

### ServerScript::OnPacketReceive
**Status:** Resolved.

`server-core/src/server/game/Scripting/ScriptMgr.h:242`:
```cpp
virtual void OnPacketReceive(WorldSession* /*session*/, WorldPacket& /*packet*/) { }
```
Subclass `ServerScript`, override `OnPacketReceive`, filter on `packet.GetOpcode()`. Use for intercepting `CMSG_MESSAGECHAT` addon-channel packets in Phase 4.

---

### Group::m_targetIcons (skull-mark assist)
**Status:** Resolved.

No `GetTargetIcons()` accessor exists. Direct public member access:
```cpp
// server-core/src/server/game/Groups/Group.h:424
ObjectGuid m_targetIcons[TARGETICONCOUNT];  // public member

// Usage:
ObjectGuid skullTarget = group->m_targetIcons[RAID_TARGET_SKULL]; // index 7
```

---

### Player::LearnTalent Signature
**Status:** Resolved.

`server-core/src/server/game/Entities/Player/Player.h:1590`:
```cpp
bool LearnTalent(uint32 talentId, uint32 talentRank);
```
`talentId` = `Talent.dbc` row ID. `talentRank` = 0-based rank (0 = rank 1).

---

### Talent / Spec API (4.3.4)
**Status:** Resolved.

`server-core/src/server/game/Entities/Player/Player.h`:
```cpp
uint32 GetPrimaryTalentTree(uint8 spec) const;  // line 1578 — returns Talent.dbc tree ID
uint8  GetActiveSpec() const;                    // line 1580 — returns 0 or 1
uint8  GetSpecsCount() const;                    // line 1582 — returns 1 or 2 (dual-spec)
```

Usage pattern:
```cpp
uint32 treeId = bot->GetPrimaryTalentTree(bot->GetActiveSpec());
```

WotLK functions that do NOT exist in CPP TC 4.3.4: `GetSpecialization()`, `GetTalentSpecialization()`, `GetActiveTalentBranchSpec()`.

---

### Addon-Message Size Cap
**Status:** Partially resolved — not blocking.

`MaxSecureAddons = 35` (WorldSession.h:1402) = max registered addon prefixes, not payload length. Actual `CHAT_MSG_ADDON` per-message cap not confirmed from source; likely standard 255-char WoW limit. Confirm before implementing Phase 4 chunking logic.

---

## Phase 5 — Strategy Cast Pipeline

Conventions for the per-spec rotation tick (`{Spec}Strategy::Update`),
discovered while debugging silent rotation failures (Frost Mage spamming
Frostfire Bolt, warlock/hunter cascades of `SpellCastResult=69`). All four
existing strategies (Frost Mage, Aff Warlock, MM Hunter, Resto Shaman)
follow these. See `cata-altbot/CLAUDE.md` "Cast pipeline gotchas" for the
runnable patterns; this section records the underlying API findings.

### Passive talent / proc aura name collision
**Status:** Resolved.

Several talent passives share `SpellName` with the proc aura they grant.
Walking `bot->GetAppliedAuras()` matching by name returns the talent
(perpetual passive) instead of the proc (consumable buff), making proc
detection read "always up."

Known pairs (mage):
- `Fingers of Frost`: 44544 talent / 74396 proc
- `Brain Freeze`:    44546 talent / 57761 proc

Filter using `SpellInfo::IsPassive()` (SpellInfo.cpp:1354-1356, returns
`HasAttribute(SPELL_ATTR0_PASSIVE)`) when walking auras by name. Same
filter is applied at cache-resolution time in
`StrategyUtil::FindSpellByFamilyName` to handle the mirrored cast-vs-passive
case in the spellbook (e.g. Molten Armor 30482 cast vs 34913 on-attacker
fire-damage trigger, both free toggles in Cata so the existing two-pass
"prefer castable" filter can't disambiguate).

### `Unit::CastSpell` returns `SpellCastResult` — propagate it
**Status:** Resolved.

`WorldObject::CastSpell` (Object.cpp:2944, signature in Object.h:513)
returns `SpellCastResult`. Discarding it makes failures invisible — the
rotation tier chain stops descending as if the cast fired, when actually
it was rejected at `Spell::prepare` time.

Convention: every cast in any strategy goes through
`StrategyUtil::CastWithLog(bot, target, spellId, SPEC_LABEL)`
(StrategyUtil.h/.cpp), which issues `bot->CastSpell` and logs on
non-`SPELL_CAST_OK`. Each strategy file declares
`constexpr char const* SPEC_LABEL = "FrostMage"` (or AffWarlock /
MmHunter / RestoShaman) in its anonymous namespace for the log prefix.
`TryCast` is a thin wrapper that adds a cooldown precheck and returns
bool; tier dispatch uses it. Maintenance, pet, and defensive sites
(which do their own preconditions) call `CastWithLog` directly. There
must be no `bot->CastSpell(...)` call inside `src/strategies/` outside
the helper itself.

SpellCastResult numeric codes worth memorizing (`SharedDefines.h`,
`enum SpellCastResult`):
- 49  = `SPELL_FAILED_LINE_OF_SIGHT`
- 53  = `SPELL_FAILED_MOVING`
- 69  = `SPELL_FAILED_NOT_READY` (typically GCD)
- 107 = `SPELL_FAILED_SPELL_IN_PROGRESS`
- 113 = `SPELL_FAILED_TARGET_AURASTATE` (e.g. Deep Freeze needs a frozen target)

### Global cooldown detection
**Status:** Resolved.

`SpellHistory::HasCooldown(SpellInfo*)` only covers a spell's own recovery
timer; it does NOT cover the GCD. Use
`SpellHistory::HasGlobalCooldown(SpellInfo*)` (SpellHistory.h:136,
implemented at SpellHistory.cpp:622-626), which is keyed by
`StartRecoveryCategory` (most rotation spells share category 133).

```cpp
if (uint32 fillerId = GetSpell(Spell::Frostbolt))   // strategy's filler
{
    SpellInfo const* fillerInfo = sSpellMgr->GetSpellInfo(fillerId);
    if (fillerInfo && bot->GetSpellHistory()->HasGlobalCooldown(fillerInfo))
        return;
}
```

Without this guard, every tier issues `CastSpell` during the post-cast
GCD and the server rejects each with `SPELL_FAILED_NOT_READY` (69),
producing one log line per tier per tick.

### Cast-in-progress detection
**Status:** Resolved.

`UNIT_STATE_CASTING` covers regular casts; `Unit::IsNonMeleeSpellCast(false)`
covers channels (Drain Soul, Mind Flay-style). Use both:

```cpp
if (bot->HasUnitState(UNIT_STATE_CASTING) || bot->IsNonMeleeSpellCast(false))
    return;
```

Without it, a 2.5s Frostbolt / Shadow Bolt / Healing Wave gets re-issued
every server tick, producing `SPELL_FAILED_SPELL_IN_PROGRESS` (107) until
the cast resolves.

### Molten Armor — `IsPassive()` filter doesn't catch the trigger ID
**Status:** Pending — log spam, not blocking gameplay.

`Molten Armor` resolves to two candidates with the same `SpellName` and
`SpellFamilyName`:
- 30482 — the player-castable buff (lvl 34, mana=0, cd=0)
- 34913 — the on-attacker fire-damage trigger (lvl 54, mana=0, cd=0)

The two-pass "prefer castable (mana cost or cooldown)" filter doesn't
disambiguate (Cata armor self-buffs are free toggles), and the
`IsPassive()` filter we added at cache time *also* doesn't drop 34913 —
suggesting 34913 is not flagged `SPELL_ATTR0_PASSIVE` in this server's
spell data. The cache picks the higher-level 34913, and
`DoMaintenance` then attempts to self-cast it every tick; it fails with
`SpellCastResult=13` (`BAD_TARGETS`) because 34913's implicit target is
the attacker, not self.

Fix candidates (when we get to it):
- Stronger filter at cache resolution: require a candidate to have at
  least one effect of type `SPELL_EFFECT_APPLY_AURA` for armor-class
  cache slots. The buff (30482) does; the damage trigger (34913) does not.
- Or: per-strategy override that explicitly picks an ID by attribute
  shape for armor self-buffs (since they're a known-shape category).

### Bot teleport — `SendTeleportPacket` takes the wrong branch
**Status:** Resolved.

`Unit::SendTeleportPacket` (`Unit.cpp:13018-13049`) branches on
`IsMovedByClient()`. For player-typed bots — which have a session even
without a real client — the predicate is true, so the helper sends
`SMSG_MOVE_TELEPORT` *only to the bot's own session* and expects a
client `MOVE_TELEPORT_ACK` to come back. That ack never arrives, so
surrounding observers (master, party, anyone else on the grid) get
zero packets — they keep rendering the bot at the old position until
their own grid scan reconciles, which can take many seconds (in the
field: until combat ends and the bot's chase generator stops issuing
splines, then the next visibility update finally shows the new
location).

`AltbotInvite::Summon`'s same-map branch must NOT call
`SendTeleportPacket`. Instead, manually emit the creature-branch
packet that `SendTeleportPacket` would emit for non-players:

```cpp
bot->m_movementInfo.pos.Relocate(destPos);
bot->m_movementInfo.guid = bot->GetGUID();
bot->m_movementInfo.time = GameTime::GetGameTimeMS();
WorldPackets::Movement::MoveUpdateTeleport moveUpdateTeleport;
moveUpdateTeleport.Status = &bot->m_movementInfo;
bot->SendMessageToSet(moveUpdateTeleport.Write(), false);
```

After this, `UpdatePosition(destPos, true)` and `UpdateObjectVisibility()`
land the server-side state, and the master's client actually sees
the relocate.

### Hunter ranged shots — `SPELL_FAILED_TOO_CLOSE` (130) at melee range
**Status:** Resolved (positional fix in `MmHunterStrategy`).

Cata 4.0.1 reduced/removed the hunter ranged dead-zone in retail data,
but this server's spell data still rejects ranged shots inside ~8y
with `SPELL_FAILED_TOO_CLOSE` (130). The strategy must reposition out
of the dead-zone before the rotation tier dispatch — otherwise the
hunter stands at melee, every shot rejects with 130, and the bot does
nothing.

`AltbotPosition::BackUpToRange(bot, anchor, desiredRange)` issues a
`MovePoint` along the anchor→bot vector to a point at `desiredRange`
yards from the anchor. `MmHunterStrategy::Update` invokes it when
`bot->GetDistance(target) < 8y` *and* `master->GetDistance(target) > 10y`
— the master-stack guard prevents breaking Bronjahm-style stack
mechanics. When master is also in melee, the bot stays put and lets
auto-attack carry the damage.

### Dead-zone backup: hostile-density check at destination
**Status:** Pending — current backup is formation-aware but not danger-aware.

Today the dead-zone backup only checks "is master at range" before
moving. It does NOT check whether the destination point has additional
mobs aggroed nearby, so in trash-dense corridors a 3-4y back-step can
pull a second pack. Cliffs, AoE, healer-LoS aren't checked either, but
those are lower-impact (the backup is small, terrain is Z-clamped, and
3-4y rarely breaks healer LoS).

Fix candidate: new helper
`AltbotPosition::CountHostilesNearPosition(bot, x, y, z, radius)`
mirroring `CountHostilesNearUnit` but anchored on a `Position` instead
of a `Unit`. Compute the prospective backup destination first, count
hostiles near it, abort the backup if `>= 2`. Take the dead-zone
shots-fail that tick and try again next tick (target may have moved /
master may have repositioned).

### Summon All: run to master on same map
**Status:** Pending — ship-as-teleport for now.

The action-bar "Summon All" button currently teleports every active bot
to the master's location (`AltbotInvite::Summon` — server-side
`UpdatePosition` for same-map, `TeleportTo` + manual ack for cross-map,
plus `InterruptNonMeleeSpells` / `CombatStop` so the bot doesn't run
back to the old pull). The original UX intent was "same map → run on
foot to me, cross-map → teleport." Deferred because:

- Needs a per-bot `_summonRunningToMaster` flag in `AltbotAI` that
  overrides combat-tick dispatch (skip strategy `Update`, skip
  `MaintainRange`, only allow defensives) until the bot reaches an
  arrival radius near master.
- Needs interaction with the existing follow gate
  (`AltbotAI.cpp:149` — `followGated = ctx.InInstance() && master->IsInCombat()`):
  while the override is active, follow logic must run regardless of the
  gate, since the whole point is to override combat behavior.
- "Arrived" radius of ~3y (current `FOLLOW_DIST`) is the natural choice;
  larger thresholds keep firing the override every tick while the bot
  jostles near master.

Cross-map case stays as teleport even after this lands — the bot can't
run to a different map.

### Warlock Demonic Circle: Summon attempted while moving
**Status:** Pending — log spam, not blocking gameplay.

`AffWarlockStrategy::DoMaintenance` runs every tick and tries to drop
the Demonic Circle anchor when out of combat. The bot is often moving
toward the master at that moment, so the cast fails with
`SpellCastResult=53` (`SPELL_FAILED_MOVING`). Same pattern affects
warlock Haunt attempts when the bot is mid-chase.

Fix candidate: shared "is the bot moving (movespline active)" guard at
the top of every strategy's `Update` — same shape as the IsCasting and
GCD guards. Skips the cast attempt cleanly instead of letting it reject
with `MOVING`.

### `MotionMaster::MoveChase` always Mutates a fresh generator
**Status:** Resolved.

`MoveChase` (MotionMaster.cpp:271-279) always
`Mutate(new ChaseMovementGenerator(...))` — replaces the active slot,
re-initializes pathing, flips `UNIT_STATE_CHASE`. Calling it every tick
produces transient "moving" state that fails in-progress casts with
`SPELL_FAILED_MOVING` (53) even when the bot is already in range.

`AltbotPosition::MaintainRange` only re-issues when `dist > range || !HasUnitState(UNIT_STATE_CHASE)` —
preserve this guard if positioning logic ever changes.
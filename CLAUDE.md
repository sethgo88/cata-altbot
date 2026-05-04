# cata-altbot — Module Context

## Structure
```
cata-altbot/src/
├── AltbotMgr.h/.cpp           ← singleton, AddAltbot(), session creation
├── AltbotAI.h/.cpp            ← per-bot update loop; combat-elapsed + instance-aware follow gating
├── AltbotFollow.cpp           ← follow/movement (idle); not called during instance combat
├── AltbotPosition.h/.cpp      ← combat positioning (MoveChase) + nearby-hostile counter
├── AltbotCombat.cpp           ← combat-tick dispatcher: routes to strategy, falls back to generic scan
├── AltbotStrategy.h           ← interface: Update(bot, master, ctx) + GetName()
├── AltbotStrategyFactory.cpp  ← spec dispatch: class + talent-tree → strategy slug → instance
├── AltbotTickContext.h        ← per-tick state (combatElapsedMs, inDungeon, inRaid)
├── AltbotCommands.cpp         ← chat command parsing
├── AltbotLoader.cpp           ← AddSC_AltbotLoader() registration
└── strategies/                ← per-spec rotation classes
    ├── StrategyUtil.h/.cpp    ← shared: FindTank / FindLowestHpAlly / CountInjured /
    │                             AllAtFullHp / FindSpellByFamilyName
    ├── RestoShamanStrategy.*  ← effect-introspection cache (heal-effect signatures)
    ├── AffWarlockStrategy.*   ← name-match cache (SPELLFAMILY_WARLOCK + SpellName)
    ├── FrostMageStrategy.*    ← name-match cache + applied-aura name walk for Brain Freeze / FoF
    └── MmHunterStrategy.*     ← name-match cache + focus rotation + Misdirection-on-tank
```

## Adding a new spec

> Before writing any cast logic, read **Cast pipeline gotchas** below — all four
> existing strategies follow those patterns and a new one needs to as well.

1. Create `src/strategies/{Spec}Strategy.{h,cpp}` inheriting `AltbotStrategy`.
2. Use `StrategyUtil::FindSpellByFamilyName(bot, SPELLFAMILY_X, "Spell Name")` for caches when
   effect-introspection can't disambiguate (warlock DoTs, mage school overlap, etc.).
   Resto-shaman-style effect signatures are fine when each ability has a unique effect shape.
   **Name collisions the matcher already handles**:
   - cast-vs-damage-trigger (e.g. Deep Freeze 44572 cast vs. 71757 damage) — disambiguated by
     the two-pass "prefer castable (has mana cost or cooldown)" filter.
   - cast-vs-passive-helper (e.g. Molten Armor 30482 cast vs. 34913 on-attacker fire damage,
     where neither has mana cost or cooldown so the castable filter doesn't fire) —
     disambiguated by skipping `info->IsPassive()` candidates outright.
   If a new strategy resolves to a helper ID and `IsOnCooldown` keeps reading false (or
   `bot->HasAura(<cached-id>)` keeps reading false despite the buff being active), that's the
   collision class to investigate.
3. Add the `TalentTab.dbc` ID + slug to `AltbotStrategyFactory.cpp` (verify ID with WDBXEditor;
   tracked in `docs/research/dbc-verification-checklist.md`).
4. DPS strategies: gate the rotation on `ctx.combatElapsedMs >= 2000` (tank threat window) but
   keep maintenance / pet / defensives running. Misdirection on tank during the window is
   *encouraged*, not gated. Healers gate only their support-DPS tier.
5. Ranged casters call `AltbotPosition::MaintainRange(bot, target, 25.0f)` once combat is up.
   Healers don't (they stay near master / party for AoE-heal radius).
6. **AoE cluster checks (ranged DPS only): anchor on the target, not the bot.**
   Use `AltbotPosition::CountHostilesNearUnit(bot, target, radius)` — never
   `CountHostilesNear(bot, radius)` for the AoE-threshold check. The bot stands
   at 25y caster range, so the area around it is empty even when the tank is
   in a 5-pack; an around-bot check effectively disables AoE for ranged DPS.
   `CountHostilesNear(bot, ...)` is reserved for *self-anchored* defensives
   (Frost Nova / Disengage / Howl of Terror — "is something meleeing me?").
   Self-anchored cone effects (Cone of Cold, etc.) gate on `bot->GetDistance(target) <= cone_range`
   *after* the around-target cluster check passes.

## Cast pipeline gotchas

Four traps that have already bitten this code. Every strategy must follow these
patterns; the existing four (Frost Mage, Aff Warlock, MM Hunter, Resto Shaman)
are the reference.

1. **Passive talents share `SpellName` with their proc auras.** Known cases:
   `Fingers of Frost` (44544 talent / 74396 proc) and `Brain Freeze`
   (44546 talent / 57761 proc) — both read as `SpellName="Fingers of Frost"` /
   `"Brain Freeze"` with `SpellFamilyName=MAGE`. Any helper that walks
   `bot->GetAppliedAuras()` matching by name **must** filter
   `info->IsPassive()` or it will return the talent every tick and read
   "proc up" forever. See `FrostMageStrategy.cpp::FindBotAuraByName` and
   `MmHunterStrategy.cpp::FindHunterAuraByName` for the canonical pattern.
   When in doubt, prefer cataloging the actual proc spell ID directly over
   name-matching.

2. **`TryCast` must propagate `SpellCastResult`.** `Unit::CastSpell` returns
   the result code; discarding it makes failures silent and the rotation
   acts as if the cast fired — so the tier chain stops descending and the
   lower-priority tier never gets its turn. Capture the result, log on
   failure, return `false`:
   ```cpp
   SpellCastResult result = bot->CastSpell(target, id, false);
   if (result != SPELL_CAST_OK)
   {
       TC_LOG_INFO("altbot", "{Spec}[%s] CastSpell %u failed: SpellCastResult=%u",
                   bot->GetName().c_str(), id, uint32(result));
       return false;
   }
   ```
   Numeric codes worth memorizing: 49 = LINE_OF_SIGHT, 53 = MOVING,
   107 = SPELL_IN_PROGRESS, 113 = TARGET_AURASTATE (e.g. Deep Freeze
   needs a frozen target).

3. **Skip the rotation while a cast is in progress.** Insert *after*
   maintenance / defensives, *before* the tier dispatch:
   ```cpp
   if (bot->HasUnitState(UNIT_STATE_CASTING) || bot->IsNonMeleeSpellCast(false))
       return;
   ```
   Without it, a 2.5s Frostbolt / Shadow Bolt / Healing Wave gets re-issued
   every server tick, producing `SPELL_FAILED_SPELL_IN_PROGRESS` (107) and
   stopping the lower tiers from getting a turn that tick.
   `IsNonMeleeSpellCast(false)` covers channels (Drain Soul, Mind Flay-style);
   `UNIT_STATE_CASTING` covers regular casts. Use both.

4. **Skip the rotation while the GCD is active.** Insert immediately after
   the cast-in-progress guard:
   ```cpp
   if (uint32 fillerId = GetSpell(Spell::Frostbolt))   // or strategy's filler
   {
       SpellInfo const* fillerInfo = sSpellMgr->GetSpellInfo(fillerId);
       if (fillerInfo && bot->GetSpellHistory()->HasGlobalCooldown(fillerInfo))
           return;
   }
   ```
   `IsCasting` only catches the cast itself; the 1-1.5s GCD that follows is
   invisible to it. Without this guard, every tier would call `CastSpell`
   during the GCD and reject with `SPELL_FAILED_NOT_READY` (69), producing the
   cascade of failure log lines (one per tier) on every post-cast tick.
   `HasGlobalCooldown` is keyed by `StartRecoveryCategory`, so any rotation
   spell from the standard GCD bucket works as the probe — pick the strategy's
   filler (Frostbolt / ShadowBolt / SteadyShot / HealingWave) since it's
   always present once the spec is selected.

5. **`MaintainRange` is idempotent only when guarded.**
   `MotionMaster::MoveChase` always `Mutate`s a fresh `ChaseMovementGenerator`
   (re-initializes pathing, flips `UNIT_STATE_CHASE`). Calling it every tick
   produces `SPELL_FAILED_MOVING` (53) mid-cast even when the bot is already
   in range. `AltbotPosition::MaintainRange` only re-issues when
   `dist > range || !HasUnitState(UNIT_STATE_CHASE)` — preserve this guard
   if positioning logic ever changes.

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
- `server-core/src/server/game/Groups/Group.h` near `GetRoll`/`CountRollVote` — `GetRolls()` public read-only accessor for `RollId` (the `Rolls` typedef is `protected`, so the return type is spelled inline). Lets `AltbotLoot::Tick` find pending rolls and auto-vote PASS / DISENCHANT.

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
| AltbotPosition | strategy/actions/MoveToTargetAction + DistanceCheck helpers |
| AltbotCombat | strategy/StrategyContext (dispatcher only — owns nothing) |
| strategies/{Spec}Strategy | strategy/specs/{class}/{spec}/Strategy.cpp |
| AltbotStrategyFactory | strategy/StrategyFactory |
| AltbotCommands | PlayerbotCommandHandler + ChatHelper |

# Using cata-altbot

A how-to for the master player. Every section is tagged:

- ✅ **Implemented** — works today.
- 🟡 **Partial** — works but with caveats noted inline.
- 🚧 **WIP** — placeholder/stub; flag exists but does nothing yet.

If you can't find a behavior here, check `src/AltbotCommandTable.cpp` for the
authoritative whisper-command list.

---

## Setup

### One-time server setup ✅
1. Apply all SQL migrations (see [sql-migrations.md](sql-migrations.md)).
2. Copy `altbot.conf.dist` → `<server-config-dir>/altbot.conf` and edit if
   you want to override defaults. All settings have sensible defaults; the
   file is optional.
3. Restart the worldserver. On boot, the module registers chat commands and
   the addon hook.

### Per-master setup ✅
On login, if you have any registered bots the server whispers you a list and
how many. Bots are **not** auto-spawned — you pick which to bring in each
session.

---

## Roster commands (`.altbot ...`)

Slash-commands typed in your chat box. Require RBAC permission
`RBAC_PERM_COMMAND_ALTBOT_ADD` (player accounts get this by default in stock TC).

| Command | What it does | Status |
|---|---|---|
| `.altbot list` | Show every character on your account + linked accounts, with status: `available` / `registered` / `ACTIVE` | ✅ |
| `.altbot add <name>` | Register a character of yours as an altbot **and** spawn it. Durable — survives restart. | ✅ |
| `.altbot remove <name>` | Unregister + despawn. Deletes the registration row and the per-bot state row. | ✅ |
| `.altbot login <name>` | Spawn an already-registered bot for this session. | ✅ |
| `.altbot logout <name>` | Despawn a bot but keep its registration + state. | ✅ |
| `.altbot link <username>` | Begin cross-account link — see [Cross-account rosters](#cross-account-rosters-) below. | ✅ |
| `.altbot unlink <username>` | Drop a previously-linked account. | ✅ |
| `.altbot links` | List currently-linked accounts. | ✅ |

### Inviting / uninviting a bot to your party ✅
Whisper the bot:
- `invite` — bot invites you to a party (or invites itself if you're the leader of an existing one).
- `uninvite` — bot leaves the party.
- `come` (or `summon`) — bot teleports to your location.

### Cross-account rosters ✅
Run `.altbot link <other-account-username>`. Within `Altbot.LinkPasswordTtlSeconds`
(default 60s) **whisper one of your bots** the password for that account.
On match, the link is recorded and characters from that account become eligible
for `.altbot add`. Only the bot of yours that you whisper can capture the
password — random whispers from elsewhere can't.

---

## Whisper commands (party / direct whisper to a bot)

These all go to a **registered + active** bot. Verb-style; case-insensitive;
arguments after a space.

### Movement & combat basics ✅
| Whisper | Effect |
|---|---|
| `follow` | Bot follows you. |
| `stay` | Bot stops moving. |
| `attack` | Bot attacks your current target. |
| `help` | Lists the whisper commands the bot knows. |

### Assist mode ✅
`assist <off\|target\|skull\|both>` — controls how the bot picks DPS targets:
- `off` — never auto-engages.
- `target` — engages whatever the master is currently targeting.
- `skull` — engages the raid-marked skull only.
- `both` (default) — skull takes precedence over master's target.

### Spec selection (Phase 6) ✅ 🟡
`spec [auto|resto|elemental|enhancement]`

- No arg or `auto` → re-detect spec from the bot's primary talent tree.
- `resto` / `restoration` / `resto-shaman` → force Resto Shaman strategy.
- `elemental` / `ele` → reserved slug; **no strategy implementation yet**, falls through to the generic combat scan.
- `enhance` / `enhancement` → reserved slug; **no strategy implementation yet**, falls through to the generic combat scan.

🟡 caveat: only **Resto Shaman** has a real strategy today. The other slugs
are accepted and persisted, but the bot uses the generic heal-then-DPS scan
until those strategies are written. The selected slug is saved to
`character_altbot.spec_override` and re-applied on next login.

### Auto-* toggles ✅
Each accepts `on` / `off` / `1` / `0` / `yes` / `no` / `true` / `false`. With
no argument the toggle flips. State is persisted in `character_altbot_state`.

| Whisper | Default | Effect |
|---|---|---|
| `mount [on\|off]` | on | Bot summons a mount when you mount up. |
| `release [on\|off]` | on | Bot releases its corpse + ghost-walks back when it dies. |
| `loot [on\|off]` | on | Bot auto-loots its kills. |
| `pass [on\|off]` | off | Bot passes on master-loot rolls. |
| `questtake [on\|off]` | off | Bot auto-accepts quests the master accepts at the same NPC. |
| `questturnin [on\|off]` | off | Bot auto-turns-in quests the master turns in at the same NPC. |

### Inventory ✅ 🟡
| Whisper | Effect |
|---|---|
| `bags` | Lists the bot's bag contents (item, count, bag/slot, guid). |
| `equip <name\|guid>` | Bot equips the named/guid'd item from its bags. |
| `sell <name\|guid>` | Bot sells the item to the nearest open vendor. |
| `drop <name\|guid>` | Bot destroys the item. |
| `trade` | Opens a trade window between bot and master. |

🟡 caveat: gear-scoring / "is this an upgrade?" logic is not in the bot. The
bot equips whatever you tell it to. Stat-aware auto-equip is the WIP feature
below.

### Talents ✅ 🟡
| Whisper | Effect |
|---|---|
| `talents` | Print the bot's current talent allocation. |
| `learn <talentId> <rank>` | Spend points into a specific talent + rank. |

🟡 caveat: you allocate talents by ID manually. There's no per-spec template
yet (that's the WIP `Altbot.AutoTalent` setting).

---

## Configuration (`altbot.conf`)

Loaded at world boot and on `.reload config`. All keys are optional and
default to the values shown.

| Key | Default | Status |
|---|---|---|
| `Altbot.AutoEquip` | `0` | 🚧 WIP — flag accepted; behavior is a stub that logs once and does nothing. Leave at `0`. |
| `Altbot.AutoTalent` | `0` | 🚧 WIP — same as above. Leave at `0`. |
| `Altbot.AddonProtocol.RateLimit` | `10` | ✅ Cap on addon-protocol verbs per master per second. |
| `Altbot.LinkPasswordTtlSeconds` | `60` | ✅ Window to confirm a `.altbot link` by whispering the password. |
| `Altbot.UpdateInterval.Follow` | `1000` ms | ✅ Tick rate for follow / mount / release / loot. |
| `Altbot.UpdateInterval.Combat` | `1500` ms | ✅ Tick rate for combat rotation. |

---

## Combat strategies (Phase 6)

The combat tick consults a per-spec **strategy** for the tier-1 priority list.
If the bot's spec has no strategy yet, the tick falls back to a generic
"heal master under 70% HP, otherwise nuke with the highest-level direct
damage spell" loop.

| Spec | Slug | Status |
|---|---|---|
| Restoration Shaman | `resto-shaman` | ✅ Implemented (Phase 6). 9-tier priority + maintenance pass + mana-mode shifts. Spirit Link / Mana Tide totem casts deferred until totem disambiguation lands. |
| Elemental Shaman | `elemental` | 🚧 WIP — slug reserved; spec doc exists at `docs/specs/elemental-shaman.md`. |
| Enhancement Shaman | `enhancement` | 🚧 WIP — slug reserved; spec doc exists at `docs/specs/enhancement-shaman.md`. |
| Holy Paladin | (none yet) | 🚧 WIP — spec doc only. |
| Resto Druid | (none yet) | 🚧 WIP — spec doc only. |
| Holy / Disc Priest | (none yet) | 🚧 WIP — spec doc only. |
| All other classes | (none yet) | 🚧 WIP — generic combat scan only. |

When more strategies land, this table grows; check
`src/strategies/` and `AltbotStrategyFactory::Create` for what's actually
wired up.

---

## Addon

The repo ships a master-side addon at `addon/CataAltbot/` for an in-game UI
similar to MultiBot.

🟡 The wire protocol (`AltbotAddonProto`) is implemented and rate-limited via
`Altbot.AddonProtocol.RateLimit`. The addon UI itself is in active
development — features land incrementally on top of the same whisper
commands listed above. Anything you can do via whisper, the protocol can do;
the UI just exposes them as buttons.

---

## Quick reference: a typical session

```
.altbot list                              # see what alts you have
.altbot add MyResto                       # register + spawn MyResto
/w MyResto invite                         # bot invites you to a party
/w MyResto follow                         # bot follows
/w MyResto spec resto                     # force Resto Shaman strategy
/w MyResto loot off                       # I'll handle the loot
... play ...
.altbot logout MyResto                    # done for now; keep registration
```

Next session: `.altbot login MyResto` brings them back with the same toggles,
spec override, and auto-* settings.

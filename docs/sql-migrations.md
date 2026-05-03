# SQL Migrations

All migrations live in `sql/` and are applied to the **characters** database
(the same DB that holds the `characters` table). Apply them in numeric order;
each is idempotent (`CREATE TABLE IF NOT EXISTS`, `ADD COLUMN IF NOT EXISTS`)
so re-running on an already-migrated DB is safe.

## Apply order

| # | File | Adds | Status |
|---|------|------|--------|
| 001 | `sql/001_create_character_altbot.sql` | `character_altbot` table (master ↔ bot registration) | NOT APPLIED |
| 002 | `sql/002_create_character_altbot_state.sql` | `character_altbot_state` table (per-bot toggles: mode, assist, auto-loot/mount/release/quest) | NOT APPLIED |
| 003 | `sql/003_create_character_altbot_account_link.sql` | `character_altbot_account_link` table (cross-account ownership) | NOT APPLIED |
| 004 | `sql/004_add_spec_override.sql` | `character_altbot.spec_override` column (Phase 6 strategy override) | NOT APPLIED |

Update the **Status** column to `APPLIED <date>` after running each one against
your characters DB so this file stays a current source of truth.

## How to apply

From a MySQL/MariaDB client connected to the characters DB:

```sh
mysql -u <user> -p <characters_db> < sql/001_create_character_altbot.sql
mysql -u <user> -p <characters_db> < sql/002_create_character_altbot_state.sql
mysql -u <user> -p <characters_db> < sql/003_create_character_altbot_account_link.sql
mysql -u <user> -p <characters_db> < sql/004_add_spec_override.sql
```

Or paste them individually in your DB tool of choice. Order matters for the
first time only; once applied, re-runs are no-ops.

## What each migration is for

### 001 — `character_altbot`
Registers which characters belong to which master. Primary key is
`(master_guid, bot_guid)`. Required before the module can do anything; without
this table `.altbot add` fails on the first INSERT.

### 002 — `character_altbot_state`
Per-bot toggle persistence (follow/stay mode, assist mode, auto-loot,
auto-mount, auto-release, auto-quest-take, auto-quest-turn-in, auto-pass).
Defaults in the schema must match the in-memory defaults in `AltbotState.h`.
Without this table the bot still spawns but every whisper-toggle only lasts
the session.

### 003 — `character_altbot_account_link`
Lets a master register characters from multiple game accounts as altbots
(needed for 25-man rosters that span the per-account character cap).
Bootstrapped via `.altbot link <username>` + whispering the alt-account
password. Only needed if you plan to use cross-account rosters.

### 004 — `character_altbot_spec_override`
Adds the `spec_override VARCHAR(32)` column to `character_altbot`. Empty =
auto-detect spec from the bot's primary talent tree; a slug like
`"resto-shaman"` forces that strategy. Set/cleared by the `spec` whisper
command (Phase 6).

## Adding a new migration

1. Pick the next unused number (currently 005). Use a 3-digit zero-padded
   prefix and a descriptive snake-case name: `005_<what_it_does>.sql`.
2. Wrap each statement so it's safe to re-run: `CREATE TABLE IF NOT EXISTS`,
   `ADD COLUMN IF NOT EXISTS`, `CREATE INDEX IF NOT EXISTS`. No raw `ALTER
   TABLE … ADD COLUMN` (will throw on second apply).
3. Add a row to the **Apply order** table above with `NOT APPLIED` status.
4. If the schema has matching in-memory defaults (e.g. `AltbotState.h`), keep
   the two in sync and note it in the file's header comment.

## Rolling back

There's no formal rollback step. To undo a migration:

- Drop the table or column manually.
- For `004`, the column add can be reversed with
  `ALTER TABLE character_altbot DROP COLUMN spec_override;`.

The module tolerates stale `spec_override` values (an unknown slug just falls
through `AltbotStrategyFactory::Create` to `nullptr`, which means "use the
generic combat scan"), so dropping that column won't crash a running server —
it'll just disable forced specs.

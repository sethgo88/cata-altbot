-- Per-bot toggle state for the altbot module.
-- Lives separate from `character_altbot` so the registration row stays a pure
-- master <-> bot association. State is keyed by bot_guid alone (a character can
-- only be a bot of one master, enforced by character_altbot's PK semantics).
--
-- Defaults must match AltbotState's default-initialized fields in AltbotState.h.
-- mode: 0 = Follow.  assist_mode: 3 = Both (master target + skull).
CREATE TABLE IF NOT EXISTS `character_altbot_state` (
  `bot_guid`           INT(10) UNSIGNED NOT NULL,
  `mode`               TINYINT(3) UNSIGNED NOT NULL DEFAULT 0,
  `assist_mode`        TINYINT(3) UNSIGNED NOT NULL DEFAULT 3,
  `auto_loot`          TINYINT(1) UNSIGNED NOT NULL DEFAULT 1,
  `auto_pass`          TINYINT(1) UNSIGNED NOT NULL DEFAULT 0,
  `auto_mount`         TINYINT(1) UNSIGNED NOT NULL DEFAULT 1,
  `auto_release`       TINYINT(1) UNSIGNED NOT NULL DEFAULT 1,
  `auto_quest_take`    TINYINT(1) UNSIGNED NOT NULL DEFAULT 0,
  `auto_quest_turn_in` TINYINT(1) UNSIGNED NOT NULL DEFAULT 0,
  PRIMARY KEY (`bot_guid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- Altbot persistence: records which characters are configured as altbots for each master player.
-- Apply to the characters database before starting the server with the altbot module.
CREATE TABLE IF NOT EXISTS `character_altbot` (
  `master_guid` INT(10) UNSIGNED NOT NULL COMMENT 'GUID of the master player',
  `bot_guid`    INT(10) UNSIGNED NOT NULL COMMENT 'GUID of the bot character',
  PRIMARY KEY (`master_guid`, `bot_guid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- Phase 6 strategy system: per-bot spec override.
-- Empty string = auto-detect from the bot's primary talent tree (default).
-- A non-empty slug (e.g. "resto-shaman") forces that strategy regardless of talents.
--
-- Idempotent: wraps the ALTER in a procedure that checks INFORMATION_SCHEMA first,
-- so it is safe to run more than once (MySQL <8.0 / MariaDB <10.3 compatible).

DROP PROCEDURE IF EXISTS `altbot_add_spec_override`;

DELIMITER $$
CREATE PROCEDURE `altbot_add_spec_override`()
BEGIN
    IF NOT EXISTS (
        SELECT 1
        FROM INFORMATION_SCHEMA.COLUMNS
        WHERE TABLE_SCHEMA = DATABASE()
          AND TABLE_NAME   = 'character_altbot'
          AND COLUMN_NAME  = 'spec_override'
    ) THEN
        ALTER TABLE `character_altbot`
            ADD COLUMN `spec_override` VARCHAR(32) NOT NULL DEFAULT ''
            COMMENT 'Empty = auto-detect from talent tree. Slug e.g. resto-shaman to force.';
    END IF;
END$$
DELIMITER ;

CALL `altbot_add_spec_override`();
DROP PROCEDURE IF EXISTS `altbot_add_spec_override`;

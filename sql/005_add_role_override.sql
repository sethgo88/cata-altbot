-- Per-bot manual role override (LFG auto-pick + raid main-tank flag).
-- 0 = None (derive from spec), 1 = Tank, 2 = Healer, 3 = Damage, 4 = MainTank.
-- MainTank submits PLAYER_ROLE_TANK to LFG and sets MEMBER_FLAG_MAINTANK in raid groups.
--
-- Idempotent: wraps the ALTER in a procedure that checks INFORMATION_SCHEMA first,
-- so it is safe to run more than once (MySQL <8.0 / MariaDB <10.3 compatible).

DROP PROCEDURE IF EXISTS `altbot_add_role_override`;

DELIMITER $$
CREATE PROCEDURE `altbot_add_role_override`()
BEGIN
    IF NOT EXISTS (
        SELECT 1
        FROM INFORMATION_SCHEMA.COLUMNS
        WHERE TABLE_SCHEMA = DATABASE()
          AND TABLE_NAME   = 'character_altbot_state'
          AND COLUMN_NAME  = 'role_override'
    ) THEN
        ALTER TABLE `character_altbot_state`
            ADD COLUMN `role_override` TINYINT(3) UNSIGNED NOT NULL DEFAULT 0
            COMMENT '0=None,1=Tank,2=Healer,3=Damage,4=MainTank';
    END IF;
END$$
DELIMITER ;

CALL `altbot_add_role_override`();
DROP PROCEDURE IF EXISTS `altbot_add_role_override`;

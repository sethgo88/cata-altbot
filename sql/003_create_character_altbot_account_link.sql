-- Cross-account altbot ownership linking.
-- Lets a master register characters from multiple accounts as altbots — needed
-- because TC's per-realm character cap (and the 4.3.4 client UI) make raising
-- the per-account count fragile, so 25-man raid bot rosters span 3+ accounts.
--
-- Linking is one-time per (master_account, linked_account) pair. The link is
-- bootstrapped via in-game flow: master types `.altbot link <username>`, then
-- whispers their bot the alt-account password within 60 seconds. Validation
-- goes through AccountMgr::CheckPassword.
--
-- Lives in the characters DB alongside character_altbot for module cohesion;
-- account_id values are stable across DBs.
CREATE TABLE IF NOT EXISTS `character_altbot_account_link` (
  `master_account` INT(10) UNSIGNED NOT NULL,
  `linked_account` INT(10) UNSIGNED NOT NULL,
  `linked_at`      TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,
  PRIMARY KEY (`master_account`, `linked_account`),
  KEY `idx_linked_account` (`linked_account`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

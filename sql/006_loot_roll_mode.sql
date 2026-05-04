-- Replace the boolean `auto_pass` toggle with a tri-state `loot_roll` mode:
--   0 = Wait        (do nothing — let the 60s roll timer expire on its own)
--   1 = Pass        (auto-vote PASS on every eligible roll)
--   2 = Disenchant  (auto-vote DISENCHANT if offered, else PASS)
--
-- The old 0/1 values map cleanly onto Wait/Pass without conversion.
ALTER TABLE `character_altbot_state`
    CHANGE COLUMN `auto_pass` `loot_roll` TINYINT(3) UNSIGNED NOT NULL DEFAULT 0;

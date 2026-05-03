-- Phase 6 strategy system: per-bot spec override.
-- Empty string = auto-detect from the bot's primary talent tree (default).
-- A non-empty slug (e.g. "resto-shaman") forces that strategy regardless of talents.
ALTER TABLE `character_altbot`
  ADD COLUMN IF NOT EXISTS `spec_override` VARCHAR(32) NOT NULL DEFAULT ''
  COMMENT 'Empty = auto-detect from talent tree. Slug e.g. resto-shaman to force.';

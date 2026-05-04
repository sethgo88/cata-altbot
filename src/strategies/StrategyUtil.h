#pragma once

#include "SharedDefines.h"

class Player;
class Unit;

namespace StrategyUtil
{
    // Returns the player most likely to be the tank: the bot's group member
    // with the highest max HP, on the same map. Falls back to `master` when
    // there's no group or no better candidate. Bot is excluded from selection.
    Player* FindTank(Player* bot, Player* master);

    // Returns the alive ally with the lowest health percent (group member or
    // bot/master if no group). `exclude` lets callers skip the tank when
    // running a "non-tank lowest" tier.
    Player* FindLowestHpAlly(Player* bot, Player* master, Player* exclude = nullptr);

    // Counts alive allies whose HP percent is below `thresholdPct`.
    int CountInjured(Player* bot, Player* master, float thresholdPct);

    // Returns true when every alive ally is at or above `minPct` HP.
    bool AllAtFullHp(Player* bot, Player* master, float minPct);

    // Walk bot's spell map and return the highest-level active spell whose
    // SpellFamilyName matches `family` and whose canonical English SpellName
    // equals `name` exactly. Returns 0 if not known. Used by warlock/mage/hunter
    // caches where effect-introspection alone can't disambiguate (e.g. all
    // shadow DoTs look the same; all curse aura applications look the same).
    uint32 FindSpellByFamilyName(Player* bot, uint32 family, char const* name);

    // Issue `bot->CastSpell(target, spellId, false)` and log spec-prefixed
    // failure on non-`SPELL_CAST_OK` results. Returns the result so callers
    // can branch on success when needed (chained casts, tier cascade).
    // Caller is responsible for any cooldown / precondition checks — this is
    // strictly the cast + log step.
    //
    // `specLabel` is a short stable string ("FrostMage", "AffWarlock", etc.)
    // used only as the log prefix; each strategy defines a `SPEC_LABEL`
    // constexpr in its anonymous namespace and passes it here.
    SpellCastResult CastWithLog(Player* bot, Unit* target, uint32 spellId, char const* specLabel);
}

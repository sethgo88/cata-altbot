#pragma once
#include <cstdint>

// Typed accessors for altbot.conf values. Reads altbot.conf from the same
// directory as worldserver.conf; hot-reloaded on the standard TC config reload.
class AltbotConfig
{
public:
    static AltbotConfig* instance();

    void Reload();   // re-fetches via sConfigMgr; safe to call mid-session.

    bool   AutoEquipEnabled()      const { return _autoEquip; }
    bool   AutoTalentEnabled()     const { return _autoTalent; }
    uint32 AddonRateLimit()        const { return _addonRateLimit; }
    uint32 LinkPasswordTtl()       const { return _linkPasswordTtl; }
    uint32 FollowIntervalMs()      const { return _followIntervalMs; }
    uint32 CombatIntervalMs()      const { return _combatIntervalMs; }

private:
    AltbotConfig() { Reload(); }

    bool   _autoEquip        = false;
    bool   _autoTalent       = false;
    uint32 _addonRateLimit   = 10;
    uint32 _linkPasswordTtl  = 60;
    uint32 _followIntervalMs = 1000;
    uint32 _combatIntervalMs = 1500;
};

#define sAltbotConfig AltbotConfig::instance()

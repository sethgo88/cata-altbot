#include "AltbotConfig.h"
#include "Config.h"
#include "Log.h"

AltbotConfig* AltbotConfig::instance()
{
    static AltbotConfig instance;
    return &instance;
}

void AltbotConfig::Reload()
{
    // TC uses GetBoolDefault / GetIntDefault. AzerothCore-style GetOption<T> is
    // not used here — the TC fork verification step in CLAUDE.md confirms the
    // sConfigMgr API shape on this codebase.
    _autoEquip        = sConfigMgr->GetBoolDefault("Altbot.AutoEquip",                  false);
    _autoTalent       = sConfigMgr->GetBoolDefault("Altbot.AutoTalent",                 false);
    _addonRateLimit   = sConfigMgr->GetIntDefault ("Altbot.AddonProtocol.RateLimit",    10);
    _linkPasswordTtl  = sConfigMgr->GetIntDefault ("Altbot.LinkPasswordTtlSeconds",     60);
    _followIntervalMs = sConfigMgr->GetIntDefault ("Altbot.UpdateInterval.Follow",      1000);
    _combatIntervalMs = sConfigMgr->GetIntDefault ("Altbot.UpdateInterval.Combat",      1500);

    if (_autoEquip)
        TC_LOG_WARN("altbot", "Altbot.AutoEquip is enabled but the auto-equip module is WIP — no-op.");
    if (_autoTalent)
        TC_LOG_WARN("altbot", "Altbot.AutoTalent is enabled but the auto-talent module is WIP — no-op.");
}

#include "AltbotConfig.h"
#include "Config.h"
#include "Log.h"
#include <boost/filesystem/path.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/property_tree/ptree.hpp>

namespace bfs = boost::filesystem;
namespace bpt = boost::property_tree;

static bpt::ptree LoadAltbotTree()
{
    bfs::path altbotConf =
        bfs::path(sConfigMgr->GetFilename()).parent_path() / "altbot.conf";

    bpt::ptree fullTree;
    try
    {
        bpt::ini_parser::read_ini(altbotConf.string(), fullTree);
        // TC ini files wrap keys in a single section; strip the wrapper.
        if (!fullTree.empty())
            return fullTree.begin()->second;
    }
    catch (bpt::ini_parser::ini_parser_error const& e)
    {
        TC_LOG_WARN("altbot", "AltbotConfig: could not read altbot.conf (%s) — using defaults.",
            e.what());
    }
    return {};
}

AltbotConfig* AltbotConfig::instance()
{
    static AltbotConfig instance;
    return &instance;
}

void AltbotConfig::Reload()
{
    bpt::ptree const tree = LoadAltbotTree();

    auto getBool = [&](char const* key, bool def) -> bool
    {
        return tree.get<bool>(bpt::ptree::path_type(key, '/'), def);
    };
    auto getInt = [&](char const* key, int def) -> int
    {
        return tree.get<int>(bpt::ptree::path_type(key, '/'), def);
    };

    _autoEquip        = getBool("Altbot.AutoEquip",               false);
    _autoTalent       = getBool("Altbot.AutoTalent",              false);
    _addonRateLimit   = getInt ("Altbot.AddonProtocol.RateLimit", 10);
    _linkPasswordTtl  = getInt ("Altbot.LinkPasswordTtlSeconds",  60);
    _followIntervalMs = getInt ("Altbot.UpdateInterval.Follow",   1000);
    _combatIntervalMs = getInt ("Altbot.UpdateInterval.Combat",   1500);

    _telemetryEnabled        = getBool("Altbot.Telemetry.Enabled",         false);
    _telemetryWhisperSummary = getBool("Altbot.Telemetry.WhisperSummary",  true);
    _telemetryPerCastLog     = getBool("Altbot.Telemetry.PerCastLog",      false);
    _telemetryMinFightMs     = getInt ("Altbot.Telemetry.MinFightMs",      15000);

    if (_autoEquip)
        TC_LOG_WARN("altbot", "Altbot.AutoEquip is enabled but the auto-equip module is WIP — no-op.");
    if (_autoTalent)
        TC_LOG_WARN("altbot", "Altbot.AutoTalent is enabled but the auto-talent module is WIP — no-op.");
}

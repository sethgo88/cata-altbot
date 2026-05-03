#include "AltbotStrategyFactory.h"
#include "AltbotStrategy.h"
#include "strategies/AffWarlockStrategy.h"
#include "strategies/FrostMageStrategy.h"
#include "strategies/MmHunterStrategy.h"
#include "strategies/RestoShamanStrategy.h"
#include "Player.h"
#include "SharedDefines.h"

namespace AltbotStrategyFactory
{

// Cata 4.3.4 talent tree DBC IDs (verified via WDBXEditor CSV export).
static constexpr uint32 SHAMAN_TREE_ELEMENTAL    = 261;
static constexpr uint32 SHAMAN_TREE_ENHANCEMENT  = 263;
static constexpr uint32 SHAMAN_TREE_RESTORATION  = 262;

// TalentTab.dbc IDs for the new specs. Standard Cata 4.3.4 values; verify via
// WDBXEditor before merge to catch any private-build divergence (the same
// process used for the shaman trees above).
static constexpr uint32 WARLOCK_TREE_AFFLICTION  = 302;
static constexpr uint32 WARLOCK_TREE_DEMONOLOGY  = 303;
static constexpr uint32 WARLOCK_TREE_DESTRUCTION = 301;

static constexpr uint32 MAGE_TREE_ARCANE         =  81;
static constexpr uint32 MAGE_TREE_FIRE           =  41;
static constexpr uint32 MAGE_TREE_FROST          =  61;

static constexpr uint32 HUNTER_TREE_BEAST        =  50;
static constexpr uint32 HUNTER_TREE_MARKSMANSHIP =  51;
static constexpr uint32 HUNTER_TREE_SURVIVAL     = 163;

static std::string SlugFromBot(Player* bot)
{
    uint8  cls  = bot->getClass();
    uint32 tree = bot->GetPrimaryTalentTree(bot->GetActiveSpec());

    if (cls == CLASS_SHAMAN)
    {
        if (tree == SHAMAN_TREE_RESTORATION)  return "resto-shaman";
        if (tree == SHAMAN_TREE_ELEMENTAL)    return "elemental";
        if (tree == SHAMAN_TREE_ENHANCEMENT)  return "enhancement";
    }
    else if (cls == CLASS_WARLOCK)
    {
        if (tree == WARLOCK_TREE_AFFLICTION)  return "affliction-warlock";
    }
    else if (cls == CLASS_MAGE)
    {
        if (tree == MAGE_TREE_FROST)          return "frost-mage";
    }
    else if (cls == CLASS_HUNTER)
    {
        if (tree == HUNTER_TREE_MARKSMANSHIP) return "marksmanship-hunter";
    }
    return "";
}

std::unique_ptr<AltbotStrategy> Create(Player* bot, std::string const& specOverride)
{
    std::string slug = specOverride.empty() ? SlugFromBot(bot) : specOverride;

    if (slug == "resto-shaman")
        return std::make_unique<RestoShamanStrategy>();
    if (slug == "affliction-warlock")
        return std::make_unique<AffWarlockStrategy>();
    if (slug == "frost-mage")
        return std::make_unique<FrostMageStrategy>();
    if (slug == "marksmanship-hunter")
        return std::make_unique<MmHunterStrategy>();

    // Future specs added here as spec docs land.
    return nullptr;
}

} // namespace AltbotStrategyFactory

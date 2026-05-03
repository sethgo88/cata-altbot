#include "AltbotStrategyFactory.h"
#include "AltbotStrategy.h"
#include "strategies/RestoShamanStrategy.h"
#include "Player.h"
#include "SharedDefines.h"

namespace AltbotStrategyFactory
{

// Cata 4.3.4 talent tree DBC IDs (verified via WDBXEditor CSV export).
static constexpr uint32 SHAMAN_TREE_ELEMENTAL   = 261;
static constexpr uint32 SHAMAN_TREE_ENHANCEMENT = 263;
static constexpr uint32 SHAMAN_TREE_RESTORATION = 262;

static std::string SlugFromBot(Player* bot)
{
    uint8  cls  = bot->getClass();
    uint32 tree = bot->GetPrimaryTalentTree(bot->GetActiveSpec());

    if (cls == CLASS_SHAMAN)
    {
        if (tree == SHAMAN_TREE_RESTORATION) return "resto-shaman";
        if (tree == SHAMAN_TREE_ELEMENTAL)   return "elemental";
        if (tree == SHAMAN_TREE_ENHANCEMENT) return "enhancement";
    }
    return "";
}

std::unique_ptr<AltbotStrategy> Create(Player* bot, std::string const& specOverride)
{
    std::string slug = specOverride.empty() ? SlugFromBot(bot) : specOverride;

    if (slug == "resto-shaman")
        return std::make_unique<RestoShamanStrategy>();

    // Future specs added here as spec docs land (HolyPaladin, FrostMage, etc.)
    return nullptr;
}

} // namespace AltbotStrategyFactory

#include "AltbotRole.h"
#include "LFG.h"
#include "Player.h"
#include "SharedDefines.h"

namespace AltbotRole
{

// Cata 4.3.4 TalentTab.dbc IDs. Shaman trio is verified by the strategy factory;
// the rest are the standard Cata IDs (TalentTab.dbc indexed by class mask).
namespace TalentTree
{
    constexpr uint32 WARRIOR_ARMS         = 161;
    constexpr uint32 WARRIOR_FURY         = 164;
    constexpr uint32 WARRIOR_PROTECTION   = 163;

    constexpr uint32 PALADIN_HOLY         = 382;
    constexpr uint32 PALADIN_PROTECTION   = 383;
    constexpr uint32 PALADIN_RETRIBUTION  = 381;

    constexpr uint32 HUNTER_BEAST_MASTERY = 361;
    constexpr uint32 HUNTER_MARKSMANSHIP  = 363;
    constexpr uint32 HUNTER_SURVIVAL      = 362;

    constexpr uint32 ROGUE_ASSASSINATION  = 182;
    constexpr uint32 ROGUE_COMBAT         = 181;
    constexpr uint32 ROGUE_SUBTLETY       = 183;

    constexpr uint32 PRIEST_DISCIPLINE    = 201;
    constexpr uint32 PRIEST_HOLY          = 202;
    constexpr uint32 PRIEST_SHADOW        = 203;

    constexpr uint32 DK_BLOOD             = 398;
    constexpr uint32 DK_FROST             = 399;
    constexpr uint32 DK_UNHOLY            = 400;

    constexpr uint32 SHAMAN_ELEMENTAL     = 261;
    constexpr uint32 SHAMAN_ENHANCEMENT   = 263;
    constexpr uint32 SHAMAN_RESTORATION   = 262;

    constexpr uint32 MAGE_ARCANE          = 81;
    constexpr uint32 MAGE_FIRE            = 41;
    constexpr uint32 MAGE_FROST           = 61;

    constexpr uint32 WARLOCK_AFFLICTION   = 302;
    constexpr uint32 WARLOCK_DEMONOLOGY   = 303;
    constexpr uint32 WARLOCK_DESTRUCTION  = 301;

    constexpr uint32 DRUID_BALANCE        = 283;
    constexpr uint32 DRUID_FERAL          = 281;
    constexpr uint32 DRUID_RESTORATION    = 282;
}

static Role FromSpecSlug(std::string const& slug)
{
    if (slug == "resto-shaman" || slug == "resto-druid" ||
        slug == "holy-paladin" || slug == "holy-priest" ||
        slug == "disc-priest")
        return Role::Healer;

    if (slug == "prot-paladin" || slug == "prot-warrior" ||
        slug == "blood-dk" || slug == "feral-bear-druid" ||
        slug == "guardian-druid")
        return Role::Tank;

    if (slug.empty())
        return Role::None;

    return Role::Damage;
}

static Role FromClassAndTree(uint8 cls, uint32 tree)
{
    switch (cls)
    {
        case CLASS_WARRIOR:
            if (tree == TalentTree::WARRIOR_PROTECTION) return Role::Tank;
            return Role::Damage;

        case CLASS_PALADIN:
            if (tree == TalentTree::PALADIN_HOLY)        return Role::Healer;
            if (tree == TalentTree::PALADIN_PROTECTION)  return Role::Tank;
            return Role::Damage;

        case CLASS_PRIEST:
            if (tree == TalentTree::PRIEST_DISCIPLINE || tree == TalentTree::PRIEST_HOLY)
                return Role::Healer;
            return Role::Damage;

        case CLASS_DEATH_KNIGHT:
            if (tree == TalentTree::DK_BLOOD) return Role::Tank;
            return Role::Damage;

        case CLASS_SHAMAN:
            if (tree == TalentTree::SHAMAN_RESTORATION) return Role::Healer;
            return Role::Damage;

        case CLASS_DRUID:
            if (tree == TalentTree::DRUID_RESTORATION) return Role::Healer;
            // Feral covers cat (DPS) and bear (Tank). Without the spec override
            // we can't tell — default to DPS; bear tanks set "feral-bear-druid".
            return Role::Damage;

        case CLASS_HUNTER:
        case CLASS_ROGUE:
        case CLASS_MAGE:
        case CLASS_WARLOCK:
            return Role::Damage;

        default:
            return Role::None;
    }
}

Role Detect(Player* bot, std::string const& specOverride, AltbotRoleOverride roleOverride)
{
    if (!bot)
        return Role::None;

    switch (roleOverride)
    {
        case AltbotRoleOverride::Tank:
        case AltbotRoleOverride::MainTank: return Role::Tank;
        case AltbotRoleOverride::Healer:   return Role::Healer;
        case AltbotRoleOverride::Damage:   return Role::Damage;
        case AltbotRoleOverride::None:     break;
    }

    if (!specOverride.empty())
        return FromSpecSlug(specOverride);

    uint32 tree = bot->GetPrimaryTalentTree(bot->GetActiveSpec());
    return FromClassAndTree(bot->getClass(), tree);
}

uint8_t ToLfgRoleMask(Role role)
{
    switch (role)
    {
        case Role::Tank:   return lfg::PLAYER_ROLE_TANK;
        case Role::Healer: return lfg::PLAYER_ROLE_HEALER;
        case Role::Damage: return lfg::PLAYER_ROLE_DAMAGE;
        case Role::None:
        default:           return lfg::PLAYER_ROLE_NONE;
    }
}

} // namespace AltbotRole

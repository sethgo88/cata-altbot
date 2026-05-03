#pragma once
#include "AltbotState.h"
#include <cstdint>
#include <string>

class Player;

namespace AltbotRole
{
    enum class Role : uint8_t
    {
        None   = 0,
        Tank   = 1,
        Healer = 2,
        Damage = 3,
    };

    // Resolves the bot's role. Precedence: explicit roleOverride (incl. MainTank
    // → Tank) > spec slug override > class + active talent tree. Cata druid
    // Feral falls back to Damage (cat) when no override is set — bear tanks
    // need an explicit role or spec override.
    Role Detect(Player* bot, std::string const& specOverride,
                AltbotRoleOverride roleOverride = AltbotRoleOverride::None);

    // PLAYER_ROLE_TANK / HEALER / DAMAGE bitmask from lfg::LfgRoles.
    // Returns 0 (PLAYER_ROLE_NONE) for Role::None.
    uint8_t ToLfgRoleMask(Role role);
}

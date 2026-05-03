#include "AltbotLfg.h"
#include "AltbotAI.h"
#include "AltbotRole.h"
#include "Group.h"
#include "LFG.h"
#include "LFGMgr.h"
#include "Player.h"

namespace AltbotLfg
{

void Tick(Player* bot, AltbotAI* ai)
{
    if (!bot || !ai)
        return;

    lfg::LfgState state = sLFGMgr->GetState(bot->GetGUID());

    if (state != lfg::LFG_STATE_ROLECHECK)
    {
        // Not in a rolecheck — clear the latch so the next one fires.
        ai->ClearLfgRoleResponded();
        return;
    }

    if (ai->HasLfgRoleResponded())
        return;

    Group* group = bot->GetGroup();
    if (!group)
        return;

    AltbotRole::Role role = AltbotRole::Detect(bot, ai->GetSpecOverride(),
                                               ai->GetState().roleOverride);
    uint8 mask = AltbotRole::ToLfgRoleMask(role);

    // CanPerformSelectedRoles validates Tank/Healer eligibility by class.
    // Damage is always allowed; for None we abort (no spec/class match).
    if (mask == lfg::PLAYER_ROLE_NONE)
        return;
    if (!sLFGMgr->CanPerformSelectedRoles(bot->getClass(), mask))
        return;

    sLFGMgr->UpdateRoleCheck(group->GetGUID(), bot->GetGUID(), mask);
    ai->MarkLfgRoleResponded();
}

} // namespace AltbotLfg

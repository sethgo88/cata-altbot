#include "AltbotLfg.h"
#include "AltbotAI.h"
#include "AltbotRole.h"
#include "Group.h"
#include "LFG.h"
#include "LFGMgr.h"
#include "Log.h"
#include "Player.h"

namespace AltbotLfg
{

static void HandleRoleCheck(Player* bot, AltbotAI* ai)
{
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

    // UpdateRoleCheck with a non-zero role mask both submits the role and
    // accepts the rolecheck — Cata uses a single CMSG_LFG_SET_ROLES packet
    // for the combined "choose + accept" step.
    TC_LOG_INFO("lfg.altbot", "Altbot %s auto-accepting rolecheck with role mask 0x%x",
                bot->GetName().c_str(), mask);
    sLFGMgr->UpdateRoleCheck(group->GetGUID(), bot->GetGUID(), mask);
    ai->MarkLfgRoleResponded();
}

static void HandleProposal(Player* bot, AltbotAI* ai)
{
    if (ai->HasLfgProposalResponded())
        return;

    uint32 proposalId = sLFGMgr->GetPendingProposalIdForPlayer(bot->GetGUID());
    if (!proposalId)
    {
        TC_LOG_INFO("lfg.altbot", "Altbot %s state=PROPOSAL but no pending proposal id found",
                    bot->GetName().c_str());
        return;
    }

    TC_LOG_INFO("lfg.altbot", "Altbot %s auto-accepting proposal %u",
                bot->GetName().c_str(), proposalId);
    sLFGMgr->UpdateProposal(proposalId, bot->GetGUID(), true);
    ai->MarkLfgProposalResponded();
}

void Tick(Player* bot, AltbotAI* ai)
{
    if (!bot || !ai)
        return;

    lfg::LfgState state = sLFGMgr->GetState(bot->GetGUID());

    // Reset latches as soon as the bot transitions out of the matching state
    // so the next rolecheck/proposal cycle fires cleanly.
    if (state != lfg::LFG_STATE_ROLECHECK)
        ai->ClearLfgRoleResponded();
    if (state != lfg::LFG_STATE_PROPOSAL)
        ai->ClearLfgProposalResponded();

    switch (state)
    {
        case lfg::LFG_STATE_ROLECHECK: HandleRoleCheck(bot, ai); break;
        case lfg::LFG_STATE_PROPOSAL:  HandleProposal(bot, ai);  break;
        default:                                                 break;
    }
}

} // namespace AltbotLfg

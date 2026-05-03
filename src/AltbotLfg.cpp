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
    {
        TC_LOG_ERROR("altbot", "AltbotLfg: '%s' in rolecheck but has no group",
                     bot->GetName().c_str());
        return;
    }

    AltbotRole::Role role = AltbotRole::Detect(bot, ai->GetSpecOverride(),
                                               ai->GetState().roleOverride);
    uint8 mask = AltbotRole::ToLfgRoleMask(role);

    // CanPerformSelectedRoles validates Tank/Healer eligibility by class.
    // Damage is always allowed; for None we abort (no spec/class match).
    if (mask == lfg::PLAYER_ROLE_NONE)
    {
        TC_LOG_ERROR("altbot", "AltbotLfg: '%s' role detect returned None (class %u tree %u override-spec='%s' override-role=%u)",
                     bot->GetName().c_str(), uint32(bot->getClass()),
                     bot->GetPrimaryTalentTree(bot->GetActiveSpec()),
                     ai->GetSpecOverride().c_str(),
                     uint32(ai->GetState().roleOverride));
        return;
    }
    if (!sLFGMgr->CanPerformSelectedRoles(bot->getClass(), mask))
    {
        TC_LOG_ERROR("altbot", "AltbotLfg: '%s' class %u cannot perform role mask 0x%x",
                     bot->GetName().c_str(), uint32(bot->getClass()), mask);
        return;
    }

    // UpdateRoleCheck with a non-zero role mask both submits the role and
    // accepts the rolecheck — Cata uses a single CMSG_LFG_SET_ROLES packet
    // for the combined "choose + accept" step.
    TC_LOG_INFO("altbot", "AltbotLfg: '%s' auto-accepting rolecheck with role mask 0x%x",
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
        TC_LOG_ERROR("altbot", "AltbotLfg: '%s' state=PROPOSAL but no pending proposal id found",
                     bot->GetName().c_str());
        return;
    }

    TC_LOG_INFO("altbot", "AltbotLfg: '%s' auto-accepting proposal %u",
                bot->GetName().c_str(), proposalId);
    sLFGMgr->UpdateProposal(proposalId, bot->GetGUID(), true);
    ai->MarkLfgProposalResponded();
}

void Tick(Player* bot, AltbotAI* ai)
{
    if (!bot || !ai)
        return;

    lfg::LfgState state = sLFGMgr->GetState(bot->GetGUID());

    // Log state transitions on the "altbot" channel — visible when the user
    // configures Logger.altbot=3,Console Server. Hidden under default config.
    if (state != ai->GetLastLfgState())
    {
        TC_LOG_INFO("altbot", "AltbotLfg: '%s' lfg state %u -> %u",
                    bot->GetName().c_str(),
                    uint32(ai->GetLastLfgState()), uint32(state));
        ai->SetLastLfgState(state);
    }

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

#include "AltbotInvite.h"
#include "AltbotAI.h"
#include "Chat.h"
#include "Group.h"
#include "GroupMgr.h"
#include "Log.h"
#include "Player.h"
#include "WorldSession.h"

namespace AltbotInvite
{

static Player* GetBot(AltbotAI* ai)
{
    return ai ? ai->GetSession()->GetPlayer() : nullptr;
}

bool Invite(Player* master, AltbotAI* ai)
{
    Player* bot = GetBot(ai);
    if (!bot)
        return false;

    Group* group = master->GetGroup();
    if (!group)
    {
        // Create a new group with master as leader.
        group = new Group();
        if (!group->Create(master))
        {
            delete group;
            TC_LOG_ERROR("altbot", "AltbotInvite::Invite: Group::Create failed for master '%s'.",
                master->GetName().c_str());
            return false;
        }
        sGroupMgr->AddGroup(group);
    }

    if (group->IsMember(bot->GetGUID()))
    {
        ChatHandler(master->GetSession()).PSendSysMessage("%s is already in your group.",
            bot->GetName().c_str());
        return true;   // idempotent — already in
    }

    if (group->IsFull())
    {
        ChatHandler(master->GetSession()).SendSysMessage("Your group is full.");
        return false;
    }

    if (!group->AddMember(bot))
    {
        ChatHandler(master->GetSession()).PSendSysMessage("Failed to add %s to your group.",
            bot->GetName().c_str());
        return false;
    }

    ChatHandler(master->GetSession()).PSendSysMessage("%s joined your group.",
        bot->GetName().c_str());
    return true;
}

bool Uninvite(Player* master, AltbotAI* ai)
{
    Player* bot = GetBot(ai);
    if (!bot)
        return false;

    Group* group = master->GetGroup();
    if (!group || !group->IsMember(bot->GetGUID()))
    {
        ChatHandler(master->GetSession()).PSendSysMessage("%s is not in your group.",
            bot->GetName().c_str());
        return true;   // idempotent
    }

    group->RemoveMember(bot->GetGUID());

    ChatHandler(master->GetSession()).PSendSysMessage("%s left your group.",
        bot->GetName().c_str());
    return true;
}

bool Summon(Player* master, AltbotAI* ai)
{
    Player* bot = GetBot(ai);
    if (!bot)
        return false;

    if (!bot->IsInWorld())
    {
        ChatHandler(master->GetSession()).PSendSysMessage("%s is not in the world yet.",
            bot->GetName().c_str());
        return false;
    }

    bool ok = bot->TeleportTo(master->GetMapId(),
                              master->GetPositionX(),
                              master->GetPositionY(),
                              master->GetPositionZ(),
                              master->GetOrientation());

    if (!ok)
    {
        ChatHandler(master->GetSession()).PSendSysMessage("Failed to summon %s.",
            bot->GetName().c_str());
        return false;
    }

    ChatHandler(master->GetSession()).PSendSysMessage("%s summoned to your location.",
        bot->GetName().c_str());
    return true;
}

} // namespace AltbotInvite

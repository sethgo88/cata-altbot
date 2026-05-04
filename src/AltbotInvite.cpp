#include "AltbotInvite.h"
#include "AltbotAI.h"
#include "Chat.h"
#include "Group.h"
#include "GroupMgr.h"
#include "Log.h"
#include "MotionMaster.h"
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

    // Bots have no real client to ack teleports. A previous TeleportTo can
    // leave the bot stuck in semaphore-teleport state — every subsequent call
    // then bails out with "already being teleported" and the bot never moves.
    // Forcibly clear the stuck state before attempting a fresh teleport.
    if (bot->IsBeingTeleported())
    {
        bot->SetSemaphoreTeleportNear(false);
        bot->SetSemaphoreTeleportFar(false);
    }

    // Break any in-progress cast — a 2.5s Frostbolt or 3s Healing Wave
    // arriving at the new position can SPELL_FAILED_MOVING after the
    // relocate, and the cast spell-target check may also reference a
    // unit no longer near the bot.
    bot->InterruptNonMeleeSpells(false);

    // Drop combat with whatever the bot was engaged with at the old
    // location. Without this, the strategy's next combat tick sees the
    // master's old target still at range from the new position, fires
    // MaintainRange, and the bot immediately MoveChases back toward the
    // pull — making the summon effectively a no-op visually. Master's
    // *current* target is re-acquired naturally on the next combat tick
    // if combat is still active.
    bot->CombatStop();
    bot->AttackStop();

    // Drop any in-flight motion (follow path, chase, etc.) so the bot doesn't
    // immediately start walking back toward where it was.
    bot->GetMotionMaster()->Clear();

    Position destPos(master->GetPositionX(),
                     master->GetPositionY(),
                     master->GetPositionZ(),
                     master->GetOrientation());

    bool sameMap = (bot->GetMapId() == master->GetMapId());

    if (sameMap)
    {
        // Player::NearTeleportTo only updates m_teleport_dest and waits for the
        // client to send MSG_MOVE_TELEPORT_ACK before actually relocating. A bot
        // session has no client to produce that ack, so the bot stays put and
        // only the saved destination changes (which is why a logout/login
        // appears to "summon" — SaveToDB picks up m_teleport_dest). Replicate
        // the non-player branch of Unit::NearTeleportTo instead: announce the
        // teleport to nearby observers, relocate server-side, refresh
        // visibility for the new position.
        bot->SendTeleportPacket(destPos);
        bot->UpdatePosition(destPos, true);
        bot->UpdateObjectVisibility();
    }
    else
    {
        bool ok = bot->TeleportTo(master->GetMapId(),
                                  destPos.GetPositionX(),
                                  destPos.GetPositionY(),
                                  destPos.GetPositionZ(),
                                  destPos.GetOrientation());

        if (!ok)
        {
            ChatHandler(master->GetSession()).PSendSysMessage("Failed to summon %s.",
                bot->GetName().c_str());
            return false;
        }

        // Cross-map: bot's session will never receive MSG_MOVE_WORLDPORT_ACK.
        // Drive the server-side overload directly so the map swap finalizes.
        if (bot->IsBeingTeleportedFar())
            bot->GetSession()->HandleMoveWorldportAck();
    }

    ChatHandler(master->GetSession()).PSendSysMessage("%s summoned to your location.",
        bot->GetName().c_str());
    return true;
}

} // namespace AltbotInvite

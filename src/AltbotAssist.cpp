#include "AltbotAssist.h"
#include "AltbotState.h"
#include "Group.h"
#include "ObjectAccessor.h"
#include "Player.h"
#include "Unit.h"

namespace AltbotAssist
{

static Unit* GetSkullTarget(Player* viewer)
{
    Group* group = viewer->GetGroup();
    if (!group)
        return nullptr;

    // TODO: TC 4.3.4 - Group has no public GetTargetIcons() accessor.
    // m_targetIcons[8] is private with only SetTargetIcon()/SendTargetIconList()
    // as public API. Skull targeting is disabled until a getter is exposed
    // (e.g., add GetTargetIcon(uint8) to Group.h in server-core).
    return nullptr;
}

static Unit* GetMasterTarget(Player* master)
{
    if (Unit* victim = master->GetVictim())
        return victim;

    return ObjectAccessor::GetUnit(*master, master->GetTarget());
}

static bool IsValidEnemy(Player* bot, Unit* target)
{
    return target
        && target->IsAlive()
        && bot->IsValidAttackTarget(target);
}

Unit* SelectTarget(Player* bot, Player* master, AltbotAssistMode mode)
{
    if (mode == AltbotAssistMode::Off)
        return nullptr;

    if (mode == AltbotAssistMode::SkullOnly || mode == AltbotAssistMode::Both)
        if (Unit* skull = GetSkullTarget(master); IsValidEnemy(bot, skull))
            return skull;

    if (mode == AltbotAssistMode::MasterTarget || mode == AltbotAssistMode::Both)
        if (Unit* mt = GetMasterTarget(master); IsValidEnemy(bot, mt))
            return mt;

    return nullptr;
}

} // namespace AltbotAssist

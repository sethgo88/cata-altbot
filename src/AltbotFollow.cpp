#include "AltbotFollow.h"
#include "Log.h"
#include "MotionMaster.h"
#include "Player.h"
#include <cmath>

static constexpr float FOLLOW_DIST       = 3.0f;
static constexpr float FOLLOW_ANGLE      = float(M_PI);
static constexpr float TELEPORT_THRESHOLD = 100.0f;

namespace AltbotFollow
{

void Update(Player* bot, Player* master)
{
    TC_LOG_DEBUG("altbot", "AltbotFollow::Update bot='%s' botMap=%u masterMap=%u dist=%.1f",
                 bot->GetName().c_str(),
                 bot->GetMapId(), master->GetMapId(),
                 bot->GetMapId() == master->GetMapId() ? bot->GetDistance(master) : -1.0f);

    // Different map — hard teleport
    if (bot->GetMapId() != master->GetMapId())
    {
        TC_LOG_INFO("altbot", "AltbotFollow: '%s' cross-map teleport botMap=%u -> masterMap=%u",
                    bot->GetName().c_str(), bot->GetMapId(), master->GetMapId());

        bool ok = bot->TeleportTo(master->GetMapId(),
                                  master->GetPositionX(), master->GetPositionY(),
                                  master->GetPositionZ(), master->GetOrientation());

        TC_LOG_INFO("altbot", "AltbotFollow: TeleportTo returned %s", ok ? "true" : "false");
        return;
    }

    float dist = bot->GetDistance(master);

    if (dist > TELEPORT_THRESHOLD)
    {
        TC_LOG_INFO("altbot", "AltbotFollow: '%s' long-range teleport dist=%.1f",
                    bot->GetName().c_str(), dist);

        bool ok = bot->TeleportTo(master->GetMapId(),
                                  master->GetPositionX(), master->GetPositionY(),
                                  master->GetPositionZ(), master->GetOrientation());

        TC_LOG_INFO("altbot", "AltbotFollow: TeleportTo returned %s", ok ? "true" : "false");
        return;
    }

    bot->GetMotionMaster()->MoveFollow(master, FOLLOW_DIST, FOLLOW_ANGLE);
}

} // namespace AltbotFollow

#pragma once

class Player;
class AltbotAI;

namespace AltbotLfg
{
    // Called from AltbotAI::Update. If the bot is currently in an LFG/LFR
    // rolecheck and hasn't responded yet, picks the role matching its spec
    // and submits it via LFGMgr::UpdateRoleCheck. Idempotent per rolecheck.
    void Tick(Player* bot, AltbotAI* ai);
}

#pragma once

class Player;
class AltbotAI;

namespace AltbotInvite
{
    // Adds the bot to the master's group (creates one if master has none).
    // Bypasses the SMSG_GROUP_INVITE / accept handshake — server-side direct add.
    bool Invite  (Player* master, AltbotAI* ai);

    // Removes the bot from the master's group.
    bool Uninvite(Player* master, AltbotAI* ai);

    // Teleports the bot to the master's exact position (cross-map fallback covered).
    bool Summon  (Player* master, AltbotAI* ai);
}

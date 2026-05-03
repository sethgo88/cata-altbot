#include "AltbotCommandTable.h"
#include "AltbotAI.h"
#include "AltbotInventory.h"
#include "AltbotInvite.h"
#include "AltbotMgr.h"
#include "AltbotTalents.h"
#include "Chat.h"
#include "MotionMaster.h"
#include "ObjectAccessor.h"
#include "Player.h"
#include "Unit.h"
#include "WorldSession.h"
#include <algorithm>
#include <cctype>
#include <cstring>
#include <string>
#include <string_view>

namespace
{

using Handler = bool(*)(Player* master, AltbotAI* ai, std::string_view args);

struct WhisperCommand
{
    char const* verb;
    Handler     handler;
};

// ---- helpers ----

static Player* BotOf(AltbotAI* ai)
{
    return ai ? ai->GetSession()->GetPlayer() : nullptr;
}

static void Reply(Player* master, std::string const& msg)
{
    ChatHandler(master->GetSession()).PSendSysMessage("%s", msg.c_str());
}

static std::string Lower(std::string_view s)
{
    std::string out(s);
    std::transform(out.begin(), out.end(), out.begin(),
        [](unsigned char c) { return std::tolower(c); });
    return out;
}

// Trim leading/trailing spaces.
static std::string_view Trim(std::string_view s)
{
    while (!s.empty() && std::isspace(static_cast<unsigned char>(s.front()))) s.remove_prefix(1);
    while (!s.empty() && std::isspace(static_cast<unsigned char>(s.back())))  s.remove_suffix(1);
    return s;
}

// Parse "on"/"off"/"true"/"false"/"1"/"0"/"yes"/"no". Empty = toggle (caller decides).
// Returns: 1 = on, 0 = off, -1 = invalid.
static int ParseOnOff(std::string_view args)
{
    std::string a = Lower(Trim(args));
    if (a == "on" || a == "1" || a == "true" || a == "yes")  return 1;
    if (a == "off" || a == "0" || a == "false" || a == "no") return 0;
    return -1;
}

// Apply on/off semantics to a state field. Empty args = toggle current value.
static bool ApplyToggle(Player* master, AltbotAI* ai, std::string_view args,
                        std::string const& label,
                        bool AltbotState::* field)
{
    std::string_view trimmed = Trim(args);
    int parsed = trimmed.empty() ? -2 : ParseOnOff(trimmed);

    if (parsed == -1)
    {
        Reply(master, "Usage: " + label + " [on|off]");
        return true;
    }

    bool desired;
    if (parsed == -2)
        desired = !(ai->GetState().*field);   // toggle
    else
        desired = parsed == 1;

    ai->MutateState([&](AltbotState& s) { s.*field = desired; });
    Reply(master, label + " is now " + (desired ? "ON" : "OFF") + ".");
    return true;
}

// ---- Phase 3 handlers: movement, lifecycle, simple combat ----

static bool CmdFollow(Player* master, AltbotAI* ai, std::string_view)
{
    Player* bot = BotOf(ai);
    if (!bot) return true;

    ai->MutateState([](AltbotState& s) { s.mode = AltbotMode::Follow; });
    Reply(master, std::string(bot->GetName()) + " is now following you.");
    return true;
}

static bool CmdStay(Player* master, AltbotAI* ai, std::string_view)
{
    Player* bot = BotOf(ai);
    if (!bot) return true;

    ai->MutateState([](AltbotState& s) { s.mode = AltbotMode::Stay; });
    bot->GetMotionMaster()->MoveIdle();
    Reply(master, std::string(bot->GetName()) + " will stay put.");
    return true;
}

static bool CmdAttack(Player* master, AltbotAI* ai, std::string_view)
{
    Player* bot = BotOf(ai);
    if (!bot) return true;

    Unit* target = ObjectAccessor::GetUnit(*master, master->GetTarget());
    if (!target || !target->IsAlive())
    {
        Reply(master, "No valid target.");
        return true;
    }
    if (!bot->IsValidAttackTarget(target))
    {
        Reply(master, "That target is not attackable.");
        return true;
    }

    bot->Attack(target, true);
    Reply(master, std::string(bot->GetName()) + " is attacking " + target->GetName() + ".");
    return true;
}

static bool CmdInvite(Player* master, AltbotAI* ai, std::string_view)
{
    AltbotInvite::Invite(master, ai);   // Invite() handles its own user feedback
    return true;
}

static bool CmdUninvite(Player* master, AltbotAI* ai, std::string_view)
{
    AltbotInvite::Uninvite(master, ai);
    return true;
}

static bool CmdSummon(Player* master, AltbotAI* ai, std::string_view)
{
    AltbotInvite::Summon(master, ai);
    return true;
}

// ---- Phase 4 handlers: polled-behavior toggles ----

static bool CmdToggleMount(Player* m, AltbotAI* ai, std::string_view args)
{
    return ApplyToggle(m, ai, args, "Auto-mount", &AltbotState::autoMount);
}

static bool CmdToggleRelease(Player* m, AltbotAI* ai, std::string_view args)
{
    return ApplyToggle(m, ai, args, "Auto-release", &AltbotState::autoRelease);
}

static bool CmdToggleLoot(Player* m, AltbotAI* ai, std::string_view args)
{
    return ApplyToggle(m, ai, args, "Auto-loot", &AltbotState::autoLoot);
}

// ---- Phase 5 handlers: hook-driven toggles ----

static bool CmdTogglePass(Player* m, AltbotAI* ai, std::string_view args)
{
    return ApplyToggle(m, ai, args, "Auto-pass", &AltbotState::autoPass);
}

static bool CmdToggleQuestTake(Player* m, AltbotAI* ai, std::string_view args)
{
    return ApplyToggle(m, ai, args, "Auto-quest-take", &AltbotState::autoQuestTake);
}

static bool CmdToggleQuestTurnIn(Player* m, AltbotAI* ai, std::string_view args)
{
    return ApplyToggle(m, ai, args, "Auto-quest-turn-in", &AltbotState::autoQuestTurnIn);
}

// ---- Phase 6 handlers: inventory + talents ----

static bool CmdShowBags(Player* master, AltbotAI* ai, std::string_view)
{
    Player* bot = BotOf(ai);
    if (!bot) return true;

    auto items = AltbotInventory::ListBags(bot);
    if (items.empty())
    {
        Reply(master, std::string(bot->GetName()) + "'s bags are empty.");
        return true;
    }

    ChatHandler chat(master->GetSession());
    chat.PSendSysMessage("%s's bags (%zu item(s)):", bot->GetName().c_str(), items.size());
    for (auto const& it : items)
    {
        chat.PSendSysMessage("  [guid %u] %s x%u (bag %u slot %u)",
            it.guidLow, it.name.c_str(), it.count, it.bag, it.slot);
    }
    return true;
}

static bool CmdEquipItem(Player* master, AltbotAI* ai, std::string_view args)
{
    Player* bot = BotOf(ai);
    if (!bot) return true;
    AltbotInventory::EquipItem(master, bot, Trim(args));
    return true;
}

static bool CmdSellItem(Player* master, AltbotAI* ai, std::string_view args)
{
    Player* bot = BotOf(ai);
    if (!bot) return true;
    AltbotInventory::SellItem(master, bot, Trim(args));
    return true;
}

static bool CmdDropItem(Player* master, AltbotAI* ai, std::string_view args)
{
    Player* bot = BotOf(ai);
    if (!bot) return true;
    AltbotInventory::DestroyItem(master, bot, Trim(args));
    return true;
}

static bool CmdOpenTrade(Player* master, AltbotAI* ai, std::string_view)
{
    Player* bot = BotOf(ai);
    if (!bot) return true;
    AltbotInventory::OpenTrade(master, bot);
    return true;
}

static bool CmdShowTalents(Player* master, AltbotAI* ai, std::string_view)
{
    Player* bot = BotOf(ai);
    if (!bot) return true;
    AltbotTalents::Show(master, bot);
    return true;
}

static bool CmdLearnTalent(Player* master, AltbotAI* ai, std::string_view args)
{
    Player* bot = BotOf(ai);
    if (!bot) return true;
    AltbotTalents::LearnFromArgs(master, bot, Trim(args));
    return true;
}

static bool CmdSetSpec(Player* master, AltbotAI* ai, std::string_view args)
{
    Player* bot = BotOf(ai);
    if (!bot) return true;

    std::string a = Lower(Trim(args));

    std::string slug;
    std::string display;

    if (a.empty() || a == "auto")
    {
        slug    = "";
        display = "auto-detect";
    }
    else if (a == "resto" || a == "restoration" || a == "resto-shaman")
    {
        slug    = "resto-shaman";
        display = "Restoration Shaman";
    }
    else if (a == "elemental" || a == "ele")
    {
        slug    = "elemental";
        display = "Elemental";
    }
    else if (a == "enhance" || a == "enhancement")
    {
        slug    = "enhancement";
        display = "Enhancement";
    }
    else
    {
        Reply(master, "Usage: spec [auto|resto|elemental|enhancement]");
        return true;
    }

    sAltbotMgr->SetBotSpec(master->GetGUID(), bot->GetGUID(), slug);
    Reply(master, "Altbot spec set to: " + display + " (applies on next combat tick).");
    return true;
}

static bool CmdSetAssist(Player* master, AltbotAI* ai, std::string_view args)
{
    std::string mode = Lower(Trim(args));
    AltbotAssistMode m;
    if      (mode == "off")    m = AltbotAssistMode::Off;
    else if (mode == "target") m = AltbotAssistMode::MasterTarget;
    else if (mode == "skull")  m = AltbotAssistMode::SkullOnly;
    else if (mode == "both")   m = AltbotAssistMode::Both;
    else
    {
        Reply(master, "Usage: assist off|target|skull|both");
        return true;
    }

    ai->MutateState([m](AltbotState& s) { s.assist = m; });
    Reply(master, "Assist mode set to '" + mode + "'.");
    return true;
}

static bool CmdHelp(Player* master, AltbotAI* /*ai*/, std::string_view)
{
    Reply(master,
        "Altbot whisper commands:");
    Reply(master,
        "  follow, stay, attack, invite, uninvite, come, help");
    Reply(master,
        "  mount/release/loot/pass/questtake/questturnin [on|off]  (toggle if no arg)");
    Reply(master,
        "  assist off|target|skull|both");
    Reply(master,
        "  bags, equip <name|guid>, sell <name|guid>, drop <name|guid>, trade");
    Reply(master,
        "  talents, learn <talentId> <rank>");
    Reply(master,
        "  spec [auto|resto|elemental|enhancement]");
    return true;
}

// ---- table ----

static constexpr WhisperCommand kCommands[] = {
    // Phase 3
    {"follow",      CmdFollow},
    {"stay",        CmdStay},
    {"attack",      CmdAttack},
    {"invite",      CmdInvite},
    {"uninvite",    CmdUninvite},
    {"come",        CmdSummon},
    {"summon",      CmdSummon},   // alias
    {"help",        CmdHelp},
    // Phase 4
    {"mount",       CmdToggleMount},
    {"release",     CmdToggleRelease},
    {"loot",        CmdToggleLoot},
    {"assist",      CmdSetAssist},
    // Phase 5
    {"pass",        CmdTogglePass},
    {"questtake",   CmdToggleQuestTake},
    {"questturnin", CmdToggleQuestTurnIn},
    // Phase 6
    {"bags",        CmdShowBags},
    {"equip",       CmdEquipItem},
    {"sell",        CmdSellItem},
    {"drop",        CmdDropItem},
    {"trade",       CmdOpenTrade},
    {"talents",     CmdShowTalents},
    {"learn",       CmdLearnTalent},
    {"spec",        CmdSetSpec},
};

} // namespace

bool AltbotCommandTable::Dispatch(Player* master, AltbotAI* ai, std::string_view msg)
{
    if (!master || msg.empty())
        return false;

    auto space = msg.find(' ');
    std::string_view verbView = (space == std::string_view::npos) ? msg : msg.substr(0, space);
    std::string_view args     = (space == std::string_view::npos) ? std::string_view{} : msg.substr(space + 1);

    std::string verb = Lower(verbView);

    for (auto const& cmd : kCommands)
    {
        if (verb == cmd.verb)
            return cmd.handler(master, ai, args);
    }
    return false;
}

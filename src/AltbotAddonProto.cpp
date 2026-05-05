#include "AltbotAddonProto.h"
#include "AltbotAI.h"
#include "AltbotAccountLink.h"
#include "AltbotInventory.h"
#include "AltbotInvite.h"
#include "AltbotMgr.h"
#include "AltbotQuest.h"
#include "AltbotTalents.h"
#include "AccountMgr.h"
#include "Chat.h"
#include "Creature.h"
#include "Group.h"
#include "Log.h"
#include "ObjectAccessor.h"
#include "Player.h"
#include "Unit.h"
#include "WorldSession.h"
#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <functional>
#include <sstream>
#include <string>
#include <unordered_set>
#include <vector>

namespace AltbotAddonProto
{

// ---- helpers ----

static std::vector<std::string> Split(std::string_view s, char delim)
{
    std::vector<std::string> out;
    size_t start = 0;
    for (size_t i = 0; i <= s.size(); ++i)
    {
        if (i == s.size() || s[i] == delim)
        {
            out.emplace_back(s.substr(start, i - start));
            start = i + 1;
        }
    }
    return out;
}

static AltbotAI* ResolveBotByGuidOrName(Player* master, std::string const& token)
{
    if (token.empty())
        return nullptr;

    // Numeric token = bot guid-low.
    bool numeric = !token.empty() && std::all_of(token.begin(), token.end(),
        [](unsigned char c) { return std::isdigit(c) != 0; });

    if (numeric)
    {
        uint32 low = static_cast<uint32>(std::strtoul(token.c_str(), nullptr, 10));
        ObjectGuid guid(HighGuid::Player, low);
        return sAltbotMgr->FindBotAI(master->GetGUID(), guid);
    }

    return sAltbotMgr->FindBotByName(master->GetGUID(), token);
}

void Send(Player* sender, Player* master, std::string const& payload)
{
    // `sender` is whichever Player object delivers the whisper; usually one of
    // master's bots, but during bootstrap (no bots online) it's the master
    // themselves self-whispering. WoW's chat layer accepts master->Whisper(self),
    // so the message round-trips through CHAT_MSG_WHISPER on the master's
    // client and the addon parses it the same way.
    if (!sender || !master)
        return;

    // Escape literal `|` as `||` so the receiving client's chat-format parser
    // sees only valid escape sequences. The addon's Parse function reverses
    // this. Without escaping, `|ALT_ROW` looks like an invalid escape (`|A`)
    // and some clients reject the incoming whisper before it reaches Lua.
    std::string raw = std::string(PREFIX) + payload;
    std::string escaped;
    escaped.reserve(raw.size() + 16);
    for (char c : raw)
    {
        escaped.push_back(c);
        if (c == '|') escaped.push_back('|');
    }
    sender->Whisper(escaped, LANG_UNIVERSAL, master);
}

// Send via any of the master's active bots (first available). Used for
// responses where the AI for a specific bot might not be relevant
// (e.g., LIST replies aren't per-bot).
static void SendVia(Player* master, std::string const& payload)
{
    auto* mgr = sAltbotMgr;
    if (!mgr) return;

    std::vector<RegisteredBot> registered = mgr->ListRegistered(master->GetGUID());
    for (auto const& reg : registered)
    {
        AltbotAI* ai = mgr->FindBotAI(master->GetGUID(), reg.guid);
        if (ai)
        {
            if (Player* bot = ai->GetSession()->GetPlayer())
            {
                if (bot->IsInWorld())
                {
                    Send(bot, master, payload);
                    return;
                }
            }
        }
    }
    // No active bots — fall back to system message so the addon can at least log it.
    ChatHandler(master->GetSession()).PSendSysMessage("[CATABOT] %s", payload.c_str());
}

// Iterate every active bot owned by `master` and call `fn(ai, bot)` once per
// bot. Used by the broadcast (`*_ALL`) verbs so the UI can drive the whole
// roster with a single click.
static void ForEachActiveBot(Player* master, std::function<void(AltbotAI*, Player*)> const& fn)
{
    auto* mgr = sAltbotMgr;
    if (!mgr) return;

    for (auto const& reg : mgr->ListRegistered(master->GetGUID()))
    {
        AltbotAI* ai = mgr->FindBotAI(master->GetGUID(), reg.guid);
        if (!ai) continue;
        Player* bot = ai->GetSession()->GetPlayer();
        if (!bot || !bot->IsInWorld()) continue;
        fn(ai, bot);
    }
}

void PushState(AltbotAI const& ai)
{
    Player* master = ObjectAccessor::FindPlayer(ai.GetMasterGuid());
    if (!master)
        return;
    Player* bot = ai.GetSession()->GetPlayer();
    if (!bot)
        return;

    AltbotState const& s = ai.GetState();
    std::string spec = ai.GetSpecOverride();
    if (spec.empty()) spec = "auto";

    std::ostringstream out;
    out << "STATE|" << ai.GetBotGuid().GetCounter()
        << "|mode="    << uint32(s.mode)
        << ";assist="  << uint32(s.assist)
        << ";role="    << uint32(s.roleOverride)
        << ";loot="    << (s.autoLoot         ? 1 : 0)
        << ";roll="    << uint32(s.lootRoll)
        << ";mount="   << (s.autoMount        ? 1 : 0)
        << ";release=" << (s.autoRelease      ? 1 : 0)
        << ";qtake="   << (s.autoQuestTake    ? 1 : 0)
        << ";qturn="   << (s.autoQuestTurnIn  ? 1 : 0)
        << ";spec="    << spec;

    Send(bot, master, out.str());
}

// ---- per-verb handlers (master is the sender; ai may be null when the
// inbound whisper went to a bot that doesn't carry semantic meaning,
// e.g., LIST. We resolve specific-bot args internally where needed.) ----

static void DoHello(Player* master, Player* bot)
{
    std::ostringstream out;
    out << "HELLO_OK|" << PROTO_VERSION;
    Send(bot, master, out.str());
}

static void DoList(Player* master, Player* bot)
{
    auto alts = sAltbotMgr->ListAvailableAlts(master);
    for (auto const& a : alts)
    {
        std::ostringstream row;
        row << "ALT_ROW|" << a.guid.GetCounter()
            << "|" << a.name
            << "|" << uint32(a.classId)
            << "|" << uint32(a.level)
            << "|" << a.accountId
            << "|" << (a.registered ? 1 : 0)
            << "|" << (a.active     ? 1 : 0);
        Send(bot, master, row.str());
    }

    std::ostringstream done;
    done << "LIST_DONE|" << alts.size();
    Send(bot, master, done.str());

    // Push current STATE for each active bot so the addon UI shows live values
    // immediately on open instead of waiting for the first toggle change.
    for (auto const& a : alts)
    {
        if (!a.active) continue;
        if (AltbotAI* ai = sAltbotMgr->FindBotAI(master->GetGUID(), a.guid))
            PushState(*ai);
    }
}

static void DoLinks(Player* master, Player* bot)
{
    auto accounts = AltbotAccountLink::GetLinkedAccounts(master->GetSession()->GetAccountId());
    for (uint32 acc : accounts)
    {
        std::string username;
        AccountMgr::GetName(acc, username);
        std::ostringstream row;
        row << "LINK_ROW|" << username << "|" << acc;
        Send(bot, master, row.str());
    }
    std::ostringstream done;
    done << "LINKS_DONE|" << accounts.size();
    Send(bot, master, done.str());
}

static void DoBags(Player* master, Player* bot, std::vector<std::string> const& parts)
{
    if (parts.size() < 2) return;

    AltbotAI* targetAi = ResolveBotByGuidOrName(master, parts[1]);
    if (!targetAi) return;
    Player* target = targetAi->GetSession()->GetPlayer();
    if (!target) return;

    auto items = AltbotInventory::ListBags(target);
    for (auto const& it : items)
    {
        std::ostringstream row;
        row << "BAG_ROW|" << targetAi->GetBotGuid().GetCounter()
            << "|" << uint32(it.bag)
            << "|" << uint32(it.slot)
            << "|" << it.entry
            << "|" << it.count
            << "|" << it.guidLow;
        Send(bot, master, row.str());
    }
    std::ostringstream done;
    done << "BAGS_DONE|" << targetAi->GetBotGuid().GetCounter() << "|" << items.size();
    Send(bot, master, done.str());
}

// Single-line `STATS|<botGuid>|kv;kv;...` snapshot, organized into the same
// categories the in-game character panel exposes (general / melee / ranged /
// spell / defense). The addon parses the kv blob and renders one category at a
// time. Fields are emitted unconditionally — empty/zero values just show as 0
// on the client, which matches Blizzard's panel behavior.
static void EmitStats(Player* master, Player* target, AltbotAI* targetAi, Player* via)
{
    if (!master || !target || !targetAi || !via)
        return;

    Powers ptype = target->GetPowerType();

    auto crit  = target->GetFloatValue(PLAYER_CRIT_PERCENTAGE);
    auto rcrit = target->GetFloatValue(PLAYER_RANGED_CRIT_PERCENTAGE);
    auto scrit = target->GetFloatValue(PLAYER_SPELL_CRIT_PERCENTAGE1);
    auto dodge = target->GetFloatValue(PLAYER_DODGE_PERCENTAGE);
    auto parry = target->GetFloatValue(PLAYER_PARRY_PERCENTAGE);
    auto block = target->GetFloatValue(PLAYER_BLOCK_PERCENTAGE);

    auto mainMin = target->GetFloatValue(UNIT_FIELD_MINDAMAGE);
    auto mainMax = target->GetFloatValue(UNIT_FIELD_MAXDAMAGE);
    auto rngMin  = target->GetFloatValue(UNIT_FIELD_MINRANGEDDAMAGE);
    auto rngMax  = target->GetFloatValue(UNIT_FIELD_MAXRANGEDDAMAGE);

    uint32 mainSpeed = target->GetBaseAttackTime(BASE_ATTACK);
    uint32 rngSpeed  = target->GetBaseAttackTime(RANGED_ATTACK);

    // Haste rating → percentage via the standard rating multiplier helper.
    float meleeHaste = target->GetRatingBonusValue(CR_HASTE_MELEE);
    float rngHaste   = target->GetRatingBonusValue(CR_HASTE_RANGED);
    float spellHaste = target->GetRatingBonusValue(CR_HASTE_SPELL);

    float meleeHit = target->GetRatingBonusValue(CR_HIT_MELEE);
    float rngHit   = target->GetRatingBonusValue(CR_HIT_RANGED);
    float spellHit = target->GetRatingBonusValue(CR_HIT_SPELL);

    float expertise   = target->GetUInt32Value(PLAYER_EXPERTISE) * 0.25f; // 4 expertise = 1%
    float resilience  = target->GetRatingBonusValue(CR_RESILIENCE_PLAYER_DAMAGE_TAKEN);
    float mastery     = target->GetFloatValue(PLAYER_MASTERY);

    // Spell power: max across schools — mirrors what the client panel shows.
    int32 spellPower = 0;
    for (uint32 sch = SPELL_SCHOOL_HOLY; sch < MAX_SPELL_SCHOOL; ++sch)
        spellPower = std::max<int32>(spellPower, target->SpellBaseDamageBonusDone(SpellSchoolMask(1 << sch)));

    std::ostringstream s;
    s << "STATS|" << targetAi->GetBotGuid().GetCounter() << "|"
      << "level="    << uint32(target->getLevel())
      << ";ilvl="    << uint32(target->GetAverageItemLevel())
      << ";hp="      << target->GetHealth()
      << ";maxhp="   << target->GetMaxHealth()
      << ";power="   << target->GetPower(ptype)
      << ";maxpower="<< target->GetMaxPower(ptype)
      << ";ptype="   << uint32(ptype)
      << ";str="     << target->GetStat(STAT_STRENGTH)
      << ";agi="     << target->GetStat(STAT_AGILITY)
      << ";sta="     << target->GetStat(STAT_STAMINA)
      << ";int="     << target->GetStat(STAT_INTELLECT)
      << ";spi="     << target->GetStat(STAT_SPIRIT)
      << ";armor="   << target->GetArmor()
      << ";dodge="   << dodge
      << ";parry="   << parry
      << ";block="   << block
      << ";resil="   << resilience
      << ";ap="      << target->GetTotalAttackPowerValue(BASE_ATTACK)
      << ";mainmin=" << mainMin
      << ";mainmax=" << mainMax
      << ";mainspd=" << mainSpeed
      << ";meleehit="<< meleeHit
      << ";crit="    << crit
      << ";exp="     << expertise
      << ";meleehaste=" << meleeHaste
      << ";rap="     << target->GetTotalAttackPowerValue(RANGED_ATTACK)
      << ";rngmin="  << rngMin
      << ";rngmax="  << rngMax
      << ";rngspd="  << rngSpeed
      << ";rnghit="  << rngHit
      << ";rngcrit=" << rcrit
      << ";rnghaste="<< rngHaste
      << ";sp="      << spellPower
      << ";spellhit="<< spellHit
      << ";spellcrit="<< scrit
      << ";spellhaste="<< spellHaste
      << ";mastery=" << mastery
      << ";masteryrating=" << target->GetRatingBonusValue(CR_MASTERY);

    Send(via, master, s.str());
}

static void DoGear(Player* master, Player* bot, std::vector<std::string> const& parts)
{
    if (parts.size() < 2) return;

    AltbotAI* targetAi = ResolveBotByGuidOrName(master, parts[1]);
    if (!targetAi) return;
    Player* target = targetAi->GetSession()->GetPlayer();
    if (!target) return;

    auto items = AltbotInventory::ListEquipped(target);
    for (auto const& it : items)
    {
        std::ostringstream row;
        row << "GEAR_ROW|" << targetAi->GetBotGuid().GetCounter()
            << "|" << uint32(it.slot)
            << "|" << it.entry
            << "|" << it.enchantId
            << "|" << it.gem1
            << "|" << it.gem2
            << "|" << it.gem3
            << "|" << it.ilvl
            << "|" << it.guidLow;
        Send(bot, master, row.str());
    }

    EmitStats(master, target, targetAi, bot);

    std::ostringstream done;
    done << "GEAR_DONE|" << targetAi->GetBotGuid().GetCounter() << "|" << items.size();
    Send(bot, master, done.str());
}

static void ApplyToggleByKey(AltbotAI* ai, std::string_view key, bool value)
{
    ai->MutateState([&](AltbotState& s)
    {
        if      (key == "loot")    s.autoLoot         = value;
        else if (key == "mount")   s.autoMount        = value;
        else if (key == "release") s.autoRelease      = value;
        else if (key == "qtake")   s.autoQuestTake    = value;
        else if (key == "qturn")   s.autoQuestTurnIn  = value;
    });
}

// Loot-roll mode: tri-state, not a boolean — its own SET_ROLL verb instead
// of routing through SET_TOGGLE.
static AltbotLootRollMode ParseLootRollMode(std::string const& s)
{
    if (s == "pass")                          return AltbotLootRollMode::Pass;
    if (s == "de" || s == "disenchant")       return AltbotLootRollMode::Disenchant;
    return AltbotLootRollMode::Wait;
}

static void DoSetToggle(Player* master, Player* bot, std::vector<std::string> const& parts)
{
    if (parts.size() < 4) return;
    AltbotAI* targetAi = ResolveBotByGuidOrName(master, parts[1]);
    if (!targetAi) return;

    bool value = (parts[3] == "on" || parts[3] == "1");
    ApplyToggleByKey(targetAi, parts[2], value);
    PushState(*targetAi);

    (void)bot;
}

static void DoSetMode(Player* master, Player* bot, std::vector<std::string> const& parts)
{
    if (parts.size() < 3) return;
    AltbotAI* targetAi = ResolveBotByGuidOrName(master, parts[1]);
    if (!targetAi) return;

    AltbotMode mode = (parts[2] == "stay") ? AltbotMode::Stay : AltbotMode::Follow;
    targetAi->SetMode(mode);
    PushState(*targetAi);

    (void)bot;
}

static void DoSetAssist(Player* master, Player* bot, std::vector<std::string> const& parts)
{
    if (parts.size() < 3) return;
    AltbotAI* targetAi = ResolveBotByGuidOrName(master, parts[1]);
    if (!targetAi) return;

    AltbotAssistMode m;
    if      (parts[2] == "off")    m = AltbotAssistMode::Off;
    else if (parts[2] == "target") m = AltbotAssistMode::MasterTarget;
    else if (parts[2] == "skull")  m = AltbotAssistMode::SkullOnly;
    else                            m = AltbotAssistMode::Both;

    targetAi->MutateState([m](AltbotState& s) { s.assist = m; });
    PushState(*targetAi);

    (void)bot;
}

static void DoSetRoll(Player* master, Player* bot, std::vector<std::string> const& parts)
{
    if (parts.size() < 3) return;
    AltbotAI* targetAi = ResolveBotByGuidOrName(master, parts[1]);
    if (!targetAi) return;

    AltbotLootRollMode m = ParseLootRollMode(parts[2]);
    targetAi->MutateState([m](AltbotState& s) { s.lootRoll = m; });
    PushState(*targetAi);

    (void)bot;
}

static void DoLifecycle(Player* master, std::string const& verb, std::vector<std::string> const& parts)
{
    if (parts.size() < 2) return;
    std::string const& botName = parts[1];

    if      (verb == "ADD")    sAltbotMgr->AddAltbot   (master, botName);
    else if (verb == "REMOVE") sAltbotMgr->RemoveAltbot(master, botName);
    else if (verb == "LOGIN")  sAltbotMgr->LoginBot    (master, botName);
    else if (verb == "LOGOUT") sAltbotMgr->LogoutBot   (master, botName);
}

static void DoInviteVerb(Player* master, std::string const& verb, std::vector<std::string> const& parts)
{
    if (parts.size() < 2) return;
    AltbotAI* targetAi = ResolveBotByGuidOrName(master, parts[1]);
    if (!targetAi) return;

    if      (verb == "INVITE")   AltbotInvite::Invite  (master, targetAi);
    else if (verb == "UNINVITE") AltbotInvite::Uninvite(master, targetAi);
    else if (verb == "SUMMON")   AltbotInvite::Summon  (master, targetAi);
}

static void DoInventoryVerb(Player* master, std::string const& verb, std::vector<std::string> const& parts)
{
    if (parts.size() < 2) return;
    AltbotAI* targetAi = ResolveBotByGuidOrName(master, parts[1]);
    if (!targetAi) return;
    Player* bot = targetAi->GetSession()->GetPlayer();
    if (!bot) return;

    std::string arg = parts.size() >= 3 ? parts[2] : "";

    if      (verb == "EQUIP") AltbotInventory::EquipItem  (master, bot, arg);
    else if (verb == "SELL")  AltbotInventory::SellItem   (master, bot, arg);
    else if (verb == "DROP")  AltbotInventory::DestroyItem(master, bot, arg);
    else if (verb == "TRADE") AltbotInventory::OpenTrade  (master, bot);
}

static void DoLearnTalent(Player* master, std::vector<std::string> const& parts)
{
    if (parts.size() < 4) return;
    AltbotAI* targetAi = ResolveBotByGuidOrName(master, parts[1]);
    if (!targetAi) return;
    Player* bot = targetAi->GetSession()->GetPlayer();
    if (!bot) return;

    uint32 talentId = static_cast<uint32>(std::strtoul(parts[2].c_str(), nullptr, 10));
    uint32 rank     = static_cast<uint32>(std::strtoul(parts[3].c_str(), nullptr, 10));
    AltbotTalents::Learn(master, bot, talentId, rank);
}

// `auto` reverts to talent-tree detection; any other slug is forwarded as-is.
// AltbotStrategyFactory falls back to the generic combat loop on unknown slugs,
// so the server doesn't need to validate the dropdown's contents.
static void DoSetSpec(Player* master, std::vector<std::string> const& parts)
{
    if (parts.size() < 3) return;
    AltbotAI* targetAi = ResolveBotByGuidOrName(master, parts[1]);
    if (!targetAi) return;

    std::string slug = parts[2];
    if (slug == "auto") slug = "";
    sAltbotMgr->SetBotSpec(master->GetGUID(), targetAi->GetBotGuid(), slug);
    PushState(*targetAi);
}

// Manual single-corpse loot. The addon button reads UnitGUID("target") at click
// time and ships it as a "0x"-prefixed 64-bit hex string (WoW Lua format).
//
// We validate the target up front so the master gets immediate feedback in
// chat (ERR|NOT_LOOTABLE|<reason>) instead of watching the bot walk to a
// corpse it can't take. The actual walk + drain happens on the bot's tick
// (AltbotLoot::TickPending) once we set the pending guid.
static void DoLoot(Player* master, Player* via, std::vector<std::string> const& parts)
{
    if (parts.size() < 3) return;

    AltbotAI* targetAi = ResolveBotByGuidOrName(master, parts[1]);
    if (!targetAi) return;
    Player* bot = targetAi->GetSession()->GetPlayer();
    if (!bot || !bot->IsInWorld()) return;

    auto sendErr = [via, master](char const* code, char const* msg)
    {
        std::ostringstream out;
        out << "ERR|" << code << "|" << msg;
        Send(via, master, out.str());
    };

    // Lua's UnitGUID is "0x" + 16 hex chars. Strip the prefix and parse the
    // full 64-bit raw value — splitting into (HighGuid, counter) requires
    // decoding the entry-encoded high half for creatures, which the uint64
    // ctor sidesteps.
    std::string const& hex = parts[2];
    char const* p = hex.c_str();
    if (hex.size() > 2 && hex[0] == '0' && (hex[1] == 'x' || hex[1] == 'X'))
        p += 2;
    uint64 raw = std::strtoull(p, nullptr, 16);
    if (raw == 0)
    {
        sendErr("NO_TARGET", "no target selected");
        return;
    }

    ObjectGuid targetGuid(raw);
    Unit* unit = ObjectAccessor::GetUnit(*bot, targetGuid);
    Creature* creature = unit ? unit->ToCreature() : nullptr;
    if (!creature)
    {
        sendErr("NOT_LOOTABLE", "target is not a creature");
        return;
    }

    if (creature->IsAlive())
    {
        sendErr("NOT_LOOTABLE", "target is alive");
        return;
    }

    if (creature->GetMapId() != bot->GetMapId())
    {
        sendErr("NOT_LOOTABLE", "target is on a different map");
        return;
    }

    if (!creature->HasDynamicFlag(UNIT_DYNFLAG_LOOTABLE))
    {
        sendErr("NOT_LOOTABLE", "nothing to loot");
        return;
    }

    // Tap eligibility: if a recipient is recorded, the bot must be that
    // player or in their group. Null recipient means no tap was ever set
    // (rare — quest mobs / dispatched spawns) and we let it through.
    if (Player* recipient = creature->GetLootRecipient())
    {
        if (recipient != bot)
        {
            Group* botGroup       = bot->GetGroup();
            Group* recipientGroup = recipient->GetGroup();
            if (!botGroup || botGroup != recipientGroup)
            {
                sendErr("NOT_OWNER", "you don't own this kill");
                return;
            }
        }
    }

    targetAi->SetPendingLootTarget(targetGuid);
}

// Strip a "0x"-prefixed hex GUID string to its 64-bit value. Returns 0 if the
// string is empty or unparseable. Shared between QUEST_NPC and QUEST_NPC_ALL.
static uint64 ParseHexGuid(std::string const& hex)
{
    char const* p = hex.c_str();
    if (hex.size() > 2 && hex[0] == '0' && (hex[1] == 'x' || hex[1] == 'X'))
        p += 2;
    return std::strtoull(p, nullptr, 16);
}

// "QUEST_NPC|<bot>|<hexGuid>" — addon-driven retroactive accept/turn-in for
// every quest the targeted NPC offers / involves that the bot is eligible for.
// Mirrors AltbotQuest::InteractWithNpc; the helper handles range validation
// and replies via PSendSysMessage to master's chat.
static void DoQuestNpc(Player* master, std::vector<std::string> const& parts)
{
    if (parts.size() < 3) return;
    AltbotAI* targetAi = ResolveBotByGuidOrName(master, parts[1]);
    if (!targetAi) return;

    uint64 raw = ParseHexGuid(parts[2]);
    if (raw == 0)
    {
        ChatHandler(master->GetSession()).PSendSysMessage("Altbot: no NPC selected.");
        return;
    }

    AltbotQuest::InteractWithNpc(master, targetAi, ObjectGuid(raw));
}

// "QUEST_NPC_ALL|<hexGuid>" — broadcast version. Each active bot independently
// runs InteractWithNpc, so range/eligibility checks are per-bot (the helper
// emits a system message per bot regardless of outcome). Bots out of range or
// on a different map just print a "can't see / not close enough" line — that's
// useful feedback, not noise.
static void DoQuestNpcAll(Player* master, std::vector<std::string> const& parts)
{
    if (parts.size() < 2) return;

    uint64 raw = ParseHexGuid(parts[1]);
    if (raw == 0)
    {
        ChatHandler(master->GetSession()).PSendSysMessage("Altbot: no NPC selected.");
        return;
    }

    ObjectGuid npcGuid(raw);
    ForEachActiveBot(master, [master, npcGuid](AltbotAI* ai, Player*)
    {
        AltbotQuest::InteractWithNpc(master, ai, npcGuid);
    });
}

// Slug → AltbotRoleOverride. Mirrors AltbotCommandTable::ParseRoleArg so the
// addon dropdown and the .altbot role slash command accept the same vocabulary.
static void DoSetRole(Player* master, std::vector<std::string> const& parts)
{
    if (parts.size() < 3) return;
    AltbotAI* targetAi = ResolveBotByGuidOrName(master, parts[1]);
    if (!targetAi) return;

    std::string slug = parts[2];
    AltbotRoleOverride r = AltbotRoleOverride::None;
    if      (slug == "tank")                                     r = AltbotRoleOverride::Tank;
    else if (slug == "healer")                                   r = AltbotRoleOverride::Healer;
    else if (slug == "dps" || slug == "damage")                  r = AltbotRoleOverride::Damage;
    else if (slug == "main-tank" || slug == "maintank" || slug == "mt") r = AltbotRoleOverride::MainTank;
    else                                                          r = AltbotRoleOverride::None;

    targetAi->MutateState([r](AltbotState& s) { s.roleOverride = r; });
    targetAi->ClearLfgRoleResponded();
    PushState(*targetAi);
}

// ---- broadcast (`*_ALL`) handlers — drive every active bot at once ----

static void DoSetModeAll(Player* master, std::vector<std::string> const& parts)
{
    if (parts.size() < 2) return;
    AltbotMode mode = (parts[1] == "stay") ? AltbotMode::Stay : AltbotMode::Follow;

    ForEachActiveBot(master, [mode](AltbotAI* ai, Player*)
    {
        ai->SetMode(mode);
        PushState(*ai);
    });
}

static void DoSetAssistAll(Player* master, std::vector<std::string> const& parts)
{
    if (parts.size() < 2) return;
    AltbotAssistMode m;
    if      (parts[1] == "off")    m = AltbotAssistMode::Off;
    else if (parts[1] == "target") m = AltbotAssistMode::MasterTarget;
    else if (parts[1] == "skull")  m = AltbotAssistMode::SkullOnly;
    else                            m = AltbotAssistMode::Both;

    ForEachActiveBot(master, [m](AltbotAI* ai, Player*)
    {
        ai->MutateState([m](AltbotState& s) { s.assist = m; });
        PushState(*ai);
    });
}

static void DoSetRollAll(Player* master, std::vector<std::string> const& parts)
{
    if (parts.size() < 2) return;
    AltbotLootRollMode m = ParseLootRollMode(parts[1]);

    ForEachActiveBot(master, [m](AltbotAI* ai, Player*)
    {
        ai->MutateState([m](AltbotState& s) { s.lootRoll = m; });
        PushState(*ai);
    });
}

static void DoSetToggleAll(Player* master, std::vector<std::string> const& parts)
{
    if (parts.size() < 3) return;
    std::string key   = parts[1];
    bool        value = (parts[2] == "on" || parts[2] == "1");

    ForEachActiveBot(master, [&key, value](AltbotAI* ai, Player*)
    {
        ApplyToggleByKey(ai, key, value);
        PushState(*ai);
    });
}

static void DoAttackAll(Player* master)
{
    Unit* target = ObjectAccessor::GetUnit(*master, master->GetTarget());
    if (!target || !target->IsAlive())
        return;

    ForEachActiveBot(master, [target](AltbotAI*, Player* bot)
    {
        if (bot->IsValidAttackTarget(target))
            bot->Attack(target, true);
    });
}

static void DoInviteAll(Player* master, std::string const& verb)
{
    ForEachActiveBot(master, [&verb, master](AltbotAI* ai, Player*)
    {
        if      (verb == "INVITE_ALL")   AltbotInvite::Invite  (master, ai);
        else if (verb == "UNINVITE_ALL") AltbotInvite::Uninvite(master, ai);
        else if (verb == "SUMMON_ALL")   AltbotInvite::Summon  (master, ai);
    });
}

// ---- entry point ----

bool TryDispatch(Player* master, AltbotAI* ai, std::string_view msg)
{
    if (msg.size() < PREFIX_LEN)
        return false;
    if (msg.compare(0, PREFIX_LEN, PREFIX) != 0)
        return false;

    // Normalize `||` → `|`: the WoW client escapes literal pipes that way for
    // SendChatMessage's chat-format parser, and some client builds preserve
    // the escape on the wire while others collapse it. Handle both — without
    // this, our split-on-`|` would receive `["", "LIST"]` for `CATABOT||LIST`.
    std::string normalized(msg);
    {
        size_t pos = 0;
        while ((pos = normalized.find("||", pos)) != std::string::npos)
        {
            normalized.erase(pos, 1);
            ++pos;
        }
    }

    std::string_view payload = std::string_view(normalized).substr(PREFIX_LEN);
    auto parts = Split(payload, '|');
    if (parts.empty())
        return true;   // matched prefix; handled (as a no-op).

    std::string const& verb = parts[0];

    // Server-bound replies are echoed back through OnPlayerChat when we
    // self-whisper the master (bootstrap path). Drop them here so we don't
    // recursively dispatch our own outbound traffic — without this guard,
    // every ALT_ROW would loop into UNKNOWN_VERB → ERR → UNKNOWN_VERB → ...
    static std::unordered_set<std::string> const OUTBOUND_VERBS = {
        "HELLO_OK", "ALT_ROW", "LIST_DONE", "LINK_ROW", "LINKS_DONE",
        "STATE", "BAG_ROW", "BAGS_DONE", "GEAR_ROW", "GEAR_DONE", "STATS", "ERR"
    };
    if (OUTBOUND_VERBS.count(verb))
        return true;

    // Reply transport: prefer the bot we received from, fall back to the master
    // self-whispering (bootstrap path — no bots online yet). Either way the
    // payload arrives at the master client through CHAT_MSG_WHISPER.
    Player* transport = (ai && ai->GetSession()) ? ai->GetSession()->GetPlayer() : nullptr;
    if (!transport) transport = master;

    if (verb == "HELLO")
        DoHello(master, transport);
    else if (verb == "LIST")
        DoList(master, transport);
    else if (verb == "LINKS")
        DoLinks(master, transport);
    else if (verb == "BAGS")
        DoBags(master, transport, parts);
    else if (verb == "GEAR")
        DoGear(master, transport, parts);
    else if (verb == "ADD" || verb == "REMOVE" || verb == "LOGIN" || verb == "LOGOUT")
        DoLifecycle(master, verb, parts);
    else if (verb == "INVITE" || verb == "UNINVITE" || verb == "SUMMON")
        DoInviteVerb(master, verb, parts);
    else if (verb == "SET_MODE")
        DoSetMode(master, transport, parts);
    else if (verb == "SET_TOGGLE")
        DoSetToggle(master, transport, parts);
    else if (verb == "SET_ASSIST")
        DoSetAssist(master, transport, parts);
    else if (verb == "SET_ROLL")
        DoSetRoll(master, transport, parts);
    else if (verb == "EQUIP" || verb == "SELL" || verb == "DROP" || verb == "TRADE")
        DoInventoryVerb(master, verb, parts);
    else if (verb == "LEARN_TALENT")
        DoLearnTalent(master, parts);
    else if (verb == "SET_SPEC")
        DoSetSpec(master, parts);
    else if (verb == "SET_ROLE")
        DoSetRole(master, parts);
    else if (verb == "LOOT")
        DoLoot(master, transport, parts);
    else if (verb == "QUEST_NPC")
        DoQuestNpc(master, parts);
    else if (verb == "QUEST_NPC_ALL")
        DoQuestNpcAll(master, parts);
    else if (verb == "SET_MODE_ALL")
        DoSetModeAll(master, parts);
    else if (verb == "SET_ASSIST_ALL")
        DoSetAssistAll(master, parts);
    else if (verb == "SET_ROLL_ALL")
        DoSetRollAll(master, parts);
    else if (verb == "SET_TOGGLE_ALL")
        DoSetToggleAll(master, parts);
    else if (verb == "ATTACK_ALL")
        DoAttackAll(master);
    else if (verb == "INVITE_ALL" || verb == "UNINVITE_ALL" || verb == "SUMMON_ALL")
        DoInviteAll(master, verb);
    else
    {
        std::ostringstream err;
        err << "ERR|UNKNOWN_VERB|" << verb;
        Send(transport, master, err.str());
    }

    return true;
}

} // namespace AltbotAddonProto

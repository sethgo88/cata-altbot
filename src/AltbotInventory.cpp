#include "AltbotInventory.h"
#include "Bag.h"
#include "Chat.h"
#include "Item.h"
#include "ItemTemplate.h"
#include "Log.h"
#include "ObjectMgr.h"
#include "Player.h"
#include "TradeData.h"
#include "WorldSession.h"
#include <algorithm>
#include <cctype>
#include <cstdlib>

namespace AltbotInventory
{

static std::string LowerCopy(std::string_view s)
{
    std::string out(s);
    std::transform(out.begin(), out.end(), out.begin(),
        [](unsigned char c) { return std::tolower(c); });
    return out;
}

static bool IsAllDigits(std::string_view s)
{
    if (s.empty()) return false;
    return std::all_of(s.begin(), s.end(),
        [](unsigned char c) { return std::isdigit(c) != 0; });
}

std::vector<BagItem> ListBags(Player* bot)
{
    std::vector<BagItem> out;
    if (!bot)
        return out;

    // Backpack (INVENTORY_SLOT_BAG_0) + bag slots 19-22.
    auto pushItem = [&](Item* item, uint8 bag, uint8 slot)
    {
        if (!item)
            return;
        ItemTemplate const* tpl = item->GetTemplate();
        if (!tpl)
            return;
        BagItem row;
        row.bag     = bag;
        row.slot    = slot;
        row.entry   = item->GetEntry();
        row.count   = item->GetCount();
        row.guidLow = item->GetGUID().GetCounter();
        row.name    = tpl->Name1;
        out.push_back(std::move(row));
    };

    // Backpack
    for (uint8 slot = INVENTORY_SLOT_ITEM_START; slot < INVENTORY_SLOT_ITEM_END; ++slot)
        pushItem(bot->GetItemByPos(INVENTORY_SLOT_BAG_0, slot),
                 INVENTORY_SLOT_BAG_0, slot);

    // Bag slots
    for (uint8 bag = INVENTORY_SLOT_BAG_START; bag < INVENTORY_SLOT_BAG_END; ++bag)
    {
        Bag* container = bot->GetBagByPos(bag);
        if (!container)
            continue;

        for (uint32 slot = 0; slot < container->GetBagSize(); ++slot)
            pushItem(bot->GetItemByPos(bag, slot), bag, slot);
    }

    return out;
}

Item* FindItemInBags(Player* bot, std::string_view query)
{
    if (!bot || query.empty())
        return nullptr;

    // GUID-low (numeric) match takes priority — exact, no ambiguity.
    if (IsAllDigits(query))
    {
        uint32 guidLow = static_cast<uint32>(std::strtoul(std::string(query).c_str(), nullptr, 10));
        for (auto const& row : ListBags(bot))
        {
            if (row.guidLow == guidLow)
                return bot->GetItemByPos(row.bag, row.slot);
        }
    }

    // Substring match on item name (case-insensitive).
    std::string needle = LowerCopy(query);
    for (auto const& row : ListBags(bot))
    {
        if (LowerCopy(row.name).find(needle) != std::string::npos)
            return bot->GetItemByPos(row.bag, row.slot);
    }

    return nullptr;
}

bool EquipItem(Player* master, Player* bot, std::string_view query)
{
    Item* item = FindItemInBags(bot, query);
    if (!item)
    {
        ChatHandler(master->GetSession()).PSendSysMessage("No item matching '%s' in %s's bags.",
            std::string(query).c_str(), bot->GetName().c_str());
        return false;
    }

    uint16 src  = item->GetPos();
    uint16 dest = 0;
    InventoryResult res = bot->CanEquipItem(NULL_SLOT, dest, item, false);
    if (res != EQUIP_ERR_OK)
    {
        ChatHandler(master->GetSession()).PSendSysMessage("Cannot equip '%s' (err %u).",
            item->GetTemplate()->Name1.c_str(), uint32(res));
        return false;
    }

    bot->RemoveItem(src >> 8, src & 0xFF, true);
    bot->EquipItem(dest, item, true);
    bot->AutoUnequipOffhandIfNeed();

    ChatHandler(master->GetSession()).PSendSysMessage("%s equipped '%s'.",
        bot->GetName().c_str(), item->GetTemplate()->Name1.c_str());
    return true;
}

bool SellItem(Player* master, Player* bot, std::string_view query)
{
    Item* item = FindItemInBags(bot, query);
    if (!item)
    {
        ChatHandler(master->GetSession()).PSendSysMessage("No item matching '%s' in %s's bags.",
            std::string(query).c_str(), bot->GetName().c_str());
        return false;
    }

    // Sell flow needs a vendor in interaction range. The bot's session has
    // the standard vendor-sell handler — we route through the player API.
    Creature* vendor = bot->GetNPCIfCanInteractWith(ObjectGuid::Empty, UNIT_NPC_FLAG_VENDOR);
    if (!vendor)
    {
        ChatHandler(master->GetSession()).PSendSysMessage("%s is not near a vendor.",
            bot->GetName().c_str());
        return false;
    }

    std::string itemName = item->GetTemplate()->Name1;
    bot->GetSession()->SendListInventory(vendor->GetGUID());
    bot->GetSession()->SendSellItem(vendor->GetGUID(), item->GetGUID(), 0);

    ChatHandler(master->GetSession()).PSendSysMessage("%s sold '%s'.",
        bot->GetName().c_str(), itemName.c_str());
    return true;
}

bool DestroyItem(Player* master, Player* bot, std::string_view query)
{
    Item* item = FindItemInBags(bot, query);
    if (!item)
    {
        ChatHandler(master->GetSession()).PSendSysMessage("No item matching '%s' in %s's bags.",
            std::string(query).c_str(), bot->GetName().c_str());
        return false;
    }

    std::string itemName = item->GetTemplate()->Name1;
    uint32 count = item->GetCount();

    bot->DestroyItemCount(item->GetEntry(), count, true);

    ChatHandler(master->GetSession()).PSendSysMessage("%s destroyed '%s'.",
        bot->GetName().c_str(), itemName.c_str());
    return true;
}

bool OpenTrade(Player* master, Player* bot)
{
    if (master->GetTrader() || bot->GetTrader())
    {
        ChatHandler(master->GetSession()).SendSysMessage("Trade window already open.");
        return false;
    }

    if (!master->IsWithinDistInMap(bot, INTERACTION_DISTANCE))
    {
        ChatHandler(master->GetSession()).PSendSysMessage("%s is too far away to trade.",
            bot->GetName().c_str());
        return false;
    }

    // Server-initiated trade: master is the initiator, bot is the trader target.
    master->SetTradeData(new TradeData(master, bot));
    bot->SetTradeData(new TradeData(bot, master));

    WorldPacket data(SMSG_TRADE_STATUS, 4 + 8);
    data << uint32(TRADE_STATUS_OPEN_WINDOW);
    data << uint64(0);
    master->GetSession()->SendPacket(&data);

    WorldPacket data2(SMSG_TRADE_STATUS, 4 + 8);
    data2 << uint32(TRADE_STATUS_OPEN_WINDOW);
    data2 << uint64(0);
    bot->GetSession()->SendPacket(&data2);

    ChatHandler(master->GetSession()).PSendSysMessage("Trade window opened with %s.",
        bot->GetName().c_str());
    return true;
}

} // namespace AltbotInventory

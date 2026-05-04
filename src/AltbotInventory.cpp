#include "AltbotInventory.h"
#include "Bag.h"
#include "Chat.h"
#include "Item.h"
#include "ItemTemplate.h"
#include "Log.h"
#include "ObjectMgr.h"
#include "Player.h"
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
        // TC 4.3.4: ItemTemplate has no Name1 field; use GetName(DEFAULT_LOCALE)
        row.name    = tpl->GetName(DEFAULT_LOCALE);
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

std::vector<EquippedItem> ListEquipped(Player* bot)
{
    std::vector<EquippedItem> out;
    if (!bot)
        return out;

    for (uint8 slot = EQUIPMENT_SLOT_START; slot < EQUIPMENT_SLOT_END; ++slot)
    {
        Item* item = bot->GetItemByPos(INVENTORY_SLOT_BAG_0, slot);
        if (!item)
            continue;
        ItemTemplate const* tpl = item->GetTemplate();
        if (!tpl)
            continue;
        EquippedItem row;
        row.slot      = slot;
        row.entry     = item->GetEntry();
        row.enchantId = item->GetEnchantmentId(PERM_ENCHANTMENT_SLOT);
        row.gem1      = item->GetEnchantmentId(SOCK_ENCHANTMENT_SLOT);
        row.gem2      = item->GetEnchantmentId(SOCK_ENCHANTMENT_SLOT_2);
        row.gem3      = item->GetEnchantmentId(SOCK_ENCHANTMENT_SLOT_3);
        row.ilvl      = tpl->GetBaseItemLevel();
        row.guidLow   = item->GetGUID().GetCounter();
        out.push_back(std::move(row));
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
        // TC 4.3.4: ItemTemplate::GetName(LocaleConstant) returns char const*
        ChatHandler(master->GetSession()).PSendSysMessage("Cannot equip '%s' (err %u).",
            item->GetTemplate()->GetName(DEFAULT_LOCALE), uint32(res));
        return false;
    }

    bot->RemoveItem(src >> 8, src & 0xFF, true);
    bot->EquipItem(dest, item, true);
    bot->AutoUnequipOffhandIfNeed();

    ChatHandler(master->GetSession()).PSendSysMessage("%s equipped '%s'.",
        bot->GetName().c_str(), item->GetTemplate()->GetName(DEFAULT_LOCALE));
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

    // TODO: TC 4.3.4 - no equivalent API. WorldSession has no public SendSellItem()
    // method; sell is handled via HandleSellItemOpcode (packet handler, not callable
    // directly). Auto-sell via NPC is not supported in this port.
    ChatHandler(master->GetSession()).PSendSysMessage(
        "Auto-sell is not supported in this build (TC 4.3.4 has no SendSellItem API).");
    return false;
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

    // TC 4.3.4: ItemTemplate::GetName(LocaleConstant) returns char const*
    std::string itemName = item->GetTemplate()->GetName(DEFAULT_LOCALE);
    uint32 count = item->GetCount();

    bot->DestroyItemCount(item->GetEntry(), count, true);

    ChatHandler(master->GetSession()).PSendSysMessage("%s destroyed '%s'.",
        bot->GetName().c_str(), itemName.c_str());
    return true;
}

bool OpenTrade(Player* master, Player* bot)
{
    // TODO: TC 4.3.4 - no equivalent API. Player has no SetTradeData() method;
    // trade is initiated only via client opcodes (CMSG_INITIATE_TRADE).
    // Server-side trade window opening is not supported in this port.
    ChatHandler(master->GetSession()).PSendSysMessage(
        "Server-initiated trade is not supported in this build (TC 4.3.4 — use in-game trade UI).");
    (void)bot;
    return false;
}

} // namespace AltbotInventory

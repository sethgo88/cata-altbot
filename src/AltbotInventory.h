#pragma once
#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

class Player;
class Item;

struct BagItem
{
    uint8       bag      = 0;
    uint8       slot     = 0;
    uint32      entry    = 0;
    uint32      count    = 0;
    uint32      guidLow  = 0;
    std::string name;
};

namespace AltbotInventory
{
    // Lists every item in the bot's bags (excluding equipped slots).
    std::vector<BagItem> ListBags(Player* bot);

    // Item-name OR item GUID-low matching, returns the first item found.
    Item* FindItemInBags(Player* bot, std::string_view query);

    // Equip the item to the appropriate slot (or first matching valid slot).
    bool EquipItem  (Player* master, Player* bot, std::string_view query);

    // Sell the item to the nearest vendor; bot must be in vendor range.
    bool SellItem   (Player* master, Player* bot, std::string_view query);

    // Permanently destroy the item from the bot's bags.
    bool DestroyItem(Player* master, Player* bot, std::string_view query);

    // Open a trade window between master and bot (master initiates).
    bool OpenTrade  (Player* master, Player* bot);
}

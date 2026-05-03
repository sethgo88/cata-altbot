-- locales/enUS.lua — string table, English (default).
-- Other locales would override entries from this default.

local _, addon = ...
addon.L = setmetatable({
    ["Add..."]                = "Add...",
    ["Refresh"]               = "Refresh",
    ["Login"]                 = "Login",
    ["Logout"]                = "Logout",
    ["Remove"]                = "Remove",
    ["Bags"]                  = "Bags",
    ["Talents"]               = "Talents",

    -- Broadcast bar
    ["Mode:"]                 = "Mode:",
    ["Combat:"]               = "Combat:",
    ["Assist:"]               = "Assist:",
    ["Group:"]                = "Group:",
    ["Toggles:"]              = "Toggles:",
    ["Follow All"]            = "Follow All",
    ["Stay All"]              = "Stay All",
    ["Attack Target"]         = "Attack Target",
    ["Invite All"]            = "Invite All",
    ["Summon All"]            = "Summon All",
    ["Uninvite All"]          = "Uninvite All",

    -- Assist modes
    ["Off"]                   = "Off",
    ["Target"]                = "Target",
    ["Skull"]                 = "Skull",
    ["Both"]                  = "Both",

    -- Toggles
    ["Mount"]                 = "Mount",
    ["Loot"]                  = "Loot",
    ["Pass"]                  = "Pass",
    ["Release"]               = "Release",
    ["QTake"]                 = "QTake",
    ["QTurn"]                 = "QTurn",

    -- Spec / general
    ["Auto-detect"]           = "Auto-detect",
    ["Linked Accounts"]       = "Linked Accounts",
    ["No alts found"]         = "No alts found",
}, { __index = function(_, k) return k end })

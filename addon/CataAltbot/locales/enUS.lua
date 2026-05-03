-- locales/enUS.lua — string table, English (default).
-- Other locales would override entries from this default.

local addon = CataAltbot
addon.L = setmetatable({
    ["Add..."]                = "Add...",
    ["Refresh"]               = "Refresh",
    ["Login"]                 = "Login",
    ["Logout"]                = "Logout",
    ["Invite"]                = "Invite",
    ["Uninvite"]              = "Uninvite",
    ["Summon"]                = "Summon",
    ["Follow"]                = "Follow",
    ["Stay"]                  = "Stay",
    ["Attack"]                = "Attack",
    ["Remove"]                = "Remove",
    ["Linked Accounts"]       = "Linked Accounts",
    ["No alts found"]         = "No alts found",
}, { __index = function(_, k) return k end })

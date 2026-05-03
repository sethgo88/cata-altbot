-- CataAltbot.lua — addon event hub + protocol layer.
--
-- Transport: pipe-delimited messages over WoW's whisper channel, with a
-- "CATABOT|" prefix. Avoids the addon-message channel entirely, which keeps
-- the server side simple (it just hooks regular CHAT_MSG_WHISPER) and works
-- without prefix-registration on the 4.3.4 client.
--
-- Master self-whispers (master → master) are also intercepted server-side as
-- the bootstrap path: this lets the addon LIST/ADD/LOGIN with zero bots online.
-- Once a bot is up, AnyActiveBotName picks it as the transport, but the
-- behavior is otherwise indistinguishable from the master-self path.

local addonName, addon = ...
CataAltbot = addon
CataAltbot.PREFIX = "CATABOT|"
CataAltbot.PROTO_VERSION = 1

-- Per-master state mirror, populated from server STATE pushes and LIST replies.
addon.alts  = {}    -- keyed by guidLow: {name, classId, level, accountId, registered, active, state}
addon.links = {}    -- linked accounts list

-- Saved variables: action-bar position.
local function InitDB()
    CataAltbotDB = CataAltbotDB or {}
end

-- ---- Send / receive primitives ----

function addon:Send(target, verbAndArgs)
    if not target or target == "" then return end
    SendChatMessage(self.PREFIX .. verbAndArgs, "WHISPER", nil, target)
end

-- Pick a whisper transport. Prefer an active bot; fall back to master self-
-- whisper, which the server hooks the same way. Always returns a valid name,
-- so callers don't need to gate.
function addon:AnyActiveBotName()
    for _, alt in pairs(self.alts) do
        if alt.active then return alt.name end
    end
    return UnitName("player")
end

function addon:Parse(msg)
    if not msg or string.sub(msg, 1, #self.PREFIX) ~= self.PREFIX then
        return nil
    end
    local payload = string.sub(msg, #self.PREFIX + 1)
    local parts = {}
    for token in string.gmatch(payload, "([^|]+)") do
        table.insert(parts, token)
    end
    return parts[1], parts
end

-- ---- Server → client verb handlers ----

local handlers = {}

handlers.HELLO_OK = function(parts)
    addon.protoVersion = tonumber(parts[2])
end

handlers.ALT_ROW = function(parts)
    local guidLow = tonumber(parts[2])
    if not guidLow then return end
    addon.alts[guidLow] = {
        guidLow    = guidLow,
        name       = parts[3],
        classId    = tonumber(parts[4]),
        level      = tonumber(parts[5]),
        accountId  = tonumber(parts[6]),
        registered = parts[7] == "1",
        active     = parts[8] == "1",
    }
end

handlers.LIST_DONE = function()
    if addon.RosterPopout and addon.RosterPopout:IsShown() then
        addon.RosterPopout:Refresh()
    end
end

handlers.LINK_ROW = function(parts)
    table.insert(addon.links, { username = parts[2], accountId = tonumber(parts[3]) })
end

handlers.LINKS_DONE = function() end

handlers.STATE = function(parts)
    local guidLow = tonumber(parts[2])
    if not guidLow or not addon.alts[guidLow] then return end
    local state = {}
    for kv in string.gmatch(parts[3] or "", "([^;]+)") do
        local k, v = string.match(kv, "([^=]+)=(.+)")
        if k and v then state[k] = v end
    end
    addon.alts[guidLow].state = state
end

handlers.BAG_ROW = function(parts)
    if addon.BagsModal then addon.BagsModal:OnBagRow(parts) end
end

handlers.BAGS_DONE = function(parts)
    if addon.BagsModal then addon.BagsModal:OnBagsDone(parts) end
end

handlers.ERR = function(parts)
    DEFAULT_CHAT_FRAME:AddMessage("|cffff5555[CataAltbot] " .. (parts[2] or "?") ..
        ": " .. (parts[3] or "") .. "|r")
end

-- ---- Public API used by UI files ----

function addon:RefreshAlts()
    self.alts = {}
    self:Send(self:AnyActiveBotName(), "LIST")
end

function addon:RefreshLinks()
    self.links = {}
    self:Send(self:AnyActiveBotName(), "LINKS")
end

-- Per-bot lifecycle / actions. The server resolves the bot by name argument,
-- so the whisper target only matters as transport — we use the same self-
-- whisper fallback everywhere.
function addon:Login(botName)    self:Send(self:AnyActiveBotName(), "LOGIN|"   .. botName) end
function addon:Logout(botName)   self:Send(self:AnyActiveBotName(), "LOGOUT|"  .. botName) end
function addon:Add(botName)      self:Send(self:AnyActiveBotName(), "ADD|"     .. botName) end
function addon:Remove(botName)   self:Send(self:AnyActiveBotName(), "REMOVE|"  .. botName) end
function addon:Invite(botName)   self:Send(self:AnyActiveBotName(), "INVITE|"  .. botName) end
function addon:Uninvite(botName) self:Send(self:AnyActiveBotName(), "UNINVITE|".. botName) end
function addon:Summon(botName)   self:Send(self:AnyActiveBotName(), "SUMMON|"  .. botName) end

function addon:RequestBags(botName)
    self:Send(self:AnyActiveBotName(), "BAGS|" .. botName)
end

function addon:LearnTalent(botName, talentId, rank)
    self:Send(self:AnyActiveBotName(), "LEARN_TALENT|" .. botName .. "|" .. talentId .. "|" .. rank)
end

function addon:SetSpec(botName, slug)
    self:Send(self:AnyActiveBotName(), "SET_SPEC|" .. botName .. "|" .. slug)
end

function addon:SetRole(botName, slug)
    self:Send(self:AnyActiveBotName(), "SET_ROLE|" .. botName .. "|" .. slug)
end

-- Broadcast verbs — server fans out across all active bots.
function addon:SetModeAll(mode)         self:Send(self:AnyActiveBotName(), "SET_MODE_ALL|"   .. mode) end
function addon:SetAssistAll(mode)       self:Send(self:AnyActiveBotName(), "SET_ASSIST_ALL|" .. mode) end
function addon:SetToggleAll(key, on)    self:Send(self:AnyActiveBotName(), "SET_TOGGLE_ALL|" .. key .. "|" .. (on and "on" or "off")) end
function addon:AttackAll()              self:Send(self:AnyActiveBotName(), "ATTACK_ALL")             end
function addon:InviteAll()              self:Send(self:AnyActiveBotName(), "INVITE_ALL")             end
function addon:UninviteAll()            self:Send(self:AnyActiveBotName(), "UNINVITE_ALL")           end
function addon:SummonAll()              self:Send(self:AnyActiveBotName(), "SUMMON_ALL")             end

-- ---- Event wiring ----

local f = CreateFrame("Frame")
f:RegisterEvent("ADDON_LOADED")
f:RegisterEvent("PLAYER_LOGIN")
f:RegisterEvent("CHAT_MSG_WHISPER")

f:SetScript("OnEvent", function(self, event, arg1, ...)
    if event == "ADDON_LOADED" and arg1 == addonName then
        InitDB()
    elseif event == "PLAYER_LOGIN" then
        if addon.ActionBar then addon.ActionBar:Show() end
    elseif event == "CHAT_MSG_WHISPER" then
        local msg = arg1
        local verb, parts = addon:Parse(msg)
        if verb and handlers[verb] then
            handlers[verb](parts)
        end
    end
end)

-- Slash command — toggles the action bar. No subcommands; the bar IS the UI.
SLASH_CATAALTBOT1 = "/cab"
SLASH_CATAALTBOT2 = "/cataaltbot"
SlashCmdList["CATAALTBOT"] = function()
    if addon.ActionBar then
        addon.ActionBar:Toggle()
        return
    end

    -- Diagnostic when the action bar didn't register — almost always means
    -- one or more UI/*.lua files didn't reach the WoW client.
    local expected = { "ActionBar", "RosterPopout", "BotButton", "BotSubMenu", "RoleMenu", "SpecMenu", "BagsModal", "TalentsModal" }
    local loaded, missing = {}, {}
    for _, k in ipairs(expected) do
        if addon[k] then table.insert(loaded, k) else table.insert(missing, k) end
    end
    DEFAULT_CHAT_FRAME:AddMessage("|cffff5555[CataAltbot]|r action bar not loaded.")
    DEFAULT_CHAT_FRAME:AddMessage("  loaded:  " .. (next(loaded)  and table.concat(loaded,  ", ") or "(none)"))
    DEFAULT_CHAT_FRAME:AddMessage("  missing: " .. (next(missing) and table.concat(missing, ", ") or "(none)"))
end

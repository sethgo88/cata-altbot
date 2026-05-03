-- CataAltbot.lua — addon event hub + protocol layer.
--
-- Transport: pipe-delimited messages over WoW's whisper channel, with a
-- "CATABOT|" prefix. Avoids the addon-message channel entirely, which keeps
-- the server side simple (it just hooks regular CHAT_MSG_WHISPER) and works
-- without prefix-registration on the 4.3.4 client.
--
-- Inbound  (master -> server): SendChatMessage("CATABOT|VERB|...", "WHISPER", nil, "BotName")
-- Outbound (server -> master): bot Whispers back "CATABOT|VERB|..." which we
--                              parse out of CHAT_MSG_WHISPER events.

local addonName, addon = ...
CataAltbot = addon
CataAltbot.PREFIX = "CATABOT|"
CataAltbot.PROTO_VERSION = 1

-- Per-master state mirror, populated from server STATE pushes and LIST replies.
addon.alts  = {}    -- keyed by guidLow: {name, classId, level, accountId, registered, active, state}
addon.links = {}    -- linked accounts list

-- Saved variables: panel position, default view filter.
local function InitDB()
    CataAltbotDB = CataAltbotDB or {}
    CataAltbotDB.position = CataAltbotDB.position or { point = "CENTER", x = 0, y = 0 }
    CataAltbotDB.showOffline = CataAltbotDB.showOffline ~= false
end

-- ---- Send / receive primitives ----

-- Send a verb to the server. `botName` is the bot the whisper goes to; for
-- some verbs (LIST, LINKS, ADD) the bot just acts as a transport, but we still
-- need an active bot to whisper. If no bots are active, falls back to whispering
-- the master themselves (server's OnChat treats master->master with prefix as
-- a no-op route — addon should pick a different bot).
function addon:Send(botName, verbAndArgs)
    if not botName or botName == "" then
        return
    end
    SendChatMessage(self.PREFIX .. verbAndArgs, "WHISPER", nil, botName)
end

-- Pick any active bot's name to use as the whisper target for stateless verbs.
function addon:AnyActiveBotName()
    for _, alt in pairs(self.alts) do
        if alt.active then return alt.name end
    end
    -- Fallback: any registered bot (even if offline; the whisper will fail but
    -- the user will see the error and know to log a bot in first).
    for _, alt in pairs(self.alts) do
        if alt.registered then return alt.name end
    end
    return nil
end

-- Parse a whisper. Returns (verb, args[]) if it's an addon message, else nil.
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

-- ---- Verb handlers (server -> client) ----

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
    if addon.MainFrame then addon.MainFrame:Refresh() end
end

handlers.LINK_ROW = function(parts)
    table.insert(addon.links, { username = parts[2], accountId = tonumber(parts[3]) })
end

handlers.LINKS_DONE = function()
    if addon.LinksTab then addon.LinksTab:Refresh() end
end

handlers.STATE = function(parts)
    local guidLow = tonumber(parts[2])
    if not guidLow or not addon.alts[guidLow] then return end
    local state = {}
    for kv in string.gmatch(parts[3] or "", "([^;]+)") do
        local k, v = string.match(kv, "([^=]+)=(.+)")
        if k and v then state[k] = v end
    end
    addon.alts[guidLow].state = state
    if addon.MainFrame then addon.MainFrame:Refresh() end
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

-- ---- Public API used by other UI files ----

function addon:RefreshAlts()
    self.alts = {}
    local target = self:AnyActiveBotName()
    if target then self:Send(target, "LIST") end
end

function addon:RefreshLinks()
    self.links = {}
    local target = self:AnyActiveBotName()
    if target then self:Send(target, "LINKS") end
end

function addon:Login(botName)  self:Send(botName, "LOGIN|" .. botName) end
function addon:Logout(botName) self:Send(botName, "LOGOUT|" .. botName) end
function addon:Add(botName)    local t = self:AnyActiveBotName(); if t then self:Send(t, "ADD|" .. botName) end end
function addon:Remove(botName) local t = self:AnyActiveBotName(); if t then self:Send(t, "REMOVE|" .. botName) end end

function addon:Invite(botName)   self:Send(botName, "INVITE|"   .. botName) end
function addon:Uninvite(botName) self:Send(botName, "UNINVITE|" .. botName) end
function addon:Summon(botName)   self:Send(botName, "SUMMON|"   .. botName) end

function addon:RequestBags(botName)
    self:Send(botName, "BAGS|" .. botName)
end

function addon:LearnTalent(botName, talentId, rank)
    self:Send(botName, "LEARN_TALENT|" .. botName .. "|" .. talentId .. "|" .. rank)
end

function addon:SetSpec(botName, slug)
    self:Send(botName, "SET_SPEC|" .. botName .. "|" .. slug)
end

-- ---- Broadcast verbs (drive every active bot in one click) ----
-- Routed through any one active bot's whisper channel; the server fans out
-- across the master's whole roster, so the choice of transport bot is
-- irrelevant.

function addon:SetModeAll(mode)
    local target = self:AnyActiveBotName(); if not target then return end
    self:Send(target, "SET_MODE_ALL|" .. mode)
end

function addon:SetAssistAll(mode)
    local target = self:AnyActiveBotName(); if not target then return end
    self:Send(target, "SET_ASSIST_ALL|" .. mode)
end

function addon:SetToggleAll(key, on)
    local target = self:AnyActiveBotName(); if not target then return end
    self:Send(target, "SET_TOGGLE_ALL|" .. key .. "|" .. (on and "on" or "off"))
end

function addon:AttackAll()
    local target = self:AnyActiveBotName(); if not target then return end
    self:Send(target, "ATTACK_ALL")
end

function addon:InviteAll()
    local target = self:AnyActiveBotName(); if not target then return end
    self:Send(target, "INVITE_ALL")
end

function addon:UninviteAll()
    local target = self:AnyActiveBotName(); if not target then return end
    self:Send(target, "UNINVITE_ALL")
end

function addon:SummonAll()
    local target = self:AnyActiveBotName(); if not target then return end
    self:Send(target, "SUMMON_ALL")
end

-- True if at least one bot is active. UI uses this to gate broadcast buttons.
function addon:HasActiveBot()
    return self:AnyActiveBotName() ~= nil
end

-- ---- Event wiring ----

local f = CreateFrame("Frame")
f:RegisterEvent("ADDON_LOADED")
f:RegisterEvent("PLAYER_LOGIN")
f:RegisterEvent("CHAT_MSG_WHISPER")

f:SetScript("OnEvent", function(self, event, arg1, ...)
    if event == "ADDON_LOADED" and arg1 == addonName then
        InitDB()
    elseif event == "PLAYER_LOGIN" then
        if addon.MainFrame then addon.MainFrame:Show() end
        addon:RefreshAlts()
    elseif event == "CHAT_MSG_WHISPER" then
        local msg, sender = arg1, ...
        local verb, parts = addon:Parse(msg)
        if verb and handlers[verb] then
            handlers[verb](parts, sender)
        end
    end
end)

-- Slash command for quick test / open panel.
SLASH_CATAALTBOT1 = "/cab"
SLASH_CATAALTBOT2 = "/cataaltbot"
SlashCmdList["CATAALTBOT"] = function(msg)
    if msg == "refresh" then
        addon:RefreshAlts()
    elseif addon.MainFrame then
        if addon.MainFrame:IsShown() then addon.MainFrame:Hide() else addon.MainFrame:Show() end
    end
end

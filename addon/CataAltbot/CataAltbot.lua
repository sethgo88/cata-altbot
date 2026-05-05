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
    -- WoW's chat parser treats `|x` as an escape (colors, hyperlinks, textures)
    -- and outright rejects unknown ones with "invalid escape code". Doubling
    -- every pipe makes the parser emit a literal `|`, so the server still sees
    -- single-pipe delimiters. Don't move this below the prefix concat — the
    -- prefix's own pipe needs the same treatment.
    local escaped = (self.PREFIX .. verbAndArgs):gsub("|", "||")
    SendChatMessage(escaped, "WHISPER", nil, target)
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
    if not msg then return nil end
    -- Server escapes literal `|` to `||` for the chat-format parser; reverse
    -- that here so the prefix check and field split see clean delimiters.
    msg = msg:gsub("||", "|")
    if string.sub(msg, 1, #self.PREFIX) ~= self.PREFIX then
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

handlers.GEAR_ROW = function(parts)
    if addon.CharacterModal then addon.CharacterModal:OnGearRow(parts) end
end

handlers.GEAR_DONE = function(parts)
    if addon.CharacterModal then addon.CharacterModal:OnGearDone(parts) end
end

handlers.STATS = function(parts)
    if addon.CharacterModal then addon.CharacterModal:OnStats(parts) end
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

-- Per-bot lifecycle / actions. Lifecycle verbs are pinned to master self-
-- whisper so the server never tears down the bot whose chat handler is on the
-- call stack — that path is a guaranteed use-after-free in Player::Whisper
-- (the post-script BuildChatPacket dereferences the now-freed Player).
-- INVITE/UNINVITE/SUMMON don't destroy anything, so any transport works.
local function selfTarget() return UnitName("player") end

function addon:Login(botName)    self:Send(selfTarget(),               "LOGIN|"   .. botName) end
function addon:Logout(botName)   self:Send(selfTarget(),               "LOGOUT|"  .. botName) end
function addon:Add(botName)      self:Send(selfTarget(),               "ADD|"     .. botName) end
function addon:Remove(botName)   self:Send(selfTarget(),               "REMOVE|"  .. botName) end
function addon:Invite(botName)   self:Send(self:AnyActiveBotName(),    "INVITE|"  .. botName) end
function addon:Uninvite(botName) self:Send(self:AnyActiveBotName(),    "UNINVITE|".. botName) end
function addon:Summon(botName)   self:Send(self:AnyActiveBotName(),    "SUMMON|"  .. botName) end

function addon:RequestBags(botName)
    self:Send(self:AnyActiveBotName(), "BAGS|" .. botName)
end

function addon:RequestGear(botName)
    self:Send(self:AnyActiveBotName(), "GEAR|" .. botName)
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

function addon:SetRoll(botName, slug)
    self:Send(self:AnyActiveBotName(), "SET_ROLL|" .. botName .. "|" .. slug)
end

-- Master-targeted manual loot. `targetGuid` is the raw "0x..." string from
-- UnitGUID("target"); the server decodes it to ObjectGuid and validates
-- lootability, replying ERR|NOT_LOOTABLE|... on failure.
function addon:Loot(botName, targetGuid)
    self:Send(self:AnyActiveBotName(), "LOOT|" .. botName .. "|" .. targetGuid)
end

-- Force the bot to accept/turn-in every quest the master's selected NPC
-- offers/involves. `targetGuid` is the raw UnitGUID("target") string. Server
-- replies with a PSendSysMessage summary in the master's chat window.
function addon:QuestNPC(botName, targetGuid)
    self:Send(self:AnyActiveBotName(), "QUEST_NPC|" .. botName .. "|" .. targetGuid)
end

-- Broadcast verbs — server fans out across all active bots.
function addon:SetModeAll(mode)         self:Send(self:AnyActiveBotName(), "SET_MODE_ALL|"   .. mode) end
function addon:SetAssistAll(mode)       self:Send(self:AnyActiveBotName(), "SET_ASSIST_ALL|" .. mode) end
function addon:SetToggleAll(key, on)    self:Send(self:AnyActiveBotName(), "SET_TOGGLE_ALL|" .. key .. "|" .. (on and "on" or "off")) end
function addon:AttackAll()              self:Send(self:AnyActiveBotName(), "ATTACK_ALL")             end
function addon:InviteAll()              self:Send(self:AnyActiveBotName(), "INVITE_ALL")             end
function addon:UninviteAll()            self:Send(self:AnyActiveBotName(), "UNINVITE_ALL")           end
function addon:SummonAll()              self:Send(self:AnyActiveBotName(), "SUMMON_ALL")             end
function addon:QuestNPCAll(targetGuid)  self:Send(self:AnyActiveBotName(), "QUEST_NPC_ALL|" .. targetGuid) end

-- ---- Chat filter — swallow our own protocol traffic ----
--
-- Affects display only; the addon's CHAT_MSG_WHISPER event still fires for
-- both inbound and outbound (WHISPER_INFORM) so the protocol isn't broken.
-- Without this, the master's chat window fills up with `CATABOT|...` lines
-- on every refresh / state push.

local function ChatFilter(_, _, msg)
    if not msg then return false end
    -- The display layer may have collapsed `||` to `|` already; check both.
    if msg:sub(1, #addon.PREFIX) == addon.PREFIX then return true end
    if msg:gsub("||", "|"):sub(1, #addon.PREFIX) == addon.PREFIX then return true end
    return false
end

ChatFrame_AddMessageEventFilter("CHAT_MSG_WHISPER",        ChatFilter)
ChatFrame_AddMessageEventFilter("CHAT_MSG_WHISPER_INFORM", ChatFilter)

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
    local expected = { "ActionBar", "RosterPopout", "BotButton", "BotSubMenu", "RoleMenu", "LootRollMenu", "SpecMenu", "BagsModal", "TalentsModal", "CharacterModal" }
    local loaded, missing = {}, {}
    for _, k in ipairs(expected) do
        if addon[k] then table.insert(loaded, k) else table.insert(missing, k) end
    end
    DEFAULT_CHAT_FRAME:AddMessage("|cffff5555[CataAltbot]|r action bar not loaded.")
    DEFAULT_CHAT_FRAME:AddMessage("  loaded:  " .. (next(loaded)  and table.concat(loaded,  ", ") or "(none)"))
    DEFAULT_CHAT_FRAME:AddMessage("  missing: " .. (next(missing) and table.concat(missing, ", ") or "(none)"))
end

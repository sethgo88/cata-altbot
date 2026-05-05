-- ActionBar.lua — main horizontal control bar.
--
-- Layout (left → right): [Attack Target] [Follow/Stay toggle] [Summon All] [Quest All] [Roster].
-- Draggable as a single unit (the parent frame, not individual buttons).
-- Centered by default; position persists in CataAltbotDB.position.
--
-- Each button is a 36×36 action-bar-style square with a built-in WoW icon and
-- an `ActionButtonTemplate`-equivalent appearance. The roster button is the
-- only one that opens a popout; the others fire-and-forget broadcast verbs.

local _, addon = ...
local AB = {}
addon.ActionBar = AB

local BUTTON_SIZE = 36
local SPACING     = 4

-- Toggle state for follow/stay; flips on click. Reflects "what would the next
-- click do" — the frame doesn't observe per-bot state since this is broadcast.
AB.followStay = "follow"

-- Frames -------------------------------------------------------------------

local frame = CreateFrame("Frame", "CataAltbotActionBar", UIParent)
frame:SetSize(BUTTON_SIZE * 5 + SPACING * 4, BUTTON_SIZE)
frame:SetMovable(true)
frame:EnableMouse(true)
frame:RegisterForDrag("LeftButton")
frame:SetScript("OnDragStart", frame.StartMoving)
frame:SetScript("OnDragStop", function(self)
    self:StopMovingOrSizing()
    if not CataAltbotDB then return end
    local point, _, relPoint, x, y = self:GetPoint()
    CataAltbotDB.position = { point = point, relPoint = relPoint, x = x, y = y }
end)
frame:Hide()

local function ApplySavedPosition()
    local p = CataAltbotDB and CataAltbotDB.position
    frame:ClearAllPoints()
    if p and p.point then
        frame:SetPoint(p.point, UIParent, p.relPoint or p.point, p.x or 0, p.y or 0)
    else
        frame:SetPoint("CENTER", UIParent, "CENTER", 0, 0)
    end
end

-- Button factory -----------------------------------------------------------

-- Mimics the look of a built-in action-bar button (square frame + icon) without
-- pulling in ActionButtonTemplate, which expects action-slot wiring we don't
-- have. The visual is good enough that it docks naturally next to Bartender.
--
-- Shift+drag on any button moves the parent action bar — same idiom as
-- Bartender/Dominos. Plain click fires the bound action.
local function MakeIconButton(parent, name, iconPath)
    local b = CreateFrame("Button", name, parent)
    b:SetSize(BUTTON_SIZE, BUTTON_SIZE)
    b:RegisterForClicks("AnyUp")
    b:RegisterForDrag("LeftButton")

    b.icon = b:CreateTexture(nil, "BACKGROUND")
    b.icon:SetAllPoints()
    b.icon:SetTexture(iconPath)
    b.icon:SetTexCoord(0.08, 0.92, 0.08, 0.92)   -- crop the icon's outer ring

    -- Border/highlight using the standard square frame textures.
    b.border = b:CreateTexture(nil, "ARTWORK")
    b.border:SetTexture("Interface\\Buttons\\UI-Quickslot2")
    b.border:SetSize(BUTTON_SIZE * 1.7, BUTTON_SIZE * 1.7)
    b.border:SetPoint("CENTER", b, "CENTER", 0, -1)

    b:SetHighlightTexture("Interface\\Buttons\\ButtonHilight-Square")
    b:SetPushedTexture  ("Interface\\Buttons\\UI-Quickslot-Depress")

    b:SetScript("OnDragStart", function(self)
        if IsShiftKeyDown() then self:GetParent():StartMoving() end
    end)
    b:SetScript("OnDragStop", function(self)
        local p = self:GetParent()
        p:StopMovingOrSizing()
        if not CataAltbotDB then return end
        local point, _, relPoint, x, y = p:GetPoint()
        CataAltbotDB.position = { point = point, relPoint = relPoint, x = x, y = y }
    end)

    return b
end

-- Tooltip helper — minimal: name + one-liner.
local function SetTip(button, title, line2)
    button:SetScript("OnEnter", function(self)
        GameTooltip:SetOwner(self, "ANCHOR_TOP")
        GameTooltip:AddLine(title)
        if line2 then GameTooltip:AddLine(line2, 0.7, 0.7, 0.7, true) end
        GameTooltip:Show()
    end)
    button:SetScript("OnLeave", function() GameTooltip:Hide() end)
end

-- Buttons ------------------------------------------------------------------

local ICON_ATTACK       = "Interface\\Icons\\Ability_DualWield"
local ICON_FOLLOW       = "Interface\\Icons\\Ability_Tracking"
local ICON_STAY         = "Interface\\Icons\\Spell_Frost_Stun"
local ICON_SUMMON       = "Interface\\Icons\\Spell_Arcane_TeleportShattrath"
local ICON_QUEST        = "Interface\\Icons\\INV_Misc_Note_01"
local ICON_ROSTER       = "Interface\\Icons\\Inv_Misc_GroupLooking"

AB.btnAttack = MakeIconButton(frame, "CataAltbotBtnAttack", ICON_ATTACK)
AB.btnAttack:SetPoint("LEFT", frame, "LEFT", 0, 0)
AB.btnAttack:SetScript("OnClick", function() addon:AttackAll() end)
SetTip(AB.btnAttack, "Attack Target", "All active bots attack your current target.")

AB.btnFollowStay = MakeIconButton(frame, "CataAltbotBtnFollowStay", ICON_FOLLOW)
AB.btnFollowStay:SetPoint("LEFT", AB.btnAttack, "RIGHT", SPACING, 0)
AB.btnFollowStay:SetScript("OnClick", function()
    if AB.followStay == "follow" then
        AB.followStay = "stay"
        AB.btnFollowStay.icon:SetTexture(ICON_STAY)
        addon:SetModeAll("stay")
    else
        AB.followStay = "follow"
        AB.btnFollowStay.icon:SetTexture(ICON_FOLLOW)
        addon:SetModeAll("follow")
    end
end)
SetTip(AB.btnFollowStay, "Follow / Stay", "Toggle: all bots follow you, or hold position.")

AB.btnSummon = MakeIconButton(frame, "CataAltbotBtnSummon", ICON_SUMMON)
AB.btnSummon:SetPoint("LEFT", AB.btnFollowStay, "RIGHT", SPACING, 0)
AB.btnSummon:SetScript("OnClick", function() addon:SummonAll() end)
SetTip(AB.btnSummon, "Summon All", "Pull every active bot to your location.")

-- Quest with selected NPC, fanned out to every active bot. Each bot accepts
-- everything the NPC offers + turns in everything it has completed; per-bot
-- range / map / eligibility checks happen server-side and reply as system
-- messages in your chat.
AB.btnQuestAll = MakeIconButton(frame, "CataAltbotBtnQuestAll", ICON_QUEST)
AB.btnQuestAll:SetPoint("LEFT", AB.btnSummon, "RIGHT", SPACING, 0)
AB.btnQuestAll:SetScript("OnClick", function()
    local targetGuid = UnitGUID("target")
    if not targetGuid or targetGuid == "" then
        DEFAULT_CHAT_FRAME:AddMessage("|cffff5555[CataAltbot] QUEST: select an NPC first|r")
        return
    end
    addon:QuestNPCAll(targetGuid)
end)
SetTip(AB.btnQuestAll, "Quest with Selected NPC (All)",
    "Every active bot accepts/turns-in with your current target.")

AB.btnRoster = MakeIconButton(frame, "CataAltbotBtnRoster", ICON_ROSTER)
AB.btnRoster:SetPoint("LEFT", AB.btnQuestAll, "RIGHT", SPACING, 0)
AB.btnRoster:SetScript("OnClick", function()
    if addon.RosterPopout then addon.RosterPopout:Toggle() end
end)
SetTip(AB.btnRoster, "Roster", "Show your altbots and group controls.")

-- Public API ---------------------------------------------------------------

function AB:Show()
    ApplySavedPosition()
    frame:Show()
    addon:RefreshAlts()
end
function AB:Hide()    frame:Hide(); if addon.RosterPopout then addon.RosterPopout:Close() end end
function AB:IsShown() return frame:IsShown() end
function AB:Toggle()  if self:IsShown() then self:Hide() else self:Show() end end

-- Anchor accessor — RosterPopout uses the roster button as its parent so it
-- moves with the action bar and stays anchored when the user drags.
function AB:RosterAnchor() return self.btnRoster end

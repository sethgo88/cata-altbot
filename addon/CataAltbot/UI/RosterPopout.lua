-- RosterPopout.lua — vertical strip that grows up from the Roster button.
--
-- Stack order (bottom → top): Refresh, Invite All, Uninvite All, then one
-- BotButton per registered alt (sorted by name). Closing the popout also
-- closes the per-bot sub-menu so we never leave orphaned popouts onscreen.
--
-- Anchored to the action bar's roster button so it tracks the bar when dragged.

local _, addon = ...
local RP = {}
addon.RosterPopout = RP

local BUTTON_SIZE = 36
local SPACING     = 4

-- Frame --------------------------------------------------------------------

local frame = CreateFrame("Frame", "CataAltbotRosterPopout", UIParent)
frame:SetSize(BUTTON_SIZE, BUTTON_SIZE)   -- height resized on each Refresh
frame:SetFrameStrata("MEDIUM")
frame:Hide()

-- Re-anchor to the action bar's roster button on every show — the bar may
-- have been dragged in between.
local function Reanchor()
    if not addon.ActionBar then return end
    local anchor = addon.ActionBar:RosterAnchor()
    frame:ClearAllPoints()
    frame:SetPoint("BOTTOM", anchor, "TOP", 0, SPACING)
end

-- Button factory (matches ActionBar style for visual consistency) ---------

local function MakeIconButton(parent, name, iconPath)
    local b = CreateFrame("Button", name, parent)
    b:SetSize(BUTTON_SIZE, BUTTON_SIZE)
    b:RegisterForClicks("AnyUp")

    b.icon = b:CreateTexture(nil, "BACKGROUND")
    b.icon:SetAllPoints()
    b.icon:SetTexture(iconPath)
    b.icon:SetTexCoord(0.08, 0.92, 0.08, 0.92)

    b.border = b:CreateTexture(nil, "ARTWORK")
    b.border:SetTexture("Interface\\Buttons\\UI-Quickslot2")
    b.border:SetSize(BUTTON_SIZE * 1.7, BUTTON_SIZE * 1.7)
    b.border:SetPoint("CENTER", b, "CENTER", 0, -1)

    b:SetHighlightTexture("Interface\\Buttons\\ButtonHilight-Square")
    return b
end

local function SetTip(button, title, line2)
    button:SetScript("OnEnter", function(self)
        GameTooltip:SetOwner(self, "ANCHOR_RIGHT")
        GameTooltip:AddLine(title)
        if line2 then GameTooltip:AddLine(line2, 0.7, 0.7, 0.7, true) end
        GameTooltip:Show()
    end)
    button:SetScript("OnLeave", function() GameTooltip:Hide() end)
end

-- Three fixed control buttons at the bottom of the popout.

local ICON_REFRESH  = "Interface\\Icons\\Spell_ChargePositive"
local ICON_INVITE   = "Interface\\Icons\\Achievement_GuildPerk_EverybodysFriend"
local ICON_UNINVITE = "Interface\\Icons\\Spell_ChargeNegative"

RP.btnRefresh = MakeIconButton(frame, "CataAltbotBtnRefresh", ICON_REFRESH)
RP.btnRefresh:SetPoint("BOTTOM", frame, "BOTTOM", 0, 0)
RP.btnRefresh:SetScript("OnClick", function() addon:RefreshAlts() end)
SetTip(RP.btnRefresh, "Refresh Roster", "Re-fetch your alts and their state.")

RP.btnInviteAll = MakeIconButton(frame, "CataAltbotBtnInviteAll", ICON_INVITE)
RP.btnInviteAll:SetPoint("BOTTOM", RP.btnRefresh, "TOP", 0, SPACING)
RP.btnInviteAll:SetScript("OnClick", function() addon:InviteAll() end)
SetTip(RP.btnInviteAll, "Invite All", "Group-invite every active bot.")

RP.btnUninviteAll = MakeIconButton(frame, "CataAltbotBtnUninviteAll", ICON_UNINVITE)
RP.btnUninviteAll:SetPoint("BOTTOM", RP.btnInviteAll, "TOP", 0, SPACING)
RP.btnUninviteAll:SetScript("OnClick", function() addon:UninviteAll() end)
SetTip(RP.btnUninviteAll, "Uninvite All", "Remove every bot from your party.")

-- Bot icon buttons recycled across refreshes.
RP.botButtons = {}

-- Public API ---------------------------------------------------------------

function RP:Open()
    Reanchor()
    self:Refresh()
    frame:Show()
end

function RP:Close()
    frame:Hide()
    if addon.BotSubMenu then addon.BotSubMenu:Hide() end
end

function RP:Toggle()
    if frame:IsShown() then self:Close() else self:Open() end
end

function RP:IsShown() return frame:IsShown() end

-- Rebuild the bot button stack from addon.alts. Stacks UPWARD from the
-- Uninvite-All button so the visual order is, bottom→top:
-- [Refresh] [Invite All] [Uninvite All] [Bot1] [Bot2] ...
function RP:Refresh()
    -- Sort alts by name for stable display.
    local sorted = {}
    for _, alt in pairs(addon.alts) do table.insert(sorted, alt) end
    table.sort(sorted, function(a, b) return (a.name or "") < (b.name or "") end)

    -- Hide any cached buttons we won't reuse this pass.
    for i = #sorted + 1, #self.botButtons do
        self.botButtons[i]:Hide()
    end

    local prevAnchor = self.btnUninviteAll
    for i, alt in ipairs(sorted) do
        local btn = self.botButtons[i]
        if not btn then
            btn = addon.BotButton:Create(frame)
            self.botButtons[i] = btn
        end
        btn:ClearAllPoints()
        btn:SetPoint("BOTTOM", prevAnchor, "TOP", 0, SPACING)
        btn:SetData(alt)
        btn:Show()
        prevAnchor = btn
    end

    -- Resize the host frame so its height equals the visible stack. This
    -- keeps mouseover regions tight; the stack itself uses anchor chaining
    -- so absolute height isn't strictly needed for layout.
    local rows = 3 + #sorted
    frame:SetHeight(rows * BUTTON_SIZE + (rows - 1) * SPACING)
end

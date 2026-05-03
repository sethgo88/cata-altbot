-- RoleMenu.lua — vertical 3-icon role picker (Tank / Healer / DPS).
--
-- Triggered from BotSubMenu's Role button. Anchored above the Role button so
-- it grows up like the roster popout. Each icon dispatches SET_ROLE with its
-- slug; the server validates whether the bot's class can perform the role.
--
-- Uses the LFG role icon sheet (Interface/LFGFrame/UI-LFG-ICON-PORTRAITROLES)
-- so the icons match the ones WoW already shows for role-checks.

local _, addon = ...
local RM = {}
addon.RoleMenu = RM

local BUTTON_SIZE = 32
local SPACING     = 4

local frame = CreateFrame("Frame", "CataAltbotRoleMenu", UIParent)
frame:SetFrameStrata("TOOLTIP")    -- above BotSubMenu
frame:SetSize(BUTTON_SIZE, BUTTON_SIZE * 3 + SPACING * 2)
frame:Hide()

-- LFG icon sheet coords (TANK on top, HEALER mid, DAMAGER bottom — chosen
-- order in the popout is Tank, Healer, DPS top→bottom).
local ROLES = {
    { slug = "tank",   label = "Tank",   coords = { 0,        19/64, 22/64,  41/64 } },
    { slug = "healer", label = "Healer", coords = { 20/64,  39/64,  1/64,  20/64 } },
    { slug = "dps",    label = "DPS",    coords = { 20/64,  39/64, 22/64,  41/64 } },
}

local LFG_ROLES_TEX = "Interface\\LFGFrame\\UI-LFG-ICON-PORTRAITROLES"

local function MakeRoleButton(parent, role)
    local b = CreateFrame("Button", nil, parent)
    b:SetSize(BUTTON_SIZE, BUTTON_SIZE)

    b.icon = b:CreateTexture(nil, "BACKGROUND")
    b.icon:SetAllPoints()
    b.icon:SetTexture(LFG_ROLES_TEX)
    b.icon:SetTexCoord(unpack(role.coords))

    b.border = b:CreateTexture(nil, "ARTWORK")
    b.border:SetTexture("Interface\\Buttons\\UI-Quickslot2")
    b.border:SetSize(BUTTON_SIZE * 1.7, BUTTON_SIZE * 1.7)
    b.border:SetPoint("CENTER", b, "CENTER", 0, -1)

    b:SetHighlightTexture("Interface\\Buttons\\ButtonHilight-Square")

    b:SetScript("OnEnter", function(self)
        GameTooltip:SetOwner(self, "ANCHOR_RIGHT")
        GameTooltip:AddLine(role.label)
        GameTooltip:Show()
    end)
    b:SetScript("OnLeave", function() GameTooltip:Hide() end)

    return b
end

RM.buttons = {}
local prev
for i, role in ipairs(ROLES) do
    local b = MakeRoleButton(frame, role)
    if prev then b:SetPoint("BOTTOM", prev, "TOP", 0, SPACING)
    else         b:SetPoint("BOTTOM", frame, "BOTTOM", 0, 0) end
    RM.buttons[i] = b
    prev = b
end

function RM:Hide() frame:Hide() end

function RM:OpenFor(roleAnchorButton, alt)
    if not alt then return end

    frame:ClearAllPoints()
    frame:SetPoint("BOTTOM", roleAnchorButton, "TOP", 0, SPACING)

    -- Toggle: same anchor twice closes.
    if frame:IsShown() and self._currentAlt and self._currentAlt.guidLow == alt.guidLow then
        self:Hide()
        return
    end
    self._currentAlt = alt

    for i, role in ipairs(ROLES) do
        self.buttons[i]:SetScript("OnClick", function()
            addon:SetRole(alt.name, role.slug)
            self:Hide()
        end)
    end

    frame:Show()
end

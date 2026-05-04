-- LootRollMenu.lua — vertical 3-icon loot-roll picker (Wait / Pass / Disenchant).
--
-- Triggered from BotSubMenu's Loot Roll button. Anchored above the button so
-- it grows up like RoleMenu. Each icon dispatches SET_ROLL with its slug; the
-- server stores the per-bot mode and pushes the new STATE back.
--
-- Tri-state, not boolean, so it gets its own popout rather than a toggle —
-- matches Role and the .altbot roll command's vocabulary (wait/pass/disenchant).

local _, addon = ...
local LRM = {}
addon.LootRollMenu = LRM

local BUTTON_SIZE = 32
local SPACING     = 4

local frame = CreateFrame("Frame", "CataAltbotLootRollMenu", UIParent)
frame:SetFrameStrata("TOOLTIP")    -- above BotSubMenu, same as RoleMenu
frame:SetSize(BUTTON_SIZE, BUTTON_SIZE * 3 + SPACING * 2)
frame:Hide()

-- Vertical order top→bottom is: Wait, Pass, Disenchant.
-- We anchor bottom→top, so list bottom-first to match RoleMenu's idiom.
local MODES = {
    { slug = "disenchant", label = "Disenchant",
      icon = "Interface\\Icons\\Inv_Enchant_Disenchant" },
    { slug = "pass",       label = "Pass",
      icon = "Interface\\Icons\\Inv_Misc_GroupLooking" },
    { slug = "wait",       label = "Wait",
      icon = "Interface\\Icons\\Inv_Misc_PocketWatch_01" },
}

local function MakeModeButton(parent, mode)
    local b = CreateFrame("Button", nil, parent)
    b:SetSize(BUTTON_SIZE, BUTTON_SIZE)

    b.icon = b:CreateTexture(nil, "BACKGROUND")
    b.icon:SetAllPoints()
    b.icon:SetTexture(mode.icon)
    b.icon:SetTexCoord(0.08, 0.92, 0.08, 0.92)

    b.border = b:CreateTexture(nil, "ARTWORK")
    b.border:SetTexture("Interface\\Buttons\\UI-Quickslot2")
    b.border:SetSize(BUTTON_SIZE * 1.7, BUTTON_SIZE * 1.7)
    b.border:SetPoint("CENTER", b, "CENTER", 0, -1)

    b:SetHighlightTexture("Interface\\Buttons\\ButtonHilight-Square")

    b:SetScript("OnEnter", function(self)
        GameTooltip:SetOwner(self, "ANCHOR_RIGHT")
        GameTooltip:AddLine(mode.label)
        GameTooltip:Show()
    end)
    b:SetScript("OnLeave", function() GameTooltip:Hide() end)

    return b
end

LRM.buttons = {}
local prev
for i, mode in ipairs(MODES) do
    local b = MakeModeButton(frame, mode)
    if prev then b:SetPoint("BOTTOM", prev, "TOP", 0, SPACING)
    else         b:SetPoint("BOTTOM", frame, "BOTTOM", 0, 0) end
    LRM.buttons[i] = b
    prev = b
end

function LRM:Hide() frame:Hide() end

function LRM:OpenFor(anchorButton, alt)
    if not alt then return end

    frame:ClearAllPoints()
    frame:SetPoint("BOTTOM", anchorButton, "TOP", 0, SPACING)

    -- Toggle: same anchor twice closes.
    if frame:IsShown() and self._currentAlt and self._currentAlt.guidLow == alt.guidLow then
        self:Hide()
        return
    end
    self._currentAlt = alt

    for i, mode in ipairs(MODES) do
        self.buttons[i]:SetScript("OnClick", function()
            addon:SetRoll(alt.name, mode.slug)
            self:Hide()
        end)
    end

    frame:Show()
end

-- Launcher.lua — persistent entry point for the panel.
--
-- Two surfaces, both always available when the addon is loaded:
--   1. A standalone draggable icon button anchored on UIParent. Click =
--      toggle MainFrame; right-click = refresh; shift-drag = reposition.
--      Position persists in CataAltbotDB.iconPosition.
--   2. A LibDataBroker-1.1 launcher data object — picked up automatically
--      by Bartender4, Titan Panel, ChocolateBar, etc. Only registered if
--      LibStub + LDB are present; we don't bundle our own copy.
--
-- The two surfaces are independent: hiding one doesn't affect the other.

local _, addon = ...
local L = {}
addon.Launcher = L

local ICON = "Interface\\Icons\\INV_Misc_GroupLooking"

local function ToggleMainFrame()
    if not (addon.MainFrame and addon.MainFrame.IsShown) then
        DEFAULT_CHAT_FRAME:AddMessage("|cffff5555[CataAltbot]|r MainFrame not loaded — check /console scriptErrors 1 then /reload for Lua errors.")
        return
    end
    if addon.MainFrame:IsShown() then addon.MainFrame:Hide()
    else                              addon.MainFrame:Show() end
end
addon.ToggleMainFrame = ToggleMainFrame

-- ---- Standalone draggable icon ----

local btn = CreateFrame("Button", "CataAltbotLauncher", UIParent)
btn:SetSize(32, 32)
btn:SetFrameStrata("MEDIUM")
btn:SetMovable(true)
btn:EnableMouse(true)
btn:RegisterForDrag("LeftButton")
btn:RegisterForClicks("AnyUp")

local iconTex = btn:CreateTexture(nil, "ARTWORK")
iconTex:SetAllPoints()
iconTex:SetTexture(ICON)
iconTex:SetTexCoord(0.08, 0.92, 0.08, 0.92)

-- Minimap-style ring border so the button reads as a clickable.
local border = btn:CreateTexture(nil, "OVERLAY")
border:SetTexture("Interface\\Minimap\\MiniMap-TrackingBorder")
border:SetSize(54, 54)
border:SetPoint("TOPLEFT", -10, 10)

btn:SetHighlightTexture("Interface\\Buttons\\ButtonHilight-Square")

local function ApplySavedPosition()
    local pos = (CataAltbotDB and CataAltbotDB.iconPosition) or { x = -100, y = 0 }
    btn:ClearAllPoints()
    btn:SetPoint("CENTER", UIParent, "CENTER", pos.x, pos.y)
end

btn:SetScript("OnDragStart", function(self)
    if IsShiftKeyDown() then self:StartMoving() end
end)
btn:SetScript("OnDragStop", function(self)
    self:StopMovingOrSizing()
    if not CataAltbotDB then return end
    local _, _, _, x, y = self:GetPoint()
    CataAltbotDB.iconPosition = { x = x, y = y }
end)

btn:SetScript("OnClick", function(_, button)
    if button == "RightButton" then addon:RefreshAlts()
    else                            ToggleMainFrame() end
end)

btn:SetScript("OnEnter", function(self)
    GameTooltip:SetOwner(self, "ANCHOR_LEFT")
    GameTooltip:AddLine("CataAltbot")
    GameTooltip:AddLine("|cffaaaaaaLeft-click:|r toggle panel")
    GameTooltip:AddLine("|cffaaaaaaRight-click:|r refresh roster")
    GameTooltip:AddLine("|cffaaaaaaShift+drag:|r reposition")
    GameTooltip:Show()
end)
btn:SetScript("OnLeave", function() GameTooltip:Hide() end)

L.button = btn
btn:Hide()

function L:Show()
    ApplySavedPosition()
    btn:Show()
end

function L:Hide() btn:Hide() end

-- ---- LibDataBroker launcher (Bartender4 / Titan Panel / etc.) ----

if LibStub then
    local LDB = LibStub("LibDataBroker-1.1", true)
    if LDB and not LDB:GetDataObjectByName("CataAltbot") then
        L.ldb = LDB:NewDataObject("CataAltbot", {
            type    = "launcher",
            text    = "CataAltbot",
            icon    = ICON,
            OnClick = function(_, button)
                if button == "RightButton" then addon:RefreshAlts()
                else                            ToggleMainFrame() end
            end,
            OnTooltipShow = function(tt)
                tt:AddLine("CataAltbot")
                tt:AddLine("|cffaaaaaaLeft-click:|r toggle panel")
                tt:AddLine("|cffaaaaaaRight-click:|r refresh roster")
            end,
        })
    end
end

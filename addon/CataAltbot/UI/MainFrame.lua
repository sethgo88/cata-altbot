-- MainFrame.lua — top-level panel: header, refresh, alt list.

local addon = CataAltbot
local MF = {}
addon.MainFrame = MF

local frame = CreateFrame("Frame", "CataAltbotMainFrame", UIParent)
frame:SetSize(420, 480)
frame:SetPoint("CENTER")
frame:SetMovable(true)
frame:EnableMouse(true)
frame:RegisterForDrag("LeftButton")
frame:SetScript("OnDragStart", frame.StartMoving)
frame:SetScript("OnDragStop", function(self)
    self:StopMovingOrSizing()
    local point, _, _, x, y = self:GetPoint()
    CataAltbotDB.position = { point = point, x = x, y = y }
end)

frame:SetBackdrop({
    bgFile   = "Interface\\Tooltips\\UI-Tooltip-Background",
    edgeFile = "Interface\\Tooltips\\UI-Tooltip-Border",
    tile = true, tileSize = 16, edgeSize = 16,
    insets = { left = 4, right = 4, top = 4, bottom = 4 },
})
frame:SetBackdropColor(0, 0, 0, 0.85)

-- Header
local title = frame:CreateFontString(nil, "OVERLAY", "GameFontHighlight")
title:SetPoint("TOPLEFT", 16, -12)
title:SetText("CataAltbot")

local close = CreateFrame("Button", nil, frame, "UIPanelCloseButton")
close:SetPoint("TOPRIGHT", -4, -4)

local refresh = CreateFrame("Button", nil, frame, "UIPanelButtonTemplate")
refresh:SetSize(80, 22)
refresh:SetPoint("TOPLEFT", 12, -36)
refresh:SetText("Refresh")
refresh:SetScript("OnClick", function() addon:RefreshAlts() end)

local addBtn = CreateFrame("Button", nil, frame, "UIPanelButtonTemplate")
addBtn:SetSize(80, 22)
addBtn:SetPoint("LEFT", refresh, "RIGHT", 4, 0)
addBtn:SetText("Add...")
addBtn:SetScript("OnClick", function()
    StaticPopup_Show("CATAALTBOT_ADD")
end)

StaticPopupDialogs["CATAALTBOT_ADD"] = {
    text = "Character name to register as an altbot:",
    button1 = "Add", button2 = "Cancel",
    hasEditBox = true, maxLetters = 12,
    OnAccept = function(self) addon:Add(self.editBox:GetText()) addon:RefreshAlts() end,
    EditBoxOnEnterPressed = function(self) addon:Add(self:GetText()) self:GetParent():Hide() addon:RefreshAlts() end,
    timeout = 0, hideOnEscape = true, whileDead = true,
}

-- Scrolling list
local scroll = CreateFrame("ScrollFrame", "CataAltbotScroll", frame, "UIPanelScrollFrameTemplate")
scroll:SetPoint("TOPLEFT", 12, -68)
scroll:SetPoint("BOTTOMRIGHT", -32, 12)

local content = CreateFrame("Frame", nil, scroll)
content:SetSize(380, 1)
scroll:SetScrollChild(content)

local rows = {}

function MF:Refresh()
    -- Hide all rows.
    for _, row in ipairs(rows) do row:Hide() end

    local i = 0
    local sorted = {}
    for _, alt in pairs(addon.alts) do table.insert(sorted, alt) end
    table.sort(sorted, function(a, b) return (a.name or "") < (b.name or "") end)

    for _, alt in ipairs(sorted) do
        i = i + 1
        local row = rows[i]
        if not row then
            row = addon.AltRow:Create(content)
            rows[i] = row
        end
        row:SetPoint("TOPLEFT", 0, -((i - 1) * 50))
        row:SetData(alt)
        row:Show()
    end

    content:SetHeight(math.max(i * 50, 1))
end

function MF:Show()
    frame:Show()
    if next(addon.alts) == nil then addon:RefreshAlts() end
end

function MF:Hide()  frame:Hide()      end
function MF:IsShown() return frame:IsShown() end

frame:Hide()

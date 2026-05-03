-- MainFrame.lua — top-level panel: header, refresh, broadcast bar, alt list.
--
-- Broadcast bar drives every active bot at once (Follow All / Stay All /
-- Attack Target / assist mode / toggles / group ops). Per-bot configuration
-- (login, spec, talents, bags) lives on the AltRow.
--
-- Toggle/assist visuals show the FIRST active bot's STATE as a snapshot.
-- Clicking a toggle broadcasts the new value to every bot via SET_TOGGLE_ALL.

local _, addon = ...
local MF = {}
addon.MainFrame = MF

local TOGGLES = {
    { key = "mount",   label = "Mount"   },
    { key = "loot",    label = "Loot"    },
    { key = "pass",    label = "Pass"    },
    { key = "release", label = "Release" },
    { key = "qtake",   label = "QTake"   },
    { key = "qturn",   label = "QTurn"   },
}

local ASSIST_MODES = {
    { key = "off",    label = "Off",    serverId = "0" },
    { key = "target", label = "Target", serverId = "1" },
    { key = "skull",  label = "Skull",  serverId = "2" },
    { key = "both",   label = "Both",   serverId = "3" },
}

local frame = CreateFrame("Frame", "CataAltbotMainFrame", UIParent)
frame:SetSize(460, 600)
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

local title = frame:CreateFontString(nil, "OVERLAY", "GameFontHighlight")
title:SetPoint("TOPLEFT", 16, -12)
title:SetText("CataAltbot")

local close = CreateFrame("Button", nil, frame, "UIPanelCloseButton")
close:SetPoint("TOPRIGHT", -4, -4)

-- ---- Top action row: refresh + add ----

local refresh = CreateFrame("Button", nil, frame, "UIPanelButtonTemplate")
refresh:SetSize(80, 22)
refresh:SetPoint("TOPLEFT", 12, -36)
refresh:SetText("Refresh")
refresh:SetScript("OnClick", function() addon:RefreshAlts() end)

local addBtn = CreateFrame("Button", nil, frame, "UIPanelButtonTemplate")
addBtn:SetSize(80, 22)
addBtn:SetPoint("LEFT", refresh, "RIGHT", 4, 0)
addBtn:SetText("Add...")
addBtn:SetScript("OnClick", function() StaticPopup_Show("CATAALTBOT_ADD") end)

StaticPopupDialogs["CATAALTBOT_ADD"] = {
    text = "Character name to register as an altbot:",
    button1 = "Add", button2 = "Cancel",
    hasEditBox = true, maxLetters = 12,
    OnAccept = function(self) addon:Add(self.editBox:GetText()); addon:RefreshAlts() end,
    EditBoxOnEnterPressed = function(self) addon:Add(self:GetText()); self:GetParent():Hide(); addon:RefreshAlts() end,
    timeout = 0, hideOnEscape = true, whileDead = true,
}

-- ---- Broadcast bar ----

local bar = CreateFrame("Frame", nil, frame)
bar:SetPoint("TOPLEFT", 12, -68)
bar:SetPoint("TOPRIGHT", -12, -68)
bar:SetHeight(160)

local function mkLabel(parent, text, x, y)
    local l = parent:CreateFontString(nil, "OVERLAY", "GameFontNormalSmall")
    l:SetPoint("TOPLEFT", x, y)
    l:SetText(text)
    return l
end

local function mkBtn(parent, label, w, x, y, anchor)
    local b = CreateFrame("Button", nil, parent, "UIPanelButtonTemplate")
    b:SetSize(w, 22)
    if anchor then b:SetPoint("LEFT", anchor, "RIGHT", 4, 0)
    else           b:SetPoint("TOPLEFT", x, y) end
    b:SetText(label)
    return b
end

-- Row 1: Mode
mkLabel(bar, "Mode:", 0, -2)
MF.btnFollowAll = mkBtn(bar, "Follow All", 90, 60, 0)
MF.btnStayAll   = mkBtn(bar, "Stay All",   90, nil, nil, MF.btnFollowAll)

MF.btnFollowAll:SetScript("OnClick", function() addon:SetModeAll("follow") end)
MF.btnStayAll  :SetScript("OnClick", function() addon:SetModeAll("stay")   end)

-- Row 2: Combat
mkLabel(bar, "Combat:", 0, -28)
MF.btnAttack = mkBtn(bar, "Attack Target", 110, 60, -26)
MF.btnAttack:SetScript("OnClick", function() addon:AttackAll() end)

-- Row 3: Assist (radio-style CheckButtons)
mkLabel(bar, "Assist:", 0, -54)

MF.assistChecks = {}
local prev
for i, mode in ipairs(ASSIST_MODES) do
    local cb = CreateFrame("CheckButton", "CataAltbotAssist"..mode.key, bar, "UICheckButtonTemplate")
    cb:SetSize(20, 20)
    if prev then cb:SetPoint("LEFT", prev.label, "RIGHT", 4, 0)
    else         cb:SetPoint("TOPLEFT", 60, -50) end
    local lbl = cb:CreateFontString(nil, "OVERLAY", "GameFontNormalSmall")
    lbl:SetPoint("LEFT", cb, "RIGHT", 0, 0)
    lbl:SetText(mode.label)
    cb.label = lbl
    cb:SetScript("OnClick", function(self)
        addon:SetAssistAll(mode.key)
        -- Uncheck siblings; STATE push will reconcile shortly anyway.
        for _, other in ipairs(MF.assistChecks) do
            other:SetChecked(other == self)
        end
    end)
    MF.assistChecks[i] = cb
    prev = cb
end

-- Row 4: Group ops
mkLabel(bar, "Group:", 0, -80)
MF.btnInviteAll   = mkBtn(bar, "Invite All",   90, 60, -78)
MF.btnSummonAll   = mkBtn(bar, "Summon All",   90, nil, nil, MF.btnInviteAll)
MF.btnUninviteAll = mkBtn(bar, "Uninvite All", 90, nil, nil, MF.btnSummonAll)

MF.btnInviteAll  :SetScript("OnClick", function() addon:InviteAll()   end)
MF.btnSummonAll  :SetScript("OnClick", function() addon:SummonAll()   end)
MF.btnUninviteAll:SetScript("OnClick", function() addon:UninviteAll() end)

-- Row 5-6: Toggles (6 checkboxes, 3 per row)
mkLabel(bar, "Toggles:", 0, -106)

MF.toggleChecks = {}
for i, t in ipairs(TOGGLES) do
    local col = (i - 1) % 3
    local row = math.floor((i - 1) / 3)
    local cb = CreateFrame("CheckButton", "CataAltbotToggle"..t.key, bar, "UICheckButtonTemplate")
    cb:SetSize(20, 20)
    cb:SetPoint("TOPLEFT", 60 + col * 110, -104 - row * 26)
    local lbl = cb:CreateFontString(nil, "OVERLAY", "GameFontNormalSmall")
    lbl:SetPoint("LEFT", cb, "RIGHT", 0, 0)
    lbl:SetText(t.label)
    cb:SetScript("OnClick", function(self)
        addon:SetToggleAll(t.key, self:GetChecked() and true or false)
    end)
    MF.toggleChecks[t.key] = cb
end

-- ---- Scrolling alt list ----

local scroll = CreateFrame("ScrollFrame", "CataAltbotScroll", frame, "UIPanelScrollFrameTemplate")
scroll:SetPoint("TOPLEFT", 12, -240)
scroll:SetPoint("BOTTOMRIGHT", -32, 12)

local content = CreateFrame("Frame", nil, scroll)
content:SetSize(420, 1)
scroll:SetScrollChild(content)

local rows = {}

-- ---- State sync ----

-- Snapshot the first active bot's STATE; used to populate broadcast-bar
-- visuals. Returns nil if no active bot has reported state yet.
local function snapshotState()
    for _, alt in pairs(addon.alts) do
        if alt.active and alt.state then return alt.state end
    end
    return nil
end

function MF:RefreshBroadcastBar()
    local has = addon:HasActiveBot()

    -- Gate every broadcast control on "at least one active bot".
    local ctrls = {
        self.btnFollowAll, self.btnStayAll, self.btnAttack,
        self.btnInviteAll, self.btnSummonAll, self.btnUninviteAll,
    }
    for _, b in ipairs(ctrls) do b:SetEnabled(has) end
    for _, cb in ipairs(self.assistChecks) do cb:SetEnabled(has) end
    for _, cb in pairs(self.toggleChecks)  do cb:SetEnabled(has) end

    -- Reflect snapshot state on the controls.
    local s = snapshotState()
    if not s then
        for _, cb in ipairs(self.assistChecks) do cb:SetChecked(false) end
        for _, cb in pairs(self.toggleChecks)  do cb:SetChecked(false) end
        return
    end

    for i, mode in ipairs(ASSIST_MODES) do
        self.assistChecks[i]:SetChecked(s.assist == mode.serverId)
    end

    for _, t in ipairs(TOGGLES) do
        self.toggleChecks[t.key]:SetChecked(s[t.key] == "1")
    end
end

function MF:Refresh()
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
        row:SetPoint("TOPLEFT", 0, -((i - 1) * 56))
        row:SetData(alt)
        row:Show()
    end

    content:SetHeight(math.max(i * 56, 1))

    self:RefreshBroadcastBar()
end

function MF:Show()
    frame:Show()
    if next(addon.alts) == nil then addon:RefreshAlts() end
end

function MF:Hide()    frame:Hide()      end
function MF:IsShown() return frame:IsShown() end

frame:Hide()

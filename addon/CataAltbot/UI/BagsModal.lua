-- BagsModal.lua — full inventory grid for one bot.
--
-- Activated by clicking a "Bags" button on an AltRow (added in Phase 9).
-- Sends BAGS request, accumulates BAG_ROW replies, renders a grid with
-- per-item action buttons (equip / sell / drop). Item icon and tooltip
-- come from GetItemInfo on the client side.

local addon = CataAltbot
local BM = {}
addon.BagsModal = BM

BM.pending = {}      -- per-bot: collected BAG_ROW entries during fetch
BM.current = nil     -- {botGuid, botName, items}

local CELL_SIZE = 36
local GRID_COLS = 8
local GRID_PAD  = 4

local function makeFrame()
    local f = CreateFrame("Frame", "CataAltbotBagsModal", UIParent)
    f:SetSize(360, 320)
    f:SetPoint("CENTER")
    f:SetMovable(true)
    f:EnableMouse(true)
    f:RegisterForDrag("LeftButton")
    f:SetScript("OnDragStart", f.StartMoving)
    f:SetScript("OnDragStop", f.StopMovingOrSizing)
    f:SetBackdrop({
        bgFile   = "Interface\\Tooltips\\UI-Tooltip-Background",
        edgeFile = "Interface\\Tooltips\\UI-Tooltip-Border",
        tile = true, tileSize = 16, edgeSize = 16,
        insets = { left = 4, right = 4, top = 4, bottom = 4 },
    })
    f:SetBackdropColor(0, 0, 0, 0.9)

    f.title = f:CreateFontString(nil, "OVERLAY", "GameFontHighlight")
    f.title:SetPoint("TOPLEFT", 12, -10)

    f.close = CreateFrame("Button", nil, f, "UIPanelCloseButton")
    f.close:SetPoint("TOPRIGHT", -4, -4)

    f.grid = CreateFrame("Frame", nil, f)
    f.grid:SetPoint("TOPLEFT", 12, -32)
    f.grid:SetPoint("BOTTOMRIGHT", -12, 60)

    -- Action panel (bottom).
    f.action = CreateFrame("Frame", nil, f)
    f.action:SetPoint("BOTTOMLEFT", 12, 12)
    f.action:SetPoint("BOTTOMRIGHT", -12, 12)
    f.action:SetHeight(40)

    f.label = f.action:CreateFontString(nil, "OVERLAY", "GameFontDisableSmall")
    f.label:SetPoint("TOPLEFT", 0, 0)
    f.label:SetText("Click an item to select; then Equip / Sell / Drop.")

    local function mkBtn(text, anchor)
        local b = CreateFrame("Button", nil, f.action, "UIPanelButtonTemplate")
        b:SetSize(70, 22)
        if anchor then b:SetPoint("LEFT", anchor, "RIGHT", 4, 0)
        else           b:SetPoint("BOTTOMLEFT", 0, 0) end
        b:SetText(text)
        return b
    end

    f.btnEquip = mkBtn("Equip")
    f.btnSell  = mkBtn("Sell",  f.btnEquip)
    f.btnDrop  = mkBtn("Drop",  f.btnSell)
    f.btnTrade = mkBtn("Trade", f.btnDrop)

    f.cells = {}

    return f
end

function BM:Open(botName, botGuid)
    if not self.frame then self.frame = makeFrame() end
    self.current = { botGuid = botGuid, botName = botName, items = {} }
    self.pending[botGuid] = {}
    self.frame.title:SetText("Bags — " .. botName)
    self.frame:Show()

    addon:RequestBags(botName)

    local f = self.frame
    f.btnEquip:SetScript("OnClick", function() self:DoAction("EQUIP") end)
    f.btnSell:SetScript ("OnClick", function() self:DoAction("SELL")  end)
    f.btnDrop:SetScript ("OnClick", function() self:DoAction("DROP")  end)
    f.btnTrade:SetScript("OnClick", function() addon:Send(botName, "TRADE|" .. botName) end)
end

function BM:OnBagRow(parts)
    local botGuid = tonumber(parts[2]); if not botGuid then return end
    self.pending[botGuid] = self.pending[botGuid] or {}
    table.insert(self.pending[botGuid], {
        bag     = tonumber(parts[3]),
        slot    = tonumber(parts[4]),
        entry   = tonumber(parts[5]),
        count   = tonumber(parts[6]),
        guidLow = tonumber(parts[7]),
    })
end

function BM:OnBagsDone(parts)
    local botGuid = tonumber(parts[2]); if not botGuid then return end
    if not self.current or self.current.botGuid ~= botGuid then return end
    self.current.items = self.pending[botGuid] or {}
    self:Render()
end

local function makeCell(parent)
    local c = CreateFrame("Button", nil, parent)
    c:SetSize(CELL_SIZE, CELL_SIZE)
    c.icon = c:CreateTexture(nil, "ARTWORK")
    c.icon:SetAllPoints()
    c.count = c:CreateFontString(nil, "OVERLAY", "NumberFontNormal")
    c.count:SetPoint("BOTTOMRIGHT", -2, 2)
    c:SetScript("OnEnter", function(self)
        if not self.entry then return end
        GameTooltip:SetOwner(self, "ANCHOR_RIGHT")
        GameTooltip:SetItemByID(self.entry)
        GameTooltip:Show()
    end)
    c:SetScript("OnLeave", function() GameTooltip:Hide() end)
    return c
end

function BM:Render()
    if not self.current or not self.frame then return end
    local f = self.frame
    local items = self.current.items

    -- Hide existing cells.
    for _, c in ipairs(f.cells) do c:Hide() end

    for i, it in ipairs(items) do
        local cell = f.cells[i]
        if not cell then
            cell = makeCell(f.grid)
            f.cells[i] = cell
        end
        local row = math.floor((i - 1) / GRID_COLS)
        local col = (i - 1) % GRID_COLS
        cell:SetPoint("TOPLEFT", col * (CELL_SIZE + GRID_PAD), -row * (CELL_SIZE + GRID_PAD))
        local _, _, _, _, _, _, _, _, _, icon = GetItemInfo(it.entry)
        cell.icon:SetTexture(icon or "Interface\\Icons\\INV_Misc_QuestionMark")
        cell.count:SetText(it.count > 1 and tostring(it.count) or "")
        cell.entry   = it.entry
        cell.guidLow = it.guidLow
        cell.name    = it.name or ""
        cell:SetScript("OnClick", function()
            self.selected = it
            f.label:SetText("Selected: item " .. it.entry .. " (guid " .. it.guidLow .. ")")
        end)
        cell:Show()
    end

    f.label:SetText(string.format("%d item(s). Click to select.", #items))
end

function BM:DoAction(verb)
    if not self.current or not self.selected then return end
    local payload = string.format("%s|%s|%d", verb, self.current.botName, self.selected.guidLow)
    addon:Send(self.current.botName, payload)
end

-- CharacterModal.lua — paper-doll-style character sheet for one bot.
--
-- We can't reuse Blizzard's PaperDollFrame because it reads "player" data via
-- hardcoded UnitStat / GetInventoryItemLink calls; layering a bot on top would
-- break the real frame whenever both were open. So this is a clone: same slot
-- artwork, same 3D-model preview when we can resolve a unit token, same
-- category-based stat panel — fed by the GEAR_ROW / STATS / GEAR_DONE replies
-- from the server.

local _, addon = ...
local CM = {}
addon.CharacterModal = CM

CM.pending = {}    -- per-bot accumulator: [botGuid] = { gear = {[slot]=row}, stats = nil }
CM.current = nil   -- {botGuid, botName, classId, level, gear, stats}

-- Slot metadata. Slot indices match server-side EquipmentSlots enum
-- (EQUIPMENT_SLOT_HEAD = 0 ... EQUIPMENT_SLOT_TABARD = 18). Order in this
-- table is the visual layout order, not the slot index. xCol = "L" (left
-- column) / "R" (right column) / "B" (bottom row); yIdx is row index 0..7
-- within the column or x position 0..2 for the bottom row.
local SLOT_DEFS = {
    -- Left column (head down to wrist).
    { slot = 0,  xCol = "L", yIdx = 0, bg = "Interface\\PaperDollInfoFrame\\UI-PaperDoll-Slot-Head"      },
    { slot = 1,  xCol = "L", yIdx = 1, bg = "Interface\\PaperDollInfoFrame\\UI-PaperDoll-Slot-Neck"      },
    { slot = 2,  xCol = "L", yIdx = 2, bg = "Interface\\PaperDollInfoFrame\\UI-PaperDoll-Slot-Shoulder"  },
    { slot = 14, xCol = "L", yIdx = 3, bg = "Interface\\PaperDollInfoFrame\\UI-PaperDoll-Slot-Chest"     }, -- back uses chest backdrop in retail
    { slot = 4,  xCol = "L", yIdx = 4, bg = "Interface\\PaperDollInfoFrame\\UI-PaperDoll-Slot-Chest"     },
    { slot = 3,  xCol = "L", yIdx = 5, bg = "Interface\\PaperDollInfoFrame\\UI-PaperDoll-Slot-Shirt"     },
    { slot = 18, xCol = "L", yIdx = 6, bg = "Interface\\PaperDollInfoFrame\\UI-PaperDoll-Slot-Tabard"    },
    { slot = 8,  xCol = "L", yIdx = 7, bg = "Interface\\PaperDollInfoFrame\\UI-PaperDoll-Slot-Wrists"    },
    -- Right column (hands down to trinket2).
    { slot = 9,  xCol = "R", yIdx = 0, bg = "Interface\\PaperDollInfoFrame\\UI-PaperDoll-Slot-Hands"     },
    { slot = 5,  xCol = "R", yIdx = 1, bg = "Interface\\PaperDollInfoFrame\\UI-PaperDoll-Slot-Waist"     },
    { slot = 6,  xCol = "R", yIdx = 2, bg = "Interface\\PaperDollInfoFrame\\UI-PaperDoll-Slot-Legs"      },
    { slot = 7,  xCol = "R", yIdx = 3, bg = "Interface\\PaperDollInfoFrame\\UI-PaperDoll-Slot-Feet"      },
    { slot = 10, xCol = "R", yIdx = 4, bg = "Interface\\PaperDollInfoFrame\\UI-PaperDoll-Slot-Finger"    },
    { slot = 11, xCol = "R", yIdx = 5, bg = "Interface\\PaperDollInfoFrame\\UI-PaperDoll-Slot-Finger"    },
    { slot = 12, xCol = "R", yIdx = 6, bg = "Interface\\PaperDollInfoFrame\\UI-PaperDoll-Slot-Trinket"   },
    { slot = 13, xCol = "R", yIdx = 7, bg = "Interface\\PaperDollInfoFrame\\UI-PaperDoll-Slot-Trinket"   },
    -- Bottom row (weapons).
    { slot = 15, xCol = "B", yIdx = 0, bg = "Interface\\PaperDollInfoFrame\\UI-PaperDoll-Slot-MainHand"  },
    { slot = 16, xCol = "B", yIdx = 1, bg = "Interface\\PaperDollInfoFrame\\UI-PaperDoll-Slot-SecondaryHand" },
    { slot = 17, xCol = "B", yIdx = 2, bg = "Interface\\PaperDollInfoFrame\\UI-PaperDoll-Slot-Ranged"    },
}

local SLOT_NAMES = {
    [0]="Head", [1]="Neck", [2]="Shoulders", [3]="Shirt", [4]="Chest", [5]="Waist",
    [6]="Legs", [7]="Feet", [8]="Wrist", [9]="Hands", [10]="Finger", [11]="Finger",
    [12]="Trinket", [13]="Trinket", [14]="Back", [15]="Main Hand", [16]="Off Hand",
    [17]="Ranged", [18]="Tabard",
}

-- TexCoord lookup for the standard class-icon atlas
-- (Interface\TargetingFrame\UI-CLASSES-CIRCLES). Used as the 3D-model fallback
-- when the bot isn't in the master's party/raid.
local CLASS_ICON_COORDS = {
    [1]  = {0,    0.25, 0,    0.25}, -- Warrior
    [2]  = {0,    0.25, 0.5,  0.75}, -- Paladin
    [3]  = {0,    0.25, 0.25, 0.5 }, -- Hunter
    [4]  = {0.49, 0.74, 0,    0.25}, -- Rogue
    [5]  = {0.49, 0.74, 0.25, 0.5 }, -- Priest
    [6]  = {0.25, 0.49, 0.5,  0.75}, -- Death Knight
    [7]  = {0.25, 0.49, 0.25, 0.5 }, -- Shaman
    [8]  = {0.25, 0.49, 0,    0.25}, -- Mage
    [9]  = {0.74, 0.98, 0.25, 0.5 }, -- Warlock
    [11] = {0.74, 0.98, 0,    0.25}, -- Druid
}

local CELL    = 40
local PAD     = 2
local PANEL_W = 540
local PANEL_H = 460

local STAT_CATEGORIES = { "General", "Melee", "Ranged", "Spell", "Defense" }

local function fmtPct(v)
    return string.format("%.2f%%", tonumber(v) or 0)
end
local function fmtInt(v)
    return tostring(math.floor(tonumber(v) or 0))
end
local function fmtSpeed(ms)
    return string.format("%.2f", (tonumber(ms) or 0) / 1000)
end

-- ---- Frame construction -------------------------------------------------

function CM:_BuildFrame()
    local f = CreateFrame("Frame", "CataAltbotCharacterModal", UIParent)
    f:SetSize(PANEL_W, PANEL_H)
    f:SetPoint("CENTER")
    f:SetMovable(true)
    f:EnableMouse(true)
    f:SetFrameStrata("HIGH")
    f:RegisterForDrag("LeftButton")
    f:SetScript("OnDragStart", f.StartMoving)
    f:SetScript("OnDragStop",  f.StopMovingOrSizing)
    f:SetBackdrop({
        bgFile   = "Interface\\Tooltips\\UI-Tooltip-Background",
        edgeFile = "Interface\\Tooltips\\UI-Tooltip-Border",
        tile = true, tileSize = 16, edgeSize = 16,
        insets = { left = 4, right = 4, top = 4, bottom = 4 },
    })
    f:SetBackdropColor(0, 0, 0, 0.92)

    f.title = f:CreateFontString(nil, "OVERLAY", "GameFontHighlight")
    f.title:SetPoint("TOPLEFT", 12, -10)

    f.close = CreateFrame("Button", nil, f, "UIPanelCloseButton")
    f.close:SetPoint("TOPRIGHT", -4, -4)

    -- Three columns: model | slots | stats. Each gets its own holder so we can
    -- anchor children relative to a stable origin without fighting the
    -- backdrop insets.
    f.modelHolder = CreateFrame("Frame", nil, f)
    f.modelHolder:SetSize(160, 320)
    f.modelHolder:SetPoint("TOPLEFT", 12, -36)

    f.slotHolder = CreateFrame("Frame", nil, f)
    f.slotHolder:SetSize(2 * CELL + PAD + 4, 8 * CELL + 7 * PAD)
    f.slotHolder:SetPoint("TOPLEFT", f.modelHolder, "TOPRIGHT", 8, 0)

    f.bottomRow = CreateFrame("Frame", nil, f)
    f.bottomRow:SetSize(3 * CELL + 2 * PAD, CELL)
    f.bottomRow:SetPoint("TOPLEFT", f.slotHolder, "BOTTOMLEFT", 0, -8)

    f.statHolder = CreateFrame("Frame", nil, f)
    f.statHolder:SetPoint("TOPLEFT", f.slotHolder, "TOPRIGHT", 16, 0)
    f.statHolder:SetPoint("BOTTOMRIGHT", f, "BOTTOMRIGHT", -12, 12)

    -- 3D model + class-crest fallback.
    f.model = CreateFrame("PlayerModel", nil, f.modelHolder)
    f.model:SetAllPoints()
    f.crest = f.modelHolder:CreateTexture(nil, "ARTWORK")
    f.crest:SetTexture("Interface\\TargetingFrame\\UI-CLASSES-CIRCLES")
    f.crest:SetSize(96, 96)
    f.crest:SetPoint("CENTER")
    f.crest:Hide()

    f.subtitle = f:CreateFontString(nil, "OVERLAY", "GameFontNormalSmall")
    f.subtitle:SetPoint("TOP", f.modelHolder, "BOTTOM", 0, -2)

    -- Slot buttons.
    f.slotButtons = {}
    for _, def in ipairs(SLOT_DEFS) do
        local b = self:_MakeSlotButton(def, f)
        f.slotButtons[def.slot] = b
    end

    -- Stat panel: category-button strip + scrollable list.
    f.catButtons = {}
    for i, name in ipairs(STAT_CATEGORIES) do
        local b = CreateFrame("Button", nil, f.statHolder, "UIPanelButtonTemplate")
        b:SetSize(60, 18)
        b:SetText(name)
        if i == 1 then b:SetPoint("TOPLEFT", 0, 0)
        else           b:SetPoint("LEFT", f.catButtons[i - 1], "RIGHT", 2, 0) end
        b:SetScript("OnClick", function() CM:_ShowCategory(name) end)
        f.catButtons[i] = b
    end

    f.statBox = CreateFrame("Frame", nil, f.statHolder)
    f.statBox:SetPoint("TOPLEFT", f.catButtons[1], "BOTTOMLEFT", 0, -6)
    f.statBox:SetPoint("BOTTOMRIGHT", f.statHolder, "BOTTOMRIGHT", 0, 0)

    f.statText = f.statBox:CreateFontString(nil, "OVERLAY", "GameFontNormal")
    f.statText:SetPoint("TOPLEFT")
    f.statText:SetPoint("TOPRIGHT")
    f.statText:SetJustifyH("LEFT")
    f.statText:SetJustifyV("TOP")
    f.statText:SetWidth(180)

    return f
end

function CM:_MakeSlotButton(def, parent)
    local b = CreateFrame("Button", nil, parent.slotHolder)
    b:SetSize(CELL, CELL)

    -- Position relative to slotHolder / bottomRow.
    if def.xCol == "L" then
        b:SetPoint("TOPLEFT", parent.slotHolder, "TOPLEFT", 0, -def.yIdx * (CELL + PAD))
    elseif def.xCol == "R" then
        b:SetPoint("TOPRIGHT", parent.slotHolder, "TOPRIGHT", 0, -def.yIdx * (CELL + PAD))
    else
        b:SetParent(parent.bottomRow)
        b:SetPoint("LEFT", parent.bottomRow, "LEFT", def.yIdx * (CELL + PAD), 0)
    end

    b.bg = b:CreateTexture(nil, "BACKGROUND")
    b.bg:SetAllPoints()
    b.bg:SetTexture(def.bg)
    -- Slot art ships with a thick frame; pull in a hair so the icon dominates.
    b.bg:SetTexCoord(0.07, 0.93, 0.07, 0.93)

    b.icon = b:CreateTexture(nil, "ARTWORK")
    b.icon:SetPoint("TOPLEFT", 2, -2)
    b.icon:SetPoint("BOTTOMRIGHT", -2, 2)
    b.icon:SetTexCoord(0.08, 0.92, 0.08, 0.92)
    b.icon:Hide()

    b.border = b:CreateTexture(nil, "OVERLAY")
    b.border:SetTexture("Interface\\Buttons\\UI-ActionButton-Border")
    b.border:SetBlendMode("ADD")
    b.border:SetAllPoints()
    b.border:Hide()

    b:SetScript("OnEnter", function(self)
        local row = CM.current and CM.current.gear and CM.current.gear[def.slot]
        GameTooltip:SetOwner(self, "ANCHOR_RIGHT")
        if row and row.entry then
            -- Full hyperlink so enchant + gem rows render in the tooltip.
            local lvl = (CM.current and CM.current.level) or 1
            local link = string.format("item:%d:%d:%d:%d:%d:0:0:0:%d",
                row.entry, row.enchantId or 0, row.gem1 or 0, row.gem2 or 0, row.gem3 or 0, lvl)
            GameTooltip:SetHyperlink(link)
        else
            GameTooltip:SetText(SLOT_NAMES[def.slot] or "Slot " .. def.slot)
        end
        GameTooltip:Show()
    end)
    b:SetScript("OnLeave", function() GameTooltip:Hide() end)

    return b
end

-- ---- Lifecycle ----------------------------------------------------------

function CM:Open(botName, botGuid, classId, level)
    if not self.frame then self.frame = self:_BuildFrame() end
    self.current = {
        botGuid  = botGuid,
        botName  = botName,
        classId  = classId,
        level    = level or 1,
        gear     = {},
        stats    = nil,
        category = "General",
    }
    self.pending[botGuid] = { gear = {}, stats = nil }
    self.frame.title:SetText("Character — " .. botName)
    self.frame.subtitle:SetText(string.format("Level %d", level or 0))
    self:_TryResolveModelUnit(botName, classId)
    self:Render()
    self.frame:Show()

    addon:RequestGear(botName)
end

function CM:OnGearRow(parts)
    local botGuid = tonumber(parts[2]); if not botGuid then return end
    local row = {
        slot      = tonumber(parts[3]),
        entry     = tonumber(parts[4]),
        enchantId = tonumber(parts[5]),
        gem1      = tonumber(parts[6]),
        gem2      = tonumber(parts[7]),
        gem3      = tonumber(parts[8]),
        ilvl      = tonumber(parts[9]),
        guidLow   = tonumber(parts[10]),
    }
    self.pending[botGuid] = self.pending[botGuid] or { gear = {}, stats = nil }
    self.pending[botGuid].gear[row.slot] = row
end

function CM:OnStats(parts)
    local botGuid = tonumber(parts[2]); if not botGuid then return end
    local kv = {}
    for tok in string.gmatch(parts[3] or "", "([^;]+)") do
        local k, v = string.match(tok, "([^=]+)=(.+)")
        if k and v then kv[k] = v end
    end
    self.pending[botGuid] = self.pending[botGuid] or { gear = {}, stats = nil }
    self.pending[botGuid].stats = kv
end

function CM:OnGearDone(parts)
    local botGuid = tonumber(parts[2]); if not botGuid then return end
    if not self.current or self.current.botGuid ~= botGuid then return end
    local pend = self.pending[botGuid] or { gear = {}, stats = nil }
    self.current.gear  = pend.gear
    self.current.stats = pend.stats
    self:Render()
end

-- ---- Rendering ----------------------------------------------------------

function CM:Render()
    if not self.frame or not self.current then return end
    local f = self.frame

    for _, def in ipairs(SLOT_DEFS) do
        local b = f.slotButtons[def.slot]
        local row = self.current.gear[def.slot]
        if row and row.entry then
            local _, _, _, _, _, _, _, _, _, icon = GetItemInfo(row.entry)
            b.icon:SetTexture(icon or "Interface\\Icons\\INV_Misc_QuestionMark")
            b.icon:Show()
        else
            b.icon:Hide()
        end
    end

    self:_ShowCategory(self.current.category or "General")
end

function CM:_ShowCategory(name)
    if not self.frame or not self.current then return end
    self.current.category = name
    local s = self.current.stats or {}
    local lines = {}

    local function add(label, value) table.insert(lines, label .. "  |cffffffff" .. value .. "|r") end

    if name == "General" then
        add("Item Level", fmtInt(s.ilvl))
        add("Health",     fmtInt(s.maxhp))
        add("Power",      fmtInt(s.maxpower))
        add("Strength",   fmtInt(s.str))
        add("Agility",    fmtInt(s.agi))
        add("Stamina",    fmtInt(s.sta))
        add("Intellect",  fmtInt(s.int))
        add("Spirit",     fmtInt(s.spi))
        add("Mastery",    string.format("%s (%s rating)", fmtPct(s.mastery), fmtInt(s.masteryrating)))
    elseif name == "Melee" then
        add("Damage",        string.format("%s - %s", fmtInt(s.mainmin), fmtInt(s.mainmax)))
        add("Speed",         fmtSpeed(s.mainspd))
        add("Attack Power",  fmtInt(s.ap))
        add("Hit",           fmtPct(s.meleehit))
        add("Crit",          fmtPct(s.crit))
        add("Expertise",     fmtPct(s.exp))
        add("Haste",         fmtPct(s.meleehaste))
    elseif name == "Ranged" then
        add("Damage",        string.format("%s - %s", fmtInt(s.rngmin), fmtInt(s.rngmax)))
        add("Speed",         fmtSpeed(s.rngspd))
        add("Attack Power",  fmtInt(s.rap))
        add("Hit",           fmtPct(s.rnghit))
        add("Crit",          fmtPct(s.rngcrit))
        add("Haste",         fmtPct(s.rnghaste))
    elseif name == "Spell" then
        add("Spell Power", fmtInt(s.sp))
        add("Hit",         fmtPct(s.spellhit))
        add("Crit",        fmtPct(s.spellcrit))
        add("Haste",       fmtPct(s.spellhaste))
    elseif name == "Defense" then
        add("Armor",      fmtInt(s.armor))
        add("Dodge",      fmtPct(s.dodge))
        add("Parry",      fmtPct(s.parry))
        add("Block",      fmtPct(s.block))
        add("Resilience", fmtPct(s.resil))
    end

    self.frame.statText:SetText(table.concat(lines, "\n"))
end

-- ---- 3D model resolution -----------------------------------------------

function CM:_TryResolveModelUnit(botName, classId)
    local f = self.frame
    f.crest:Hide()
    f.model:ClearModel()
    f.model:Show()

    local function setUnit(tok)
        f.model:SetUnit(tok)
        f.model:SetCamera(0)
        return true
    end

    for i = 1, 4 do
        local tok = "party" .. i
        if UnitExists(tok) and UnitName(tok) == botName then return setUnit(tok) end
    end
    for i = 1, 40 do
        local tok = "raid" .. i
        if UnitExists(tok) and UnitName(tok) == botName then return setUnit(tok) end
    end

    -- Fallback: class crest. Avoid TargetUnit(name) — it'd steal master's target.
    f.model:Hide()
    local coords = CLASS_ICON_COORDS[classId]
    if coords then
        f.crest:SetTexCoord(coords[1], coords[2], coords[3], coords[4])
    else
        f.crest:SetTexCoord(0, 1, 0, 1)
    end
    f.crest:Show()
    return false
end

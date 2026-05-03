-- AltRow.lua — one row per available alt.
--
-- Operational commands (follow/stay/attack/assist/toggles) are NOT here —
-- they live in the MainFrame broadcast bar. The row is configuration only:
-- login, logout, spec, talents, bags, remove. State badges show what the
-- bot currently has applied so a broadcast click is verifiable at a glance.

local _, addon = ...
local AltRow = {}
addon.AltRow = AltRow

local CLASS_NAMES = {
    [1]="Warrior", [2]="Paladin", [3]="Hunter", [4]="Rogue", [5]="Priest",
    [6]="Death Knight", [7]="Shaman", [8]="Mage", [9]="Warlock", [11]="Druid",
}

local CLASS_COLORS = {
    [1]={0.78,0.61,0.43}, [2]={0.96,0.55,0.73}, [3]={0.67,0.83,0.45},
    [4]={1.00,0.96,0.41}, [5]={1.00,1.00,1.00}, [6]={0.77,0.12,0.23},
    [7]={0.00,0.44,0.87}, [8]={0.41,0.80,0.94}, [9]={0.58,0.51,0.79},
    [11]={1.00,0.49,0.04},
}

-- Server's AltbotMode + AltbotAssistMode enums serialized to STATE strings.
local MODE_LABEL = { ["0"] = "F", ["1"] = "S" }            -- Follow / Stay
local ASSIST_LABEL = { ["0"] = "off", ["1"] = "T", ["2"] = "K", ["3"] = "B" }

-- Which short letter shows for each toggle in the badge strip; column order
-- matches the MainFrame toggles row for visual consistency.
local TOGGLE_KEYS = { "mount", "loot", "pass", "release", "qtake", "qturn" }
local TOGGLE_LETTERS = { mount = "M", loot = "L", pass = "P", release = "R", qtake = "Q", qturn = "T" }

function AltRow:Create(parent)
    local row = CreateFrame("Frame", nil, parent)
    row:SetSize(420, 56)

    row.bg = row:CreateTexture(nil, "BACKGROUND")
    row.bg:SetAllPoints()
    row.bg:SetTexture(0.1, 0.1, 0.1, 0.5)

    row.name = row:CreateFontString(nil, "OVERLAY", "GameFontNormal")
    row.name:SetPoint("TOPLEFT", 6, -4)

    row.status = row:CreateFontString(nil, "OVERLAY", "GameFontDisableSmall")
    row.status:SetPoint("TOPLEFT", row.name, "BOTTOMLEFT", 0, -2)

    row.badges = row:CreateFontString(nil, "OVERLAY", "GameFontDisableSmall")
    row.badges:SetPoint("LEFT", row.status, "RIGHT", 8, 0)

    -- Spec dropdown (top-right). Only meaningful when the bot is active.
    row.specDropdown = CreateFrame("Frame", nil, row, "UIDropDownMenuTemplate")
    row.specDropdown:SetPoint("TOPRIGHT", 4, -2)

    -- Action buttons (bottom-right, right-to-left).
    local function mkBtn(label, anchor)
        local b = CreateFrame("Button", nil, row, "UIPanelButtonTemplate")
        b:SetSize(62, 20)
        if anchor then b:SetPoint("RIGHT", anchor, "LEFT", -2, 0)
        else           b:SetPoint("BOTTOMRIGHT", -4, 4) end
        b:SetText(label)
        return b
    end

    row.btnRemove  = mkBtn("Remove")
    row.btnTalents = mkBtn("Talents", row.btnRemove)
    row.btnBags    = mkBtn("Bags",    row.btnTalents)
    row.btnLogout  = mkBtn("Logout",  row.btnBags)
    row.btnLogin   = mkBtn("Login",   row.btnLogout)

    function row:SetData(alt)
        self.alt = alt

        local cls   = CLASS_NAMES[alt.classId] or ("class " .. tostring(alt.classId))
        local color = CLASS_COLORS[alt.classId]
        local nameStr = string.format("%s  |cffaaaaaa(lvl %d %s)|r",
            alt.name or "?", alt.level or 0, cls)
        self.name:SetText(nameStr)
        if color then
            self.name:SetTextColor(color[1], color[2], color[3])
        else
            self.name:SetTextColor(1, 1, 1)
        end

        local statusText = alt.active and "|cff55ff55ACTIVE|r"
                        or (alt.registered and "|cffaaaaffregistered|r" or "|cffffaa00available|r")
        self.status:SetText(statusText .. "  |cff888888acct " .. (alt.accountId or 0) .. "|r")

        -- State badge strip — only meaningful when active.
        if alt.active and alt.state then
            local s = alt.state
            local mode   = MODE_LABEL[s.mode]    or "?"
            local assist = ASSIST_LABEL[s.assist] or "?"
            local togs   = ""
            for _, k in ipairs(TOGGLE_KEYS) do
                local on = s[k] == "1"
                togs = togs .. (on
                    and ("|cff55ff55" .. TOGGLE_LETTERS[k] .. "|r")
                    or  ("|cff444444" .. TOGGLE_LETTERS[k] .. "|r"))
            end
            self.badges:SetText("|cffaaaaaa[|r" .. mode .. " " .. assist .. " " .. togs .. "|cffaaaaaa]|r")
        else
            self.badges:SetText("")
        end

        -- Spec dropdown.
        if alt.active and addon.SpecMenu and addon.SpecMenu.SPECS_BY_CLASS[alt.classId] then
            self.specDropdown:Show()
            local botName = alt.name
            local classId = alt.classId
            addon.SpecMenu:Attach(self.specDropdown, classId,
                function() return alt.state and alt.state.spec or "auto" end,
                function(slug) addon:SetSpec(botName, slug) end)
        else
            self.specDropdown:Hide()
        end

        -- Login is the single entry point for any inactive alt: registered
        -- ones spawn directly via LOGIN, unregistered ones hit ADD which
        -- does register + spawn server-side in one call.
        self.btnLogin  :SetEnabled(not alt.active)
        self.btnLogout :SetEnabled(alt.active)
        self.btnRemove :SetEnabled(alt.registered)
        self.btnBags   :SetEnabled(alt.active)
        self.btnTalents:SetEnabled(alt.active)

        self.btnLogin:SetScript("OnClick", function()
            if alt.registered then addon:Login(alt.name)
            else                   addon:Add(alt.name) end
            addon:RefreshAlts()
        end)
        self.btnLogout :SetScript("OnClick", function() addon:Logout (alt.name); addon:RefreshAlts() end)
        self.btnRemove :SetScript("OnClick", function() addon:Remove (alt.name); addon:RefreshAlts() end)
        self.btnBags   :SetScript("OnClick", function() addon.BagsModal:Open(alt.name, alt.guidLow) end)
        self.btnTalents:SetScript("OnClick", function() addon.TalentsModal:Show(alt.name) end)
    end

    return row
end

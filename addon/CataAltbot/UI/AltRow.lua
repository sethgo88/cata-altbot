-- AltRow.lua — one row per available alt. Shows class-colored name, level,
-- status, and inline action buttons.

local addon = CataAltbot
local AltRow = {}
addon.AltRow = AltRow

local CLASS_NAMES = {
    [1]="Warrior", [2]="Paladin", [3]="Hunter", [4]="Rogue", [5]="Priest",
    [6]="Death Knight", [7]="Shaman", [8]="Mage", [9]="Warlock", [11]="Druid",
}

function AltRow:Create(parent)
    local row = CreateFrame("Frame", nil, parent)
    row:SetSize(380, 48)

    row.bg = row:CreateTexture(nil, "BACKGROUND")
    row.bg:SetAllPoints()
    row.bg:SetTexture(0.1, 0.1, 0.1, 0.5)

    row.name = row:CreateFontString(nil, "OVERLAY", "GameFontNormal")
    row.name:SetPoint("TOPLEFT", 6, -4)

    row.sub = row:CreateFontString(nil, "OVERLAY", "GameFontDisableSmall")
    row.sub:SetPoint("TOPLEFT", row.name, "BOTTOMLEFT", 0, -2)

    -- Action buttons (right-aligned).
    local function mkBtn(label, anchor, dx)
        local b = CreateFrame("Button", nil, row, "UIPanelButtonTemplate")
        b:SetSize(60, 18)
        if anchor then b:SetPoint("RIGHT", anchor, "LEFT", -2, 0)
        else           b:SetPoint("RIGHT", -4, 8) end
        b:SetText(label)
        return b
    end

    row.btnLogin    = mkBtn("Login")
    row.btnLogout   = mkBtn("Logout",   row.btnLogin)
    row.btnInvite   = mkBtn("Invite",   row.btnLogout)
    row.btnSummon   = mkBtn("Summon",   row.btnInvite)

    row.btnFollow   = mkBtn("Follow")
    row.btnFollow:ClearAllPoints()
    row.btnFollow:SetPoint("RIGHT", -4, -10)

    row.btnStay     = mkBtn("Stay",     row.btnFollow)
    row.btnAttack   = mkBtn("Attack",   row.btnStay)
    row.btnBags     = mkBtn("Bags",     row.btnAttack)
    row.btnTalents  = mkBtn("Talents",  row.btnBags)
    row.btnRemove   = mkBtn("Remove",   row.btnTalents)

    function row:SetData(alt)
        self.alt = alt
        local cls = CLASS_NAMES[alt.classId] or ("class " .. tostring(alt.classId))
        self.name:SetText(string.format("%s  |cffaaaaaa(lvl %d %s)|r",
            alt.name or "?", alt.level or 0, cls))
        local status = alt.active and "|cff55ff55ACTIVE|r"
                     or (alt.registered and "|cffaaaaffregistered|r" or "|cffffaa00available|r")
        self.sub:SetText(status .. "  |cff888888account " .. (alt.accountId or 0) .. "|r")

        self.btnLogin:SetEnabled(alt.registered and not alt.active)
        self.btnLogout:SetEnabled(alt.active)
        self.btnRemove:SetEnabled(alt.registered)
        for _, b in ipairs({self.btnInvite, self.btnSummon, self.btnFollow, self.btnStay, self.btnAttack, self.btnBags, self.btnTalents}) do
            b:SetEnabled(alt.active)
        end

        self.btnLogin:SetScript  ("OnClick", function() addon:Login   (alt.name) end)
        self.btnLogout:SetScript ("OnClick", function() addon:Logout  (alt.name) end)
        self.btnRemove:SetScript ("OnClick", function() addon:Remove  (alt.name); addon:RefreshAlts() end)
        self.btnInvite:SetScript ("OnClick", function() addon:Invite  (alt.name) end)
        self.btnSummon:SetScript ("OnClick", function() addon:Summon  (alt.name) end)
        self.btnFollow:SetScript ("OnClick", function() addon:SetMode (alt.name, "follow") end)
        self.btnStay:SetScript   ("OnClick", function() addon:SetMode (alt.name, "stay")   end)
        self.btnAttack:SetScript ("OnClick", function() addon:Send    (alt.name, "ATTACK|" .. alt.name) end)
        self.btnBags:SetScript   ("OnClick", function() addon.BagsModal:Open(alt.name, alt.guidLow) end)
        self.btnTalents:SetScript("OnClick", function() addon.TalentsModal:Show(alt.name) end)
    end

    return row
end

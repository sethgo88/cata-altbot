-- BotSubMenu.lua — horizontal action strip for one bot.
--
-- Triggered by right-click on a BotButton. Anchored to the right of the
-- triggering button so it visually extends out of the bot icon.
--
-- Buttons: [Bags] [Character] [Talents] [Spec] [Role] [LootRoll] [Summon] [Login/Logout] [Remove].
-- Spec opens the existing class-aware UIDropDown; Role opens RoleMenu (a
-- vertical 3-icon popout for tank/healer/dps); LootRoll opens LootRollMenu
-- (wait/pass/disenchant); the rest are direct actions.
--
-- Single-instance; OpenFor(botButton, alt) repositions and rebinds.

local _, addon = ...
local SM = {}
addon.BotSubMenu = SM

local BUTTON_SIZE = 32
local SPACING     = 4

-- Frame --------------------------------------------------------------------

local frame = CreateFrame("Frame", "CataAltbotBotSubMenu", UIParent)
frame:SetFrameStrata("HIGH")    -- above RosterPopout so it overlaps cleanly
frame:Hide()

-- Button factory -----------------------------------------------------------

local function MakeIconButton(parent, iconPath)
    local b = CreateFrame("Button", nil, parent)
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

local function SetTip(button, title)
    button:SetScript("OnEnter", function(self)
        GameTooltip:SetOwner(self, "ANCHOR_TOP")
        GameTooltip:AddLine(title)
        GameTooltip:Show()
    end)
    button:SetScript("OnLeave", function() GameTooltip:Hide() end)
end

local ICON_BAGS      = "Interface\\Icons\\INV_Misc_Bag_07"
local ICON_CHARACTER = "Interface\\Icons\\INV_Shirt_GuildTabard_01"
local ICON_TALENT    = "Interface\\Icons\\Spell_Nature_NatureTouchGrow"
local ICON_SPEC    = "Interface\\Icons\\Inv_Inscription_82_Tome_C"
local ICON_ROLE    = "Interface\\Icons\\Achievement_GuildPerk_HavingaBall"
local ICON_SUMMON  = "Interface\\Icons\\Spell_Arcane_TeleportShattrath"
local ICON_LOGIN   = "Interface\\Icons\\Spell_Holy_Resurrection"
local ICON_LOGOUT  = "Interface\\Icons\\Spell_Magic_LesserInvisibilty"
local ICON_REMOVE  = "Interface\\Icons\\Spell_Shadow_DeathPact"

-- Loot-roll mode icons. The button on the strip swaps between these to reflect
-- the bot's current state (mirrors btnToggle's login/logout swap).
local ICON_ROLL_WAIT = "Interface\\Icons\\Inv_Misc_PocketWatch_01"
local ICON_ROLL_PASS = "Interface\\Icons\\Inv_Misc_GroupLooking"
local ICON_ROLL_DE   = "Interface\\Icons\\Inv_Enchant_Disenchant"

-- Build all 8 children once; OpenFor binds them to the active alt.

SM.btnBags      = MakeIconButton(frame, ICON_BAGS);      SetTip(SM.btnBags,      "Bags")
SM.btnCharacter = MakeIconButton(frame, ICON_CHARACTER); SetTip(SM.btnCharacter, "Character")
SM.btnTalents   = MakeIconButton(frame, ICON_TALENT);    SetTip(SM.btnTalents,   "Talents")
SM.btnSpec     = MakeIconButton(frame, ICON_SPEC);      SetTip(SM.btnSpec,     "Spec")
SM.btnRole     = MakeIconButton(frame, ICON_ROLE);      SetTip(SM.btnRole,     "Role")
SM.btnLootRoll = MakeIconButton(frame, ICON_ROLL_WAIT); SetTip(SM.btnLootRoll, "Loot Roll")
SM.btnSummon   = MakeIconButton(frame, ICON_SUMMON);    SetTip(SM.btnSummon,   "Summon")
SM.btnToggle   = MakeIconButton(frame, ICON_LOGIN);     SetTip(SM.btnToggle,   "Login")
SM.btnRemove   = MakeIconButton(frame, ICON_REMOVE);    SetTip(SM.btnRemove,   "Remove")

-- Layout left → right.
local children = { SM.btnBags, SM.btnCharacter, SM.btnTalents, SM.btnSpec, SM.btnRole, SM.btnLootRoll, SM.btnSummon, SM.btnToggle, SM.btnRemove }
for i, b in ipairs(children) do
    if i == 1 then
        b:SetPoint("LEFT", frame, "LEFT", 0, 0)
    else
        b:SetPoint("LEFT", children[i - 1], "RIGHT", SPACING, 0)
    end
end
frame:SetSize(#children * BUTTON_SIZE + (#children - 1) * SPACING, BUTTON_SIZE)

-- Hidden dropdown frame for the spec UIDropDownMenuTemplate. Built lazily so
-- we don't pay the dropdown init cost until the user clicks Spec.
local specDropdown

-- Public API ---------------------------------------------------------------

function SM:Hide()
    frame:Hide()
    if addon.RoleMenu     then addon.RoleMenu:Hide()     end
    if addon.LootRollMenu then addon.LootRollMenu:Hide() end
end

-- Anchor to the right of the bot icon and rebind every button to `alt`.
-- Snapping to a fresh alt on each call lets a single sub-menu instance
-- service the entire roster.
function SM:OpenFor(botButton, alt)
    if not alt then return end

    -- Reposition.
    frame:ClearAllPoints()
    frame:SetPoint("LEFT", botButton, "RIGHT", SPACING, 0)

    -- Toggle behavior: clicking the same bot twice closes the strip.
    if frame:IsShown() and self._currentAlt and self._currentAlt.guidLow == alt.guidLow then
        self:Hide()
        return
    end
    self._currentAlt = alt

    -- Login/logout icon swaps based on active state.
    if alt.active then
        self.btnToggle.icon:SetTexture(ICON_LOGOUT)
        SetTip(self.btnToggle, "Logout")
    else
        self.btnToggle.icon:SetTexture(ICON_LOGIN)
        SetTip(self.btnToggle, "Login")
    end

    -- Loot-roll icon reflects the bot's current mode (Wait=0, Pass=1, DE=2).
    -- state may be nil before the first STATE push; default to Wait.
    local rollMode = tonumber(alt.state and alt.state.roll) or 0
    if     rollMode == 2 then self.btnLootRoll.icon:SetTexture(ICON_ROLL_DE);   SetTip(self.btnLootRoll, "Loot Roll: Disenchant")
    elseif rollMode == 1 then self.btnLootRoll.icon:SetTexture(ICON_ROLL_PASS); SetTip(self.btnLootRoll, "Loot Roll: Pass")
    else                       self.btnLootRoll.icon:SetTexture(ICON_ROLL_WAIT); SetTip(self.btnLootRoll, "Loot Roll: Wait")
    end

    -- Bind all action buttons to this alt.
    self.btnBags:SetScript("OnClick", function()
        if addon.BagsModal then addon.BagsModal:Open(alt.name, alt.guidLow) end
    end)

    self.btnCharacter:SetScript("OnClick", function()
        if addon.CharacterModal then
            addon.CharacterModal:Open(alt.name, alt.guidLow, alt.classId, alt.level)
        end
    end)

    self.btnTalents:SetScript("OnClick", function()
        if addon.TalentsModal then addon.TalentsModal:Show(alt.name) end
    end)

    self.btnSpec:SetScript("OnClick", function(specBtn)
        if not addon.SpecMenu then return end
        if not specDropdown then
            specDropdown = CreateFrame("Frame", "CataAltbotSpecDropdown", UIParent, "UIDropDownMenuTemplate")
        end
        addon.SpecMenu:Attach(specDropdown, alt.classId,
            function() return (alt.state and alt.state.spec) or "auto" end,
            function(slug) addon:SetSpec(alt.name, slug) end)
        -- "cursor" anchor avoids requiring the spec button to have a frame name.
        ToggleDropDownMenu(1, nil, specDropdown, "cursor", 0, 0)
    end)

    self.btnRole:SetScript("OnClick", function()
        if addon.RoleMenu then addon.RoleMenu:OpenFor(self.btnRole, alt) end
    end)

    self.btnLootRoll:SetScript("OnClick", function()
        if addon.LootRollMenu then addon.LootRollMenu:OpenFor(self.btnLootRoll, alt) end
    end)

    self.btnSummon:SetScript("OnClick", function()
        addon:Summon(alt.name)
    end)

    self.btnToggle:SetScript("OnClick", function()
        if alt.active then
            addon:Logout(alt.name)
            alt.active = false
        else
            if alt.registered then addon:Login(alt.name)
            else                   addon:Add  (alt.name) end
            alt.active = true
        end
        -- Immediate repaint of the just-toggled bot's icon — don't wait for the
        -- LIST round-trip. The eventual LIST_DONE will reconcile if the server
        -- disagreed with the optimistic flip.
        if addon.RosterPopout then addon.RosterPopout:Refresh() end
        addon:RefreshAlts()
        SM:Hide()
    end)

    self.btnRemove:SetScript("OnClick", function()
        StaticPopup_Show("CATAALTBOT_CONFIRM_REMOVE", alt.name, nil, alt)
    end)

    frame:Show()
end

StaticPopupDialogs["CATAALTBOT_CONFIRM_REMOVE"] = {
    text = "Remove %s from your altbot roster? Their saved state will be deleted.",
    button1 = "Remove", button2 = "Cancel",
    OnAccept = function(_, alt)
        if alt and alt.name then
            addon:Remove(alt.name)
            addon:RefreshAlts()
        end
    end,
    timeout = 0, hideOnEscape = true, whileDead = true,
}

-- BotButton.lua — class-icon button for one alt in the roster popout.
--
-- Visual: WoW class-icon (CLASS_ICON_TCOORDS) on the standard "circle classes"
-- texture. Brightened (full color) when the bot is active, dimmed to ~40%
-- alpha and desaturated when offline.
--
-- Click semantics:
--   left-click on offline bot -> login + invite + summon (one-touch fielding)
--   left-click on active bot  -> no-op (per UX spec; sub-menu stays the trigger)
--   right-click any bot       -> toggle the BotSubMenu, anchored to this button

local _, addon = ...
local BB = {}
addon.BotButton = BB

local BUTTON_SIZE = 36

-- WoW class tokens used for CLASS_ICON_TCOORDS lookup.
local CLASS_TOKEN = {
    [1]  = "WARRIOR",
    [2]  = "PALADIN",
    [3]  = "HUNTER",
    [4]  = "ROGUE",
    [5]  = "PRIEST",
    [6]  = "DEATHKNIGHT",
    [7]  = "SHAMAN",
    [8]  = "MAGE",
    [9]  = "WARLOCK",
    [11] = "DRUID",
}

local CLASS_ICON_TEXTURE = "Interface\\TargetingFrame\\UI-Classes-Circles"

-- Activate / deactivate visual: greyscale + alpha for offline.
local function ApplyActive(button, active)
    if active then
        button.icon:SetVertexColor(1, 1, 1, 1)
        button.icon:SetDesaturated(false)
    else
        button.icon:SetVertexColor(0.5, 0.5, 0.5, 0.55)
        button.icon:SetDesaturated(true)
    end
end

function BB:Create(parent)
    local b = CreateFrame("Button", nil, parent)
    b:SetSize(BUTTON_SIZE, BUTTON_SIZE)
    b:RegisterForClicks("AnyUp")

    b.icon = b:CreateTexture(nil, "BACKGROUND")
    b.icon:SetAllPoints()
    b.icon:SetTexture(CLASS_ICON_TEXTURE)

    b.border = b:CreateTexture(nil, "ARTWORK")
    b.border:SetTexture("Interface\\Buttons\\UI-Quickslot2")
    b.border:SetSize(BUTTON_SIZE * 1.7, BUTTON_SIZE * 1.7)
    b.border:SetPoint("CENTER", b, "CENTER", 0, -1)

    b:SetHighlightTexture("Interface\\Buttons\\ButtonHilight-Square")

    -- Stash the alt record on the button itself so click/tooltip handlers
    -- can read fresh data without closure capture rot when the row recycles.
    b.alt = nil

    function b:SetData(alt)
        self.alt = alt
        local token = CLASS_TOKEN[alt.classId]
        if token and CLASS_ICON_TCOORDS[token] then
            self.icon:SetTexCoord(unpack(CLASS_ICON_TCOORDS[token]))
        else
            self.icon:SetTexCoord(0, 1, 0, 1)
        end
        ApplyActive(self, alt.active)
    end

    b:SetScript("OnClick", function(self, button)
        local alt = self.alt
        if not alt then return end

        if button == "RightButton" then
            if addon.BotSubMenu then addon.BotSubMenu:OpenFor(self, alt) end
            return
        end

        -- Left-click on offline = field the bot in one shot. The server treats
        -- LOGIN as a no-op for unregistered alts, so we route those through ADD
        -- (which registers + spawns), then INVITE+SUMMON.
        if not alt.active then
            if alt.registered then addon:Login(alt.name)
            else                   addon:Add  (alt.name) end
            addon:Invite(alt.name)
            addon:Summon(alt.name)
            -- Optimistic state flip; LIST_DONE will reconcile.
            alt.active = true
            ApplyActive(self, true)
        end
        -- Left-click on already-active is intentionally a no-op.
    end)

    -- Tooltip — minimal: name and lvl/class.
    b:SetScript("OnEnter", function(self)
        local alt = self.alt
        if not alt then return end
        GameTooltip:SetOwner(self, "ANCHOR_LEFT")
        GameTooltip:AddLine(alt.name or "?")
        local cls = (CLASS_TOKEN[alt.classId] or "?"):lower():gsub("^%l", string.upper)
        GameTooltip:AddLine(string.format("Lvl %d %s", alt.level or 0, cls), 0.7, 0.7, 0.7)
        GameTooltip:AddLine(alt.active and "|cff55ff55Active|r" or "|cffaaaaaaOffline|r")
        GameTooltip:AddLine("|cffaaaaaaRight-click for actions|r")
        GameTooltip:Show()
    end)
    b:SetScript("OnLeave", function() GameTooltip:Hide() end)

    return b
end

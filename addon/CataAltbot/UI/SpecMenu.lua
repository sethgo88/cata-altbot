-- SpecMenu.lua — class-aware spec dropdown for AltRow.
--
-- Slugs match the docs/specs/ filenames (canonical convention). The server
-- stores whichever slug we send into character_altbot.spec_override; unknown
-- slugs harmlessly fall through to the generic combat loop in
-- AltbotStrategyFactory::Create until a per-spec strategy class lands.

local _, addon = ...
local SM = {}
addon.SpecMenu = SM

-- {label, slug} pairs. "auto" reverts to talent-tree autodetect.
SM.SPECS_BY_CLASS = {
    [1]  = { -- Warrior
        { "Auto-detect",  "auto" },
        { "Fury",         "fury-warrior" },
    },
    [2]  = { -- Paladin
        { "Auto-detect",  "auto" },
        { "Holy",         "holy-paladin" },
        { "Retribution",  "retribution-paladin" },
    },
    [3]  = { -- Hunter
        { "Auto-detect",  "auto" },
        { "Marksmanship", "marksmanship-hunter" },
    },
    [4]  = { -- Rogue
        { "Auto-detect",  "auto" },
        { "Combat",       "combat-rogue" },
    },
    [5]  = { -- Priest
        { "Auto-detect",  "auto" },
        { "Holy",         "holy-priest" },
        { "Discipline",   "disc-priest" },
        { "Shadow",       "shadow-priest" },
    },
    [6]  = { -- Death Knight
        { "Auto-detect",  "auto" },
        { "Frost",        "frost-death-knight" },
    },
    [7]  = { -- Shaman
        { "Auto-detect",  "auto" },
        { "Restoration",  "resto-shaman" },
        { "Elemental",    "elemental-shaman" },
        { "Enhancement",  "enhancement-shaman" },
    },
    [8]  = { -- Mage
        { "Auto-detect",  "auto" },
        { "Frost",        "frost-mage" },
    },
    [9]  = { -- Warlock
        { "Auto-detect",  "auto" },
        { "Affliction",   "affliction-warlock" },
    },
    [11] = { -- Druid
        { "Auto-detect",  "auto" },
        { "Restoration",  "resto-druid" },
        { "Balance",      "balance-druid" },
        { "Feral (Cat)",  "feral-cat-druid" },
    },
}

function SM:LabelForSlug(classId, slug)
    local list = self.SPECS_BY_CLASS[classId]
    if not list then return "auto" end
    if not slug or slug == "" then slug = "auto" end
    for _, entry in ipairs(list) do
        if entry[2] == slug then return entry[1] end
    end
    return slug   -- unknown slug — show raw so the user sees what's stored
end

-- Wires a UIDropDownMenu attached to `frame` so it shows the class's specs and
-- calls `onSelect(slug)` when the user picks one. `getCurrentSlug` is a
-- function so the menu always reflects fresh state when reopened.
function SM:Attach(frame, classId, getCurrentSlug, onSelect)
    UIDropDownMenu_SetWidth(frame, 110)

    UIDropDownMenu_Initialize(frame, function(_, level)
        local list = SM.SPECS_BY_CLASS[classId]
        if not list then return end

        local current = getCurrentSlug() or "auto"
        if current == "" then current = "auto" end

        for _, entry in ipairs(list) do
            local info = UIDropDownMenu_CreateInfo()
            info.text     = entry[1]
            info.value    = entry[2]
            info.checked  = (entry[2] == current)
            info.func     = function()
                onSelect(entry[2])
                UIDropDownMenu_SetText(frame, entry[1])
                CloseDropDownMenus()
            end
            UIDropDownMenu_AddButton(info, level)
        end
    end)

    UIDropDownMenu_SetText(frame, SM:LabelForSlug(classId, getCurrentSlug()))
end

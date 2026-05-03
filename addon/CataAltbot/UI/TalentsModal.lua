-- TalentsModal.lua — minimal per-point allocation UI.
--
-- The full Cata talent tree (3 trees × ~26 talents each, with prereq lines)
-- is a substantial UI build. This Phase 9 version is a lean form: master
-- enters talentId + rank, addon sends LEARN_TALENT. The bot's `talents`
-- whisper command supplies the current free-points counter.
--
-- A future polish pass renders the full visual tree using TALENT_ROW data
-- (server-side enumeration of sTalentStore — not implemented in Phase 7
-- since the verb table omits TALENT_ROW from the initial protocol).

local _, addon = ...
local TM = {}
addon.TalentsModal = TM

local function buildFrame()
    local f = CreateFrame("Frame", "CataAltbotTalentsModal", UIParent)
    f:SetSize(320, 180)
    f:SetPoint("CENTER")
    f:SetMovable(true)
    f:EnableMouse(true)
    f:RegisterForDrag("LeftButton")
    f:SetScript("OnDragStart", f.StartMoving)
    f:SetScript("OnDragStop", f.StopMovingOrSizing)
    f:SetBackdrop({
        bgFile = "Interface\\Tooltips\\UI-Tooltip-Background",
        edgeFile = "Interface\\Tooltips\\UI-Tooltip-Border",
        tile = true, tileSize = 16, edgeSize = 16,
        insets = { left = 4, right = 4, top = 4, bottom = 4 },
    })
    f:SetBackdropColor(0, 0, 0, 0.9)

    f.title = f:CreateFontString(nil, "OVERLAY", "GameFontHighlight")
    f.title:SetPoint("TOPLEFT", 12, -10)

    f.close = CreateFrame("Button", nil, f, "UIPanelCloseButton")
    f.close:SetPoint("TOPRIGHT", -4, -4)

    f.help = f:CreateFontString(nil, "OVERLAY", "GameFontDisableSmall")
    f.help:SetPoint("TOPLEFT", 12, -36)
    f.help:SetWidth(290)
    f.help:SetJustifyH("LEFT")
    f.help:SetText("Find talent IDs in the Cata talent calc URL (e.g., wowhead.com/cata/talent-calc/...). The talentId is the spell-tab-row-col integer.")

    local function mkLabel(text, anchor, dy)
        local l = f:CreateFontString(nil, "OVERLAY", "GameFontNormal")
        l:SetPoint("TOPLEFT", 12, dy)
        l:SetText(text)
        return l
    end
    local function mkInput(anchor, dx)
        local e = CreateFrame("EditBox", nil, f, "InputBoxTemplate")
        e:SetSize(80, 22)
        e:SetAutoFocus(false)
        e:SetPoint("LEFT", anchor, "RIGHT", dx or 6, 0)
        return e
    end

    f.talentLabel = mkLabel("Talent ID:", nil, -94)
    f.talentInput = mkInput(f.talentLabel)
    f.rankLabel   = mkLabel("Rank:",      nil, -120)
    f.rankInput   = mkInput(f.rankLabel)

    f.btnLearn = CreateFrame("Button", nil, f, "UIPanelButtonTemplate")
    f.btnLearn:SetSize(80, 22)
    f.btnLearn:SetPoint("BOTTOMRIGHT", -12, 12)
    f.btnLearn:SetText("Learn")

    return f
end

function TM:Show(botName)
    if not self.frame then self.frame = buildFrame() end
    self.frame.title:SetText("Talents — " .. (botName or "?"))
    self.frame:Show()

    self.frame.btnLearn:SetScript("OnClick", function()
        local talentId = tonumber(self.frame.talentInput:GetText())
        local rank     = tonumber(self.frame.rankInput:GetText())
        if not talentId or not rank then
            DEFAULT_CHAT_FRAME:AddMessage("|cffff5555[CataAltbot]|r need numeric talentId + rank")
            return
        end
        addon:LearnTalent(botName, talentId, rank)
    end)
end

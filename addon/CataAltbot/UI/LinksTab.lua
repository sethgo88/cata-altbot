-- LinksTab.lua — show linked accounts; mirror of `.altbot links`.

local addon = CataAltbot
local LT = {}
addon.LinksTab = LT

function LT:Refresh()
    if not self.frame or not self.frame:IsShown() then return end
    self.text:SetText(self:Format())
end

function LT:Format()
    if not addon.links or #addon.links == 0 then
        return "No linked accounts. Use '.altbot link <username>' to bootstrap."
    end
    local lines = { string.format("%d linked account(s):", #addon.links) }
    for _, link in ipairs(addon.links) do
        table.insert(lines, string.format("  %s (account %d)", link.username, link.accountId))
    end
    return table.concat(lines, "\n")
end

-- Lightweight popup window. Shown on demand from a future MainFrame button.
function LT:Show()
    if not self.frame then
        local f = CreateFrame("Frame", "CataAltbotLinksFrame", UIParent)
        f:SetSize(360, 220)
        f:SetPoint("CENTER")
        f:SetBackdrop({
            bgFile = "Interface\\Tooltips\\UI-Tooltip-Background",
            edgeFile = "Interface\\Tooltips\\UI-Tooltip-Border",
            tile = true, tileSize = 16, edgeSize = 16,
            insets = { left = 4, right = 4, top = 4, bottom = 4 },
        })
        f:SetBackdropColor(0, 0, 0, 0.85)

        local title = f:CreateFontString(nil, "OVERLAY", "GameFontHighlight")
        title:SetPoint("TOPLEFT", 12, -10)
        title:SetText("CataAltbot — Linked Accounts")

        local text = f:CreateFontString(nil, "OVERLAY", "GameFontNormal")
        text:SetPoint("TOPLEFT", 12, -36)
        text:SetPoint("BOTTOMRIGHT", -12, 12)
        text:SetJustifyH("LEFT")
        text:SetJustifyV("TOP")

        local close = CreateFrame("Button", nil, f, "UIPanelCloseButton")
        close:SetPoint("TOPRIGHT", -4, -4)

        self.frame = f
        self.text  = text
    end
    self.frame:Show()
    addon:RefreshLinks()
end

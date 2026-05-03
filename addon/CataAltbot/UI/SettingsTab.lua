-- SettingsTab.lua — per-bot toggles. Reads STATE pushes pushed by the server.
-- This is a placeholder file in Phase 8; the full settings UI is wired into
-- the AltRow as a popup panel in a follow-up polish pass.

local addon = CataAltbot
addon.SettingsTab = {}

function addon.SettingsTab:Show(botName)
    -- Toggle helpers callable from chat or a future popup. Keep the addon
    -- functional even before the dedicated tab is built.
    DEFAULT_CHAT_FRAME:AddMessage("|cffffff55[CataAltbot]|r SettingsTab placeholder for "
        .. (botName or "?"))
end

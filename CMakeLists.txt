# cata-altbot CMakeLists.txt
#
# Integration steps:
#   1. Symlink or copy this directory to:
#      server-core/src/server/scripts/Custom/cata-altbot/
#   2. In server-core/src/server/scripts/Custom/CMakeLists.txt add:
#      add_subdirectory(cata-altbot)
#   3. In server-core/src/server/scripts/Custom/custom_script_loader.cpp add:
#      void AddSC_AltbotLoader();
#      // and in the AddCustomScripts() function:
#      AddSC_AltbotLoader();

set(altbot_SOURCES
    src/AltbotAccountLink.cpp
    src/AltbotAddonProto.cpp
    src/AltbotAI.cpp
    src/AltbotAssist.cpp
    src/AltbotPosition.cpp
    src/AltbotPositionManager.cpp
    src/AltbotCombat.cpp
    src/AltbotCommands.cpp
    src/AltbotCommandTable.cpp
    src/AltbotConfig.cpp
    src/AltbotFollow.cpp
    src/AltbotInventory.cpp
    src/AltbotInvite.cpp
    src/AltbotLfg.cpp
    src/AltbotLoader.cpp
    src/AltbotLoot.cpp
    src/AltbotMgr.cpp
    src/AltbotMount.cpp
    src/AltbotQuest.cpp
    src/AltbotRelease.cpp
    src/AltbotRole.cpp
    src/AltbotStrategyFactory.cpp
    src/AltbotTalents.cpp
    src/strategies/AffWarlockStrategy.cpp
    src/strategies/FrostMageStrategy.cpp
    src/strategies/MmHunterStrategy.cpp
    src/strategies/RestoShamanStrategy.cpp
    src/strategies/StrategyUtil.cpp
)

target_sources(scripts PRIVATE ${altbot_SOURCES})
target_include_directories(scripts PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

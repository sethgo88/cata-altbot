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
    src/AltbotMgr.cpp
    src/AltbotLoader.cpp
    src/AltbotAI.cpp
    src/AltbotFollow.cpp
)

target_sources(scripts PRIVATE ${altbot_SOURCES})
target_include_directories(scripts PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

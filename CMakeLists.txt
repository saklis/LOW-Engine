cmake_minimum_required(VERSION 3.30)
project(Lords-of-War)

set(ENGINE_NAME "LowEngine")
set(GAME_NAME "Lords-of-War")

option(BUILD_SHARED_ENGINE "Build LowEngine as a shared library" ON)

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_EXTENSIONS ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/$<CONFIG>)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/$<CONFIG>)

if (UNIX)
    set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
    set(CMAKE_INSTALL_RPATH "$ORIGIN")
endif()

set(ASSETS_DIR "${CMAKE_SOURCE_DIR}/lords-of-war/assets")

###############################################################################
# OUTPUT
###############################################################################

set(BUILD_OUTPUT_DEBUG "${CMAKE_SOURCE_DIR}/bin/Debug")
set(BUILD_OUTPUT_RELEASE "${CMAKE_SOURCE_DIR}/bin/Release")

###############################################################################
# 3-rd PARTY
###############################################################################

# MinGW-libs
set(MINGW_LIBS_DIR "${CMAKE_SOURCE_DIR}/3rd_party/mingw-libs")
set(MINGW_LIBS
        libgcc_s_dw2-1.dll
        libstdc++-6.dll
        libwinpthread-1.dll
)

# SFML
set(SFML_DIR "${CMAKE_SOURCE_DIR}/3rd_party/SFML")
set(BUILD_SHARED_LIBS ON CACHE BOOL "Build SFML as static libraries" FORCE)
set(SFML_BUILD_GRAPHICS ON CACHE BOOL "Build SFML Graphics module" FORCE)
set(SFML_BUILD_WINDOW ON CACHE BOOL "Build SFML Window module" FORCE)
set(SFML_BUILD_SYSTEM ON CACHE BOOL "Build SFML System module" FORCE)
set(SFML_BUILD_AUDIO ON CACHE BOOL "Build SFML Audio module" FORCE)
set(SFML_BUILD_NETWORK OFF CACHE BOOL "Build SFML Network module" FORCE)
set(SFML_BUILD_EXAMPLES OFF CACHE BOOL "Build SFML examples" FORCE)
set(SFML_BUILD_TESTS OFF CACHE BOOL "Build SFML tests/benchmarks" FORCE)
add_subdirectory(${SFML_DIR} EXCLUDE_FROM_ALL)
foreach (sfmlTarget IN ITEMS sfml-system sfml-window sfml-graphics sfml-network sfml-audio)
    if (TARGET ${sfmlTarget})
        set_target_properties(${sfmlTarget} PROPERTIES
                RUNTIME_OUTPUT_DIRECTORY_DEBUG ${BUILD_OUTPUT_DEBUG}
                RUNTIME_OUTPUT_DIRECTORY_RELEASE ${BUILD_OUTPUT_RELEASE}
        )
    endif ()
endforeach ()

# ImGui-SFML + Dear ImGui
set(IMGUI_SFML_FIND_SFML OFF)
set(IMGUI_DIR "${CMAKE_SOURCE_DIR}/3rd_party/DearImGui")
set(IMGUI_SFML_DIR "${CMAKE_SOURCE_DIR}/3rd_party/imgui-sfml")
set(IMGUI_SFML_DEPS sfml-system sfml-window sfml-graphics)
add_subdirectory(${IMGUI_SFML_DIR})
target_link_libraries(ImGui-SFML PRIVATE ${IMGUI_SFML_DEPS})
if (TARGET ImGui-SFML)
    set_target_properties(ImGui-SFML PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY_DEBUG ${BUILD_OUTPUT_DEBUG}
            RUNTIME_OUTPUT_DIRECTORY_RELEASE ${BUILD_OUTPUT_RELEASE}
    )
endif ()

# SPDLOG
set(SPDLOG_DIR "${CMAKE_SOURCE_DIR}/3rd_party/spdlog")
add_subdirectory(${SPDLOG_DIR} EXCLUDE_FROM_ALL)
if (TARGET spdlog)
    set_target_properties(spdlog PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY_DEBUG ${BUILD_OUTPUT_DEBUG}
            RUNTIME_OUTPUT_DIRECTORY_RELEASE ${BUILD_OUTPUT_RELEASE}
    )
endif ()

###############################################################################
# LOW ENGINE RUNTIME
###############################################################################

# engine source
file(GLOB_RECURSE ENGINE_SOURCE_FILES "${PROJECT_SOURCE_DIR}/low-engine/*.cpp")
file(GLOB_RECURSE ENGINE_HEADER_FILES "${PROJECT_SOURCE_DIR}/low-engine/*.h")

# create the engine library (.dll/.so)
if (BUILD_SHARED_ENGINE)
    add_library(${ENGINE_NAME} SHARED
            ${ENGINE_SOURCE_FILES}
            ${ENGINE_HEADER_FILES}
    )
else ()
    add_library(${ENGINE_NAME} STATIC
            ${ENGINE_SOURCE_FILES}
            ${ENGINE_HEADER_FILES}
    )
endif ()
set_target_properties(${ENGINE_NAME} PROPERTIES OUTPUT_NAME "LowEngine" PREFIX "")

# link libraries to engine
target_include_directories(${ENGINE_NAME} PUBLIC
        "${PROJECT_SOURCE_DIR}/low-engine"
        "${SFML_DIR}/include"
        "${SFGUI_DIR}/include"
)

# Link LowEngine to third-party libs
target_link_libraries(${ENGINE_NAME} PUBLIC
        sfml-system
        sfml-window
        sfml-graphics
        sfml-audio
        # ImGui <- linked by ImGui-SFML
        ImGui-SFML
        spdlog::spdlog
)

# High performance logging (optional)
target_compile_definitions(${ENGINE_NAME} PUBLIC SPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_DEBUG)

# output
set_target_properties(${ENGINE_NAME} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY_DEBUG ${BUILD_OUTPUT_DEBUG}
        RUNTIME_OUTPUT_DIRECTORY_RELEASE ${BUILD_OUTPUT_RELEASE}
)

###############################################################################
# LORDS OF WAR GAME
###############################################################################

file(GLOB_RECURSE LOWGAME_SOURCE_FILES "${PROJECT_SOURCE_DIR}/lords-of-war/*.cpp")
file(GLOB_RECURSE LOWGAME_HEADER_FILES "${PROJECT_SOURCE_DIR}/lords-of-war/*.h")

# executable
add_executable(${GAME_NAME} ${LOWGAME_SOURCE_FILES} ${LOWGAME_HEADER_FILES})
set_target_properties(${GAME_NAME} PROPERTIES OUTPUT_NAME "Lords of War")

# Link the executable against our LowEngine library
target_link_libraries(${GAME_NAME} PRIVATE
        LowEngine
)

target_include_directories(${GAME_NAME} PRIVATE
        "${PROJECT_SOURCE_DIR}/lords-of-war"
)

# output

set_target_properties(${GAME_NAME} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY_DEBUG ${BUILD_OUTPUT_DEBUG}
        RUNTIME_OUTPUT_DIRECTORY_RELEASE ${BUILD_OUTPUT_RELEASE}
)

###############################################################################
# MinGW-libs
###############################################################################

if (WIN32)
foreach (libFile IN LISTS MINGW_LIBS)
    add_custom_command(
            TARGET ${GAME_NAME}
            POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${MINGW_LIBS_DIR}/${libFile}"
            "$<IF:$<CONFIG:Debug>,${BUILD_OUTPUT_DEBUG},${BUILD_OUTPUT_RELEASE}>/${libFile}"
            COMMENT "Copying ${libFile} to output dir"
    )
endforeach ()
elseif (UNIX)
    # skip - libraries should be available in OS
endif ()

###############################################################################
# POST-build commands
###############################################################################

# copy assets to output
set(OUTPUT_ASSETS_DIR "$<IF:$<CONFIG:Debug>,${BUILD_OUTPUT_DEBUG},${BUILD_OUTPUT_RELEASE}>/assets")

add_custom_target(copy_assets ALL
        COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different "${ASSETS_DIR}" "${OUTPUT_ASSETS_DIR}"
        COMMENT "Copying assets directory to output"
)

add_dependencies(${PROJECT_NAME} copy_assets)
###############################################################################
# SETUP
###############################################################################

cmake_minimum_required(VERSION 3.10 FATAL_ERROR)

# Policy CMP0079 for ImGUI-SFML when building under MSVC
if (POLICY CMP0079)
    cmake_policy(SET CMP0079 NEW)
endif ()
project(LowEngine)

if (CMAKE_GENERATOR MATCHES "Visual Studio.*")
    set(CMAKE_SYSTEM_VERSION 10.0) # Windows SDK 11.0
endif ()

# define a macro that helps defining an option - "stolen" from SFML
macro(low_set_option var default type docstring)
    if (NOT DEFINED ${var})
        set(${var} ${default})
    endif ()
    set(${var} ${${var}} CACHE ${type} ${docstring} FORCE)
endmacro()

###############################################################################
# CONFIGURATION
###############################################################################

low_set_option(BUILD_LOW_EDITOR ON BOOL "Build the Low Editor along with the engine")
low_set_option(BUILD_LOW_ENGINE_SHARED ON BOOL "Build LowEngine as a shared library")


low_set_option(LOW_ENGINE_NAME "LowEngine" STRING "Name of Low Engine library")
low_set_option(LOW_EDITOR_NAME "LowEditor" STRING "Name of Low Editor executable")

low_set_option(ASSETS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/low-editor/assets" STRING "Asset directory for Low Editor")

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_EXTENSIONS ON)

if (MSVC)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>" CACHE STRING "" FORCE)
endif ()

if (UNIX)
    set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
    set(CMAKE_INSTALL_RPATH "$ORIGIN")
endif ()

###############################################################################
# OUTPUT
###############################################################################

set(BUILD_OUTPUT_DEBUG "${CMAKE_CURRENT_SOURCE_DIR}/bin/Debug")
set(BUILD_OUTPUT_RELEASE "${CMAKE_CURRENT_SOURCE_DIR}/bin/Release")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin/$<CONFIG>)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin/$<CONFIG>)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin/$<CONFIG>)

###############################################################################
# 3-rd PARTY for LOW ENGINE
###############################################################################

# MinGW-libs
set(MINGW_LIBS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/3rd_party/mingw-libs")
set(MINGW_LIBS
        libgcc_s_dw2-1.dll
        libstdc++-6.dll
        libwinpthread-1.dll
)

# SFML
set(SFML_DIR "${CMAKE_CURRENT_SOURCE_DIR}/3rd_party/SFML")
set(BUILD_SHARED_LIBS ON CACHE BOOL "Build SFML as static libraries" FORCE)
set(SFML_BUILD_GRAPHICS ON CACHE BOOL "Build SFML Graphics module" FORCE)
set(SFML_BUILD_WINDOW ON CACHE BOOL "Build SFML Window module" FORCE)
set(SFML_BUILD_SYSTEM ON CACHE BOOL "Build SFML System module" FORCE)
set(SFML_BUILD_AUDIO ON CACHE BOOL "Build SFML Audio module" FORCE)
set(SFML_BUILD_NETWORK OFF CACHE BOOL "Build SFML Network module" FORCE)
set(SFML_BUILD_EXAMPLES OFF CACHE BOOL "Build SFML examples" FORCE)
set(SFML_BUILD_TESTS OFF CACHE BOOL "Build SFML tests/benchmarks" FORCE)
if (CMAKE_GENERATOR MATCHES "Visual Studio")
    set(SFML_GENERATE_PDB ON CACHE BOOL "Include PDB files" FORCE)
endif ()
add_subdirectory(${SFML_DIR} EXCLUDE_FROM_ALL)
foreach (sfmlTarget IN ITEMS sfml-system sfml-window sfml-graphics sfml-network sfml-audio)
    if (TARGET ${sfmlTarget})
        set_target_properties(${sfmlTarget} PROPERTIES
                RUNTIME_OUTPUT_DIRECTORY_DEBUG ${BUILD_OUTPUT_DEBUG}
                RUNTIME_OUTPUT_DIRECTORY_RELEASE ${BUILD_OUTPUT_RELEASE}
        )
    endif ()
endforeach ()

# TGUI (Texus’ GUI for SFML)
set(TGUI_DIR "${CMAKE_CURRENT_SOURCE_DIR}/3rd_party/TGUI")
set(TGUI_BUILD_GUI_BUILDER OFF CACHE BOOL "TRUE to build the GUI Builder" FORCE)
set(TGUI_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(TGUI_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(TGUI_BACKEND SFML_GRAPHICS CACHE STRING "Sets the backend to use for rendering and OS interaction." FORCE)
set(TGUI_SHARED_LIBS ON CACHE BOOL "Sets whether you want to build shared or static libraries." FORCE)
set(TGUI_CXX_STANDARD "20" CACHE STRING "Sets which c++ standard should be used by TGUI." FORCE)
set(TGUI_USE_STATIC_STD_LIBS OFF CACHE BOOL "Sets whether TGUI should link to the dynamic or static version of the std library." FORCE)
add_subdirectory(${TGUI_DIR} EXCLUDE_FROM_ALL)
if (TARGET tgui)
    set_target_properties(tgui PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY_DEBUG ${BUILD_OUTPUT_DEBUG}
            RUNTIME_OUTPUT_DIRECTORY_RELEASE ${BUILD_OUTPUT_RELEASE}
    )
endif ()

# SPDLOG
set(SPDLOG_DIR "${CMAKE_CURRENT_SOURCE_DIR}/3rd_party/spdlog")
add_subdirectory(${SPDLOG_DIR} EXCLUDE_FROM_ALL)
if (TARGET spdlog)
    set_target_properties(spdlog PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY_DEBUG ${BUILD_OUTPUT_DEBUG}
            RUNTIME_OUTPUT_DIRECTORY_RELEASE ${BUILD_OUTPUT_RELEASE}
    )
endif ()

# Nlohmann JSON
set(NLOHMANN_JSON_DIR "${CMAKE_CURRENT_SOURCE_DIR}/3rd_party/nlohmann_json")
add_subdirectory(${NLOHMANN_JSON_DIR} EXCLUDE_FROM_ALL)

###############################################################################
# LOW ENGINE RUNTIME
###############################################################################

# engine source
file(GLOB_RECURSE ENGINE_SOURCE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/low-engine/*.cpp")
file(GLOB_RECURSE ENGINE_HEADER_FILES "${CMAKE_CURRENT_SOURCE_DIR}/low-engine/*.h")

# create the engine library (.dll/.so)
if (BUILD_LOW_ENGINE_SHARED)
    add_library(${LOW_ENGINE_NAME} SHARED
            ${ENGINE_SOURCE_FILES}
            ${ENGINE_HEADER_FILES}
    )
else ()
    add_library(${LOW_ENGINE_NAME} STATIC
            ${ENGINE_SOURCE_FILES}
            ${ENGINE_HEADER_FILES}
    )
endif ()

# export symbols for dynamic library API.
if (MSVC)
    # While MinGW exports all by default, MSVC needs to be told to do so.
    set_target_properties(${LOW_ENGINE_NAME}
            PROPERTIES
            OUTPUT_NAME "LowEngine"
            PREFIX ""
            WINDOWS_EXPORT_ALL_SYMBOLS ON # <- export all symbols ... except not all! Only functions
    )

    # define the export macro (LOWENGINE_EXPORTS) for the engine - required to export global variables
    target_compile_definitions(${LOW_ENGINE_NAME}
            PUBLIC SPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_DEBUG # <- High performance logging
    )
else ()
    set_target_properties(${LOW_ENGINE_NAME}
            PROPERTIES
            OUTPUT_NAME "LowEngine"
            PREFIX ""
    )

    target_compile_definitions(${LOW_ENGINE_NAME}
            PUBLIC SPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_DEBUG # <- High performance logging
    )
endif ()

# link libraries to engine
target_include_directories(${LOW_ENGINE_NAME} PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/low-engine"
        "${SFML_DIR}/include"
        #        "${SFGUI_DIR}/include"
        "${NLOHMANN_JSON_DIR}/include"
)

# Link LowEngine to third-party libs
target_link_libraries(${LOW_ENGINE_NAME} PUBLIC
        sfml-system
        sfml-window
        sfml-graphics
        sfml-audio
        TGUI::TGUI
        spdlog::spdlog
        nlohmann_json::nlohmann_json
)

# output
set_target_properties(${LOW_ENGINE_NAME}
        PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY_DEBUG ${BUILD_OUTPUT_DEBUG}
        RUNTIME_OUTPUT_DIRECTORY_RELEASE ${BUILD_OUTPUT_RELEASE}
)

###############################################################################
# 3-rd PARTY for LOW EDITOR
###############################################################################

# ImGui Pack
set(IMGUI_DIR "${CMAKE_CURRENT_SOURCE_DIR}/3rd_party/DearImGui")
set(IMGUI_SFML_DIR "${CMAKE_CURRENT_SOURCE_DIR}/3rd_party/imgui-sfml")
set(IMGUIFILEDIALOG_DIR "${CMAKE_CURRENT_SOURCE_DIR}/3rd_party/ImGuiFileDialog")

set(IMGUI_SRC
    "${IMGUI_DIR}/imgui.cpp"
    "${IMGUI_DIR}/imgui_draw.cpp"
    "${IMGUI_DIR}/imgui_tables.cpp"
    "${IMGUI_DIR}/imgui_widgets.cpp"
)

set(IMGUI_SFML_SRC
    "${IMGUI_SFML_DIR}/imgui-SFML.cpp"
)

set(IMGUIFILEDIALOG_SRC
    "${IMGUIFILEDIALOG_DIR}/ImGuiFileDialog.cpp"
)

add_library(ImGuiPack SHARED
        ${IMGUI_SRC}
        ${IMGUI_SFML_SRC}
        ${IMGUIFILEDIALOG_SRC}
)

if (MSVC)
    set_target_properties(ImGuiPack
            PROPERTIES
            OUTPUT_NAME "ImGuiPack"
            PREFIX ""
            WINDOWS_EXPORT_ALL_SYMBOLS ON
    )
else ()
    set_target_properties(ImGuiPack
            PROPERTIES
            OUTPUT_NAME "ImGuiPack"
            PREFIX ""
    )
endif ()

target_include_directories(ImGuiPack PUBLIC
        ${IMGUI_DIR}
        ${IMGUI_SFML_DIR}
        ${IMGUIFILEDIALOG_DIR}
        "${SFML_DIR}/include"
)

target_link_libraries(ImGuiPack PUBLIC
        sfml-system
        sfml-window
        sfml-graphics
        opengl32
)

###############################################################################
# LOW EDITOR
###############################################################################

if (BUILD_LOW_EDITOR)

    file(GLOB_RECURSE LOWEDITOR_SOURCE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/low-editor/*.cpp")
    file(GLOB_RECURSE LOWEDITOR_HEADER_FILES "${CMAKE_CURRENT_SOURCE_DIR}/low-editor/*.h")

    # executable
    add_executable(${LOW_EDITOR_NAME} ${LOWEDITOR_SOURCE_FILES} ${LOWEDITOR_HEADER_FILES})
    set_target_properties(${LOW_EDITOR_NAME} PROPERTIES OUTPUT_NAME "LOW Editor")

    # Link the executable against our LowEngine library
    target_link_libraries(${LOW_EDITOR_NAME}
            PRIVATE
            LowEngine
            ImGuiPack # <- ImGui Pack with ImGui, ImGui-SFML and ImGuiFileDialog
    )

    target_include_directories(${LOW_EDITOR_NAME}
            PRIVATE
            "${CMAKE_CURRENT_SOURCE_DIR}/low-editor"
            "${IMGUI_DIR}"
            "${IMGUI_SFML_DIR}"
            "${IMGUIFILEDIALOG_DIR}"
            "${SFML_DIR}/include"
    )

    # output

    set_target_properties(${LOW_EDITOR_NAME} PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY_DEBUG ${BUILD_OUTPUT_DEBUG}
            RUNTIME_OUTPUT_DIRECTORY_RELEASE ${BUILD_OUTPUT_RELEASE}
    )

endif ()

###############################################################################
# MinGW-libs
###############################################################################

if (WIN32)
    if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        foreach (libFile IN LISTS MINGW_LIBS)
            add_custom_command(
                    TARGET ${LOW_ENGINE_NAME}
                    POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E copy_if_different
                    "${MINGW_LIBS_DIR}/${libFile}"
                    "$<IF:$<CONFIG:Debug>,${BUILD_OUTPUT_DEBUG},${BUILD_OUTPUT_RELEASE}>/${libFile}"
                    COMMENT "Copying ${libFile} to output dir"
            )
        endforeach ()
    endif ()
elseif (UNIX)
    # skip - libraries should be available in OS
endif ()

###############################################################################
# POST-build commands
###############################################################################

# copy editor assets to output
if (BUILD_LOW_EDITOR)

    set(OUTPUT_ASSETS_DIR "$<IF:$<CONFIG:Debug>,${BUILD_OUTPUT_DEBUG},${BUILD_OUTPUT_RELEASE}>/assets")

    add_custom_target(copy_assets ALL
            COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different "${ASSETS_DIR}" "${OUTPUT_ASSETS_DIR}"
            COMMENT "Copying assets directory to output"
    )

    add_dependencies(${PROJECT_NAME} copy_assets)

endif ()
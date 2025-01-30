cmake_minimum_required(VERSION 3.30)
project(Lords-of-War)

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS ON)

# include submodules
set(SFML_DIR "${CMAKE_SOURCE_DIR}/3rd_party/SFML")
add_subdirectory(${SFML_DIR} EXCLUDE_FROM_ALL)
set(SPDLOG_DIR "${CMAKE_SOURCE_DIR}/3rd_party/spdlog")
add_subdirectory(${SPDLOG_DIR} EXCLUDE_FROM_ALL)

###############################################################################
# LOW ENGINE RUNTIME
###############################################################################

file(GLOB_RECURSE ENGINE_SOURCE_FILES "${PROJECT_SOURCE_DIR}/low-engine/*.cpp")
file(GLOB_RECURSE ENGINE_HEADER_FILES "${PROJECT_SOURCE_DIR}/low-engine/*.h")

# create the engine shared library (.dll/.so)
add_library(LowEngine SHARED
        ${ENGINE_SOURCE_FILES}
        ${ENGINE_HEADER_FILES}
)
set_target_properties(LowEngine PROPERTIES OUTPUT_NAME "LowEngine")

# link libraries to engine
target_include_directories(LowEngine PRIVATE
        "${PROJECT_SOURCE_DIR}/low-engine"
        "${SFML_DIR}/include"
)

target_link_directories(LowEngine PRIVATE
        "${SFML_DIR}/lib"
)

# Link LowEngine to third-party libs
target_link_libraries(LowEngine PRIVATE
        sfml-system
        sfml-window
        sfml-graphics
        spdlog::spdlog
)
# High performance logging (optional)
target_compile_definitions(LowEngine PRIVATE SPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_DEBUG)

###############################################################################
# LORDS OF WAR GAME
###############################################################################

file(GLOB_RECURSE LOWGAME_SOURCE_FILES "${PROJECT_SOURCE_DIR}/lords-of-war/*.cpp")
file(GLOB_RECURSE LOWGAME_HEADER_FILES "${PROJECT_SOURCE_DIR}/lords-of-war/*.h")

# executable
add_executable(Lords-of-War ${LOWGAME_SOURCE_FILES} ${LOWGAME_HEADER_FILES})

# Link the executable against our LowEngine library
target_link_libraries(Lords-of-War PRIVATE
        LowEngine
)

target_include_directories(Lords-of-War PRIVATE
        "${PROJECT_SOURCE_DIR}/lords-of-war"
)

###############################################################################
# OUTPUT
###############################################################################

# Output directory for the executable
set_target_properties(Lords-of-War PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY_DEBUG   "${CMAKE_SOURCE_DIR}/bin/Debug"
        RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/bin/Release"
)

# Similarly for the DLL (so it lands in the same folder as the EXE, making it easier to run)
set_target_properties(LowEngine PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY_DEBUG   "${CMAKE_SOURCE_DIR}/bin/Debug"
        RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/bin/Release"
)

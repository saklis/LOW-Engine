cmake_minimum_required(VERSION 3.30)
project(LifeSim)

# CONFIGURATION SECTION

set(HEADER_FILES
)

set(SOURCE_FILES
        "${PROJECT_SOURCE_DIR}/source/main.cpp"
)

# CONFIGURATION SECTION END

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS ON)

# submodules
set(SFML_DIR "${CMAKE_SOURCE_DIR}/3rd_party/SFML")
add_subdirectory(${SFML_DIR} EXCLUDE_FROM_ALL)
set(SPDLOG_DIR "${CMAKE_SOURCE_DIR}/3rd_party/spdlog")
add_subdirectory(${SPDLOG_DIR} EXCLUDE_FROM_ALL)

add_executable(${PROJECT_NAME} ${SOURCE_FILES} ${HEADER_FILES})

target_include_directories(${PROJECT_NAME} PRIVATE
        "${PROJECT_SOURCE_DIR}/include"
        "${SFML_DIR}/include"
        "${SPDLOG_DIR}/include"
)

target_link_directories(${PROJECT_NAME} PRIVATE
        "${SFML_DIR}/lib"
        "${SPDLOG_DIR}/cmake-build-release"
)

target_link_libraries(${PROJECT_NAME} PRIVATE
        sfml-system sfml-window sfml-graphics
        spdlog::spdlog
)

# high performance logging
target_compile_definitions(${PROJECT_NAME} PRIVATE SPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_DEBUG)

# output dirs
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR}/bin/Debug)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/bin/Release)

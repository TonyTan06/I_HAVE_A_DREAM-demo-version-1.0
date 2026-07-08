cmake_minimum_required(VERSION 3.20)

project(I_HAVE_A_DREAM VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(ENABLE_TESTS "Enable tests" OFF)

# ============================================================
# Local dependencies
# ============================================================
# Expected structure:
#
# project-root/
# ├── third_party/
# │   ├── raylib/
# │   └── googletest/
# ├── src/
# ├── tests/
# └── CMakeLists.txt
#
# raylib should be version 5.5.
# googletest should be version 1.16.0.
# ============================================================

set(RAYLIB_SOURCE_DIR
    "${CMAKE_CURRENT_SOURCE_DIR}/third_party/raylib"
    CACHE PATH
    "Path to local raylib source directory"
)

if(NOT EXISTS "${RAYLIB_SOURCE_DIR}/CMakeLists.txt")
    message(FATAL_ERROR
        "raylib was not found at: ${RAYLIB_SOURCE_DIR}\n"
        "Please place raylib source code inside third_party/raylib."
    )
endif()

set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(BUILD_GAMES OFF CACHE BOOL "" FORCE)

add_subdirectory(
    "${RAYLIB_SOURCE_DIR}"
    "${CMAKE_BINARY_DIR}/third_party/raylib"
    EXCLUDE_FROM_ALL
)

# ============================================================
# Game domain library
# ============================================================

set(GAME_DOMAIN_SOURCES
    src/character.cpp
    src/player.cpp
    src/enemy.cpp
    src/item.cpp
)

add_library(game_domain ${GAME_DOMAIN_SOURCES})

target_include_directories(game_domain PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# ============================================================
# Main executable
# ============================================================

add_executable(I_HAVE_A_DREAM src/main.cpp)

target_link_libraries(I_HAVE_A_DREAM PRIVATE
    game_domain
    raylib
)

# ============================================================
# Warnings
# ============================================================

function(enable_project_warnings target)
    if(MSVC)
        target_compile_options(${target} PRIVATE /W4 /WX)
    else()
        target_compile_options(${target} PRIVATE -Wall -Wextra -Wpedantic -Werror)
    endif()
endfunction()

enable_project_warnings(game_domain)
enable_project_warnings(I_HAVE_A_DREAM)

# macOS frameworks needed by raylib
if(APPLE)
    target_link_libraries(I_HAVE_A_DREAM PRIVATE
        "-framework IOKit"
        "-framework Cocoa"
        "-framework OpenGL"
    )
endif()

# ============================================================
# Tests
# ============================================================

if(ENABLE_TESTS)
    include(CTest)

    set(GTEST_SOURCE_DIR
        "${CMAKE_CURRENT_SOURCE_DIR}/third_party/googletest"
        CACHE PATH
        "Path to local GoogleTest source directory"
    )

    if(NOT EXISTS "${GTEST_SOURCE_DIR}/CMakeLists.txt")
        message(FATAL_ERROR
            "GoogleTest was not found at: ${GTEST_SOURCE_DIR}\n"
            "Please place GoogleTest source code inside third_party/googletest."
        )
    endif()

    set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)

    add_subdirectory(
        "${GTEST_SOURCE_DIR}"
        "${CMAKE_BINARY_DIR}/third_party/googletest"
        EXCLUDE_FROM_ALL
    )

    file(GLOB TEST_SOURCES CONFIGURE_DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/tests/*_test.cpp"
    )

    if(NOT TEST_SOURCES)
        message(FATAL_ERROR
            "No test sources found. Add at least one file matching tests/*_test.cpp."
        )
    endif()

    add_executable(game_tests ${TEST_SOURCES})

    target_link_libraries(game_tests PRIVATE
        game_domain
        GTest::gtest_main
    )

    enable_project_warnings(game_tests)

    include(GoogleTest)
    gtest_discover_tests(game_tests)

    find_package(Python3 REQUIRED COMPONENTS Interpreter)

    add_test(
        NAME test_file_coverage
        COMMAND ${Python3_EXECUTABLE}
            ${CMAKE_CURRENT_SOURCE_DIR}/scripts/check_test_coverage.py
            --src-dir ${CMAKE_CURRENT_SOURCE_DIR}/src
            --tests-dir ${CMAKE_CURRENT_SOURCE_DIR}/tests
            --exclude main.cpp
    )
endif()
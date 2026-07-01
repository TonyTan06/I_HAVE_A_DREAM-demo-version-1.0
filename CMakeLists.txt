cmake_minimum_required(VERSION 3.20)

project(I_HAVE_A_DREAM VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)

option(ENABLE_TESTS "Enable tests" OFF)
option(ENABLE_SANITIZERS "Enable memory error and leak checking" OFF)

set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(BUILD_GAMES OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
    raylib
    GIT_REPOSITORY https://github.com/raysan5/raylib.git
    GIT_TAG 5.5
    GIT_SHALLOW TRUE
)

FetchContent_MakeAvailable(raylib)

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

add_executable(I_HAVE_A_DREAM src/main.cpp)

target_link_libraries(I_HAVE_A_DREAM PRIVATE
    game_domain
    raylib
)

function(enable_project_warnings target)
    if(MSVC)
        target_compile_options(${target} PRIVATE /W4 /WX)
    else()
        target_compile_options(${target} PRIVATE -Wall -Wextra -Wpedantic -Werror)
    endif()
endfunction()

function(enable_project_sanitizers target)
    if(NOT ENABLE_SANITIZERS)
        return()
    endif()

    if(MSVC)
        target_compile_options(${target} PRIVATE /fsanitize=address /Zi)
    elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
        target_compile_options(${target} PRIVATE
            -fsanitize=address,undefined
            -fno-omit-frame-pointer
        )
        target_link_options(${target} PRIVATE
            -fsanitize=address,undefined
        )
    else()
        message(WARNING
            "ENABLE_SANITIZERS is ON, but ${CMAKE_CXX_COMPILER_ID} is not supported."
        )
    endif()
endfunction()

enable_project_warnings(game_domain)
enable_project_warnings(I_HAVE_A_DREAM)
enable_project_sanitizers(game_domain)
enable_project_sanitizers(I_HAVE_A_DREAM)

if(APPLE)
    target_link_libraries(I_HAVE_A_DREAM PRIVATE
        "-framework IOKit"
        "-framework Cocoa"
        "-framework OpenGL"
    )
endif()

if(ENABLE_TESTS)
    include(CTest)

    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG v1.16.0
        GIT_SHALLOW TRUE
    )
    FetchContent_MakeAvailable(googletest)

    file(GLOB TEST_SOURCES CONFIGURE_DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/tests/*_test.cpp"
    )

    if(NOT TEST_SOURCES)
        message(FATAL_ERROR
            "No test sources found. Add at least one file matching tests/*_test.cpp."
        )
    endif()

    add_executable(game_tests ${TEST_SOURCES})
    target_link_libraries(game_tests PRIVATE game_domain GTest::gtest_main)
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

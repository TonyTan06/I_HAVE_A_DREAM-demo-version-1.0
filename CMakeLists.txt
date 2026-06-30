cmake_minimum_required(VERSION 3.20)

project(I_HAVE_A_DREAM VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(CTest)
include(FetchContent)

# Keep application startup separate from reusable game/domain code.
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
target_link_libraries(I_HAVE_A_DREAM PRIVATE game_domain)

function(enable_project_warnings target)
    if(MSVC)
        target_compile_options(${target} PRIVATE /W4 /WX)
    else()
        target_compile_options(${target} PRIVATE -Wall -Wextra -Wpedantic -Werror)
    endif()
endfunction()

enable_project_warnings(game_domain)
enable_project_warnings(I_HAVE_A_DREAM)

if(BUILD_TESTING)
    # GoogleTest is downloaded by CMake the first time you configure the project.
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

    add_executable(game_tests ${TEST_SOURCES})
    target_link_libraries(game_tests PRIVATE game_domain GTest::gtest_main)
    enable_project_warnings(game_tests)

    include(GoogleTest)
    gtest_discover_tests(game_tests)

    # This test fails CI when a production source file does not have
    # tests/<source_name>_test.cpp. main.cpp is deliberately excluded.
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

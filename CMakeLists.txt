cmake_minimum_required(VERSION 3.20)

project(I_HAVE_A_DREAM
    VERSION 1.0
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(I_HAVE_A_DREAM
    src/main.cpp
    src/player.cpp
    src/character.cpp
)

target_include_directories(I_HAVE_A_DREAM PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

if(MSVC)
    target_compile_options(I_HAVE_A_DREAM PRIVATE /W4 /WX)
else()
    target_compile_options(I_HAVE_A_DREAM PRIVATE
        -Wall
        -Wextra
        -Wpedantic
        -Werror
    )
endif()

include(CTest)

if(BUILD_TESTING)
    add_test(
        NAME main_runs
        COMMAND I_HAVE_A_DREAM
    )

    set_tests_properties(main_runs PROPERTIES
        PASS_REGULAR_EXPRESSION "hello world"
    )
endif()

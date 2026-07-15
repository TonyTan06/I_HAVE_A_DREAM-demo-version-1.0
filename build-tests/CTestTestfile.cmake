# CMake generated Testfile for 
# Source directory: /Users/tan/Desktop/I HAVE A DREAM/I_HAVE_A_DREAM-demo-version-1.0
# Build directory: /Users/tan/Desktop/I HAVE A DREAM/I_HAVE_A_DREAM-demo-version-1.0/build-tests
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
include("/Users/tan/Desktop/I HAVE A DREAM/I_HAVE_A_DREAM-demo-version-1.0/build-tests/game_tests[1]_include.cmake")
add_test([=[test_file_coverage]=] "/usr/local/Frameworks/Python.framework/Versions/3.12/bin/python3.12" "/Users/tan/Desktop/I HAVE A DREAM/I_HAVE_A_DREAM-demo-version-1.0/scripts/check_test_coverage.py" "--src-dir" "/Users/tan/Desktop/I HAVE A DREAM/I_HAVE_A_DREAM-demo-version-1.0/src" "--tests-dir" "/Users/tan/Desktop/I HAVE A DREAM/I_HAVE_A_DREAM-demo-version-1.0/tests" "--exclude" "main.cpp")
set_tests_properties([=[test_file_coverage]=] PROPERTIES  _BACKTRACE_TRIPLES "/Users/tan/Desktop/I HAVE A DREAM/I_HAVE_A_DREAM-demo-version-1.0/CMakeLists.txt;93;add_test;/Users/tan/Desktop/I HAVE A DREAM/I_HAVE_A_DREAM-demo-version-1.0/CMakeLists.txt;0;")
subdirs("_deps/raylib-build")
subdirs("_deps/googletest-build")

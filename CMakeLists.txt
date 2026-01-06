cmake_minimum_required(VERSION 3.10)
project(scanf_project C)

set(CMAKE_C_STANDARD 99)

# PROGRAM 1: Interactive Demo (Uses main.c and your library)
add_executable(my_app main.c my_scanf.c)

# PROGRAM 2: Test Suite (Uses test_suite.c and your library)
add_executable(run_tests test_suite.c my_scanf.c)

# Required to link the math library (libm) for fabs() on Linux/Unix systems
if(UNIX)
    target_link_libraries(run_tests m)
endif()
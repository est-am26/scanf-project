cmake_minimum_required(VERSION 3.20)
project(scanf_project)

set(CMAKE_C_STANDARD 11)

# Aquí le dices qué archivos componen tu programa
add_executable(scanf_project main.c my_scanf.c
        test_suite.c)
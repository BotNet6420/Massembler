cmake_minimum_required(VERSION 4.0)
project(assembler C)

set(CMAKE_C_STANDARD 99)

add_executable(assembler main.c
        symbol_table.c
        symbol_table.h)

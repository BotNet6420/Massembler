cmake_minimum_required(VERSION 3.0)
project(assembler C)

set(CMAKE_C_STANDARD 99)

# Shared library
add_library(assembler_core SHARED
        src/core/symbol_table.c
        src/core/assembler.c
)

target_include_directories(assembler_core
        PUBLIC
        ${PROJECT_SOURCE_DIR}/include/
        PRIVATE
        ${PROJECT_SOURCE_DIR}/src/
)

# CLI tool
add_executable(assembler_cli
    src/main.c
)

target_link_libraries(assembler_cli
    PRIVATE assembler_core
)

# Testing/Debugging executable
add_executable(assembler_testing_cli
    src/testing.c
)

target_include_directories(assembler_testing_cli
    PRIVATE
        ${PROJECT_SOURCE_DIR}/src
)

target_link_libraries(assembler_testing_cli
    PRIVATE assembler_core
)

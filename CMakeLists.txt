cmake_minimum_required(VERSION 3.25.1)
project(massembler C)

set(CMAKE_C_STANDARD 99)

# Shared library
add_library(massembler_core SHARED
        src/core/symbol_table.c
        src/core/assembler.c
        src/core/logger.c
        src/core/logger.h
)

target_include_directories(massembler_core
        PUBLIC
        ${PROJECT_SOURCE_DIR}/include/
        PRIVATE
        ${PROJECT_SOURCE_DIR}/src/
)

# CLI tool
add_executable(massembler
    src/main.c
)

target_link_libraries(massembler
    PRIVATE massembler_core
)

# Testing/Debugging executable
add_executable(testing_cli
    src/testing.c
)

target_include_directories(testing_cli
    PRIVATE
        ${PROJECT_SOURCE_DIR}/src
)

target_link_libraries(testing_cli
    PRIVATE massembler_core
)

cmake_minimum_required(VERSION 3.28)
project(untitled C)

set(CMAKE_C_STANDARD 90)

add_library(untitled STATIC
        operand_validation.c
        first_pass.c
        second_pass.c
        symbol_table.c
        encoder.c
        main.c
        macros.c
        utils.c
        globals.h)

add_executable(assembler main.c
        utils.h)
target_link_libraries(assembler untitled)
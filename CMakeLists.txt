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
        macros.c)

add_executable(assembler main.c)
target_link_libraries(assembler untitled)
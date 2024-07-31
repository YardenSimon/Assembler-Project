cmake_minimum_required(VERSION 3.25)
project(Assembler_Project C)

set(CMAKE_C_STANDARD 90)

add_executable(Assembler_Project main.c macros.c macros.h encoder.h encoder.c first_pass.h first_pass.c operand_validation.c operand_validation.h symbol_table.c symbol_table.h opcode_groups.h opcode_groups.h)

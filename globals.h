#ifndef GLOBALS_H
#define GLOBALS_H

/* File and line constraints */
#define MAX_LINE_LENGTH 80
#define MAX_FILENAME_LENGTH 256

/* Memory constraints */
#define MEMORY_SIZE 4096
#define INITIAL_MEMORY_ADDRESS 100

/* Symbol and label constraints */
#define MAX_LABEL_LENGTH 31
#define MAX_SYMBOL_LENGTH 31

/* Instruction constraints */
#define MAX_OPERANDS 2
#define MAX_OPCODE_LENGTH 4

/* Word size */
#define WORD_SIZE 15

/* Number of registers */
#define NUM_REGISTERS 8

/* Opcode definitions */
#define NUM_OPCODES 16

typedef enum {
    mov, cmp, add, sub, lea, clr, not, inc,
    dec, jmp, bne, red, prn, jsr, rts, stop
} OpCode;

extern const char* OPCODE_NAMES[NUM_OPCODES];


/* Assembler directives */
#define NUM_DIRECTIVES 4
extern const char* DIRECTIVE_NAMES[NUM_DIRECTIVES];

// /* Error codes */
// typedef enum {
//     ERROR_NONE,
//     ERROR_SYNTAX,
//     ERROR_INVALID_LABEL,
//     ERROR_INVALID_OPERAND,
//     ERROR_INVALID_INSTRUCTION,
//     ERROR_FILE_NOT_FOUND,
//     ERROR_MEMORY_ALLOCATION
// } ErrorCode;

/* Register names */
typedef enum {
    R0, R1, R2, R3, R4, R5, R6, R7
} Register;

/* File extensions */
#define SOURCE_FILE_EXT ".as"
#define MACRO_FILE_EXT ".am"
#define OBJECT_FILE_EXT ".ob"
#define EXTERNALS_FILE_EXT ".ext"
#define ENTRIES_FILE_EXT ".ent"

/* Instruction field bit counts */
#define OPCODE_BITS 4
#define SOURCE_ADDR_BITS 4
#define DEST_ADDR_BITS 4
#define ARE_BITS 3

/* ARE (Absolute, Relocatable, External) type */
typedef enum {
    ARE_ABSOLUTE = 4,
    ARE_RELOCATABLE = 2,
    ARE_EXTERNAL = 1
} AREType;

#endif /* GLOBALS_H */
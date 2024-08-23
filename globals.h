#ifndef GLOBALS_H
#define GLOBALS_H

#define MAX_LINE_LENGTH 80
#define MAX_FILENAME_LENGTH 256
#define MEMORY_SIZE 4096
#define INITIAL_MEMORY_ADDRESS 100
#define MAX_LABEL_LENGTH 31
#define MAX_SYMBOL_LENGTH 31
#define MAX_DATA_SECTIONS 100
#define MAX_OPERANDS 2
#define MAX_OPCODE_LENGTH 4
#define WORD_SIZE 15
#define NUM_REGISTERS 8
#define NUM_OPCODES 16

#define MAX_STRINGS 1000
#define MAX_STRING_LENGTH 31
#define NUM_DIRECTIVES 4

extern char string_table[MAX_STRINGS][MAX_STRING_LENGTH];
extern int string_count;
extern char current_filename[MAX_FILENAME_LENGTH];
extern int memory_address;

typedef enum {
    mov, cmp, add, sub, lea, clr, not, inc,
    dec, jmp, bne, red, prn, jsr, rts, stop
} OpCode;

extern const char* OPCODE_NAMES[NUM_OPCODES];
extern const char* DIRECTIVE_NAMES[NUM_DIRECTIVES];

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

/* ARE type */
typedef enum {
    ARE_ABSOLUTE = 4,
    ARE_RELOCATABLE = 2,
    ARE_EXTERNAL = 1
} AREType;

/*
 * The start and end addresses define the range of memory locations used for storing data in each data section.
 * By knowing these addresses, we can precisely identify which memory locations are used for data
 */
typedef struct {
    int start_address;
    int end_address;
} DataSection;

extern DataSection data_sections[MAX_DATA_SECTIONS];
extern int data_section_count;
typedef unsigned short MachineWord;

extern int current_line_number;
#endif
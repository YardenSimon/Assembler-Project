#ifndef ENCODER_H
#define ENCODER_H
#include "globals.h"
#define MAX_OPERAND_LENGTH 20

/* MachineWord is a 15-bit value in assembly language.
 * We store it in a 16-bit unsigned short, but only use in the lower 15 bits. */
extern int memory_size;
extern MachineWord *memory;

/* Node structure for the linked list of encoded data */
typedef struct EncodedDataNode {
 MachineWord word;
 struct EncodedDataNode *next;
} EncodedDataNode;

/* Data structure to store in all the encoded directives */
typedef struct {
 EncodedDataNode *head;
 EncodedDataNode *tail;
 int count;
} EncodedData;

extern EncodedData encoded_data;

/* AddressingMethod enum represents the five addressing methods in our assembly language:
 * 0 - None: No operand
 * 1 - Immediate: A constant value, e.g., #5
 * 2 - Direct: A memory address or label
 * 3 - Index: Addressing using a base address and an index register, e.g., *r3
 * 4 - Register: Direct register addressing, e.g., r7 */
typedef enum {
 ADDR_NONE,
 ADDR_IMMEDIATE,
 ADDR_DIRECT,
 ADDR_INDEX,
 ADDR_REGISTER
} AddressingMethod;

/* Structure to hold information about every opcode */
typedef struct {
 const char *name;
 OpCode value;
} OpcodeInfo;

/* Initializes the encoded data structure.
* This function sets up an empty linked list for storing encoded directive words.
* It should be called at the beginning of the assembly process.
*/
void init_encoded_data(void);

/*
 * This function encodes a single assembly instruction into machine code.
 * It performs the following steps:
 * Extracts operands from the instruction string
 * Determines the addressing methods for source and destination operands
 * Validates the operands
 * Encodes the opcode and addressing methods into a machine word
 * Stores the encoded instruction in memory
 * Encodes the operands separately
 * The function uses global variables IC and DC to keep track of
 * instruction and data counters, respectively.
 */
void encode_instruction(const char *instruction, OpCode command_name);

/* This function encodes assembly directives (.data and .string) into machine code.
 * It handles two types of directives:
 * .data: Encodes a list of integer values
 * .string: Encodes a string as ASCII values, including a null terminator
 * The function parses the operands, converts them to appropriate machine words,
 * and stores them in the encoded data structure. It also performs error checking for invalid input.
 */
void encode_directive(const char* directive, const char* operands);

/* Frees the memory allocated for the encoded data structure.*/
void free_encoded_data(void);

/*
 * This function determines the addressing method of an operand based on its format.
 * It recognizes the following addressing methods:
 * - Immediate: Starts with '#'
 * - Index: Starts with '*'
 * - Register: Starts with 'r' followed by a digit (0-7)
 * - Direct: Any other valid operand
 * Returns:
 *   The determined AddressingMethod enum value
 */
AddressingMethod get_addressing_method(const char* operand);

/* AREType enum represents the ARE (Absolute, Relocatable, External) bits:
 * ARE_ABSOLUTE (4): The A bit (bit 2) is on, used for constants and instructions
 * ARE_RELOCATABLE (2): The R bit (bit 1) is on, used for relocatable addresses
 * ARE_EXTERNAL (1): The E bit (bit 0) is on, used for external references */
void set_ARE(MachineWord* word, AREType are);

/* Adds a new encoded word to the end of the encoded data list. */
void add_encoded_data_to_memory(void);

#endif
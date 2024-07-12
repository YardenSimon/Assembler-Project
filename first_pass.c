#include "first_pass.h"
#include "encoder.h"

void addSymbol(const char* name, int address) {
    /* Add a symbol to the symbol table if there is space */
    if (symbolCount < MAX_SYMBOLS) {
        strncpy(symbolTable[symbolCount].name, name, 30);
        symbolTable[symbolCount].name[30] = '\0';
        symbolTable[symbolCount].address = address;
        symbolCount++;
    } else {
        printf("Error: Symbol table full\n");
    }
}

int isLabel(const char* line) {
    char* colon;

    /* Check if the line contains a label */
    colon = strchr(line, ':');
    return colon != NULL && colon != line;
}

void getLabelName(const char* line, char* label) {
    /* Extract the label name from the line */
    sscanf(line, "%[^:]", label);
}

void firstPass(const char* filename) {
    FILE* file;
    char line[MAX_LINE_LENGTH];
    char label[31];
    char* instruction;

    /* Open the file for reading */
    file = fopen(filename, "r");
    if (file == NULL) {
        printf("Error opening file: %s\n", filename);
        return;
    }

    IC = STARTING_ADDRESS;
    DC = 0;

    /* Process each line in the file */
    while (fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\n")] = 0; /* Remove newline character */

        if (isLabel(line)) {
            getLabelName(line, label);
            addSymbol(label, IC);
            instruction = strchr(line, ':');
            if (instruction) {
                instruction++;
                while (isspace(*instruction)) instruction++;
                encodeInstruction(instruction);
            }
        } else {
            encodeInstruction(line);
        }
    }

    DC += IC - STARTING_ADDRESS;

    /* Close the file */
    fclose(file);
}

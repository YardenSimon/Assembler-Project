#include "first_pass.h"
#include "encoder.h"

void addSymbol(const char* name, int address) {
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
    char* colon = strchr(line, ':');
    return colon != NULL && colon != line;
}

void getLabelName(const char* line, char* label) {
    sscanf(line, "%[^:]", label);
}

void firstPass(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        printf("Error opening file: %s\n", filename);
        return;
    }

    char line[MAX_LINE_LENGTH];
    IC = STARTING_ADDRESS;
    DC = 0;

    while (fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\n")] = 0;

        if (isLabel(line)) {
            char label[31];
            getLabelName(line, label);
            addSymbol(label, IC);
            char* instruction = strchr(line, ':');
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

    fclose(file);
}
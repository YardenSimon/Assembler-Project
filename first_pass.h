#ifndef FIRST_PASS_H
#define FIRST_PASS_H

#include "assembler.h"

void addSymbol(const char* name, int address);
int isLabel(const char* line);
void getLabelName(const char* line, char* label);
void firstPass(const char* filename);

#endif // FIRST_PASS_H
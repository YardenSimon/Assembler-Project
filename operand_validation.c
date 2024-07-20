#include <ctype.h>
#include <string.h>

/* Assume this is declared in a header file */
extern int isLabel(const char* line);

int findCommand(const char* line) {
    int i, j;
    int cmdLen;

    /* Skip spaces at start */
    while (*line && isspace((unsigned char)*line)) {
        line++;
    }

    /* Check for and skip label if present */
    if (isLabel(line)) {
        /* Find the colon */
        for (i = 0; line[i] && line[i] != ':'; i++)
            ;
        if (line[i] == ':') {
            line += i + 1;  /* Skip past the label and colon */
            /* Skip spaces after the label */
            while (*line && isspace((unsigned char)*line)) {
                line++;
            }
        }
    }

    /* Check group1 */
    for (j = 0; j < group1_count; j++) {
        cmdLen = strlen(group1[j]);
        if (strncmp(line, group1[j], cmdLen) == 0 &&
            (line[cmdLen] == '\0' || isspace((unsigned char)line[cmdLen]))) {
            return (int)(line - strchr(line, line[0])); /* Return command position */
        }
    }

    /* Check group2 */
    for (j = 0; j < group2_count; j++) {
        cmdLen = strlen(group2[j]);
        if (strncmp(line, group2[j], cmdLen) == 0 &&
            (line[cmdLen] == '\0' || isspace((unsigned char)line[cmdLen]))) {
            return (int)(line - strchr(line, line[0])); /* Return command position */
        }
    }

    /* Check group3 */
    for (j = 0; j < group3_count; j++) {
        cmdLen = strlen(group3[j]);
        if (strncmp(line, group3[j], cmdLen) == 0 &&
            (line[cmdLen] == '\0' || isspace((unsigned char)line[cmdLen]))) {
            return (int)(line - strchr(line, line[0])); /* Return command position */
        }
    }

    return -1; /* No valid command found */
}
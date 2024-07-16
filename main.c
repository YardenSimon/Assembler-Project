#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "macros.h"


/* Function prototype for validate_assembly_file
void validate_assembly_file(const char *filename);*/

int main() {
    const char *input =
            "MAIN: add r3, LIST\n"
            "LOOP: prn #48\n"
            "      macr m_macr\n"
            "      cmp r3, #-6\n"
            "      bne END\n"
            "endmacr\n"
            "      lea STR, r6\n"
            "      inc r6\n"
            "      mov *r6, K\n"
            "      sub r1, r4\n"
            "      m_macr\n"
            "      dec K\n"
            "      jmp LOOP\n"
            "END:  stop\n"
            "STR:  .string \"abcd\"\n"
            "LIST: .data 6, -9\n"
            "      .data -100\n"
            "K:    .data 31\n";

    // Create temporary input file
    FILE *temp_input = fopen("temp_input.as", "w");
    if (temp_input == NULL) {
        perror("Failed to create temporary input file");
        return 1;
    }
    fputs(input, temp_input);
    fclose(temp_input);

    // Create output file
    const char *output_filename = "output.as";

    // Process macros (only once)
    printf("Processing macros:\n");
    replace_macros("temp_input.as", output_filename);

    // Print defined macros
    printf("Defined macros:\n");
    int i = 0;
    for(i = 0; i < macro_count; i++) {
        printf("Macro name: %s\n", macros[i].name);
        printf("Macro content:\n%s", macros[i].content);
        printf("--------------------\n");
    }

    // Print the contents of the output file
    FILE *output_file = fopen(output_filename, "r");
    if (output_file == NULL) {
        perror("Failed to open output file");
        return 1;
    }

    printf("Processed output:\n\n");
    char line[256];
    while (fgets(line, sizeof(line), output_file)) {
        printf("%s", line);
    }
    fclose(output_file);

    // Clean up
    remove("temp_input.as");

    return 0;
}
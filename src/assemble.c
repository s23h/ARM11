#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utilities.h"
#include "assemble_utils/symbol_table.h"
#include "assemble_utils/assemble_instruction.h"

#define MAX_TOKENS 6
#define MAXC 511

symbol_table *opcodes;

// Breaks down a line into its constituent tokens -- e.g. mov r1, r2 would be turned into an string array containing
// mov, r1, r2 as its elements
char **tokenize(char *line, int *num_tokens) {
    char delim[] = " ,:\n";
    char **token_array = malloc(MAX_TOKENS * sizeof(char*));
    for (int i = 0; i < MAX_TOKENS; i++) {
        token_array[i] = NULL;
    }

    char *token;

    int i = 0;
    token = strtok(line, delim);

    while (token != NULL) {
        token_array[i] = token;
        i++;
        token = strtok(NULL, delim);
    }
    *num_tokens = i;

    return token_array;
}

void init_opcodes(symbol_table *opcodes) {
    init_table(opcodes);

    insert_back(opcodes, "and", 0);
    insert_back(opcodes, "eor", 1);
    insert_back(opcodes, "sub", 2);
    insert_back(opcodes, "rsb", 3);
    insert_back(opcodes, "add", 4);
    insert_back(opcodes, "orr", 12);
    insert_back(opcodes, "mov", 13);
    insert_back(opcodes, "tst", 8);
    insert_back(opcodes, "teq", 9);
    insert_back(opcodes, "cmp", 10);

    insert_back(opcodes, "beq", 0);
    insert_back(opcodes, "bne", 1);
    insert_back(opcodes, "bge", 10);
    insert_back(opcodes, "blt", 11);
    insert_back(opcodes, "bgt", 12);
    insert_back(opcodes, "ble", 13);
    insert_back(opcodes, "bal", 14);
    insert_back(opcodes, "b", 14);
}

int main(int argc, char **argv) {
    // Records the number of data items written to the end of the assembled instruction file by ldr/str.
    opcodes = malloc(sizeof(symbol_table));
    init_opcodes(opcodes);
    symbol_table labels_addresses;
    init_table(&labels_addresses);

    // The first pass over the source file that builds up the symbol table.
    FILE *first_pass = fopen(argv[1], "r");
    int32_t address_counter = 0;
    if (first_pass != NULL)
    {
        char line[MAXC + 1]; // +1 for the null character at the end
        while (fgets(line, sizeof(line), first_pass) != NULL) /* read a line */
        {
            if (line[0] == '\n') {
              continue;
            }
            int temp;
            char **tokens = tokenize(line, &temp);
            if (get_instruction_type(tokens) == LABEL) {
                insert_back(&labels_addresses, tokens[0], address_counter);
            }
            else {
                address_counter += 4;
            }

            free(tokens);

        }
        fclose(first_pass);

    }
    else
    {
        perror(""); /* why didn't the file open? */
    }
    int32_t end_address = address_counter;

    // The second pass over the source file, outputs the binary instructions.
    FILE *input = fopen(argv[1], "r");
    FILE *output = fopen(argv[2], "wb");

    if (input != NULL && output != NULL)
    {
        address_counter = 0;
        char line[MAXC + 1]; // +1 for the null character at the end
        while (fgets(line, sizeof(line), input) != NULL)
        {
            if (line[0] == '\n') {
              continue;
            }
            int num_tokens = 0;
            char **tokens = tokenize(line, &num_tokens);
            instruction_type type = get_instruction_type(tokens);
            int32_t instruction = 0;
            switch (type) {
                case DATA_PROCESSING: instruction = assemble_dp(tokens);
                break;
                case MULTIPLY: instruction = assemble_mult(tokens);
                break;
                case BRANCH: instruction = assemble_branch(tokens, address_counter, &labels_addresses);
                break;
                case DATA_TRANSFER: instruction = assemble_dt(tokens, address_counter, input, output, num_tokens,
                                                              end_address);
                break;
                case SPECIAL: instruction = assemble_special(tokens);
                break;
                default: continue;
                break;
            }
            fwrite(&instruction, sizeof(int32_t), 1, output);
            address_counter += 4;
            free(tokens);
        }
        fclose(input);
        fclose(output);
    }
    else
    {
        perror("");
    }

    destroy_table(&labels_addresses);
    destroy_table(opcodes);
    free(opcodes);
}

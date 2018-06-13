#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utilities.h"
#include "assemble_utils/symbolTable.h"
#include "assemble_utils/assembleInstruction.h"

#define MAX_TOKENS 6
#define MAXC 511

symbol_Table* opcodes;

// Breaks down a line into its constituent tokens -- e.g. mov r1, r2 would be turned into an string array containing
// mov, r1, r2 as its elements
char** tokenize(char* line, int* numTokens) {
    char delim[] = " ,:\n";
    char** tokenArray = malloc(MAX_TOKENS * sizeof(char*));
    for (int i = 0; i < MAX_TOKENS; i++) {
        tokenArray[i] = NULL;
    }

    char *token;

    int i = 0;
    token = strtok(line, delim);

    while (token != NULL) {
        tokenArray[i] = token;
        i++;
        token = strtok(NULL, delim);
    }
    *numTokens = i;

    return tokenArray;
}

void init_Opcodes(symbol_Table* opcodes) {
  initTable(opcodes);

  insert_Back(opcodes, "and", 0);
  insert_Back(opcodes, "eor", 1);
  insert_Back(opcodes, "sub", 2);
  insert_Back(opcodes, "rsb", 3);
  insert_Back(opcodes, "add", 4);
  insert_Back(opcodes, "orr", 12);
  insert_Back(opcodes, "mov", 13);
  insert_Back(opcodes, "tst", 8);
  insert_Back(opcodes, "teq", 9);
  insert_Back(opcodes, "cmp", 10);

  insert_Back(opcodes, "beq", 0);
  insert_Back(opcodes, "bne", 1);
  insert_Back(opcodes, "bge", 10);
  insert_Back(opcodes, "blt", 11);
  insert_Back(opcodes, "bgt", 12);
  insert_Back(opcodes, "ble", 13);
  insert_Back(opcodes, "bal", 14);
  insert_Back(opcodes, "b", 14);
}

int main(int argc, char** argv) {
    // Records the number of data items written to the end of the assembled instruction file by ldr/str.
    opcodes = malloc(sizeof(symbol_Table));
    init_Opcodes(opcodes);
    symbol_Table labels_Addresses;
    initTable(&labels_Addresses);

    // The first pass over the source file that builds up the symbol table.
    FILE *first_Pass = fopen(argv[1], "r");
    int32_t address_Counter = 0;
    if (first_Pass != NULL)
    {
        char line[MAXC + 1]; // +1 for the null character at the end
        while (fgets(line, sizeof(line), first_Pass) != NULL) /* read a line */
        {
            if (line[0] == '\n') {
              continue;
            }
            int temp;
            char** tokens = tokenize(line, &temp);
            if (get_Instruction_Type(tokens) == LABEL) {
                insert_Back(&labels_Addresses, tokens[0], address_Counter);
            }
            else {
                address_Counter += 4;
            }

            free(tokens);

        }
        fclose(first_Pass);

    }
    else
    {
        perror(""); /* why didn't the file open? */
    }
    int32_t end_Address = address_Counter;

    // The second pass over the source file, outputs the binary instructions.
    FILE *input = fopen(argv[1], "r");
    FILE *output = fopen(argv[2], "wb");

    if (input != NULL && output != NULL)
    {
        address_Counter = 0;
        char line[MAXC + 1]; // +1 for the null character at the end
        while (fgets(line, sizeof(line), input) != NULL)
        {
            if (line[0] == '\n') {
              continue;
            }
            int num_Tokens = 0;
            char** tokens = tokenize(line, &num_Tokens);
            instruction_Type type = get_Instruction_Type(tokens);
            int32_t instruction = 0;
            switch (type) {
                case DATA_PROCESSING: instruction = assemble_DP(tokens);
                break;
                case MULTIPLY: instruction = assemble_Mult(tokens);
                break;
                case BRANCH: instruction = assemble_Branch(tokens, address_Counter, &labels_Addresses);
                break;
                case DATA_TRANSFER: instruction = assemble_DT(tokens, address_Counter, input, output, num_Tokens, end_Address);
                break;
                case SPECIAL: instruction = assemble_Special(tokens);
                break;
                default: continue;
                break;
            }
            fwrite(&instruction, sizeof(int32_t), 1, output);
            address_Counter += 4;
            free(tokens);
        }
        fclose(input);
        fclose(output);
    }
    else
    {
        perror("");
    }

    destroy_Table(&labels_Addresses);
    destroy_Table(opcodes);
    free(opcodes);
}

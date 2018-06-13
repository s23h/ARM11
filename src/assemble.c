#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utilities.h"
#include "symbolTable.h"
#include "assembleInstruction.h"

#define MAX_TOKENS 6
#define MAXC 511

static symbolTable opcodes;

// Records the number of data items written to the end of the assembled instruction file by ldr/str.
static int numDataWrites = 0;

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

int main(int argc, char** argv) {
    initTable(&opcodes);

    insertBack(&opcodes, "and", 0);
    insertBack(&opcodes, "eor", 1);
    insertBack(&opcodes, "sub", 2);
    insertBack(&opcodes, "rsb", 3);
    insertBack(&opcodes, "add", 4);
    insertBack(&opcodes, "orr", 12);
    insertBack(&opcodes, "mov", 13);
    insertBack(&opcodes, "tst", 8);
    insertBack(&opcodes, "teq", 9);
    insertBack(&opcodes, "cmp", 10);

    insertBack(&opcodes, "beq", 0);
    insertBack(&opcodes, "bne", 1);
    insertBack(&opcodes, "bge", 10);
    insertBack(&opcodes, "blt", 11);
    insertBack(&opcodes, "bgt", 12);
    insertBack(&opcodes, "ble", 13);
    insertBack(&opcodes, "bal", 14);
    insertBack(&opcodes, "b", 14);

    /*
    for (tableIter iter = tableBegin(&opcodes); iter != tableEnd(&opcodes); iter = iter -> next) {
        printf("%u\n", tableLookup(&opcodes, iter -> key));
    }*/

    /*char instruction[] = "mul r1, r12, r3";
    int32_t ins = assembleMult(tokenize(instruction));
    printf("%d", ins); */

    symbolTable labelsAddresses;
    initTable(&labelsAddresses);

    // The first pass over the source file that builds up the symbol table.
    FILE *firstPass = fopen(argv[1], "r");
    int32_t addressCounter = 0;
    if (firstPass != NULL)
    {
        char line[MAXC + 1]; // +1 for the null character at the end
        while (fgets(line, sizeof(line), firstPass) != NULL) /* read a line */
        {
            if (line[0] == '\n') {
              continue;
            }
            int temp;
            char** tokens = tokenize(line, &temp);
            if (getInstructionType(tokens) == LABEL) {
                insertBack(&labelsAddresses, tokens[0], addressCounter);
            }
            else {
                addressCounter += 4;
            }

        }
        fclose(firstPass);
    }
    else
    {
        perror(""); /* why didn't the file open? */
    }
    int32_t endAddress = addressCounter;

    /*
    for (tableIter iter = tableBegin(&labelsAddresses); iter != tableEnd(&labelsAddresses); iter = iter -> next) {
        printf("%s %u\n", iter -> key, tableLookup(&labelsAddresses, iter -> key));
    }*/

    // The second pass over the source file, outputs the binary instructions.
    FILE *input = fopen(argv[1], "r");
    FILE *output = fopen(argv[2], "wb");

    if (input != NULL && output != NULL)
    {
        addressCounter = 0;
        char line[MAXC + 1]; // +1 for the null character at the end
        while (fgets(line, sizeof(line), input) != NULL)
        {
            if (line[0] == '\n') {
              continue;
            }
            int numTokens = 0;
            char** tokens = tokenize(line, &numTokens);
            instructionType type = getInstructionType(tokens);
            int32_t instruction = 0;
            switch (type) {
                case DATA_PROCESSING: instruction = assembleDP(tokens); break;
                case MULTIPLY: instruction = assembleMult(tokens); break;
                case BRANCH: instruction = assembleBranch(tokens, addressCounter, &labelsAddresses); break;
                case DATA_TRANSFER: instruction = assembleDT(tokens, addressCounter, input, output, numTokens, endAddress); break;
                case SPECIAL: instruction = assembleSpecial(tokens); break;
                default: continue; break;
            }
            fwrite(&instruction, sizeof(int32_t), 1, output);
            addressCounter += 4;
        }
        fclose(input);
        fclose(output);
    }
    else
    {
        perror("");
    }

    destroyTable(&labelsAddresses);
    destroyTable(&opcodes);
}

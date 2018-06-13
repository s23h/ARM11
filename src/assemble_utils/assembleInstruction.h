#ifndef ASSEMBLE_INSTRUCTION_H
#define ASSEMBLE_INSTRUCTION_H

#include <stdint.h>

static symbolTable opcodes;

// Records the number of data items written to the end of the assembled instruction file by ldr/str.
static int numDataWrites = 0;

typedef enum { DATA_PROCESSING, MULTIPLY, DATA_TRANSFER, BRANCH, LABEL, SPECIAL } instructionType;

int32_t assembleDP(char** tokens);

int32_t assembleMult(char** tokens);

int32_t assembleBranch(char** tokens, int32_t currentAddress, symbolTable* labelsMap);

int32_t assembleDT(char** tokens, int32_t currentAddress, FILE* input, FILE* output, int numTokens, int32_t endAddress);

int32_t assembleSpecial(char** tokens);

instructionType getInstructionType(char** tokens);

#endif

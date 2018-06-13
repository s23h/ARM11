#ifndef ASSEMBLE_INSTRUCTION_H
#define ASSEMBLE_INSTRUCTION_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

static int numDataWrites = 0;
extern symbolTable* opcodes;

typedef enum { DATA_PROCESSING, MULTIPLY, DATA_TRANSFER, BRANCH, LABEL, SPECIAL } instructionType;

instructionType getInstructionType(char** tokens);

int32_t assembleDP(char** tokens);

int32_t assembleMult(char** tokens);

int32_t assembleBranch(char** tokens, int32_t currentAddress, symbolTable* labelsMap);

int32_t assembleDT(char** tokens, int32_t currentAddress, FILE* input, FILE* output, int numTokens, int32_t endAddress);

int32_t assembleSpecial(char** tokens);

#endif

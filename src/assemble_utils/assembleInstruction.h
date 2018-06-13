#ifndef ASSEMBLE_INSTRUCTION_H
#define ASSEMBLE_INSTRUCTION_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

static int num_Data_Writes = 0;
extern symbol_Table* opcodes;

typedef enum { DATA_PROCESSING, MULTIPLY, DATA_TRANSFER, BRANCH, LABEL, SPECIAL } instruction_Type;

instruction_Type get_Instruction_Type(char** tokens);

int32_t assemble_DP(char** tokens);

int32_t assemble_Mult(char** tokens);

int32_t assemble_Branch(char** tokens, int32_t current_Address, symbol_Table* labels_Map);

int32_t assemble_DT(char** tokens, int32_t current_Address, FILE* input, FILE* output, int num_Tokens, int32_t end_Address);

int32_t assemble_Special(char** tokens);

#endif

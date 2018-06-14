#ifndef ASSEMBLE_INSTRUCTION_H
#define ASSEMBLE_INSTRUCTION_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

extern symbol_table* opcodes;

typedef enum { DATA_PROCESSING, MULTIPLY, DATA_TRANSFER, BRANCH, LABEL, SPECIAL } instruction_type;

instruction_type get_instruction_type(char **tokens);

int32_t assemble_dp(char **tokens);

int32_t assemble_mult(char **tokens);

int32_t assemble_branch(char **tokens, int32_t currentddress, symbol_table *labels_map);

int32_t assemble_dt(char **tokens, int32_t current_address, FILE *input, FILE *output, int num_tokens,
                    int32_t end_address);

int32_t assemble_special(char **tokens);

#endif

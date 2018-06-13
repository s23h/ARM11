#ifndef EXECUTE_INSTRUCTION_H
#define EXECUTE_INSTRUCTION_H

#include <stdint.h>
#include "decodedInstruction.h"

#define RAM_SIZE 65536

uint8_t check_condition(uint8_t cond, int32_t *registers);

void execute_dp(decoded_instruction decoded, int32_t *registers);

int execute_branch(decoded_instruction decoded, int32_t *registers);

void execute_multiply(decoded_instruction decoded, int32_t *registers);

void execute_dt(decoded_instruction decoded, int32_t *registers, uint8_t *mainMemory);

#endif

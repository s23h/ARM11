#ifndef EXECUTE_INSTRUCTION_H
#define EXECUTE_INSTRUCTION_H

#include <stdint.h>
#include "decodedInstruction.h"

#define RAM_SIZE 65536

uint8_t check_Condition(uint8_t cond, int32_t* registers);

void execute_DP(decoded_Instruction decoded, int32_t* registers);

int execute_Branch(decoded_Instruction decoded, int32_t* registers);

void execute_Multiply(decoded_Instruction decoded, int32_t* registers);

void execute_DT(decoded_Instruction decoded, int32_t* registers, uint8_t* mainMemory);

#endif

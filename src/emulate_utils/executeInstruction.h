#ifndef EXECUTE_INSTRUCTION_H
#define EXECUTE_INSTRUCTION_H

#include <stdint.h>
#include "decodedInstruction.h"

#define RAM_SIZE 65536

uint8_t checkCondition(uint8_t cond, int32_t* registers);

void executeDP(decodedInstruction decoded, int32_t* registers);

int executeBranch(decodedInstruction decoded, int32_t* registers);

void executeMultiply(decodedInstruction decoded, int32_t* registers);

void executeDT(decodedInstruction decoded, int32_t* registers, uint8_t* mainMemory);

#endif

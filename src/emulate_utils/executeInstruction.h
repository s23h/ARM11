#ifndef EXECUTE_INSTRUCTION_H
#define EXECUTE_INSTRUCTION_H

#include <stdint.h>

int8_t checkCondition(uint8_t cond, int32_t* registers);

void executeDP(decodedInstruction decoded, int32_t* registers);

void executeBranch(decodedInstruction decoded, int32_t* registers);

void executeMultiply(decodedInstruction decoded, int32_t* registers);

void executeDT(decodedInstruction decoded, int32_t* registers);

#endif

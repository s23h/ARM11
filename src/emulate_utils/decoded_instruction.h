#ifndef DECODED_INSTRUCTION_H
#define DECODED_INSTRUCTION_H

#include <stdint.h>

// Represents a decoded instructions with all its components.
// Since this struct is shared between all 4 instruction types, there
// will be unused fields in the struct.

// To represent the 4 different types of instructions.
typedef enum { DATA_PROCESSING, MULTIPLY, DATA_TRANSFER, BRANCH } instruction_type;

typedef struct {
    instruction_type type;
    uint8_t cond;
    uint8_t i;
    uint8_t opcode;
    uint8_t s;
    uint8_t rn;
    uint8_t rd;
    uint16_t operand2;
    uint8_t a;
    uint8_t rs;
    uint8_t rm;
    uint8_t p;
    uint8_t u;
    uint8_t l;
    uint8_t rest;
    uint32_t offset;
} decoded_instruction;

instruction_type get_instruction_type(int32_t instruction);

decoded_instruction decode_dp(int32_t instruction);

decoded_instruction decode_branch(int32_t instruction);

decoded_instruction decode_multiply(uint32_t instruction);

decoded_instruction decode_dt(int32_t instruction);

#endif

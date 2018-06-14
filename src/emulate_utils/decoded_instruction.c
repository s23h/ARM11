#include <stdint.h>

#include "../utilities.h"
#include "decoded_instruction.h"

// Checks the bit pattern of the instruction to determine its type.
instruction_type get_instruction_type(int32_t instruction) {
    if (CHECK_BIT(instruction, 27) && !CHECK_BIT(instruction, 26) && CHECK_BIT(instruction, 25) && !CHECK_BIT(instruction, 24)) {
        return BRANCH;
    }
    else if (!CHECK_BIT(instruction, 27) && CHECK_BIT(instruction, 26) && !CHECK_BIT(instruction, 22) && !CHECK_BIT(instruction, 21)) {
        return DATA_TRANSFER;
    }
    else if (!CHECK_BIT(instruction, 27) && !CHECK_BIT(instruction, 26) && !CHECK_BIT(instruction, 25) && !CHECK_BIT(instruction, 24)
        && !CHECK_BIT(instruction, 23) && !CHECK_BIT(instruction, 22)
        && CHECK_BIT(instruction, 7) && !CHECK_BIT(instruction, 6) && !CHECK_BIT(instruction, 5) && CHECK_BIT(instruction, 4)) {
        return MULTIPLY;
    }
    else {
        return DATA_PROCESSING;
    }
}

//=========================================================================
// Returns the struct containing the decoded components of the provided Data Processing instruction.
decoded_instruction decode_dp(int32_t instruction) {
    decoded_instruction decoded;
    decoded.type = DATA_PROCESSING;

    uint32_t result = extract_bits(instruction, 4, 29);
    decoded.cond = ((uint8_t*)(&result))[0];

    result = extract_bits(instruction, 1, 26);
    decoded.i = ((uint8_t*)(&result))[0];

    result = extract_bits(instruction, 4, 22);
    decoded.opcode = ((uint8_t*)(&result))[0];

    result = extract_bits(instruction, 1, 21);
    decoded.s = ((uint8_t*)(&result))[0];

    result = extract_bits(instruction, 4, 17);
    decoded.rn = ((uint8_t*)(&result))[0];

    result = extract_bits(instruction, 4, 13);
    decoded.rd = ((uint8_t*)(&result))[0];

    result = extract_bits(instruction, 12, 1);
    decoded.operand2 = ((uint16_t*)(&result))[0];

    return decoded;
}

//=========================================================================
// Decodes the provided Branch instruction and returns the struct representing it
decoded_instruction decode_branch(int32_t instruction) {
    decoded_instruction decoded;
    decoded.type = BRANCH;
    uint32_t result = extract_bits(instruction, 4, 29);
    decoded.cond = ((uint8_t*)(&result))[0];
    decoded.offset = extract_bits(instruction, 24, 1);

    return decoded;
}

//=========================================================================
// Decodes a Multiply instruction.
decoded_instruction decode_multiply(uint32_t instruction) {
    decoded_instruction decoded;
    decoded.type = MULTIPLY;

    uint32_t result = 0;
    result = extract_bits(instruction, 4, 29);
    decoded.cond = ((uint8_t*)(&result))[0];

    result = extract_bits(instruction, 1, 22);
    decoded.a = ((uint8_t*)(&result))[0];

    result = extract_bits(instruction, 1, 21);
    decoded.s = ((uint8_t*)(&result))[0];

    result = extract_bits(instruction, 4, 17);
    decoded.rd = ((uint8_t*)(&result))[0];

    result = extract_bits(instruction, 4, 13);
    decoded.rn = ((uint8_t*)(&result))[0];

    result = extract_bits(instruction, 4, 9);
    decoded.rs = ((uint8_t*)(&result))[0];

    result = extract_bits(instruction, 4, 1);
    decoded.rm = ((uint8_t*)(&result))[0];

    return decoded;
}


//=========================================================================
// Decodes Single Data Transfer instructions
decoded_instruction decode_dt(int32_t instruction) {
    decoded_instruction decoded;
    decoded.type = DATA_TRANSFER;

    uint32_t result = 0;
    result = extract_bits(instruction, 4, 29);
    decoded.cond = ((uint8_t*)(&result))[0];

    result = extract_bits(instruction, 1, 26);
    decoded.i = ((uint8_t*)(&result))[0];

    result = extract_bits(instruction, 1, 25);
    decoded.p = ((uint8_t*)(&result))[0];

    result = extract_bits(instruction, 1, 24);
    decoded.u = ((uint8_t*)(&result))[0];

    result = extract_bits(instruction, 1, 21);
    decoded.l = ((uint8_t*)(&result))[0];

    result = extract_bits(instruction, 4, 17);
    decoded.rn = ((uint8_t*)(&result))[0];

    result = extract_bits(instruction, 4, 13);
    decoded.rd = ((uint8_t*)(&result))[0];

    decoded.offset = extract_bits(instruction, 12, 1);

    return decoded;
}

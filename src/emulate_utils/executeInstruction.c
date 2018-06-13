#include <stdlib.h>
#include "executeInstruction.h"

// Checks the cond code to determine whether the instruction should execute.
uint8_t checkCondition(uint8_t cond, int32_t* registers) {
    if (cond == 14) {
        return 1;
    }
    else if (cond == 0) {
        return CHECK_BIT(registers[16], 30);
    }
    else if (cond == 1) {
        return !CHECK_BIT(registers[16], 30);
    }
    else if (cond == 10) {
        return (CHECK_BIT(registers[16], 31) && CHECK_BIT(registers[16], 28))
                            || (!CHECK_BIT(registers[16], 31) && !CHECK_BIT(registers[16], 28));
    }
    else if (cond == 11) {
        return (CHECK_BIT(registers[16], 31) && !CHECK_BIT(registers[16], 28))
                            || (!CHECK_BIT(registers[16], 31) && CHECK_BIT(registers[16], 28));

    }
    else if (cond == 12) {
        return !CHECK_BIT(registers[16], 30) && ((CHECK_BIT(registers[16], 31) && CHECK_BIT(registers[16], 28))
                            || (!CHECK_BIT(registers[16], 31) && !CHECK_BIT(registers[16], 28)));
    }
    else if (cond == 13) {
        return CHECK_BIT(registers[16], 30) || ((CHECK_BIT(registers[16], 31) && !CHECK_BIT(registers[16], 28))
                            || (!CHECK_BIT(registers[16], 31) && CHECK_BIT(registers[16], 28)));
    }

    // should be unreachable
    return 0;
}


// Executes a decoded Data Processing instruction
void executeDP(decodedInstruction decoded, int32_t* registers) {
    if (!checkCondition(decoded.cond, registers)) {
        return;
    }

    // Interpreting operand2
    int32_t op2 = 0;
    uint8_t carry = 0;
    int shiftApplied = 0;

    // For the immediate constant case
    if (decoded.i == 1) {
        uint8_t imm = ((uint8_t*)&(decoded.operand2))[0];
        uint32_t immExtend = imm;
        uint8_t rotate = ((uint8_t*)&(decoded.operand2))[1];
        rotate *= 2;
        op2 = rightRotate(immExtend, rotate);
    }
    else {
        // To extract the last 4 bits
        uint8_t rm = extractBits(decoded.operand2, 4, 1);
        uint32_t val = registers[rm];
        // To extract the 5-bit shift value
        uint8_t shiftVal = extractBits(decoded.operand2, 5, 8);
        if (shiftVal > 0) {
            shiftApplied = 1;
        }
        // To extract the 2 bit shift type
        uint8_t shiftType = extractBits(decoded.operand2, 2, 6);

        //Logical left lsl
        if (shiftType == 0) {
            carry = CHECK_BIT(val, 32 - shiftVal);
            op2 = val << shiftVal;
        }
        // Logical right lsr
        else if (shiftType == 1) {
            carry = CHECK_BIT(val, shiftVal - 1);
            op2 = val >> shiftVal;
        }
        // Arithmetic right
        else if (shiftType == 2) {
            carry = CHECK_BIT(val, shiftVal - 1);
            val = val >> shiftVal;
            if (CHECK_BIT(val, 31) == 1) {
                uint32_t mask = intPow(2, 32 - shiftVal) * (intPow(2, shiftVal) - 1);
                op2 = val | mask;
            }
            else {
                op2 = val;
            }
        }
        // Right rotate
        else if (shiftType == 3) {
            carry = CHECK_BIT(val, shiftVal - 1);
            op2 = rightRotate(val, shiftVal);
        }
    }

    // ************************************************


    // Executing instruction based on the opcode

    int32_t result;
    uint32_t unsignedResult;

    // Calculates the result of the instruction
    // The unsignedResult variable is for checking unsigned overflows and underflows
    switch (decoded.opcode) {
        case 0: result = registers[decoded.rn] & op2; break;
        case 1: result = registers[decoded.rn] ^ op2; break;
        case 2: result = registers[decoded.rn] - op2;
          unsignedResult = (uint32_t)registers[decoded.rn] - (uint32_t)op2; break;
        case 3: result = op2 - registers[decoded.rn];
          unsignedResult = (uint32_t)op2 - (uint32_t)registers[decoded.rn]; break;
        case 4: result = registers[decoded.rn] + op2;
          unsignedResult = (uint32_t)registers[decoded.rn] + (uint32_t)op2; break;
        case 8: result = registers[decoded.rn] & op2; break;
        case 9: result = registers[decoded.rn] ^ op2; break;
        case 10: result = registers[decoded.rn] - op2; unsignedResult = (uint32_t)registers[decoded.rn] - (uint32_t)op2; break;
        case 12: result = registers[decoded.rn] | op2; break;
        case 13: result = op2; break;
    }

    // Write to register
    if (decoded.opcode != 8 && decoded.opcode != 9 && decoded.opcode != 10) {
        registers[decoded.rd] = result;
    }

    // If S bit is set
    if (decoded.s == 1) {
        // Set C bit for CSPR

        // For the logic operations
        if (shiftApplied && (decoded.opcode == 0 || decoded.opcode == 1 || decoded.opcode == 12 || decoded.opcode == 9
        || decoded.opcode == 8 || decoded.opcode == 13)) {
            setBit32(&(registers[16]), 29, carry);
        }
        // addition
        else if (decoded.opcode == 4) {
            if (unsignedResult < registers[decoded.rn]) { // overflow test
                setBit32(&(registers[16]), 29, 1);
            }
            else {
                setBit32(&(registers[16]), 29, 0);
            }
        }
        else if (decoded.opcode == 2 || decoded.opcode == 10) {
            if (unsignedResult > registers[decoded.rn]) { // underflow test
                setBit32(&(registers[16]), 29, 0);
            }
            else {
                setBit32(&(registers[16]), 29, 1);
            }
        }
        else if (decoded.opcode == 3) {
            if (unsignedResult > op2) {
                setBit32(&(registers[16]), 29, 0);
            }
            else {
                setBit32(&(registers[16]), 29, 1);
            }
        }
        // *********************************************
        // Set the Z bit
        if (result == 0) {
            setBit32(&(registers[16]), 30, 1);
        }
        else {
            setBit32(&(registers[16]), 30, 0);
        }

        // Set the N bit
        setBit32(&(registers[16]), 31, CHECK_BIT(result, 31));
    }
}

int executeBranch(decodedInstruction decoded, int32_t* registers) {
    if(!checkCondition(decoded.cond, registers)) {
        return 0;
    }
    uint32_t shifted = extractBits(decoded.offset << 2, 24, 1);
    int32_t offset = sign_extend_24_32(shifted);
    registers[15] += offset;
    return 1;
}

// Executes a decoded Multiply instruction.
void executeMultiply(decodedInstruction decoded, int32_t* registers) {
    if (!checkCondition(decoded.cond, registers)) {
        return;
    }

    int64_t result = registers[decoded.rm] * registers[decoded.rs];
    int32_t truncated = ((int32_t*)(&result))[0];
    if (decoded.a) {
        truncated += registers[decoded.rn];
    }

    registers[decoded.rd] = truncated;

    if (decoded.s) {
        setBit32(&(registers[16]), 31, CHECK_BIT(truncated, 31));
        if (truncated == 0) {
            setBit32(&(registers[16]), 30, 1);
        }
        else {
            setBit32(&(registers[16]), 30, 0);
        }
    }
}

// Executes Single Data Transfer instructions
void executeDT(decodedInstruction decoded, int32_t* registers, uint8_t* mainMemory) {
    if (!checkCondition(decoded.cond, registers)) {
        return;
    }

    uint32_t offset = 0;
    if (decoded.i) {
        // To extract the last 4 bits
        uint8_t rm = extractBits(decoded.offset, 4, 1);
        uint32_t val = registers[rm];
        // To extract the 5-bit shift value
        uint8_t shiftVal = extractBits(decoded.offset, 5, 8);
        // To extract the 2 bit shift type
        uint8_t shiftType = extractBits(decoded.offset, 2, 6);

        //Logical left lsl
        if (shiftType == 0) {
            offset = val << shiftVal;
        }
        // Logical right lsr
        else if (shiftType == 1) {
            offset = val >> shiftVal;
        }
        // Arithmetic right
        else if (shiftType == 2) {
            val = val >> shiftVal;
            if (CHECK_BIT(val, 31) == 1) {
                uint32_t mask = intPow(2, 32 - shiftVal) * (intPow(2, shiftVal) - 1);
                offset = val | mask;
            }
            else {
                offset = val;
            }
        }
        // Right rotate
        else if (shiftType == 3) {
            offset = rightRotate(val, shiftVal);
        }
    }
    else {
        offset = decoded.offset;
    }

    // Calculates the address to read/load from
    int32_t addressToEffect;
    if (decoded.p && decoded.u) {
        addressToEffect = registers[decoded.rn] + offset;
    }
    else if (decoded.p && !decoded.u) {
        addressToEffect = registers[decoded.rn] - offset;
    }
    else {
        addressToEffect = registers[decoded.rn];
        if (decoded.u) {
            registers[decoded.rn] += offset;
        }
        else {
            registers[decoded.rn] -= offset;
        }
    }

    if (decoded.l) {
      switch (addressToEffect) {
        case 0x20200000:
        case 0x20200004:
        case 0x20200008:
        case 0x20200028:
        case 0x2020001c: registers[decoded.rd] = addressToEffect; break;
        default: break;
      }
    }


    if (addressToEffect == 0x20200000) {
      printf("One GPIO pin from 0 to 9 has been accessed\n");
      return;
    }
    else if (addressToEffect == 0x20200004) {
      printf("One GPIO pin from 10 to 19 has been accessed\n");
      return;
    }
    else if (addressToEffect == 0x20200008) {
      printf("One GPIO pin from 20 to 29 has been accessed\n");
      return;
    }
    else if (addressToEffect == 0x20200028) {
      printf("PIN OFF\n");
      return;
    }
    else if (addressToEffect == 0x2020001c) {
      printf("PIN ON\n");
      return;
    }
    else if (addressToEffect >= RAM_SIZE) {
      printf("Error: Out of bounds memory access at address 0x%08x\n", addressToEffect);
      return;
    }

    // Read/load from memory
    if (decoded.l) {
        registers[decoded.rd] = ((int32_t*)(&(mainMemory[addressToEffect])))[0];
    }
    else {
        ((int32_t*)(&(mainMemory[addressToEffect])))[0] = registers[decoded.rd];
    }
}

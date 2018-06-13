#include <stdlib.h>
#include <printf.h>

#include "../utilities.h"
#include "executeInstruction.h"
#include "decodedInstruction.h"

// Checks the cond code to determine whether the instruction should execute.
uint8_t check_condition(uint8_t cond, int32_t *registers) {
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
void execute_dp(decoded_instruction decoded, int32_t *registers) {
    if (!check_condition(decoded.cond, registers)) {
        return;
    }

    // Interpreting operand2
    int32_t op2 = 0;
    uint8_t carry = 0;
    int shift_applied = 0;

    // For the immediate constant case
    if (decoded.i == 1) {
        uint8_t imm = ((uint8_t*)&(decoded.operand2))[0];
        uint32_t imm_extend = imm;
        uint8_t rotate = ((uint8_t*)&(decoded.operand2))[1];
        rotate *= 2;
        op2 = right_rotate(imm_extend, rotate);
    }
    else {
        // To extract the last 4 bits
        uint8_t rm = extract_bits(decoded.operand2, 4, 1);
        uint32_t val = registers[rm];
        // To extract the 5-bit shift value
        uint8_t shift_val = extract_bits(decoded.operand2, 5, 8);
        if (shift_val > 0) {
            shift_applied = 1;
        }
        // To extract the 2 bit shift type
        uint8_t shift_type = extract_bits(decoded.operand2, 2, 6);

        //Logical left lsl
        if (shift_type == 0) {
            carry = CHECK_BIT(val, 32 - shift_val);
            op2 = val << shift_val;
        }
        // Logical right lsr
        else if (shift_type == 1) {
            carry = CHECK_BIT(val, shift_val - 1);
            op2 = val >> shift_val;
        }
        // Arithmetic right
        else if (shift_type == 2) {
            carry = CHECK_BIT(val, shift_val - 1);
            val = val >> shift_val;
            if (CHECK_BIT(val, 31) == 1) {
                uint32_t mask = int_pow(2, 32 - shift_val) * (int_pow(2, shift_val) - 1);
                op2 = val | mask;
            }
            else {
                op2 = val;
            }
        }
        // Right rotate
        else if (shift_type == 3) {
            carry = CHECK_BIT(val, shift_val - 1);
            op2 = right_rotate(val, shift_val);
        }
    }

    // ************************************************


    // Executing instruction based on the opcode

    int32_t result;
    uint32_t unsigned_result;

    // Calculates the result of the instruction
    // The unsignedResult variable is for checking unsigned overflows and underflows
    switch (decoded.opcode) {
        case 0: result = registers[decoded.rn] & op2; break;
        case 1: result = registers[decoded.rn] ^ op2; break;
        case 2: result = registers[decoded.rn] - op2;
          unsigned_result = (uint32_t)registers[decoded.rn] - (uint32_t)op2; break;
        case 3: result = op2 - registers[decoded.rn];
          unsigned_result = (uint32_t)op2 - (uint32_t)registers[decoded.rn]; break;
        case 4: result = registers[decoded.rn] + op2;
          unsigned_result = (uint32_t)registers[decoded.rn] + (uint32_t)op2; break;
        case 8: result = registers[decoded.rn] & op2; break;
        case 9: result = registers[decoded.rn] ^ op2; break;
        case 10: result = registers[decoded.rn] - op2; unsigned_result = (uint32_t)registers[decoded.rn] - (uint32_t)op2; break;
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
        if (shift_applied && (decoded.opcode == 0 || decoded.opcode == 1 || decoded.opcode == 12 || decoded.opcode == 9
        || decoded.opcode == 8 || decoded.opcode == 13)) {
            set_bit32(&(registers[16]), 29, carry);
        }
        // addition
        else if (decoded.opcode == 4) {
            if (unsigned_result < registers[decoded.rn]) { // overflow test
                set_bit32(&(registers[16]), 29, 1);
            }
            else {
                set_bit32(&(registers[16]), 29, 0);
            }
        }
        else if (decoded.opcode == 2 || decoded.opcode == 10) {
            if (unsigned_result > registers[decoded.rn]) { // underflow test
                set_bit32(&(registers[16]), 29, 0);
            }
            else {
                set_bit32(&(registers[16]), 29, 1);
            }
        }
        else if (decoded.opcode == 3) {
            if (unsigned_result > op2) {
                set_bit32(&(registers[16]), 29, 0);
            }
            else {
                set_bit32(&(registers[16]), 29, 1);
            }
        }
        // *********************************************
        // Set the Z bit
        if (result == 0) {
            set_bit32(&(registers[16]), 30, 1);
        }
        else {
            set_bit32(&(registers[16]), 30, 0);
        }

        // Set the N bit
        set_bit32(&(registers[16]), 31, CHECK_BIT(result, 31));
    }
}

int execute_branch(decoded_instruction decoded, int32_t *registers) {
    if(!check_condition(decoded.cond, registers)) {
        return 0;
    }
    uint32_t shifted = extract_bits(decoded.offset << 2, 24, 1);
    int32_t offset = sign_extend_24_32(shifted);
    registers[15] += offset;
    return 1;
}

// Executes a decoded Multiply instruction.
void execute_multiply(decoded_instruction decoded, int32_t *registers) {
    if (!check_condition(decoded.cond, registers)) {
        return;
    }

    int64_t result = registers[decoded.rm] * registers[decoded.rs];
    int32_t truncated = ((int32_t*)(&result))[0];
    if (decoded.a) {
        truncated += registers[decoded.rn];
    }

    registers[decoded.rd] = truncated;

    if (decoded.s) {
        set_bit32(&(registers[16]), 31, CHECK_BIT(truncated, 31));
        if (truncated == 0) {
            set_bit32(&(registers[16]), 30, 1);
        }
        else {
            set_bit32(&(registers[16]), 30, 0);
        }
    }
}

// Executes Single Data Transfer instructions
void execute_dt(decoded_instruction decoded, int32_t *registers, uint8_t *mainMemory) {
    if (!check_condition(decoded.cond, registers)) {
        return;
    }

    uint32_t offset = 0;
    if (decoded.i) {
        // To extract the last 4 bits
        uint8_t rm = extract_bits(decoded.offset, 4, 1);
        uint32_t val = registers[rm];
        // To extract the 5-bit shift value
        uint8_t shif_val = extract_bits(decoded.offset, 5, 8);
        // To extract the 2 bit shift type
        uint8_t shift_type = extract_bits(decoded.offset, 2, 6);

        //Logical left lsl
        if (shift_type == 0) {
            offset = val << shif_val;
        }
        // Logical right lsr
        else if (shift_type == 1) {
            offset = val >> shif_val;
        }
        // Arithmetic right
        else if (shift_type == 2) {
            val = val >> shif_val;
            if (CHECK_BIT(val, 31) == 1) {
                uint32_t mask = int_pow(2, 32 - shif_val) * (int_pow(2, shif_val) - 1);
                offset = val | mask;
            }
            else {
                offset = val;
            }
        }
        // Right rotate
        else if (shift_type == 3) {
            offset = right_rotate(val, shif_val);
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

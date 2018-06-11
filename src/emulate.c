#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "utilities.h"

#define RAM_SIZE 65536

// Loads the provided file into main memory, and returns the size of the file.
int loadFile(uint8_t* memory, const char* fileName) {
    FILE* binaryFile = fopen(fileName, "rb");
    if (binaryFile == NULL) {
        perror("Error opening the binary file!");
        exit(EXIT_FAILURE);
    }

    fseek(binaryFile, 0, SEEK_END);
    int size = ftell(binaryFile);
    fseek(binaryFile, 0, SEEK_SET);

    fread(memory, 1, size, binaryFile);

    fclose(binaryFile);
    return size;
}

// To represent the 4 different types of instructions.
typedef enum { DATA_PROCESSING, MULTIPLY, DATA_TRANSFER, BRANCH } instructionType;

// Checks the bit pattern of the instruction to determine its type.
instructionType getInstructionType(uint32_t instruction) {
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

// Represents a decoded instructions with all its components.
typedef struct {
    instructionType type;
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
} decodedInstruction;

// Returns the struct contained the decoded components of the provided Data Processing instruction.
decodedInstruction decodeDP(uint32_t instruction) {
    decodedInstruction decoded;
    decoded.type = DATA_PROCESSING;

    uint32_t result = extractBits(instruction, 4, 29);
    decoded.cond = ((uint8_t*)(&result))[0];

    result = extractBits(instruction, 1, 26);
    decoded.i = ((uint8_t*)(&result))[0];

    result = extractBits(instruction, 4, 22);
    decoded.opcode = ((uint8_t*)(&result))[0];

    result = extractBits(instruction, 1, 21);
    decoded.s = ((uint8_t*)(&result))[0];

    result = extractBits(instruction, 4, 17);
    decoded.rn = ((uint8_t*)(&result))[0];

    result = extractBits(instruction, 4, 13);
    decoded.rd = ((uint8_t*)(&result))[0];

    result = extractBits(instruction, 12, 1);
    decoded.operand2 = ((uint16_t*)(&result))[0];

    return decoded;
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
    switch (decoded.opcode) {
        case 0: result = registers[decoded.rn] & op2; break;
        case 1: result = registers[decoded.rn] ^ op2; break;
        case 2: result = registers[decoded.rn] - op2; unsignedResult = (uint32_t)registers[decoded.rn] - (uint32_t)op2; break;
        case 3: result = op2 - registers[decoded.rn]; unsignedResult = (uint32_t)op2 - (uint32_t)registers[decoded.rn]; break;
        case 4: result = registers[decoded.rn] + op2; unsignedResult = (uint32_t)registers[decoded.rn] + (uint32_t)op2; break;
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

decodedInstruction decodeBranch(uint32_t instruction) {
    decodedInstruction decoded;
    decoded.type = BRANCH;
    uint32_t result = extractBits(instruction, 4, 29);
    decoded.cond = ((uint8_t*)(&result))[0];
    decoded.offset = extractBits(instruction, 24, 1);

    return decoded;
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

//=========================================================================

// Decodes a Multiply instruction.
decodedInstruction decodeMultiply(uint32_t instruction) {
    decodedInstruction decoded;
    decoded.type = MULTIPLY;

    uint32_t result = 0;
    result = extractBits(instruction, 4, 29);
    decoded.cond = ((uint8_t*)(&result))[0];

    result = extractBits(instruction, 1, 22);
    decoded.a = ((uint8_t*)(&result))[0];

    result = extractBits(instruction, 1, 21);
    decoded.s = ((uint8_t*)(&result))[0];

    result = extractBits(instruction, 4, 17);
    decoded.rd = ((uint8_t*)(&result))[0];

    result = extractBits(instruction, 4, 13);
    decoded.rn = ((uint8_t*)(&result))[0];

    result = extractBits(instruction, 4, 9);
    decoded.rs = ((uint8_t*)(&result))[0];

    result = extractBits(instruction, 4, 1);
    decoded.rm = ((uint8_t*)(&result))[0];

    return decoded;
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

//=========================================================================
// Decodes Single Data Transfer instructions
decodedInstruction decodeDT(uint32_t instruction) {
    decodedInstruction decoded;
    decoded.type = DATA_TRANSFER;

    uint32_t result = 0;
    result = extractBits(instruction, 4, 29);
    decoded.cond = ((uint8_t*)(&result))[0];

    result = extractBits(instruction, 1, 26);
    decoded.i = ((uint8_t*)(&result))[0];

    result = extractBits(instruction, 1, 25);
    decoded.p = ((uint8_t*)(&result))[0];

    result = extractBits(instruction, 1, 24);
    decoded.u = ((uint8_t*)(&result))[0];

    result = extractBits(instruction, 1, 21);
    decoded.l = ((uint8_t*)(&result))[0];

    result = extractBits(instruction, 4, 17);
    decoded.rn = ((uint8_t*)(&result))[0];

    result = extractBits(instruction, 4, 13);
    decoded.rd = ((uint8_t*)(&result))[0];

    decoded.offset = extractBits(instruction, 12, 1);

    return decoded;
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

    if (addressToEffect >= RAM_SIZE) {
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
  //=========================================================================

int main(int argc, char **argv) {
    // The main memory of the Raspberry Pi
    uint8_t* mainMemory = malloc(RAM_SIZE);
    memset(mainMemory, 0, RAM_SIZE);

    // Reading the binary code into main memory
    int size = loadFile(mainMemory, argv[1]);

    // Creates the registers
    int32_t* registers = malloc(17 * sizeof(int32_t));
    memset(registers, 0, 17 * sizeof(int32_t));

    // The three stage pipeline
    decodedInstruction decoded;
    int32_t fetched;

    int decodeAvailable = 0;
    int fetchAvailable = 0;
    int halt = 0;

    while (1) {
        // The execution stage of the pipeline.
        if (decodeAvailable) {
            if (halt) {
                break;
            }
            switch (decoded.type) {
                    case DATA_PROCESSING : executeDP(decoded, registers); break;
                    case MULTIPLY : executeMultiply(decoded, registers); break;
                    case DATA_TRANSFER : executeDT(decoded, registers, mainMemory); break;
                    case BRANCH : if (executeBranch(decoded, registers)) { decodeAvailable = 0; fetchAvailable = 0; } break;
                    // should be unreachable
                    default : break;
            }
        }

        // The decode stage of the pipeline.
        if (fetchAvailable) {
            decodeAvailable = 1;
            if (fetched == 0) {
                halt = 1;
            }
            else {
                switch (getInstructionType(fetched)) {
                    case DATA_PROCESSING : decoded = decodeDP(fetched); break;
                    case MULTIPLY : decoded = decodeMultiply(fetched); break;
                    case DATA_TRANSFER : decoded = decodeDT(fetched); break;
                    case BRANCH : decoded = decodeBranch(fetched); break;
                    // should be unreachable
                    default : break;
                }
            }
        }
        fetched = ((int32_t*)(&mainMemory[registers[15]]))[0];
        fetchAvailable = 1;
        registers[15] += 4;
    }

    printf("Registers:\n");
    for (int i = 0; i < 13; i++) {
        printf("$%d\t:\t", i);
        printf("%d\t(0x%08x)\n", registers[i], registers[i]);
    }

    printf("PC\t:\t%d\t(0x%08x)\n", registers[15], registers[15]);
    printf("CPSR\t:\t%d\t(0x%08x)\n", registers[16], registers[16]);

    printf("Non-zero memory:\n");

    for (int i = 0; i < RAM_SIZE; i += 4) {
      if (((int32_t*)(mainMemory))[i / 4] != 0) {
          printf("0x%08x : 0x%02x%02x%02x%02x\n", i, mainMemory[i], mainMemory[i + 1],
          mainMemory[i + 2], mainMemory[i + 3]);
      }
    }

    free(mainMemory);
    free(registers);
    return 0;
  }

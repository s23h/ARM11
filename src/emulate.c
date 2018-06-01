#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "utilities.h"

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

    uint32_t result;

    // Calculates the result of the instruction
    switch (decoded.opcode) {
        case 0: result = registers[decoded.rn] & op2; break;
        case 1: result = registers[decoded.rn] ^ op2; break;
        case 2: result = registers[decoded.rn] - op2; break;
        case 3: result = op2 - registers[decoded.rn]; break;
        case 4: result = registers[decoded.rn] + op2; break;
        case 8: result = registers[decoded.rn] & op2; break;
        case 9: result = registers[decoded.rn] ^ op2; break;
        case 10: result = registers[decoded.rn] - op2; break;
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
        if (decoded.opcode == 0 || decoded.opcode == 1 || decoded.opcode == 12 || decoded.opcode == 9
        || decoded.opcode == 8 || decoded.opcode == 13) {
            setBit32(&(registers[16]), 29, carry);
        }
        // addition
        else if (decoded.opcode == 4) {
            if (result < registers[decoded.rn]) { // overflow test
                setBit32(&(registers[16]), 29, 1);
            }
            else {
                setBit32(&(registers[16]), 29, 0);
            }
        }
        else if (decoded.opcode == 2 || decoded.opcode == 10) {
            if (result > registers[decoded.rn]) { // underflow test
                setBit32(&(registers[16]), 29, 0);
            }
            else {
                setBit32(&(registers[16]), 29, 1);
            }
        }
        else if (decoded.opcode == 3) {
            if (result > op2) {
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

void executeBranch(decodedInstruction decoded, int32_t* registers) {
    if(!checkCondition(decoded.cond, registers)) {
        return;
    }
    uint32_t shifted = extractBits(decoded.offset << 2, 24, 1);
    int32_t offset = sign_extend_24_32(shifted);
    registers[15] += offset;
}

//=========================================================================

decodedInstruction decodeMultiply(uint32_t instruction) {
    decodedInstruction decoded;
    decoded.type = MULTIPLY;

    uint32_t result = 0;
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

    result = extractBits(instruction, 12, 1);
    decoded.operand2 = ((uint16_t*)(&result))[0];
    return decoded;
}

uint64_t multiply(uint32_t n1, uint32_t n2) {
    uint64_t result = 0;

    uint32_t R = n1;
    uint32_t M = n2;

    while (R > 0) {
        if (R & 1) {
            result += M;
        }
        R >>= 1;
        M <<= 1;
    }

    return result;
}


void executeMultiply(decodedInstruction decoded, int32_t* registers) {

  uint32_t result;

  uint32_t op1 = registers[decoded.rm];
  uint32_t op2 = registers[decoded.rs];

  uint64_t partialResult = multiply(op1, op2);
  result = extractBitsFrom64(partialResult, 32, 1);

  if (decoded.a == 1) {
      uint32_t acc = registers[decoded.rn];

      uint8_t carry = 0; //carry
      while (acc != 0) {
          //find carry and shift it left
          carry = (result & acc) << 1;
          //find the sum
          result = result ^ acc;
          acc = carry;
      }
    }
  }

//=========================================================================
decodedInstruction decodeDT(uint32_t instruction) {
    decodedInstruction decoded;
    decoded.type = DATA_TRANSFER;

    uint32_t result = extractBits(instruction, 4, 29);
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


void executeDT(decodedInstruction decoded, int32_t* registers, uint8_t* mainMemory) {
    if (!checkCondition(decoded.cond, registers)) {
        return;
    }

    uint16_t offset;
    if(decoded.cond==0){
      offset = decoded.operand2;
    } else {
      // To extract the last 4 bits
      uint8_t rm = extractBits(decoded.operand2, 4, 1);
      uint32_t val = registers[rm];
      // To extract the 5-bit shift value
      uint8_t shiftVal = extractBits(decoded.operand2, 5, 8);
      // To extract the 2 bit shift type
      uint8_t shiftType = extractBits(decoded.operand2, 2, 6);

      //Logical left lsl
      if (shiftType == 0) {
          carry = CHECK_BIT(val, 32 - shiftVal);
          offset = val << shiftVal;
      }
      // Logical right lsr
      else if (shiftType == 1) {
          carry = CHECK_BIT(val, shiftVal - 1);
          offset = val >> shiftVal;
      }
      // Arithmetic right
      else if (shiftType == 2) {
          carry = CHECK_BIT(val, shiftVal - 1);
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
          carry = CHECK_BIT(val, shiftVal - 1);
          offset = rightRotate(val, shiftVal);
      }

    }

// LOAD from memory into register (l=1)
    if (decoded.p == 1 && decoded.u == 1 && decoded.l == 1) {
        registers[decoded.rd] = mainMemory[registers[decoded.rn + offset]];
    } else if (decoded.p == 1 && decoded.u == 0 && decoded.l == 1) {
        registers[decoded.rd] = mainMemory[registers[decoded.rn - offset]];
    } else if (decoded.p == 0 && decoded.u == 1 && decoded.l == 1) {
        registers[decoded.rd] = mainMemory[registers[decoded.rn] + offset];
    } else if (decoded.p == 0 && decoded.u == 0 && decoded.l == 1) {
        registers[decoded.rd] = mainMemory[registers[decoded.rn] - offset];
    }

// STORE from register to memory (l=0)

    if (decoded.p == 1 && decoded.u == 1 && decoded.l == 0) {
        mainMemory[registers[decoded.rd]] = registers[decoded.rn + offset];
    } else if (decoded.p == 1 && decoded.u == 0 && decoded.l == 0) {
        mainMemory[registers[decoded.rd]] = registers[decoded.rn - offset];
    }  else if (decoded.p == 0 && decoded.u == 1 && decoded.l == 0) {
        mainMemory[registers[decoded.rd]] = registers[decoded.rn] + offset;
    } else if (decoded.p == 0 && decoded.u == 0 && decoded.l == 0) {
        mainMemory[registers[decoded.rd]] = registers[decoded.rn] - offset;
    }

  }

  //=========================================================================

int main(int argc, char **argv) {
    // The main memory of the Raspberry Pi
    uint8_t* mainMemory = malloc(65536);
    memset(mainMemory, 0, 65536);

    // Reading the binary code into main memory
    int size = loadFile(mainMemory, argv[1]);

    // Creates the registers
    int32_t* registers = malloc(17 * sizeof(int32_t));
    memset(registers, 0, 17 * sizeof(int32_t));



    // The three stage pipeline
    /*while (1) {
        if (executeAvailable) {
            if (halt) {
                break;
            }
            else {
                switch (decoded.type) {
                    case DATA_PROCESSING : executeDP(decoded, registers); break;
                    case MULTIPLY : executeMultiply(decoded, registers); break;
                    case DATA_TRANSFER : executeDT(decoded, registers, mainMemory); break;
                    case BRANCH : executeBranch(decoded, registers); executeAvailable = 0; break;
                    // should be unreachable
                    default : break;
                }
            }
        }
        if (decodeAvailable) {
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
        fetched = ((uint32_t*)(&mainMemory[registers[15]]))[0];
        registers[15] += 4;
        if (decodeAvailable) {
            executeAvailable = 1;
        }
        decodeAvailable = 1;
    }*/

    decodedInstruction decoded;
    uint32_t fetched;

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
                    case BRANCH : executeBranch(decoded, registers); decodeAvailable = 0; fetchAvailable = 0; break;
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
        fetched = ((uint32_t*)(&mainMemory[registers[15]]))[0];
        fetchAvailable = 1;
        registers[15] += 4;
    }

    printf("Registers:\n");
    for (int i = 0; i < 13; i++) {
        printf("%d : ", i);
        printf("%d %X\n", registers[i], registers[i]);
    }

    printf("PC : %d %X\n", registers[15], registers[15]);
    printf("CPSR : %d %X\n", registers[16], registers[16]);

    printf("Non-zero memory locations: \n");

    for (int i = 0; i < size; i++) {
        if (i % 4 == 0) {
            if (((uint32_t*)(mainMemory))[i / 4] != 0) {
                printf("%d : %X\n", i, ((uint32_t*)(mainMemory))[i / 4]);
            }
        }
    }

    free(mainMemory);
    free(registers);
    return 0;
  }

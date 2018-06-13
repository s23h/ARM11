#include "symbolTable.h"
#include <stdlib.h>
#include <string.h>
#include "utilities.h"

// Returns a 32-bit value representing the machine code instruction corresponding to the specified
// Data Processing instruction, represented in token form
int32_t assembleDP(char** tokens) {
    int32_t opcode = tableLookup(&opcodes, tokens[0]);

    int32_t rd = 0;
    int32_t rn = 0;
    int32_t operand2 = 0;
    int s = 0;

    int op2Position = 0;
    int base = 10;

    char* ptr;
    switch (opcode) {
        case 8:
        case 9:
        case 10:
            op2Position = 2;
            s = 1;
            rd = 0;
            rn = (int32_t)(strtol(tokens[1] + 1, &ptr, 10)); break;
        case 13:
            op2Position = 2;
            s = 0;
            rd = (int32_t)(strtol(tokens[1] + 1, &ptr, 10));
            rn = 0; break;
        default:
            op2Position = 3;
            s = 0;
            rd = (int32_t)(strtol(tokens[1] + 1, &ptr, 10));
            rn = (int32_t)(strtol(tokens[2] + 1, &ptr, 10)); break;
    }

    if (tokens[op2Position][1] == '0' && tokens[op2Position][2] == 'x') {
        base = 16;
    }

    int iBit = 0;
    if (tokens[op2Position][0] == 'r') {
      iBit = 0;
    }
    else if (tokens[op2Position][0] == '#') {
      iBit = 1;
    }

    operand2 = (int32_t)(strtol(tokens[op2Position] + 1, &ptr, base));

    int32_t numRotates = 0;

    while ((operand2 > 255 || operand2 < 0) && numRotates < 30) {
      operand2 = leftRotate(operand2, 2);
      numRotates += 2;
    }

    if (operand2 > 255) {
      fprintf(stderr, "The immediate constant value cannot be represented.");
      exit(EXIT_FAILURE);
    }

    int32_t mask = (numRotates / 2) << 8;
    operand2 = operand2 | mask;

    int32_t instruction = -503316480;

    // Set the S bit
    setBit32(&instruction, 20, s);

    // Set the I bit
    setBit32(&instruction, 25, iBit);

    // Set the opcode
    mask = opcode << 21;
    instruction = instruction | mask;

    // Set Rn
    mask = rn << 16;
    instruction = instruction | mask;

    // Set Rd
    mask = rd << 12;
    instruction = instruction | mask;

    mask = operand2;
    instruction = instruction | mask;

    return instruction;
}

// Returns a 32-bit value representing the machine code instruction corresponding to the specified
// Multiply instruction, represented in token form
int32_t assembleMult(char** tokens) {
    // Sets the Accumulate bit
    int a = 0;
    if (strcmp(tokens[0], "mla") == 0) {
        a = 1;
    }

    char* ptr;

    // Sets the register values
    int32_t rd = (int32_t)(strtol(tokens[1] + 1, &ptr, 10));
    int32_t rm = (int32_t)(strtol(tokens[2] + 1, &ptr, 10));
    int32_t rs = (int32_t)(strtol(tokens[3] + 1, &ptr, 10));

    int32_t rn = 0;
    if (a) {
        rn = (int32_t)(strtol(tokens[4] + 1, &ptr, 10));
    }

    int32_t instruction = -536870768;
    setBit32(&instruction, 21, a);

    int32_t mask = rd << 16;
    instruction = instruction | mask;

    mask = rn << 12;
    instruction = instruction | mask;

    mask = rs << 8;
    instruction = instruction | mask;

    mask = rm;
    instruction = instruction | mask;

    return instruction;
}

int32_t assembleBranch(char** tokens, int32_t currentAddress, symbolTable* labelsMap) {
    int32_t cond = 0;
    cond = tableLookup(&opcodes, tokens[0]);

    int32_t offset = tableLookup(labelsMap, tokens[1]) - currentAddress;
    offset -= 8; // takes into account of the 8 byte pipeline effect

    int32_t offset_26_bit = 0;
    if (offset < 0) {
        setBit32(&offset_26_bit, 25, 1);
        int32_t mask = ((int32_t)intPow(2, 25)) + offset;
        offset_26_bit = offset_26_bit | mask;
    }
    else {
        offset_26_bit = offset;
    }

    int32_t offset_24_bit = extractBits(offset_26_bit >> 2, 24, 1);

    int32_t instruction = 167772160;
    int32_t mask = cond << 28;
    instruction = instruction | mask;

    mask = offset_24_bit;
    instruction = instruction | mask;

    return instruction;
}

// Assembles Single Data Transfer instructions, returns 32-bit machine code instruction.
int32_t assembleDT(char** tokens, int32_t currentAddress, FILE* input, FILE* output, int numTokens, int32_t endAddress) {
    int L = 0;
    if (strcmp(tokens[0], "ldr") == 0) {
        L = 1;
    }

    char* ptr;
    int32_t rd = (int32_t)(strtol(tokens[1] + 1, &ptr, 10));
    int32_t rn = 0;
    int U = 1;
    int P = 1;
    int I = 0;

    int32_t offset = 0;

    // Numeric constant
    if (tokens[2][0] == '=') {
        int32_t num = (int32_t)(strtol(tokens[2] + 1, &ptr, 16));
        if (num <= 255) {
            tokens[0] = "mov";
            tokens[2][0] = '#';
            return assembleDP(tokens);
        }

        unsigned long inputPos = ftell(input);
        unsigned long outputPos = ftell(output);

        fseek(input, inputPos, SEEK_SET);

        offset = endAddress + numDataWrites * 4 - currentAddress;

        // Writes the =expression in 4 bytes at the end of the assembled program. We have to first fill up the file with zeroes, so that the expression
        // is written to the file at the right address.
        if (numDataWrites == 0) {
          int32_t* zeroes = calloc(4, offset / 4);
          fwrite(zeroes, 4, offset / 4, output);
          free(zeroes);
        }

        fseek(output, 0, SEEK_END);
        fwrite(&num, 4, 1, output);
        numDataWrites++;
        fseek(output, outputPos, SEEK_SET);

        offset -= 8; // takes into account of the 8 byte pipeline effect

        rn = 15;
    }
    // Register case
    else {
      if (numTokens == 3) {
        rn = (int32_t)(strtol(tokens[2] + 2, &ptr, 10));
        offset = 0;
      }
      else {
        if (tokens[2][strlen(tokens[2]) - 1] == ']') {
          P = 0;
        }

        rn = (int32_t)(strtol(tokens[2] + 2, &ptr, 10));
        int base = 10;
        if (strlen(tokens[3]) >= 4 && tokens[3][2] == 'x') {
          base = 16;
        }
        if (strlen(tokens[3]) >= 5 && tokens[3][3] == 'x') {
          base = 16;
        }
        offset = (int32_t)(strtol(tokens[3] + 1, &ptr, base));
        if (offset < 0) {
          offset = -1 * offset;
          U = 0;
        }
      }
    }

    int32_t instruction = -469762048;
    setBit32(&instruction, 25, I);
    setBit32(&instruction, 24, P);
    setBit32(&instruction, 23, U);
    setBit32(&instruction, 20, L);

    int32_t mask = rn << 16;
    instruction = instruction | mask;

    mask = rd << 12;
    instruction = instruction | mask;

    instruction = instruction | offset;

    return instruction;
}

int32_t assembleSpecial(char** tokens) {
  if (strcmp(tokens[0], "andeq") == 0) {
    return 0;
  }
  else {
    char* ptr;
    int32_t rd = (int32_t)(strtol(tokens[1] + 1, &ptr, 10));
    int32_t operand2 = rd;
    int base = 10;
    if (tokens[2][2] == 'x') {
      base = 16;
    }
    int32_t shiftVal = (int32_t)(strtol(tokens[2] + 1, &ptr, base));
    int32_t mask = shiftVal << 7;
    operand2 = operand2 | mask;

    int32_t instruction = 3785359360;
    instruction = instruction | operand2;
    mask = rd << 12;
    instruction = instruction | mask;
    return instruction;
  }
}

// Determines the instruction type from the list of tokens provided.
instructionType getInstructionType(char** tokens) {
    if ((strcmp(tokens[0], "add") == 0) || (strcmp(tokens[0], "sub") == 0) || (strcmp(tokens[0], "rsb") == 0)
        || (strcmp(tokens[0], "and") == 0) || (strcmp(tokens[0], "eor") == 0) || (strcmp(tokens[0], "orr") == 0)
        || (strcmp(tokens[0], "mov") == 0) || (strcmp(tokens[0], "tst") == 0) || (strcmp(tokens[0], "teq") == 0)
        || (strcmp(tokens[0], "cmp") == 0)) {
        return DATA_PROCESSING;
    }

    if ((strcmp(tokens[0], "mul") == 0) || (strcmp(tokens[0], "mla") == 0)) {
        return MULTIPLY;
    }

    if ((strcmp(tokens[0], "ldr") == 0) || (strcmp(tokens[0], "str") == 0)) {
        return DATA_TRANSFER;
    }

    if ((strcmp(tokens[0], "beq") == 0) || (strcmp(tokens[0], "bne") == 0) || (strcmp(tokens[0], "bge") == 0) ||
        (strcmp(tokens[0], "blt") == 0) || (strcmp(tokens[0], "bgt") == 0) || (strcmp(tokens[0], "ble") == 0) ||
        (strcmp(tokens[0], "b") == 0) || strcmp(tokens[0], "bal") == 0) {
        return BRANCH;
    }

    if ((strcmp(tokens[0], "lsl") == 0) || (strcmp(tokens[0], "andeq") == 0)) {
        return SPECIAL;
    }

    return LABEL;
}

#include <stdlib.h>
#include <string.h>

#include "symbolTable.h"
#include "../utilities.h"
#include "assembleInstruction.h"

// Determines the instruction type from the list of tokens provided.
instruction_Type get_Instruction_Type(char** tokens) {
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


// Returns a 32-bit value representing the machine code instruction corresponding to the specified
// Data Processing instruction, represented in token form
int32_t assemble_DP(char** tokens) {
    int32_t opcode = table_Lookup(opcodes, tokens[0]);

    int32_t rd = 0;
    int32_t rn = 0;
    int32_t operand_2 = 0;
    int s = 0;

    int op_2_Position = 0;
    int base = 10;

    char* ptr;
    switch (opcode) {
        case 8:
        case 9:
        case 10:
            op_2_Position = 2;
            s = 1;
            rd = 0;
            rn = (int32_t)(strtol(tokens[1] + 1, &ptr, 10));
            break;
        case 13:
            op_2_Position = 2;
            s = 0;
            rd = (int32_t)(strtol(tokens[1] + 1, &ptr, 10));
            rn = 0;
            break;
        default:
            op_2_Position = 3;
            s = 0;
            rd = (int32_t)(strtol(tokens[1] + 1, &ptr, 10));
            rn = (int32_t)(strtol(tokens[2] + 1, &ptr, 10));
            break;
    }

    if (tokens[op_2_Position][1] == '0' && tokens[op_2_Position][2] == 'x') {
        base = 16;
    }

    int iBit = 0;
    if (tokens[op_2_Position][0] == 'r') {
        iBit = 0;
    }
    else if (tokens[op_2_Position][0] == '#') {
        iBit = 1;
    }

    operand_2 = (int32_t)(strtol(tokens[op_2_Position] + 1, &ptr, base));

    int32_t num_Rotates = 0;

    while ((operand_2 > 255 || operand_2 < 0) && num_Rotates < 30) {
        operand_2 = left_Rotate(operand_2, 2);
        num_Rotates += 2;
    }

    if (operand_2 > 255) {
        fprintf(stderr, "The immediate constant value cannot be represented.");
        exit(EXIT_FAILURE);
    }

    int32_t mask = (num_Rotates / 2) << 8;
    operand_2 = operand_2 | mask;

    int32_t instruction = -503316480;

    // Set the S bit
    set_Bit_32(&instruction, 20, s);

    // Set the I bit
    set_Bit_32(&instruction, 25, iBit);

    // Set the opcode
    mask = opcode << 21;
    instruction = instruction | mask;

    // Set Rn
    mask = rn << 16;
    instruction = instruction | mask;

    // Set Rd
    mask = rd << 12;
    instruction = instruction | mask;

    mask = operand_2;
    instruction = instruction | mask;

    return instruction;
}

// Returns a 32-bit value representing the machine code instruction corresponding to the specified
// Multiply instruction, represented in token form
int32_t assemble_Mult(char** tokens) {
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
    set_Bit_32(&instruction, 21, a);

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

int32_t assemble_Branch(char** tokens, int32_t current_Address, symbol_Table* labels_Map) {
    int32_t cond = 0;
    cond = table_Lookup(opcodes, tokens[0]);

    int32_t offset = table_Lookup(labels_Map, tokens[1]) - current_Address;
    offset -= 8; // takes into account of the 8 byte pipeline effect

    int32_t offset_26_bit = 0;
    if (offset < 0) {
        set_Bit_32(&offset_26_bit, 25, 1);
        int32_t mask = ((int32_t) int_Pow(2, 25)) + offset;
        offset_26_bit = offset_26_bit | mask;
    }
    else {
        offset_26_bit = offset;
    }

    int32_t offset_24_bit = extract_Bits(offset_26_bit >> 2, 24, 1);

    int32_t instruction = 167772160;
    int32_t mask = cond << 28;
    instruction = instruction | mask;

    mask = offset_24_bit;
    instruction = instruction | mask;

    return instruction;
}

// Assembles Single Data Transfer instructions, returns 32-bit machine code instruction.
int32_t assemble_DT(char** tokens, int32_t current_Address, FILE* input, FILE* output, int num_Tokens, int32_t end_Address) {
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
            return assemble_DP(tokens);
        }

        unsigned long input_Pos = ftell(input);
        unsigned long output_Pos = ftell(output);

        fseek(input, input_Pos, SEEK_SET);

        offset = end_Address + (num_Data_Writes * 4) - current_Address;

        // Writes the =expression in 4 bytes at the end of the assembled program. We have to first fill up the file with zeroes, so that the expression
        // is written to the file at the right address.
        if (num_Data_Writes == 0) {
            int32_t* zeroes = calloc(4, offset / 4);
            fwrite(zeroes, 4, offset / 4, output);
            free(zeroes);
        }

        fseek(output, 0, SEEK_END);
        fwrite(&num, 4, 1, output);
        num_Data_Writes++;
        fseek(output, output_Pos, SEEK_SET);

        offset -= 8; // takes into account of the 8 byte pipeline effect

        rn = 15;
    }
    // Register case
    else {

        if (num_Tokens == 3) {
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
    set_Bit_32(&instruction, 25, I);
    set_Bit_32(&instruction, 24, P);
    set_Bit_32(&instruction, 23, U);
    set_Bit_32(&instruction, 20, L);

    int32_t mask = rn << 16;
    instruction = instruction | mask;

    mask = rd << 12;
    instruction = instruction | mask;

    instruction = instruction | offset;

    return instruction;
}

int32_t assemble_Special(char** tokens) {
    if (strcmp(tokens[0], "andeq") == 0) {
        return 0;
    }

    else {
        char* ptr;
        int32_t rd = (int32_t)(strtol(tokens[1] + 1, &ptr, 10));
        int32_t operand_2 = rd;
        int base = 10;

    if (tokens[2][2] == 'x') {
        base = 16;
    }

    int32_t shift_Val = (int32_t)(strtol(tokens[2] + 1, &ptr, base));
    int32_t mask = shift_Val << 7;
    operand_2 = operand_2 | mask;

    int32_t instruction = -509607936;
    instruction = instruction | operand_2;
    mask = rd << 12;
    instruction = instruction | mask;
    return instruction;
    }
}

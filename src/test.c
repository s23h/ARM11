#include <stdio.h>
#include "symbolTable.h"
#include <stdlib.h>
#include <string.h>
#include "utilities.h"

#define MAX_TOKENS 6
#define MAXC 511

typedef enum { DATA_PROCESSING, MULTIPLY, DATA_TRANSFER, BRANCH, LABEL, SPECIAL } instruction_Type;

symbol_Table opcodes;

// Breaks down a line into its constituent tokens -- e.g. mov r1, r2 would be turned into an string array containing
// mov, r1, r2 as its elements
char** tokenize(char* line) {
    char delim[] = " ,:\n";
    char** token_Array = malloc(MAX_TOKENS * sizeof(char*));
    for (int i = 0; i < MAX_TOKENS; i++) {
        token_Array[i] = NULL;
    }

    char *token;

    int i = 0;
    token = strtok(line, delim);

    while (token != NULL) {
        token_Array[i] = token;
        i++;
        token = strtok(NULL, delim);
    }

    return token_Array;
}

// Returns a 32-bit value representing the machine code instruction corresponding to the specified
// Data Processing instruction, represented in token form
int32_t assemble_DP(char** tokens) {
    int32_t opcode = table_Lookup(&opcodes, tokens[0]);

    int32_t rd = 0;
    int32_t rn = 0;
    int32_t operand_2 = 0;
    int s = 0;

    int op2_Position = 0;
    int base = 10;

    char* ptr;
    switch (opcode) {
        case 8:
        case 9:
        case 10:
            op2_Position = 2;
            s = 1;
            rd = 0;
            rn = (int32_t)(strtol(tokens[1] + 1, &ptr, 10)); break;
        case 13:
            op2_Position = 2;
            s = 0;
            rd = (int32_t)(strtol(tokens[1] + 1, &ptr, 10));
            rn = 0; break;
        default:
            op2_Position = 3;
            s = 0;
            rd = (int32_t)(strtol(tokens[1] + 1, &ptr, 10));
            rn = (int32_t)(strtol(tokens[2] + 1, &ptr, 10)); break;
    }

    if (tokens[op2_Position][1] == '0' && tokens[op2_Position][2] == 'x') {
        base = 16;
    }

    int i_Bit = 0;
    if (tokens[op2_Position][0] == 'r') {
      i_Bit = 0;
    }
    else if (tokens[op2_Position][0] == '#') {
      i_Bit = 1;
    }

    operand_2 = (int32_t)(strtol(tokens[op2_Position] + 1, &ptr, base));

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
    set_Bit_32(&instruction, 25, i_Bit);

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

int32_t assemble_Branch(char** tokens, int32_t currentAddress, symbol_Table* labelsMap) {
    int32_t cond = 0;
    cond = table_Lookup(&opcodes, tokens[0]);

    int32_t offset = table_Lookup(labelsMap, tokens[1]) - currentAddress;
    offset -= 8; // takes into account of the 8 byte pipeline effect

    int32_t offset_26_bit = 0;
    if (offset < 0) {
        set_Bit_32(&offset_26_bit, 25, 1);
        int32_t mask = ((int32_t)int_Pow(2, 25)) + offset;
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
int32_t assemble_DT(char** tokens, int32_t current_Address, FILE* input, FILE* output) {
    int L = 0;
    if (strcmp(tokens[0], "ldr")) {
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

        // Works out the address at the end of the assembled program by going through line by line.
        int32_t address_Counter = current_Address + 4;

        char line[MAXC + 1]; // +1 for the null character at the end
        while (fgets(line, sizeof(line), input) != NULL)
        {
            address_Counter += 4;
        }

        fseek(input, input_Pos, SEEK_SET);

        offset = address_Counter - current_Address;

        // Writes the =expression in 4 bytes at the end of the assembled program. We have to first fill up the file with zeroes, so that the expression
        // is written to the file at the right address.
        int32_t* zeroes = calloc(4, offset);
        fwrite(zeroes, 4, offset, output);
        fwrite(&num, 4, 1, output);
        free(zeroes);
        fseek(output, output_Pos, SEEK_SET);

        offset -= 8; // takes into account of the 8 byte pipeline effect

        rn = 15;
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

int main(int argc, char** argv) {
    initTable(&opcodes);

    insert_Back(&opcodes, "and", 0);
    insert_Back(&opcodes, "eor", 1);
    insert_Back(&opcodes, "sub", 2);
    insert_Back(&opcodes, "rsb", 3);
    insert_Back(&opcodes, "add", 4);
    insert_Back(&opcodes, "orr", 12);
    insert_Back(&opcodes, "mov", 13);
    insert_Back(&opcodes, "tst", 8);
    insert_Back(&opcodes, "teq", 9);
    insert_Back(&opcodes, "cmp", 10);

    insert_Back(&opcodes, "beq", 0);
    insert_Back(&opcodes, "bne", 1);
    insert_Back(&opcodes, "bge", 10);
    insert_Back(&opcodes, "blt", 11);
    insert_Back(&opcodes, "bgt", 12);
    insert_Back(&opcodes, "ble", 13);
    insert_Back(&opcodes, "bal", 14);
    insert_Back(&opcodes, "b", 14);

    /*
    for (tableIter iter = tableBegin(&opcodes); iter != tableEnd(&opcodes); iter = iter -> next) {
        printf("%u\n", tableLookup(&opcodes, iter -> key));
    }*/

    /*char instruction[] = "mul r1, r12, r3";
    int32_t ins = assemble_Mult(tokenize(instruction));
    printf("%d", ins); */

    symbol_Table labels_Addresses;
    initTable(&labels_Addresses);

    // The first pass over the source file that builds up the symbol table.
    FILE *first_Pass = fopen(argv[1], "r");
    int32_t address_Counter = 0;
    if (first_Pass != NULL)
    {
        char line[MAXC + 1]; // +1 for the null character at the end
        while (fgets(line, sizeof(line), first_Pass) != NULL) /* read a line */
        {
            if (line[0] == '\n') {
              continue;
            }
            char** tokens = tokenize(line);
            if (get_Instruction_Type(tokens) == LABEL) {
                insert_Back(&labels_Addresses, tokens[0], address_Counter);
            }
            else {
                address_Counter += 4;
            }

        }
        fclose(first_Pass);
    }
    else
    {
        perror(""); /* why didn't the file open? */
    }

    /*
    for (tableIter iter = tableBegin(&labelsAddresses); iter != tableEnd(&labelsAddresses); iter = iter -> next) {
        printf("%s %u\n", iter -> key, tableLookup(&labelsAddresses, iter -> key));
    }*/

    // The second pass over the source file, outputs the binary instructions.
    FILE *input = fopen(argv[1], "r");
    FILE *output = fopen(argv[2], "wb");

    if (input != NULL && output != NULL)
    {
        address_Counter = 0;
        char line[MAXC + 1]; // +1 for the null character at the end
        while (fgets(line, sizeof(line), input) != NULL)
        {
            if (line[0] == '\n') {
              continue;
            }
            char** tokens = tokenize(line);
            printf("%s\n", tokens[0]);
            instruction_Type type = get_Instruction_Type(tokens);
            if (type == LABEL) {
                continue;
            }
            int32_t instruction = 0;
            switch (type) {
                case DATA_PROCESSING: instruction = assemble_DP(tokens); break;
                case MULTIPLY: instruction = assemble_Mult(tokens); break;
                case BRANCH: instruction = assemble_Branch(tokens, address_Counter, &labels_Addresses); break;
                case DATA_TRANSFER: instruction = assemble_DT(tokens, address_Counter, input, output); break;
            }
            fwrite(&instruction, sizeof(int32_t), 1, output);
            address_Counter += 4;
        }
        fclose(input);
        fclose(output);
    }
    else
    {
        perror("");
    }

    destroy_Table(&labels_Addresses);
    destroy_Table(&opcodes);
}

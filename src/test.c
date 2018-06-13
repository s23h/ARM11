#include <stdio.h>
#include "symbolTable.h"
#include <stdlib.h>
#include <string.h>
#include "utilities.h"

#define MAX_TOKENS 6
#define MAXC 511

typedef enum { DATA_PROCESSING, MULTIPLY, DATA_TRANSFER, BRANCH, LABEL, SPECIAL } instruction_type;

symbol_table opcodes;

// Breaks down a line into its constituent tokens -- e.g. mov r1, r2 would be turned into an string array containing
// mov, r1, r2 as its elements
char **tokenize(char *line) {
    char delim[] = " ,:\n";
    char **token_array = malloc(MAX_TOKENS * sizeof(char*));
    for (int i = 0; i < MAX_TOKENS; i++) {
        token_array[i] = NULL;
    }

    char *token;

    int i = 0;
    token = strtok(line, delim);

    while (token != NULL) {
        token_array[i] = token;
        i++;
        token = strtok(NULL, delim);
    }

    return token_array;
}

// Returns a 32-bit value representing the machine code instruction corresponding to the specified
// Data Processing instruction, represented in token form
int32_t assemble_dp(char **tokens) {
    int32_t opcode = table_lookup(&opcodes, tokens[0]);

    int32_t rd = 0;
    int32_t rn = 0;
    int32_t operand2 = 0;
    int s = 0;

    int op2_position = 0;
    int base = 10;

    char *ptr;
    switch (opcode) {
        case 8:
        case 9:
        case 10:
            op2_position = 2;
            s = 1;
            rd = 0;
            rn = (int32_t)(strtol(tokens[1] + 1, &ptr, 10)); break;
        case 13:
            op2_position = 2;
            s = 0;
            rd = (int32_t)(strtol(tokens[1] + 1, &ptr, 10));
            rn = 0; break;
        default:
            op2_position = 3;
            s = 0;
            rd = (int32_t)(strtol(tokens[1] + 1, &ptr, 10));
            rn = (int32_t)(strtol(tokens[2] + 1, &ptr, 10)); break;
    }

    if (tokens[op2_position][1] == '0' && tokens[op2_position][2] == 'x') {
        base = 16;
    }

    int i_Bit = 0;
    if (tokens[op2_position][0] == 'r') {
      i_Bit = 0;
    }
    else if (tokens[op2_position][0] == '#') {
      i_Bit = 1;
    }

    operand2 = (int32_t)(strtol(tokens[op2_position] + 1, &ptr, base));

    int32_t num_rotates = 0;

    while ((operand2 > 255 || operand2 < 0) && num_rotates < 30) {
        operand2 = left_rotate(operand2, 2);
        num_rotates += 2;
    }

    if (operand2 > 255) {
        fprintf(stderr, "The immediate constant value cannot be represented.");
        exit(EXIT_FAILURE);
    }

    int32_t mask = (num_rotates / 2) << 8;
    operand2 = operand2 | mask;

    int32_t instruction = -503316480;

    // Set the S bit
    set_bit32(&instruction, 20, s);

    // Set the I bit
    set_bit32(&instruction, 25, i_Bit);

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
int32_t assemble_mult(char **tokens) {
    // Sets the Accumulate bit
    int a = 0;
    if (strcmp(tokens[0], "mla") == 0) {
        a = 1;
    }

    char *ptr;

    // Sets the register values
    int32_t rd = (int32_t)(strtol(tokens[1] + 1, &ptr, 10));
    int32_t rm = (int32_t)(strtol(tokens[2] + 1, &ptr, 10));
    int32_t rs = (int32_t)(strtol(tokens[3] + 1, &ptr, 10));

    int32_t rn = 0;

    if (a) {
        rn = (int32_t)(strtol(tokens[4] + 1, &ptr, 10));
    }

    int32_t instruction = -536870768;
    set_bit32(&instruction, 21, a);

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

int32_t assemble_branch(char **tokens, int32_t current_address, symbol_table *labels_map) {
    int32_t cond = 0;
    cond = table_lookup(&opcodes, tokens[0]);

    int32_t offset = table_lookup(labels_map, tokens[1]) - current_address;
    offset -= 8; // takes into account of the 8 byte pipeline effect

    int32_t offset_26_bit = 0;
    if (offset < 0) {
        set_bit32(&offset_26_bit, 25, 1);
        int32_t mask = ((int32_t) int_pow(2, 25)) + offset;
        offset_26_bit = offset_26_bit | mask;
    }
    else {
        offset_26_bit = offset;
    }

    int32_t offset_24_bit = extract_bits(offset_26_bit >> 2, 24, 1);

    int32_t instruction = 167772160;
    int32_t mask = cond << 28;
    instruction = instruction | mask;

    mask = offset_24_bit;
    instruction = instruction | mask;

    return instruction;
}

// Assembles Single Data Transfer instructions, returns 32-bit machine code instruction.
int32_t assemble_dt(char **tokens, int32_t current_address, FILE *input, FILE *output) {
    int L = 0;
    if (strcmp(tokens[0], "ldr")) {
        L = 1;
    }

    char *ptr;
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
            return assemble_dp(tokens);
        }

        unsigned long input_pos = ftell(input);
        unsigned long output_pos = ftell(output);

        // Works out the address at the end of the assembled program by going through line by line.
        int32_t address_counter = current_address + 4;

        char line[MAXC + 1]; // +1 for the null character at the end
        while (fgets(line, sizeof(line), input) != NULL)
        {
            address_counter += 4;
        }

        fseek(input, input_pos, SEEK_SET);

        offset = address_counter - current_address;

        // Writes the =expression in 4 bytes at the end of the assembled program. We have to first fill up the file with zeroes, so that the expression
        // is written to the file at the right address.
        int32_t *zeroes = calloc(4, offset);
        fwrite(zeroes, 4, offset, output);
        fwrite(&num, 4, 1, output);
        free(zeroes);
        fseek(output, output_pos, SEEK_SET);

        offset -= 8; // takes into account of the 8 byte pipeline effect

        rn = 15;
    }

    int32_t instruction = -469762048;
    set_bit32(&instruction, 25, I);
    set_bit32(&instruction, 24, P);
    set_bit32(&instruction, 23, U);
    set_bit32(&instruction, 20, L);

    int32_t mask = rn << 16;
    instruction = instruction | mask;

    mask = rd << 12;
    instruction = instruction | mask;

    instruction = instruction | offset;

    return instruction;
}

// Determines the instruction type from the list of tokens provided.
instruction_type get_instruction_type(char **tokens) {
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

int main(int argc, char **argv) {
    init_table(&opcodes);

    insert_back(&opcodes, "and", 0);
    insert_back(&opcodes, "eor", 1);
    insert_back(&opcodes, "sub", 2);
    insert_back(&opcodes, "rsb", 3);
    insert_back(&opcodes, "add", 4);
    insert_back(&opcodes, "orr", 12);
    insert_back(&opcodes, "mov", 13);
    insert_back(&opcodes, "tst", 8);
    insert_back(&opcodes, "teq", 9);
    insert_back(&opcodes, "cmp", 10);

    insert_back(&opcodes, "beq", 0);
    insert_back(&opcodes, "bne", 1);
    insert_back(&opcodes, "bge", 10);
    insert_back(&opcodes, "blt", 11);
    insert_back(&opcodes, "bgt", 12);
    insert_back(&opcodes, "ble", 13);
    insert_back(&opcodes, "bal", 14);
    insert_back(&opcodes, "b", 14);

    /*
    for (tabl_iter iter = tableBegin(&opcodes); iter != tableEnd(&opcodes); iter = iter -> next) {
        printf("%u\n", tableLookup(&opcodes, iter -> key));
    }*/

    /*char instruction[] = "mul r1, r12, r3";
    int32_t ins = assemble_mult(tokenize(instruction));
    printf("%d", ins); */

    symbol_table labels_addresses;
    init_table(&labels_addresses);

    // The first pass over the source file that builds up the symbol table.
    FILE *first_pass = fopen(argv[1], "r");
    int32_t address_counter = 0;
    if (first_pass != NULL)
    {
        char line[MAXC + 1]; // +1 for the null character at the end
        while (fgets(line, sizeof(line), first_pass) != NULL) /* read a line */
        {
            if (line[0] == '\n') {
              continue;
            }
            char **tokens = tokenize(line);
            if (get_instruction_type(tokens) == LABEL) {
                insert_back(&labels_addresses, tokens[0], address_counter);
            }
            else {
                address_counter += 4;
            }

        }
        fclose(first_pass);
    }
    else
    {
        perror(""); /* why didn't the file open? */
    }

    /*
    for (tabl_iter iter = tableBegin(&labelsAddresses); iter != tableEnd(&labelsAddresses); iter = iter -> next) {
        printf("%s %u\n", iter -> key, tableLookup(&labelsAddresses, iter -> key));
    }*/

    // The second pass over the source file, outputs the binary instructions.
    FILE *input = fopen(argv[1], "r");
    FILE *output = fopen(argv[2], "wb");

    if (input != NULL && output != NULL)
    {
        address_counter = 0;
        char line[MAXC + 1]; // +1 for the null character at the end
        while (fgets(line, sizeof(line), input) != NULL)
        {
            if (line[0] == '\n') {
              continue;
            }
            char **tokens = tokenize(line);
            printf("%s\n", tokens[0]);
            instruction_type type = get_instruction_type(tokens);
            if (type == LABEL) {
                continue;
            }
            int32_t instruction = 0;
            switch (type) {
                case DATA_PROCESSING: instruction = assemble_dp(tokens); break;
                case MULTIPLY: instruction = assemble_mult(tokens); break;
                case BRANCH: instruction = assemble_branch(tokens, address_counter, &labels_addresses); break;
                case DATA_TRANSFER: instruction = assemble_dt(tokens, address_counter, input, output); break;
            }
            fwrite(&instruction, sizeof(int32_t), 1, output);
            address_counter += 4;
        }
        fclose(input);
        fclose(output);
    }
    else
    {
        perror("");
    }

    destroy_table(&labels_addresses);
    destroy_table(&opcodes);
}

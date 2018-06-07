#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "symbolTable.h"

#define MAXC 511
#define MAXL 10

// The maximum possible number of tokens in a line.
#define MAX_TOKENS 6

char*** tokenise(const char* fileName);
char** breakDown(char *line, const char *delim);

int main(int argc, char **argv) {
  char *** strings = tokenise(argv[1]);
  for (int i=0; i< sizeof(strings); i++) {
    for (int j=0; j< sizeof(strings[i]); j++) {
      printf("%s", strings[i][j]);
    }
  }

  return EXIT_SUCCESS;
}

void writeToFile(const char* sourceName, const char* destName) {
    FILE* sourceFile = fopen(sourceName, "rb");
    if (sourceFile == NULL) {
        perror("Error opening the source binary file!");
        exit(EXIT_FAILURE);
    }

    FILE* destFile = fopen(destName, "wb");
    if (destFile == NULL) {
        perror("Error opening the destination binary file!");
        exit(EXIT_FAILURE);
    }

    char c = fgetc(sourceFile);
    while (c != EOF) {
        fputc(c, destFile);
        c = fgetc(sourceFile);
    }

    fclose(sourceFile);
    fclose(destFile);
}

char*** tokenise(const char* fileName)
{
    char*** tokens; //[MAXL][][] = NULL;
    char** temp;//[1][MAXC] = NULL; /* pointer to array of type char [MAXC] */
    int i, n = 0;
    FILE *file = fopen(fileName, "rb");

    if (file == NULL) {  /* valdiate file open for reading */
        perror("Error opening the source binary file!\n");
        exit(EXIT_FAILURE);
    }

    if (!(tokens = malloc (MAXL * sizeof *tokens))) { /* allocate MAXL arrays */
        perror("Error: virtual memory exhausted 'tokens'!\n");
        exit(EXIT_FAILURE);
    }

    while (n < MAXL && fgets (temp[0], MAXC, file)) { /* read each line */
        char** lineOfTokens = breakDown(temp[0], ", ");
        tokens[n] = lineOfTokens;

        char **p = tokens[n];                  /* assign pointer */
        for (; **p && **p != '\n'; p++) {
        }                                    /* find 1st '\n'  */
        *p = 0;
        n++;                                 /* nul-terminate  */
    }

    if (file != stdin) {
        fclose(file);
    }

    free(tokens);

}

// Breaks down a line into its constituents tokens -- e.g. mov r1, r2 becomes a string array containing
// mov, r1, r2
char** tokenize(char *line, char* delim) {
    char **array = malloc(sizeof(char *));
    if (array) {
        size_t n = 1;
        char *token = strtok(line, delim);
        while (token) {
            char **tmp = realloc(array, (n + 1) * sizeof(char *));
            if (tmp == NULL) {
                break;
            }
            array = tmp;
            ++n;
            array[n - 2] = malloc(strlen(token) + 1);
            if (array[n - 2] != NULL) {
                strcpy(array[n - 2], token);
            }
            token = strtok(NULL, delim);
        }
        array[n - 1] = NULL;
    }
    return array;
}

typedef enum { DATA_PROCESSING, MULTIPLY, DATA_TRANSFER, BRANCH } instructionType;

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

static int is_label(char *instruction) {
  return instruction[strlen(instruction) - 1] == ':';
}

void remove_colon(char *str) {
  //replaces ':' with EOF
//  str[length - 1] = '\0';
}

void parseArgumentsDP(decodedInstruction d, char** line) {
  d.rd = (int) strtol(line[1]++, (char **)NULL, 10);
  d.rn = (int) strtol(line[2]++, (char **)NULL, 10);
  if (line[3][0] == '#'){
    d.i = 1;
    int base = (line[3][1] == '0' && line[3][1] == 'x') ? 16 : 10;
    d.operand2 = (int) strtol(line[1]++, (char **)NULL, base);
  } else {
    d.i = 0;
    d.operand2 = (int) strtol(line[3]++, (char **)NULL, 10);
  }
}

// Returns a 32-bit value representing the machine code instruction corresponding to the specified
// Data Processing instruction, represented in token form
int32_t assembleDP(char** tokens) {
    uint32_t opcode = tableLookup(&opcodes, tokens[0]);

    uint8_t rd = 0;
    uint8_t rn = 0;
    uint32_t operand2 = 0;
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
            rn = (uint8_t)(strtol(tokens[1] + 1, &ptr, 10)); break;
        case 13:
            op2Position = 2;
            s = 0;
            rd = (uint8_t)(strtol(tokens[1] + 1, &ptr, 10));
            rn = 0; break;
        default:
            op2Position = 3;
            s = 0;
            rd = (uint8_t)(strtol(tokens[1] + 1, &ptr, 10));
            rn = (uint8_t)(strtol(tokens[2] + 1, &ptr, 10)); break;
    }

    if (tokens[op2Position][1] == '0' && tokens[op2Position][2] == 'x') {
        base = 16;
    }

    operand2 = (uint32_t)(strtol(tokens[op2Position] + 1, &ptr, base));

    if (operand2 > 255) {
        printf("Immediate constant cannot be represented using 8 bits!");
        exit(EXIT_FAILURE);
    }

    int32_t instruction = -503316480;

    // Set the S bit
    setBit32(&instruction, 20, s);

    // Set the opcode
    int32_t mask = opcode << 21;
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
    uint8_t rd = (uint8_t)(strtol(tokens[1] + 1, &ptr, 10));
    uint8_t rm = (uint8_t)(strtol(tokens[2] + 1, &ptr, 10));
    uint8_t rs = (uint8_t)(strtol(tokens[3] + 1, &ptr, 10));

    uint8_t rn = 0;
    if (a) {
        rn = (uint8_t)(strtol(tokens[4] + 1, &ptr, 10));
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

decodedInstruction* readTokens(char*** tokens) {
  decodedInstruction* instructions = malloc(sizeof(tokens)*sizeof(decodedInstruction));
  uint32_t address = 0;
  for (int i=0; i<sizeof(tokens); i++) {
    address += 4;
      if (is_label(tokens[i][0])) {
        remove_colon(tokens[i][0]); // TODO: WHAT TO DO IF ITS A LABEL!
      } else {
        decodedInstruction d;
        d.s = 0;
        d.cond = 14;

        if (!strcmp(tokens[i][0], "mov")) {
          d.type = DATA_PROCESSING;
          d.opcode = 13;
          d.rd = (int) strtol(tokens[i][1]++, (char **)NULL, 10);
          if (tokens[i][2][0] == '#'){
            d.i = 1;
            int base = (tokens[i][3][1] == '0' && tokens[i][3][1] == 'x') ? 16 : 10;
            d.operand2 = (int) strtol(tokens[i][1]++, (char **)NULL, base);
          } else {
            d.i = 0;
            d.operand2 = (int) strtol(tokens[i][3]++, (char **)NULL, 10);
          }
        }

        if (!strcmp(tokens[i][0], "and")) {
          d.type = DATA_PROCESSING;
          d.opcode = 0;
        }
        if (!strcmp(tokens[i][0], "eor")) {
          d.type = DATA_PROCESSING;
          d.opcode = 1;
        }
        if (!strcmp(tokens[i][0], "sub")) {
          d.type = DATA_PROCESSING;
          d.opcode = 2;
        }
        if (!strcmp(tokens[i][0], "rsb")) {
          d.type = DATA_PROCESSING;
          d.opcode = 3;
        }
        if (!strcmp(tokens[i][0], "add")) {
          d.type = DATA_PROCESSING;
          d.opcode = 4;
        }
        if (!strcmp(tokens[i][0], "oor")) {
          d.type = DATA_PROCESSING;
          d.opcode = 12;
        }
        if (!strcmp(tokens[i][0], "tst")) {
          d.type = DATA_PROCESSING;
          d.opcode = 8;
          d.s = 1;
        }

        if (!strcmp(tokens[i][0], "teq")) {
          d.type = DATA_PROCESSING;
          d.opcode = 9;
          d.s = 1;
        }

        if (!strcmp(tokens[i][0], "cmp")) {
          d.type = DATA_PROCESSING;
          d.opcode = 10;
          d.s = 1;
        }

        if (d.type == DATA_PROCESSING) {
          parseArgumentsDP(d, tokens[i]);
          instructions[i] = d;

        }




      }
  }
}

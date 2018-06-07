#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

char*** tokenise(const char* fileName);
char** breakDown(char *line, const char *delim);

void printBits(uint32_t x) {
    int i;
    uint32_t mask = 1 << 31;
    for (i=0; i<32; ++i) {
        if ((x & mask) == 0) {
            printf("0");
        }
        else
        {
            printf("1");
        }
        x = x << 1;
    }
    printf("\n");
}


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
    char*** tokens[MAXL] = NULL;
    char** temp[1][MAXC] = NULL; /* pointer to array of type char [MAXC] */
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

        char *p = lines[n];                  /* assign pointer */
        for (; *p && *p != '\n'; p++) {
        }                                    /* find 1st '\n'  */
        *p = 0;
        n++;                                 /* nul-terminate  */
    }

    if (file != stdin) {
        fclose(file);
    }

    free(tokens);
}

char** breakDown(char *line, const char *delim) {
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

void parseArgumentsDP(decodedInstruction d, char** tokens, int i) {
  d.rd = (int) strtol(tokens[i][1]++, (char **)NULL, 10);
  d.rn = (int) strtol(tokens[i][2]++, (char **)NULL, 10);
  if (tokens[i][3][0] == '#'){
    d.i = 1;
    int base = (tokens[i][3][1] == '0' && tokens[i][3][1] == 'x') ? 16 : 10;
    d.operand2 = (int) strtol(tokens[i][1]++, (char **)NULL, base);
  } else {
    d.i = 0;
    d.operand2 = (int) strtol(tokens[i][3]++, (char **)NULL, 10);
  }
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

        if (d.type = DATA_PROCESSING) {
          parseArgumentsDP(d, tokens[i], i);
          instructions[i] = d;

        }




      }
  }
}

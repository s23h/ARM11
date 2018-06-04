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
  char *** stringz = tokenise(argv[1]);
  for (int i=0; i< sizeof(stringz); i++) {
    for (int j=0; j< sizeof(stringz[i]); j++) {
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
    FILE* file = fopen(fileName, "r"); /* should check the result */
    char line[511];

    while (fgets(line, sizeof(line), file)) {
        char** token = breakDown(line, ' ');
    }
    /* may check feof here to make a difference between eof and io failure -- network
       timeout for instance */

    fclose(file);
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

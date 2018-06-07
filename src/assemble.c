#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

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

int main(void)
{
    char** breakDown(char *line, const char *delim);

    char s[] = "Hello World,51,66";
    char **array = breakDown( s, " ," );

    if ( array != NULL )
    {
        for ( char **p = array; *p; ++p ) puts( *p );

        for ( char **p = array; *p; ++p ) free( *p );
        free( array );
    }

    return 0;
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
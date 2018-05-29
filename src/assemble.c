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

int main(int argc, char **argv) {
  return EXIT_SUCCESS;
}

void writeToFile(const char* sourceName, const char* destinationName) {
    FILE* sourceFile = fopen(sourceName, "rb");
    if (sourceFile == NULL) {
        perror("Error opening the source binary file!");
        exit(EXIT_FAILURE);
    }

    FILE* destFile = fopen(destinationName, "wb");
    if (destFile == NULL) {
        perror("Error opening the destination binary file!");
        exit(EXIT_FAILURE);
    }

    char c = fgetc(sourceFile);
    while (c != EOF) {
        fputc(c, destFile);
        fgetc(sourceFile);
    }

    fclose(sourceFile);
    fclose(destFile);
}

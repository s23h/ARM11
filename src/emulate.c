#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#define CHECK_BIT(val, bit_no) (((val) >> (bit_no)) & 1)

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

void loadFile(char* memory, const char* fileName) {
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
}

typedef enum { DATA_PROCESSING, MULTIPLY, DATA_TRANSFER, BRANCH } instructionType;

instructionType getInstructionType(int32_t instruction) {
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

int main(int argc, char **argv) {

    // The main memory of the Raspberry Pi
    char* mainMemory = malloc(65536);
    memset(mainMemory, 0, 65536);

    // Reading the binary code into main memory
    loadFile(mainMemory, argv[1]);

    // Creates the registers
    int32_t* registers = malloc(17 * sizeof(int32_t));
    memset(registers, 0, 17 * sizeof(int32_t));

    int32_t decoded;
    int32_t fetched;

    /*fetched = mainMemory[registers[15]];
    registers[15] += 4;

    decoded =

    while (true) {

    }*/

    /*int32_t* ptr = (int32_t*)mainMemory;
    for (int i = 0; i < 10; i++) {
        printf("%d", getInstructionType(ptr[i]));
        printf(" ");
    }*/


    free(mainMemory);
    free(registers);
    return 0;
}

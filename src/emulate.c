#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#define CHECK_BIT(val, bit_no) (((val) >> (bit_no)) & 1)

/*
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
}*/

void loadFile(uint8_t* memory, const char* fileName) {
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

instructionType getInstructionType(uint32_t instruction) {
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

// k is number of bits to extract, p is the starting position (starts from 1, which is the right most/least significant bit)
uint32_t extractBits(uint32_t number, int k, int p)
{
    return (((1 << k) - 1) & (number >> (p - 1)));
}


// Represents a data processing instruction
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
    uint32_t offset;
} decodedInstruction;

// Decodes data processing instructions
decodedInstruction decodeDP(uint32_t instruction) {
    decodedInstruction decoded;
    decoded.type = DATA_PROCESSING;
    uint32_t result = extractBits(instruction, 4, 29);
    decoded.cond = ((uint8_t*)(&result))[0];
    result = extractBits(instruction, 1, 26);
    decoded.i = ((uint8_t*)(&result))[0];
    result = extractBits(instruction, 4, 22);
    decoded.opcode = ((uint8_t*)(&result))[0];
    result = extractBits(instruction, 1, 21);
    decoded.s = ((uint8_t*)(&result))[0];
    result = extractBits(instruction, 4, 17);
    decoded.rn = ((uint8_t*)(&result))[0];
    result = extractBits(instruction, 4, 13);
    decoded.rd = ((uint8_t*)(&result))[0];
    result = extractBits(instruction, 12, 1);
    decoded.operand2 = ((uint16_t*)(&result))[0];
    return decoded;
}

uint8_t checkCondition(uint8_t cond, uint32_t* registers) {
    if (cond == 14) {
        return 1;
    }
    else if (cond == 0) {
        return CHECK_BIT(registers[16], 30);
    }
    else if (cond == 1) {
        return !CHECK_BIT(registers[16], 30);
    }
    else if (cond == 10) {
        return (CHECK_BIT(registers[16], 31) && CHECK_BIT(registers[16], 28))
                            || (!CHECK_BIT(registers[16], 31) && !CHECK_BIT(registers[16], 28));
    }
    else if (cond == 11) {
        return (CHECK_BIT(registers[16], 31) && !CHECK_BIT(registers[16], 28))
                            || (!CHECK_BIT(registers[16], 31) && CHECK_BIT(registers[16], 28));

    }
    else if (cond == 12) {
        return !CHECK_BIT(registers[16], 30) && ((CHECK_BIT(registers[16], 31) && CHECK_BIT(registers[16], 28))
                            || (!CHECK_BIT(registers[16], 31) && !CHECK_BIT(registers[16], 28)));
    }
    else if (cond == 13) {
        return CHECK_BIT(registers[16], 30) || ((CHECK_BIT(registers[16], 31) && !CHECK_BIT(registers[16], 28))
                            || (!CHECK_BIT(registers[16], 31) && CHECK_BIT(registers[16], 28)));
    }
    // should be unreachable
    return 0;
}

void executeDP(decodedInstruction decoded) {

}

int main(int argc, char **argv) {

    // The main memory of the Raspberry Pi
    uint8_t* mainMemory = malloc(65536);
    memset(mainMemory, 0, 65536);

    // Reading the binary code into main memory
    loadFile(mainMemory, argv[1]);

    // Creates the registers
    uint32_t* registers = malloc(17 * sizeof(uint32_t));
    memset(registers, 0, 17 * sizeof(uint32_t));

    uint32_t decoded;
    uint32_t fetched;

    /*fetched = mainMemory[registers[15]];
    registers[15] += 4;

    decoded =

    while (true) {

    }*/

    /*uint32_t* ptr = (uint32_t*)mainMemory;
    for (int i = 0; i < 10; i++) {
        printf("%d", getInstructionType(ptr[i]));
        printf(" ");
    }*/


    /*uint32_t result = extractBits(4194303999, 12, 21);
    printf("%d", result);
    uint16_t bits = ((uint16_t*)(&result))[0];
    printf("%d", bits);*/

    /*for (int i = 0; i < 8; i++) {
        uint8_t j = mainMemory[i];
        printf("%u", j);
        printf(" ");
    }*/

    uint32_t instruction = ((uint32_t*)(mainMemory))[1];
    decodedInstruction jesus = decodeDP(instruction);
    printf("%u", jesus.operand2);


    free(mainMemory);
    free(registers);
    return 0;
}

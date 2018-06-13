#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "utilities.h"
#include "decodedInstruction.h"
#include "executeInstruction.h"

#define RAM_SIZE 65536

// Loads the provided file into main memory, and returns the size of the file.
int loadFile(uint8_t* memory, const char* fileName) {
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
    return size;
}
  //=========================================================================

int main(int argc, char **argv) {
    // The main memory of the Raspberry Pi
    uint8_t* mainMemory = malloc(RAM_SIZE);
    memset(mainMemory, 0, RAM_SIZE);

    // Reading the binary code into main memory
    int size = loadFile(mainMemory, argv[1]);

    // Creates the registers
    int32_t* registers = malloc(17 * sizeof(int32_t));
    memset(registers, 0, 17 * sizeof(int32_t));

    // The three stage pipeline
    decodedInstruction decoded;
    int32_t fetched;

    int decodeAvailable = 0;
    int fetchAvailable = 0;
    int halt = 0;

    while (1) {
        // The execution stage of the pipeline.
        if (decodeAvailable) {
            if (halt) {
                break;
            }
            switch (decoded.type) {
                    case DATA_PROCESSING : executeDP(decoded, registers); break;
                    case MULTIPLY : executeMultiply(decoded, registers); break;
                    case DATA_TRANSFER : executeDT(decoded, registers, mainMemory); break;
                    case BRANCH : if (executeBranch(decoded, registers)) { decodeAvailable = 0; fetchAvailable = 0; } break;
                    // should be unreachable
                    default : break;
            }
        }

        // The decode stage of the pipeline.
        if (fetchAvailable) {
            decodeAvailable = 1;
            if (fetched == 0) {
                halt = 1;
            }
            else {
                switch (getInstructionType(fetched)) {
                    case DATA_PROCESSING : decoded = decodeDP(fetched); break;
                    case MULTIPLY : decoded = decodeMultiply(fetched); break;
                    case DATA_TRANSFER : decoded = decodeDT(fetched); break;
                    case BRANCH : decoded = decodeBranch(fetched); break;
                    // should be unreachable
                    default : break;
                }
            }
        }
        fetched = ((int32_t*)(&mainMemory[registers[15]]))[0];
        fetchAvailable = 1;
        registers[15] += 4;
    }

    printf("Registers:\n");
    for (int i = 0; i < 13; i++) {
        printf("$%d\t:\t", i);
        printf("%d\t(0x%08x)\n", registers[i], registers[i]);
    }

    printf("PC\t:\t%d\t(0x%08x)\n", registers[15], registers[15]);
    printf("CPSR\t:\t%d\t(0x%08x)\n", registers[16], registers[16]);

    printf("Non-zero memory:\n");

    for (int i = 0; i < RAM_SIZE; i += 4) {
      if (((int32_t*)(mainMemory))[i / 4] != 0) {
          printf("0x%08x : 0x%02x%02x%02x%02x\n", i, mainMemory[i], mainMemory[i + 1],
          mainMemory[i + 2], mainMemory[i + 3]);
      }
    }

    free(mainMemory);
    free(registers);
    return 0;
  }

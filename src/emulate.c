#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "utilities.h"
#include "emulate_utils/decodedInstruction.h"
#include "emulate_utils/executeInstruction.h"

// Loads the provided file into main memory.
void load_File(uint8_t* memory, const char* file_Name) {
    FILE* binary_File = fopen(file_Name, "rb");
    if (binary_File == NULL) {
        perror("load_File: ");
        exit(EXIT_FAILURE);
    }

    // Works out the size, in bytes, of the input file
    fseek(binary_File, 0, SEEK_END);
    int size = ftell(binary_File);
    fseek(binary_File, 0, SEEK_SET);

    // Reads the bytes in the file into main memory
    fread(memory, 1, size, binary_File);

    fclose(binary_File);
}

int main(int argc, char** argv) {
    // The main memory of the Raspberry Pi
    uint8_t* main_Memory = malloc(RAM_SIZE);
    memset(main_Memory, 0, RAM_SIZE);

    // Reading the binary code into main memory
    load_File(main_Memory, argv[1]);

    // Creates the registers
    int32_t* registers = malloc(17 * sizeof(int32_t));
    memset(registers, 0, 17 * sizeof(int32_t));

    // The three stage pipeline
    decoded_Instruction decoded;
    int32_t fetched;

    int decode_Available = 0;
    int fetch_Available = 0;
    int halt = 0;

    while (1) {
        // The execution stage of the pipeline.
        if (decode_Available) {
            if (halt) {
                break;
            }
            switch (decoded.type) {
                case DATA_PROCESSING : execute_DP(decoded, registers); break;
                case MULTIPLY : execute_Multiply(decoded, registers); break;
                case DATA_TRANSFER : execute_DT(decoded, registers, main_Memory); break;
                case BRANCH : if (execute_Branch(decoded, registers)) { decode_Available = 0; fetch_Available = 0; } break;
                // should be unreachable
                default : break;
            }
        }

        // The decode stage of the pipeline.
        if (fetch_Available) {
            decode_Available = 1;

            if (fetched == 0) {
                halt = 1;
            }

            else {
                switch (get_Instruction_Type(fetched)) {
                    case DATA_PROCESSING : decoded = decode_DP(fetched);
                    break;
                    case MULTIPLY : decoded = decode_Multiply(fetched);
                    break;
                    case DATA_TRANSFER : decoded = decode_DT(fetched);
                    break;
                    case BRANCH : decoded = decode_Branch(fetched);
                    break;
                    // should be unreachable
                    default : break;
                }
            }
        }
        fetched = ((int32_t*)(&main_Memory[registers[15]]))[0];
        fetch_Available = 1;
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
        if (((int32_t*)(main_Memory))[i / 4] != 0) {
            printf("0x%08x : 0x%02x%02x%02x%02x\n", i, main_Memory[i], main_Memory[i + 1],
            main_Memory[i + 2], main_Memory[i + 3]);
        }
    }

    free(main_Memory);
    free(registers);
    return 0;
  }

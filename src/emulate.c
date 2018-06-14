#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "utilities.h"
#include "emulate_utils/decoded_instruction.h"
#include "emulate_utils/execute_instruction.h"

// Loads the provided file into main memory.
void load_file(uint8_t *memory, const char *file_name) {
    FILE *binary_file = fopen(file_name, "rb");
    if (binary_file == NULL) {
        perror("load_file: ");
        exit(EXIT_FAILURE);
    }

    // Works out the size, in bytes, of the input file
    fseek(binary_file, 0, SEEK_END);
    int size = ftell(binary_file);
    fseek(binary_file, 0, SEEK_SET);

    // Reads the bytes in the file into main memory
    fread(memory, 1, size, binary_file);

    fclose(binary_file);
}

int main(int argc, char **argv) {
    // The main memory of the Raspberry Pi
    uint8_t *main_memory = malloc(RAM_SIZE);
    memset(main_memory, 0, RAM_SIZE);

    // Reading the binary code into main memory
    load_file(main_memory, argv[1]);

    // Creates the registers
    int32_t *registers = malloc(17 * sizeof(int32_t));
    memset(registers, 0, 17 * sizeof(int32_t));

    // The three stage pipeline
    decoded_instruction decoded;
    int32_t fetched;

    int decode_available = 0;
    int fetch_available = 0;
    int halt = 0;

    while (1) {
        // The execution stage of the pipeline.
        if (decode_available) {
            if (halt) {
                break;
            }
            switch (decoded.type) {
                case DATA_PROCESSING :
                    execute_dp(decoded, registers); break;
                case MULTIPLY :
                    execute_multiply(decoded, registers); break;
                case DATA_TRANSFER :
                    execute_dt(decoded, registers, main_memory); break;
                case BRANCH : if (execute_branch(decoded, registers)) { decode_available = 0; fetch_available = 0; } break;
                // should be unreachable
                default : break;
            }
        }

        // The decode stage of the pipeline.
        if (fetch_available) {
            decode_available = 1;

            if (fetched == 0) {
                halt = 1;
            }

            else {
                switch (get_instruction_type(fetched)) {
                    case DATA_PROCESSING : decoded = decode_dp(fetched);
                    break;
                    case MULTIPLY : decoded = decode_multiply(fetched);
                    break;
                    case DATA_TRANSFER : decoded = decode_dt(fetched);
                    break;
                    case BRANCH : decoded = decode_branch(fetched);
                    break;
                    // should be unreachable
                    default : break;
                }
            }
        }
        fetched = ((int32_t*)(&main_memory[registers[15]]))[0];
        fetch_available = 1;
        registers[15] += 4;
    }

    printf("Registers:\n");
    for (int i = 0; i <= 9; i++) {
        printf("$%d  :", i);
        printf("%11d (0x%08x)\n", registers[i], registers[i]);
    }
    for (int i = 10; i <= 12; i++) {
        printf("$%d :", i);
        printf("%11d (0x%08x)\n", registers[i], registers[i]);
    }

    printf("PC  :%11d (0x%08x)\n", registers[15], registers[15]);
    printf("CPSR:%11d (0x%08x)\n", registers[16], registers[16]);

    printf("Non-zero memory:\n");

    for (int i = 0; i < RAM_SIZE; i += 4) {
        if (((int32_t*)(main_memory))[i / 4] != 0) {
            printf("0x%08x: 0x%02x%02x%02x%02x\n", i, main_memory[i], main_memory[i + 1],
            main_memory[i + 2], main_memory[i + 3]);
        }
    }

    free(main_memory);
    free(registers);
    return 0;
  }

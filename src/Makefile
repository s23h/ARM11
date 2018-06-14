CC      = gcc
CFLAGS  = -Wall -g -D_POSIX_SOURCE -D_BSD_SOURCE -std=c99 -Werror -pedantic

.SUFFIXES: .c .o

.PHONY: all clean

all: assemble emulate

assemble: assemble.o utilities.o assemble_utils/symbol_table.o assemble_utils/assemble_instruction.o

emulate: emulate.o emulate_utils/decoded_instruction.o emulate_utils/execute_instruction.o utilities.o

decoded_instruction.o: decoded_instruction.h utilities.h

executeInstruction.o: execute_instruction.h utilities.h

symbolTable.o: symbol_table.h

assembleInstruction.o: assemble_instruction.h

utilities.o: utilities.h


clean:
	rm -f $(wildcard *.o)
	rm -f assemble
	rm -f emulate

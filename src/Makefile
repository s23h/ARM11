CC      = gcc
CFLAGS  = -Wall -g -D_POSIX_SOURCE -D_BSD_SOURCE -std=c99 -Werror -pedantic

.SUFFIXES: .c .o

.PHONY: all clean

all: assemble emulate

assemble: assemble.o utilities.o assemble_utils/symbolTable.o assemble_utils/assembleInstruction.o

emulate: emulate.o emulate_utils/decoded_Instruction.o emulate_utils/executeInstruction.o utilities.o

decoded_Instruction.o: decoded_Instruction.h utilities.h

executeInstruction.o: executeInstruction.h utilities.h

symbolTable.o: symbolTable.h

assembleInstruction.o: assembleInstruction.h

utilities.o: utilities.h


clean:
	rm -f $(wildcard *.o)
	rm -f assemble
	rm -f emulate

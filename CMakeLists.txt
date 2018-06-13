cmake_minimum_required(VERSION 3.10)
project(arm11_34)

set(CMAKE_CXX_STANDARD 11)

include_directories(src)
include_directories(src/assemble_utils)
include_directories(src/emulate_utils)

add_executable(arm11_34
        doc/Checkpoint.tex
        doc/Makefile
        doc/Report.tex
        programs/gpio.s
        src/assemble_utils/assembleInstruction.c
        src/assemble_utils/assembleInstruction.h
        src/assemble_utils/symbolTable.c
        src/assemble_utils/symbolTable.h
        src/emulate_utils/decodedInstruction.c
        src/emulate_utils/decodedInstruction.h
        src/emulate_utils/executeInstruction.c
        src/emulate_utils/executeInstruction.h
        src/assemble.c
        src/emulate.c
        src/test.c
        src/utilities.c
        src/utilities.h)

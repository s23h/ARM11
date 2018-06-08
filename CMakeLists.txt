cmake_minimum_required(VERSION 3.10)
project(arm11_34)

set(CMAKE_CXX_STANDARD 11)

include_directories(src)

add_executable(arm11_34
        cmake-build-debug/arm11_34.cbp
        cmake-build-debug/cmake_install.cmake
        cmake-build-debug/CMakeCache.txt
        cmake-build-debug/Makefile
        doc/Checkpoint.tex
        doc/Makefile
        doc/Report.tex
        programs/gpio.s
        src/assemble.c
        src/emulate.c
        src/Makefile
        src/symbolTable.c
        src/symbolTable.h
        src/utilities.c
        src/utilities.h)

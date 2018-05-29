cmake_minimum_required(VERSION 3.10)
project(arm11_34 C)

set(CMAKE_C_STANDARD 11)

add_executable(arm11_34
        src/assemble.c
        src/emulate.c)

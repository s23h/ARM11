#include "utilities.h"
#include <stdint.h>

// k is number of bits to extract, p is the starting position (starts from 1, which is the right most/least significant bit).
int32_t extract_Bits(int32_t number, int k, int p)
{
    return (((1 << k) - 1) & (number >> (p - 1)));
}

int32_t left_Rotate(int32_t n, int d) {
    return (n << d) | ((n >> (32 - d)) & ~(-1 << d));
}

// Rotates bits to the right by the specified amount.
int32_t right_Rotate(int32_t n, int d) {
    return (n >> d)| (n << (32 - d));
}

// Power function designed specifically for integers.
int32_t int_Pow(int32_t x, int n) {
    int32_t number = 1;

    for (int i = 0; i < n; ++i)
        number *= x;

    return(number);
}

// Takes a 32-bit word and sets the bit at the given position.
void set_Bit_32(int32_t* word, int pos, int val) {
    if (val == 0) {
        uint32_t mask = ~int_Pow(2, pos);
        *word = *word & mask;
    }
    else if (val == 1) {
        uint32_t mask = int_Pow(2, pos);
        *word = *word | mask;
    }
}

// Takes a 8-bit word and sets the bit at the desired position.
void set_Bit_8(int8_t* rest, int pos, int val) {
    if (val == 0) {
        uint8_t mask = ~int_Pow(2, pos);
        *rest = *rest & mask;
    }
    else if (val == 1) {
        uint8_t mask = int_Pow(2, pos);
        *rest = *rest | mask;
    }
}

// Takes a 24-bit number and sign extends to 32-bit
int32_t sign_Extend_24_32(int32_t x) {
    const int bits = 24;
    int32_t m = 1u << (bits - 1);
    return (x ^ m) - m;
}

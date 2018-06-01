#include "utilities.h"
#include <stdint.h>

// k is number of bits to extract, p is the starting position (starts from 1, which is the right most/least significant bit).
uint32_t extractBits(uint32_t number, int k, int p)
{
    return (((1 << k) - 1) & (number >> (p - 1)));
}

// Same as above, but extracts the bits from a 64-bit number, instead of a 32-bit.
uint32_t extractBitsFrom64(uint64_t number, int k, int p)
{
    return (((1 << k) - 1) & (number >> (p - 1)));
}

// Rotates bits to the right by the specified amount.
uint32_t rightRotate(uint32_t n, unsigned int d) {
   return (n >> d)|(n << (32 - d));
}

// Power function designed specifically for integers.
uint32_t intPow(uint32_t x,int n) {
    uint32_t number = 1;

    for (int i = 0; i < n; ++i)
        number *= x;

    return(number);
}

// Takes a 32-bit word and sets the bit at the given position.
void setBit32(uint32_t* word, int pos, int val) {
    if (val == 0) {
        uint32_t mask = ~intPow(2, pos);
        *word = *word & mask;
    }
    else if (val == 1) {
        uint32_t mask = intPow(2, pos);
        *word = *word | mask;
    }
}

// Takes a 8-bit word and sets the bit at the desired position.
void setBit8(uint8_t* rest, int pos, int val) {
    if (val == 0) {
        uint8_t mask = ~intPow(2, pos);
        *rest = *rest & mask;
    }
    else if (val == 1) {
        uint8_t mask = intPow(2, pos);
        *rest = *rest | mask;
    }
}

// Takes a 24-bit number and sign extends to 32-bit
uint32_t sign_extend_24_32(uint32_t x) {
    const int bits = 24;
    uint32_t m = 1u << (bits - 1);
    return (x ^ m) - m;
}

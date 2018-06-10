#ifndef UTILITIES_H
#define UTILITIES_H

#include <stdint.h>

#define CHECK_BIT(val, bit_no) (((val) >> (bit_no)) & 1)

// k is number of bits to extract, p is the starting position (starts from 1, which is the right most/least significant bit).
uint32_t extractBits(uint32_t number, int k, int p);

// Same as above, but extracts the bits from a 64-bit number, instead of a 32-bit.
uint32_t extractBitsFrom64(uint64_t number, int k, int p);

// Rotates bits to the left by the specified amount.
uint32_t leftRotate(uint32_t n, int d);

// Rotates bits to the right by the specified amount.
uint32_t rightRotate(uint32_t n, int d);

// Power function designed specifically for integers.
uint32_t intPow(uint32_t x,int n);

// Takes a 32-bit word and sets the bit at the given position.
void setBit32(uint32_t* word, int pos, int val);

// Takes a 8-bit word and sets the bit at the desired position.
void setBit8(uint8_t* rest, int pos, int val);

// Takes a 24-bit number and sign extends it to 32-bit.
uint32_t sign_extend_24_32(uint32_t x);

#endif

#ifndef UTILITIES_H
#define UTILITIES_H

#include <stdint.h>

#define CHECK_BIT(val, bit_no) (((val) >> (bit_no)) & 1)

// k is number of bits to extract, p is the starting position (starts from 1, which is the right most/least significant bit).
int32_t extractBits(int32_t number, int k, int p);

int32_t leftRotate(int32_t n, int d);

// Rotates bits to the right by the specified amount.
int32_t rightRotate(int32_t n, int d);

// Power function designed specifically for integers.
int32_t intPow(int32_t x,int n);

// Takes a 32-bit word and sets the bit at the given position.
void setBit32(int32_t* word, int pos, int val);

// Takes a 8-bit word and sets the bit at the desired position.
void setBit8(int8_t* rest, int pos, int val);

// Takes a 24-bit number and sign extends to 32-bit
int32_t sign_extend_24_32(int32_t x);

#endif

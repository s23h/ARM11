#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include <stdint.h>

// This file defines a symbol table ADT, based on the implementation of the doubly linked list from the C lectures.

// Represents a string-int pair in the symbol table
struct keyValuePair {
    char* key;
    uint32_t value;
    struct keyValuePair* next;
    struct keyValuePair* prev;
};

typedef struct keyValuePair pair;

typedef struct {
    pair *header;
    pair *footer;
} symbolTable;

// Allocates memory for a key-value pair
pair* allocatePair(void);

// Frees the memory that holds the specified pair
void freePair(pair* pairToFree);

// Initialises a new symbol table
void initTable(symbolTable* t);

typedef pair* tableIter;

// Returns a iterator pointing to the beginning of the list.
tableIter tableBegin(symbolTable* t);

// Returns a iterator pointing to one past the end of the list.
tableIter tableEnd(symbolTable *t);

// Inserts a new pair before the current iterator.
void tableInsert(symbolTable* t, tableIter iter, char* key, int32_t value);

// Inserts a new pair at the front of the list.
void insertFront(symbolTable* t, char* key, int32_t value);

// Inserts a new pair at the back of the list.
void insertBack(symbolTable* t, char* key, int32_t value);

void destroyTable(symbolTable* t);

int32_t tableLookup(symbolTable* t, char* key);

#endif

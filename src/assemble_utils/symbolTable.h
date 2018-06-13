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
} symbol_Table;

// Allocates memory for a key-value pair
pair* allocate_Pair(void);

// Frees the memory that holds the specified pair
void free_Pair(pair* pairToFree);

// Initialises a new symbol table
void initTable(symbol_Table* t);

typedef pair* tableIter;

// Returns a iterator pointing to the beginning of the list.
tableIter table_Begin(symbol_Table* t);

// Returns a iterator pointing to one past the end of the list.
tableIter table_End(symbol_Table* t);

// Inserts a new pair before the current iterator.
void table_Insert(symbol_Table* t, tableIter iter, char* key, int32_t value);

// Inserts a new pair at the front of the list.
void insert_Front(symbol_Table* t, char* key, int32_t value);

// Inserts a new pair at the back of the list.
void insert_Back(symbol_Table* t, char* key, int32_t value);

void destroy_Table(symbol_Table* t);

int32_t table_Lookup(symbol_Table* t, char* key);

#endif

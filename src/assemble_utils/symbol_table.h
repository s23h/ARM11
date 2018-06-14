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
} symbol_table;

// Allocates memory for a key-value pair
pair* allocate_pair(void);

// Frees the memory that holds the specified pair
void free_pair(pair *pair_to_free);

// Initialises a new symbol table
void init_table(symbol_table *t);

typedef pair *tabl_iter;

// Returns a iterator pointing to the beginning of the list.
tabl_iter table_begin(symbol_table *t);

// Returns a iterator pointing to one past the end of the list.
tabl_iter table_end(symbol_table *t);

// Inserts a new pair before the current iterator.
void table_insert(symbol_table *t, tabl_iter iter, char *key, int32_t value);

// Inserts a new pair at the front of the list.
void insert_front(symbol_table *t, char *key, int32_t value);

// Inserts a new pair at the back of the list.
void insert_back(symbol_table *t, char *key, int32_t value);

void destroy_table(symbol_table *t);

int32_t table_lookup(symbol_table *t, char *key);

#endif

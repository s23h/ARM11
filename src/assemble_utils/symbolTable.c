#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "symbolTable.h"

// This file defines a symbol table ADT, based on the implementation of the doubly linked list from the C lectures.

// Allocates memory for a key-value pair
pair* allocate_Pair(void) {
    pair* newPair = malloc(sizeof(pair));

    if (newPair == NULL) {
        perror("allocate_Pair");
        exit(EXIT_FAILURE);
    }

    return newPair;
}

// Frees the memory that holds the specified pair
void free_Pair(pair* pairToFree) {
    free(pairToFree);
}

// Initialises a new symbol table
void init_Table(symbol_Table* t) {
    t -> header = allocate_Pair();
    t -> footer = allocate_Pair();
    t -> header -> prev = NULL;
    t -> footer -> next = NULL;
    t -> header -> next = t -> footer;
    t -> footer -> prev = t -> header;
}

// Returns a iterator pointing to the beginning of the list.
tableIter table_Begin(symbol_Table* t) {
    return t -> header -> next;
}

// Returns a iterator pointing to one past the end of the list.
tableIter table_End(symbol_Table* t) {
    return t -> footer;
}

// Inserts a new pair before the current iterator.
void table_Insert(symbol_Table* t, tableIter iter, char* key, int32_t value) {
    pair* newPair = allocate_Pair();
    newPair -> value = value;
    newPair -> key = malloc((strlen(key) + 1) * sizeof(char));
    strcpy(newPair -> key, key);

    newPair -> prev = iter -> prev;
    newPair -> next = iter;

    iter -> prev -> next = newPair;
    iter -> prev = newPair;
}

// Inserts a new pair at the front of the list.
void insert_Front(symbol_Table* t, char* key, int32_t value) {
    table_Insert(t, table_Begin(t), key, value);
}

// Inserts a new pair at the back of the list.
void insert_Back(symbol_Table* t, char* key, int32_t value) {
    table_Insert(t, table_End(t), key, value);
}

void destroy_Table(symbol_Table* t) {
    pair* p = t -> header;
    while (p != NULL) {
        pair* next = p -> next;
        free_Pair(p);
        p = next;
    }
}

// Looks up the value for the given symbol, assumes that all keys being looked up are present in the table.
int32_t table_Lookup(symbol_Table* t, char* key) {
    for (tableIter iter = table_Begin(t); iter != table_End(t); iter = iter -> next) {
        if (strcmp(iter -> key, key) == 0) {
            return iter -> value;
        }
    }
    // Should be unreachable.
    return 0;
}

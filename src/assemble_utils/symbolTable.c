#include "symbolTable.h"
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// This file defines a symbol table ADT, based on the implementation of the doubly linked list from the C lectures.

// Allocates memory for a key-value pair
pair* allocatePair(void) {
    pair* newPair = malloc(sizeof(pair));
    if (newPair == NULL) {
        perror("allocatePair");
        exit(EXIT_FAILURE);
    }

    return newPair;
}

// Frees the memory that holds the specified pair
void freePair(pair* pairToFree) {
    free(pairToFree);
}

// Initialises a new symbol table
void initTable(symbolTable* t) {
    t -> header = allocatePair();
    t -> footer = allocatePair();
    t -> header -> prev = NULL;
    t -> footer -> next = NULL;
    t -> header -> next = t -> footer;
    t -> footer -> prev = t -> header;
}

// Returns a iterator pointing to the beginning of the list.
tableIter tableBegin(symbolTable* t) {
    return t -> header -> next;
}

// Returns a iterator pointing to one past the end of the list.
tableIter tableEnd(symbolTable *t) {
    return t -> footer;
}

// Inserts a new pair before the current iterator.
void tableInsert(symbolTable* t, tableIter iter, char* key, int32_t value) {
    pair* newPair = allocatePair();
    newPair -> value = value;
    newPair -> key = malloc((strlen(key) + 1) * sizeof(char));
    strcpy(newPair -> key, key);

    newPair -> prev = iter -> prev;
    newPair -> next = iter;

    iter -> prev -> next = newPair;
    iter -> prev = newPair;
}

// Inserts a new pair at the front of the list.
void insertFront(symbolTable* t, char* key, int32_t value) {
    tableInsert(t, tableBegin(t), key, value);
}

// Inserts a new pair at the back of the list.
void insertBack(symbolTable* t, char* key, int32_t value) {
    tableInsert(t, tableEnd(t), key, value);
}

void destroyTable(symbolTable* t) {
    pair* p = t -> header;
    while (p != NULL) {
        pair* next = p -> next;
        freePair(p);
        p = next;
    }
}

// Looks up the value for the given symbol, assumes that all keys being looked up are present in the table.
int32_t tableLookup(symbolTable* t, char* key) {
    for (tableIter iter = tableBegin(t); iter != tableEnd(t); iter = iter -> next) {
        if (strcmp(iter -> key, key) == 0) {
            return iter -> value;
        }
    }
    // Should be unreachable.
    return 0;
}

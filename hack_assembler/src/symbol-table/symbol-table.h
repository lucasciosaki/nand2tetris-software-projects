#ifndef _SYMBOL_TABLE_H
#define _SYMBOL_TABLE_H

// Symbol table structure definition.
typedef struct _symboltable SymbolTable;

// Adds a symbol and its memory address to the table.
void symboltable_addEntry(SymbolTable *st, char *symbol, int address);

// Checks if a symbol exists in the table.
bool symboltable_contains(SymbolTable *st, char *symbol);

// Returns the memory address of a symbol, or -1 if not found.
int symboltable_getAddress(SymbolTable *st, char *symbol);

// Creates and initializes a symbol table with predefined symbols.
SymbolTable *symboltable_create();

// Frees all memory allocated for the symbol table and sets pointer to NULL.
void symboltable_delete(SymbolTable **st_ref);  

#endif
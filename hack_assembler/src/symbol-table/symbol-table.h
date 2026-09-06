#ifndef _SYMBOL_TABLE_H
#define _SYMBOL_TABLE_H
typedef struct _symboltable SymbolTable;

void symboltable_addEntry(SymbolTable *st, char *symbol, int address);

bool symboltable_contains(SymbolTable *st, char *symbol);

int symboltable_getAddress(SymbolTable *st, char *symbol);

SymbolTable *symboltable_create();

void symboltable_delete(SymbolTable **st_ref);  

#endif
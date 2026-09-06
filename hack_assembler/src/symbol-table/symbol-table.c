#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define BUCKET_COUNT 1024

// Linked list node for handling hash collisions.
typedef struct _node{
    char *symbol;
    int address;
    struct _node *next;
} Node;

// Hash table with separate chaining buckets.
typedef struct _symboltable{
    Node *buckets[BUCKET_COUNT];
} SymbolTable;

// djb2 hash function mapping string to bucket index.
static unsigned int _hash (const char *str){
    unsigned long hash = 5381;
    int c;
    while((c = *str++)){
        hash = ((hash << 5) + hash) + c;
    }
    return hash % BUCKET_COUNT;
}

// Inserts a new symbol-address pair into the table.
void symboltable_addEntry(SymbolTable *st, char *symbol, int address){
    if(!st || !symbol) return;

    Node *node = (Node *) malloc(sizeof(Node));
    if(!node) return;

    char *new_symbol = (char *) malloc((strlen(symbol) + 1) * sizeof(char));
    strcpy(new_symbol, symbol);
    node->symbol = new_symbol;
    node->address = address;
    node->next = NULL;

    unsigned int index = _hash(symbol);
    if(st->buckets[index] != NULL){
        node->next = st->buckets[index];
    }
    st->buckets[index] = node;
    return;
}

// Checks if a symbol already exists in the table.
bool symboltable_contains(SymbolTable *st, char *symbol){
    if(!st || !symbol) return false;

    unsigned int index = _hash(symbol);
    Node *node = st->buckets[index];
    while(node != NULL){
        if(strcmp(symbol, node->symbol) == 0) return true;
        node = node->next;
    }
    return false;
}

// Retrieves the address associated with a symbol, or -1 if missing.
int symboltable_getAddress(SymbolTable *st, char *symbol){
    if(!st || !symbol) return -1;

    unsigned int index = _hash(symbol);
    Node *node = st->buckets[index];
    while(node != NULL){
        if(strcmp(symbol, node->symbol) == 0) return node->address;
        node = node->next;
    }
    return -1;
}

// Allocates table and pre-populates predefined Hack symbols.
SymbolTable *symboltable_create(){
    SymbolTable *st = (SymbolTable *) malloc (sizeof(SymbolTable));
    if(!st){
        printf("ERROR: Unable to alloc Symbol Table");
        return NULL;
    }
    for(int i = 0; i < BUCKET_COUNT; i++){
        st->buckets[i] = NULL;
    }

    symboltable_addEntry(st, "SP", 0);
    symboltable_addEntry(st, "LCL", 1);
    symboltable_addEntry(st, "ARG", 2);
    symboltable_addEntry(st, "THIS", 3);
    symboltable_addEntry(st, "THAT", 4);
    symboltable_addEntry(st, "R0", 0);
    symboltable_addEntry(st, "R1", 1);
    symboltable_addEntry(st, "R2", 2);
    symboltable_addEntry(st, "R3", 3);
    symboltable_addEntry(st, "R4", 4);
    symboltable_addEntry(st, "R5", 5);
    symboltable_addEntry(st, "R6", 6);
    symboltable_addEntry(st, "R7", 7);
    symboltable_addEntry(st, "R8", 8);
    symboltable_addEntry(st, "R9", 9);
    symboltable_addEntry(st, "R10", 10);
    symboltable_addEntry(st, "R11", 11);
    symboltable_addEntry(st, "R12", 12);
    symboltable_addEntry(st, "R13", 13);
    symboltable_addEntry(st, "R14", 14);
    symboltable_addEntry(st, "R15", 15);
    symboltable_addEntry(st, "SCREEN", 16384);
    symboltable_addEntry(st, "KBD", 24576);

    return st;
}

// Deallocates all table nodes, strings, and the table itself.
void symboltable_delete(SymbolTable **st_ref) {              
        if (!st_ref || !(*st_ref)) return;                       
                                                                 
        SymbolTable *st = *st_ref;                               
                                                                                       
        for (int i = 0; i < BUCKET_COUNT; i++) {                 
            Node *current = st->buckets[i];                      
                                                                
            while (current != NULL) {                            
                Node *temp = current;
                current = current->next;
                 
                if (temp->symbol != NULL) {
                    free(temp->symbol);
                }

                free(temp);
            }
        }      
        free(st);
        *st_ref = NULL;
    }
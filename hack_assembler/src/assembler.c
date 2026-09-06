#include <stdio.h>
#include "./parser/parser.h"
#include "./code/code.h"
#include <string.h>
#include "./symbol-table/symbol-table.h"
#include <ctype.h>


// Creates and opens a .hack output file based on the assembly input filename.
FILE *create_hack_file(const char *asm_filename) {
    char hack_filename[256];
    
    
    strncpy(hack_filename, asm_filename, sizeof(hack_filename) - 1);
    hack_filename[sizeof(hack_filename) - 1] = '\0';

         
    char *dot = strrchr(hack_filename, '.');
    if (dot != NULL) {
        strcpy(dot, ".hack");
    } else {
        strcat(hack_filename, ".hack");
    }

    return fopen(hack_filename, "w");
}

// Converts a non-negative integer into a 16-bit binary ASCII string.
void int_to_bin16(int value, char *dest) {                        
        for (int i = 15; i >= 0; i--) {                                                                                               
            dest[15 - i] = ((value >> i) & 1) ? '1' : '0';            
        }                                                             
        dest[16] = '\0';            
}    


// Main entry point orchestrating the two-pass assembler.
int main(int argc, char *argv[]){
    if(argc != 2){
        printf("CORRECT USAGE: ./assembler [.asm code]");
    }
    char *assembly_filename = argv[1];
    FILE *hack_file = create_hack_file(assembly_filename);
    if(!hack_file) exit(1);
    
    Parser *parser = parser_from_file(assembly_filename);
    if(!parser) exit(2);

    SymbolTable *st = symboltable_create();
    if(!st) exit(3);

    // Pass 1: Scan instructions and bind ROM addresses to label declarations (LABEL).
    int counter = 0;
    while(parser_has_more_commands(parser)){
        
        parser_advance(parser);
        switch (parser_command_type(parser))
        {
        case COMMAND_A:
            counter++;
            break;
        case COMMAND_C:
            counter++;
            break;
        case COMMAND_L:
            char *symbol = parser_symbol(parser);
            symboltable_addEntry(st, symbol, counter);
            break;
        default:
            break;
        }
    }
    // Rewind parser to beginning for the second pass.
    parser_reset(parser);

    // Pass 2: Translate instructions, resolve symbols/variables, and write machine code.
    int curMem = 16;
    while(parser_has_more_commands(parser)){
        char command[17];
        parser_advance(parser);
        switch (parser_command_type(parser))
        {
        case COMMAND_A:
            char *symbol = parser_symbol(parser);
            if(isdigit((unsigned char) symbol[0])){
                int value = atoi(symbol);
                int_to_bin16(value, command);
            }
            else{
                if(symboltable_contains(st, symbol)){
                    int address = symboltable_getAddress(st, symbol);
                    int_to_bin16(address, command);
                }
                else{
                    symboltable_addEntry(st, symbol, curMem);
                    int_to_bin16(curMem, command);
                    curMem++;
                }
            }
            break;
        case COMMAND_C:
            char *dest_mnemonic, *comp_mnemonic, *jmp_mnemonic;
            dest_mnemonic = parser_dest(parser);
            comp_mnemonic = parser_comp(parser);
            jmp_mnemonic = parser_jmp(parser);

            command[0] = '1';
            command[1] = '1';
            command[2] = '1';
            strcpy(command + 3, code_comp(comp_mnemonic));
            strcpy(command + 10, code_dest(dest_mnemonic));
            strcpy(command + 13, code_jmp(jmp_mnemonic));
            break;
        case COMMAND_L:
            continue;
        default:
            break;
        }

        fprintf(hack_file, "%s\n", command);
    }
    // Deallocate resources and flush output file.
    parser_delete(&parser);
    symboltable_delete(&st);
    fclose(hack_file);
}
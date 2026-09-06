#include <stdio.h>
#include "./parser/parser.h"
#include "./code/code.h"
#include "string.h"

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

void int_to_bin16(int value, char *dest) {                        
        for (int i = 15; i >= 0; i--) {                                                                                               
            dest[15 - i] = ((value >> i) & 1) ? '1' : '0';            
        }                                                             
        dest[16] = '\0';            
}    


int main(int argc, char *argv[]){
    if(argc != 2){
        printf("CORRECT USAGE: ./assembler [.asm code]");
    }
    char *assembly_filename = argv[1];
    FILE *hack_file = create_hack_file(assembly_filename);
    if(!hack_file) exit(1);
    
    Parser *parser = parser_from_file(assembly_filename);
    if(!parser) exit(1);

    while(parser_has_more_commands(parser)){
        char command[17];
        parser_advance(parser);
        switch (parser_command_type(parser))
        {
        case COMMAND_A:
            char *symbol = parser_symbol(parser);
            int value = atoi(symbol);
            int_to_bin16(value, command);
            break;
        case COMMAND_L:
            /* code */
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
        
        default:
            break;
        }

        fprintf(hack_file, "%s\n", command);
    }
    
}
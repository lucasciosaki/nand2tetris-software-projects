#include "code.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// Mapping between assembly mnemonic and binary bits.
typedef struct {
    char *mnemonic;
    char *bits;
} Entry;

// Lookup table for destination field (3 bits).
static const Entry DEST_TABLE[] = {
    {"null", "000"},
    {"M", "001"},
    {"D", "010"},
    {"MD", "011"},
    {"A", "100"},
    {"AM", "101"},
    {"AD", "110"},
    {"AMD", "111"}
};

static const size_t DEST_TABLE_SIZE = sizeof(DEST_TABLE) / sizeof(DEST_TABLE[0]); 


// Lookup table for computation field (7 bits: a + c1..c6).
static const Entry COMP_TABLE[] = {
    {"0", "0101010"},
    {"1", "0111111"},
    {"-1", "0111010"},
    {"D", "0001100"},
    {"A", "0110000"},
    {"!D", "0001101"},
    {"!A", "0110001"},
    {"-D", "0001111"},
    {"-A", "0110011"},
    {"D+1", "0011111"},
    {"A+1", "0110111"},
    {"D-1", "0001110"},
    {"A-1", "0110010"},
    {"D+A", "0000010"},
    {"D-A", "0010011"},
    {"A-D", "0000111"},
    {"D&A", "0000000"},
    {"D|A", "0010101"},
    {"M", "1110000"},
    {"!M", "1110001"},
    {"-M", "1110011"},
    {"M+1", "1110111"},
    {"M-1", "1110010"},
    {"D+M", "1000010"},
    {"D-M", "1010011"},
    {"M-D", "1000111"},
    {"D&M", "1000000"},
    {"D|M", "1010101"}
};

static const size_t COMP_TABLE_SIZE = sizeof(COMP_TABLE) / sizeof(COMP_TABLE[0]); 

// Lookup table for jump field (3 bits).
static const Entry JMP_TABLE[] = {
    {"null", "000"},
    {"JGT", "001"},
    {"JEQ", "010"},
    {"JGE", "011"},
    {"JLT", "100"},
    {"JNE", "101"},
    {"JLE", "110"},
    {"JMP", "111"}
};

static const size_t JMP_TABLE_SIZE = sizeof(JMP_TABLE) / sizeof(JMP_TABLE[0]); 


// Translates dest mnemonic to 3-bit binary string.
char *code_dest (char *dest_mnemonic){
    if(!dest_mnemonic) return "000";
    for(size_t i = 0; i < DEST_TABLE_SIZE; i++){
        if(strcmp(dest_mnemonic, DEST_TABLE[i].mnemonic) == 0){
            return DEST_TABLE[i].bits;
        }
    }
    return NULL;
}

// Translates comp mnemonic to 7-bit binary string.
char *code_comp (char *comp_mnemonic){
    if(!comp_mnemonic) return "0000000";
    for(size_t i = 0; i < COMP_TABLE_SIZE; i++){
        if(strcmp(comp_mnemonic, COMP_TABLE[i].mnemonic) == 0){
            return COMP_TABLE[i].bits;
        }
    }
    return NULL;
}

// Translates jump mnemonic to 3-bit binary string.
char *code_jmp (char *jmp_mnemonic){
    if(!jmp_mnemonic) return "000";
    for(size_t i = 0; i < JMP_TABLE_SIZE; i++){
        if(strcmp(jmp_mnemonic, JMP_TABLE[i].mnemonic) == 0){
            return JMP_TABLE[i].bits;
        }
    }
    return NULL;
}
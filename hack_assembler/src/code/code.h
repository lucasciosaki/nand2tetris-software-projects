#ifndef _CODE_H
#define _CODE_H

// Returns 3-bit binary code for dest mnemonic.
char *code_dest (char *dest_mnemonic);

// Returns 7-bit binary code for comp mnemonic.
char *code_comp (char *comp_mnemonic);

// Returns 3-bit binary code for jump mnemonic.
char *code_jmp (char *jmp_mnemonic);

#endif
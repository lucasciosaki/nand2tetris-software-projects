#ifndef _PARSER_H
#define _PARSER_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

// Parser state structure definition.
typedef struct _parser  Parser;

// Classification of Hack assembly command types.
typedef enum {
    COMMAND_A,        // @value or @symbol
    COMMAND_C,        // dest=comp;jump
    COMMAND_L,        // (LABEL) pseudo-command
    COMMAND_INVALID   // unrecognized or invalid syntax
} CommandType;

// Initializes a parser by reading and cleaning an assembly file.
Parser *parser_from_file(char *filename_assembly);

// Frees all memory associated with the parser.
void parser_delete(Parser **parser_ref);

// Returns the CommandType of the current instruction.
CommandType parser_command_type(Parser *parser);

// Checks if there are more commands left to parse.
bool parser_has_more_commands(Parser *parser);

// Advances parser to the next command.
void parser_advance(Parser *parser);

// Extracts the symbol from an A-command or L-command.
char *parser_symbol(Parser *parser);

// Extracts the dest mnemonic from a C-command.
char *parser_dest(Parser *parser);

// Extracts the comp mnemonic from a C-command.
char *parser_comp(Parser *parser);

// Extracts the jump mnemonic from a C-command.
char *parser_jmp(Parser *parser);

// Resets iteration back to the beginning of commands.
void parser_reset(Parser *parser);


#endif
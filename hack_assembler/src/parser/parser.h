#ifndef _PARSER_H
#define _PARSER_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

typedef struct _parser  Parser;

typedef enum {
    COMMAND_A,
    COMMAND_C,
    COMMAND_L,
    COMMAND_INVALID
} CommandType;

Parser *parser_from_file(char *filename_assembly);
void parser_delete(Parser **parser_ref);
CommandType parser_command_type(Parser *parser);
bool parser_has_more_commands(Parser *parser);
void parser_advance(Parser *parser);
char *parser_symbol(Parser *parser);
char *parser_dest(Parser *parser);
char *parser_comp(Parser *parser);
char *parser_jmp(Parser *parser);
void parser_reset(Parser *parser);


#endif
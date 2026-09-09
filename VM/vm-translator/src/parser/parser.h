#ifndef VM_PARSER_H_
#define VM_PARSER_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

typedef struct _parser Parser;

typedef enum {
    C_ARITHMETIC,  // [OP]     
    C_PUSH,        // push [segment] [index]
    C_POP,         // pop [segment] [index]
    C_LABEL,       // label [symbol]
    C_GOTO,        // goto [symbol]
    C_IF,          // if-goto [symbol]
    C_FUNCTION,    // function [functionName] [nLocals]
    C_RETURN,      // return
    C_CALL,        // call [functionName] [nArgs]
    C_INVALID   // unrecognized or invalid syntax
} CommandType;


Parser *parser_from_file(char *filename_vm);
Parser *parser_create(FILE *file_vm);
void parser_delete(Parser **parser_ref);
bool parser_has_more_commands(Parser *parser);
void parser_advance(Parser *parser);
CommandType parser_command_type(Parser *parser);
char *parser_arg1(Parser *parser);
int parser_arg2(Parser *parser);

#endif
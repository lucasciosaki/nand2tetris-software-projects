#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "parser.h"

typedef struct _parser{
    char **commands;
    int ncommands;
    int curcommand;
    char cursymbol [128];
    char curdest [5];
    char curjmp [5];
    char curcomp [4];
} Parser;

Parser *parser_from_file(char *filename_assembly);
Parser *parser_create(FILE *file_assembly);
void parser_delete(Parser **parser_ref);
CommandType parser_command_type(Parser *parser);
bool parser_has_more_commands(Parser *parser);
void parser_advance(Parser *parser);
char *parser_symbol(Parser *parser);
char *parser_dest(Parser *parser);
char *parser_comp(Parser *parser);
char *parser_jmp(Parser *parser);
void parser_reset(Parser *parser);


//Create a Parser from a assembly filename
Parser *parser_from_file(char *filename_assembly){
    FILE *file_assembly;
    if (filename_assembly == NULL){
        printf("ERROR: Invalid Filename\n");
        return NULL;
    }
    if (!(file_assembly = fopen(filename_assembly, "r"))){
        printf("ERROR: Unable to open Assembly File\n");
        return NULL;
    }

    Parser *parser = parser_create(file_assembly);
    
    fclose(file_assembly);
    
    return parser;
}

//Create a Parser from a assembly file
Parser *parser_create(FILE *file_assembly){

    if (file_assembly == NULL){
        printf("ERROR: Invalid File\n");
        return NULL;
    }
    
    Parser *parser;
    
    if(!(parser = (Parser *) malloc(sizeof(Parser)))){
        printf("ERROR: Unable to alloc parser\n");
        goto err_parser;
    }

    

    int capacity = 32;

    char **commands;
    if(!(commands = (char **) malloc(sizeof(char *)*capacity))){
        printf("ERROR: Unable to alloc parser->commands\n");
        goto err_commands;
    }


    parser->ncommands = 0;
    parser->commands = commands;
    parser->curcommand = -1;

    char *buffer = NULL;
    size_t buffer_len = 0;
    ssize_t nread;



    while((nread = getline(&buffer, &buffer_len, file_assembly)) != -1){
        if(nread > 0 && (buffer[nread-1] == '\n' || buffer[nread-1] == '\r')){
            buffer[nread-1] = '\0';
        }
        
        char *comment = strstr(buffer, "//");                                                       
        if (comment) *comment = '\0';                                                               
                                                                                                    
                                                            
        int r = 0, w = 0;                                                                           
        while (buffer[r] != '\0') {                                                                 
            if (buffer[r] != ' ' && buffer[r] != '\t' && buffer[r] != '\r' && buffer[r] != '\n') {  
                buffer[w++] = buffer[r];                                                            
            }                                                                                       
            r++;                                                                                    
        }                                                                                           
        buffer[w] = '\0';                                                                           
                                                                                                    
                                                                             
        if (w == 0) continue;  

        if(parser->ncommands >= capacity){
            capacity *= 2;
            char **temp = realloc(parser->commands, capacity * sizeof(char *));
            if(!temp){
                printf("ERROR: Unable to reallocate parser->commands");
                goto err_command;
            }
            parser->commands = temp;
            
        }

        char *command = (char *)malloc(strlen(buffer) + 1);
        if(!command){
            printf("ERROR: Unable to allocate parser->commands[%d]", parser->ncommands);
            goto err_command;
        }
        strcpy(command, buffer);
        parser->commands[parser->ncommands++] = command;
    }
    free(buffer);
    
    if(parser->ncommands == 0) return parser;

    char **temp = realloc(parser->commands, parser->ncommands * sizeof(char *));
    if(!temp){
        printf("ERROR: Unable to reallocate parser->commands");
        goto err_command;
    }
    parser->commands = temp;
    

    return parser;

    err_command:
        free(buffer);
        for(int i = 0; i < parser->ncommands; i++){
            free(parser->commands[i]);
        }
        free(parser->commands);
        
    err_commands:
        free(parser);

    err_parser:
        return NULL;
        
}

void parser_delete(Parser **parser_ref){
    if(!(parser_ref) || !(*parser_ref)) return;
    Parser *parser = *parser_ref;
    for(int i = 0; i < parser->ncommands; i++){
        free(parser->commands[i]);
    }
    free(parser->commands);
    free(parser);
    *parser_ref = NULL;
    return;
}

bool parser_has_more_commands(Parser *parser){
    if(!parser) return false;
    return (parser->curcommand + 1) < parser->ncommands ? true : false;
}

void parser_advance(Parser *parser){
    if(parser_has_more_commands(parser)){
        parser->curcommand++;
    }
}

CommandType parser_command_type(Parser *parser){
    
    if(!parser || parser->curcommand < 0 || parser->curcommand >= parser->ncommands)
        return COMMAND_INVALID;

    char *command = parser->commands[parser->curcommand];
    if (command[0] == '@'){        
        return COMMAND_A;
    }
    if (command[0] == '('){
    
        return COMMAND_L;
    }
    return COMMAND_C;
}

char *parser_symbol(Parser *parser){
    if(!parser) return NULL;

    char *command = parser->commands[parser->curcommand];

    CommandType command_type = parser_command_type(parser);
    switch (command_type)
    {
    case COMMAND_A:
        
        strcpy(parser->cursymbol, command + 1);
        return parser->cursymbol;
       
    
    case COMMAND_L:
        size_t len_symbol = strlen(command) - 2;
        strncpy(parser->cursymbol, command + 1, len_symbol);
        parser->cursymbol[len_symbol] = '\0';
        return parser->cursymbol;

    default:
        return NULL;
    }
}

char *parser_dest(Parser *parser){
    if(!parser || parser_command_type(parser) != COMMAND_C) return NULL;
    
    char *command = parser->commands[parser->curcommand];
    if(command[0] == '=' || command[0] == ';') return NULL;
    for(size_t i = 0; i < strlen(command); i++){
        if (command[i] == '=')
        {
            strncpy(parser->curdest, command, i);
            parser->curdest[i] = '\0';
            return parser->curdest;
        }
    }
    return NULL;
}

char *parser_comp(Parser *parser){
    if(!parser || parser_command_type(parser) != COMMAND_C) return NULL;

    char *command = parser->commands[parser->curcommand];

    if(command[0] == '=' || command[0] == ';') return NULL;

    size_t start = 0;
    for(size_t i = 0; i < strlen(command); i++){
        if (command[i] == '=')
        {
            start = i + 1;
            break;
        }
    }


    for(size_t i = start; i < strlen(command); i++){
        if(command[i] == ';'){

            strncpy(parser->curcomp, command + start, (i - start));
            parser->curcomp[(i - start)] = '\0';
            return parser->curcomp;
        }
    }

    strncpy(parser->curcomp, command + start, strlen(command) - start);
    parser->curcomp[strlen(command) - start] = '\0';
    return parser->curcomp;
}

char *parser_jmp(Parser *parser){
    if(!parser || parser_command_type(parser) != COMMAND_C) return NULL;

    char *command = parser->commands[parser->curcommand];

    if(command[0] == '=' || command[0] == ';') return NULL;
    
    for(size_t i = 0; i < strlen(command); i++){
        if (command[i] == ';')
        {
            strncpy(parser->curjmp, command + i + 1, strlen(command) - i);
            parser->curjmp[strlen(command) - i] = '\0';
            return parser->curjmp;
        }
    }

    return NULL;
}

void parser_reset(Parser *parser){
    if(!parser) return;
    parser->curcommand = -1;
    return;
}

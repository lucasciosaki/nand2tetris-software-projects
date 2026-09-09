#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "parser.h"

typedef struct _parser{
    char **commands;
    int ncommands;
    int curcommand;
    char cur_arg1[128];
    char cur_arg2[128];
} Parser;


Parser *parser_from_file(char *filename_vm);
Parser *parser_create(FILE *file_vm);
void parser_delete(Parser **parser_ref);
bool parser_has_more_commands(Parser *parser);
void parser_advance(Parser *parser);
CommandType parser_command_type(Parser *parser);
char *parser_arg1(Parser *parser);
int parser_arg2(Parser *parser);

// Creates a Parser instance from an vm filename.
Parser *parser_from_file(char *filename_vm){
    FILE *file_vm;
    if (filename_vm == NULL){
        printf("ERROR: Invalid Filename\n");
        return NULL;
    }
    if (!(file_vm = fopen(filename_vm, "r"))){
        printf("ERROR: Unable to open vm File\n");
        return NULL;
    }

    Parser *parser = parser_create(file_vm);
    
    fclose(file_vm);
    
    return parser;
}

// Reads vm file, strips comments and whitespace, and stores commands in memory.
Parser *parser_create(FILE *file_vm){

    if (file_vm == NULL){
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



    while((nread = getline(&buffer, &buffer_len, file_vm)) != -1){
        if(nread > 0 && (buffer[nread-1] == '\n' || buffer[nread-1] == '\r')){
            buffer[nread-1] = '\0';
        }
        
        char *comment = strstr(buffer, "//");                                                       
        if (comment) *comment = '\0';                                                               
                                                                                                    
                                                            
        int r = 0, w = 0;                   
        bool spacing = false;                                                        
        while (buffer[r] != '\0') {                                                                 
            if (buffer[r] != '\r' && buffer[r] != '\n') {      
                if(buffer[r] == ' ' || buffer[r] == '\t'){
                    if(!spacing && w > 0){
                        buffer[w++] = buffer[r];  
                        spacing = true;
                    }
                }    
                else{
                    buffer[w++] = buffer[r];
                    if(spacing) spacing = false;
                }                 
               
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


// Frees all commands, string buffers, and the Parser structure.
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

// Checks if there are more instructions left in the file.
bool parser_has_more_commands(Parser *parser){
    if(!parser) return false;
    return (parser->curcommand + 1) < parser->ncommands ? true : false;
}

// Advances the parser to the next command.
void parser_advance(Parser *parser){
    if(parser_has_more_commands(parser)){
        parser->curcommand++;
    }
}


// Returns the type of the current command.
CommandType parser_command_type(Parser *parser){
    
    if(!parser || parser->curcommand < 0 || parser->curcommand >= parser->ncommands)
        return C_INVALID;

    char command[128];
    strcpy(command, parser->commands[parser->curcommand]);
    char *space = strchr(command, ' ');
    if(space){
        *space = '\0';
    }

     
    if(strcmp(command, "add") == 0) return C_ARITHMETIC;
    if(strcmp(command, "sub") == 0) return C_ARITHMETIC;
    if(strcmp(command, "neg") == 0) return C_ARITHMETIC;
    if(strcmp(command, "eq") == 0) return C_ARITHMETIC;
    if(strcmp(command, "gt") == 0) return C_ARITHMETIC;
    if(strcmp(command, "lt") == 0) return C_ARITHMETIC;
    if(strcmp(command, "and") == 0) return C_ARITHMETIC;
    if(strcmp(command, "or") == 0) return C_ARITHMETIC;
    if(strcmp(command, "not") == 0) return C_ARITHMETIC;

    if(strcmp(command, "push") == 0) return C_PUSH;

    if(strcmp(command, "pop") == 0) return C_POP;

    if(strcmp(command, "label") == 0) return C_LABEL;

    if(strcmp(command, "goto") == 0) return C_GOTO;

    if(strcmp(command, "if-goto") == 0) return C_IF;

    if(strcmp(command, "function") == 0) return C_FUNCTION;

    if(strcmp(command, "return") == 0) return C_RETURN;

    if(strcmp(command, "call") == 0) return C_CALL;

    return C_INVALID;
}

char *parser_arg1(Parser *parser){
    if(!parser) return NULL;
    
    CommandType commandtype = parser_command_type(parser);
    if(commandtype == C_RETURN || commandtype == C_INVALID) return NULL;

    char *command = parser->cur_arg1;
    strcpy(command, parser->commands[parser->curcommand]);
    
    char *commandComponent0 = strtok(command, " ");
    
    if(commandtype == C_ARITHMETIC){
        return commandComponent0;    
    }
    
    char *commandComponent1 = strtok(NULL, " ");
    return commandComponent1;
}

int parser_arg2(Parser *parser){
    if(!parser) return __INT_MAX__;
    
    CommandType commandtype = parser_command_type(parser);
    if(!(commandtype == C_PUSH ||
         commandtype == C_POP ||
         commandtype == C_FUNCTION ||
         commandtype == C_CALL)
        ) return __INT_MAX__;

    char *command = parser->cur_arg2;
    strcpy(command, parser->commands[parser->curcommand]);
    
    char *commandComponent = strtok(command, " ");
   
    
    commandComponent = strtok(NULL, " ");
    
    commandComponent = strtok(NULL, " ");
    
    return atoi(commandComponent);
}



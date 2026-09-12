#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <dirent.h>
#include "./codewriter/codewriter.h"
#include "./parser/parser.h"

#define OUTPUT_MAX_SIZE 256

void get_output_filename(const char *input_path, const struct stat *st, char *output, size_t out_size){
     if (!input_path || !st || !output || out_size == 0) return;
    
    if(S_ISDIR(st->st_mode)){
        char clean_path[256];
        strncpy(clean_path, input_path, sizeof(clean_path)-1);
        clean_path[sizeof(clean_path) - 1] = '\0';
        size_t len = strlen(clean_path);
        while (len>1 && clean_path[len - 1] == '/')
        {
            clean_path[--len] = '\0';
        }
        
        const char *dir_name = strrchr(clean_path, '/');
        dir_name = (dir_name != NULL) ? dir_name + 1 : clean_path;

        snprintf(output, out_size, "%s/%s.asm", clean_path, dir_name);
    }
    else{
        const char *last_slash = strrchr(input_path, '/');
        const char *filename = (last_slash != NULL) ? last_slash + 1 : input_path;

        const char *lastdot = strrchr(filename, '.');

        int base_len = (lastdot != NULL) ? (int) (lastdot - input_path) : (int) strlen(input_path);

        snprintf(output, out_size, "%.*s.asm", base_len, input_path);

    }
    return;
}

void translate_file(Parser *parser, CodeWriter *cw){
    while (parser_has_more_commands(parser))
    {
        parser_advance(parser);
        CommandType commandtype = parser_command_type(parser);
        char *arg1;
        int arg2;
        switch (commandtype)
        {
        case C_ARITHMETIC:
            arg1 = parser_arg1(parser);
            codewriter_write_arithmetic(cw, arg1);
            break;
        
        case C_PUSH:
            arg1 = parser_arg1(parser);
            
            arg2 = parser_arg2(parser);
            
            codewriter_write_pushpop(cw, "push", arg1, arg2);
            break;
        
        case C_POP:
            arg1 = parser_arg1(parser);
            
            arg2 = parser_arg2(parser);
            
            codewriter_write_pushpop(cw, "pop", arg1, arg2);
            break;

        case C_LABEL:
            arg1 = parser_arg1(parser);
            
            codewriter_write_label(cw, arg1);
            break;
        
        case C_GOTO:
            arg1 = parser_arg1(parser);
            
            codewriter_write_goto(cw, arg1);
            break; 
        
        case C_IF:
            arg1 = parser_arg1(parser);
    
            codewriter_write_if(cw, arg1);
            break; 

        case C_CALL:
            arg1 = parser_arg1(parser);
            arg2 = parser_arg2(parser);

            codewriter_write_call(cw, arg1, arg2);
            break; 
        
        case C_FUNCTION:
            arg1 = parser_arg1(parser);
            arg2 = parser_arg2(parser);

            codewriter_write_function(cw, arg1, arg2);
            break; 

        case C_RETURN:
        
            codewriter_write_return(cw);
            break; 

        default:
            break;
        }
    }
}

int main(int argc, char *argv[]){
    if(argc != 2) exit (1);
    
    char *path = argv[1];    
    struct stat path_stat;

    if (stat(path, &path_stat) != 0) {
        perror(path);
        exit(2);
    }

    char output[OUTPUT_MAX_SIZE];
    get_output_filename(path, &path_stat, output, OUTPUT_MAX_SIZE);

    
    CodeWriter *cw = codewriter_from_filename(output);
    

    if(S_ISDIR(path_stat.st_mode)){
        DIR *dir = opendir(path);
        if(!dir){
            exit(3);
        }

        codewriter_write_init(cw);
        struct dirent *entry;
        while((entry = readdir(dir)) != NULL){
            if( strcmp(entry->d_name, ".") == 0 ||
            strcmp(entry->d_name, "..") == 0){
                continue;
            }

            char *dot = strrchr(entry->d_name, '.');                   
            if (!dot || strcmp(dot, ".vm") != 0) {                     
                continue; // Skip non-.vm files                    
            } 

            char full_path[512];
            snprintf(full_path, sizeof(full_path), "%s/%s", path, entry->d_name);
            
            char vm_basename[256];
            int base_len = (int)(dot - entry->d_name);
            snprintf(vm_basename, sizeof(vm_basename), "%.*s", base_len, entry->d_name);

            codewriter_set_filename(cw, vm_basename);

            Parser *parser = parser_from_file(full_path);
            translate_file(parser, cw);

            parser_delete(&parser);
        }
        closedir(dir);
    }
    else{
        
        char *dot = strrchr(path, '.');                   
        if (!dot || strcmp(dot, ".vm") != 0) {                     
            exit(4);                   
        } 

        const char *last_slash = strrchr(path, '/');
        const char *filename_start = (last_slash != NULL) ? last_slash + 1 : path;
                
        int base_len = (int) (dot - filename_start);
        
        char vm_basename[256];   
        snprintf(vm_basename, sizeof(vm_basename), "%.*s", base_len, filename_start);
        
        codewriter_set_filename(cw, vm_basename);

        Parser *parser = parser_from_file(path);
        translate_file(parser, cw);

        parser_delete(&parser);
    }

    codewriter_delete(&cw);
    return 0;
}   


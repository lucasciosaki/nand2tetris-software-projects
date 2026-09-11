#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "codewriter.h"

typedef struct _code_writer {
    FILE *output_file;
    char current_VM_filename[256];
    int label_id;
} CodeWriter;

CodeWriter *codewriter_from_filename(char *filename_output);
CodeWriter *codewriter_create(FILE *file_output);
void codewriter_set_filename(CodeWriter *cw, char *vm_filename);
void codewriter_write_arithmetic(CodeWriter *cw, char *command);
void codewriter_write_pushpop(CodeWriter *cw, char *command, char *segment, int index);

CodeWriter *codewriter_from_filename(char *filename_output){
    
    FILE *file_output;
    
    if (filename_output == NULL){
        printf("ERROR: Invalid Filename\n");
        return NULL;
    }
    if (!(file_output = fopen(filename_output, "w"))){
        printf("ERROR: Unable to open Output File\n");
        return NULL;
    }

    CodeWriter *cw = codewriter_create(file_output);
    
    return cw;
}

CodeWriter *codewriter_create(FILE *file_output){
    if(!file_output){
        printf("ERROR: Invalid File \n");
        return NULL;
    }

    CodeWriter *cw = (CodeWriter *) malloc(sizeof(CodeWriter));
    if(!cw){
        printf("ERROR: Unable to alloc Code Writer\n");
        return NULL;
    }  

    cw->output_file = file_output;
    strcpy(cw->current_VM_filename, "");
    cw->label_id = 0;

    return cw;
}

void codewriter_delete(CodeWriter **cw_ref){
    if(!cw_ref || !(*cw_ref)) return;
    CodeWriter *cw = *cw_ref;
    fclose(cw->output_file);
    free(cw);
    *cw_ref = NULL;
}

void codewriter_set_filename(CodeWriter *cw, char *vm_filename){
    if(!cw || !vm_filename) return;
    strcpy(cw->current_VM_filename, vm_filename);
    return;
}

void codewriter_write_arithmetic(CodeWriter *cw, char *command){
    if(!cw || !command) return;

    if(strcmp(command, "add") == 0){
        fprintf(cw->output_file, "@SP\n");
        fprintf(cw->output_file, "AM=M-1\n"); //1. A, StackAddress = StackAddress - 1 
        fprintf(cw->output_file, "D=M\n"); //2. D = y = RAM[StackAddress]
        fprintf(cw->output_file, "A=A-1\n"); //3. A = StackAddress - 1
        fprintf(cw->output_file, "M=D+M\n"); //4. RAM[StackAddress] = RAM[StackAddress] - y 
        return;
    }

    if(strcmp(command, "sub") == 0){
        fprintf(cw->output_file, "@SP\n"); //1. A = SP 
        fprintf(cw->output_file, "AM=M-1\n"); //2. A, StackAddress = StackAddress - 1 
        fprintf(cw->output_file, "D=M\n"); //3. D = y = RAM[StackAddress]
        fprintf(cw->output_file, "A=A-1\n"); //4. A = StackAddress - 1
        fprintf(cw->output_file, "M=M-D\n"); //5. RAM[StackAddress] = RAM[StackAddress] - y 
        return;
    }

    if(strcmp(command, "neg") == 0){
        fprintf(cw->output_file, "@SP\n"); //1. A = SP
        fprintf(cw->output_file, "A=M-1\n"); //2. A = StackAddress - 1 
        fprintf(cw->output_file, "M=-M\n"); //3. RAM[StackAddress - 1] = -y   
        return;
    }

    if(strcmp(command, "eq") == 0){
        int id = cw->label_id++;
        fprintf(cw->output_file, "@SP\n"); //1. A = SP
        fprintf(cw->output_file, "AM=M-1\n"); //2. A, StackAddress = StackAddress - 1 
        fprintf(cw->output_file, "D=M\n"); //3. D = y = RAM[StackAddress]
        fprintf(cw->output_file, "A=A-1\n"); //4. A = StackAddress - 1
        fprintf(cw->output_file, "D=M-D\n"); //5. D = x - y
        fprintf(cw->output_file, "M=-1\n"); //6. RAM[StackAddress] = -1
        fprintf(cw->output_file, "@EQ_END_%d\n", id); //7. A = EQ_END
        fprintf(cw->output_file, "D;JEQ\n"); //8. If x = y : goto EQ_END
        fprintf(cw->output_file, "@SP\n"); // 9. A = SP
        fprintf(cw->output_file, "A=M-1\n"); //10. A = StackAddress-1
        fprintf(cw->output_file, "M=0\n"); // 11. RAM[StackAddress - 1] = 0
        fprintf(cw->output_file, "(EQ_END_%d)\n", id); 
        return;
    }   

    if(strcmp(command, "gt") == 0){
        int id = cw->label_id++;
        fprintf(cw->output_file, "@SP\n"); //1. A = SP
        fprintf(cw->output_file, "AM=M-1\n"); //2. A, StackAddress = StackAddress - 1 
        fprintf(cw->output_file, "D=M\n"); //3. D = y = RAM[StackAddress]
        fprintf(cw->output_file, "A=A-1\n"); //4. A = StackAddress - 1
        fprintf(cw->output_file, "D=M-D\n"); //5. D = x - y
        fprintf(cw->output_file, "M=-1\n"); //6. RAM[StackAddress] = -1
        fprintf(cw->output_file, "@GT_END_%d\n", id); //7. A = GT_END
        fprintf(cw->output_file, "D;JGT\n"); //8. If x = y : goto GT_END
        fprintf(cw->output_file, "@SP\n"); // 9. A = SP
        fprintf(cw->output_file, "A=M-1\n"); //10. A = StackAddress-1
        fprintf(cw->output_file, "M=0\n"); // 11. RAM[StackAddress - 1] = 0
        fprintf(cw->output_file, "(GT_END_%d)\n", id); 
        return;
    }   

    if(strcmp(command, "lt") == 0){
        int id = cw->label_id++;
        fprintf(cw->output_file, "@SP\n"); //1. A = SP
        fprintf(cw->output_file, "AM=M-1\n"); //2. A, StackAddress = StackAddress - 1 
        fprintf(cw->output_file, "D=M\n"); //3. D = y = RAM[StackAddress]
        fprintf(cw->output_file, "A=A-1\n"); //4. A = StackAddress - 1
        fprintf(cw->output_file, "D=M-D\n"); //5. D = x - y
        fprintf(cw->output_file, "M=-1\n"); //6. RAM[StackAddress] = -1
        fprintf(cw->output_file, "@LT_END_%d\n", id); //7. A = LT_END
        fprintf(cw->output_file, "D;JLT\n"); //8. If x = y : goto LT_END
        fprintf(cw->output_file, "@SP\n"); // 9. A = SP
        fprintf(cw->output_file, "A=M-1\n"); //10. A = StackAddress-1
        fprintf(cw->output_file, "M=0\n"); // 11. RAM[StackAddress - 1] = 0
        fprintf(cw->output_file, "(LT_END_%d)\n", id); 
        return;
    }
    
    if(strcmp(command, "and") == 0){
        fprintf(cw->output_file, "@SP\n"); //1. A = SP
        fprintf(cw->output_file, "AM=M-1\n"); //2. A, StackAddress = StackAddress - 1 
        fprintf(cw->output_file, "D=M\n"); //3. D = y = RAM[StackAddress]
        fprintf(cw->output_file, "A=A-1\n"); //4. A = StackAddress - 1
        fprintf(cw->output_file, "M=D&M\n"); //5. RAM[StackAddress - 1] = x & y
        return;
    }
    
    if(strcmp(command, "or") == 0){
        fprintf(cw->output_file, "@SP\n"); //1. A = SP
        fprintf(cw->output_file, "AM=M-1\n"); //2. A, StackAddress = StackAddress - 1 
        fprintf(cw->output_file, "D=M\n"); //3. D = y = RAM[StackAddress]
        fprintf(cw->output_file, "A=A-1\n"); //4. A = StackAddress - 1
        fprintf(cw->output_file, "M=D|M\n"); //5. RAM[StackAddress - 1] = x & y
        return;
    }
    
    if(strcmp(command, "not") == 0){
        fprintf(cw->output_file, "@SP\n"); //1. A = SP
        fprintf(cw->output_file, "A=M-1\n"); //2. A = StackAddress - 1 
        fprintf(cw->output_file, "M=!M\n"); //3. RAM[StackAddress - 1] = !y   
        return;
    }
    
}


void codewriter_write_pushpop(CodeWriter *cw, char *command, char *segment, int index){
    if(!cw || !command || !segment || index == __INT_MAX__) return;
    
    char *segment_symbol = NULL;
    if(strcmp(segment, "local") == 0) segment_symbol = "LCL";
    else if(strcmp(segment, "argument") == 0) segment_symbol = "ARG";
    else if(strcmp(segment, "this") == 0) segment_symbol = "THIS";
    else if(strcmp(segment, "that") == 0) segment_symbol = "THAT";
        

    if(strcmp(command, "push") == 0){
        
        //Handle constant segment
        if(strcmp(segment, "constant") == 0){
            switch (index)
            {
            case -1:
                fprintf(cw->output_file, "D=-1\n");
                break;
            
            case 0:
                fprintf(cw->output_file, "D=0\n");
                break;
            
            case 1:
                fprintf(cw->output_file, "D=1\n");
                break;
            
            default:
                fprintf(cw->output_file, "@%d\n", index); 
                fprintf(cw->output_file, "D=A\n");     
                break;
            }     
        }
        
        //Handle (local, argument, this and that) segments
        if (segment_symbol != NULL) {
            switch (index)
            {
            case 0:                                                    
                fprintf(cw->output_file, "@%s\n", segment_symbol); //A = SegmentBasePointer    
                fprintf(cw->output_file, "A=M\n"); // A = SegmentBase
                fprintf(cw->output_file, "D=M\n"); // D = *(SegmentBase + 0)
                break;                                                 
            case 1:                                                    
                fprintf(cw->output_file, "@%s\n", segment_symbol); //A = SegmentBasePointer
                fprintf(cw->output_file, "A=M+1\n"); // A = SegmentBase + 1              
                fprintf(cw->output_file, "D=M\n"); // D = *(SegmentBase + 1)     
                break;                                                 
            case 2:                                                    
                fprintf(cw->output_file, "@%s\n", segment_symbol); //A = SegmentBasePointer
                fprintf(cw->output_file, "A=M+1\n"); // A = SegmentBase + 1                  
                fprintf(cw->output_file, "A=A+1\n"); // A = SegmentBase + 2                     
                fprintf(cw->output_file, "D=M\n"); // D = *(SegmentBase + 2)         
                break;                                                 
            default:                                                   
                fprintf(cw->output_file, "@%s\n", segment_symbol); //A = SegmentBasePointer
                fprintf(cw->output_file, "D=M\n"); //D = SegmentBase
                fprintf(cw->output_file, "@%d\n", index); //A = index 
                fprintf(cw->output_file, "A=D+A\n"); //A = SegmentBase + index  
                fprintf(cw->output_file, "D=M\n"); //D = *(SegmentBase + index)          
                break;                 
            }
        }

        if(strcmp(segment, "pointer") == 0){
            if(index == 0){ 
                fprintf(cw->output_file, "@THIS\n"); //A = THIS
                fprintf(cw->output_file, "D=M\n"); //D = this_segment_base
            }
            if(index == 1){ 
                fprintf(cw->output_file, "@THAT\n"); //A = THAT
                fprintf(cw->output_file, "D=M\n"); //D = that_segment_base
            }
        }

        if(strcmp(segment, "temp") == 0){
            //A = temp index
               
            if (index >= 0 && index <= 7) {
                fprintf(cw->output_file, "@%d\n", 5 + index); // R5..R12   
                fprintf(cw->output_file, "D=M\n");
           
            }
        }

        if(strcmp(segment, "static") == 0){
            fprintf(cw->output_file, "@%s.%d\n", cw->current_VM_filename, index); 
            fprintf(cw->output_file, "D=M\n"); // D = *(temp + index)
        }


        //Store D in stack
        fprintf(cw->output_file, "@SP\n"); //A = SP
        fprintf(cw->output_file, "AM=M+1\n"); //A = M = StackBase + 1
        fprintf(cw->output_file, "A=A-1\n"); //A = NewStackBase - 1
        fprintf(cw->output_file, "M=D\n"); //*(NewStackBase - 1) = *(SegmentBase + index)
    }
        
    if(strcmp(command, "pop") == 0){
        
        if(index < 3 || segment_symbol == NULL){
                fprintf(cw->output_file, "@SP\n"); //A = SP 
                fprintf(cw->output_file, "AM=M-1\n"); //A = M = StackBase - 1
                fprintf(cw->output_file, "D=M\n"); //D = *(StackBase - 1)
        }

        
        if (segment_symbol != NULL) {
            
            //Store stack head in D
            
            switch (index)
            {
            case 0:                                                    
                fprintf(cw->output_file, "@%s\n", segment_symbol); //A = Segment_Pointer    
                fprintf(cw->output_file, "A=M\n"); // A = SegmentBase
                fprintf(cw->output_file, "M=D\n"); // *(SegmentBase) = D  
                break;                                                 
            case 1:                                                    
                fprintf(cw->output_file, "@%s\n", segment_symbol); //A = Segment_Pointer 
                fprintf(cw->output_file, "A=M+1\n"); // A = SegmentBase + 1                
                fprintf(cw->output_file, "M=D\n"); // *(SegmentBase + 1) = D                  
                break;                                                 
            case 2:                                                    
                fprintf(cw->output_file, "@%s\n", segment_symbol); //A = Segment_Pointer   
                fprintf(cw->output_file, "A=M+1\n"); // A = SegmentBase + 1                    
                fprintf(cw->output_file, "A=A+1\n"); // A = SegmentBase + 2                   
                fprintf(cw->output_file, "M=D\n"); // *(SegmentBase + 2) = D                  
                break;                                                 
            default:                    
                
                fprintf(cw->output_file, "@%s\n", segment_symbol); //A = Segment_Pointer
                fprintf(cw->output_file, "D=M\n"); //D = SegmentBase
                fprintf(cw->output_file, "@%d\n", index); //A = index
                fprintf(cw->output_file, "D=D+A\n"); //D = SegmentBase + index
                fprintf(cw->output_file, "@R13\n"); //A = R13
                fprintf(cw->output_file, "M=D\n"); //*(R13) = SegmentBase + index

                fprintf(cw->output_file, "@SP\n"); //A = SP 
                fprintf(cw->output_file, "AM=M-1\n"); //A = M = StackBase - 1
                fprintf(cw->output_file, "D=M\n"); //D = *(StackBase - 1)
                fprintf(cw->output_file, "@R13\n"); //A = R13
                fprintf(cw->output_file, "A=M\n"); //A = *(R13) = SegmentBase + index
                fprintf(cw->output_file, "M=D\n");  // *(SegmentBase + index) = *(StackBase - 1)
                                     
                break;                 
            }
        }

        if(strcmp(segment, "pointer") == 0){

            if(index == 0){ 
                fprintf(cw->output_file, "@THIS\n"); //A = THIS
                fprintf(cw->output_file, "M=D\n"); // this_segment_base = *(StackBase - 1)
            }
            if(index == 1){ 
                fprintf(cw->output_file, "@THAT\n"); //A = THAT
                fprintf(cw->output_file, "M=D\n"); // that_segment_base = *(StackBase - 1)
            }
        }

        if(strcmp(segment, "temp") == 0){

            //A = temp index
            if (index >= 0 && index <= 7) {
                fprintf(cw->output_file, "@%d\n", 5 + index); // R5..R12   
                fprintf(cw->output_file, "M=D\n");
            }
        }

        if(strcmp(segment, "static") == 0){

            fprintf(cw->output_file, "@%s.%d\n", cw->current_VM_filename, index); 
            fprintf(cw->output_file, "M=D\n"); // D = *(temp + index)
        }


    }
}


#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "codewriter.h"

typedef struct _code_writer {
    FILE *output_file;
    char current_VM_filename[256];
    char current_function[256];
    int label_id;
} CodeWriter;

CodeWriter *codewriter_from_filename(char *filename_output);
CodeWriter *codewriter_create(FILE *file_output);
void codewriter_set_filename(CodeWriter *cw, char *vm_filename);
void codewriter_write_init(CodeWriter *cw);
void codewriter_write_arithmetic(CodeWriter *cw, char *command);
void codewriter_write_pushpop(CodeWriter *cw, char *command, char *segment, int index);
void codewriter_write_label(CodeWriter *cw, char *label);
void codewriter_write_goto(CodeWriter *cw, char *label);
void codewriter_write_if(CodeWriter *cw, char *label);

void codewriter_write_call(CodeWriter *cw, char *function, int nArgs);
void codewriter_write_function(CodeWriter *cw,  char *function, int nLocals);
void codewriter_write_return(CodeWriter *cw);

void codewriter_write_init(CodeWriter *cw){
    if(!cw || !cw->output_file) return;

    fprintf(cw->output_file, "@256\n");
    fprintf(cw->output_file, "D=A\n");
    fprintf(cw->output_file, "@SP\n");
    fprintf(cw->output_file, "M=D\n");

    codewriter_write_call(cw, "Sys.init", 0);

    return;
}

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
    strcpy(cw->current_function, "");
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
        fprintf(cw->output_file, "D;JGT\n"); //8. If x > y : goto GT_END
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
        fprintf(cw->output_file, "D;JLT\n"); //8. If x < y : goto LT_END
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

void codewriter_write_label(CodeWriter *cw, char *label){
    if(!cw || !cw->output_file) return;
    
    fprintf(cw->output_file, "(%s$%s)\n", cw->current_function, label);
    return;
}

void codewriter_write_goto(CodeWriter *cw, char *label){
    if(!cw || !cw->output_file) return;

    fprintf(cw->output_file, "@%s$%s\n", cw->current_function, label);
    fprintf(cw->output_file, "0;JMP\n");
}

void codewriter_write_if(CodeWriter *cw, char *label){
    if(!cw || !cw->output_file) return;

    fprintf(cw->output_file, "@SP\n"); // A = SP
    fprintf(cw->output_file, "AM=M-1\n"); // A, StackBase = StackBase - 1
    fprintf(cw->output_file, "D=M\n"); // D = StackBase - 1
    fprintf(cw->output_file, "@%s$%s\n", cw->current_function, label);
    fprintf(cw->output_file, "D;JNE\n"); // if D != 0 -> goto label

    return;
}

void codewriter_write_call(CodeWriter *cw, char *function, int nArgs){
    if(!cw || !cw->output_file) return;

    int id = cw->label_id++;
    //push return-address
    const char *fn = (cw->current_function[0] != '\0') ? cw->current_function : "bootstrap"; 
    fprintf(cw->output_file, "@%s$RETURN%d\n", fn, id);
    fprintf(cw->output_file, "D=A\n");
    fprintf(cw->output_file, "@SP\n");
    fprintf(cw->output_file, "AM=M+1\n");
    fprintf(cw->output_file, "A=A-1\n");
    fprintf(cw->output_file, "M=D\n");

    //push LCL
    fprintf(cw->output_file, "@LCL\n");
    fprintf(cw->output_file, "D=M\n");
    fprintf(cw->output_file, "@SP\n");
    fprintf(cw->output_file, "AM=M+1\n");
    fprintf(cw->output_file, "A=A-1\n");
    fprintf(cw->output_file, "M=D\n");
    
    //push ARG
    fprintf(cw->output_file, "@ARG\n");
    fprintf(cw->output_file, "D=M\n");
    fprintf(cw->output_file, "@SP\n");
    fprintf(cw->output_file, "AM=M+1\n");
    fprintf(cw->output_file, "A=A-1\n");
    fprintf(cw->output_file, "M=D\n");
    
    
    //push THIS
    fprintf(cw->output_file, "@THIS\n");
    fprintf(cw->output_file, "D=M\n");
    fprintf(cw->output_file, "@SP\n");
    fprintf(cw->output_file, "AM=M+1\n");
    fprintf(cw->output_file, "A=A-1\n");
    fprintf(cw->output_file, "M=D\n");
    

    //push THAT
    fprintf(cw->output_file, "@THAT\n");
    fprintf(cw->output_file, "D=M\n");
    fprintf(cw->output_file, "@SP\n");
    fprintf(cw->output_file, "AM=M+1\n");
    fprintf(cw->output_file, "A=A-1\n");
    fprintf(cw->output_file, "M=D\n");
    
    //ARG = SP-n-5
    fprintf(cw->output_file,"@SP\n");
    fprintf(cw->output_file,"D=M\n");
    fprintf(cw->output_file,"@%d\n", nArgs + 5);
    fprintf(cw->output_file,"D=D-A\n");
    fprintf(cw->output_file,"@ARG\n");
    fprintf(cw->output_file, "M=D\n");

    //LCL = SP
    fprintf(cw->output_file,"@SP\n");
    fprintf(cw->output_file,"D=M\n");
    fprintf(cw->output_file,"@LCL\n");
    fprintf(cw->output_file, "M=D\n");

    //goto function
    fprintf(cw->output_file, "@%s\n", function);
    fprintf(cw->output_file, "0;JMP\n");
    
    fprintf(cw->output_file, "(%s$RETURN%d)\n", fn, id);

    return;
}

void codewriter_write_function(CodeWriter *cw,  char *function, int nLocals){
    if(!cw || !cw->output_file) return;

    //cw->current_function = function;
    strcpy(cw->current_function, function);

    fprintf(cw->output_file, "(%s)\n", function); //(function)

    //Initiate locals = 0
    if(nLocals > 0){
        fprintf(cw->output_file, "@SP\n"); //A = SP
        fprintf(cw->output_file, "D=M\n"); // D = StackBase

        fprintf(cw->output_file, "@LCL\n"); // A = LCL
        fprintf(cw->output_file, "AM=D\n"); // A = LocalBase = StackBase

    }
    for(int i = 0; i < nLocals; i++){
        fprintf(cw->output_file, "M=0\n"); // *(Localbase + i) = 0

        if(i != nLocals - 1)
            fprintf(cw->output_file, "A=A+1\n"); // A = Localbase + i + 1
        else{
            fprintf(cw->output_file, "D=A+1\n"); // D = Localbase + i + 1
            fprintf(cw->output_file, "@SP\n"); // A = SP
            fprintf(cw->output_file, "M=D\n"); // StackBase = Localbase + i + 1
        }
    }
    
}

void codewriter_write_return(CodeWriter *cw){

    
    fprintf(cw->output_file, "@LCL\n");
    fprintf(cw->output_file, "D=M\n"); //D = LCL

    fprintf(cw->output_file, "@R14\n"); //&FRAME = R14 
    fprintf(cw->output_file, "M=D\n"); //FRAME = LCL 


    fprintf(cw->output_file, "@5\n"); 
    fprintf(cw->output_file, "A=D-A\n"); //A = FRAME - 5
    fprintf(cw->output_file, "D=M\n"); //D = *(FRAME - 5)

    fprintf(cw->output_file, "@R15\n"); //&RET = R15 
    fprintf(cw->output_file, "M=D\n"); //RET = *(FRAME - 5) 


    fprintf(cw->output_file, "@SP\n"); //A = SP
    fprintf(cw->output_file, "AM=M-1\n"); //A = StackBase
    fprintf(cw->output_file, "D=M\n"); //D = pop()

    fprintf(cw->output_file, "@ARG\n"); //A = ARG
    fprintf(cw->output_file, "A=M\n");  //A = ArgBase
    fprintf(cw->output_file, "M=D\n");  //*(ArgBase) = pop()

    fprintf(cw->output_file, "D=A\n"); //D = ArgBase
    fprintf(cw->output_file, "@SP\n"); //A = SP
    fprintf(cw->output_file, "M=D+1\n"); //StackBase = ArgBase + 1

    fprintf(cw->output_file, "@R14\n");
    fprintf(cw->output_file, "AM=M-1\n"); //A = *(R14) = FRAME - 1
    fprintf(cw->output_file, "D=M\n"); // D = *(FRAME - 1)
    fprintf(cw->output_file, "@THAT\n");
    fprintf(cw->output_file, "M=D\n"); // That = *(FRAME - 1)
    

    fprintf(cw->output_file, "@R14\n");
    fprintf(cw->output_file, "AM=M-1\n"); //A = *(R14) = FRAME - 2
    fprintf(cw->output_file, "D=M\n"); // D = *(FRAME - 2)
    fprintf(cw->output_file, "@THIS\n");
    fprintf(cw->output_file, "M=D\n"); // This = *(FRAME - 2)
    
    fprintf(cw->output_file, "@R14\n");
    fprintf(cw->output_file, "AM=M-1\n"); //A = *(R14) = FRAME - 3
    fprintf(cw->output_file, "D=M\n"); // D = *(FRAME - 3)
    fprintf(cw->output_file, "@ARG\n");
    fprintf(cw->output_file, "M=D\n"); // Arg = *(FRAME - 3)

    fprintf(cw->output_file, "@R14\n");
    fprintf(cw->output_file, "AM=M-1\n"); //A = *(R14) = FRAME - 4
    fprintf(cw->output_file, "D=M\n"); // D = *(FRAME - 4)
    fprintf(cw->output_file, "@LCL\n");
    fprintf(cw->output_file, "M=D\n"); // Lcl = *(FRAME - 4)

    fprintf(cw->output_file, "@R15\n");
    fprintf(cw->output_file, "A=M\n");
    fprintf(cw->output_file, "0;JMP\n");
    
}


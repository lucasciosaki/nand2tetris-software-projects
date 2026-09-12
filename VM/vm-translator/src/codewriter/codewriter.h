#ifndef VM_CODEWRITER_H_
#define VM_CODEWRITER_H_

typedef struct _code_writer CodeWriter;

CodeWriter *codewriter_from_filename(char *filename_output);
CodeWriter *codewriter_create(FILE *file_output);
void codewriter_delete(CodeWriter **cw_ref);

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

#endif
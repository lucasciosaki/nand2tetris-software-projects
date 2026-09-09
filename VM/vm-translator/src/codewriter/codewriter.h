#ifndef VM_CODEWRITER_H_
#define VM_CODEWRITER_H_

typedef struct _code_writer CodeWriter;

CodeWriter *codewriter_from_filename(char *filename_output);
CodeWriter *codewriter_create(FILE *file_output);
void codewriter_set_filename(CodeWriter *cw, char *vm_filename);
void codewriter_write_arithmetic(CodeWriter *cw, char *command);
void codewriter_write_pushpop(CodeWriter *cw, char *command, char *segment, int index);
void codewriter_delete(CodeWriter **cw_ref);

#endif
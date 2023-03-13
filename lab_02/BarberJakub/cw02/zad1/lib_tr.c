#include"lib_tr.h"


size_t get_file_size_lib(FILE* file){
    fseek(file, 0L, SEEK_END);
    long size = ftell(file);
    rewind(file);
    return (size_t) size;
}
bool tr_lib(char s, char t, const char* from_filepath, const char* to_filepath){
    FILE* from_file = fopen(from_filepath,"r");
    if (from_file==NULL){
        printf("Error while opening start file\n");
        return false;
    }
    FILE* to_file = fopen(to_filepath,"w");
    if (to_file==NULL){
        printf("Error while opening result file\n");
        return false;
    }

    size_t size = get_file_size_lib(from_file);

    char * buffer = calloc(size, sizeof(char));
    fread(buffer,sizeof(char),size,from_file);

    char *buffer_start=buffer;
    while (*buffer){
        if (*buffer == s){
            *buffer =t;
        }
        buffer++;
    }

    fwrite(buffer_start,sizeof(char),size,to_file);
    free(buffer_start);
    fclose(from_file);
    fclose(to_file);
    return true;
}
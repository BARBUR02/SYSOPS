#include "sys_tr.h"



size_t get_file_size_sys(int file_d){
    size_t size = (size_t)lseek(file_d, 0L, SEEK_END);
    return size;
}
bool tr_sys(char s, char t, const char* from_filepath, const char* to_file_path){
    int from_file_d = open(from_filepath, O_RDONLY );
    if (from_file_d==-1){
        printf("Error while opening start file\n");
        return false;
    }
    int to_file_d = open(from_filepath, O_WRONLY | O_CREAT | O_APPEND  );
    if (to_file_d==-1){
        printf("Error while opening result file\n");
        return false;
    }
    size_t size = get_file_size_sys(from_file_d);

    char * buffer = calloc(size, sizeof(char));
    read(from_file_d,buffer,sizeof(char)*size);

    char *buffer_start=buffer;
    while (*buffer){
        if (*buffer == s){
            *buffer =t;
        }
        buffer++;
    }
  
    write(to_file_d,buffer,sizeof(char)*size);
    free(buffer_start);
    close(from_file_d);
    close(to_file_d);
    return true;


}
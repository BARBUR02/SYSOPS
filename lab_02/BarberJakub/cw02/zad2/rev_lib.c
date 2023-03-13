#include"rev_lib.h"


char* rev(char * buff){
    char tmp;
    int len = strlen(buff);
    for (int i=0;i<len/2;i++){
        tmp=buff[i];
        buff[i]=buff[len-i-1];
        buff[len-i-1]=tmp;
    }
    return buff;
}

size_t get_file_size(FILE * f){
    fseek(f,0L,SEEK_END);
    size_t size = (size_t)ftell(f);
    rewind(f);
    return size;
}

//by pasting every char: 
bool reverse_by_char(FILE* from_f, FILE* to_f){
    char buffer[2]; // for storing null character
    size_t size = get_file_size(from_f);
    int iterations = size;
    fseek(from_f,1,SEEK_END);
    for (int i=0;i<iterations;i++){
        fseek(from_f,-2,SEEK_CUR);
        int end_i = fread(buffer,sizeof(char), 1,from_f);
        buffer[end_i]=0;
        fprintf(to_f,"%s",rev(buffer));
    }
    return true;
}

bool reverse_by_byte(FILE* from_f, FILE* to_f){
    int block=1024;
    char buffer[block+1];
    size_t size = get_file_size(from_f);
    int iterations = size / block;
    int spare = size % block;
    fseek(from_f,block,SEEK_END);
    for (int i=0;i<iterations;i++){
        fseek(from_f,-(2*block),SEEK_CUR);
        int end_i = fread(buffer,sizeof(char), block,from_f);
        buffer[end_i]=0;
        fprintf(to_f,"%s",rev(buffer));
    }
    fseek(from_f,-spare-block,SEEK_CUR);
    int end_i = fread(buffer,sizeof(char), spare,from_f);
    buffer[end_i]=0;
    fprintf(to_f,"%s",rev(buffer));
    return true;
}
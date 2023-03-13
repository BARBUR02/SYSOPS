#include<stdio.h>
#include<stdlib.h>
#include"rev_lib.h"
#include<time.h>

#define BILLION 1e-9

int main(int argc, char* argv[]){
    struct timespec begin,end;
    clock_gettime(CLOCK_REALTIME, &begin);
    // printf("%s",argv[1]);
    // printf("%s\n",argv[2]);
    if (argc!=3){
        printf("Incorrect number of arguments expected: <filename> <filename>");
        return 1;
    }
    char * from_filepath = argv[1];
    char * to_filepath = argv[2];
    FILE * from_file = fopen(from_filepath,"r");
    if (!from_file){
        printf("Problems in opening the first file");
        return 1;
    }
    FILE * to_file = fopen(to_filepath,"w");
    if (!to_file){
        printf("Problems in opening/creating the destination file");
        return 1;
    }

    #ifndef BYTE
        reverse_by_char(from_file,to_file);
    #else 
        reverse_by_byte(from_file,to_file);
    #endif
    
    fclose(from_file);
    fclose(to_file);

    clock_gettime(CLOCK_REALTIME,&end);

    double time_taken = (end.tv_sec-begin.tv_sec) + (end.tv_nsec-begin.tv_nsec)*BILLION;
    printf("Time taken: %lfs\n",time_taken);

    return 0;

}
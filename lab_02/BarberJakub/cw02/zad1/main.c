#include<stdio.h>
#include<time.h>
#include<string.h>
#ifdef SYS_MODE
    #include"sys_tr.h"
#else 
    #include"lib_tr.h"
#endif

#define BILLION 1e-9

bool char_check(char* arg){
    return (strlen(arg)==1) ? true : false;
}

int main(int argc, char* argv[]){
    struct timespec begin,end;
    clock_gettime(CLOCK_REALTIME, &begin);
    char to_replace = argv[1][0];
    char replacement = argv[2][0];
    char *input_file_name=argv[3];
    char *output_file_name=argv[4];
    
    if (!char_check(argv[1]) || !(char_check(argv[2]))){
        printf("First two arguments provided MUST be chars\n");
        return 1;
    }
    if (argc!=5){
        printf("Program accepts 4 arguments: <char> <char> <string> <string>\n");
        return 1;
    }


    #ifdef SYS_MODE
    tr_sys(to_replace,replacement,input_file_name,output_file_name);
    #else 
    tr_lib(to_replace,replacement,input_file_name,output_file_name);
    #endif

    clock_gettime(CLOCK_REALTIME,&end);

    double time_taken = (end.tv_sec-begin.tv_sec) + (end.tv_nsec-begin.tv_nsec)*BILLION;
    printf("Time taken: %lfs\n",time_taken);
    return 0;
}
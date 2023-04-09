#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<string.h>
#include<sys/types.h>
#include<unistd.h>
#include<time.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<sys/wait.h>


#define FIFO_PATH "/tmp/integral_queue"
#define BILLION 1e-9

bool is_numerical(char* str){
    if(strspn(str,"0123456789")==strlen(str) && str[0]!='0'){
        return true;
    }
    return false;
}


int main(int argc, char* argv[]){
    if (argc!=3){
        printf("Incorrect argument count: expected 2\n");
        exit(1);
    }

    if (!is_numerical(argv[2])){
        printf("Argument 2 should be a correct integer number\n");
    }
    struct timespec start,end;
    int buff_size = 256;
    char buff[buff_size];
    char arg_1[buff_size];
    char arg_2[buff_size];
    clock_gettime(CLOCK_REALTIME,&start);
    int n = atoi(argv[2]);
    double delta = (double) 1 / (double) n;
    double prev_delta=0;
    double current_delta=0;

    mkfifo(FIFO_PATH,0666);

    for (int i=0;i<n;i++){
        prev_delta=current_delta;
        current_delta+=delta;
        if (fork() ==0 ){
            snprintf(arg_1,buff_size,"%lf",prev_delta);
            snprintf(arg_2,buff_size,"%lf",current_delta);
            execl("./integral_util","integral_util",arg_1,arg_2,argv[1],NULL);
            // fflush(NULL);
        }
    }

    double result =0.0;
    int fifo = open(FIFO_PATH,O_RDONLY);
    int already_added=0;
    while (already_added<n){
        // printf("CAGED... \n");
        size_t size = read(fifo,buff,buff_size);
        buff[size]=0;

        char del[]="\n";

        char * num = strtok(buff,del);
        while (num){
            result += strtod(num,NULL);
            num=strtok(NULL,del);
            already_added++;
        }
    }
    close(fifo);

    clock_gettime(CLOCK_REALTIME,&end);
    double time_elapsed = (end.tv_sec-start.tv_sec) + (end.tv_nsec-start.tv_nsec)*BILLION;
    printf("Value for integral of f(x) for n = %d : %lf\nTime taken: %lfs\n",n,result,time_elapsed);

    return 0;
}
#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<string.h>
#include<sys/types.h>
#include<unistd.h>
#include<time.h>
#include<sys/wait.h>

#define BILLION 1e-9

double f(double x){
    return 4 / (x*x+1);
}

bool is_numerical(char* str){
    if(strspn(str,"0123456789")==strlen(str) && str[0]!='0'){
        return true;
    }
    return false;
}

double calc_integral(double x, double x1,double dx){
    double result =0.0;
    x+=dx/2.0;
    while (x < x1 ){
        result += f(x)*dx;
        x+=dx; 
    }
    return result;
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
    clock_gettime(CLOCK_REALTIME,&start);
    double dx = strtod(argv[1],NULL);
    int n = atoi(argv[2]);
    int fd_arr[n][2];
    double delta = (double) 1 / (double) n;
    double prev_delta=0;
    double current_delta=0;
    for (int i=0;i<n;i++){
        prev_delta=current_delta;
        current_delta+=delta;
        pipe(fd_arr[i]);
        if (fork() ==0 ){
            close(fd_arr[i][0]);
            double num = calc_integral(prev_delta,current_delta,dx);
            size_t size = snprintf(buff,buff_size,"%lf",num);
            write(fd_arr[i][1],buff,size);
            exit(0);
        }
    }

    while(wait(NULL)>0);

    double result =0.0;
    for (int i=0; i<n;i++){
        size_t size = read(fd_arr[i][0], buff, buff_size );
        buff[size]=0;
        result += strtod(buff,NULL);
    }

    clock_gettime(CLOCK_REALTIME,&end);
    double time_elapsed = (end.tv_sec-start.tv_sec) + (end.tv_nsec-start.tv_nsec)*BILLION;
    printf("Value for integral of f(x) for n = %d : %lf\nTime taken: %lfs\n",n,result,time_elapsed);

    return 0;
}
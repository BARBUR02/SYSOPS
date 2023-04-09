#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<unistd.h>
#include<fcntl.h>

#define FIFO_PATH "/tmp/integral_queue"


double f(double x){
    return 4 / (x*x+1);
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
    
    int buff_size = 256;
    char buff[buff_size];
    double a = strtod(argv[1],NULL);
    double b = strtod(argv[2],NULL);
    double dx = strtod(argv[3],NULL);

    double local_res = calc_integral(a,b,dx);
    
    size_t size = snprintf(buff, buff_size,"%lf\n",local_res);

    int fifo = open(FIFO_PATH, O_WRONLY);
    int res =write(fifo,buff,size);
    if (res<0){
        printf("Error while opening file\n");
        exit(1);
    }
    close(fifo);

    return 0;
}
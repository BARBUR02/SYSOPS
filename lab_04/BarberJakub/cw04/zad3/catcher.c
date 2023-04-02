#include<stdio.h>
#include<signal.h>
#include<stdlib.h>
#include<stdbool.h>
#include<sys/types.h>
#include<time.h>
#include<unistd.h>

int number_of_requests=0;

typedef enum{
    NUM = 1,
    CURR_TIME = 2,
    REQUEST_NUM = 3,
    TIME_EACH_SEC = 4,
    FINISH = 5
} STATE;


STATE state =NUM;
bool done = true;



void print_time(){
    time_t raw_time;
    struct tm* time_info;
    time(&raw_time);
    time_info = localtime( &raw_time);
    printf("%s\n", asctime(time_info));
}

void print_nums(){
    int i=0;
    while(i<100){
        printf("NUMBER: %d\n",++i);
    }
}

void print_request_num(){
    printf("Current request counter : %d\n", number_of_requests);
}

void terminate(){
    printf("Terminating catcher ...\n");
    fflush(NULL);
    exit(0);
}

void handler(int signo, siginfo_t* info, void* context){
    pid_t pid = info->si_pid;
    int req = info->si_status;
    // printf("Reqeuest test: %d\n",req);

    if (req<1 || req>5){
        printf("Invalid request\n");
    }
    else{
        number_of_requests++;
        state = (STATE) req;
        done= false;
    }
    kill(pid,SIGUSR1);
}

double max_util(double a,double b){
    return  a>b ? a : b;
}


int main(int argc, char* argv[]){
    if (argc!=1){
        printf("Incorrect call, expected no arguments\n");
        exit(1);
    }
    printf("Catcher running ... PID: %d\n",getpid());
    double time_acc;
    clock_t begin= clock();
    clock_t end = clock();
    bool time_flag= false;
    struct sigaction action;
    sigemptyset(&action.sa_mask);
    action.sa_sigaction = handler;
    action.sa_flags = SA_SIGINFO;
    sigaction(SIGUSR1,&action,NULL);
    while(true){
        if (done) continue;

        if (time_flag){
            double time_delta = max_util(0.0, (double)(end-begin)/CLOCKS_PER_SEC);
            time_acc += time_delta;
            if ( (int) time_acc >0){
                time_acc=0.0;
                print_time();
            }
            begin = clock();
        }

        switch(state){
            case NUM:
                 print_nums();
                 break;
            case CURR_TIME:
                print_time();
                break; 
            case REQUEST_NUM:
                print_request_num();
                break;
            case TIME_EACH_SEC:
                if (!time_flag){
                    time_acc=0;
                }
                time_flag=true;
                break;
            case FINISH:
                terminate();
                break;
        }
        if (state != TIME_EACH_SEC){
            time_flag=false;
            done=true;
        }
        if (time_flag){
            end = clock();
        }

    }

    return 0;
}
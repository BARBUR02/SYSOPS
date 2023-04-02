#include<stdio.h>
#include<signal.h>
#include<stdlib.h>
#include<stdbool.h>
#include<sys/types.h>
#include<time.h>
#include<unistd.h>

bool confirmed = false;

void handler(int signo){
    printf("Received confirmation\n");
    confirmed = true;
}

int main(int argc, char* argv[]){
    if (argc < 3){
        printf("Incorrect call, specify at least 2 arguments <PID> <state>*\n");
        exit(1);
    }
    int catcher_pid = atoi(argv[1]);
    int state;
    for( int i=2;  i<argc; i++){
        state = atoi(argv[i]);
        printf("i: %d, state: %d\n",i,state);

        struct sigaction action;
        sigemptyset(&action.sa_mask);
        action.sa_handler = handler;
        sigval_t sig_val = {state};
        confirmed = false;
        sigaction(SIGUSR1,&action, NULL);

        sigqueue(catcher_pid, SIGUSR1, sig_val);
        clock_t start = clock();
        while (!confirmed){
            if (clock()-start> CLOCKS_PER_SEC*4){
                printf("No confirmation for about 4 second, retrying...\n");
                i--;
                break;
            }
        }

    }


    return 0;
}
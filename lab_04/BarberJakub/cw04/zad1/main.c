#include<stdio.h>
#include<signal.h>
#include<unistd.h>
#include<stdlib.h>
#include<sys/types.h>
#include<string.h>
#include<stdbool.h>
#include<sys/wait.h>

bool parent = true;

typedef enum {
    INVALID,
    IGNORE, 
    HANDLER,
    MASK,
    PENDING
} TASK;

TASK compare_to_str(char* str){
    TASK ans = INVALID;
    if (strcmp(str,"ignore")==0) ans = IGNORE;
    if (strcmp(str,"handler")==0) ans = HANDLER;
    if (strcmp(str,"mask")==0) ans = MASK;
    if (strcmp(str,"pending")==0) ans = PENDING;
    return ans;
}

void block_sig(){
    sigset_t sigset;
    sigemptyset(&sigset);
    sigaddset(&sigset,SIGUSR1);
    sigprocmask(SIG_BLOCK, &sigset, NULL);
}

void is_pending(){
    sigset_t sigset;
    sigpending(&sigset);
    if (sigismember(&sigset,SIGUSR1)){
        printf("PID: %d, signal pending\n",getpid());
    }
    else{
        printf("PID: %d, signal not pending\n",getpid());
    }
}

void action(int sig_n){
    printf("PID: %d, Signal received. \n",getpid());
}

void raise_signal(){
    printf("PID: %d, Raising signal.\n", getpid());
    raise(SIGUSR1);
}

void run_task(TASK task){
    switch(task){
        case IGNORE:
            if (parent){
                signal(SIGUSR1, SIG_IGN);
            }
            raise_signal();
            break;
        case HANDLER:
            if (parent){

                signal(SIGUSR1, action);
            }
            raise_signal();
            break;
        case MASK:
            if (parent){
                signal(SIGUSR1, action);
            }
            if (parent){
                block_sig();
                raise_signal();
            }
            break;
        case PENDING:
            if (parent){

                signal(SIGUSR1, action);
            }
            if (parent){
                block_sig();
                raise_signal();
            }
            is_pending();
            break;
        case INVALID:
            break;
            
    }
}

int main(int argc, char *argv[]){
    if (argc!=2 && argc!=3){
        printf("Incorrect arguments count\n");
        exit(1);
    }

    #ifdef EXEC 
    if (argc ==3){
        if (strcmp(argv[2],"exec")!=0){
            printf("Incorrect call (only accepts exec call)\n");
            exit(1);
        }
    }
    #else 
    if (argc==3){
        printf("Incorrect argument count\n");
        exit(1);
    }
    #endif

    TASK task = compare_to_str(argv[1]);
    if (task == INVALID){
        printf("Invalid program instruction\n");
        exit(1);
    }

    #ifdef EXEC 
    if (task==HANDLER){
        printf("handler option not supported in EXEC mode\n");
        exit(1);
    }
    if (argc==3){
        parent = false;
    }
    #endif

    run_task(task);
    fflush(NULL);

    #ifdef EXEC
        if (parent){
            execl(argv[0],"placeholder",argv[1],"exec",NULL);
        }
    #else 
        pid_t tmp = fork();
        if (tmp ==0){
            parent=false;
            run_task(task);
        }
        else{
            wait(NULL);
        }
    #endif

    fflush(NULL);
    return 0;
}
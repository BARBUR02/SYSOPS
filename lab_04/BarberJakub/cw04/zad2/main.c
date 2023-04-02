#include<stdio.h>
#include<stdlib.h>
#include<signal.h>
#include<sys/wait.h>
#include<unistd.h>

#define MAX_CALLS 3
int call_id=0;

void info_action(int sig_n, siginfo_t* info, void*context){
    printf("Signal number : %d\n", info->si_signo);
    printf("UID : %d\n", info->si_uid);
    printf("PID : %d\n", info->si_pid);
    printf("POSIX timer : %d\n", info->si_timerid);
    printf("Exit val : %d\n", info->si_status);
}

void test_siginfo(struct sigaction action){
    sigemptyset(&action.sa_mask);
    action.sa_sigaction = info_action;
    action.sa_flags = SA_SIGINFO;
    sigaction(SIGUSR1, &action,NULL);
    raise(SIGUSR1);
    printf("\n");
    pid_t tmp = fork();
    if (tmp==0){
        // kill(getpid(),SIGUSR1);
        raise(SIGUSR1);
        exit(0);
    }
    else{
        wait(NULL);
    }
    printf("\n");
}

void nodefer_action(int sig_n, siginfo_t* info, void*context){
    printf("NODEFER callID: %d\n",call_id++);
    if (call_id<MAX_CALLS){
        raise(SIGUSR1);
    }

}

void test_nodefer(struct sigaction action){
    call_id=0;
    sigemptyset(&action.sa_mask);
    action.sa_sigaction = nodefer_action;
    action.sa_flags = SA_NODEFER;
    sigaction(SIGUSR1, &action,NULL);
    
    raise(SIGUSR1);
}

void test_resethand(struct sigaction action){
    call_id=0;
    sigemptyset(&action.sa_mask);
    action.sa_sigaction = nodefer_action;
    action.sa_flags = SA_RESETHAND;
    sigaction(SIGUSR1, &action,NULL);
    raise(SIGUSR1);
}

int main(int argc, char* argv[]){
    if (argc!= 1){
        printf("Invalid argument count\n");
        exit(1);
    }

    struct sigaction action;
    printf("Testing SIGINFO flag : \n");
    test_siginfo(action);

    printf("\nTesting SA_NODEFER\n");
    test_nodefer(action);

    printf("\nTesting SA_RESETHAND\n");
    test_resethand(action);

    return 0;
}
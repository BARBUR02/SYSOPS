#include<stdio.h>
#include<string.h>
#include<stdbool.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>

bool is_digit(char* candidate){
    const char digits[]="0123456789";
    if (strspn(candidate,digits)== strlen(candidate) && candidate[0]!='0'){
        return true;
    }
    return false;
}


int main(int argc, char * argv[]){
    // Validation : 
    if (argc != 2){
        printf("Expected one argument!\n");
        return 1;
    }
    int n;

    if (is_digit(argv[1]) ){
        n = atoi(argv[1]);
    }
    else{
        printf("Given input is not a correct number\n");
        return 1;
    }
    // printf("Given number : %d\n",n);


    pid_t child_pid;
    printf("Main programms PID: %d\n", (int) getpid() );
    int counter = n;
    child_pid = 1;
    while (true){
        if (child_pid!=0){
        if (counter ==0){
            while (wait(NULL)>0);
            break;
        }
        // printf("Proces rodzica: Proces rodzicca ma pid : %d\n", (int)getpid());
        // printf("Proces rodzica: Proces dziecka ma pid : %d\n", (int)child_pid);
        counter-=1;
        child_pid = fork();
        }
        else{
            printf("Proces rodzicca ma pid : %d\tProces dziecka ma pid: %d\n", (int)getppid(),(int)getpid());
            // printf("Proces dziecka ma pid : %d\n", (int)getpid());
            exit(0);
        }
    }
    if (child_pid>0){
        printf("Proces macierzysty, argv[1] : %d\n",n);
    }



    return 0;
}
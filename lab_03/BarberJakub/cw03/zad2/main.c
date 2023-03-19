#include<stdio.h>
#include<string.h>
#include<stdbool.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>



int main(int argc,char* argv[]){
    if (argc != 2 ){
        printf("Incorrect call - expected 1 argument (directory_path)\n");
        return 1;
    }

    printf("%s ",argv[0]);
    fflush(stdout);

    execl("/bin/ls", "ls","-l",argv[1],NULL);


    // pid_t child_pid= fork();
    // if (child_pid==0){
    //     printf("%s",argv[0]);
    //     // execl("/bin/ls", "ls","-l",argv[1],NULL);
    // } else{
    //     while (wait(NULL)>0);
    //     execl("/bin/ls", "ls","-l",argv[1],NULL);
    // }


    return 0;
}
#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<sys/types.h>
#include<signal.h>
#include<unistd.h>
#include<string.h>
#include "memory_utils.h"
#include "sem_utils.h"
#include "queue_util.h"
#include "utils.h"

#define COOLDOWN 4
#define RANGE 2

int semQueue;
int semChairs;
int semBarbers;
int buff_mutex;


int main(){
    // printf("[BARBER] PID: %d\n", getpid());
    srand(time(NULL)+getpid());
    char *queue = attach_memory_block(HOME_P,BUFFER_SIZE);
    if (queue == NULL){
        printf("[OPENING  SHARED MEMORY] error in Barber");
        exit(1);
    }
    semQueue = open_sem(SEM_QUEUE);
    semChairs = open_sem(SEM_CHAIRS);
    semBarbers = open_sem(SEM_BARBER);
    buff_mutex = open_sem(MUTEX);


    printf("[BARBER] PID: %d\n", getpid());
    fflush(stdout);
    sleep(1);

    for (;;){
        // printf("Barber : Queue lengtrh, waiting for : %d \n", strlen(queue));
        wait_sem(semBarbers);
        post_sem(buff_mutex);
        // printf("[BARBER] PID: %d |Curr queue size: %ld\n",getpid(), strlen(queue));
        // wait_sem(buff_mutex);
        char case_id = pop(queue);
        post_sem(buff_mutex); 

        int process_time = rand()%RANGE +1;
        printf("[BARBER] PID: %d | Case started no: %d\n", getpid(),case_id);
        fflush(stdout);
        sleep(process_time);
        printf("[BARBER] PID: %d | Case finished no: %d\n", getpid(),case_id);
        fflush(stdout);

        post_sem(semChairs);
        post_sem(semQueue);

        if (isEmpty(queue)){
            sleep(COOLDOWN);
            // printf("Barber : Queue lengtrh : %d \n", strlen(queue));
            if (isEmpty(queue)){
                break;
            }
        }

    }

    printf("[BARBER] PID: %d , no client waiting -> closing...\n",getpid());
    fflush(stdout);

    detach_memory_block(queue);


    return 0;
}
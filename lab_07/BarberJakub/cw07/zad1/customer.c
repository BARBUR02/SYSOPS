#include<stdio.h>
#include<sys/types.h>
#include<signal.h>
#include<unistd.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>
#include "utils.h"


#include "sem_utils.h"
#include "queue_util.h"
#include "memory_utils.h"

#define COOLDOWN 7
#define RANGE 3

int semQueue;
int semChairs;
int semBarbers;
int buff_mutex;

int main(){
    
    // printf("[CUSTOMER] PID: %d\n", getpid());
    srand(time(NULL)+ getpid());
    char *queue = attach_memory_block(HOME_P,BUFFER_SIZE);
    if (queue == NULL){
        printf("[OPENING  SHARED MEMORY] error in Customer");
        exit(1);
    }
    semQueue = open_sem(SEM_QUEUE);
    semChairs = open_sem(SEM_CHAIRS);
    semBarbers = open_sem(SEM_BARBER);
    buff_mutex = open_sem(MUTEX);


    char case_id = (char) (rand()%128);
     printf("[CUSTOMER] PID: %d | with case_ID: %d\n", getpid(), case_id);
     
    if (strlen(queue) >= QUEUE_SIZE) {
        printf("[CUSTOMER] PID: %d | Queue is full, can't push new client to queue\n",getpid());
        fflush(stdout);
        return 0;
    }
    //  else{
    //     push(queue,case_id);
    // }

    // printf("[CUSTOMER] PID: %d\n", getpid());
    // wait_sem(buff_mutex);
    // if (isFull(queue)){
    //     printf("[CUSTOMER] PID: %d | Queue is full, can't push new client to queue\n",getpid());
    //     post_sem(buff_mutex);
    //     return 0;
    // }
    // push(queue,case_id);
    // post_sem(buff_mutex);
    fflush(stdout);
    wait_sem(semQueue);
    wait_sem(buff_mutex);
    // char case_id = (char) (rand()%128);
    // post_sem(buff_mutex);
    // printf("[CUSTOMER] PID: %d | with case_ID: %d\n", getpid(), case_id);
    // usleep(100);
    fflush(stdout);
    // if (isFull(queue)){
    //     printf("[CUSTOMER] PID: %d | Queue is full, can't push new client to queue\n",getpid());
    //     return 0;
    // }
    push(queue,case_id);
    post_sem(buff_mutex);

    post_sem(semBarbers);
    wait_sem(semChairs);

    printf("[CUSTOMER] PID: %d Done...\n", getpid());
    fflush(stdout);


    detach_memory_block(queue);


    return 0;
}
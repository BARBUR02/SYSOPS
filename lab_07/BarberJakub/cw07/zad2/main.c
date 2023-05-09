#include<stdio.h>
#include<sys/ipc.h>
#include<sys/shm.h>
#include<sys/sem.h>
#include<sys/types.h>
#include<unistd.h>
#include "utils.h"
#include<sys/wait.h>
#include<wait.h>

#include "utils.h"
#include "memory_utils.h"
#include "queue_util.h"
#include "sem_utils.h"
#include<semaphore.h>

sem_t * semQueue;
sem_t * semChairs;
sem_t * semBarbers;
sem_t * buff_mutex;

int main(){
    printf("[Simulation Programm]\nBARBERS count: %d\nQUEUE count: %d\nCHAIR COUNT: %d\nCUSTOMER count:%d\n",
        BARBER_COUNT, QUEUE_SIZE, CHAIR_COUNT ,CUSTOMER_COUNT
    );
    fflush(stdout);

    char *shared = attach_memory_block(HOME_P, BUFFER_SIZE);
    if (shared == NULL){
        exit(1);
    }
    shared[0] = '\0';

    //unlinking
    sem_unlink(SEM_BARBER);
    sem_unlink(SEM_QUEUE);
    sem_unlink(SEM_CHAIRS);
    sem_unlink(MUTEX);

    // creating
    semBarbers =  create_sem(SEM_BARBER, 0);
    semQueue =  create_sem(SEM_QUEUE, CHAIR_COUNT);
    semChairs = create_sem(SEM_CHAIRS,0);
    buff_mutex =  create_sem(MUTEX,1);

    //  Creating barbers:
    for (int i=0;i<BARBER_COUNT;i++){
        if (fork()==0){
        execl(BARBER_EXEC, BARBER_EXEC,NULL);
        }
    }
    sleep(1);
    // while(wait(NULL)>0);
    printf("Barbers processing finished ...\n");
    fflush(stdout);


     //  Creating customers:
    for (int i=0;i<CUSTOMER_COUNT;i++){
        if (fork()==0){
        execl(CUSTOMER_EXEC, CUSTOMER_EXEC,NULL);
    }
    }
    // while(wait(NULL)>0);
    printf("Customers processing finished ...\n");
    fflush(stdout);

    while(wait(NULL)>0);


    if (!destroy_memory_block(HOME_P)){
        printf("[DESTRUCTION ERROR] shared memory release error\n");
        return 1;
    }

    sem_close(semBarbers);
    sem_close(semQueue);
    sem_close(semChairs);
    sem_close(buff_mutex);
    printf("[SIMULATION] Simulation finished!\n");
    fflush(stdout);

    return 0;
}

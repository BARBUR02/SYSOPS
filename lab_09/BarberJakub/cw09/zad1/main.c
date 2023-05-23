#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<pthread.h>
#include<stdbool.h>


pthread_mutex_t simulationMutex;
pthread_cond_t condCheckSanta;

pthread_barrier_t reindeerBarrier;
pthread_barrier_t elfBarrier;


int elfWaitingCount=0;
int reindeerWaitingCount=0;


void * elf_func(void * arg){
    // Elfs are working ...
    int* id = (int *) arg;
    while (true){
        time_t workingTime = rand()%4 +2;
        sleep(workingTime);
        pthread_mutex_lock(&simulationMutex);
        if (elfWaitingCount < 3){
            elfWaitingCount++;

            if (elfWaitingCount == 3){
                printf("[ELF ID: %d ] Waking Santa up!\n", *id);
            }
            else{
                printf("[ELF ID: %d ] Waiting for Santa ...\n", *id);
            }
            pthread_cond_signal(&condCheckSanta);
        } else{
            printf("[ELF ID: %d ] All slots taken, solving problem on my own ...\n", *id);
            pthread_mutex_unlock(&simulationMutex);
            continue;
        }
        pthread_mutex_unlock(&simulationMutex);
        pthread_barrier_wait(&elfBarrier);
    }
    free(id);
    return NULL;
}

void * reindeer_func(void * arg){
    int* id = (int *) arg;
    while (true){
        // Reindeers are on holiday ...
        time_t holidayTime = rand()%6 +5;
        printf("[REINDEER ID: %d ] On holiday ...\n", *id);
        sleep(holidayTime);
        pthread_mutex_lock(&simulationMutex);
        reindeerWaitingCount+=1;
        if (reindeerWaitingCount==9){
            printf("[REINDEER ID: %d ] Waking Up Santa ...\n", *id);
        } else{
            printf("[REINDEER ID: %d ] Back from holiday ...\n",*id);
        }
        pthread_cond_signal(&condCheckSanta);
        pthread_mutex_unlock(&simulationMutex);

        pthread_barrier_wait(&reindeerBarrier);
    }

    free(id);
    return NULL;
}

int main(){
    srand(time(NULL));
    pthread_barrier_init(&reindeerBarrier, NULL , 10);
    pthread_barrier_init(&elfBarrier, NULL , 4);

    pthread_t elfs[5];
    pthread_t reindeer[9];

    // Mutexes
    pthread_mutex_init(&simulationMutex, NULL);
    

    // Condition variables
    pthread_cond_init(&condCheckSanta, NULL);

    
    for ( int  i=0;i<5;i++){
        int * id = malloc(sizeof(int));
        *id = i;
        pthread_create(&elfs[i],NULL, elf_func,id);
    }

    for ( int  i=0;i<10;i++){
        int * id = malloc(sizeof(int));
        *id = i;
        pthread_create(&reindeer[i],NULL, reindeer_func,id);
    }
    
    while (true){
        pthread_mutex_lock(&simulationMutex);
        // santa waiting for either of 2 breakpoints
        while ( reindeerWaitingCount< 9 && elfWaitingCount<3){
            pthread_cond_wait(&condCheckSanta, &simulationMutex);
        }
        // we have higher priority on helping elves,
        // Then we worry about providing gifts
        if (elfWaitingCount == 3){
            printf("[SANTA] Santa is awake. Helping elves ...\n");
            time_t napTime = rand()%2+1;
            sleep(napTime);
            elfWaitingCount=0;
            pthread_barrier_wait(&elfBarrier);
        }if (reindeerWaitingCount==9){
            printf("[SANTA] Santa is awake. Providing gifts ...\n");
            time_t napTime = rand()%3+2;
            sleep(napTime);
            reindeerWaitingCount=0;
            pthread_barrier_wait(&reindeerBarrier);
        }
        
        pthread_mutex_unlock(&simulationMutex);

    }

    // Destroying created structures
    pthread_cond_destroy(&condCheckSanta);
    return 0;
}
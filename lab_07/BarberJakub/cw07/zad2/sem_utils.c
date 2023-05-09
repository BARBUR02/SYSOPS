#include "sem_utils.h"
#include "utils.h"
#include<semaphore.h>

sem_t * create_sem(char * filename, int initial){
    //  getenv("HOME")
    // union semun arg;
    sem_t * semaphore = sem_open(filename, O_CREAT | O_EXCL, 0644, initial);
    
    
    if (semaphore == SEM_FAILED){
        printf("[SEM_OPEN] error in createing semaphore\n  ");
        return NULL;
    }
    return semaphore;

}
sem_t * open_sem(char * filename){
    sem_t * semaphore = sem_open(filename, 0);
    if (semaphore==SEM_FAILED){
        printf("[SEMO_PEN] Couldn't open semaphore\n");
        return NULL;
    }
    return semaphore;
}








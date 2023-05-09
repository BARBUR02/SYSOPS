#include "sem_utils.h"
#include "utils.h"

int create_sem(char * filename, int initial){
    //  getenv("HOME")
    // union semun arg;
    union semun {
        int val;
        struct semid_ds *buf;
        ushort array[1];
    } arg;
    arg.val = initial;
    key_t key = ftok( HOME_P,filename[0]);
    if (key ==-1){
        printf("[FTOK] error in create semaphore\n  ");
        return -1;
    }
    int sem_id = semget(key, 1, 0664 | IPC_CREAT);
    if (sem_id==-1){
        printf("[SEMGET] error in create semaphore\n");
        return -1;
    } 
    if (semctl(sem_id,0,SETVAL,arg)==-1){
        printf("[SEMCTL] error in create semaphore\n");
        return -1;
    }
    return sem_id;

}
int open_sem(char * filename){
    key_t key = ftok(HOME_P,filename[0]);
    if (key==-1){
        printf("[FTOK] Couldn't open semaphore\n");
        return -1;
    }
    return semget(key,1,0);
}


void close_sem(int sem_id){}

void unlink_sem(char *filename){
    int sem_id = open_sem(filename);
    if (sem_id==-1){
        printf("[UNLINK ERROR] failed to unlink semaphore \n");
        return;
    }
    semctl(sem_id,0,IPC_RMID);
}

void wait_sem(int sem_id){
    struct sembuf operation = { 0 , -1, 0};
    if (semop(sem_id,&operation,1)== -1){
        printf("[SEMOP] error in WAIT_sem\n");
    }
}


void post_sem(int sem_id){
    struct sembuf operation = { 0 , 1, 0};
    if (semop(sem_id,&operation,1)== -1){
        printf("[SEMOP] error in POST_sem\n");
    }
}
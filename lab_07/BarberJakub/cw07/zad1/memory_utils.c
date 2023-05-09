#include<stdlib.h>
#include<unistd.h>
#include<stdio.h>
#include<sys/stat.h>
#include<sys/shm.h>
#include<sys/ipc.h>

#include "memory_utils.h"

#define IPC_RESULT_ERROR (-1)


// code made along with YT tutorial Jacob Sorber channel

static int get_shared_block(char * filename, int size){
    key_t key;

    // requesting the key which is linked to a filename so other programms will access it

    key = ftok(filename,0);
    if (key == -IPC_RESULT_ERROR){
        return IPC_RESULT_ERROR;
    }

    // getting shared block -> (will cereate it if it doesn't exist yet)
    return shmget(key,size,0644 | IPC_CREAT);
}


char * attach_memory_block(char *filename, int size){
    int shared_block_id = get_shared_block(filename,size);
    char *result;

    if (shared_block_id == IPC_RESULT_ERROR){
        return NULL;
    }

    result = shmat(shared_block_id,NULL,0);
    if (result == (char*)IPC_RESULT_ERROR){
        return NULL;
    }
    return result;
}


bool detach_memory_block(char * block){
    return (shmdt(block) != IPC_RESULT_ERROR);
}


bool destroy_memory_block(char *filename){
    int shared_block_id = get_shared_block(filename,0);

    if (shared_block_id == IPC_RESULT_ERROR){
        return NULL;
    }
    return (shmctl(shared_block_id, IPC_RMID, NULL) != IPC_RESULT_ERROR);
}


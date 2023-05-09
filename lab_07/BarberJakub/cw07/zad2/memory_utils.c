#include<stdlib.h>
#include<unistd.h>
#include<stdio.h>
#include<sys/stat.h>
#include<sys/mman.h>
#include<sys/types.h>
#include<sys/shm.h>
#include<sys/ipc.h>
#include<fcntl.h>

#include "memory_utils.h"

#define IPC_RESULT_ERROR (-1)


// code made along with YT tutorial Jacob Sorber channel

static int get_shared_block(char * filename, int size){
    // key_t key;

    // requesting the key which is linked to a filename so other programms will access it

    int desc = shm_open(filename,O_CREAT | O_RDWR, 0644);
    if (desc == -1){
        return -1;
    }

    if ( ftruncate(desc,size)==-1){
        printf("[TRUNCATE] error\n");
        return -1;
    }

    // getting shared block -> (will cereate it if it doesn't exist yet)
    return desc;
}


char * attach_memory_block(char *filename, int size){
    int shared_block_id = get_shared_block(filename,size);
    char *result;

    if (shared_block_id == -1){
        return NULL;
    }

    result = mmap(0,size,PROT_READ | PROT_WRITE, MAP_SHARED,shared_block_id,0);
    // if (result == (char*)IPC_RESULT_ERROR){
    //     return NULL;
    // }
    return result;
}


bool detach_memory_block(char * block){
    return (shmdt(block) != IPC_RESULT_ERROR);
}


bool destroy_memory_block(char *filename){
    // int shared_block_id = get_shared_block(filename,0);

    // if (shared_block_id == IPC_RESULT_ERROR){
    //     return NULL;
    // }
    return (shm_unlink(filename) != -1);
}


#ifndef SEM_UTILS_H
#define SEM_UTILS_H

#include<stdio.h>
#include<sys/types.h>
#include<sys/sem.h>
#include<fcntl.h>
#include<sys/ipc.h>
#include<semaphore.h>



sem_t * create_sem(char * filename, int initial);
sem_t*  open_sem(char * filename);

#endif
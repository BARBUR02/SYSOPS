#ifndef SEM_UTILS_H
#define SEM_UTILS_H

#include<stdio.h>
#include<sys/types.h>
#include<sys/sem.h>
#include<fcntl.h>
#include<sys/ipc.h>



int create_sem(char * filename, int initial);
int open_sem(char * filename);
void close_sem(int sem_id);
void wait_sem(int sem_id);
void post_sem(int sem_id);
void unlink_sem(char *filename);

#endif
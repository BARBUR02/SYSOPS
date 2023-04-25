#ifndef UTILS_H
#define UTILS_H

#include<stdio.h>
#include<string.h>
#include<string.h>
#include<time.h>
#include<mqueue.h>
#include<signal.h>
#include <stdlib.h>

#include<fcntl.h>
// #include <stdlib.h>
#include<sys/types.h>
#include<sys/stat.h>
// #include<errno.h>


#define SERVER_QUEUE "/server_queue"
#define SERVER_ID 1
#define MAX_MESSAGE_SIZE 512
#define MAX_CLIENT_COUNT 10
#define ID_LEN 3

char get_randomc(){
    return rand()%('Z'-'A'+1)+'A';
}

typedef enum MessageType {
    INIT = 1,
    LIST = 2,
    TOONE = 3,
    TOALL = 4,
    STOP = 5
} MessageType;


typedef struct MessageBuff{
    int client_id;
    int other_id;
    long type;
    char content[MAX_MESSAGE_SIZE];
    struct tm time_struct;
}MessageBuff;


#endif
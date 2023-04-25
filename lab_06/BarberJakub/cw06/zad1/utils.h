#ifndef UTILS_H
#define UTILS_H

#include<stdio.h>
#include<string.h>
#include<string.h>
#include<time.h>
#include<sys/msg.h>
#include<signal.h>


#define HOME_P getenv("HOME")
#define SERVER_ID 1
#define MAX_CLIENT_COUNT 10
#define MAX_MESSAGE_SIZE 512

typedef enum MessageType {
    INIT = 1,
    LIST = 2,
    TOONE = 3,
    TOALL = 4,
    STOP = 5
} MessageType;


typedef struct MessageBuff{
    long type;
    key_t queue_key;
    int client_id;
    int other_id;
    char content[MAX_MESSAGE_SIZE];
    struct tm time_struct;
}MessageBuff;


#endif
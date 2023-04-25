#include<stdio.h>
#include<string.h>
#include<string.h>
#include<time.h>
#include<stdlib.h>
#include<sys/msg.h>
#include<signal.h>

#include"utils.h"

int first_avaiable_ID = 0;
int squeue;
key_t clientQueues[MAX_CLIENT_COUNT];

void _INIT(MessageBuff *messBuff);

void _STOP(int client_ID);

void _EXIT();

void _LIST(int clientID);

void _TO_ALL(MessageBuff *messBuff);

void _TO_ONE(MessageBuff *messBuff);

void store_message_logs(MessageBuff *messBuff);



int main(){
    for (int i=0; i<MAX_CLIENT_COUNT;i++){
        clientQueues[i]=-1;
    }

    key_t queue_key = ftok(HOME_P, SERVER_ID);
    squeue = msgget(queue_key, IPC_CREAT | 0666);
    printf("[SERVER] SERVER KEY : %d\n",queue_key);
    printf("[SERVER] QUEUE KEY : %d\n",squeue);
    signal(SIGINT, _EXIT);

    MessageBuff *messbuff = malloc(sizeof(MessageBuff));
    while(1){
        int val =msgrcv(squeue, messbuff, sizeof(MessageBuff), -6,0);
        if (val==-1){
            printf("ERROR in server msgrcv\n");
        }
        switch(messbuff->type){

            case INIT:
                printf("[SERVER] Received INIT\n");
                _INIT(messbuff);
                break;

            case LIST:
                printf("[SERVER] Received LIST\n");
                _LIST(messbuff->client_id);
                store_message_logs(messbuff);
                break;
            case TOALL:
                printf("[SERVER] Received TOALL\n");
                _TO_ALL(messbuff);
                store_message_logs(messbuff);
                break;
            case TOONE:
                printf("[SERVER] Received TOONE\n");
                _TO_ONE(messbuff);
                store_message_logs(messbuff);
                break;
            case STOP:
                _STOP(messbuff->client_id);
                store_message_logs(messbuff);
                break;
            default:
                printf("TYPE : %ld\nMESSAGE: %s\nCLIENT: %d\n OTHER: %d", messbuff->type, messbuff->content,messbuff->client_id,messbuff->other_id);
                printf("Unhandled message type!\n");

        }


    }

    return 0;
}


void _INIT(MessageBuff *messBuff){
    while (clientQueues[first_avaiable_ID]!=-1 && first_avaiable_ID < MAX_CLIENT_COUNT-1){
        first_avaiable_ID++;
    }

    if (clientQueues[first_avaiable_ID] != -1 && first_avaiable_ID == MAX_CLIENT_COUNT-1 ){
        messBuff  ->client_id =-1;
    } else{
        messBuff -> client_id = first_avaiable_ID;
        clientQueues[first_avaiable_ID] = messBuff->queue_key;

        if (first_avaiable_ID<MAX_CLIENT_COUNT-1){
            first_avaiable_ID++;
        }
    }

    int clientQueueID = msgget(messBuff->queue_key,0);
    msgsnd(clientQueueID, messBuff, sizeof(MessageBuff),0);
    store_message_logs(messBuff);

}

void _STOP(int client_ID){
    clientQueues[client_ID]=-1;

    if (client_ID<first_avaiable_ID){
        first_avaiable_ID=client_ID;
    }
}

void _EXIT(){
    MessageBuff *messBuff = malloc(sizeof(MessageBuff));
    for (int i=0;i<MAX_CLIENT_COUNT;i++){
        printf("In LOOP...\n");
        if (clientQueues[i]!= -1){
            messBuff->type = STOP;
            int clientQueueID = msgget(clientQueues[i],0);
            msgsnd(clientQueueID, messBuff,sizeof(MessageBuff),0);
            msgrcv(squeue, messBuff,sizeof(MessageBuff),STOP,0);
            printf("Mess received : ) \n");
        }
    }
    msgctl(squeue, IPC_RMID, NULL);
    exit(0);
}

void _LIST(int clientID){
    MessageBuff *messBuff = malloc(sizeof(MessageBuff));
    strcpy(messBuff->content,"");

    for (int i=0 ; i< MAX_CLIENT_COUNT; i++){
        if (clientQueues[i]!=-1){
            sprintf(messBuff->content + strlen(messBuff->content),"ID %d is running...\n",i);
        }
    }

    messBuff->type = LIST;
    int clientQueueID = msgget(clientQueues[clientID],0);
    msgsnd(clientQueueID, messBuff, sizeof(MessageBuff),0);
}

void _TO_ALL(MessageBuff *messBuff){
    for (int i=0 ; i<MAX_CLIENT_COUNT;i++){
        if (clientQueues[i]!= -1 && i != messBuff->client_id){
            int otherQueueID = msgget(clientQueues[i],0);
            msgsnd(otherQueueID,messBuff,sizeof(MessageBuff),0);
        }
    }

}

void _TO_ONE(MessageBuff *messBuff){
    int otherQueueID = msgget(clientQueues[messBuff->other_id],0);
    msgsnd(otherQueueID,messBuff,sizeof(MessageBuff),0);
}

void store_message_logs(MessageBuff *messBuff){
    struct tm tmp_time = messBuff->time_struct;

    FILE *result_file = fopen("logs.txt", "a");

    switch (messBuff->type) {
        case INIT:
            if (messBuff->client_id == -1) {
                fprintf(result_file, "(INIT) Max number of clients is reached!\n");
            } else {
                fprintf(result_file, "MESSAGE : INIT Client ID: %d\n", messBuff->client_id);
            }
            break;
        case LIST:
            fprintf(result_file, "MESSAGE : LIST Client ID: %d\n", messBuff->client_id);
            break;
        case TOALL:
            fprintf(result_file, "Message: %s\n", messBuff->content);
            fprintf(result_file, "MESSAGE : 2ALL Client ID: %d\n", messBuff->client_id);
            break;
        case TOONE:
            fprintf(result_file, "Message: %s\n", messBuff->content);
            fprintf(result_file, "MESSAGE :2ONE Sender ID: %d, Receiver ID %d\n", messBuff->client_id, messBuff->other_id);
            break;
        case STOP:
            fprintf(result_file, "MESSAGE : STOP Client ID: %d\n", messBuff->client_id);
            break;
    }

    fprintf(result_file, "was sent at: %02d:%02d:%02d\n\n\n",
            tmp_time.tm_hour,
            tmp_time.tm_min,
            tmp_time.tm_sec);

    fclose(result_file);
}
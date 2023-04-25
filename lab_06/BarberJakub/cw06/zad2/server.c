#include<stdio.h>
#include<string.h>
#include<string.h>
#include<time.h>
#include<mqueue.h>
#include<signal.h>
#include"utils.h"
#include<fcntl.h>
#include <stdlib.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<errno.h>

mqd_t serverQueue;
char *clientQueues[MAX_CLIENT_COUNT];
int first_available_ID = 0;

void _INIT(MessageBuff mess);
void _STOP(int clientID);
void _EXIT();
void _LIST(int cID);
void _TOALL(MessageBuff mess);
void _TOONE(MessageBuff mess);
void _LOG(MessageBuff mess);


int main(){
    for ( int i=0; i<MAX_CLIENT_COUNT;i++){
        clientQueues[i]= NULL;
    }
    // mq_unlink(SERVER_QUEUE);
    struct mq_attr attr;
    // attr.mq_flags=0;
    // attr.mq_msgsize= sizeof(MessageBuff);
    attr.mq_msgsize= sizeof(MessageBuff);
    attr.mq_maxmsg = MAX_CLIENT_COUNT;
    // attr.mq_maxmsg =20;

    // char *test = "/test_queue";

    // printf("Before opning damn ququq\n");
    // if (errno !=0){
    //     printf("Error occured BEFORE\n");
    //     perror("Blad\n");
    // }
    printf("looking for path: %s\n ", SERVER_QUEUE);
    serverQueue =  mq_open(SERVER_QUEUE,  O_RDWR | O_CREAT  ,0666, &attr);
    // serverQueue =  mq_open(SERVER_QUEUE,  O_RDWR | O_CREAT  ,0666, NULL);
    // mq_setattr(serverQueue, &attr, NULL);

    // if (serverQueue==-1){
    //     printf("FAILURE!!!\n");
    // }
    // attr.mq_curmsgs = 0;
    // while ( (serverQueue =  mq_open(SERVER_QUEUE,  O_RDWR | O_CREAT  ,0666,&attr) == -1) ){
        // printf("Loading queue...\n");
        // sleep(0.2);
    // };
    MessageBuff messBuff;
    if (errno !=0){
        
        printf("Blad: %s\n", strerror(errno));
    }
    signal(SIGINT,_EXIT);
    printf("Server Queue ID : %d",serverQueue);

    while(1){
        // printf("Before receiving ...\n");
        // while (mq_receive(serverQueue, (char *) &messBuff, sizeof(MessageBuff),NULL) ==-1) {
            // printf("Error: %s\n",strerror(errno));
        // };
        // if ( mq_receive(serverQueue, (char *) &messBuff, sizeof(MessageBuff),NULL) ) continue;
        mq_receive(serverQueue, (char *) &messBuff, sizeof(MessageBuff),NULL);
        // printf("Server Queue ID : %d",serverQueue);
        // printf("After receiving ...\n");

        MessageType type= messBuff.type;
        switch(type){
            case INIT:
                printf("Received INIT\n");
                _INIT(messBuff);
                break;
            case LIST:
                printf("Received LIST\n");
                _LIST(messBuff.client_id);
                _LOG(messBuff);
                break;
            case TOONE:
                printf("Received TOONE\n");
                _TOONE(messBuff);
                _LOG(messBuff);
                break;
            case TOALL:
                printf("Received TOALL\n");
                _TOALL(messBuff);
                _LOG(messBuff);
                break;
            case STOP:
                printf("Received STOP\n");
                _STOP(messBuff.client_id);
                _LOG(messBuff);
                break;
            default:
                printf("Received unknown command\n");
        }

    }

    return 0;
}



void _INIT(MessageBuff mess){
    while (clientQueues[first_available_ID ]!=NULL && first_available_ID   < MAX_CLIENT_COUNT-1){
        first_available_ID ++;
    }

    if (clientQueues[first_available_ID ] != NULL && first_available_ID  == MAX_CLIENT_COUNT-1 ){
        mess.client_id =-1;
    } else{
        mess.client_id = first_available_ID ;
        clientQueues[first_available_ID ] = (char *) calloc(ID_LEN,sizeof(char)); 
        strcpy(clientQueues[first_available_ID ], mess.content);

        if (first_available_ID <MAX_CLIENT_COUNT-1){
            first_available_ID ++;
        }
    }

    mqd_t clientQueueKey = mq_open(mess.content, O_RDWR );
    // printf("In server INIT: %d\n",clientQueueKey);
    mq_send(clientQueueKey, (char *) &mess, sizeof(MessageBuff),0);
    mq_close(clientQueueKey);
    _LOG(mess);

}
void _STOP(int clientID){
    clientQueues[clientID]= NULL;
    if (clientID<first_available_ID){
        first_available_ID = clientID;
    }
}
void _EXIT(){
    MessageBuff mess;
    for (int i=0;i<MAX_CLIENT_COUNT;i++){
        if (clientQueues[i]==NULL) continue;
        mess.type = STOP;
        mqd_t clientQueueKey = mq_open(clientQueues[i], O_RDWR );
        mq_send(clientQueueKey, (char *) &mess, sizeof(MessageBuff),0);
        mq_receive(serverQueue,(char *) &mess, sizeof(MessageBuff),NULL);
        mq_close(clientQueueKey);
    }

    mq_close(serverQueue);
    mq_unlink(SERVER_QUEUE);
    exit(0);
}

void _LIST(int clientID){
    MessageBuff mess;
    strcpy(mess.content,"");
    for (int i=0 ; i< MAX_CLIENT_COUNT; i++){
        if (clientQueues[i]!=NULL){
            sprintf(mess.content + strlen(mess.content),"ID %d is running...\n",i);
        }
    }
    mqd_t clientQueueKey = mq_open(clientQueues[clientID], O_RDWR );
    mq_send(clientQueueKey, (char *) &mess, sizeof(MessageBuff),0);
    mq_close(clientQueueKey);
}
void _TOALL(MessageBuff mess){
    for (int i=0;i<MAX_CLIENT_COUNT;i++){
        if (i == mess.client_id || clientQueues[i]==NULL  ) continue;
        mqd_t receiver_clientQueueKey = mq_open(clientQueues[i], O_RDWR );
        mq_send(receiver_clientQueueKey, (char *) &mess, sizeof(MessageBuff),0);
        mq_close(receiver_clientQueueKey);

    }


}
void _TOONE(MessageBuff mess){
    mqd_t receiver_clientQueueKey = mq_open(clientQueues[mess.other_id], O_RDWR );
    mq_send(receiver_clientQueueKey, (char *) &mess, sizeof(MessageBuff),0);
    mq_close(receiver_clientQueueKey);

}
void _LOG(MessageBuff mess){
    struct tm tmp_time = mess.time_struct;

    FILE *result_file = fopen("logs.txt", "a");

    MessageType type = mess.type;
    switch (type) {
        case INIT:
            if (mess.client_id == -1) {
                fprintf(result_file, "MESSAGE : INIT Max number of clients is reached!\n");
            } else {
                fprintf(result_file, "MESSAGE : INIT Client ID: %d\n", mess.client_id);
            }
            break;
        case LIST:
            fprintf(result_file, "MESSAGE : LIST Client ID: %d\n", mess.client_id);
            break;
        case TOALL:
            fprintf(result_file, "Message: %s\n", mess.content);
            fprintf(result_file, "MESSAGE : 2ALL Client ID: %d\n", mess.client_id);
            break;
        case TOONE:
            fprintf(result_file, "Message: %s\n", mess.content);
            fprintf(result_file, "MESSAGE : 2ONE Sender ID: %d, Receiver ID %d\n", mess.client_id, mess.other_id);
            break;
        case STOP:
            fprintf(result_file, "MESSAGE : STOP Client ID: %d\n", mess.client_id);
            break;
    }

    fprintf(result_file, "was sent at: %02d:%02d:%02d\n\n\n",
            tmp_time.tm_hour,
            tmp_time.tm_min,
            tmp_time.tm_sec);

    fclose(result_file);

}

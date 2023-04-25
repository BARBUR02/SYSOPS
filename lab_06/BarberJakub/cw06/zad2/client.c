#include<stdio.h>
#include<string.h>
#include<string.h>
#include<time.h>
#include<mqueue.h>
#include<sys/msg.h>
#include<sys/time.h>
#include<signal.h>
#include <stdlib.h>
#include<fcntl.h>
#include"utils.h"


int clientID;
char queueName[ID_LEN];
mqd_t localQueueKey;
mqd_t serverQueueKey;


int init_handler();
void stop_handler();
void server_message_handler();
void list_handler();
void to_one_handler(int other_id, char* message);
void to_all_handler(char * mess);

void generate_queue_name(){
    queueName[0]='/';
    for (int i=1; i<ID_LEN;i++){
        queueName[i] = get_randomc();
    }
}



int main(){
    srand(time(NULL));
    generate_queue_name();
    printf("My new name : %s\n",queueName);
    // mq_unlink(queueName);
    serverQueueKey = mq_open(SERVER_QUEUE, O_RDWR);
    struct mq_attr attr;
    // attr.mq_msgsize= sizeof(MessageBuff);
    attr.mq_flags =0;
    attr.mq_maxmsg = MAX_CLIENT_COUNT;
    attr.mq_msgsize= sizeof(MessageBuff);
    // attr.mq_maxmsg = MAX_CLIENT_COUNT;
    localQueueKey =  mq_open(queueName, O_RDWR | O_CREAT,0666,&attr);
    // mq_setattr(localQueueKey,&attr, NULL);
    
    clientID = init_handler();
    signal(SIGINT, stop_handler);
    size_t len =0;
    ssize_t read;
    char * command = NULL;
    while (1){
        printf("User command: ");
        read = getline(&command, &len, stdin);
        command[read-1]='\0';

        server_message_handler();

        if (strcmp(command,"")==0){
            continue;
        }

        char *curr_command = strtok(command," ");
        if (strcmp(curr_command,"LIST")==0){
            list_handler();
        }
        else if (strcmp(curr_command,"TOONE")==0){
            curr_command = strtok(NULL," ");
            int dest_ID = atoi(curr_command);
            curr_command = strtok(NULL," ");
            char *message = curr_command;
            to_one_handler(dest_ID,message);
        }
        else if (strcmp(curr_command,"TOALL")==0){
            curr_command = strtok(NULL," ");
            char *message = curr_command;
            to_all_handler(message);
        }else{
            printf("Unknokn command\n");
        }


    }


    return 0;
}


int init_handler(){

    time_t time_tmp = time(NULL);
    MessageBuff messBuff;

    messBuff.time_struct = *localtime(&time_tmp);
    messBuff.type = INIT;
    
    strcpy(messBuff.content,queueName);
    printf("SENT in messbuff conent : %s\n",messBuff.content);
    printf("In server INIT local queueKey: %d\n",localQueueKey);
    mq_send(serverQueueKey, (char *) &messBuff, sizeof(MessageBuff),0);
    mq_receive(localQueueKey, (char *) &messBuff, sizeof(MessageBuff),NULL);
    
    
    
    
    int client_id = messBuff.client_id;
    if (client_id == -1){
        printf("Client limit had been reached ... leaving ...\n");
        exit(0);
    }
    printf("My client ID: %d\n", client_id);
    return client_id;
}




void list_handler(){
    time_t time_tmp = time(NULL);
    MessageBuff messBuff;

    messBuff.time_struct = *localtime(&time_tmp);
    messBuff.type = LIST;
    messBuff.client_id = clientID;
    // printf("%d",serverQueueKey);

    mq_send(serverQueueKey,(char *) &messBuff, sizeof(MessageBuff),0);
    mq_receive(localQueueKey, (char *) &messBuff, sizeof(MessageBuff),NULL);
    printf("%s\n", messBuff.content);
}

void stop_handler(){
    time_t time_tmp = time(NULL);
    MessageBuff messBuff;

    messBuff.time_struct = *localtime(&time_tmp);
    messBuff.type = STOP;
    messBuff.client_id = clientID;
    
    mq_send(serverQueueKey, (char *) &messBuff, sizeof(MessageBuff),0);
    mq_close(serverQueueKey);
    // mq_close(localQueueKey);
    exit(0);
}


void server_message_handler(){
    MessageBuff messBuff;
    struct timespec tmp;
    clock_gettime(CLOCK_REALTIME, &tmp);
    tmp.tv_sec += 0.1;
    while (mq_timedreceive(localQueueKey,(char*) &messBuff, sizeof(MessageBuff),NULL,&tmp)!=-1){
        if (messBuff.type ==STOP){
            printf("Got STOP message from server, exiting ...\n");
            stop_handler();
        }else{
            struct tm tmp_time = messBuff.time_struct;
            printf("Message from %d has been sent at %02d:%02d:%02d\nCONTENT:%s\n", messBuff.client_id,
            tmp_time.tm_hour,
            tmp_time.tm_min,
            tmp_time.tm_sec,
            messBuff.content);
        }
    }
}


void to_one_handler(int other_id, char* mess){
    time_t time_tmp = time(NULL);
    MessageBuff messBuff;

    messBuff.time_struct = *localtime(&time_tmp);
    messBuff.type = TOONE;
    strcpy(messBuff.content, mess);
    messBuff.client_id = clientID;
    messBuff.other_id = other_id;

    mq_send(serverQueueKey, (char *) &messBuff, sizeof(MessageBuff),0);
}

void to_all_handler(char * mess){
    time_t time_tmp = time(NULL);
    MessageBuff messBuff;

    messBuff.time_struct = *localtime(&time_tmp);
    messBuff.type = TOALL;
    strcpy(messBuff.content, mess);
    messBuff.client_id = clientID;

    mq_send(serverQueueKey, (char *) &messBuff, sizeof(MessageBuff),0);
}


#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>
#include<sys/msg.h>
#include<signal.h>
#include"utils.h"


key_t queueKey;
int queueID;
int serverqueueID;
int client_id;

int init_handler();
void stop_handler();
void server_message_handler();
void list_handler();
void to_one_handler(int other_id, char* message);
void to_all_handler(char * mess);



int main(){
    srand(time(NULL));

    queueKey = ftok(HOME_P, rand()%255+1);
    queueID = msgget(queueKey, IPC_CREAT | 0666);
    key_t serverKey = ftok(HOME_P,SERVER_ID);
    // printf("SERVER KEY : %d\n",serverKey);
    // printf("BEFORE msgget\n");
    serverqueueID = msgget(serverKey,0);
    // printf("SERVER QUEUE: %d\n",serverqueueID);
    // printf("AFTER\n");
    client_id = init_handler();

    signal(SIGINT, stop_handler);

    size_t len =0;
    ssize_t read;
    char* command = NULL;
    // printf("BEFORE LOOP\n");
    while(1){
        printf("COMMAND>>  ");
        read = getline(&command, &len, stdin);
        command[read-1] ='\0';
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
    MessageBuff *messBuff = malloc(sizeof(MessageBuff));

    messBuff -> time_struct = *localtime(&time_tmp);
    messBuff -> type = INIT;
    messBuff -> queue_key = queueKey;

    msgsnd(serverqueueID, messBuff, sizeof(MessageBuff),0);
    // printf("Watiing for msfrcv ... : \n");
    msgrcv(queueID, messBuff, sizeof(MessageBuff),0,0);

    int client_id = messBuff->client_id;
    if (client_id == -1){
        printf("Too many clients ... leaving ...\n");
        exit(0);
    }
    printf("My ID: %d\n", client_id);
    return client_id;
}




void list_handler(){
    time_t time_tmp = time(NULL);
    MessageBuff *messBuff = malloc(sizeof(MessageBuff));

    messBuff -> time_struct = *localtime(&time_tmp);
    messBuff -> type = LIST;
    messBuff -> client_id = client_id;

    msgsnd(serverqueueID, messBuff, sizeof(MessageBuff),0);
    msgrcv(queueID, messBuff, sizeof(MessageBuff),0,0);
    printf("%s\n", messBuff->content);
}

void stop_handler(){
    time_t time_tmp = time(NULL);
    MessageBuff *messBuff = malloc(sizeof(MessageBuff));

    messBuff -> time_struct = *localtime(&time_tmp);
    messBuff -> type = STOP;
    messBuff->client_id = client_id;
    
    msgsnd(serverqueueID, messBuff, sizeof(MessageBuff),0);
    msgctl(queueID, IPC_RMID, NULL);
    exit(0);
}


void server_message_handler(){
    MessageBuff *messBuff = malloc(sizeof(MessageBuff));
    while (msgrcv(queueID,messBuff, sizeof(MessageBuff),0, IPC_NOWAIT)>=0){
        if (messBuff->type ==STOP){
            printf("Got STOP message from server, exiting ...\n");
            stop_handler();
        }else{
            struct tm tmp_time = messBuff -> time_struct;
            printf("Message from client %d has been sent at %02d:%02d:%02d\nCONTENT:%s\n", messBuff->client_id,
            tmp_time.tm_hour,
            tmp_time.tm_min,
            tmp_time.tm_sec,
            messBuff->content);

        }
    }
}


void to_one_handler(int other_id, char* mess){
    time_t time_tmp = time(NULL);
    MessageBuff *messBuff = malloc(sizeof(MessageBuff));

    messBuff -> time_struct = *localtime(&time_tmp);
    messBuff -> type = TOONE;
    strcpy(messBuff->content, mess);
    messBuff -> client_id = client_id;
    messBuff -> other_id = other_id;
    msgsnd(serverqueueID, messBuff, sizeof(MessageBuff),0);
}

void to_all_handler(char * mess){
    time_t time_tmp = time(NULL);
    MessageBuff *messBuff = malloc(sizeof(MessageBuff));

    messBuff -> time_struct = *localtime(&time_tmp);
    messBuff -> type = TOALL;
    strcpy(messBuff->content, mess);
    messBuff -> client_id = client_id;
    
    msgsnd(serverqueueID, messBuff, sizeof(MessageBuff),0);
}
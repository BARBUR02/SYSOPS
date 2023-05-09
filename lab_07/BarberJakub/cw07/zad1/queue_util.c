#include "queue_util.h"
#include "utils.h"

char pop(char * q){
    if (isEmpty(q)){
        printf("[QUEUE EMPTY] error unsucessfull pop. Queue empty\n");
        return '\0';
    }
    char res = q[0];
    memcpy(q,q+1,strlen(q)+1);
    return res;

}


void push(char * q, char el){
    if (isFull(q)){
        printf("[QUEUE FULL] can't push to queue\n");
        return;
    }
    int size = strlen(q);
    q[size]= el;
    q[size+1]='\0';
}


bool isFull(char * q){      return (strlen(q) == QUEUE_SIZE);}
bool isEmpty(char * q){     return (strlen(q)==0);}
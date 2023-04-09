#include<stdio.h>
#include<stdbool.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>

typedef struct {
    char* email;
    int index;
} cmp_util;

bool send(char* receiver, char* topic, char* content){
    char command[100];
    sprintf(command,"mail -s \"%s\" %s",topic, receiver);
    FILE* mail_input = popen(command,"w");
    fwrite( content, strlen(content), sizeof(char),mail_input); 
    pclose(mail_input);
    return true;
}


static int myCompare(const void* a, const void* b){
    const char* p1 = *(char**)a;
    const char* p2 = *(char**)b;
    return strcmp(p1,p2);
}

int cmp(const void* a, const void* b){
    const  cmp_util* p1 =  (cmp_util*) a;
    const  cmp_util* p2 = (cmp_util*) b;
    return myCompare(p1->email, p2->email);
}

void sort_mails(bool e_mail){
    char command[] = "mail";
    FILE* mail_input = popen(command,"r");
    int bufferLength = 255;
    // fwrite( "q",sizeof(char),1,mail_input);
    char * line;
    char tmpCopy[bufferLength];
    line =(char *) malloc(sizeof(char)*bufferLength);
    char* val = fgets(line,bufferLength,mail_input);
    if (val==NULL){
        return;
    }
    char * token = strtok(line," ");
    token = strtok(NULL," ");
    int size = atoi(token);
    char ** lines = (char **) malloc(sizeof(char*) * size);
    cmp_util* utils[size];
    int i=0;
    while (i<size){
        line =(char *) malloc(sizeof(char)*bufferLength);
        fgets(line,bufferLength,mail_input);
        lines[i] = line;
        strcpy(tmpCopy,line);
        char * token = strtok(tmpCopy," ");
        for ( int j=0;j<2;j++){
            token = strtok(NULL," ");
        }
        char *tokenCpy = (char* ) malloc(sizeof(char)* strlen(token));
        strcpy(tokenCpy,token);
        
        cmp_util* curr = malloc(sizeof(cmp_util));
        curr->email=tokenCpy;
        curr->index=i;
        utils[i] = curr;
        i++;
    }
    

    if (e_mail){
        qsort(utils,size,sizeof(cmp_util*),cmp); 
    }
    

    for ( int i=0;i<size;i++){
        printf("%s",lines[utils[i]->index]);
    }
    
    exit(0);
    // fseek(mail_input,0L,SEEK_END);
    // puts("q");
    // FILE* mail_tmp = popen(command,"w");
    // fwrite("q",1,sizeof(char),mail_input);
    // pclose(mail_input);
    // pclose(mail_tmp);
    // write(STDIN_FILENO, "q\n",1);
    
    // printf("mail programm closed!\n");
}

int main(int argc, char* argv[]){
    if (argc!= 4 && argc!=2){
        printf("Incorrect argument count!\n");
        exit(1);
    }
    bool send_mode = argc==4 ? true : false;    
    if (send_mode){
        send(argv[1],argv[2],argv[3]);
    }
    else{
        if (strcmp(argv[1],"nadawca")!=0 && strcmp(argv[1],"data")!=0  ){
            printf("Incompatible argument: expected : <nadawca/data>\n");
            exit(1);
        }
        bool nadawca = false;
        if (strcmp(argv[1],"nadawca")==0) {nadawca=true;}
        sort_mails(nadawca);
    }
    return 0;
}
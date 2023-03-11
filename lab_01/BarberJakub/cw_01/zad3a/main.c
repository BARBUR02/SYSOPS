#ifdef DDL_ACTIVE
    #include "dynamic.h"
#else 
    #include"interface.h"
#endif

#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<string.h>
#include<time.h>
#include<sys/times.h>
// #include"interface.h"
#include "dlmanager.h"

#define COMMAND_COUNT 6

enum command {
    INIT,
    COUNT,
    SHOW,
    DELETE,
    DESTROY,
    EXIT
};

static const char * command_names[] = {
    [INIT]="init",
    [COUNT]="count",
    [SHOW]="show",
    [DELETE]="delete",
    [DESTROY]="destroy",
    [EXIT] = "exit"
};

bool running = true;
wc_provider * interface;
void parse_command();

void init(){
    load_symbols("./libwordcounter.so");
    interface= NULL;
    running = true;
}


void exit_f(){
    running=false;
}

int main(){
    init();
    // printf("Hello to our REPL interface...\n");

    while (running){
        parse_command();
        // investigate_provider(*interface);
    }

    // printf("Hello world\n");
    // printf("Executing wc command for test.txt file ...\n");
    // system("wc test.txt");

    // wc_provider* test_struct = init_provider();

    // investigate_provider(test_struct);
    // struct timespec begin,end;
    // clock_gettime(CLOCK_REALTIME, &begin);
    // process_add_block(test_struct, "test.txt");
    // process_add_block(test_struct, "test2.txt");
    // process_add_block(test_struct, "test3.txt");
    // process_add_block(test_struct, "test4.txt");
    // process_add_block(test_struct, "test5.txt");
    // process_add_block(test_struct, "test6.txt");
    // process_add_block(test_struct, "test7.txt");
    // investigate_provider(test_struct);
    // show_block(test_struct,3);
    // show_block(test_struct,2);
    // show_block(test_struct,1);
    // show_block(test_struct,0);
    // show_block(test_struct,22);
    // show_block(test_struct,-2);
    
    // remove_block(test_struct,0);

    // investigate_provider(test_struct);
    // remove_block(test_struct,5);

    // investigate_provider(test_struct);

    // remove_block(test_struct,3);

    // investigate_provider(test_struct);

    // remove_block(test_struct,0);
    // investigate_provider(test_struct);
    // remove_block(test_struct,3);
    // investigate_provider(test_struct);
    // investigate_provider(test_struct);
    // destroy_wc_provider(test_struct);
    // end=clock();
    // clock_t delta = end-start;
    // double time_taken = ((double) delta)/CLOCKS_PER_SEC;
    // clock_gettime(CLOCK_REALTIME, &end);
    

    return 0;
}






void parse_command(){
    // printf(" > ");
    printf("\n");
    struct timespec begin,end;
    struct tms tms_begin,tms_end;
    size_t size = 255;
    char *line= NULL;
    char** args = calloc(2,sizeof(char*));
    getline(&line,&size,stdin);
    char* p;
    // printf("\n%s\n",line);
    const char corr[]= " \n";
    int flag =0;
    int run_command=0;
    int i;
    clock_gettime(CLOCK_REALTIME, &begin);
    times(&tms_begin);
    if (strspn(line, " \r\n\t") == strlen(line)){
        // printf("\n");
        return;
    }
    const char digits[] ="0123456789";
    for (p = strtok(line,corr), i=0;p;p=strtok(NULL,corr), i++ ){
        if (i>1){
            flag = 1;
            break;
        }
        // printf("%s\n",p);
        args[i] = p;
    }
    if (flag == 1){
        printf("Too many parameters: expected format: <command_name> <argument>\n");
        return;
    }
    if(i==1){
        if (strcmp(command_names[EXIT],args[0])==0){
            destroy_wc_provider(interface);
            running = false;
            return;
        }
        else if (strcmp(command_names[DESTROY],args[0])==0){
            destroy_wc_provider(interface);
            interface=NULL;
            run_command=1;
            // return;
        }
        else {        
            printf("No arguments provided: expected format: <command_name> <argument>\n");
            return;
        }
    }
    enum command given_command;
    for (int i=INIT;i<COMMAND_COUNT;i++){
        if (strcmp(command_names[i],args[0])==0){
            given_command = i;
            break;
        }
    }
    switch(given_command){
        case INIT:
            if (strspn(*(args+1),digits)== strlen(*(args+1)) && *(args+1)[0]!='0' ){
                interface = init_provider(atoi(args[1]));
                run_command=1;
            }
            else{
                printf("Argument should be a number greater than 0\n");  
            }
            break;
        case COUNT:
            process_add_block(interface, args[1]);
            run_command=1;
            break;
        case SHOW:
            if ((strspn(*(args+1),digits)== strlen(*(args+1)) && *(args+1)[0]!='0' ) ||( (strlen(*(args+1))==1 && *(args+1)[0]=='0')) ){
                    show_block(interface,atoi(args[1]));
                    run_command=1;
                }
            else{
                printf("Argument should be a number\n");
            }
            break;
        case DELETE:
            if ( (strspn(*(args+1),digits)== strlen(*(args+1)) && *(args+1)[0]=='0' ) || (strlen(*(args+1))==1 && *(args+1)[0]!='0')){
                    remove_block(interface,atoi(args[1]));
                    run_command=1;
                }
            else{
                printf("Argument should be a number\n");
            }
            break;
        default:
        if (run_command==0){
            printf("Unknown command\n");
            // return;
        }
            break;
    }
    free(line);
    free(args);
    if (run_command){
        clock_gettime(CLOCK_REALTIME, &end);
        times(&tms_end);
        printf("\t%-15s %ld ns\n","REAL:", end.tv_nsec - begin.tv_nsec );
        printf("\t%-15s %ld ticks\n","SYS:", tms_end.tms_cstime - tms_begin.tms_cstime );
        printf("\t%-15s %ld ticks\n", "USER:",tms_end.tms_cutime - tms_begin.tms_cutime );
    }
}
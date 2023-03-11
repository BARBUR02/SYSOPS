#include"interface.h"
#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<string.h>
#include<time.h>
#include"interface.h"


wc_provider* init_provider(int size){
    wc_provider* created_provider =(wc_provider*) malloc(sizeof(wc_provider));
    created_provider->max_size = size;
    created_provider->curr_size = 0;
    created_provider->results = (char **)calloc(created_provider->max_size ,sizeof(char *) );
    // printf("\nInited provider ...\n");
    // investigate_provider(created_provider);
    return created_provider;
}

void investigate_provider(wc_provider* count_struct){
    if (count_struct == NULL){
        printf("No active WC_PROVIDER initialized yet\n");
        return;
    }
    printf("\nInvestigating values ...\n");
    printf("Strings contained : \n");
    for ( int i=0; i< count_struct->curr_size;i++){
        printf("%d => %s\n",i, *((count_struct->results)+i) );
    }
    printf("Max size : %d\n", count_struct->max_size);
    printf("Curr size : %d\n", count_struct->curr_size);
}

bool read_block(char **block, const char* file_name){
    FILE *file = fopen(file_name,"r");
    if (file==NULL){
        return false;
    }
    fseek(file,0,SEEK_END);
    // printf("\nFile size: %ld\n",ftell(file));
    long num_of_bytes = ftell(file);
    char * result = calloc(num_of_bytes,sizeof (char));
    rewind(file);
    fread(result ,sizeof (char), num_of_bytes, file);
    (*block) = result;
    fclose(file);
    return true;
}

void process_add_block(wc_provider* count_struct, const char* filepath){
    if (count_struct == NULL){
        printf("No active WC_PROVIDER initialized yet\n");
        return;
    }
    if (count_struct->curr_size==count_struct->max_size){
        count_struct->results=realloc(count_struct->results,2*count_struct->max_size*(sizeof(char*)));
        count_struct->max_size*=2;
    }
    char command[255];
    snprintf(command,255,"wc %s > /tmp/tempX",filepath);
    // printf("%s",command);
    int res = system(command);
    if (res == -1){
        printf("File not found\n");
    }
    if (read_block(&count_struct->results[count_struct->curr_size], "/tmp/tempX")){
        count_struct->curr_size++;
        // printf("Success\n");
        // printf("\n\n================\n\n");
    }else{
        printf("Error in counting file content\n");
    }
    // system("rm /tmp/temp");
    remove("/tmp/tempX");
}


void show_block(wc_provider* wc_struct, int index){
    if (wc_struct == NULL){
        printf("No active WC_PROVIDER initialized yet\n");
        return;
    }
    if (index>= wc_struct->curr_size){
        if (wc_struct->curr_size>0){
        printf("Given index is too large, current maximal index is %d\n",wc_struct->curr_size-1);
        }else{
            printf("Currently no data in wc_array, add data to read indexes\n");
        }
    }
    else if (index < 0){
        printf("Negative indexes not allowed\n");
    }
    else {
        printf("%s",*(wc_struct->results+index));
    }

}

void remove_block(wc_provider * wc_struct,int index){
    if (wc_struct == NULL){
        printf("No active WC_PROVIDER initialized yet\n");
        return;
    }
    int curr_num= wc_struct->curr_size;
    if (index>=curr_num){
        if (wc_struct->curr_size>0){
        printf("Given index is too large, current maximal index is %d\n",wc_struct->curr_size-1);
        }else{
            printf("Currently no data in wc_array, add data to read indexes\n");
        }
    }
    else if (index <0){
        printf("Negative indexes not allowed\n");
    }
    else{
        int elements_to_move = curr_num - index -1;
        char * to_remove = (wc_struct->results)[index];
        free(to_remove);
        memmove(wc_struct->results+index, wc_struct->results+index+1,elements_to_move*(sizeof(char*)));
        wc_struct->curr_size-=1;
        // printf("\nElements to move: %ld",elements_to_move*(sizeof(char*)));
        // printf("Element at index: %d - REMOVED\n",index);
    }
}


void destroy_wc_provider(wc_provider * wc_struct){
    if (wc_struct==NULL){
        printf("No PROVIDER detected to be destroyed\n");
        return;
    }
    int curr_size = wc_struct->curr_size;
    for (int i=0;i<curr_size;i++){
        remove_block(wc_struct,0);
    }
    free(wc_struct->results);
    free(wc_struct);
    printf("WC_PROVIDER destroyed\n");

}
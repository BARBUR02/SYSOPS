#ifndef LIB_DYNAMIC_H
#define LIB_DYNAMIC_H

#include<stdlib.h>
#include<stdbool.h>
#include<string.h>
#include<time.h>


typedef struct{
    char ** results;
    int max_size;
    int curr_size;
} wc_provider;

wc_provider* (*init_provider)(int );
void (*investigate_provider)(wc_provider*);
bool (*read_block)(char **, const char*);
void (*process_add_block)(wc_provider* , const char* );
void (*show_block)(wc_provider* , int );
void (*remove_block)(wc_provider * ,int );
void (*destroy_wc_provider)(wc_provider * );

#endif
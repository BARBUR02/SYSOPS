#ifndef INTERFACE_H
#define INTERFACE_H

#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<string.h>
#include<time.h>


typedef struct{
    char ** results;
    int max_size;
    int curr_size;
} wc_provider;

wc_provider* init_provider(int size);
void investigate_provider(wc_provider* count_struct);
bool read_block(char **block, const char* file_name);
void process_add_block(wc_provider* count_struct, const char* filepath);
void show_block(wc_provider* wc_struct, int index);
void remove_block(wc_provider * wc_struct,int index);
void destroy_wc_provider(wc_provider * wc_struct);

#endif
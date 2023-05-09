#ifndef MEMORY_UTILS_H
#define MEMORY_UTILS_H

#include<stdbool.h>


char * attach_memory_block(char * filename, int size);
bool detach_memory_block(char * block);
bool destroy_memory_block(char *filename);


#define BLOCK_SIZE 4096


#endif
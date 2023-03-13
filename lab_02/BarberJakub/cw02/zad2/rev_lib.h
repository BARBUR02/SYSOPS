#ifndef _REV_LIB_H_
#define _REV_LIB_H_

#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<string.h>


char* rev(char * buff);
bool reverse_file(FILE* from_f, FILE* to_f);
bool reverse_by_byte(FILE* from_f, FILE* to_f);
bool reverse_by_char(FILE* from_f, FILE* to_f);
size_t get_file_size(FILE * f);

#endif
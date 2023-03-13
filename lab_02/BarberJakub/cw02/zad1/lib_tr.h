#ifndef _LIB_TR_H_
#define _LIB_TR_H

#include<stdio.h>
#include <stdlib.h>
#include<stdbool.h>


size_t get_file_size_lib(FILE* file);
bool tr_lib(char s, char t, const char* from_filepath, const char* to_file_path);


#endif
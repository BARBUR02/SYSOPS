#ifndef _SYS_TR_H_
#define _SYS_TR_H

#include<stdio.h>
#include <stdlib.h>
#include<stdbool.h>
#include<fcntl.h>
#include<unistd.h>


size_t get_file_size_sys(int file);
bool tr_sys(char s, char t, const char* from_filepath, const char* to_file_path);


#endif
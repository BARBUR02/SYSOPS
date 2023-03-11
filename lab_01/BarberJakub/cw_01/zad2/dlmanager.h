#ifndef DLLMANAGER_H
#define DDLMANAGER_H

#ifdef DDL_ACTIVE 
#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>

void load_symbols(const char* filepath){
    void * handle = dlopen(filepath, RTLD_LAZY);

    if (handle == NULL){
        printf("DDL Manager not found, incorrect path name : %s \n",filepath);
        exit(1);
        return;
    }

    *(void**)(&init_provider) = dlsym(handle,"init_provider" );
    *(void**)(&read_block) = dlsym(handle,"read_block" );
    *(void**)(&process_add_block) = dlsym(handle,"process_add_block" );
    *(void**)(&show_block) = dlsym(handle,"show_block" );
    *(void**)(&remove_block) = dlsym(handle,"remove_block" );
    *(void**)(&destroy_wc_provider) = dlsym(handle,"destroy_wc_provider" );

}

#else
void load_symbols(const char* filepath){}
#endif

#endif

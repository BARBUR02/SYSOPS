#include<stdlib.h>
#include<sys/wait.h>
#include<unistd.h>
#include<limits.h>
#include<dirent.h>
#include<sys/stat.h>
#include<stdio.h>
#include<sys/types.h>
#include<string.h>
#include<stdbool.h>

#define BUFFER_SIZE 255

char exe_path[PATH_MAX];
char curr_path[PATH_MAX];
char new_path[PATH_MAX];


bool file_check(char * filepath, char* cmp){
    // printf("Trying to open file : %s\n",filepath);
    FILE* file = fopen(filepath,"r");
    if (file == NULL){
        perror("Error in opening file\n");
        return false;
    }
    char buffer_f[BUFFER_SIZE+1];
    rewind(file);
    // char* buffer;
    int to_read = strlen(cmp);
    size_t res = fread(buffer_f,sizeof( char ), to_read, file);
    // printf("Wczytano %d znakow, \t",(int)res);
    buffer_f[res] =0;
    // printf("Buffer: %s,\tArg_string: %s\n",buffer_f,cmp);
    // buffer[res] =0;
    if (strcmp(buffer_f,cmp)==0){
        fclose(file);
        return true;
    }

    fclose(file);
    return false;
}

int traverse_dir(char * dir_path, char* str_beggining, char* exe_path){
    // printf("Dir path: %s, PID: %d\n",dir_path,(int)getpid());
    DIR* dir=NULL;
    pid_t child_pid;
    struct dirent * pDirent = NULL;
    struct stat statBuff;
    dir = opendir(dir_path);
    if (dir ==NULL){
        perror("Error while opening Dir\n");
        return 1;
    }
    // char curr_path[PATH_MAX];
    // char new_path[PATH_MAX];
    strcpy(curr_path,dir_path);
    while( NULL != (pDirent = readdir(dir))){
        if (strcmp(pDirent->d_name,".")==0 || strcmp(pDirent->d_name,"..")==0) continue;
        strcpy(new_path, dir_path);
        strcat(new_path, "/");
        strcat(new_path,pDirent->d_name);
        int stat_res = stat(new_path,&statBuff);
        if (stat_res==-1){
            perror("Error in stat()\n");
            return 1;
        }
        if (S_ISDIR(statBuff.st_mode)){
            child_pid = fork();
        }

        if (child_pid==0){
            // strcat(dir_path, pDirent->d_name);
            // printf("Calling recursion ...\tNew location: %s \n",new_path);
            // traverse_dir(new_path,str_beggining,exe_path);
            execl(exe_path,exe_path,new_path,str_beggining, NULL);
            break;
        }
        else {
            if (!S_ISDIR(statBuff.st_mode)){
                if (file_check(new_path,str_beggining)){
                    char to_print[PATH_MAX];
                    if (strlen(new_path)>PATH_MAX){
                        perror("Path exceeds PATH_MAX length\n");
                        exit(1);
                    }
                    if (realpath(new_path, to_print)==NULL){
                        perror("Failed in realpath\n");
                        return 1;
                    }
                    
                    printf("FILEPATH: %s,\tPID : %d\n",to_print,(int)getpid());
                    fflush(NULL);
                }

            }
        }
        
    }
    while (wait(NULL)>0);
    closedir(dir);
    return 0;

}


int main(int argc, char* argv[]){
    
    if (argc!=3){
        printf("Incorrect number of arguments, two expectes: <dir_path><beginning_string>\n");
        return 1;
    }
    if (strlen(argv[2])>255){
        printf("Maximal len of rgv[2] should be 255\n");
        return 1;
    }
    // char* err = realpath(argv[0], exe_path);
    if (realpath(argv[0], exe_path)==NULL){
        perror("Failed in realpath\n");
        return 1;
    }
    // printf("Loaded in realpath: %s\n",exe_path);
    // fflush(stdout);
    char* dir_path = argv[1];
    char* string_start = argv[2];
    traverse_dir(dir_path,string_start,exe_path);

    return 0;
}
#include<stdio.h>
#include<dirent.h>
#include<stdlib.h>
#include<sys/stat.h>
#include<string.h>
#include<unistd.h>

size_t get_file_size(char* fname){
    FILE * f = fopen(fname,"r");
    fseek(f,0L,SEEK_END);
    size_t size = ftell(f);
    rewind(f);
    fclose(f);
    return size;
}


int main(int argc, char* argv[]){
    if (argc!=2){
        printf("Incorrect call: should provide exactly 1 parameter <dirpath>\n");
        return 1;
    }
    char* dirpath = argv[1];
    DIR *dir;
    struct dirent *pDirent=NULL;
    struct stat statBuff;
    dir = opendir(dirpath);
    if (dir==NULL){
        printf("Provided directory_path doesn't exist\n");
        return 1;
    }
    long long summary_size;

    // option with concatenating strings -> getting long complex paths
    // char filepath[256];

    int opened = chdir(dirpath);
    if (opened ==-1){
        printf("Couldn't enter directory\n");
        return 1;
    }

    while( NULL != (pDirent = readdir(dir))){
        // strcpy(filepath,dirpath);
        // strcat(filepath,pDirent->d_name);
        // stat(filepath,&statBuff);
        // printf("%s\n",filepath);
        // stat(filepath,&statBuff);
        stat(pDirent->d_name,&statBuff);
        if (S_ISDIR(statBuff.st_mode)){
            continue;
        }
        // printf("\n%30ld\t%s",get_file_size(filepath),pDirent->d_name);
         printf("\t%6ld\t%s\n",statBuff.st_size,pDirent->d_name);
         summary_size+=statBuff.st_size;
    }

    printf("\t%6lld\tSUM\n",summary_size);
    closedir(dir);
    return 0;
}
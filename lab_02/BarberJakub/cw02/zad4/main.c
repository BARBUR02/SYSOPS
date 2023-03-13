#include<stdio.h>
#include<ftw.h>

long long  sum =0;

int file_action(const char* filepath, const struct stat * sb, int typeflag){
    if (typeflag== FTW_D){
        return 0;
    }
    printf("\t%6ld\t%s\n",sb->st_size,filepath);
    sum+=sb->st_size;
    return 0;
}


int main(int argc,char * argv[]){
    if (argc!=2){
        printf("Incorrect Call: expected format <dirpath>\n");
        return 1;
    }

    
    char * filepath = argv[1];
    int res = ftw(filepath,file_action,1);
    if (res==-1){
        printf("Error in opening given directory\n");
        return 1;
    }
   
    printf("\t%6lld\tSUM\n",sum); 

    return 0;
}
#include <unistd.h>
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include "time.h"
#include <fcntl.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>

 
void print(  struct dirent *entry){
 struct stat buf;
      lstat(entry->d_name,&buf);
 char filemode[11];
 int i = 0;
    while (i < 9) {
        if (buf.st_mode & (1 << i)) {
            switch (i % 3) {
                case 0:
                    filemode[9 - i] = 'x';
                    break;
                case 1:
                    filemode[9 - i] = 'w';
                    break;
                case 2:
                    filemode[9 - i] = 'r';
                    break;
            }
        } else {
            filemode[9 - i] = '-';
        }
        i++;
    }
     filemode[10] = '\0';
     if(S_ISDIR(buf.st_mode))
     printf("d%-11s",filemode);
     else
     printf("-%-11s",filemode);

    printf("%2ld ",buf.st_nlink);
    printf("%s  ",getpwuid(buf.st_uid)->pw_name);
    printf("%s  ",getgrgid(buf.st_gid)->gr_name);
         printf("%7ld",buf.st_size);
        time_t t = buf.st_mtime;
	struct tm time;
	localtime_r(&t,&time);
	printf("%3d月",time.tm_mon+1);
	printf("%02d ",time.tm_mday);
	printf("%02d: %02d",time.tm_hour,time.tm_min);
	printf(" %-15s\n",entry->d_name);
}

void printdir(char *dir, int depth){
    DIR *dp;
    struct dirent *entry;
    struct stat statbuf;
    if  ((dp = opendir(dir)) == NULL){
         printf("open dir erorr\n");
         return;
    }
    chdir(dir);
    while((entry=readdir(dp))!=NULL ){
    lstat(entry->d_name,&statbuf);
   if(S_ISDIR(statbuf.st_mode)){
    	if(strcmp(entry->d_name,".")==0||strcmp(entry->d_name,"..")==0){
    	continue;
    	}
    	print(entry);
    	printdir(entry->d_name,depth+4);
    }
    else{
     //printf("depth : %-3d",depth);
     print(entry);
    }
    }
        chdir("..");
        closedir(dp);
        return;
}

int main(int argc,char* argv[])//第二个参数指定路径
{
	if(argc == 1) 
	{
		printf("please input path!\n");
	}
	else
	printdir(argv[1],0);
	return 0;
}


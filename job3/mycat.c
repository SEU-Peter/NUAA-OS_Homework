#include<stdio.h>
#include<stdlib.h>

#include<sys/stat.h>
#include<unistd.h>

#include<sys/types.h>
#include<fcntl.h>

int main(int argc,char *argv[]){
        struct stat st;
        int fsize;
        int fd;
        char *buffer;


        fd=open(argv[1],O_RDONLY);
        if(fd<0){
                printf("Error:open file wrong!\n");
                exit(0);
        }

        stat(argv[1],&st);
        fsize=st.st_size;
        buffer=(char*)malloc((1+fsize)*sizeof(char));

        if(!buffer){
                printf("Error:memory wrong!\n");
                exit(0);
        }

        read(fd,buffer,fsize);
        write(1,buffer,fsize);

        close(fd);
        return 0;
}


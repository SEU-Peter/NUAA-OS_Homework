#include<stdio.h>
#include<stdlib.h>

#include<sys/types.h>
#include<unistd.h>

#include<sys/stat.h>
#include<fcntl.h>

int main(int argc,char *argv[]){
        int fd;
        int fsize;
        char *buffer;
        struct stat st;

        if(argc!=3){
                printf("Error:parameter wrong!\n");
                exit(0);
        }

        fd=open(argv[1],O_RDONLY);
        if(fd<0){
                printf("Error:can't open the read-file!\n");
                exit(0);
        }

        stat(argv[1],&st);
        fsize=st.st_size;

        buffer=(char *)malloc((1+fsize)*sizeof(char));
        if(!buffer){
                printf("Error:memory wrong!\n");
                exit(0);
        }

        read(fd,buffer,fsize);

        close(fd);

        fd=open(argv[2],O_WRONLY|O_CREAT);
        if(fd<0){
                printf("Error:can't open the write-file!\n");
                exit(0);
        }

        write(fd,buffer,fsize);

        close(fd);
        free(buffer);
        return 0;
}


#include<stdio.h>
#include<sys/wait.h>
#include<unistd.h>
#include<sys/types.h>
#include<stdlib.h>

void mysys(char *str){
        pid_t pid;

        if(str==NULL){
                printf("Error:wrong shell string!\n");
                exit(0);
        }
        pid=fork();
        if(pid==0)
                execl("/bin/sh","sh","-c",str,NULL);
        wait(NULL);
}

int main(){
        printf("---------------------------------\n");
        mysys("echo a b c d");
        printf("---------------------------------\n");
        mysys("ls /");
        printf("---------------------------------\n");
        return 0;
}


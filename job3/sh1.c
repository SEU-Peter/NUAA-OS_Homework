#include<stdio.h>
#include<sys/wait.h>
#include<unistd.h>
#include<sys/types.h>
#include<stdlib.h>
#include<string.h>
#include<pwd.h>


#define MAX_PATH_LEN 200
#define CMD_LEN 200
int cmd_num;
char *cmd_array[100];



void cmd(){
    pid_t pid;

    pid=fork();
    if(pid==0){
        if(execvp(cmd_array[0],cmd_array)<0){
                printf("Error:command not found\n");
        }
    }
    wait(NULL);
}

void print_user(){
        struct passwd *pwd;
        char path_name[MAX_PATH_LEN];
        getcwd(path_name,MAX_PATH_LEN);
        pwd=getpwuid(getuid());
        printf("%s@kali:%s$ ",pwd->pw_name,path_name);
}

void get_cmd(){
        int i=0;
        char str[CMD_LEN];
        char *buf;
        cmd_num=0;
        memset(cmd_array,0,100);
        fgets(str,CMD_LEN,stdin);
        buf=strtok(str," \n");
        while(buf!=NULL){
                cmd_array[i]=(char *)malloc(sizeof(*buf));
                strcpy(cmd_array[i++],buf);
                buf=strtok(NULL," \n");
        }
        cmd_array[i]=NULL;
        cmd_num=i;
}


void mycd(){
        struct passwd *pwd;
        char path_name[MAX_PATH_LEN];
        pwd=getpwuid(getuid());
        if(cmd_num==1){
                strcpy(path_name,pwd->pw_dir);
                if(chdir(path_name)==-1){
                        perror("mycd:chdir..");
                        exit(1);
                }
        }
        else{
                if(chdir(cmd_array[1])==-1){
                        printf("mycd: cd:%s:No such directory\n",cmd_array[1]);
                }
        }
}


void myexit(){
        exit(0);
}

void mypwd(){
        char path_name[MAX_PATH_LEN];
        if(getcwd(path_name,MAX_PATH_LEN)){
                printf("%s\n",path_name);
        }
        else{
                perror("Error:getcwd...\n");
                exit(0);
        }
}

int main(){

        while(1){

                print_user();
                get_cmd();

                if(cmd_array[0]==NULL)
                        return 0;
                else if(strcmp(cmd_array[0],"exit")==0)
                        myexit();
                else if(strcmp(cmd_array[0],"pwd")==0)
                        mypwd();
                else if(strcmp(cmd_array[0],"cd")==0)
                        mycd();
                else cmd();
        }
        return 0;
}

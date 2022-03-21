#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

int main(int argc, char *argv[])
{	
	DIR *dp=NULL;
	if(argv[1]!=NULL)
	{
		dp= opendir(argv[1]) ;	
	}
	 struct dirent *sdp;
		 	 //打开目录
 	if(dp==NULL)
 	{
		if(argv[1]!=NULL)
		{
			perror("opendir error\n");
                	exit(1);	
		}
   		else
		{
			char buf[80];
               		getcwd(buf,sizeof(buf));
			dp= opendir(buf) ;	
		}
 	}
	
	while((sdp = readdir(dp)) != NULL)   //读目录
 	{
  		if(strcmp(sdp->d_name, ".") == 0)	//将“.” 排除，否则会死循环
  		{
   			continue;
  		}
  		if(strcmp(sdp->d_name, "..") == 0) //将“..” 排除，否则会死循环
 		 {
  			 continue;
 		 }
    		printf("%s\t",sdp->d_name);		//打印目录
	 }
 	printf("\n");
	closedir(dp);
	return 0;
}

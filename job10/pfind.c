#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pthread.h>

#define WORKER_NUMBER 4
#define QUEUELENGTH 8

typedef struct{
	int is_end;
	char path[128];
	char string[128];
}TASK;


TASK task_queue[QUEUELENGTH];
int in,out;


typedef struct {
    int value;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
} sema_t;

sema_t mutex_sema;
sema_t empty_buffer_sema;
sema_t full_buffer_sema;

void sema_init(sema_t *sema, int value)
{
    sema->value = value;
    pthread_mutex_init(&sema->mutex, NULL);
    pthread_cond_init(&sema->cond, NULL);
}

void sema_wait(sema_t *sema)
{
    pthread_mutex_lock(&sema->mutex);
    while (sema->value <= 0)
        pthread_cond_wait(&sema->cond, &sema->mutex);
    sema->value--;
    pthread_mutex_unlock(&sema->mutex);
}

void sema_signal(sema_t *sema)
{
    pthread_mutex_lock(&sema->mutex);
    ++sema->value;
    pthread_cond_signal(&sema->cond);
    pthread_mutex_unlock(&sema->mutex);
}

void find_file(char *path, char *target)
{
	FILE *file = fopen(path, "r");
	char line[256];
	while (fgets(line, sizeof(line), file)) {
		if (strstr(line, target))
		printf("%s: %s", path, line);
	}
	fclose(file);
}

void find_dir(char *path, char *target)
{
	DIR *dir = opendir(path);
	struct dirent *entry;
	while (entry = readdir(dir)) 
	{
		if (strcmp(entry->d_name, ".") == 0)
			continue;

		if (strcmp(entry->d_name, "..") == 0)
			continue;

		if (entry->d_type == DT_DIR)
		{
			char newdirpath[256];
			strcpy(newdirpath,path);
			strcat(newdirpath,"/");
			strcat(newdirpath,entry->d_name);
			find_dir(newdirpath,target);
		}			

		if (entry->d_type == DT_REG)
		{
			sema_wait(&empty_buffer_sema);
			sema_wait(&mutex_sema);
			
			 task_queue[in].is_end = 0;
			 strcpy(task_queue[in].path,path);
			 strcat(task_queue[in].path,"/");
			 strcat(task_queue[in].path,entry->d_name);
			 strcpy(task_queue[in].string,target);
			 in = (in + 1) % QUEUELENGTH;

			sema_signal(&mutex_sema);
			sema_signal(&full_buffer_sema);
		}
 	}
	closedir(dir);
} 


void *worker_entry(void *arg)
{
	while(1)
	{
		TASK *task;
	    sema_wait(&full_buffer_sema);
        sema_wait(&mutex_sema);
		
		task = &task_queue[out];
		out = (out+1)%QUEUELENGTH;
				
        sema_signal(&mutex_sema);
        sema_signal(&empty_buffer_sema);
	
		if(task->is_end)
			break;
		find_file(task->path,task->string);
	}
}


int main(int argc, char *argv[])
{
	if (argc != 3) {
		puts("Usage: sfind file string");
		return 0;
	}
	char *path = argv[1];
	char *string = argv[2];
	struct stat info;
	stat(path, &info);

	puts("111");

	if (S_ISREG(info.st_mode))
	{
		find_file(path, string);
		return 0;
	}

	puts("222");

	sema_init(&mutex_sema, 1);
    sema_init(&empty_buffer_sema, QUEUELENGTH - 1);
    sema_init(&full_buffer_sema, 0);

	pthread_t worker_tid[WORKER_NUMBER];
	for(int i=0;i<WORKER_NUMBER;i++)
		pthread_create(&worker_tid[i],NULL,worker_entry,NULL);
	
	find_dir(path,string);
	for(int i=0;i<WORKER_NUMBER;i++)
	{
		sema_wait(&empty_buffer_sema);
		sema_wait(&mutex_sema);
			
		task_queue[in].is_end = 1;
		in = (in + 1) % QUEUELENGTH;

		sema_signal(&mutex_sema);
		sema_signal(&full_buffer_sema);
	}

	for(int i=0;i<WORKER_NUMBER;i++)
		pthread_join(worker_tid[i], NULL);
	return 0;
}


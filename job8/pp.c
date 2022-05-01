/*
+ 系统中有2个线程：ping 线程和 pong 线程
+ ping 线程先执行
+ ping 线程执行流程如下
  1. 打印输出 ping
  2. 等待 pong 线程输出
  3. 执行第 1 步
+ pong 线程执行流程如下
  1. 打印输出 pong
  2. 等待 ping 线程输出
  3. 执行第 1 步
+ 程序输出结果
  ping
  pong
  ping
  pong
  ...
  */

#include <stdio.h>
#include <unistd.h>
#include <pthread.h>


pthread_mutex_t mutex;
pthread_cond_t wait_ping;
pthread_cond_t wait_pong;
int flag=0;


void *ping(void *arg)
{
    while(1)
    {
        pthread_mutex_lock(&mutex);
        if(flag==0)
            pthread_cond_wait(&wait_pong, &mutex);
        printf("ping\n");
        flag=0;
        pthread_cond_signal(&wait_ping);
        pthread_mutex_unlock(&mutex);

        
    }
}

void *pong(void *arg)
{
    while(1)
    {
        pthread_mutex_lock(&mutex);
        if(flag==1)
            pthread_cond_wait(&wait_ping, &mutex);
        printf("pong\n");
        flag=1;
        pthread_cond_signal(&wait_pong);
        pthread_mutex_unlock(&mutex);
        
    }
}
int main()
{ 
    pthread_t consumer_tid;

    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&wait_ping, NULL);
    pthread_cond_init(&wait_pong, NULL);
    pthread_create(&consumer_tid, NULL, pong, NULL);
 
    ping(NULL);
   
    pthread_join(consumer_tid, NULL);
    return 0;
}
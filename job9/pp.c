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



typedef struct {
    int value;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
} sema_t;

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

sema_t mutex_sema;
sema_t wait_ping;
sema_t wait_pong;
int flag=1;


void *ping(void *arg)
{
    while(1)
    {
        
        if(flag==0)
            sema_wait(&wait_pong);
        sema_wait(&mutex_sema);
        printf("ping\n");
        sleep(1);
        flag=0;
        sema_signal(&mutex_sema);
        sema_signal(&wait_ping);
        

        
    }
}

void *pong(void *arg)
{
    while(1)
    {
        
        if(flag==1)
            sema_wait(&wait_ping);
        sema_wait(&mutex_sema);
        printf("pong\n");
        sleep(1);
        flag=1;
        sema_signal(&mutex_sema);
        sema_signal(&wait_pong);
        
        
    }
}
int main()
{ 
    pthread_t consumer_tid;

    sema_init(&mutex_sema, 1);
    sema_init(&wait_ping, 0);
    sema_init(&wait_pong, 0);
    
    pthread_create(&consumer_tid, NULL, pong, NULL);
 
    ping(NULL);
   
    pthread_join(consumer_tid, NULL);
    return 0;
}
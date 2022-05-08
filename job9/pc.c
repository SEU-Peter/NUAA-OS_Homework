/*
+ 系统中有3个线程：生产者、计算者、消费者
+ 系统中有2个容量为4的缓冲区：buffer1、buffer2
+ 生产者
  - 生产'a'、'b'、'c'、‘d'、'e'、'f'、'g'、'h'八个字符
  - 放入到buffer1
  - 打印生产的字符
+ 计算者
  - 从buffer1取出字符
  - 将小写字符转换为大写字符，按照 input:OUTPUT 的格式打印
  - 放入到buffer2
+ 消费者
  - 从buffer2取出字符
  - 打印取出的字符
+ 程序输出结果(实际输出结果是交织的)
a
b
c
...
    a:A
    b:B
    c:C
    ...
        A
        B
        C
        ...
        */

#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

#define CAPACITY 4
int buffer1[CAPACITY];
int buffer2[CAPACITY];

int in_buffer1;
int out_buffer1;
int in_buffer2;
int out_buffer2;

int buffer1_is_empty()
{
    return in_buffer1 == out_buffer1;
}

int buffer1_is_full()
{
    return (in_buffer1 + 1) % CAPACITY == out_buffer1;
}

int buffer2_is_empty()
{
    return in_buffer2 == out_buffer2;
}

int buffer2_is_full()
{
    return (in_buffer2 + 1) % CAPACITY == out_buffer2;
}

int get_buffer1_item()
{
    int item;

    item = buffer1[out_buffer1];
    out_buffer1 = (out_buffer1 + 1) % CAPACITY;
    return item;
}

int get_buffer2_item()
{
    int item;

    item = buffer2[out_buffer2];
    out_buffer2 = (out_buffer2 + 1) % CAPACITY;
    return item;
}

void put_buffer1_item(int item)
{
    buffer1[in_buffer1] = item;
    in_buffer1 = (in_buffer1 + 1) % CAPACITY;
}

void put_buffer2_item(int item)
{
    buffer2[in_buffer2] = item;
    in_buffer2 = (in_buffer2 + 1) % CAPACITY;
}


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
sema_t empty_buffer1_sema;
sema_t full_buffer1_sema;
sema_t empty_buffer2_sema;
sema_t full_buffer2_sema;

#define ITEM_COUNT (CAPACITY * 2)



void *produce(void *arg)
{
    int i;
    int item;

    for (i = 0; i < ITEM_COUNT; i++) { 
        sema_wait(&empty_buffer1_sema);
        sema_wait(&mutex_sema);
        item = 'a' + i;
        put_buffer1_item(item);
        printf(" %c\n", item); 

        sema_signal(&mutex_sema);
        sema_signal(&full_buffer1_sema);
        
    }
    return NULL;
}

void *calculate(void *arg)
{
    int i;
    int item;

    for (i = 0; i < ITEM_COUNT; i++) { 
        sema_wait(&full_buffer1_sema);
        sema_wait(&mutex_sema);
        
        item = get_buffer1_item()+'A'-'a';
        
        sema_signal(&mutex_sema);
        sema_signal(&empty_buffer1_sema);

        sema_wait(&empty_buffer2_sema);
        sema_wait(&mutex_sema);
        
        put_buffer2_item(item);
        printf("\t %c:%c\n",item-'A'+'a',item); 

        sema_signal(&mutex_sema);
        sema_signal(&full_buffer2_sema);
    }
    return NULL;
}
void *consume(void *arg)
{
    int i;
    int item;

    for (i = 0; i < ITEM_COUNT; i++) { 
        
        sema_wait(&full_buffer2_sema);
        sema_wait(&mutex_sema);

        item = get_buffer2_item(); 
        printf("\t\t %c\n", item); 

        sema_signal(&mutex_sema);
        sema_signal(&empty_buffer2_sema);
    }
    return NULL;
}
int main()
{ 
    pthread_t consumer_tid1;
    pthread_t consumer_tid2;    
    sema_init(&mutex_sema, 1);
    sema_init(&empty_buffer1_sema, CAPACITY - 1);
    sema_init(&full_buffer1_sema, 0);
    sema_init(&empty_buffer2_sema, CAPACITY - 1);
    sema_init(&full_buffer2_sema, 0);

    pthread_create(&consumer_tid1, NULL, consume, NULL);
    pthread_create(&consumer_tid2, NULL, calculate, NULL);
    produce(NULL); 
    pthread_join(consumer_tid1, NULL);
    pthread_join(consumer_tid2, NULL);
    return 0;
}
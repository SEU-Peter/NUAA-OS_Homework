#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdlib.h>

#define N 100
#define MAX 1000
#define CPU_NUM 2

struct param{
    int *array;
    int start;
    int end;
};
int arr[N], res[N];

void *select_sort(void *arg)
{
    struct param *param = (struct param *)arg;
    struct result *result;

    for(int i = param->start; i < param->end; i++)
    {
        int pos = i;
        for(int j = i + 1; j < param->end; j++)
            if(param->array[pos] > param->array[j]){
                pos = j;
            }
        int t = param->array[pos];
        param->array[pos] = param->array[i];
        param->array[i] = t;
    }
    return NULL;
}

void merge_sort()
{
    int n = N / 2, m = N;
    int i = 0, j = n, k = 0;
    while(i < n && j < m)
    {
        if(arr[i] <= arr[j]) res[k++] = arr[i++];
        else res[k++] = arr[j++];
    }
    while(i < n) res[k++] = arr[i++];
    while(j < m) res[k++] = arr[j++];
}

void print_array(int *array)
{
    for(int i = 0; i < N; i++)
    {
        if(i % 10 == 0 && i != 0)
            puts("");
        printf("%4d ", array[i]);
    }
    puts("");
}

int main()
{
    for(int i = 0; i < N; i++)
        arr[i] = (rand() % MAX);

    printf("[未排序的数组]\n");
    print_array(arr);

    pthread_t workers[CPU_NUM];
    struct param params[CPU_NUM];
    struct param *param;

    for(int i = 0; i < CPU_NUM; i++)
    {
        param = &params[i];
        param->array = arr;
        param->start = i * N / CPU_NUM;
        param->end = (i + 1) * N / CPU_NUM;
        pthread_create(&workers[i], NULL, select_sort, param);
    }

    for(int i = 0; i < CPU_NUM; i++)
    {
        pthread_join(workers[i], NULL);
    }

    printf("\n[排序后未归并的数组]\n");
    print_array(arr);

    merge_sort();
    printf("\n[归并后的数组]\n");
    print_array(res);

    return 0;
}

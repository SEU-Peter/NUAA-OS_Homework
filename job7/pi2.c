#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>

const int NUMBER = 1e5 + 10;
const int N = 100;

double PI;
struct param{
    int start;
    int end;
};

struct result{
    double sum;
};

void *worker(void *arg)
{
    int i;
    struct param *param = (struct param *)arg;
    struct result * result;
    double worker_output = 0;

    for(i = param->start; i <= param->end; i++)
    {
        if(i % 2) worker_output += 1 / (2.0 * i - 1);
        else worker_output -= 1 / (2.0 * i - 1);
    }
    result = malloc(sizeof(struct result));
    result->sum = worker_output;
    return result;
}

int main()
{
    pthread_t worker_tids[N];
    struct param params[N];
    PI = 0;

    int i;
    for(i = 0; i < N; i++)
    {
        struct param *param;
        param = &params[i];
        param->start = i * NUMBER + 1;
        param->end = (i + 1) * NUMBER;
        pthread_create(&worker_tids[i], NULL, worker, param);
    }

    for(i = 0; i < N; i++){
        struct result *result;
        pthread_join(worker_tids[i], (void **)&result);
        PI += result->sum;
        free(result);
    }
    PI *= 4;
    printf("PI:%.10lf\n", PI);

    return 0;
}

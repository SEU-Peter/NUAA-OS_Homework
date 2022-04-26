#include <stdio.h>
#include <pthread.h>

#define NUMBER 1e7 + 10

double worker_output;

void *worker(void *arg)
{
    int i;
    int n =  NUMBER / 2;
    for (i = 0; i < n; i++)
    {
        if(i%2 == 0)
        {
            worker_output += (1/(2.0*i+1));
        }
        else
        {
            worker_output -= (1/(2.0*i+1));
        }
    }
    return NULL;
}

double master_output;

void master()
{
    int i;

    int n =  NUMBER / 2;
    for (i = n; i < NUMBER; i++)
    {
        if(i%2 == 0)
        {
            master_output += (1/(2.0*i+1));
        }
        else
        {
            master_output -= (1/(2.0*i+1));
        }
    }
}

int main()
{ 
    pthread_t worker_tid;
    double total;

    pthread_create(&worker_tid, NULL, worker, NULL);
    master(); 
    pthread_join(worker_tid, NULL);
    total = master_output + worker_output;
    printf("master_output = %.10lf, worker_output = %.10lf, total = %.10lf, pai = %.10lf\n", master_output, worker_output, total,total*4);
    return 0;
}
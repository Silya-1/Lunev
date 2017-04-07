#define _GNU_SOURCE             /* See feature_test_macros(7) */
#include <sched.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>
#include <math.h>
#define COUNT 2240

typedef  struct Data_t
{
    double start_point;
    double end_point;
    pthread_t pthreads_id;
    int cpu;
    char trash[36 + 6400];
} Data;


void *thread_func(void *data)
{
	cpu_set_t cpu_set;
	CPU_ZERO(&cpu_set);
	CPU_SET((*(Data*)data).cpu , &cpu_set);
	pthread_setaffinity_np((*(Data*)data).pthreads_id, sizeof(cpu_set_t), &cpu_set);
       double start_point = (*(Data*)data).start_point;
        double end_point = (*(Data*)data).end_point;
        double counter = start_point;
        double dx = 0.0000001;
        double sum = 0;

        while(counter<end_point)
        {
        	sum += counter*counter*dx;
       		counter +=dx;
    	}
    (*(Data*)data).start_point = sum;
    return NULL;
}

int minimum (int a, int b)
{
	return a > b ? b : a;
}

int main(int argc, const char * argv[]) {


    if(argc != 2)
    {
        printf("not enough argumnets\n");
        return 0;
    }

    int n, max;
    max = sysconf(_SC_NPROCESSORS_ONLN);
    Data points[max]  __attribute__((aligned(128)));
   // printf("%p   %p\n", &points[0], &points[1]);
    n = (int) strtol(argv[1], NULL, 10);
    n = minimum(n, max);
    printf("%d\n", max);
    double integral_sum = 0;


    for(int i = 0; i<max; i++)
    {		
    	if(i < n)
    	{
       	 	points[i].start_point = (COUNT/n)*i;
        	points[i].end_point = points[i].start_point + (COUNT/n);
        	points[i].cpu = i % n;
        	pthread_create(&(points[i].pthreads_id), NULL, thread_func,(void *)&(points[i]));
        }else
        {
        	points[i].start_point = (COUNT/n)*0;
        	points[i].end_point = points[i].start_point + (COUNT/n);
        	points[i].cpu = i % max;
        	pthread_create(&(points[i].pthreads_id), NULL, thread_func,(void *)&(points[i]));
        }	
    }
    for(int i = 0; i<max; i++)
    {
    	pthread_join(points[i].pthreads_id, NULL);
    	if(i < n)
        	integral_sum += points[i].start_point;
    }

    printf("%f\n",(float)integral_sum);
    //free(pthreads_ids);
    //free(points);

    return 0;
}

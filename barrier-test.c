#include "sthread.h"
#include <stdio.h>
#include <sys/time.h>
#include <stdlib.h>
#include <sys/mman.h>

#ifndef N
#define N 2
#endif
#define M 16

int *a;
struct timeval tv1, tv2;
struct timezone tz1, tz2;
int duration;
sthread_barrier_t barrier;

void *func(void *args)
{
	int i, k, j;
	i = (int)args;
	for(k=0;k<1024*M/N;k++)
		a[k] = k;

	for(j=0;j<1000;j++)
	for(k=1;k<1024*M/N;k++)
		a[k] = a[k-1] * a[k+1];

	sthread_barrier_wait(&barrier);

	for(k=0;k<1024*M/N;k++)
		a[k] = k;


	sthread_exit(NULL);
	return NULL;
}

int main()
{
	int i;
	sthread_t thread[N];
	sthread_barrier_init(&barrier, NULL, N);
	a = (int *)mmap(NULL, 1024 * sizeof(int) * M, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1 ,0);

	gettimeofday(&tv1, &tz1);
	for(i=0;i<N;i++)
		sthread_create(&thread[i], NULL, func, (void *)i);

	sthread_main_wait(N);

	for(i=0;i<N;i++)
		sthread_join(thread[i], NULL);
	gettimeofday(&tv2, &tz2);

	duration = 1000000*(tv2.tv_sec - tv1.tv_sec) + tv2.tv_usec - tv1.tv_usec;
	printf("exec time %d \n", duration);

	sthread_barrier_destroy(&barrier);
	sthread_main_exit(0);

	return 0;
}

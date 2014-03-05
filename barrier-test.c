#include "sthread.h"
#include <stdio.h>
#include <sys/time.h>
#include <stdlib.h>
#include <sys/mman.h>

#ifndef N
#define N 2
#endif
#define M 2

// allocate M pages
int a[1024*M] __attribute__((aligned(4096)));
int b[1024*M] __attribute__((aligned(4096)));

struct timeval tv1, tv2;
struct timezone tz1, tz2;
int duration;

sthread_barrier_t barrier;

void *func(void *args)
{
	int p, i, k, j;
	i = (int)args;

	for(p=0;p<100;p++) {
			for(k=i*M/N;k<(i+1)*M/N;k++) {
					for(j=0;j<1024;j++)
							a[k*1024+j] = b[k*1024+j] + 1;
			}

			sthread_barrier_wait(&barrier);

			for(k=i*M/N;k<(i+1)*M/N;k++) {
					for(j=0;j<1024;j++)
							b[k*1024+j] = a[k*1024+j] + 1;
			}

			sthread_barrier_wait(&barrier);

	}

	sthread_exit(NULL);
	return NULL;
}

int main()
{
	int i;
	sthread_t thread[N];
	sthread_barrier_init(&barrier, NULL, N);

	sthread_shared(a, sizeof(int)*1024*M);
	sthread_shared(b, sizeof(int)*1024*M);

	gettimeofday(&tv1, &tz1);
	for(i=0;i<N;i++)
		sthread_create(&thread[i], NULL, func, (void *)i);

	sthread_main_wait(N);

	for(i=0;i<N;i++)
		sthread_join(thread[i], NULL);
	gettimeofday(&tv2, &tz2);

	duration = 1000000*(tv2.tv_sec - tv1.tv_sec) + tv2.tv_usec - tv1.tv_usec;
	printf("exec time %d \n", duration);

	for(i=0;i<1024*M;i++)
		printf("%d ", b[i]);
	printf("\n");

	sthread_barrier_destroy(&barrier);
	sthread_main_exit(0);

	return 0;
}

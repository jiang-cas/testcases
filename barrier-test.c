#include "sthread.h"
#include <stdio.h>

#define N 8

int *a;
sthread_barrier_t barrier;

void *func(void *args)
{
	int i, k;
	sthread_t thread;
	thread = sthread_self();
	i = thread.tid;
	a[i] = i;
	sthread_barrier_wait(&barrier);

	if(i > 0 && i < N-1)
		a[i] = a[i-1] + a[i+1];


	sthread_exit(NULL);
	return NULL;
}

int main()
{
	int i;
	sthread_t thread[N];
	sthread_barrier_init(&barrier, NULL, N);
	a = (int *)mvshared_malloc(sizeof(int) * N);

	for(i=0;i<N;i++)
		sthread_create(&thread[i], NULL, func, NULL);

	sthread_main_wait(N);

//	sthread_barrier_wait(&barrier);

	for(i=0;i<N;i++)
		sthread_join(thread[i], NULL);

	for(i=0;i<N;i++)
		printf("%d ", a[i]);
	printf("\n");

	mvshared_free(a);
	return 0;
}

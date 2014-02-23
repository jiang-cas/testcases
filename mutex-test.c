#include "sthread.h"
#include <stdio.h>

#define N 10

int *a;
sthread_mutex_t mutex;

void *func(void *args)
{
	int i, j;
	sthread_t thread;
	thread = sthread_self();
	i = thread.tid;
	sthread_mutex_lock(&mutex);
	or(j=0;j<N;j++)
		a[j] = i + j + 10;
	sthread_mutex_unlock(&mutex);
	sthread_exit(NULL);
	return NULL;
}

int main()
{
	int i;
	sthread_t thread[N];
	sthread_mutex_init(&mutex, NULL);
	a = (int *)mvshared_malloc(sizeof(int) * N);

	for(i=0;i<N;i++)
		sthread_create(&thread[i], NULL, func, NULL);

	sthread_main_wait(N);

	for(i=0;i<N;i++)
		sthread_join(thread[i], NULL);
	
	for(i=0;i<N;i++)
		printf("%d ", a[i]);
	printf("\n");
	
	sthread_mutex_destroy(&mutex);
	mvshared_free(a);
	return 0;
}

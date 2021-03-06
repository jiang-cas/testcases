#include "sthread.h"
#include <stdio.h>
#include <sys/time.h>

#ifndef N
#define N 2
#endif

int *a;
sthread_mutex_t mutex;
struct timeval tv1, tv2;
struct timezone tz1, tz2;
int duration;

void *func(void *args)
{
	int i, j,k;
	sthread_t thread;
	thread = sthread_self();
	i = thread.tid;
	sthread_mutex_lock(&mutex);
	for(k=0;k<1000000/N;k++)
	for(j=0;j<256;j++)
		a[j] = i * j + 10;
	sthread_mutex_unlock(&mutex);
	sthread_exit(NULL);
	return NULL;
}

int main()
{
	int i;
	sthread_t thread[N];
	sthread_mutex_init(&mutex, NULL);
	a = (int *)mvshared_malloc(sizeof(int) * 1024);

	gettimeofday(&tv1, &tz1);
	for(i=0;i<N;i++)
		sthread_create(&thread[i], NULL, func, NULL);

	sthread_main_wait(N);

	for(i=0;i<N;i++)
		sthread_join(thread[i], NULL);
	
	gettimeofday(&tv2, &tz2);
	duration = 1000000*(tv2.tv_sec - tv1.tv_sec)+tv2.tv_usec -tv1.tv_usec;
	printf("exec time %d \n", duration);
	
	sthread_mutex_destroy(&mutex);
	mvshared_free(a);
	return 0;
}

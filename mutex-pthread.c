#include <pthread.h>
#include <stdio.h>
#include <sys/time.h>

#ifndef N
#define N 2
#endif

int *a;
pthread_mutex_t mutex;
struct timeval tv1, tv2;
struct timezone tz1, tz2;
int duration;

void *func(void *args)
{
	int i, j,k;
	pthread_t thread;
	thread = pthread_self();
	i = thread.tid;
	pthread_mutex_lock(&mutex);
	for(k=0;k<1000000/N;k++)
	for(j=0;j<256;j++)
		a[j] = i * j + 10;
	pthread_mutex_unlock(&mutex);
	pthread_exit(NULL);
	return NULL;
}

int main()
{
	int i;
	pthread_t thread[N];
	pthread_mutex_init(&mutex, NULL);
	a = (int *)mvshared_malloc(sizeof(int) * 1024);

	gettimeofday(&tv1, &tz1);
	for(i=0;i<N;i++)
		pthread_create(&thread[i], NULL, func, NULL);


	for(i=0;i<N;i++)
		pthread_join(thread[i], NULL);
	
	gettimeofday(&tv2, &tz2);
	duration = 1000000*(tv2.tv_sec - tv1.tv_sec)+tv2.tv_usec -tv1.tv_usec;
	printf("exec time %d \n", duration);
	
	pthread_mutex_destroy(&mutex);
	mvshared_free(a);
	return 0;
}

#include <pthread.h>
#include <stdio.h> 
#include <sys/time.h>
#include <unistd.h>
#include <stdlib.h>

#ifndef N 
#define N  3 
#endif

#define TCOUNT 150
#define COUNT_LIMIT 140
 
int     *count; 
pthread_mutex_t count_mutex; 
pthread_cond_t count_threshold_cv; 

struct timeval tv1, tv2;
struct timezone tz1, tz2;
int *duration;
 
void *inc_count(void *idp)  
{ 
  int j,i; 
  double result=0.0; 
 
  for (i=0; i<TCOUNT; i++) { 
    pthread_mutex_lock(&count_mutex); 
    (*count)++; 
 
    /*  
    Check the value of count and signal waiting thread when condition is 
    reached.  Note that this occurs while mutex is locked.  
    */ 
    if (*count == COUNT_LIMIT) { 
	gettimeofday(&tv2, &tz2);
	*duration = 1000000*(tv2.tv_sec - tv1.tv_sec)+tv2.tv_usec -tv1.tv_usec;
      pthread_cond_signal(&count_threshold_cv); 
      //printf("inc_count(): thread %d, count = %d  Threshold reached.\n", *my_id, *count); 
      } 
    //printf("inc_count(): thread %d, count = %d, unlocking mutex\n", *my_id, *count); 
    pthread_mutex_unlock(&count_mutex); 
 
    /* Do some work so threads can alternate on mutex lock */ 
    for (j=0; j<1000000; j++) 
      result = result + j; 
    } 
	pthread_exit(NULL); 
} 
 
void *watch_count(void *idp)  
{ 
 
  //printf("Starting watch_count(): thread %d\n", *my_id); 
 
  /* 
  Lock mutex and wait for signal.  Note that the pthread_cond_wait  
  routine will automatically and atomically unlock mutex while it waits.  
  Also, note that if COUNT_LIMIT is reached before this routine is run by 
  the waiting thread, the loop will be skipped to prevent pthread_cond_wait 
  from never returning.  
  */ 
  pthread_mutex_lock(&count_mutex); 
  if (*count<COUNT_LIMIT) { 
    pthread_cond_wait(&count_threshold_cv, &count_mutex); 
    //printf("watch_count(): thread %d Condition signal received.\n", *my_id); 
    } 
  pthread_mutex_unlock(&count_mutex); 
  pthread_exit(NULL); 
} 
 
int main (int argc, char *argv[]) 
{ 
  int i, rc; 
  pthread_t threads[N]; 

	count = (int *)malloc(sizeof(int));
	*count = 0;

	duration = (int *)malloc(sizeof(int));
//  pthread_attr_t attr; 
 
  /* Initialize mutex and condition variable objects */ 
  pthread_mutex_init(&count_mutex, NULL); 
  pthread_cond_init (&count_threshold_cv, NULL); 
 
  /* For portability, explicitly create threads in a joinable state */ 
 // pthread_attr_init(&attr); 
 // pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE); 

 gettimeofday(&tv1, &tz1);
 for(i=0;i<N-1;i++) 
	 pthread_create(&threads[i], NULL, inc_count, NULL); 
  pthread_create(&threads[N-1], NULL, watch_count, NULL); 
	
 
  /* Wait for all threads to complete */ 
  for (i=0; i<N; i++) { 
    pthread_join(threads[i], NULL); 
  } 
  //printf ("Main(): Waited on %d  threads. Done.\n", N); 
 
	//*duration = 1000000 * (tv2.tv_sec - tv1.tv_sec) + tv2.tv_usec - tv1.tv_usec;
printf("copy time %d usec \n", *duration);

  /* Clean up and exit */ 
 // pthread_attr_destroy(&attr); 
  pthread_mutex_destroy(&count_mutex); 
  pthread_cond_destroy(&count_threshold_cv); 
  //pthread_exit(NULL); 
} 

#include "sthread.h"
#include <stdio.h> 
 
#define NUM_THREADS  3 
#define TCOUNT 10 
#define COUNT_LIMIT 12 
 
int     *count; 
int     thread_ids[3] = {0,1,2}; 
sthread_mutex_t count_mutex; 
sthread_cond_t count_threshold_cv; 
 
void *inc_count(void *idp)  
{ 
  int j,i; 
  double result=0.0; 
  int *my_id = (int *)idp; 
 
  for (i=0; i<TCOUNT; i++) { 
    sthread_mutex_lock(&count_mutex); 
    (*count)++; 
    printf("inc_count(): thread %d, count = %d, locking mutex\n", *my_id, *count); 
 
    /*  
    Check the value of count and signal waiting thread when condition is 
    reached.  Note that this occurs while mutex is locked.  
    */ 
    if (*count == COUNT_LIMIT) { 
      sthread_cond_signal(&count_threshold_cv); 
      printf("inc_count(): thread %d, count = %d  Threshold reached.\n", *my_id, *count); 
      } 
    printf("inc_count(): thread %d, count = %d, unlocking mutex\n", *my_id, *count); 
    sthread_mutex_unlock(&count_mutex); 
 
    /* Do some work so threads can alternate on mutex lock */ 
    for (j=0; j<1000; j++) 
      result = result + j; 
    } 
	sthread_exit(NULL); 
} 
 
void *watch_count(void *idp)  
{ 
  int *my_id = (int *)idp; 
 
  printf("Starting watch_count(): thread %d\n", *my_id); 
 
  /* 
  Lock mutex and wait for signal.  Note that the pthread_cond_wait  
  routine will automatically and atomically unlock mutex while it waits.  
  Also, note that if COUNT_LIMIT is reached before this routine is run by 
  the waiting thread, the loop will be skipped to prevent pthread_cond_wait 
  from never returning.  
  */ 
  sthread_mutex_lock(&count_mutex); 
  if (*count<COUNT_LIMIT) { 
    sthread_cond_wait(&count_threshold_cv, &count_mutex); 
    printf("watch_count(): thread %d Condition signal received.\n", *my_id); 
    } 
  sthread_mutex_unlock(&count_mutex); 
  sthread_exit(NULL); 
} 
 
int main (int argc, char *argv[]) 
{ 
  int i, rc; 
  sthread_t threads[3]; 

	count = (int *)mvshared_malloc(sizeof(int));
	*count = 0;
//  pthread_attr_t attr; 
 
  /* Initialize mutex and condition variable objects */ 
  sthread_mutex_init(&count_mutex, NULL); 
  sthread_cond_init (&count_threshold_cv, NULL); 
 
  /* For portability, explicitly create threads in a joinable state */ 
 // pthread_attr_init(&attr); 
 // pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE); 
  sthread_create(&threads[0], NULL, inc_count, (void *)&thread_ids[0]); 
  sthread_create(&threads[1], NULL, inc_count, (void *)&thread_ids[1]); 
  sthread_create(&threads[2], NULL, watch_count, (void *)&thread_ids[2]); 
	
sthread_main_wait(3);
 
  /* Wait for all threads to complete */ 
  for (i=0; i<NUM_THREADS; i++) { 
    sthread_join(threads[i], NULL); 
  } 
  printf ("Main(): Waited on %d  threads. Done.\n", NUM_THREADS); 
 
  /* Clean up and exit */ 
 // pthread_attr_destroy(&attr); 
  sthread_mutex_destroy(&count_mutex); 
  sthread_cond_destroy(&count_threshold_cv); 
  //sthread_exit(NULL); 
} 

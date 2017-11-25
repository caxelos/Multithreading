#include <pthread.h>
#include <stdio.h>
#include <errno.h>
//you can represent a semaphore by an INT that is synchronized by a mutex. 

//pthread_mutex_init
//pthread_mutex_lock
//pthread_mutex_destroy
//pthread_mutex_unlock
extern pthread_mutex_t mutex;
pthread_mutexattr_t myattr;
extern  pthread_cond_t cond;

volatile int waiting = 0;


int my_down(pthread_mutex_t *mutex);


int my_init(pthread_mutex_t *mutex, int val)  {
  pthread_mutexattr_t myattr;
  
  if (pthread_mutexattr_init(&myattr) != 0 ) {
    printf("Error at initializing the attributes. Exiting\n");
    return -1;
  }
 
  if (pthread_mutexattr_settype(&myattr,  PTHREAD_MUTEX_NORMAL) != 0 ) {
    printf("Error at initializing the attributes. Exiting\n");
    return -1;
  }

  if (pthread_mutex_init(mutex, &myattr) != 0) {
    printf("Error at creating mutex. Exiting\n");  
    return -1;
  }


  if (val == 0)  {
    if (my_down(mutex) != 0)
      return -1;
      
  }


  return 0;
}

int my_up(pthread_mutex_t *mutex)  {

  if ( pthread_mutex_unlock(mutex) != 0)  {
    printf("Error unlocking the mutex. Exiting\n");
    return -1;
  }


  waiting--;
  printf("send signal now...\n");
    if ( pthread_cond_signal(&cond) != 0)  {
      printf("Error sending signal. Exiting\n");
      return -1;
    }
  

  return 0;
}


int my_down(pthread_mutex_t *mutex) {
  
  if ( pthread_mutex_lock(mutex) != 0)  { 
     printf("Error locking the mutex. Exiting\n");
  }

  
  while (waiting==1){
    pthread_cond_wait(&cond, mutex);
      
  }
  waiting++;
   
 
  return 0;
}

int my_destroy(pthread_mutex_t *mutex) {

  return 0;
}


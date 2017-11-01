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



int my_init(void)  {
   
  if (pthread_mutexattr_init(&myattr) != 0 ) {
    printf("Error at initializing the attributes. Exiting\n");
    return -1;
  }
 
  if (pthread_mutexattr_settype(&myattr,  PTHREAD_MUTEX_ERRORCHECK) != 0 ) {
    printf("Error at initializing the attributes. Exiting\n");
    return -1;
  }

  if (pthread_mutex_init(&mutex, &myattr) != 0) {
    printf("Error at creating mutex. Exiting\n");  
    return -1;
  }

  if (pthread_cond_init(&cond, NULL) != 0) {
    printf("Error at initializing condition. Exiting\n");
    return -1;
  }

  return 0;
}

int my_up(void)  {

  if ( pthread_mutex_unlock(&mutex) != 0)  {
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


int my_down(void) {


  waiting++;
  while (waiting==2)  {
    pthread_cond_wait(&cond, &mutex);
      
  }
    
  while (1) {
    if ( pthread_mutex_lock(&mutex) != 0)  {
      printf("sleep now...\n");
      pthread_cond_wait(&cond, &mutex);
      
      printf("Error locking the mutex. Exiting\n");
    }
    else break;
  }

 
  return 0;
}

int my_destroy(void) {

  return 0;
}


#include <pthread.h>
#include <stdio.h>
//you can represent a semaphore by an INT that is synchronized by a mutex. 

//pthread_mutex_init
//pthread_mutex_lock
//pthread_mutex_destroy
//pthread_mutex_unlock
extern pthread_mutex_t mutex;


int my_init(void)  {
  
  
  
  if (pthread_mutex_init(&mutex, NULL) != 0) {
    printf("Error at creating mutex. Exiting\n");  
    return -1;
  }

  return 0;
}

int my_up(void)  {
  if ( pthread_mutex_unlock(&mutex) == -1)  {
    printf("Error unlocking the mutex. Exiting\n");
    return -1;
  }

  return 0;
}


int my_down(void) {
  if ( pthread_mutex_lock(&mutex) == -1)  {
    printf("Error locking the mutex. Exiting\n");
    return -1;
  }
  
  return 0;
}

int my_destroy(void) {

  return 0;
}
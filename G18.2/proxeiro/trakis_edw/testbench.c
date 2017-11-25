#include <stdio.h>
#include <pthread.h>
#include "myBinSema.h"

#define MAXIMUM 100

pthread_mutex_t  mutex;
pthread_mutex_t  mutexA;
pthread_mutex_t  mutexB;
pthread_cond_t cond;
volatile int i = 0;

void *thread_A(void *argms)  {
  
  while (i < MAXIMUM)  {   
    printf("A waits the mutex\n");    
    if (my_down(&mutex) == -1)
      return NULL;
    
      
    printf("A took the mutex\n");
    i++;
    
    
    printf("A leaving the mutex\n");
    if (my_up(&mutex) == -1)
      return NULL;
      
    
  }
    
    return NULL;
}


int main(int argc, char *argv[])  {
   pthread_t tidA;

 
//    if (my_init(&mutex,1) == -1)
//      return -1;
      if (my_init(&mutexA,1) == -1)
        return -1;
      if (my_init(&mutexB,1) == -1)
        return -1;
   
   if ( pthread_create(&tidA, NULL, thread_A, NULL) == -1 )  {
      printf("Error creating thread. Exiting\n");
      return 0;
    }
 
    
   while (i < MAXIMUM)  {

    printf("B waits the mutex\n");     
    if (my_down(&mutex) == -1)
      return -1;
  
    printf("B took the mutex\n");
    i++;
    printf("B leaving the muter\n");
    
    if (my_up(&mutex) == -1)
      return -1;
  
  
    
  }  
  if ( pthread_join(tidA, NULL) == -1)  {
    printf("Error at join\n");
    return -1;
  }


  printf("i is %d\n", i);
  return 0;
}

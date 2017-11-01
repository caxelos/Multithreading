#include <stdio.h>
#include <pthread.h>
#include "myBinSema.h"

pthread_mutex_t  mutex;
volatile int i = 0;

void *thread_A(void *argms)  {
  
  while (i < 20)  {   
    printf("A waits the mutex\n");    
    if (my_down() == -1)
      return NULL;
    
    printf("A took the mutex\n");
    i++;
    
    
    printf("A leaving the mutex\n");
    if (my_up() == -1)
      return NULL;
  
    
    
  }
    
    return NULL;
}


int main(int argc, char *argv[])  {
   pthread_t tidA;
 
    if (my_init() == -1)
      return -1;
   
   if ( pthread_create(&tidA, NULL, thread_A, NULL) == -1 )  {
      printf("Error creating thread. Exiting\n");
      return 0;
    }
 
    
   while (i < 20)  {

    printf("B waits the mutex\n");     
    if (my_down() == -1)
      return -1;
  
    
    printf("B took the mutex\n");
    i++;
    printf("B leaving the muter\n");
    
    if (my_up() == -1)
      return -1;
  
  
    
  }  
  if ( pthread_join(tidA, NULL) == -1)  {
    printf("Error at join\n");
    return -1;
  }

  return 0;
}
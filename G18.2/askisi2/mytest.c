#include <stdio.h>
#include "pthread.h"
#include "myBinSema.h"

/*
int my_init(pthread_mutex_t *mutex);

int my_up(pthread_mutex_t *mutex);

int my_down(pthread_mutex_t *mutex);

int y_destroy(pthread_mutex_t *mutex);
*/


pthread_mutex_t mutex;

int main(int argc, char *argv[])  {

  my_init(&mutex);  
  my_down(&mutex);
  my_down(&mutex);

 printf("printed!\n");

  return 0;
}
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>

#include "myBinSema.h"


struct passenger {
  pthread_t tid;
  int passId;
};
typedef struct passenger passengerT;


int program_counter = 0;
volatile int passInsideTrain = 0;

bsem allowEntrance, waitForEntrance;
bsem allowExit, waitForExit;
bsem beginTrain;


void *thread_passCode(void *passArgs)  {

  passengerT *args = (passengerT *)passArgs;
  
  printf("Passenger %d waiting...\n", args->passId );
  my_down( &allowEntrance );
  passInsideTrain++;
  printf("Passenger %d got inside the train...\n", args->passId );
  my_up( &waitForEntrance );
  
  if (passInsideTrain == 5)
    my_up( &beginTrain );

  /* - Ma den exoume pei oti oi passengers katastrefontai molis ftasoume??!
   * 
   */
  
  return NULL;
}

int main( int argc, char *argv[] )  {
  passengerT *passArgs;
  int dailyPassengers = 0;
  int fd;//control file descriptor
  int i;
  char buffer[3];
  int curr_time = 500;
  
  /*
   *open file including the arrival times
   */  
  fd = open("control_file.txt", O_RDONLY, S_IRWXU);
  if (fd < 0) {
    printf("Error opening control file. Exiting\n");
    return -1;
  }
  
  
  /*
   * Initialize the semaphores
   */
  if ( my_init( &allowEntrance, 0) == -1)  {
    printf("Error initializing \"allowEntrance\" semaphore. Exiting\n");
    return -1;
  }
  if ( my_init( &waitForEntrance, 0) == -1)  {
    printf("Error initializing \"allowEntrance\" semaphore. Exiting\n");
    return -1;
  }
  if ( my_init( &allowExit, 0) == -1)  {
    printf("Error initializing \"allowEntrance\" semaphore. Exiting\n");
    return -1;
  }
  if ( my_init( &waitForExit, 0) == -1)  {
    printf("Error initializing \"allowEntrance\" semaphore. Exiting\n");
    return -1;
  }
  if ( my_init( &beginTrain, 0) == -1)  {
    printf("Error initializing \"allowEntrance\" semaphore. Exiting\n");
    return -1;
  }


  
  
  printf("**************** Start the Process ******************************\n");
  
  
 /*
  * find the nexts(passengers) from the input file, according to their arrival time
  */
  while (passInsideTrain <= 5)  {
    i = -1;
    do {
      i++;
      read(fd, buffer+i, sizeof(char) );  
      
    } while (buffer[i] != '\n');
  
    if ( atoi(buffer) > curr_time ) { // if time arrival is bigger than the current time
      buffer[i] = '\0';
      printf("curr_time: %d seconds\n", curr_time);
      printf("next passenger: %d seconds\n", atoi(buffer) );
      lseek(fd, -( strlen(buffer)+1 ), SEEK_CUR );
      
      
      curr_time++;
    }
    else { // a new passenger now can go to the entrance, create a thread for him
      dailyPassengers++;
  
      passArgs = (passengerT *)malloc( sizeof(passengerT) );
      passArgs->passId=dailyPassengers;
          
      pthread_create( &(passArgs)->tid, NULL, (void *)thread_passCode, (void *)passArgs );
      
      
     /*
      * Now train code to insert him into the trai
      */
      my_up( &allowEntrance );
      
      my_down( &waitForEntrance );         
    
    }
  }
  printf("train is full\n");
  buffer[i] = '\0';
  printf("************** End Of Process ******************************\n");
  
  printf("last buffer is %d\n", atoi(buffer) );
  
  
  return 0;
}
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>

#include "myBinSema.h"

#define MAX_TRAIN_SEATS 5

struct passenger {
  pthread_t tid;
  int passId;
  int arrivalTime;
};
typedef struct passenger passengerT;


volatile int curr_time=0;
volatile int passInsideTrain = 0;
volatile int trainSize;

bsem allowEntrance, waitForEntrance;
bsem allowExit, waitForExit;
bsem beginTrain;


void *thread_passCode(void *passArgs)  {

  passengerT *args = (passengerT *)passArgs;
  
  //printf("Passenger: passenger %d waiting for entrance...\n", args->passId);// , args->arrivalTime, curr_time);
  my_down( &allowEntrance );
  passInsideTrain++;
  printf("Passenger: passenger %d got inside the train. Total passengers: %d. Arrival time: %d secs, Exit time: %d secs\n",args->passId, passInsideTrain, args->arrivalTime, curr_time);
  my_up( &waitForEntrance );
  
  if (passInsideTrain == trainSize)  {
    //printf("Passenger: I am number %d, in one minite begin the train...\n", args->passId);
    
    my_up( &beginTrain );
    //printf("Passenger: I am number %d, please begin the train...\n", args->passId); 
  }  
    
 /*
  * Now the train is traveling...
  */ 
   
  //printf("Passenger: passenger %d waiting for exit...\n", args->passId );
  my_down( &allowExit );
  passInsideTrain--;
  printf("Passenger: passenger %d exited the train. Total passengers: %d. Arrival time: %d secs, Exit time: %d secs\n", args->passId, passInsideTrain, args->arrivalTime, curr_time);
  
  my_up( &waitForExit);    

    
  return NULL;
}

int main( int argc, char *argv[] )  {
  passengerT *passArgs;
  int dailyPassengers = 0, waitingPassengers = 0;
  
  int fd;//control file descriptor
  int i;
  char buffer[3];
  int numOfRoutes = 0;
  
  /*
   *open file including the arrival times
   */  
  fd = open(argv[1], O_RDONLY, S_IRWXU);
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

  printf("Give the size of the train: ");
  scanf("%d", &trainSize);
  
  
  printf("\n**************** Start the Process ******************************\n\n");
  
  
 /*
  * find the nexts(passengers) from the input file, according to their arrival time and put the to the train
  */
  
  while (1) {
   printf("\n**************** Next route: %d ************************\n", numOfRoutes); 
   /*
    * begin the next route
    */
     while( waitingPassengers < trainSize || curr_time < atoi(buffer) )  {//twra 3s...irthe sta 4

      i = -1;
      do {
        i++;
        if (0 == read(fd, buffer+i, sizeof(char) ) ) { //EOF reached
          printf("************** End Of Process ******************************\n\n");
          printf("*** NumOfRoutes: %d\n", numOfRoutes);
          return 0;  
        }  
      
      } while (buffer[i] != '\n');
  
      if ( atoi(buffer) > curr_time ) { // havent arived yet
        buffer[i] = '\0';
  //      printf("curr_time: %d seconds\n", curr_time);
  //      printf("next passenger: %d seconds\n", atoi(buffer) );
        lseek(fd, -( strlen(buffer)+1 ), SEEK_CUR );
      
        curr_time++;
      }
      else { // a new passenger now can go to the entrance, create a thread for him
        buffer[i] = '\0';
        dailyPassengers++;
        waitingPassengers++;

        passArgs = (passengerT *)malloc( sizeof(passengerT) );
        passArgs->passId=dailyPassengers;
        passArgs->arrivalTime = atoi(buffer);
          
        pthread_create( &(passArgs)->tid, NULL, (void *)thread_passCode, (void *)passArgs );
      
      }
   }      
   waitingPassengers = waitingPassengers - trainSize;
   printf("**** Number of passengers waiting: %d. Number of passengers on board: %d\n", waitingPassengers, passInsideTrain);
       /*
        * Now train code to insert the next passenger into the train
        */
    while (1)  {
        
	if (passInsideTrain == trainSize)
          break;
        else {
	  my_up( &allowEntrance );
	  //printf("Train: Train allowed entrance to passenger\n");
          my_down( &waitForEntrance );        
	}
        //printf("Train: Train received the signal from passenger to get in...\n");
      
    }
  
    printf("\nTrain: waiting for train to begin\n");
    my_down( &beginTrain );
    printf("Train: received the signal from the last passenger to start\n");
 
  
   /*
    * train beginned
    */
    printf("\n\n **** train is full and it starts traveling ***\n\n");

   /*
    * train stopped
    */
 
 
 
 
  
   /*
    * Now put these passengers out of the train
    */
    while (passInsideTrain != 0)  {
      my_up( &allowExit );
      //printf("Train: Train allowed exit\n");
      my_down( &waitForExit );
      //printf("Train: received the signal from passenger that exited...\n");
    }
  
 
   numOfRoutes++;
 } 
  
  return 0;
}

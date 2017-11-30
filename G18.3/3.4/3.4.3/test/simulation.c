#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>

#include "myBinSema.h"
#include "semlib.h"

#define TRUE 0
#define FALSE 1


#define CCR_DECLARE(label) \
          bsem R_mtx, R_q1, R_q2; \
          volatile int R_n1=0, R_n2=0;
          
          
#define CCR_INIT(label) \
          mybsem_init(&R_mtx,1); mybsem_init(&R_q1,0);mybsem_init(&R_q2,0);

#define CCR_EXEC(label, cond, body) \
          printf("before_mtx_down\n");down(&R_mtx);printf("after_mtx_down\n"); \
          while (!cond)  { \
            R_n1++; \
            if (R_n2 > 0) { \
              R_n2--; \
              printf("before_0th_up\n");up(&R_q2);printf("after_0th_down\n"); \
            } \
            else { \
              printf("after_mtx_up\n");up(&R_mtx);printf("after_mtx_up\n"); \
            } \
            /*printf("before_down\n");*/down(&R_q1);/*printf("after_down\n");*/ \
            R_n2++; \
            if (R_n1 > 0) { \
              R_n1--; \
              up(&R_q1); \
            } \
            else { \
              up(&R_q2); \
            } \
            down(&R_q2); \
          } \
          body \
          if (R_n1 > 0) { \
            R_n1--; \
            /*printf("before_1st_up\n");*/up(&R_q1);/*printf("after_1st_up\n");*/ \
          } \
          else if (R_n2 > 0) { \
            R_n2--; \
            /*printf("before_2nd_up\n");*/up(&R_q2);/*printf("after_2nd_up\n");*/ \
          } \
          else { \
            printf("before_mtx_up\n");up(&R_mtx);printf("after_mtx_up\n"); \
          }


struct passenger {
  pthread_t tid;
  int passId;
  int arrivalTime;
};
typedef struct passenger passengerT;


volatile int curr_time=0;
volatile int passInsideTrain = 0;
volatile int trainSize;

pthread_cond_t cond_allowEntrance, cond_waitForEntrance, cond_waitForExit, cond_allowExit, cond_beginTrain;

pthread_mutex_t mutex_allowEntrance, mutex_waitForEntrance, mutex_waitForExit, mutex_allowExit, mutex_beginTrain;

volatile int var_allowEntrance = FALSE, var_waitForEntrance = FALSE, var_waitForExit = FALSE, var_allowExit = FALSE, var_beginTrain = FALSE;

//bsem /*allowEntrance,*/ waitForEntrance;
bsem beginTrain;
CCR_DECLARE(label);

void *thread_passCode(void *passArgs)  {

  passengerT *args = (passengerT *)passArgs;
  
  printf("Passenger: passenger %d waiting for entrance...\n", args->passId);// , args->arrivalTime, curr_time);

  
  CCR_EXEC(label, (passInsideTrain < 5), passInsideTrain++; printf("Passenger: passenger %d got inside the train. Total passengers: %d. Arrival time: %d secs, Exit time: %d secs\n",args->passId, passInsideTrain, args->arrivalTime, curr_time);); /*sleep(1)*/
    
  if (passInsideTrain == trainSize)  {
    printf("Passenger: I am number %d, the last passenger. Start the train\n", args->passId);
    
    //my_up( &beginTrain );
    pthread_mutex_lock( &mutex_beginTrain );
    var_beginTrain = TRUE;
    pthread_cond_signal( &cond_beginTrain );
    pthread_mutex_unlock( &mutex_beginTrain );
  }
   
 /*
  * Now the train is traveling...
  */ 
   
  //printf("Passenger: passenger %d waiting for exit...\n", args->passId );
  //my_down( &allowExit );
  
  pthread_mutex_lock( &mutex_allowExit);
  while ( var_allowExit == FALSE ) {
    pthread_cond_wait( &cond_allowExit, &mutex_allowExit);
  }
  var_allowExit = FALSE;
  pthread_mutex_unlock( &mutex_allowExit);

  
  passInsideTrain--;
  printf("Passenger: passenger %d exited the train. Total passengers: %d. Arrival time: %d secs, Exit time: %d secs\n", args->passId, passInsideTrain, args->arrivalTime, curr_time);fflush(stdout);
  
  //my_up( &waitForExit);  
  pthread_mutex_lock( &mutex_waitForExit );
  var_waitForExit = TRUE;
  pthread_cond_signal( &cond_waitForExit );
  pthread_mutex_unlock( &mutex_waitForExit );


  free( args );  

    
  return NULL;
}

int main( int argc, char *argv[] )  {  
  passengerT *passArgs;
  int dailyPassengers = 0, waitingPassengers = 0;
  
  int fd;//control file descriptor
  int i;
  char buffer[3];
  int numOfRoutes = 1;
  int eof = 0;
  
  /*
   *open file including the arrival times
   */  
  fd = open(argv[1], O_RDONLY, S_IRWXU);
  if (fd < 0) {
    printf("Error opening control file. Exiting\n");
    return -1;
  }
  
  
  /*
   * Init the region 
   */
  CCR_INIT(label);
  /*
   * Initialize the semaphores
   */
  if ( my_init( &mutex_allowEntrance, 1) == -1)  {
    printf("Error initializing \"mutex_allowEntrance\" semaphore. Exiting\n");
    return -1;
  }
  if ( my_init(&mutex_waitForEntrance, 1) == -1) {
    printf("Error initializing \"mutex_waitForEntrance\" semaphore. Exiting\n");
    return -1;
  }
  if ( my_init( &mutex_allowExit, 1) == -1)  {
    printf("Error initializing \"allowEntrance\" semaphore. Exiting\n");
    return -1;
  }
  if ( my_init(&mutex_waitForExit, 1) == -1) {
    printf("Error initializing \"mutex_waitForEntrance\" semaphore. Exiting\n");
    return -1;
  }
  if ( my_init(&mutex_beginTrain, 1) == -1) {
    printf("Error initializing \"mutex_waitForEntrance\" semaphore. Exiting\n");
    return -1;
  }

   
  
/*  if ( my_init( &allowEntrance, 0) == -1)  {
    printf("Error initializing \"allowEntrance\" semaphore. Exiting\n");
    return -1;
  }*/
  /*
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
*/  if ( my_init( &beginTrain, 0) == -1)  {
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
    while( (/*waitingPassengers < trainSize && */curr_time <= atoi(buffer) ) &&  eof == 0 )  {//twra 3s...irthe sta 4
 	//printf("reading: %d, current: %d\n", atoi(buffer), curr_time); 
      i = -1;
      do {
        i++;
        if (0 == read(fd, buffer+i, sizeof(char) ) ) { //EOF reached
	  eof = 1;	      
        }  
      
      } while (buffer[i] != '\n' && eof != 1);
     

      buffer[i] = '\0';	
      if ( atoi(buffer) > curr_time ) { // havent arived yet
        lseek(fd, -( strlen(buffer)+1 ), SEEK_CUR );
        curr_time++;
	if (waitingPassengers >= trainSize)
	  break;
      }
      else { 
        dailyPassengers++;
        waitingPassengers++;
 
        passArgs = (passengerT *)malloc( sizeof(passengerT) );
        passArgs->passId=dailyPassengers;
        passArgs->arrivalTime = atoi(buffer);
        
  
        pthread_create( &(passArgs)->tid, NULL, (void *)thread_passCode, (void *)passArgs );
        //printf("Waiting Passengers: %d, current time: %dsecs, arrival time of current passenger: %dsecs. A thread created for him\n", waitingPassengers, curr_time, atoi(buffer) );
      }


      if (eof == 1)
        break;     	

    } 
 
 
   //CCR_EXEC(label, (passInsideTrain == 5), );   
   while (passInsideTrain != 5) { }
   
   printf("\nTrain Officer: Number of passengers waiting: %d. Number of passengers on board: %d\n\n", waitingPassengers, passInsideTrain);
   waitingPassengers = waitingPassengers - trainSize;
       /*
        * Now train code to insert the next passenger into the train
        */
    
    
    
    
  
    printf("\n**************************************\nTrain: waiting for train to begin\n");
    //my_down( &beginTrain );
    pthread_mutex_lock( &mutex_beginTrain);
    while ( var_beginTrain == FALSE ) {
      pthread_cond_wait( &cond_waitForEntrance, &mutex_beginTrain );
    }
    var_beginTrain = FALSE;
    pthread_mutex_unlock( &mutex_beginTrain);


    printf("Train: I received the signal from the last passenger to start\n");
 
  
   /*
    * train beginned
    */
    printf("Train: I am full and I start traveling\n");

   /*
    * train stopped
    */
 
 
   printf("Train: I reached the destination\n**************************************\n\n"); 
 
  
   /*
    * Now put these passengers out of the train
    */
    while (passInsideTrain != 0)  {
      printf("Train: Train allowed exit\n");
      //my_up( &allowExit );
       pthread_mutex_lock( &mutex_allowExit );
       var_allowExit = TRUE;
       pthread_cond_signal( &cond_allowExit );
       pthread_mutex_unlock( &mutex_allowExit );

    
    
      //my_down( &waitForExit );
      pthread_mutex_lock( &mutex_waitForExit);
      while ( var_waitForExit == FALSE ) {
        pthread_cond_wait( &cond_waitForExit, &mutex_waitForExit );
      }
      var_waitForExit = FALSE;
      pthread_mutex_unlock( &mutex_waitForExit);

      printf("Train: received the signal from passenger that exited...\n");fflush(stdout);
    }
  
   if (waitingPassengers == 0 && eof == 1)
     break;
 
   numOfRoutes++;

 } 
  
  printf("************** End Of Process ******************************\n\n");


/*
  if (my_destroy( &allowEntrance ) == -1)
    return -1;

  if (my_destroy( &waitForEntrance ) == -1)
    return -1;

  if (my_destroy( &allowExit ) == -1)
    return -1;

  if (my_destroy( &waitForExit ) == -1)
    return -1;
*/  
  if (my_destroy( &beginTrain ) == -1)
    return -1;
 

  return 0;
}

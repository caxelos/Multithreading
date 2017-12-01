#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>

#include "myBinSema.h"
#include "semlib.h"

#define TRUE 1
#define FALSE 0


#define CCR_DECLARE(label) \
          volatile int label##_mode = FALSE; pthread_mutex_t label##_mtx; \
          pthread_cond_t label##_condvar;             
          
#define CCR_INIT(label) \
          pthread_mutex_init( &label##_mtx, NULL ); \
          //var = FALSE;
          

#define CCR_EXEC(label,cond,body) \
          pthread_mutex_lock( &label##_mtx); \
          while (cond == FALSE)  { \
            pthread_cond_wait( &label##_condvar, &label##_mtx); \
          } \
          body \
          pthread_mutex_unlock( &label##_mtx ); \
            


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
CCR_DECLARE(ENTRANCE);
CCR_DECLARE(WAIT_FOR_ALL_PASS);
CCR_DECLARE(ALLOW_EXIT);
CCR_DECLARE(WAIT_FOR_ALL_EXIT);
CCR_DECLARE(PASS_SIGNAL_TO_BEGIN);
CCR_DECLARE(TRAIN_WAITS_UNTIL_BEGIN);
//CCR_DECLARE(BEGIN_TRAIN);
//CCR_DECLARE(EXIT);

void *thread_passCode(void *passArgs)  {

  passengerT *args = (passengerT *)passArgs;
  
  printf("Passenger: passenger %d waiting for entrance...\n", args->passId);// , args->arrivalTime, curr_time);

  
  
  
   //kane wait edw
  CCR_EXEC(ENTRANCE, /*(passInsideTrain < trainSize)*/ (ENTRANCE_mode == TRUE),    
  printf("Passenger: passenger %d got inside the train. Total passengers: %d. Arrival time: %d secs, Exit time: %d secs\n",args->passId, passInsideTrain+1, args->arrivalTime, curr_time);
  passInsideTrain++;
  if (passInsideTrain == trainSize)  {
    
  
    ENTRANCE_mode = FALSE; 
    pthread_mutex_lock(&WAIT_FOR_ALL_PASS_mtx);
    WAIT_FOR_ALL_PASS_mode = TRUE;
    pthread_cond_signal(&WAIT_FOR_ALL_PASS_condvar);
    pthread_mutex_unlock(&WAIT_FOR_ALL_PASS_mtx);
  
    printf("Passenger: I am number %d, the last passenger. Start the train\n", args->passId);


    PASS_SIGNAL_TO_BEGIN_mode = FALSE; 
    pthread_mutex_lock(&TRAIN_WAITS_UNTIL_BEGIN_mtx);
    TRAIN_WAITS_UNTIL_BEGIN_mode = TRUE;
    pthread_cond_signal(&TRAIN_WAITS_UNTIL_BEGIN_condvar);
    pthread_mutex_unlock(&TRAIN_WAITS_UNTIL_BEGIN_mtx);

   
   /* 
    //my_up( &beginTrain );
    pthread_mutex_lock( &mutex_beginTrain );
    var_beginTrain = TRUE;
    pthread_cond_signal( &cond_beginTrain );
    pthread_mutex_unlock( &mutex_beginTrain );*/
  }
  ); 
      
 /*
  * Now the train is traveling...
  */ 
   
  //printf("Passenger: passenger %d waiting for exit...\n", args->passId );
  


 CCR_EXEC(ALLOW_EXIT, (ALLOW_EXIT_mode == TRUE),    
  passInsideTrain--;
  if (passInsideTrain == 0)  {
    ALLOW_EXIT_mode = FALSE; 
    pthread_mutex_lock(&WAIT_FOR_ALL_EXIT_mtx);
    WAIT_FOR_ALL_EXIT_mode = TRUE;
    pthread_cond_signal(&WAIT_FOR_ALL_EXIT_condvar);
    pthread_mutex_unlock(&WAIT_FOR_ALL_EXIT_mtx);
  
  }
  ); /*sleep(1)*/

 printf("Passenger: passenger %d exited the train. Total passengers: %d. Arrival time: %d secs, Exit time: %d secs\n", args->passId, passInsideTrain, args->arrivalTime, curr_time);
  

  

/*  
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
*/

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
  CCR_INIT(ENTRANCE);
  CCR_INIT(WAIT_FOR_ALL_PASS);
  CCR_INIT(ALLOW_EXIT);
  CCR_INIT(WAIT_FOR_ALL_EXIT);
  CCR_INIT(PASS_SIGNAL_TO_BEGIN);
  CCR_INIT(TRAIN_WAITS_UNTIL_BEGIN);
 
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
    
   pthread_mutex_lock(&ENTRANCE_mtx);
   ENTRANCE_mode = TRUE;
   pthread_cond_broadcast(&ENTRANCE_condvar);
   pthread_mutex_unlock(&ENTRANCE_mtx);
   CCR_EXEC(WAIT_FOR_ALL_PASS, (WAIT_FOR_ALL_PASS_mode == TRUE),    
      WAIT_FOR_ALL_PASS_mode = FALSE;      
   );
   
   printf("\nTrain Officer: Number of passengers waiting: %d. Number of passengers on board: %d\n\n", waitingPassengers, passInsideTrain);
   waitingPassengers = waitingPassengers - trainSize;
       /*
        * Now train code to insert the next passenger into the train
        */    
    printf("\n**************************************\nTrain: waiting for train to begin\n");

  
   pthread_mutex_lock(&PASS_SIGNAL_TO_BEGIN_mtx);
   PASS_SIGNAL_TO_BEGIN_mode = TRUE;
   pthread_cond_broadcast(&PASS_SIGNAL_TO_BEGIN_condvar);
   pthread_mutex_unlock(&PASS_SIGNAL_TO_BEGIN_mtx);
   CCR_EXEC(TRAIN_WAITS_UNTIL_BEGIN, (TRAIN_WAITS_UNTIL_BEGIN_mode == TRUE),
      TRAIN_WAITS_UNTIL_BEGIN_mode = FALSE;
   );


   /* 
    //my_down( &beginTrain );
    pthread_mutex_lock( &mutex_beginTrain);
    while ( var_beginTrain == FALSE ) {
      pthread_cond_wait( &cond_waitForEntrance, &mutex_beginTrain );
    }
    var_beginTrain = FALSE;
    pthread_mutex_unlock( &mutex_beginTrain);
   */






    printf("Train: I received the signal from the last passenger to start\n");  
    printf("Train: I am full and I start traveling\n"); 
    printf("Train: I reached the destination\n**************************************\n\n"); 

   
   /*
    * Now put these passengers out of the train
    */
   pthread_mutex_lock(&ALLOW_EXIT_mtx);
   ALLOW_EXIT_mode = TRUE;
   pthread_cond_broadcast(&ALLOW_EXIT_condvar);
   pthread_mutex_unlock(&ALLOW_EXIT_mtx);
   CCR_EXEC(WAIT_FOR_ALL_EXIT, (WAIT_FOR_ALL_EXIT_mode == TRUE),
     WAIT_FOR_ALL_EXIT_mode = FALSE;
   );
   printf("Train: received the signal from passenger that exited...\n");
    
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
  
  if (my_destroy( &beginTrain ) == -1)
    return -1;
*/ 

  return 0;
}

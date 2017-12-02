#include <pthread.h>
#include <stdio.h>
#include "mandelCore.h"
#include "mandelThreaded.h"
#include "semlib.h"
//#include <sched.h>

#define TRUE 1
#define FALSE 0


#define WORKING 1
#define NOT_WORKING 2
#define DONE 3

taskT *tasks;

CCR_EXTERN(SLAVE_WAITS_THE_DRAWER);
CCR_EXTERN(DRAWER_SIGNALS_THE_SLAVE);
CCR_EXTERN(SLAVE_SIGNAL_ENDED_CALCS);
CCR_EXTERN(DRAWER_WAITS_FOR_CALCS);

//extern bsem calcReady, waitWhileDrawing;
extern int next;
extern pthread_cond_t cond_waitWhileDrawing, cond_calcReady;
extern pthread_mutex_t mutex_waitWhileDrawing, mutex_calcReady;
extern int var_waitWhileDrawing, var_calcReady;
 
/* init_threads()
- called by the main thread
- it creates nofslices threads and initializes them as NOT_WORKING
- its thread runs forever in the waitUntiGetTask function 
*/
int init_threads(int numOfThreads)  {
  int i;

  for (i = 0; i < numOfThreads; i++)  {
    //tasks[i].status = NOT_WORKING;
  /*  
    if ( mybsem_init( &(tasks[i]).sem, 0) != 0 ) {
      printf("Error initializing the mutex. Exiting\n");
      return -1;
    }
  */
  
  tasks[i].condVal = FALSE;
    if ( pthread_create( &(tasks[i]).tid, NULL, (void *)waitUntilGetTask, (void *)&tasks[i]) != 0) {
      printf("Error creating thread %d. Exiting\n", i);
      return -1;
    }
  }

  return 0;
}


/* waitUntilGetTask()
- Called by init_threads()
- Here run all the "slave" threads, those who do the calculations
- If the main thread marks a thread as WORKING from main function
the thread calls the mandel_Calc function and then marks itself as
DONE
- If a thread is not in WORKING status, it gives the processor to
the next available thread
*/
void *waitUntilGetTask(void *newtask)  {    
  static int slaveNum = 0;
  int i;
  taskT *slave = (taskT *)newtask;
  
  i = slaveNum;
  slaveNum++;
  while (1)  {
      //down( &(slave->sem) );
      pthread_mutex_lock( &slave->mutex);
      while ( slave->condVal == FALSE ) {
        pthread_cond_wait( &slave->cond, &slave->mutex );
      }
      slave->condVal = FALSE;
      pthread_mutex_unlock( &slave->mutex);
  
      
      mandel_Calc( slave->pars, slave->maxIterations, slave->res );
  
 
      pthread_mutex_lock(&SLAVE_WAITS_THE_DRAWER_mtx);
      SLAVE_WAITS_THE_DRAWER_mode = TRUE;
      pthread_cond_broadcast(&SLAVE_WAITS_THE_DRAWER_condvar);
      pthread_mutex_unlock(&SLAVE_WAITS_THE_DRAWER_mtx);
      CCR_EXEC(DRAWER_SIGNALS_THE_SLAVE, (DRAWER_SIGNALS_THE_SLAVE_mode == TRUE),
         DRAWER_SIGNALS_THE_SLAVE_mode = FALSE;

   );

      
      next = i;
      
      // these tin kainourgia thesi mnimis
      //up( &calcReady );//slave->status = DONE;
      SLAVE_SIGNAL_ENDED_CALCS_mode = FALSE; 
      pthread_mutex_lock(&DRAWER_WAITS_FOR_CALCS_mtx);
      DRAWER_WAITS_FOR_CALCS_mode = TRUE;
      pthread_cond_signal(&DRAWER_WAITS_FOR_CALCS_condvar);
      pthread_mutex_unlock(&DRAWER_WAITS_FOR_CALCS_mtx);

  
  }

 return NULL; 
}

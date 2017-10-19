#include <pthread.h>
#include <stdio.h>
#include "mandelCore.h"
#include "mandelThreaded.h"
#include <sched.h>


#define WORKING 1
#define NOT_WORKING 2
#define DONE 3

taskT *tasks;


int find_next_finished_thread(int numOfThreads)  {
  int i = 0;

  
  while (1)  {
    if (tasks[i].status == DONE)  {
	tasks[i].status = NOT_WORKING;	
	
        return i;
    }
    i = (i+1)%numOfThreads;
  }


  return 0;
}


 
/* init_threads()
- called by the main thread
- it creates nofslices threads and initializes them as NOT_WORKING
- its thread runs forever in the waitUntiGetTask function 
*/
int init_threads(int numOfThreads)  {
  int i;

  for (i = 0; i < numOfThreads; i++)  {
    tasks[i].status = NOT_WORKING;

    if ( pthread_create( &(tasks[i]).tid, NULL, (void *)waitUntilGetTask, (void *)&tasks[i]) != 0) {//(void *) (intptr_t)i) != 0) {
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

  taskT *slave = (taskT *)newtask;
  while (1)  {
    if ( slave->status == WORKING)  {
      mandel_Calc( slave->pars, slave->maxIterations, slave->res );
      slave->status = DONE;
    }
    if (sched_yield() != 0)  {
      printf("problem at yield, waitUntilGetTask\n");
      return NULL;
    }
  }

 return NULL; 
}

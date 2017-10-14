#include <stdio.h>
#include <stdlib.h>


struct task {
  mandel_Pars *pars;//param
  int *res;//param
  int maxIterations;//param
  pthread_t tid;
  int status;
};
typedef struct task taskT;

extern int init_threads(int numOfThreads) ;

extern void *waitUntilGetTask(void *newtask);

extern int find_next_available_thread(int numOfThreads);


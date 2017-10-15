#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define MAX_SIZE 100

struct quickArgs {
  int left;
  int right; 
};
typedef struct quickArgs quickArgsT;

struct task {
  pthread_t tid;
  quickArgsT *args;
  struct task *next;
};
typedef struct task taskT;  

int array[MAX_SIZE];
taskT *taskHead = NULL;


void destroy_task(taskT *destroyTask) {
  static int i=0;
  i++;
  printf("%da\n", i);
  taskT *curr = NULL; 
  taskT *prev = NULL;

  if (destroyTask == taskHead)  {
    free( taskHead->args );
    free( taskHead );
    return ;
  }
     
  prev = taskHead;
  curr = taskHead->next;
  while (1)  {
    if (curr == destroyTask)  {
      prev->next = curr->next;
      free( curr->args );
      free( curr );
      curr = NULL;
      printf("%db\n", i);
      return ;
    }
    prev = curr;
    curr = curr->next;
  } 

  return ;

}

//tin xreiazomai gt alliws den mporw na kanw destroy
void insert_to_task_list(taskT *newtask)  {
  taskT *currtask;
  
  currtask = taskHead;
  while (currtask->next != NULL) {
    currtask = currtask->next;
  }
  currtask->next = newtask;

  return ;
}  
 

taskT *createTask(int left, int right)  {
  quickArgsT *newArgs = NULL;
  taskT *newtask = NULL;
  
  newArgs = (quickArgsT *)malloc( sizeof(quickArgsT) );
  if (newArgs == NULL)  {
    printf("Error allocating args memory\n");
    return NULL;
  } 
  newArgs->left = left;
  newArgs->right = right;
  
  newtask = (taskT *)malloc( sizeof(taskT) );
  if (newtask == NULL)  {
    printf("Error allocating task memory\n");
    return NULL;
  }    
  newtask->args = newArgs;
  newtask->next = NULL;  
 


  //arxi krisimou
  //insert_to_task_list(newtask);
  //telos krisimou
  
  
  return newtask;
}

//void *myquicksort(int *array, int left, int right )  {
void *myquicksort(void *nextTask) {
  static int numOfThreads=1;
  static int created = 1;
  static int destroyed = 0;
  //take left or right node
  taskT *mytask = (taskT *)nextTask;
  taskT *rTask = NULL;
  taskT *lTask = NULL;
   
  //init counters
  int i = mytask->args->left-1;//left-1;
  int j = mytask->args->right;
  int o = array[mytask->args->right];
  int temp;
        
  while (1)  {
    while ( array[++i] < o )  {    
    }  
    
    while( o < array[--j] )  {
    }
    
    if (i >= j)
      break;
      
    temp = array[i];
    array[i] = array[j];
    array[j] = temp;  
  }
  
  temp = array[i];
  array[i] = array[mytask->args->right];
  array[mytask->args->right] = temp;

   
  if (mytask->args->left < i-1)  {
    numOfThreads++;
    created++;     
    lTask = createTask(mytask->args->left, i-1);
    if (lTask == NULL) {
      return NULL;
    }
    
  
    //printf("createdThread:(%d,%d)\n", mytask->lTask->args->left, mytask->lTask->args->right);     
    if (pthread_create( &(lTask->tid), NULL, (void *)myquicksort, (void *)lTask) != 0)  {     
      printf("Error creating left thread. Exiting\n");
      return NULL;
    }
  }
      
  if (i+1 < mytask->args->right)  {
    numOfThreads++;
    rTask = createTask(i+1, mytask->args->right);
    if ( rTask == NULL) {
      return NULL;
    }
    created++;
     
 
    //printf("createdThread:(%d,%d)\n", mytask->rTask->args->left, mytask->rTask->args->right);
    if ( pthread_create( &( rTask->tid), NULL, (void *)myquicksort, (void *) rTask) != 0 ) {
      printf("Error creating right thread. Exiting\n");
      return NULL;
    }
      
  }

   //printf("finishedThread:(%d,%d)\n", mytask->args->left, mytask->args->right);
  if (mytask == taskHead) {
    while ( numOfThreads != 1) {
      if (sched_yield() != 0) {
        printf("Error at CPU relinquish. Exiting\n");
        return NULL;
      }
    }
    printf("created:%d, destroyed:%d\n", created, destroyed);
  }

  //printf("FREEThread:(%d,%d)\n", mytask->args->left, mytask->args->right);
  free( mytask->args );
  free( mytask );
 

  return NULL;
}

int main(int argc, char *argv[])  {
  quickArgsT *args;
  int i;
  
  //init array
  for (i = 0; i < MAX_SIZE; i++)  {
    array[i] = i%10;  
  } 
  
  //init main thread
  taskHead = (taskT *)malloc( sizeof(taskT) );
  if (taskHead == NULL)
    return -1;


  args = (quickArgsT *)malloc( sizeof(quickArgsT) );
  if (args == NULL)  {
    printf("Error allocating main thread. Exiting\n");
    return -1;
  }
  args->left = 0;
  args->right = MAX_SIZE-1;
  taskHead->args = args; 
 
  myquicksort(taskHead);


  //print final array

  for (i = 0; i < MAX_SIZE; i++)  {
    printf("%d, ", array[i]);
  }
  putchar('\n');
  
  return 0;
}

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define MAX_SIZE 100

struct quickArgs {
  int left;
  int right; 
};
typedef struct quickArgs quickArgsT;

struct task {
  pthread_t tid;
  quickArgsT *args;
  int childThreads;
  struct task *parent;
};
typedef struct task taskT;  

int array[MAX_SIZE];
taskT *taskHead = NULL;



taskT *createTask(taskT *parent, int left, int right)  {
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
  newtask->childThreads = 0;
  newtask->parent = parent;

  return newtask;
}


void *myquicksort(void *nextTask) {
  
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
   
    mytask->childThreads++;    
    lTask = createTask(mytask, mytask->args->left, i-1);
    if (lTask == NULL) {
      return NULL;
    }
    
     
    if (pthread_create( &(lTask->tid), NULL, (void *)myquicksort, (void *)lTask) != 0)  {     
      printf("Error creating left thread. Exiting\n");
      return NULL;
    }
  }
      
  if (i+1 < mytask->args->right)  {
  
    mytask->childThreads++;
    rTask = createTask(mytask, i+1, mytask->args->right);
    if ( rTask == NULL) {
      return NULL;
    }

     
 
    if ( pthread_create( &( rTask->tid), NULL, (void *)myquicksort, (void *) rTask) != 0 ) {
      printf("Error creating right thread. Exiting\n");
      return NULL;
    }
      
  }



  while ( mytask->childThreads != 0) {
    printf("thread %d waiting\n", (int)mytask->tid);
    if (sched_yield() != 0) {
      printf("Error at CPU relinquish. Exiting\n");
      return NULL;
    }
  }

  if (mytask != taskHead)
    mytask->parent->childThreads--;
  
  
  free( mytask->args );
  free( mytask );

  return NULL;
}


int read_number()  {
  int i = 0;
  char number_str[7];

  while (1) {  
    number_str[i] = getchar();
    if (number_str[i] == ' ')
      continue;
    else if (number_str[i] == '\n')  {
      number_str[i] = '\0';
      break;
    }
    else
      i++;
  }

   return atoi(number_str);
}


int main(int argc, char *argv[])  {
  quickArgsT *args;
  int i=0, numOfElements;
  
 
  
  printf("Please number of elements<enter>: ");
  numOfElements = read_number();

  //init array
  for (i = 0; i < numOfElements; i++)  {
    printf("no.%d: ", i+1);
    array[i] = read_number();
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
  args->right = numOfElements-1;
  taskHead->args = args; 
  taskHead->childThreads = 0;
  taskHead->parent = NULL;
  myquicksort(taskHead);


  for (i = 0; i < numOfElements; i++)  {
    printf("%d, ", array[i]);
  }
  putchar('\n');
  
  return 0;
}

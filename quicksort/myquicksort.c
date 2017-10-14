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
  struct task *next;
  volatile int t;
  volatile int taking;
  volatile int number;
};
typedef struct task taskT;  

int array[MAX_SIZE];
taskT *taskHead = NULL;


int priority(int t, int number) {
  taskT *curr;
  
  for (curr = taskHead; curr != NULL; curr=curr->next)  {
    if (t == 0)
      return 0;
    else if (curr->t < t)
      return 0;
    else if (curr->t == t && curr->number < number)
      return 0;    
      
  }  
  printf("priority\n");
  return 1; 
}

void bakery_algorithm(taskT *newtask)  {
  taskT *curr;
  
  newtask->taking = 1;
  newtask->t = -1;  
  for (curr=taskHead; curr != NULL; curr=curr->next) {
    if (curr->t > newtask->t)
      newtask->t = curr->t;
  }
  newtask->t = newtask->t + 1;
  newtask->taking = 0;
  
      
  for (curr=taskHead; curr != NULL; curr=curr->next) {
    while (curr->taking == 1) {
    
    }
    while (priority(curr->t, curr->number) == 1) {
      printf("waiting\n");
     
    }
  }  
  
  
}

void delete_thread(taskT *delThread) {
  taskT *curr;
  
  for (curr = head; curr != NULL; curr=curr->next) [
    if (curr->next == delThread)  {
      curr->next = delThread->next;
      free( delThread );
      return ;
    }
  } 




void destroy_task_list(void) {
  taskT *curr = taskHead; 
  taskT *prev = NULL;
    
  for (curr = taskHead; curr->next != NULL; curr = curr->next ) {
    prev = curr;
    curr = curr->next;
    free(prev);
  }
  free(curr);
}

void insert_to_task_list(taskT *newtask)  {
  taskT *currtask;
  if (taskHead == NULL) {
    taskHead = newtask;
  }
  else {
    currtask = taskHead;
    while (currtask->next != NULL) {
      currtask = currtask->next;
    }
    currtask->next = newtask;
  }  
  
  
  return ;    
}


taskT *createTask(int left, int right)  {
  static int numOfThreads = 0;  
  
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
  newtask->taking = 0;
  newtask->t = 0;
  newtask->number = numOfThreads;
  
  numOfThreads++;
  bakery_algorithm(newtask);
  //arxi krisimou
  insert_to_task_list(newtask);
  //telos krisimou
  newtask->t = 0;
  
  
  return newtask;
}

//void *myquicksort(int *array, int left, int right )  {
void *myquicksort(void *args) {
  //take left or right node
  taskT *mytask = (taskT *)args;
  printf("called\n");
  
  //init counters
  int i = mytask->args->left-1;//left-1;
  int j = mytask->args->right;
  int o = array[mytask->args->right];
  int temp;
  
  taskT *newLeftTask = NULL;
  taskT *newRightTask = NULL;

      
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
    printf("inside\n");    
    newLeftTask = createTask(mytask->args->left, i-1);
    if (newLeftTask == NULL) {
      return NULL;
    }      
    if (pthread_create( &(newLeftTask->tid), NULL, (void *)myquicksort, (void *)newLeftTask) != 0)  {
      printf("Error creating left thread. Exiting\n");
      return NULL;
    }
  }
      
  if (i+1 < mytask->args->right)  {
    printf("inside\n");
    newRightTask = createTask(i+1, mytask->args->right);
    if (newRightTask == NULL) {
      return NULL;
    }  
   
    if ( pthread_create( &(newRightTask->tid), NULL, (void *)myquicksort, (void *)newRightTask) != 0 ) {
      printf("Error creating right thread. Exiting\n");
      return NULL;
    }
      
  }
 
  return NULL;
}

int main(int argc, char *argv[])  {
  taskT mainTask;
  quickArgsT *args;
  int i;
  
  //init array
  for (i = 0; i < MAX_SIZE; i++)  {
    array[i] = i%10;  
  } 


  //init main thread
  args = (quickArgsT *)malloc( sizeof(quickArgsT) );
  if (args == NULL)  {
    printf("Error allocating main thread. Exiting\n");
    return -1;
  }
  args->left = 0;
  args->right = MAX_SIZE-1;
  mainTask.args = args;
  
  myquicksort(&mainTask);
  //print final array
  for (i = 0; i < MAX_SIZE; i++)  {
    printf("%d, ", array[i]);
  }
  putchar('\n');
  //destroy_task_list();
  
  return 0;
}

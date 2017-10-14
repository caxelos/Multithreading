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
  struct task *taskNext;
};
typedef struct task taskT;  



int array[MAX_SIZE];
taskT *taskHead = NULL;

//int final_array[MAX_SIZE];

void destroy_task_list(void) {
  taskT *curr = taskHead; 
  taskT *prev = NULL;
  
  for (curr = head; curr != NULL; curr = curr->next ) {
    prev = curr;
    curr = curr->next;
    free(prev);
  }
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

taskT *createArgs(int left, int right)  {
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
  insert_to_task_list(newtask);
  
  return newtask;
}

void *myquicksort(int *array, int left, int right )  {
  int i = left-1;
  int j = right;
  int o = array[right];
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
  array[i] = array[right];
  array[right] = temp;

  
  if (left < i-1)  {
    newLeftTask = createTask(left, i-1);
    if (newLeftTask == NULL)
      return NULL;
      
    if (pthread_create( &(newLeftTask->tid), NULL, (void *)myquicksort, (void *)newLeftTask->args ) != 0)  {
      printf("Error creating left thread. Exiting\n");
      return NULL;
    }
  }
  else {
    return NULL;
  }  
    
  if (i+1 < right)  {
    newRightTask = createTask(i+1, right);
    if (newRightTask == NULL)
      return NULL;
  
    if ( pthread_create( &(newRightTask->tid), NULL, (void *)myquicksort, (void *)newRightTask->args) != 0 ) {
      printf("Error creating right thread. Exiting\n");
      return NULL;
    }
      
  }
  else {
    return NULL;
  }

  return NULL;
}

int main(int argc, char *argv[])  {
  int i;
  
  //init array
  for (i = 0; i < MAX_SIZE; i++)  {
    array[i] = i%10;  
  } 


  myquicksort(array, 0, MAX_SIZE-1);
  //print final array
  for (i = 0; i < MAX_SIZE; i++)  {
    printf("%d, ", array[i]);
  }
  putchar('\n');
  destroy_task_list();
  
  return 0;
}



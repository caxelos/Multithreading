#include <stdio.h>

#define MAX_SIZE 100
int array[MAX_SIZE];

//int final_array[MAX_SIZE];

void myquicksort(int *array, int left, int right )  {
  int i = left-1;
  int j = right;
  int o = array[right];
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
  array[i] = array[right];
  array[right] = temp;

left -> X -> X -> X -> right
  
  if (left < i-1)  {
    //create thread
    myquicksort(array, left, i-1);
  }
  else {
  //kill thread
  }  
    
  if (i+1 < right)  {
    // create thread
    myquicksort(array, i+1, right);
  }
  else {
  //kill thread
  }

  return ;
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


  printf("p=%d\n", p);
  return 0;
}



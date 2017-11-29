#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include "semlib.h"


int total_passengers = 0;
#define CCR_DECLARE(label) \
          bsem R_mtx, R_q1, R_q2; \
          int R_n1=0, R_n2=0;
           
          
#define CCR_INIT(label) \
          mybsem_init(&R_mtx,1); mybsem_init(&R_q1,0);mybsem_init(&R_q2,0);

#define CCR_EXEC(label, cond, body) \
          down(&R_mtx); \
          while (!cond)  { \
            R_n1++; \
            if (R_n2 > 0) { \
              R_n2--; \
              up(&R_q2); \
            } \
            else { \
              up(&R_mtx); \
            } \
            down(&R_q1); \
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
            up(&R_q1); \
          } \
          else if (R_n2 > 0) { \
            R_n2--; \
            up(&R_q2); \
          } \
          else { \
            up(&R_mtx); \
          }          

void *poulo(void *args)  {
   total_passengers++;
   printf("Thread %d: Waiting to get in CS...\n", *(int *)args );
   
   CCR_DECLARE(label);
   CCR_INIT(label);
   CCR_EXEC(label, (total_passengers <= 5), printf("Thread %d: Inside the CS\n ",*(int *)args);sleep(1););
   
   total_passengers--;
   printf("Thread %d: Just its out of CS...Total passengers: %d\n", *(int *)args, total_passengers );
  return NULL;
}

int main(int argc, char *argv[]) {
  int i = 0;
  pthread_t tid[20];


  for (i = 0; i < 20; i++)  {
    sleep(1);
    pthread_create(&tid[i], NULL, (void *)poulo, (void *)&i);
  }
  
  
  for (i = 0; i < 20; i++)  {
    pthread_join(tid[i], NULL);
  }

 
  return 0;
}
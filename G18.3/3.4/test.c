#include <stdio.h>
#include <pthread.h>
#include "semlib.h"

#define CCR_DECLARE(label) \
          bsem R_mtx, R_q1, R_q2; \
          int R_n1=0, R_n2=0; printf("%d%d\n", R_n1,R_n2);
           
          
#define CCR_INIT(label) \
          mybsem_init(&R_mtx,1); mybsem_init(&R_q1,0);mybsem_init(&R_q2,0);

#define CCR_EXEC(label, cond, body) \
          down(&R_mtx); \
          while (cond == 0)  { \
            R_n1++; \
            if (R_n2 > 0) { \
              R_n2--; \
              up(R_q2); \
            } \
            else { \
              up(R_mtx); \
            } \
            down(R_q1); \
            R_n2++; \
            if (R_n1 > 0) { \
              R_n1--; \
              up(R_q1); \
            } \
            else { \
              up(R_q2); \
            } \
            down(R_q2); \
          } \
          body \
          if (R_n1 > 0) { \
            R_n1--; \
            up(R_q1); \
          } \
          else if (R_n2 > 0) { \
            R_n2--; \
            up(R_q2); \
          } \
          else { \
            up(R_mtx); \
          }          


int main(int argc, char *argv[]) {

  CCR_DECLARE(label);


  return 0;
}
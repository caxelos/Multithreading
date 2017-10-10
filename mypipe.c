#include "mypipe.h"

#define MAX_EMPTY_TIMES 100
#define MAX_WRITE_TIMES 1000


extern char final_text[150];
struct buffinfo {
  int size;
  volatile int rdptr;
  volatile int wrptr;
};
typedef struct buffinfo *bufferT;


volatile bufferT buff = NULL;
int pipefd[2];


void pipe_init(int size)  {
  buff = (bufferT)malloc( sizeof(bufferT) );
  if (buff == NULL) {
    printf("Error allocating memory\n");
    exit(-1);
  }
  buff->size=size;  
  buff->rdptr=0;
  buff->wrptr=0;


  if ( pipe(pipefd) == -1)  {
    printf("Error at pipe\n");
    exit(-1);  
  }
 

}



void pipe_write(char c)  { 
  int nxt_wr_ptr = (buff->wrptr + 1)%buff->size;

  
  while (nxt_wr_ptr == buff->rdptr)  { //check if it is full
    printf("write: buffer is full\n");
  }   
  
  //arxi krisimou tmimatos-grapse ston agwgo    
  if (write (pipefd[1], &c,1) != 1)  {//circbuffer[wrptr] = c;
    printf("pipe_write:Error writing 1 byte\n");
    exit(-1);
  }  
  //telos krisimou tmimatos-auksise ton write-pointer
  printf("write: %c, pos: %d\n", c,buff->wrptr);
  buff->wrptr = nxt_wr_ptr;
 
    
  return ;

}


int pipe_read(void)  {
  char c;  
  int empty_times=0;
  //arxi krisimou tmimatos
  while (buff->wrptr == buff->rdptr) {//if empty
  
    empty_times++;
    printf("read(%d,%d): buffer is empty.%d times\n",buff->wrptr,buff->rdptr, empty_times);
    if (empty_times == MAX_EMPTY_TIMES) 
      return 0;
   
  }
  
  if (1 != read (pipefd[0], &c, 1) ) {
    printf("Error reading 1 Byte\n");
    exit(-1);
  }
  
  //telos krisimou kwdika - auksise ton read-pointer
  printf("read: %c, pos: %d\n", c,buff->rdptr);
  buff->rdptr = (buff->rdptr+1)%buff->size;
  
  final_text[ strlen(final_text) ] = c;  
  
  return 1;  
}




void pipe_close()  {
 
  if ( close(pipefd[0]) == -1  )  {
    printf("Error closing pipe read\n");
    return ;
  }
  if ( close(pipefd[1]) == -1  )  {
    printf("Error closing pipe write\n");
    return ;
  }
}


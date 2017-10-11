#include "mypipe.h"

struct buffinfo {
  int size;
  volatile int rdptr;
  volatile int wrptr;
  volatile int full;
};
typedef struct buffinfo *bufferT;

volatile bufferT buff = NULL;
int pipefd[2];
int fdin;
int fdout;
int endOfFile = 0;

void pipe_init(int size)  {
  buff = (bufferT)malloc( sizeof(bufferT) );
  if (buff == NULL) {
    printf("Error allocating memory\n");
    exit(-1);
  }
  buff->size=size;  
  buff->rdptr=0;
  buff->wrptr=0;
  buff->full =0;

  if ( pipe(pipefd) == -1)  {
    printf("Error at pipe\n");
    exit(-1);  
  }
 

}



void pipe_write(char c)  { 
  int nxt_wr_ptr = (buff->wrptr + 1)%buff->size;
  
  //allaksa apo nxt se curr
  while ((buff->wrptr==buff->rdptr))  {
    if (buff->full == 0) {
      buff->full = 1;
      break;
    } 
    printf("write: buffer is full\n");
  }   
  
  //arxi krisimou tmimatos-grapse ston agwgo    
  if (write (pipefd[1], &c,1) != 1)  {//circbuffer[wrptr] = c;
    printf("pipe_write:Error writing 1 byte:\n");
    exit(-1);
  }  
  //telos krisimou tmimatos-auksise ton write-pointer
  printf("write: %c, pos: %d\n", c,buff->wrptr);
  buff->wrptr = nxt_wr_ptr;
 
    
  return ;

}


int pipe_read(void)  {
  char c;  
  int bytesWrite;

  //arxi krisimou tmimatos
  while (buff->wrptr == buff->rdptr) {//if empty
    if (buff->full == 1)  {
      buff->full = 0;
      break ;
    }
    if (endOfFile == 1)
      return 0;  
  }
  
  if (1 != read (pipefd[0], &c, 1) ) {
    printf("Error reading 1 Byte\n");
    exit(-1);
  }
  
  //telos krisimou kwdika - auksise ton read-pointer
  bytesWrite = write(fdout, &c, sizeof(char) );
  if (bytesWrite == -1 )  {
    printf("Write error at writer() function.Exiting\n");
    exit(-1); 
  }     


  printf("read: %c, pos: %d\n", c,buff->rdptr);
  buff->rdptr = (buff->rdptr+1)%buff->size;
  
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


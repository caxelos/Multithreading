#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>

#define BUF_SIZE 10

struct buffinfo {
  int size;
  volatile int rdptr;
  volatile int wrptr;
};
typedef struct buffinfo *bufferT;

bufferT buff = NULL;
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

void pipe_write(char c)  {
 
  int nxt_wr_ptr = (buff->wrptr + 1)%buff->size;

  while (1)  {
    if ( nxt_wr_ptr != buff->rdptr )  {//check if it is full
      break;
    }
    printf("write: thread waiting\n");
  }   
  
  //arxi krisimou tmimatos-grapse ston agwgo    
  if (write (pipefd[1], &c,1) != 1)  {//circbuffer[wrptr] = c;
    printf("Error writing 1 byte\n");
    exit(-1);
  }  
  //telos krisimou tmimatos-auksise ton write-pointer
  buff->wrptr = nxt_wr_ptr;
  printf("write: %c, pos: %d\n", c,nxt_wr_ptr);
  return ;

}


int pipe_read(void)  {
  char c;  
  //arxi krisimou tmimatos
  while (1) {
    if (buff->wrptr != buff->rdptr)  { //if empty      
      break;
    }
  }
  if (1 != read (pipefd[0], &c, 1) ) {
    printf("Error reading 1 Byte\n");
    exit(-1);
  }
  
  //telos krisimou kwdika - auksise ton read-pointer
  buff->rdptr = (buff->rdptr+1)%buff->size;
  printf("read: %c, pos: %d\n", c,buff->rdptr);

  
  //kalese tin pipe_close() gia kleisimo tou agwgou

  
  return 1;  
}


void *thread_reader()  {
   int readval;
   
   while(1){
      readval = pipe_read();
      if (readval == 0)  {
        return NULL;
      }   
   }

 
}


void writer() {
   
   char  ch='A';
   int i = 0;
   
   while(1){
     pipe_write(ch+i);    
     i = (i+1)%24;//Letter of alphabet in ordermax numbers in alphabet is 24.    
   }

}



int main(int argc, char **argv)  {
  pthread_t tid1;
  
  pipe_init(BUF_SIZE);  
  if (pthread_create(&tid1,NULL,thread_reader,NULL) != 0) {
    printf("Error creating reader thread\n");
    exit(-1);
  }
  
  writer();  
    /*
  if (pthread_create(&tid2,NULL,thread_writer,NULL) != 0) {
    printf("Error creating writer thread\n");
    exit(-1);
  }  */
  
  
  //writer();  
   
  if (pthread_join(tid1,NULL) != 0) {
    printf("error at read thread\n");
    exit(-1);
  }
  /*
  if (pthread_join(tid2,NULL) != 0) {
    printf("error at write thread\n");
    exit(-1);
  }*/
 

  return 0;

}

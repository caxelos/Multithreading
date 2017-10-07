#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

#define BUF_SIZE 10
#define MAX_EMPTY_TIMES 100
#define MAX_WRITE_TIMES 1000

struct buffinfo {
  int size;
  volatile int rdptr;
  volatile int wrptr;
};
typedef struct buffinfo *bufferT;


const char text[150] = "I must send this text correctly. If it hasn't been sent in order there are problems in sychronization\n"; 
char final_text[150] = "\0";
bufferT buff = NULL;
int pipefd[2];
volatile int join_threads;



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
    printf("write: buffer is full\n");
  }   
  
  //arxi krisimou tmimatos-grapse ston agwgo    
  if (write (pipefd[1], &c,1) != 1)  {//circbuffer[wrptr] = c;
    printf("pipe_write:Error writing 1 byte\n");
    exit(-1);
  }  
  //telos krisimou tmimatos-auksise ton write-pointer
  buff->wrptr = nxt_wr_ptr;
  printf("write: %c, pos: %d\n", c,nxt_wr_ptr);
 
  return ;

}


int pipe_read(void)  {
  char c;  
  int empty_times=0;
  //arxi krisimou tmimatos
  while (1) {
    if (buff->wrptr != buff->rdptr)  { //if empty      
      break;
    }
    empty_times++;
    printf("read: buffer is empty.%d times\n", empty_times);
    if (empty_times == MAX_EMPTY_TIMES) 
      return 0;
   
  }
  if (1 != read (pipefd[0], &c, 1) ) {
    printf("Error reading 1 Byte\n");
    exit(-1);
  }
  
  //telos krisimou kwdika - auksise ton read-pointer
  buff->rdptr = (buff->rdptr+1)%buff->size;
  printf("read: %c, pos: %d\n", c,buff->rdptr);
  final_text[ strlen(final_text) ] = c;  
  
  return 1;  
}


void *thread_reader()  {
   int readval;
   
   while(1){
      readval = pipe_read();
      if (readval == 0)  {
        printf("Destroy pipe\n");
        pipe_close();
        join_threads = 1;
        return NULL;
      }   
   }

 
}


void writer() { 
   int i = 0;
   
   while(1){
     pipe_write(text[i]);    
     i++;
     if (text[i] == '\n') {
       printf("write: Write limit reached\n"); 
       return ;
     }     
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
  
  while (join_threads == 0) {
  
  }
  /*
  if (pthread_join(tid1,NULL) != 0) {
    printf("error at read thread\n");
    exit(-1);
  }*/
  
  printf("text: %s\n", text);
  printf("final_text: %s\n", final_text);

  return 0;

}

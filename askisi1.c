#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>

#define BUF_SIZE 10

struct buffinfo {
  int size;
  int rdptr;
  int wrptr;
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


/*  
  circbuffer = (char *)malloc( size*sizeof(char) );
  if (circbuffer == NULL)  {
    printf("Error allocating mem\n");
    return ;
  }
*/
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

int pipe_read(char *c)  {
   
  if (buff->wrptr == buff->rdptr)  { //if empty      
    //perimene mexri na graftoun data
  }
  
  if (1 != read (pipefd[0], c, 1) ) {
    printf("Error reading 1 Byte\n");
    return -1;
  }
  
  buff->rdptr = (buff->rdptr+1)%buff->size;
  printf("read: %c\n", *c );
  
  
  //kalese tin pipe_close() gia kleisimo tou agwgou
  
  return 1;  
}


void pipe_write(char c)  {
  int nxt_wr_ptr = (buff->wrptr + 1)%buff->size;
  
  if ( nxt_wr_ptr == buff->rdptr )  {//check if it is full
    //perimene mexri na diavastoun dedomena
  } 
  
  buff->wrptr = nxt_wr_ptr;
  //circbuffer[wrptr] = c;
  if (write (pipefd[1], &c,1) != 1)  {
    printf("Error writing 1 byte\n");
    exit(-1);
  }

  printf("write: %c\n", c);
  return ;

}

/*
void *thread_reader()  {
   while(1){
      char    ch;
      int     result;

      result = read (pipefd[0],&ch,1);
      if (result != 1) {
        perror("read");
        exit(3);
      }
      
      printf ("Reader: %c\n", ch);   
   }
}

*/
void *thread_writer()  {
   char  ch='A';
   int i = 0;
   while(1){
     pipe_write(ch+i); 
   
     i = (i+1)%24;//Letter of alphabet in ordermax numbers in alphabet is 24.    
   }
}


int main(int argc, char **argv)  {
  
  pipe_init(BUF_SIZE);
  pthread_t  tid1, tid2;
  
/*
  pthread_create(&tid1,NULL,thread_reader,NULL);
  pthread_create(&tid2,NULL,thread_writer,NULL);

  pthread_join(tid1,NULL);
  pthread_join(tid2,NULL);
*/  

  return 0;

}
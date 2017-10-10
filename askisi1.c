#include "mypipe.h"

volatile int join_threads=0;
const char text[150] = "I must send this text correctly. If it hasn't been sent in order there are problems in sychronization\n"; 
char final_text[150] = "\0";


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
    printf("poulo again\n");  
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

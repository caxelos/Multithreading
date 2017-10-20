#include "mypipe.h"

volatile int join_threads=0;
extern int fdin;
extern int fdout;
extern volatile int endOfFile;

//char *cyclBuffer=NULL;
/* thread_pipereader()
- Kaleitai mesw tis pthread_create
- Diavaze apto kukliko buffer kai grapse se ena arxeio
- Molis diavasei apto buffer miden xaraktires, kane deallocate
to buffer kai dwse sima na enwthei to thread me to kurio thread
*/
void *thread_pipereader(void *filename)  {
   int readval;
   
   fdout = open( (char *)filename , O_RDWR | O_CREAT, S_IRWXU ); 
   if (fdout == -1 )  {
     printf("Error opening output file. Please use command \"rm output.txt\" before running again\n");
     return NULL;
   }
    
   while(1){
      readval = pipe_read();
      if (readval == 0)  {
        join_threads = 1;
        return NULL;
      }
     
   }
}


/* pipewriter()
- diavazei input apo ena arxeio kai kalei tin sunartisi pipe_write()
gia na grapsei ston agwgo, xrisimopoiwntas tin read 
- Molis diavasei miden bytes apto input.txt, simainei oti ftasame sto EOF
- Tote termatise kai  gyrna stin main, opou meta perimenei na 
termatistei kai to allo thread
*/
void pipewriter(char *filename) { 
   int bytesRead;
   char c;
 
   fdin = open( (char *)filename , O_RDONLY, S_IRUSR );
   if (fdin == -1 )  {
     printf("Error opening input file. Exiting\n");
     return ;
   }

   while(1){
     bytesRead = read(fdin, &c, sizeof(char) );
     if (bytesRead == -1)  {
       printf("Read error at writer() function.Exiting\n");
       exit (-1) ; 
     }
     else if (bytesRead == 0) {//Eof 
       endOfFile = 1;
       return;
     }  
     pipe_write(c); 
   }
}

/*
Nikitakis Panagiotis, aem 1717
Axelos Christos, aem 1814

Algorithmos Peterson ws algorithmos sugxronismou
*/
int main(int argc, char **argv)  {
  pthread_t tid1;
  
  /*
  pare to size san 1h parametro apto command line
  */
  pipe_init( atoi(argv[1]) );  
  
  /*
  - ftiakse ena nima kai steilto na ektelesei ta read apto buffer
  - dwstou parametro apto command line to output.txt gia na grafei oti diavazei
  */
  if (pthread_create(&tid1,NULL,(void *)thread_pipereader,(void *)argv[3]) != 0) {
    printf("Error creating reader thread\n");
    exit(-1);
  }
  
  
  /*
  - to kurio nima trexei ta writes 
  - dwstou apto command line to input.txt
  */
  pipewriter( argv[2] );  
  
  /*
  - edw ulopoihse to pthread_join xrisimopoiwntas tin volatile metavliti join_threads
  - to thread pou kanei read, an den lavei alla dedomena, kanei to join_threads iso me
  ena kai termatizei
  */
  while (join_threads == 0) {
   
  }
  
  pipe_close();
 
  return 0;
}

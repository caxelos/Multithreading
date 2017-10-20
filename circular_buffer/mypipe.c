#include "mypipe.h"

/* struct buffinfo
 - size: to megethos tou pinaka
 - rdptr: i thesi anagnwsis ston pinaka
 - wrptr: i thesi eggrafis ston pinaka
 - full: energos simatoforos(oxi kanonikos), molis ginei miden mplokarei i write. 
 - empty: energos simatoforos(oxi kanonikos), molis ginei miden mplokarei i read
*/
struct buffinfo {
  int size;
  int rdptr;
  int wrptr;
  int full;
  int empty;
  int turn_full, turn_empty;
  int want_full1, want_full2;
  int want_empty1, want_empty2;
};
typedef struct buffinfo *bufferT;


/*
- To parapanw buffer prepei na nai volatile gia na min xrisimopoiountai oi registers
  otan kaneis anatheseis sta pedia tou struct(den xreiazetai ola ta peidia na nai volatile, 
  mono to empty kai full xreiazetai na einai)
*/
volatile bufferT buff = NULL;


/*
file descriptors twn input.txt kai output.txt
*/
int fdin;
int fdout;
volatile int endOfFile = 0;
volatile char *cyclBuffer;



/*
 void pipe_init(int size)
  - Desmevei dinamika mnimi gia to kukliko buffer
  - Desmevei dinamika mnimi gia ena struct pou exei plirofories sxetika me to buffer, opws 
  to megethos tou buffer, to simeio anagnwsis kai egrafis safto, an einai gemato kai an einai adeio
  - arxikopoiw tis times tou struct wste na pragmatopoihsw ton algorithmo Peterson
*/
void pipe_init(int size)  {
  buff = (bufferT)malloc( sizeof(bufferT) );
  if (buff == NULL) {
    printf("Error allocating memory\n");
    exit(-1);
  }
  buff->size=size;  
  buff->rdptr=0;
  buff->wrptr=0;
  buff->full =size;
  buff->empty=0;
  buff->want_full1 = 0; 
  buff->want_full2 = 0;
  buff->want_empty1 = 0;
  buff->want_empty2 = 0;

/*
  cyclBuffer = (char *)malloc( size*sizeof(char) ); 
  if (cyclBuffer==NULL)  {
    printf("Error allocating cyclBuffer memory. Exiting\n");
    exit (-1);
  }
*/
  return ;
}



/* pipe_write()
- Grafei enan xaraktira ston buffer. Tin kalei i thread_pipereader()
*/
void pipe_write(char c)  { 
  int nxt_wr_ptr = (buff->wrptr + 1)%buff->size;
  
  //allaksa apo nxt se curr
  while (buff->full == 0)  {
 
  } 

  //arxi algorithmou peterson
  buff->want_full1 = 1;
  buff->turn_full = 2;
  while ( (buff->want_full2 == 1) && (buff->turn_full != 1) ) {

  }
  //arxi CS
  buff->full--;
  //telos CS
  buff->want_full1 = 0;
  //telos algorithmou peterson
  
  cyclBuffer[buff->wrptr] = c;
  buff->wrptr = nxt_wr_ptr;


  //arxi algorithmou peterson
  buff->want_empty1 = 1;
  buff->turn_empty = 2;
  while ( (buff->want_empty2 == 1) && (buff->turn_empty != 1) ) {

  }

  // Arxi CS
  buff->empty++;
  // Telos CS
  buff->want_empty1 = 0;
  //telos algorithmou peterson
    
  return ;

}

/* pipe_read()
- Diavazei ena xaraktira apto buffer
- To pws epitigxanetai sigxronismos to anaferoume ston psevdokodika
- Tin kalei i thread_pipereader
*/
int pipe_read(void)  {
  char c;  
  int bytesWrite;

  while (buff->empty == 0) {//if empty
   if (endOfFile == 1)  {
       return 0; 
   }    
  }

  //arxi algorithmou peterson
  buff->want_empty2 = 1;
  buff->turn_empty = 1;
  while ( (buff->want_empty1 == 1) && (buff->turn_empty != 2) ){

  }
  //arxi CS
  buff->empty--;
  //telos CS
  buff->want_empty2 = 0;
  //telos algorithmou peterson
  
  c = cyclBuffer[buff->rdptr];
  bytesWrite = write(fdout, &c, sizeof(char) );
  if (bytesWrite == -1 )  {
    printf("Write error at writer() function.Exiting\n");
    exit(-1); 
  }     


  buff->rdptr = (buff->rdptr+1)%buff->size;

  
  //arxi algorithmou peterson
  buff->want_full2 = 1;
  buff->turn_full = 1;
  while ( (buff->want_full1 == 1) && (buff->turn_full != 2) ){

  } 
  //arxi CS
  buff->full++;
  //telos CS
  buff->want_full2 = 0;
  //telos algorithmou peterson

  return 1;  
}



/*
kane free ton buffer
*/
void pipe_close()  {

  //free( cyclBuffer );

}

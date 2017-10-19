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

extern char *cyclBuffer;



/*
 void pipe_init(int size)
  - Desmevei dinamika mnimi gia to kukliko buffer
  - Desmevei dinamika mnimi gia ena struct pou exei plirofories sxetika me to buffer, opws 
  to megethos tou buffer, to simeio anagnwsis kai egrafis safto, an einai gemato kai an einai adeio
  - ta pedia full kai empty toy struct dilwnoun an einai adeios i gematos o pinakas. Ta simata afta
  pou einai volatile int moiazoun stin leitourgia tous me tous simatoforous alla xrisimopoioun energi 
  anamoni. Arxikopoiw ton full = size kai ton empty = 0. O empty einai 0 epeidi arxika einai adeios o
  pinakas kai prepei na mplokarei to read, enw o full arxikopoieitai se size, epeidi arxika einai adeios
  kai kathe fora pou kanoume write ton meiwnoume kata ena, mexri na blokarei i write
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
  buff->full =size;//semaphore
  buff->empty=0;//semaphore
  
  cyclBuffer = (char *)malloc( size*sizeof(char) );
  if (cyclBuffer==NULL)  {
    printf("Error allocating cyclBuffer memory. Exiting\n");
    exit (-1);
  }

  return ;
}



/* pipe_write()

- Grafei enan xaraktira ston buffer. O sugxrotismos epitigxanetai

*/
void pipe_write(char c)  { 
  int nxt_wr_ptr = (buff->wrptr + 1)%buff->size;
  
  //allaksa apo nxt se curr
  while (buff->full == 0)  {
 
  } 
  buff->full--;
  //arxi krisimou tmimatos-grapse ston agwgo   
    
  cyclBuffer[buff->wrptr] = c;
  
  //telos krisimou tmimatos-auksise ton write-pointer
  buff->wrptr = nxt_wr_ptr;
  
  buff->empty++;
 
    
  return ;

}


int pipe_read(void)  {
  char c;  
  int bytesWrite;

  while (buff->empty == 0) {//if empty
    if (endOfFile == 1)
      return 0;    
  }
  buff->empty--;
  
  //arxi krisimou tmimatos
  
  c = cyclBuffer[buff->rdptr];
  
  //telos krisimou kwdika - auksise ton read-pointer 
  
  bytesWrite = write(fdout, &c, sizeof(char) );
  if (bytesWrite == -1 )  {
    printf("Write error at writer() function.Exiting\n");
    exit(-1); 
  }     

  buff->rdptr = (buff->rdptr+1)%buff->size;
  
  buff->full++;
  return 1;  
}




void pipe_close()  {
 
  free( cyclBuffer );

}


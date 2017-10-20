#include "mypipe.h"

/* struct buffinfo
 - size: to megethos tou pinaka
 - rdptr: i thesi anagnwsis ston pinaka
 - wrptr: i thesi eggrafis ston pinaka
 - full: energos simatoforos(oxi kanonikos), molis ginei miden mplokarei i write. 
 - empty: energos simatoforos(oxi kanonikos), molis ginei miden mplokarei i read
*/

#define WRITE 1
#define READ 0

#define FULL 3
#define EMPTY 4

struct buffinfo {
  int size;
  int rdptr;
  int wrptr;
  
  int full;
  int empty;
  volatile int modify_empty;
  volatile int modify_full;
  //volatile int turn_empty;
  //volatile int turn_full;
  volatile int empty_t[2];
  volatile int empty_taking[2];
  volatile int full_t[2];
  volatile int full_taking[2];
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

  buff->empty_taking[0] = 0; buff->empty_t[0] = 0;
  buff->empty_taking[1] = 0; buff->empty_t[1] = 0;

  buff->full_taking[0] = 0;  buff->full_t[0] = 0;
  buff->full_taking[1] = 0;  buff->full_t[1] = 0;

  //buff->modify_full = 1;
  //buff->modify_empty = 1;
  //buff->turn_empty = WRITE;
  //buff->turn_full = WRITE;

  cyclBuffer = (char *)malloc( size*sizeof(char) );
  if (cyclBuffer==NULL)  {
    printf("Error allocating cyclBuffer memory. Exiting\n");
    exit (-1);
  }

  return ;
}


int priority(int i, int type)  {
  if ( type == FULL )  {
    if (buff->full_t[i] == 0)
      return 0;
    if (buff->full_t[(i+1)%2]  < buff->full_t[i])
      return 0;
    else if (buff->full_t[(i+1)%2]  > buff->full_t[i])
      return 1;
    else return ((i+1)%2 > i);
   }    
   else {
     if (buff->empty_t[i] == 0)
      return 0;
     if (buff->empty_t[(i+1)%2]  < buff->empty_t[i])
      return 0;
     else if (buff->empty_t[(i+1)%2]  > buff->empty_t[i])
      return 1;
     else return ((i+1)%2 > i);
   }


  return 0;
}


/* pipe_write()

- Grafei enan xaraktira ston buffer. Tin kalei i thread_pipereader()
- To pws petuxainetai o sygxronismos tha to anaferoume ston psevdokwdika
*/
void pipe_write(char c)  {
  int tmp; 
  int nxt_wr_ptr = (buff->wrptr + 1)%buff->size;
  
  //allaksa apo nxt se curr
  while (buff->full == 0)  {
 
  } 


  //while (buff->modify_full == 0 ) {//|| buff->turn_full == READ) {

  //}
  //buff->modify_full = 0;


  buff->full_taking[WRITE] = 1;
  tmp = buff->full_t[READ];
  buff->full_t[WRITE] = tmp+1;//buff->full_t[READ] + 1;
  buff->full_taking[WRITE] = 0;
  while (buff->full_taking[READ] == 1)  { }
  while (priority(READ, FULL) )  { }
  buff->full--;
  buff->full_t[WRITE] = 0;

  //buff->turn_full = READ;
  //buff->modify_full = 1;
  //arxi krisimou tmimatos-grapse ston agwgo   
    
  cyclBuffer[buff->wrptr] = c;
  
  //telos krisimou tmimatos-auksise ton write-pointer
  buff->wrptr = nxt_wr_ptr;
  

  //while (buff->modify_empty == 0 ) {//|| buff->turn_empty == READ) {

  //}
  //buff->modify_empty = 0;

  buff->empty_taking[WRITE] = 1;
  tmp = buff->empty_t[READ];
  buff->empty_t[WRITE] = tmp+1;//buff->empty_t[READ] + 1;
  buff->empty_taking[WRITE] = 0;
  while (buff->empty_taking[READ] == 1)  { }
  while (priority(READ, EMPTY) )  { }
  buff->empty++;
  buff->empty_t[WRITE] = 0;
  //buff->turn_empty = READ;
  //buff->modify_empty = 1;
 
    
  return ;

}

/* pipe_read()
- Diavazei ena xaraktira apto buffer
- To pws epitigxanetai sigxronismos to anaferoume ston psevdokodika
- Tin kalei i thread_pipereader
*/
int pipe_read(void)  {
  int tmp;

  char c;  
  int bytesWrite;

  while (buff->empty == 0) {//if empty
    if (endOfFile == 1)
      return 0;    
  }



 
  //while (buff->modify_empty == 0 ) {//|| buff->turn_empty == WRITE) {
   
  //}
  //buff->modify_empty = 0;
  buff->empty_taking[READ] = 1;
  tmp = buff->empty_t[WRITE];
  buff->empty_t[READ] = tmp+1; //buff->empty_t[WRITE] + 1;
  buff->empty_taking[READ] = 0;
  while (buff->empty_taking[WRITE] == 1)  { }
  while (priority(WRITE, EMPTY) )  { }
  buff->empty--;
  buff->empty_t[READ] = 0;
  //buff->turn_empty = WRITE;
  //buff->modify_empty = 1;
  //arxi krisimou tmimatos
  
  c = cyclBuffer[buff->rdptr];
  
  //telos krisimou kwdika - auksise ton read-pointer 
  
  bytesWrite = write(fdout, &c, sizeof(char) );
  if (bytesWrite == -1 )  {
    printf("Write error at writer() function.Exiting\n");
    exit(-1); 
  }     

  buff->rdptr = (buff->rdptr+1)%buff->size;
  
  
  //while (buff->modify_full == 0) {// || buff->turn_full == WRITE) {

  //}
  //buff->modify_full = 0; 

  buff->full_taking[READ] = 1;
  tmp = buff->full_t[WRITE];
  buff->full_t[READ] = tmp+1;//buff->full_t[WRITE] + 1;
  buff->full_taking[READ] = 0;
  while (buff->full_taking[WRITE] == 1)  { }
  while (priority(WRITE, FULL) )  { }
  buff->full++;
  buff->full_t[READ] = 0;
  // buff->turn_full = WRITE;
  //buff->modify_full = 1;
  return 1;  
}



/*
kane free ton buffer
*/
void pipe_close()  {
 
  free( cyclBuffer );

}


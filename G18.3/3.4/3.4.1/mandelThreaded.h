#define TRUE 1
#define FALSE 0

#define CCR_EXTERN(label) \
          extern volatile int label##_mode; pthread_mutex_t label##_mtx; \
          pthread_cond_t label##_condvar;      

#define CCR_DECLARE(label) \
          volatile int label##_mode = FALSE; pthread_mutex_t label##_mtx; \
          pthread_cond_t label##_condvar;             
          
#define CCR_INIT(label) \
          pthread_mutex_init( &label##_mtx, NULL ); \
          //var = FALSE;
          

#define CCR_EXEC(label,cond,body) \
          pthread_mutex_lock( &label##_mtx); \
          while (cond == FALSE)  { \
            pthread_cond_wait( &label##_condvar, &label##_mtx); \
          } \
          body \
          pthread_mutex_unlock( &label##_mtx ); \
            

struct task {
  mandel_Pars *pars;//param
  int *res;//param
  int maxIterations;//param
  pthread_t tid;
  pthread_mutex_t  sem;
  pthread_mutex_t mutex;
  pthread_cond_t cond;
  volatile int condVal;
   
};
typedef struct task taskT;

extern int init_threads(int numofslices) ;

extern void *waitUntilGetTask(void *numOfThread);

extern int find_next_finished_thread(int numOfThreads);

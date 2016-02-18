#include <pthread.h>
#include <semaphore.h>

#define N_CONSUMERS 8
#define N_QUEUE 1024

/*

Array of work, FILO first (FIFO queue later)
-add work => fetch_and_increase counter, CAS with nullptr, will spinlock if
-remove work =>  (fetch_and_decrease counter, CAS with nullptr) <= adder can overwrite if not atomic!.

*/

/*
 Array of work
 [...| HEAD->W0 W1 W2 W3<-TAIL | ....]
 [head|tail] = 1 int.
 -add work =>  fetch_and_increase tail, write to tail
 -remove work => fetch_and_increase head, check that head!=tail (empty queue) -> sleep, execute fetched value
*/

/*
 multi-executer:
  given array of work-items & executers, semaphore locked
  wake "all" by unlocking semaphore.
  fetch work from queue using atomics untill "clean".
  then wait on semaphore again.
  use another semaphore to block the main thread. Counting up
*/

typedef struct{
  sem_t main_sleeper;
  sem_t worker_sleeper;

  pthread_t consumers[N_CONSUMERS];
  unsigned work_items;
  void (*queue[N_QUEUE])(void *arg);
  void* args[N_QUEUE];

} workQ_t;

void* wait_for_message(void *arg){
  workQ_t* queue = arg;
  pthread_cond_wait(&queue->cond, )
}

void initializeQ(workQ_t *q){
  q->cond = PTHREAD_COND_INITIALIZER;
  for(unsigned u = 0; u < N_CONSUMERS; u++){
    pthread_create(q->consumers+u, NULL, &wait_for_message, q);
  }
  work_items = 0;
}

void add_work_item(workQ_t *q, void (*work)(void*), void *arg){
  q->queue[work_items] = work;
  q->args[work_items] = args;
  work_items++;
}

void execute

void (*GLOBAL_Q[N_QUEUE])(void * arg);

int main(int argc, char** argv){
  workQ_t Q;
  initializeQ(&Q);
  for(unsigned u = 0; u < N_CONSUMERS; u++)
    pthread_create(consumers+u, NULL, &wait_for_message, &Q);
  return 0;
}

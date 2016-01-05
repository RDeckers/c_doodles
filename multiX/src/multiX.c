#include <multiX.h>

#include <unistd.h> //for the number of cpus
#include <stdlib.h>

void* wait_for_message(void *arg){
  workQ_t *q = arg;
  while(1){
    pthread_mutex_lock(&q->lock);//Can only enter after producer is waiting
    if(q->n_consumers == __sync_add_and_fetch(&q->waiting_consumers, 1)){
      pthread_cond_signal(&q->all_threads_sleeping);//Producer can now continue *after* we wait and release the lock.
    }
    pthread_cond_wait(&q->execute_work, &q->lock);//waiting to be told to execute the q.
    pthread_mutex_unlock(&q->lock);//immidiatly release the lock

    int id;
    while(0 <= (id = __sync_fetch_and_sub(&q->last_id, 1))){//while there is work, fetch it atomically
      work_item_t work = q->queue[id];
      (work.func)(work.args);
    }
  }
}

void initializeQ(workQ_t *q, size_t max_queue_size){
  q->n_consumers = sysconf(_SC_NPROCESSORS_ONLN); //TODO: error checking
  q->execute_work = (pthread_cond_t) PTHREAD_COND_INITIALIZER;
  q->all_threads_sleeping = (pthread_cond_t) PTHREAD_COND_INITIALIZER;
  q->lock = (pthread_mutex_t) PTHREAD_MUTEX_INITIALIZER;
  q->consumers = malloc(sizeof(pthread_t)*q->n_consumers);
  q->last_id = -1;
  q->waiting_consumers = 0;
  q->queue = malloc(sizeof(work_item_t)*max_queue_size);

  pthread_mutex_lock(&q->lock);//so all threads will wait for us to finish setting up
  for(int i = 0; i < q->n_consumers; i++){
    pthread_create(q->consumers+i, NULL, &wait_for_message, q);
  }
  pthread_cond_wait(&q->all_threads_sleeping, &q->lock);//We are done setting up and want to wait for all threads to finish.
  //we have the lock.
}

void add_work_item(workQ_t *q, void (*work_function)(void *), void* args){
  work_item_t work_item = {.func = work_function, .args = args};
  q->queue[++q->last_id] = work_item;//add new item, not thread safe.
}

void finish(workQ_t *q){
  q->waiting_consumers = 0;
  pthread_cond_broadcast(&q->execute_work);
  pthread_cond_wait(&q->all_threads_sleeping, &q->lock);
  q->last_id = -1;
}

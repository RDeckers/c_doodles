#ifndef MULTIX_H
#define MULTIX_H

#include <pthread.h>

typedef struct{
  void (*func)(void *arg);
  void* args;
}work_item_t;

typedef struct{
  pthread_cond_t execute_work;
  pthread_cond_t all_threads_sleeping;
  int all_threads_sleeping_flag;
  pthread_mutex_t lock;

  pthread_t *consumers;
  int n_consumers;
  int waiting_consumers;

  int last_id;
  work_item_t* queue;
} workQ_t;

//void* wait_for_message(void *arg);
void initializeQ(workQ_t *q, size_t max_queue_size);
void add_work_item(workQ_t *q, void (*work_function)(void *), void* args);
void finish(workQ_t *q);

#endif

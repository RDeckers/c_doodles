#ifndef MULTIX_H
#define MULTIX_H

#include <hwloc.h>
#include <pthread.h>

#define WORK_Q_INTERNAL_Q_MAX_INDEX (1 << 8)-1

struct work_item_dq;

typedef struct{
  void (*func)(struct work_item_dq* parent, void *args);
  void* args;
}work_item_t;

typedef struct{
  unsigned log_capacity;
  work_item_t array[];
}sized_work_item_array_t;

typedef struct work_item_dq{
  int top;
  volatile int bottom;
  sized_work_item_array_t* volatile storage;
}work_item_dq_t;

typedef struct{
  pthread_cond_t execute_work;
  pthread_cond_t all_threads_sleeping;
  int all_threads_sleeping_flag;
  pthread_mutex_t lock;

  pthread_t *consumers;
  work_item_dq_t **queues;
  int n_consumers;
  volatile int active_consumers;
  volatile int sleeping_consumers;

} workQ_t;

typedef struct{
  workQ_t *q;
  int id;
}Q_initializer_t;

typedef enum{
  BIND_TO_LOGICAL_PROCESSOR = (1 << 0)
} workQ_flags;

//void* wait_for_message(void *arg);
//void initializeQ(workQ_t *q, size_t max_queue_size);
workQ_t* new_workQ(workQ_flags flags);

int add_work_item(workQ_t *q, void (*work_function)(work_item_dq_t*, void *), void* args);
int add_execute_work_item(workQ_t *q, void (*work_function)(work_item_dq_t*, void *), void* args);
int add_work_item_to_dq(work_item_dq_t *dq, void (*work_function)(work_item_dq_t*, void *), void* args);

void finish(workQ_t *q);

work_item_dq_t* new_work_item_dq(unsigned log_capacity);
void resize_work_item_dq(work_item_dq_t *dq, int bottom, int top);
int is_empty_work_item_dq(work_item_dq_t *dq);
void push_bottom_work_item_dq(work_item_dq_t *dq, work_item_t task);
work_item_t pop_top_work_item_dq(work_item_dq_t *dq);
work_item_t pop_bottom_work_item_dq(work_item_dq_t *dq);

#endif

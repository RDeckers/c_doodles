#include <containers/atomic_dequeue.h>

a_dequeue_t* a_dequeue_new(int log2_capacity);{
  a_dequeue_t* new_dequeue = malloc(sizeof(a_dequeue_t) + sizeof(void*)*(1 << log2_capacity));
  new_dequeue->log2_capacity = log2_capacity;
  return new_dequeue;
}

void a_dequeue_resize(a_dequeue_t *adq){
  
}

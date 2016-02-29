#include <multiX.h>
#include <utilities/logging.h>
//#include <sched.h> //for sched_getcpu() debugging
#include <unistd.h> //for the number of cpus
#include <stdlib.h>

work_item_dq_t* new_work_item_dq(unsigned log_capacity){
  unsigned  capacity = 1 << log_capacity;
  work_item_dq_t *new_dq = malloc(sizeof(work_item_dq_t));
  ////report(INFO, "allocationg %u bytes for a dq", sizeof(work_item_dq_t));
  new_dq->top = 0;
  new_dq->bottom = 0;
  ////report(INFO, "allocationg %u bytes for storage", sizeof(sized_work_item_array_t)+sizeof(work_item_t)*capacity);
  sized_work_item_array_t *storage = malloc(sizeof(sized_work_item_array_t)+sizeof(work_item_t)*capacity);
  storage->log_capacity = log_capacity;
  new_dq->storage = storage;
  return new_dq;
}

void resize_work_item_dq(work_item_dq_t *dq, int bottom, int top){
  //report(WARN, "resizing %p", dq);
  unsigned new_log_capacity = dq->storage->log_capacity+1;
  unsigned new_capacity = 1 << new_log_capacity;
  unsigned old_capacity = 1 << dq->storage->log_capacity;
  sized_work_item_array_t *storage = malloc(sizeof(sized_work_item_array_t)+sizeof(work_item_t)*new_capacity);
  storage->log_capacity = new_log_capacity;
  //report(INFO, "starting to copy old elements");
  for(int i = top; i < bottom; i++){
    //report(INFO, "copying %d", i);
    storage->array[i % new_capacity] = dq->storage->array[i % old_capacity];
  }
  //report(PASS, "copied all old elements");
  sized_work_item_array_t *old_storage = dq->storage;
  dq->storage = storage;
  //report(INFO, "swapped storage %p for %p", old_storage, storage);
  free(old_storage); //TODO: can this ever double free?
}

int is_empty_work_item_dq(work_item_dq_t *dq){
  int local_top = dq->top;
  int local_bottom = dq->bottom;
  return local_bottom <= local_top;
}

void push_bottom_work_item_dq(work_item_dq_t *dq, work_item_t task){
  ////report(INFO, "Pushing [%p, %p] to %p", task.func, task.args, dq );
  int old_bottom = dq->bottom;
  int old_top = dq->top;
  int size = old_bottom - old_top;
  if(size >= (1 << dq->storage->log_capacity) ){
    //report(WARN, "Resize required");
    resize_work_item_dq(dq, old_bottom, old_top);
  }
  dq->storage->array[(old_bottom) % (1 << dq->storage->log_capacity)] = task;
  dq->bottom = old_bottom+1;
  ////report(PASS, "succesfully pushed!");
}

work_item_t pop_top_work_item_dq(work_item_dq_t *dq){
  int old_top = dq->top;
  int new_top = old_top + 1;
  int old_bottom = dq->bottom;
  int size = old_bottom-old_top;
  if(size <= 0){
    return (work_item_t){.func = NULL, .args = NULL};
  }
  work_item_t task = dq->storage->array[old_top % (1 << dq->storage->log_capacity)];
  if(__sync_bool_compare_and_swap(&dq->top, old_top, new_top)){
    return task;
  }
  return (work_item_t){.func = NULL, .args = NULL};
}

work_item_t pop_bottom_work_item_dq(work_item_dq_t *dq){
  dq->bottom--;
  int old_top = dq->top;
  int new_top = old_top + 1;
  int size = dq->bottom - old_top;
  if(size < 0){
    dq->bottom = old_top;
    return (work_item_t){.func = NULL, .args = NULL};
  }
  work_item_t task = dq->storage->array[dq->bottom % (1 << dq->storage->log_capacity)];
  if(size > 0){
    return task;
  }
  if(!__sync_bool_compare_and_swap(&dq->top, old_top, new_top)){
    task = (work_item_t){.func = NULL, .args = NULL};
  }
  dq->bottom = old_top + 1;
  return task;
}

void* wait_for_message(void *arg){
  Q_initializer_t *initializer = arg;
  workQ_t *q = initializer->q;
  int id = initializer->id;
  report(INFO, "Created thread %d", id);
  ////report(INFO, "Created thread & queue %d", id);
  work_item_t work = pop_bottom_work_item_dq(q->queues[id]);
  while(1){
    //__sync_add_and_fetch(&q->active_consumers, 1);
    ///*if(id == 1)*/ //report(INFO, "main loop");
    while(NULL != work.func){
      /*if(id == 1)*/ //report(INFO, "executing work");
      //report(FAIL, "calling %p(%d, %p)", work.func, id, work.args);
      (work.func)(q->queues[id], work.args);
      work = pop_bottom_work_item_dq(q->queues[id]);
    }
    __sync_fetch_and_sub(&q->active_consumers, 1);
    while(NULL == work.func){
      ///*if(id == 1)*/ //report(INFO, "trying to steal work");
      int victim = rand() % (q->n_consumers);
      if(!is_empty_work_item_dq(q->queues[1+victim])){
        /*if(id == 1)*/ //report(PASS, "found work");
        __sync_add_and_fetch(&q->active_consumers, 1);
        work = pop_top_work_item_dq(q->queues[1+victim]);
        if( NULL == work.func){
          /*if(id == 1)*/ //report(FAIL, "failed to steal work");
          __sync_fetch_and_sub(&q->active_consumers, 1);
        }
      }
      if(0 == q->active_consumers){
        ///*/*if(id == 1)*/ //report(INFO, "All consumers inactive, checking the main queue");
        if(!is_empty_work_item_dq(q->queues[0])){
          /*if(id == 1)*/ //report(PASS, "found work in the main queue %p", q->queues[0]->storage);
          __sync_add_and_fetch(&q->active_consumers, 1);
          work = pop_top_work_item_dq(q->queues[0]);
          if( NULL == work.func){
            /*if(id == 1)*/ //report(FAIL, "failed to steal from main queue");
            __sync_fetch_and_sub(&q->active_consumers, 1);
          }
        }
        else{//all threads sleeping, main thread empty
          pthread_mutex_lock(&q->lock);
          //report(INFO, "thread %d going to sleep", id);
          if(q->n_consumers == ++q->sleeping_consumers){
            //report(INFO, "thread %d last to sleep, signalling", id);
            pthread_cond_signal(&q->all_threads_sleeping);
          }
          pthread_cond_wait(&q->execute_work, &q->lock);
          --q->sleeping_consumers;
          pthread_mutex_unlock(&q->lock);
          //report(PASS, "thread %d woke up", id);
        }
      }
    }
  }
}

workQ_t* new_workQ(workQ_flags flags){
  workQ_t *q = malloc(sizeof(workQ_t));
  hwloc_obj_type_t binding_target = HWLOC_OBJ_CORE;
  if(flags & BIND_TO_LOGICAL_PROCESSOR){
    binding_target = HWLOC_OBJ_PU;
  }
  hwloc_topology_t topology;
  // Allocate, initialize, and perform topology detection
  hwloc_topology_init(&topology);//TODO: can this cause issues with multiple queues?
  hwloc_topology_load(topology);

  q->n_consumers = hwloc_get_nbobjs_by_type(topology, binding_target); //TODO: error checking
  //printf("Making a Q with %d consumers.\n", q->n_consumers);

  q->execute_work = (pthread_cond_t) PTHREAD_COND_INITIALIZER;
  q->all_threads_sleeping = (pthread_cond_t) PTHREAD_COND_INITIALIZER;
  q->lock = (pthread_mutex_t) PTHREAD_MUTEX_INITIALIZER;
  q->consumers = malloc(sizeof(pthread_t)*q->n_consumers);
  q->queues = malloc(sizeof(typeof(*q->queues))*(q->n_consumers+1));
  for(int i = 0; i < q->n_consumers+1; i++){
    q->queues[i] = new_work_item_dq(10); //TODO: segfault if < 2
  }
  q->active_consumers = q->n_consumers;
  q->sleeping_consumers = 0;


  pthread_mutex_lock(&q->lock);//so all threads will wait for us to finish setting up
  Q_initializer_t args[q->n_consumers];
  for(int i = 0; i < q->n_consumers; i++){
    args[i].id = i+1;
    args[i].q = q;
    pthread_create(q->consumers+i, NULL, &wait_for_message, &args[i]);
    hwloc_obj_t binding_object = hwloc_get_obj_by_type(topology, binding_target, i);
    hwloc_set_thread_cpubind(topology, q->consumers[i], binding_object->cpuset, 0);
  }
  pthread_cond_wait(&q->all_threads_sleeping, &q->lock);//We are done setting up and want to wait for all threads to finish.
  pthread_mutex_unlock(&q->lock);
  return q;
}

/*int add_execute_work_item(workQ_t *q, void (*work_function)(void *), void* args){
  if(!add_work_item(q, work_function, args)){
    return 0;
  }
  pthread_cond_signal(&q->execute_work);
  return 1;
}*/

int add_work_item_to_dq(work_item_dq_t *dq, void (*work_function)(work_item_dq_t*, void *), void* args){
  work_item_t work_item = {.func = work_function, .args = args};
  push_bottom_work_item_dq(dq, work_item);
}

int add_work_item(workQ_t *q, void (*work_function)(work_item_dq_t* parent, void *), void* args){
  work_item_t work_item = {.func = work_function, .args = args};
  add_work_item_to_dq(q->queues[0], work_function, args);
}

void finish(workQ_t *q){
  //report(INFO, "trying to grab lock");
  pthread_mutex_lock(&q->lock);
  //report(PASS, "grabbed lock");
  pthread_cond_broadcast(&q->execute_work);
  //report(INFO, "waiting on all threads to go to bed again...");
  pthread_cond_wait(&q->all_threads_sleeping, &q->lock);
  pthread_mutex_unlock(&q->lock);
  //report(PASS, "done!");
}

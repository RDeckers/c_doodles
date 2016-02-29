#include <containers/atomic_stack.h>

a_stack_t* a_stack_new(int log2_capacity){
  a_stack_t *new_stack = malloc(sizeof(a_stack_t));
  if(NULL == new_stack){
    return new_stack;
  }
  new_stack->base = malloc(sizeof(int)*(1<<log2_capacity));
  if(NULL == new_stack->base){
    free(new_stack);
    return NULL;
  }
  new_stack->log2_capacity = log2_capacity;
  new_stack->head_index = -1;
}

void a_stack_push(a_stack_t *stack, int item){
  int current_capacity = 1 << stack->log2_capacity;
}

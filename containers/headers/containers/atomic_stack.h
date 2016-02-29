typedef struct{
  int log2_capacity;
  int head_index;
  int *base;
}a_stack_t;

a_stack_t* a_stack_new(int log2_capacity);

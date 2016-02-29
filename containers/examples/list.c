#include <containers/primitive_lists.h>
#include <stdio.h>

int main(int argc, char **argv){
  listi_t *int_list = listCreatei();
  listd_t *double_list = listCreated(0);
  for(int i = 1; i <= 10; i++){
    printf("appending %d\n", i);
    listAppendi(int_list, i+1);
    listAppendd(double_list, i*i);
  }
  listNodei_t *currentNode_i = int_list->head;
  while(NULL != currentNode_i->next){
    printf("%d\n",currentNode_i->data);
    currentNode_i = currentNode_i->next;
  }
  listNoded_t *currentNode_d = double_list->tail;
  while(NULL != currentNode_d->next){
     printf("%f\n",currentNode_d->data);
     currentNode_d = currentNode_d->next;
  }
  return 0;
}

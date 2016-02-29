#include <stdlib.h>


#define DEFINE_LIST(SUFFIX, TYPE)\
typedef struct listNode##SUFFIX{TYPE data;struct listNode ## SUFFIX *next;}listNode##SUFFIX##_t;\
typedef struct {listNode##SUFFIX##_t *head, *tail;} list##SUFFIX##_t;\
int listAppend##SUFFIX(list##SUFFIX##_t *list, TYPE element);\
list##SUFFIX##_t* listCreate##SUFFIX()

//Double linked list.
#define DEFINE_DLIST(SUFFIX, TYPE)\
  typedef struct dlistNode##SUFFIX{TYPE data;struct dlistNode ## SUFFIX *next, *prev;}dlistNode##SUFFIX##_t;\
  typedef struct {dlistNode##SUFFIX##_t *head, *tail;} dlist##SUFFIX##_t;\
  int dlistAppend##SUFFIX(dlist##SUFFIX##_t *list, TYPE element);\
  dlist##SUFFIX##_t* dlistCreate##SUFFIX(TYPE firstElement);

#define IMPLEMENT_LIST(SUFFIX, TYPE )/*SUFFIX can also be left empty, this will create list_t and listAppend like things*/\
 int listAppend##SUFFIX(list##SUFFIX##_t *list, TYPE element){\
   listNode##SUFFIX##_t *new_tail = malloc(sizeof(listNode##SUFFIX##_t));\
   if(NULL == new_tail)\
    return -1;\
   if(NULL != list->tail){\
    list->tail->next = new_tail;\
   }\
   else{\
     list->head = new_tail;\
   }\
   list->tail = new_tail;\
   new_tail->next = NULL;\
   new_tail->data = element;\
   return 0;\
 }\
 list##SUFFIX##_t* listCreate##SUFFIX(){\
   list##SUFFIX##_t* listp = malloc(sizeof(list##SUFFIX##_t));\
   if(listp == NULL) return NULL;\
   listp->head = NULL;\
   listp->tail = NULL;\
   return listp;\
 }

#define IMPLEMENT_DLIST(SUFFIX, TYPE)\
  int dlistAppend##SUFFIX(dlist##SUFFIX##_t *list, TYPE element){\
   list->tail->next = malloc(sizeof(dlistNode##SUFFIX##_t));\
   if(list->tail->next == NULL)\
    return -1;\
   /*list->size++;*/\
   list->tail->next->prev = list->tail;\
   list->tail = list->tail->next;\
   list->tail->data = element;\
   list->tail->next = NULL;\
   return 0;\
 }\
 dlist##SUFFIX##_t* dlistCreate##SUFFIX(TYPE firstElement){\
   dlist##SUFFIX##_t* listp = malloc(sizeof(dlist##SUFFIX##_t));\
   if(listp == NULL) return NULL;\
   listp->head = malloc(sizeof(dlistNode##SUFFIX##_t));\
   if(listp->head == NULL) return NULL;\
   listp->tail = listp->head;\
   listp->tail->next = NULL;\
   listp->tail->prev = NULL;\
   listp->head->data = firstElement;\
   return listp;\
 }

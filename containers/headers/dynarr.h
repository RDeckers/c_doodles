#include <stdlib.h>
#include <string.h>

#define DEFINE_DYNARR(SUFFIX, TYPE)\
  typedef struct{TYPE *base; size_t count; size_t size;}dynarr##SUFFIX##_t;\
  dynarr##SUFFIX##_t* createDynarr##SUFFIX(unsigned Size){\
    Size  = 0 == Size? 128 : Size;\
    dynarr##SUFFIX##_t* ret = malloc(sizeof(dynarr##SUFFIX##_t));\
    if(!ret) return ret;\
    ret->base = malloc(sizeof(TYPE)*Size);\
    if(!ret->base){\
      free(ret);\
      return NULL;\
    }\
    ret->count = 0;\
    ret->size = Size;\
	return ret;\
  }\
\
  extern inline void forcePushDynarr##SUFFIX(dynarr##SUFFIX##_t *dynarr, TYPE x){\
    *((dynarr->base)+(dynarr->count++)) = x;\
  }\
\
  extern inline void forcePushArrayDynarr##SUFFIX(dynarr##SUFFIX##_t *dynarr, TYPE *array, size_t n){\
    memmove(dynarr->base+(dynarr->count), array, sizeof(TYPE)*n);\
    dynarr->count += n;\
  }\
  \
  int pushDynarr##SUFFIX(dynarr##SUFFIX##_t *dynarr, TYPE x){\
    if(dynarr->count >= dynarr->size){\
      TYPE *new_base = realloc(dynarr->base, sizeof(TYPE)*dynarr->size*2);\
      if(NULL == new_base){return - 1;}\
      else{dynarr->base = new_base; dynarr->size *= 2;}\
    }\
    forcePushDynarr##SUFFIX(dynarr, x);\
    return 0;\
  }\
  int pushArrayDynarr##SUFFIX(dynarr##SUFFIX##_t *dynarr, TYPE *array, size_t n){\
    if(dynarr->count + n >= dynarr->size){\
      TYPE *new_base = realloc(dynarr->base, sizeof(TYPE)*(dynarr->size+n));\
      if(NULL == new_base){return - 1;}\
      else{dynarr->base = new_base; dynarr->size = dynarr->size+n;}\
    }\
    forcePushArrayDynarr##SUFFIX(dynarr, array, n);\
    return 0;\
  }\
\
  extern inline TYPE forcePopDynarr##SUFFIX(dynarr##SUFFIX##_t *dynarr){\
    return *(dynarr->base+(--(dynarr->count)));\
  }\
\
  int PopDynarr##SUFFIX(dynarr##SUFFIX##_t *dynarr, TYPE *y){\
    if(dynarr->count == 0) /*will cause issues if you pop an empty dynarr.*/\
      return -1;\
    *y = forcePopDynarr##SUFFIX(dynarr);\
    return 0;\
  }\
  int shrinkToFitDynarr##SUFFIX(dynarr##SUFFIX##_t *dynarr){\
    if(dynarr->count != dynarr->size){\
      TYPE *new_base = realloc(dynarr->base, sizeof(TYPE)*dynarr->count);\
      if(NULL == new_base){return - 1;}\
      else{dynarr->base = new_base; dynarr->size = dynarr->count;}\
    }\
    return 0;\
  }

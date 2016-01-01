#include "../headers/dynarr.h"
#include <stdio.h>
#include <stdint.h>

DEFINE_STACK(f, float);
DEFINE_STACK(u64, uint64_t);

int main(int argc, char **argv){
  dynarrf_t *float_dynarr = createDynarrf(256);
  dynarru64_t *u64_dynarr = createDynarru64(5);

  for(unsigned u = 1; u < 10; u++){
    pushDynarru64(u64_dynarr, u);
    printf("After pushing %u, dynarr now contains %u/%u items @ %p\n", u, u64_dynarr->count, u64_dynarr->size, u64_dynarr->base);
  }
  printf("dynarr now contains %u/%u items @ %p\n", u64_dynarr->count, u64_dynarr->size, u64_dynarr->base);
  pushArrayDynarru64(u64_dynarr, u64_dynarr->base, u64_dynarr->count);
  printf("dynarr now contains %u/%u items @ %p\n", u64_dynarr->count, u64_dynarr->size, u64_dynarr->base);
  uint64_t sum = 0, tmp;
  while(!PopDynarru64(u64_dynarr, &tmp)){
    sum += tmp;
  }
  printf("sum = %u, should be %u\n", sum, (9*10));

  printf("dynarr now contains %u/%u items @ %p\n", float_dynarr->count, float_dynarr->size, float_dynarr->base);
  for(float f = 1; f <= 1234567890; f *= 1.01){
    pushDynarrf(float_dynarr, f);
  }
  printf("dynarr now contains %u/%u items @ %p\n", float_dynarr->count, float_dynarr->size, float_dynarr->base);
  shrinkToFitDynarrf(float_dynarr);
  printf("dynarr now contains %u/%u items @ %p\n", float_dynarr->count, float_dynarr->size, float_dynarr->base);

}

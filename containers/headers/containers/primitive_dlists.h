#include <containers/generic_lists_def.h>
#include <stdint.h>

//signed integers
DEFINE_DLIST(i, int);
DEFINE_DLIST(l, long);
DEFINE_DLIST(i8, int8_t);
DEFINE_DLIST(i16, int16_t);
DEFINE_DLIST(i32, int32_t);
DEFINE_DLIST(i64, int64_t);

//unsigned integers
DEFINE_DLIST(u, unsigned);
DEFINE_DLIST(ul, unsigned long);
DEFINE_DLIST(u8, uint8_t);
DEFINE_DLIST(u16, uint16_t);
DEFINE_DLIST(u32, uint32_t);
DEFINE_DLIST(u64, uint64_t);

//pointers
DEFINE_DLIST(p, void*);

//floating types
DEFINE_DLIST(f, float);
DEFINE_DLIST(d, double);

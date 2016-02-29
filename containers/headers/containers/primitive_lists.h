#include <containers/generic_lists_def.h>
#include <stdint.h>

//signed integers
DEFINE_LIST(i, int);
DEFINE_LIST(l, long);
DEFINE_LIST(i8, int8_t);
DEFINE_LIST(i16, int16_t);
DEFINE_LIST(i32, int32_t);
DEFINE_LIST(i64, int64_t);

//unsigned integers
DEFINE_LIST(u, unsigned);
DEFINE_LIST(ul, unsigned long);
DEFINE_LIST(u8, uint8_t);
DEFINE_LIST(u16, uint16_t);
DEFINE_LIST(u32, uint32_t);
DEFINE_LIST(u64, uint64_t);

//pointers
DEFINE_LIST(p, void*);

//floating types
DEFINE_LIST(f, float);
DEFINE_LIST(d, double);

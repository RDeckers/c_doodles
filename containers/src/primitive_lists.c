#include <containers/primitive_lists.h>

//signed integers
IMPLEMENT_LIST(i, int);
IMPLEMENT_LIST(l, long);
IMPLEMENT_LIST(i8, int8_t);
IMPLEMENT_LIST(i16, int16_t);
IMPLEMENT_LIST(i32, int32_t);
IMPLEMENT_LIST(i64, int64_t);

//unsigned integers
IMPLEMENT_LIST(u, unsigned);
IMPLEMENT_LIST(ul, unsigned long);
IMPLEMENT_LIST(u8, uint8_t);
IMPLEMENT_LIST(u16, uint16_t);
IMPLEMENT_LIST(u32, uint32_t);
IMPLEMENT_LIST(u64, uint64_t);

//pointers
IMPLEMENT_LIST(p, void*);

//floating types
IMPLEMENT_LIST(f, float);
IMPLEMENT_LIST(d, double);

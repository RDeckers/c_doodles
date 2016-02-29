#include <containers/primitive_dlists.h>

//signed integers
IMPLEMENT_DLIST(i, int);
IMPLEMENT_DLIST(l, long);
IMPLEMENT_DLIST(i8, int8_t);
IMPLEMENT_DLIST(i16, int16_t);
IMPLEMENT_DLIST(i32, int32_t);
IMPLEMENT_DLIST(i64, int64_t);

//unsigned integers
IMPLEMENT_DLIST(u, unsigned);
IMPLEMENT_DLIST(ul, unsigned long);
IMPLEMENT_DLIST(u8, uint8_t);
IMPLEMENT_DLIST(u16, uint16_t);
IMPLEMENT_DLIST(u32, uint32_t);
IMPLEMENT_DLIST(u64, uint64_t);

//pointers
IMPLEMENT_DLIST(p, void*);

//floating types
IMPLEMENT_DLIST(f, float);
IMPLEMENT_DLIST(d, double);

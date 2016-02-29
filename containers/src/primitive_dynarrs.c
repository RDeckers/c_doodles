#include <containers/primitive_dynarrs.h>

//signed integers
IMPLEMENT_DYNARR(i, int);
IMPLEMENT_DYNARR(l, long);
IMPLEMENT_DYNARR(i8, int8_t);
IMPLEMENT_DYNARR(i16, int16_t);
IMPLEMENT_DYNARR(i32, int32_t);
IMPLEMENT_DYNARR(i64, int64_t);

//unsigned integers
IMPLEMENT_DYNARR(u, unsigned);
IMPLEMENT_DYNARR(ul, unsigned long);
IMPLEMENT_DYNARR(u8, uint8_t);
IMPLEMENT_DYNARR(u16, uint16_t);
IMPLEMENT_DYNARR(u32, uint32_t);
IMPLEMENT_DYNARR(u64, uint64_t);

//pointers
IMPLEMENT_DYNARR(p, void*);

//floating types
IMPLEMENT_DYNARR(f, float);
IMPLEMENT_DYNARR(d, double);

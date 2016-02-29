#include <containers/generic_dynarr_def.h>
#include <stdint.h>

//signed integers
DEFINE_DYNARR(i, int);
DEFINE_DYNARR(l, long);
DEFINE_DYNARR(i8, int8_t);
DEFINE_DYNARR(i16, int16_t);
DEFINE_DYNARR(i32, int32_t);
DEFINE_DYNARR(i64, int64_t);

//unsigned integers
DEFINE_DYNARR(u, unsigned);
DEFINE_DYNARR(ul, unsigned long);
DEFINE_DYNARR(u8, uint8_t);
DEFINE_DYNARR(u16, uint16_t);
DEFINE_DYNARR(u32, uint32_t);
DEFINE_DYNARR(u64, uint64_t);

//pointers
DEFINE_DYNARR(p, void*);

//floating types
DEFINE_DYNARR(f, float);
DEFINE_DYNARR(d, double);

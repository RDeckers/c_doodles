#include <stdint.h>
#include <immintrin.h>

typedef int8_t v32c __attribute__ ((vector_size (32)));
typedef int8_t v16c __attribute__ ((vector_size (16)));
typedef int8_t v8c __attribute__ ((vector_size (8)));

typedef uint8_t v32uc __attribute__ ((vector_size (32)));
typedef uint8_t v16uc __attribute__ ((vector_size (16)));
typedef uint8_t v8uc __attribute__ ((vector_size (8)));

typedef int32_t v8i __attribute__ ((vector_size (32)));
typedef int32_t v4i __attribute__ ((vector_size (16)));
typedef int32_t v2i __attribute__ ((vector_size (8)));

typedef uint32_t v8ui __attribute__ ((vector_size (32)));
typedef uint32_t v4ui __attribute__ ((vector_size (16)));
typedef uint32_t v2ui __attribute__ ((vector_size (8)));

typedef float v8f __attribute__ ((vector_size (32)));
typedef float v4f __attribute__ ((vector_size (16)));
typedef float v2f __attribute__ ((vector_size (8)));

typedef double v4d __attribute__ ((vector_size (32)));
typedef double v2d __attribute__ ((vector_size (16)));

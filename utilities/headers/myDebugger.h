#ifdef _MY_DEBUG
#include <immintrin.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>

#if defined(__i386__)
static __inline__ uint32_t rdtsc(void)
{
    uint32_t x;
    __asm__ volatile (".byte 0x0f, 0x31" : "=A" (x));
    return x;
}

#elif defined(__x86_64__)

static __inline__ uint64_t rdtsc(void)
{
    uint32_t hi, lo;
    __asm__ __volatile__ ("rdtsc" : "=a"(lo), "=d"(hi));
    return ( (uint64_t)lo)|( ((uint64_t)hi)<<32 );
}

#endif


#define timeCall(x,t) t =rdtsc();\
x;\
t = rdtsc()-t

#ifdef _REPORT_FUNCTION_NAME
#define _FN fprintf(stderr, "\033[1;37m[%s]", __PRETTY_FUNCTION__)
#else
#define _FN do{}while(0)
#endif

#ifdef _REPORT_LINE_NR
#define _LN fprintf(stderr, "(%d)", __LINE__)
#else
#define _LN do{}while(0)
#endif

#ifdef _REPORT_TS
#include <time.h>
#define _TS fprintf(stderr, "\033[1;37m[%7.4f]",((float)clock())/CLOCKS_PER_SEC)
#else
#define _TS do { } while(0)
#endif

#define _REPORT_PREFIX _TS;_FN;_LN
#define reportInfo(FORMAT, ...)  _REPORT_PREFIX;fprintf(stderr, "\033[1;34m[INFO]\033[0m" FORMAT "%s", ##__VA_ARGS__, "\n");
#define reportWarn(FORMAT, ...)  _REPORT_PREFIX;fprintf(stderr, "\033[0;31m[WARN]\033[0m" FORMAT "%s", ##__VA_ARGS__, "\n");
#define reportPass(FORMAT, ...)  _REPORT_PREFIX;fprintf(stderr, "\033[1;32m[PASS]\033[0m" FORMAT "%s", ##__VA_ARGS__, "\n");
#define reportFail(FORMAT, ...)  _REPORT_PREFIX;fprintf(stderr, "\033[1;31m[FAIL]\033[0m" FORMAT "%s", ##__VA_ARGS__, "\n");

#else
#define timeCall(x,t) x
#define reportInfo(FORMAT, ...) do { } while(0)
#define reportWarn(FORMAT, ...) do { } while(0)
#define reportPass(FORMAT, ...) do { } while(0)
#define reportFail(FORMAT, ...) do { } while(0)
#endif

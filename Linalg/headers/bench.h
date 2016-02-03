#ifndef _MY_BENCH_H
#define _MY_BENCH_H
static unsigned long long rdtscp(void)
{
    unsigned long long tsc;
    __asm__ __volatile__(
        "rdtscp;"
        "shl $32, %%rdx;"
        "or %%rdx, %%rax"
        : "=a"(tsc)
        :
        : "%rcx", "%rdx");

    return tsc;
}


#define TIME_CALL(x,t) t =rdtscp();\
x;\
t = rdtscp()-t
#endif

#include <time.h>

//returns the time passed between start and end in ns.
double time_diff(struct timespec *start, struct timespec *end);
//returns the time in ns elapsed since T.
double elapsed_since(struct timespec *T);
//gettime wrapper
void tick(struct timespec *T);
//updates T and returns the elapsed ns.
double tock(struct timespec *T);
// //A utility macro which allows you to easily time a piece of code with TIME_CALL(my code)(ony works in GCC AFAIK)
// #define TIME_CALL(F) ({\
//   struct timespec T;\
//   tick(&T);\
//   F ;\
//   elapsed_since(&T);\
// })

#include <stdio.h>

typedef enum{
  PASS,
  INFO,
  WARN,
  FAIL
}report_level_t;

extern FILE* DEFAULT_REPORT_STREAM;
extern int REPORT_TIMESTAMP;
extern int REPORT_W_COLORS;

void report(report_level_t level, const char* format, ...) __attribute__((format(printf, 2, 3)));
void reportf(report_level_t level, int timestamped, int colored, const char* format, ...) __attribute__((format(printf, 4, 5)));
void freport(FILE* stream, report_level_t level, const char* format, ...) __attribute__((format(printf, 3, 4)));
void freportf(FILE* stream, report_level_t level, int timestamped, int colored, const char* format, ...) __attribute__((format(printf, 5, 6)));

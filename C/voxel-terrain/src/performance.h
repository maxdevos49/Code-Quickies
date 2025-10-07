#ifndef PERFORMANCE_H
#define PERFORMANCE_H 1

#include <time.h>

static double get_time_ms(void) {
  struct timespec ts;
  clock_gettime(CLOCK_MONOTONIC, &ts);
  return (ts.tv_sec * 1000.0) + (ts.tv_nsec / 1e6);
}

#endif

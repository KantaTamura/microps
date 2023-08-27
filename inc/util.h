#ifndef MICROPS_UTIL_H
#define MICROPS_UTIL_H

#include <stdio.h>

#define errorf(...) lprintf(stderr, 'E', __FILE__, __LINE__, __func__, __VA_ARGS__)
#define warnf(...)  lprintf(stderr, 'W', __FILE__, __LINE__, __func__, __VA_ARGS__)
#define infof(...)  lprintf(stderr, 'I', __FILE__, __LINE__, __func__, __VA_ARGS__)
#define debugf(...) lprintf(stderr, 'D', __FILE__, __LINE__, __func__, __VA_ARGS__)

extern int lprintf(FILE *stream, int level, const char *file, int line, const char *func, const char *fmt, ...);

#endif //MICROPS_UTIL_H

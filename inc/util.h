#ifndef MICROPS_UTIL_H
#define MICROPS_UTIL_H

#include <stdio.h>
#include <stdint.h>

// array

#define countof(x) ((sizeof(x) / sizeof(*x)))

// logging

#define errorf(...) lprintf(stderr, 'E', __FILE__, __LINE__, __func__, __VA_ARGS__)
#define warnf(...)  lprintf(stderr, 'W', __FILE__, __LINE__, __func__, __VA_ARGS__)
#define infof(...)  lprintf(stderr, 'I', __FILE__, __LINE__, __func__, __VA_ARGS__)
#define debugf(...) lprintf(stderr, 'D', __FILE__, __LINE__, __func__, __VA_ARGS__)

#ifndef HEXDUMP
#define debugdump(...) hexdump(stderr, __VA_ARGS__)
#else
#define debugdump(...)
#endif

extern int lprintf(FILE *stream, int level, const char *file, int line, const char *func, const char *fmt, ...);
extern void hexdump(FILE *stream, const void *data, size_t size);

// byteorder

extern uint16_t hton16(uint16_t x);
extern uint32_t hton32(uint32_t x);
extern uint16_t ntoh16(uint16_t x);
extern uint32_t ntoh32(uint32_t x);

// checksum

extern uint16_t checksum16(uint16_t *addr, uint16_t count, uint32_t init);

#endif //MICROPS_UTIL_H

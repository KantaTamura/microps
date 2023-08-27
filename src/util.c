#include "util.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>
#include <ctype.h>
#include <time.h>
#include <sys/time.h>

// ANSI escape codes
#define ANSI_RESET  "\e[0m"
#define ANSI_RED    "\e[31m"
#define ANSI_GREEN  "\e[32m"
#define ANSI_YELLOW "\e[33m"
#define ANSI_BLUE   "\e[34m"
#define ANSI_CYAN   "\e[36m"

// logging

int lprintf(FILE *stream, int level, const char *file, int line, const char *func, const char *fmt, ...) {
    struct timeval tv;
    struct tm tm;
    char timestamp[32];
    char *color = ANSI_RESET;
    int n = 0;
    va_list ap;

    switch (level) {
        case 'E': color = ANSI_RED;    break;
        case 'W': color = ANSI_YELLOW; break;
        case 'I': color = ANSI_BLUE;   break;
        case 'D': color = ANSI_GREEN;  break;
    }

    flockfile(stream);
    gettimeofday(&tv, NULL);
    strftime(timestamp, sizeof(timestamp), "%T", localtime_r(&tv.tv_sec, &tm));
    n += fprintf(stream, "%s.%03d %s[%c] %s%s: %s", timestamp, (int)(tv.tv_usec / 1000), color, level, ANSI_CYAN, func, ANSI_RESET);
    va_start(ap, fmt);
    n += vfprintf(stream, fmt, ap);
    va_end(ap);
    n += fprintf(stream, " (%s%s:%d%s)\n", ANSI_CYAN, file, line, ANSI_RESET);
    funlockfile(stream);
    return n;
}

void hexdump(FILE *stream, const void *data, size_t size) {
    unsigned char *src;

    flockfile(stream);
    src = (unsigned char *)data;
    fprintf(stream, "+------+-------------------------------------------------+------------------+\n");
    for (int offset = 0; offset < (int)size; offset += 16) {
        fprintf(stream, "| %04x | ", offset);
        for (int index = 0; index < 16; index++) {
            if (offset + index < (int)size) {
                fprintf(stream, "%02x ", 0xff & src[offset + index]);
            } else {
                fprintf(stream, "   ");
            }
        }
        fprintf(stream, "| ");
        for (int index = 0; index < 16; index++) {
            if (offset + index < (int)size) {
                if (isascii(src[offset + index]) && isprint(src[offset + index])) {
                    fprintf(stream, "%c", src[offset + index]);
                } else {
                    fprintf(stream, ".");
                }
            } else {
                fprintf(stream, " ");
            }
        }
        fprintf(stream, " |\n");
    }
    fprintf(stream, "+------+-------------------------------------------------+------------------+\n");
    funlockfile(stream);
}

// byteorder

enum byteorder {
    BYTEORDER_LITTLE_ENDIAN,
    BYTEORDER_BIG_ENDIAN,
    BYTEORDER_UNKNOWN
};

static enum byteorder endian = BYTEORDER_UNKNOWN;

static enum byteorder get_byteorder() {
    uint32_t x = 0x00000001;
    uint8_t *p = (uint8_t *)&x;
    return (p[0] == 0x01) ? BYTEORDER_LITTLE_ENDIAN : BYTEORDER_BIG_ENDIAN;
}

static uint16_t byte_swap16(uint16_t x) {
    return (x & 0x00ff) << 8 | (x & 0xff00) >> 8;
}

static uint32_t byte_swap32(uint32_t x) {
    return (x & 0x000000ff) << 24 | (x & 0x0000ff00) << 8 |
           (x & 0x00ff0000) >> 8  | (x & 0xff000000) >> 24;
}

// host to network byte order conversion (16bit)
uint16_t hton16(uint16_t x) {
    if (endian == BYTEORDER_UNKNOWN) {
        endian = get_byteorder();
    }
    return (endian == BYTEORDER_LITTLE_ENDIAN) ? byte_swap16(x) : x;
}

// host to network byte order conversion (32bit)
uint32_t hton32(uint32_t x) {
    if (endian == BYTEORDER_UNKNOWN) {
        endian = get_byteorder();
    }
    return (endian == BYTEORDER_LITTLE_ENDIAN) ? byte_swap32(x) : x;
}

// network to host byte order conversion (16bit)
uint16_t ntoh16(uint16_t x) {
    if (endian == BYTEORDER_UNKNOWN) {
        endian = get_byteorder();
    }
    return (endian == BYTEORDER_LITTLE_ENDIAN) ? byte_swap16(x) : x;
}

// network to host byte order conversion (32bit)
uint32_t ntoh32(uint32_t x) {
    if (endian == BYTEORDER_UNKNOWN) {
        endian = get_byteorder();
    }
    return (endian == BYTEORDER_LITTLE_ENDIAN) ? byte_swap32(x) : x;
}

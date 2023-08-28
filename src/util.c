#include "util.h"

#include <stdio.h>
#include <stdarg.h>
#include <ctype.h>
#include <time.h>
#include <sys/time.h>

#include "platform.h"

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

// queue

struct queue_entry {
    queue_entry *next;
    void *data;
};

void queue_init(queue_head *queue) {
    queue->head = NULL;
    queue->tail = NULL;
    queue->num = 0;
}

void *queue_push(queue_head *queue, void *data) {
    queue_entry *entry;

    if (!queue) {
        errorf("queue is NULL");
        return NULL;
    }
    if ((entry = memory_alloc(sizeof(queue_entry))) == NULL) {
        errorf("memory_alloc() failed");
        return NULL;
    }

    entry->next = NULL;
    entry->data = data;

    if (queue->tail) {
        queue->tail->next = entry;
    }
    queue->tail = entry;

    if (!queue->head) {
        queue->head = entry;
    }

    queue->num++;
    return data;
}

void *queue_pop(queue_head *queue) {
    queue_entry *entry;
    void *data;

    if (!queue || !queue->head) {
        errorf("queue is NULL");
        return NULL;
    }

    entry = queue->head;
    queue->head = entry->next;
    if (!queue->head) {
        queue->tail = NULL;
    }
    queue->num--;

    data = entry->data;
    memory_free(entry);
    return data;
}

void *queue_peek(queue_head *queue) {
    if (!queue || !queue->head) {
        errorf("queue is NULL");
        return NULL;
    }
    return queue->head->data;
}

void queue_foreach(queue_head *queue, void (*func)(void *arg, void *data), void *arg) {
    queue_entry *entry;

    if (!queue || !func) {
        errorf("queue or func is NULL");
        return;
    }

    for (entry = queue->head; entry; entry = entry->next) {
        func(arg, entry->data);
    }
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

// checksum

uint16_t checksum16(uint16_t *addr, uint16_t count, uint32_t init) {
    uint32_t sum = init;

    while (count > 1) {
        sum += *addr++;
        count -= 2;
    }
    // if count is odd then add the last 8 bits
    if (count > 0) {
        sum += *(uint8_t *)addr++;
        count -= 1;
    }
    // add the carries
    while (sum >> 16) {
        sum = (sum & 0xffff) + (sum >> 16);
    }
    return ~(uint16_t)sum;
}

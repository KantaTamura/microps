#include "util.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
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

SRCS := $(wildcard src/*.c)
OBJS := $(patsubst src/%.c, obj/%.o, $(SRCS))
INCS := $(wildcard inc/*.h)

ifeq ($(shell uname), Linux)
	CC := gcc
	CFLAGS := -g -Wall -Wextra -pthread -std=c11 -I inc
	LINUX_SRCS := $(wildcard src/linux/*.c)
	SRCS += $(LINUX_SRCS)
	OBJS += $(patsubst src/linux/%.c, obj/%.o, $(LINUX_SRCS))
	INCS += $(wildcard inc/linux/*.h)
else
	$(error "unsupported platform")
endif

.SUFFIXES:
.SUFFIXES: .c .o

.PHONY: clean

all: microps

microps: $(OBJS)
	$(CC) $(OBJS) $(CFLAGS) -o $@

obj/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJS): $(INCS)

clean:
	rm -f microps $(OBJS)

SRCS := $(wildcard src/*.c)
OBJS := $(patsubst src/%.c, obj/%.o, $(SRCS))
INCS := $(wildcard inc/*.h)

TESTS := $(wildcard tests/*.c)
TEST_EXE := $(patsubst tests/%.c, tests/%, $(TESTS))

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

all: microps test

microps: $(OBJS)
	$(CC) main.c $(OBJS) $(CFLAGS) -o $@

obj/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJS): $(INCS)

test : $(TEST_EXE)

tests/% : tests/%.c $(OBJS)
	$(CC) $(CFLAGS) $< $(OBJS) -o $@

clean:
	rm -f microps $(OBJS) $(TEST_EXE)

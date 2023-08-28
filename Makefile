SRCS := $(wildcard src/*.c)
OBJS := $(patsubst src/%.c, obj/%.o, $(SRCS))
INCS := $(wildcard inc/*.h)

DRIVER_SRCS := $(wildcard src/driver/*.c)
SRCS += $(DRIVER_SRCS)
OBJS += $(patsubst src/driver/%.c, obj/driver/%.o, $(DRIVER_SRCS))
INCS += $(wildcard inc/driver/*.h)

TESTS := $(wildcard tests/*.c)
TEST_EXE := $(patsubst tests/%.c, tests/%, $(TESTS))

PLATFORM_DIR :=

ifeq ($(shell uname), Linux)
	CC := gcc
	CFLAGS := -g -Wall -Wextra -pthread -std=gnu11 -I inc -I inc/driver -I inc/platform/linux
	PLATFORM_DIR := src/platform/linux
	LINUX_SRCS := $(wildcard src/platform/linux/*.c)
	SRCS += $(LINUX_SRCS)
	OBJS += $(patsubst src/platform/linux/%.c, obj/platform/linux/%.o, $(LINUX_SRCS))
	INCS += $(wildcard inc/platform/linux/*.h)
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

obj/driver/%.o: src/driver/%.c | create_driver_dir
	$(CC) $(CFLAGS) -c $< -o $@

create_driver_dir:
	mkdir -p obj/driver

$(OBJS): $(INCS)

test : $(TEST_EXE)

tests/% : tests/%.c $(OBJS)
	$(CC) $(CFLAGS) $< $(OBJS) -o $@

clean:
	rm -f microps $(OBJS) $(TEST_EXE)

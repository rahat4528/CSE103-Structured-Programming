CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -Wpedantic -O2 -Isrc
TARGET = railway_system

SRCS = src/main.c src/train.c src/booking.c src/data.c src/fileio.c src/utils.c
OBJS = $(SRCS:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

test: $(TARGET)
	bash tests/run_tests.sh

clean:
	rm -f $(OBJS) $(TARGET) $(TARGET).exe *.bak tests/*.tmp tickets/*.txt

.PHONY: all test clean

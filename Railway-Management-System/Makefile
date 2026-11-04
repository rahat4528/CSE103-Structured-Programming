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

clean:
	rm -f $(OBJS) $(TARGET) $(TARGET).exe *.bak tickets/*.txt

.PHONY: all clean

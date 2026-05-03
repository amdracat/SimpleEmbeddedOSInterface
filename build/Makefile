CC = gcc
CFLAGS = -I../inc -pthread
SRCS = $(wildcard ../src/*.c)
OBJS = $(SRCS:.c=.o)
TARGET = main

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

.PHONY: clean
clean:
	rm -f $(OBJS) $(TARGET)
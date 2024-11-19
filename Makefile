CC = gcc
CFLAGS = -Wall -Wextra -g -std=c11
TARGET = qr
SRCS = qr.c qrcodegen.c TinyPngOut.c
OBJS = $(SRCS:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(TARGET) $(OBJS) out.png

.PHONY: all clean

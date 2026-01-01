CC = gcc
CFLAGS = -Wall -Wextra -Iinclude

TARGET = heap_test

SRC = src/heap.c src/main.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET)

clean:
	rm -f $(TARGET)

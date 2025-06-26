CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -pthread
TARGET = sudoku_validator
SRCDIR = src
SOURCES = $(SRCDIR)/sudoku_validator.c
HEADERS = $(SRCDIR)/sudoku_validator.h

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(SOURCES) $(HEADERS)
	$(CC) $(CFLAGS) -I$(SRCDIR) -o $(TARGET) $(SOURCES)

clean:
	rm -f $(TARGET)

test: $(TARGET)
	./$(TARGET) test.txt

install: $(TARGET)
	cp $(TARGET) /usr/local/bin/

.SUFFIXES: .c .o
.c.o:
	$(CC) $(CFLAGS) -c $
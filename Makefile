CC=gcc
CFLAGS=-Wall -pedantic -std=gnu99
SOURCES=main.c options.c utilities.c io.c purifier.c lib/filelock.c
EXECUTABLE=codepurifier

all:
	$(CC) $(CFLAGS) $(SOURCES) -o $(EXECUTABLE)

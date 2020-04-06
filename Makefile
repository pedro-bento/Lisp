exec = lisp
CC = gcc
CFLAGS = -Wall

$(exec): $(wildcard src/*.c)
	$(CC) $(CFLAGS) -o $(exec) src/include.c

install:
	make -B
	cp $(exec) /usr/local/bin/$(exec)

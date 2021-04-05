# -*- Makefile -*-

CC = aarch64-none-linux-gnu-gcc
CFLAGS = -g -Wall

BIN = bin/main
OBJ = obj

all: $(BIN)

$(BIN): obj/main.o obj/opticSensor.o
	mkdir -p bin
	$(CC) $(CFLAGS) $^ -o $@

obj/main.o: src/main.c
	mkdir -p obj
	$(CC) $(CFLAGS) -c $^ -o $@

obj/%.o: src/%.c
	$(CC) $(CFLAGS) -c $^ -o $@

clear:
	$(RM) -r bin obj


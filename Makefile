# -*- Makefile -*-

CC = aarch64-none-linux-gnu-gcc
CFLAGS = -g -Wall

BIN = bin/main
OBJ = obj

all: $(BIN)

$(BIN): obj/main.o obj/opticSensor.o
	mkdir bin
	$(CC) $(CFLAGS) $^ -o $@

obj/main.o: src/main.c
	mkdir obj
	$(CC) $(CFLAGS) -c $^ -o $@

obj/%.o: src/%.c
	$(CC) $(CFLAGS) -c $^ -o $@

clear:
ifeq ($(OS),Windows_NT) 
	del /Q bin\* obj\*
else
	rm -r bin/* obj/*
endif

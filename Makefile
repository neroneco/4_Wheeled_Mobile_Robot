# -*- Makefile -*-

CC := aarch64-none-linux-gnu-gcc
CFLAGS := -g -Wall -lm -lncurses

SRC := src
BIN := bin
OBJ := obj

SOURCES := $(wildcard src/*.c)
OBJECTS := $(patsubst src/%.c,obj/%.o,$(SOURCES))


all: $(BIN)/main

$(OBJ):
	mkdir -p obj

$(BIN):
	mkdir -p bin

$(BIN)/main: $(OBJECTS)
	$(CC) $(CFLAGS) $^ -o $@

obj/%.o: $(OBJ) $(SRC)/%.c
	$(CC) $(CFLAGS) -c $(SRC)/$*.c -o $@

distanceMeasure: $(OBJ)/distanceMeasure.o $(OBJ)/opticSensor.o $(OBJ)/i2c_com.o
	mkdir -p bin
	$(CC) $(CFLAGS) $^ -o $(BIN)/$@

barometer: $(OBJ)/barometer.o $(OBJ)/altimu-10-v5.o $(OBJ)/i2c_com.o
	mkdir -p bin
	$(CC) $(CFLAGS) $^ -o $(BIN)/$@

gyroscope: $(OBJ)/gyroscope.o $(OBJ)/altimu-10-v5.o $(OBJ)/i2c_com.o
	mkdir -p bin
	$(CC) $(CFLAGS) $^ -o $(BIN)/$@

magnCalibrate: $(OBJ)/magnCalibrate.o $(OBJ)/altimu-10-v5.o $(OBJ)/i2c_com.o
	mkdir -p bin
	$(CC) $(CFLAGS) $^ -o $(BIN)/$@

clear:
	$(RM) -r $(BIN) $(OBJ)


# -*- Makefile -*-

CC_ARM := aarch64-none-linux-gnu-gcc
CC_x86_64 := gcc
CFLAGS := -g -Wall

SRC := src
BIN_ARM := bin_arm
OBJ_ARM := obj_arm
BIN_x86_64 := bin_x86_64
OBJ_x86_64 := obj_x86_64

SOURCES := $(wildcard src/*.c)
OBJECTS := $(patsubst src/%.c,obj/%.o,$(SOURCES))


all: client controller server sensors wheels_control

$(OBJ_ARM):
	mkdir -p $(OBJ_ARM)

$(BIN_ARM):
	mkdir -p $(BIN_ARM)

$(OBJ_x86_64):
	mkdir -p $(OBJ_x86_64)

$(BIN_x86_64):
	mkdir -p $(BIN_x86_64)

$(OBJ_ARM)/%.o: $(OBJ_ARM) $(SRC)/%.c
	$(CC_ARM) $(CFLAGS) -c $(SRC)/$*.c -o $@

%: $(OBJ_ARM)/%.o
	mkdir -p $(BIN_ARM)
	$(CC_ARM) $(CFLAGS) $^ -o $(BIN_ARM)/$@

$(OBJ_x86_64)/%.o: $(OBJ_x86_64) $(SRC)/%.c
	$(CC_x86_64) $(CFLAGS) -c $(SRC)/$*.c -o $@

%: $(OBJ_x86_64)/%.o
	mkdir -p $(BIN_x86_64)
	$(CC_x86_64) $(CFLAGS) $^ -o $(BIN_x86_64)/$@

client: $(OBJ_x86_64)/altimu-10-v5.o $(OBJ_x86_64)/i2c_com.o $(OBJ_x86_64)/client.o
	mkdir -p $(BIN_x86_64)
	$(CC_x86_64) $(CFLAGS) $^ -o $(BIN_x86_64)/$@ -lm

controller: $(OBJ_x86_64)/controller.o
	mkdir -p $(BIN_x86_64)
	$(CC_x86_64) $(CFLAGS) -pthread $^ -o $(BIN_x86_64)/$@

server: $(OBJ_ARM)/server.o
	mkdir -p $(BIN_ARM)
	$(CC_ARM) $(CFLAGS) $^ -o $(BIN_ARM)/$@

sensors: $(OBJ_ARM)/sensors.o $(OBJ_ARM)/altimu-10-v5.o $(OBJ_ARM)/i2c_com.o
	mkdir -p $(BIN_ARM)
	$(CC_ARM) $(CFLAGS) $^ -o $(BIN_ARM)/$@ -lm

wheels_control: $(OBJ_ARM)/wheels_control.o $(OBJ_ARM)/nanohat_motor.o $(OBJ_ARM)/i2c_com.o
	mkdir -p $(BIN_ARM)
	$(CC_ARM) $(CFLAGS) $^ -o $(BIN_ARM)/$@

clean:
	$(RM) -r $(BIN_ARM) $(OBJ_ARM) $(BIN_x86_64) $(OBJ_x86_64)


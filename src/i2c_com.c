/*
 * Simple prototype functions to communicate with
 * devices via i2c protocol
 * 
 * In this function read, wirte functions are
 * used. Alternative version might be done with
 * i2c_smbus_read_byte_data, i2c_smbus_write_byte_data
 */

#pragma GCC diagnostic ignored "-Wimplicit-function-declaration"

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <stdint.h>
#include <linux/i2c-dev.h>

#include "i2c_com.h"


int i2c_init(struct i2c_table* table){
	table->file = open(table->filename, O_RDWR);
	if (table->file < 0){
		// error handling
		fprintf(stderr,"ERROR:(dev addr: %d) in open: %s\n",table->addr,strerror(errno));
		exit(1);
	}

	// specifying address of communication
	if(ioctl(table->file, I2C_SLAVE, table->addr) < 0){
        // error handling
		fprintf(stderr,"ERROR:(dev addr: %d) in ioctl: %s\n",table->addr,strerror(errno));
		exit(1);
	}
    return 0;
}

// writing to device
int i2c_write(struct i2c_table* table, uint8_t* buf, uint8_t num){

	if(write(table->file, buf, num) != num){
        // error handling
		fprintf(stderr,"ERROR:(dev addr: %d) in write: %s\n",table->addr,strerror(errno));
		exit(1);
	}

	return 0;
}

// reading from device
int i2c_read(struct i2c_table* table, uint8_t* buf, uint8_t num){

	if(read(table->file, buf, num) != num){
        // error handling
		fprintf(stderr,"ERROR:(dev addr: %d) in read: %s\n",table->addr,strerror(errno));
		exit(1);
	}

	return 0;
}

// closing communication with device
int i2c_close(struct i2c_table* table){
	close(table->file);

	return 0;
}
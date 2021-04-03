/*
Simple prototype function to communicate with
NanoHat motor module (PCA9685 chip, precisely)

In this function read, wirte functions are
used. Alternative version might be done with
i2c_smbus_read_byte_data, i2c_smbus_write_byte_data
*/

#pragma GCC diagnostic ignored "-Wimplicit-function-declaration"

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <stdint.h>
#include <linux/i2c-dev.h>

#include "opticSensor.h"


int optic_sens_init(struct optic_sens_table* table){

    // opening device
	table->file = open(table->filename, O_RDWR);
	if (table->file < 0){
		// error handling
		fprintf(stderr,"ERROR: in open: %s\n",strerror(errno));
		exit(1);
	}

	// specifying address of communication
	if(ioctl(table->file, I2C_SLAVE, table->addr) < 0){
		fprintf(stderr,"ERROR: in ioctl: %s\n",strerror(errno));
		exit(1);
	}
    return 0;
}

// writing to device
int optic_sens_write(struct optic_sens_table* table, char buf){

	if(write(table->file, buf, 1) != 1){
		fprintf(stderr,"ERROR: in write: %s\n",strerror(errno));
		exit(1);
	}
	else{
		fprintf(stdout,"Writing to device...\n");
	}
	return 0;
}

// reading from device
int optic_sens_read(struct optic_sens_table* table, char buf){

	if(read(table->file, buf, 1) != 1){
		fprintf(stderr,"ERROR: in read: %s\n",strerror(errno));
		exit(1);
	}
	else{
		fprintf(stdout,"Reading from the device...\n");
		fprintf(stdout,"Value read: %d \n", buf);
	}
	return 0;
}

int optic_sens_close(struct optic_sens_table* table){
	close(table->file);

	return 0;
}

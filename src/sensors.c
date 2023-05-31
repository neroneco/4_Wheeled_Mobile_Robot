#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <stdint.h>
#include <time.h>
#include <linux/i2c-dev.h>

#include "i2c_com.h"
#include "altimu-10-v5.h"


int main(int argc, char **argv){

	fcntl(0, F_SETFL, O_NONBLOCK);

	uint8_t* buffer; // initializing buffer for communication
	buffer = malloc(20);
	memset(buffer,0x0,20);


	//---ADC_INITIAZLIATION---
	struct i2c_table sensor_table = {0, 0x48, "/dev/i2c-0"};

	buffer[0] = 0b00000001;  // control byte: single-ended output on chanel AIN1

	i2c_init( &sensor_table);
	i2c_write(&sensor_table, buffer,1);
	//---ADC_INITIALIZATION_END---



	//---ACCELEROMETER_INITIALIZATION---
	struct i2c_table acce_table = {
	                              .file = 0,
		                      .addr = LSM6DS33_ADDRESS,
		                      .filename = "/dev/i2c-0"
			              };
        i2c_init(&acce_table);
	set_up_LSM6DS33(acce_table,buffer);
	//---ACCELEROMETER_INITIALIZATION_END---


	char buf[2]={0};
	while(read(0,buf,1)==-1){

	    buffer[0] = 0b00000100;
	    i2c_write(&sensor_table, buffer,1);
	    
	    i2c_read(&sensor_table, buffer,4);

	    buffer[0]=buffer[2];
	    buffer[1]=buffer[3];

	    get_data_accelerometer(acce_table,buffer+2);

	    usleep(5000);
	    write(1,buffer,8);

	}

	i2c_close(&sensor_table);
	i2c_close(&acce_table);
	free(buffer);

	return 0;
}


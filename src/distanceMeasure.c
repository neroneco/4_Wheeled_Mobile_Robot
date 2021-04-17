#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <stdint.h>
#include <linux/i2c-dev.h>

#include "i2c_com.h"
#include "opticSensor.h"

#define V_REF 	   3.3
#define RESOLUTION 256


int main(int argc, char **argv){

	uint8_t buffer; // initializing buffer for communication

	struct i2c_table sensor_table = {0, 0x48, "/dev/i2c-0"};

	buffer = 0b00100001; 					  // control byte: single-ended output on chanel AIN1

	i2c_init( &sensor_table);
	i2c_write(&sensor_table, &buffer,1); // not sure if this is needed TODO: check if it is

	printf("\n Reading from the ADC:\n"
	       " ______ \n");

	float real_voltage_value;
	float distance;

	while(1){
		i2c_read(&sensor_table, &buffer,1);
		real_voltage_value = (V_REF/RESOLUTION)*buffer;
		distance = voltage_to_distance(real_voltage_value);
		printf("| %5.2f |\n",distance);
		sleep(1);
	}

	i2c_close(&sensor_table);

	return 0;
}

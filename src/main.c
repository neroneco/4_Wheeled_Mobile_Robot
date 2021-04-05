#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <stdint.h>
#include <linux/i2c-dev.h>

#include "opticSensor.h"

#define V_REF 	   3.3
#define RESOLUTION 256


int main(int argc, char **argv){

	uint8_t buffer; // initializing buffer for communication

	struct optic_sens_table sensor_table = {0, 0x48, "/dev/i2c-0"};

/*
	sensor_table->addr = 0x48;
	strcpy(sensor_table->filename, "/dev/i2c-0");
*/


	buffer = 0b00100001; 					  // control byte: single-ended output on chanel AIN1
	
	optic_sens_init( &sensor_table);
	optic_sens_write(&sensor_table, &buffer); // not sure if this is needed TODO: check if it is

	printf("Reading from the ADC:\n");
	printf("---------------------\n");

	float real_voltage_value;
	float distance;

	while(1){
		optic_sens_read(&sensor_table, &buffer);
		real_voltage_value = (V_REF/RESOLUTION)*buffer;
		distance = voltage_to_distance(real_voltage_value);
		printf("| %5.2f |\n",distance);
		sleep(1);
	}

	optic_sens_close(&sensor_table);

	return 0;
}

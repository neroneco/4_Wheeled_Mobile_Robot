#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <stdint.h>
#include <linux/i2c-dev.h>

#include "opticSensor.h"


int main(int argc, char **argv){

	char buffer = 'A'; // initializing buffer for communication

	struct optic_sens_table sensor_table = {0, 0x48, "/dev/i2c-0"};

/*
	sensor_table->addr = 0x48;
	strcpy(sensor_table->filename, "/dev/i2c-0");
*/

	optic_sens_init( &sensor_table);
	optic_sens_write(&sensor_table, buffer);

	while(1){
		optic_sens_read(&sensor_table, buffer);
		sleep(1);
	}

	optic_sens_close(&sensor_table);

	return 0;
}

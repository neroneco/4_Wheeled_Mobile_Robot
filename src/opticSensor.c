/*
Simple prototype functions to communicate with
devices via i2c protocol

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


float voltage_to_distance(float voltage){

	float distance;
	float a;   // angular coefficient

	struct vol_to_dis_table tab = {
			{2.60,2.10,1.65,1.50,1.39,1.13,0.98,0.85,0.75,0.68,0.62,0.57,0.54,0.50,0.47,0.43,1.41},   // voltage
			{10,12,16,18,20,25,30,35,40,45,50,55,60,65,70,75,80}   // coresponding distance
								  };

	for(int i=0; i<17; i++){
		if(voltage>=tab.voltage[i+1] && voltage<=tab.voltage[i]){
			a = (tab.voltage[i+1]-tab.voltage[i])/(tab.distance[i+1]-tab.distance[i]);
			distance = tab.distance[i] - (tab.voltage[i]-voltage)/a;
			break;
		}
		else{
			distance = 0;
		}
	}
	return distance;
}

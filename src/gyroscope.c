#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <math.h>

#include "i2c_com.h"
#include "altimu-10-v5.h"


int main(int argc, char **argv){

    fcntl(0, F_SETFL, O_NONBLOCK);

    uint8_t* buffer; 
    buffer = malloc(20);

    struct object* acce;

    acce = malloc(sizeof(struct object));

    /* Setting up device tables: */
    struct i2c_table acce_table = {
                                  .file = 0,
                                  .addr = LSM6DS33_ADDRESS,
                                  .filename = "/dev/i2c-0"
                                  };

    i2c_init(&acce_table);

    /* SETTING UP DEVICES: */
    set_up_LSM6DS33(acce_table,buffer);
    
    char buf[2]={0};
    while(read(0,buf,1)==-1){

        /* RECEIVING DATA: */
        get_data_accelerometer(acce_table,buffer);

        acce->raw[X].LSB = buffer[0];
        acce->raw[X].MSB = buffer[1];
        acce->raw[Y].LSB = buffer[2];
        acce->raw[Y].MSB = buffer[3];
        acce->raw[Z].LSB = buffer[4];
        acce->raw[Z].MSB = buffer[5];

        acce->values[X] = acce_convert(acce->raw[X]);
        acce->values[Y] = acce_convert(acce->raw[Y]);
        acce->values[Z] = acce_convert(acce->raw[Z]);

        /* It is possible to measure only         */
        /* Roll and Pitch with the accelerometer: */
        acce->degrees[X] = vector_to_degree(acce->values[X],acce->values[Y],acce->values[Z]);
        acce->degrees[Y] = vector_to_degree(acce->values[Y],acce->values[X],acce->values[Z]);

        /* PRINTING DATA: */
        printf("x=%f y=%f \n",acce->degrees[X],acce->degrees[Y]);

	usleep(1000000);
    }

    printf("\nExiting...\n");

    i2c_close(&acce_table);
    free(buffer);
    free(acce);

    return 0;
}


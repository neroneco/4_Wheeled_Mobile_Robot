#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>

#include "i2c_com.h"
#include "altimu-10-v5.h"

int main(int argc, char **argv){

    uint8_t* buffer; 
    buffer = malloc(10);

    /* Setting up device table: */
    struct i2c_table gyro_table = {
                                  .file = 0,
                                  .addr = LSM6DS33_ADDRESS,
                                  .filename = "/dev/i2c-0"
                                 };
    
    /* Sets reading delay to 0.5 seconds: */
    struct timespec delay = {
                             .tv_sec  = 0,
                             .tv_nsec = 500000000
                            };

    i2c_init(&gyro_table);

    printf("\n  Reading from LSM6DS33 chip:    \n"
           " ________________________________  \n"
           "| Pitch(x): | Roll(y): | Yaw(z): | \n"
           "|___________|__________|_________| \n");


    /* SETTING UP DEVICE: */
    // address of the devices register:
    buffer[0] = CTRL2_G;

    // value to be written to register:
    buffer[1] = 0b01100000; // set 416 Hz (high performance mode)

    i2c_write(&gyro_table, buffer, 3);
    
    buffer[0] = CTRL3_C;
    buffer[1] = 0b01000100; // set BDU(Block Data Update) bit and IF_INC(auto increment address) bit
    i2c_write(&gyro_table, buffer, 3);
    /*--------------------*/

    while(1){

        /* RECEIVING DATA: */
        buffer[0] = OUT_G;

        i2c_write(&gyro_table,buffer,1);
        i2c_read( &gyro_table,buffer,6);
        /*-----------------*/

        /* PRINTING DATA: */
        printf("|%5d|%5d|%5d|%5d|%5d|%5d|\n",
                buffer[0],
                buffer[1],
                buffer[2],
                buffer[3],
                buffer[4],
                buffer[5]
              );
        /*----------------*/
        
        nanosleep(&delay,NULL);
    }

    i2c_close(&gyro_table);
    free(buffer);

    return 0;
}
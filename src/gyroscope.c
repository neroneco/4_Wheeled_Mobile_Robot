#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>

#include "i2c_com.h"
#include "altimu-10-v5.h"

int main(int argc, char **argv){

    uint8_t* buffer; 
    buffer = malloc(10);

    float dps_X;
    float dps_Y;
    float dps_Z;
    
    float degree_X;
    float degree_Y;
    float degree_Z;

    struct data_16_bit data_X;
    struct data_16_bit data_Y;
    struct data_16_bit data_Z;


    /* Setting up device table: */
    struct i2c_table gyro_table = {
                                  .file = 0,
                                  .addr = LSM6DS33_ADDRESS,
                                  .filename = "/dev/i2c-0"
                                 };
    
    /* Sets reading delay to 0.5 seconds: */
    struct timespec delay = {
                             .tv_sec  = 0,
                             .tv_nsec = 50000000
                            };

    i2c_init(&gyro_table);

    /* SETTING UP DEVICE: */
    set_up_LSM6DS33(gyro_table,buffer);

    printf("\n  Reading from LSM6DS33 chip:    \n"
       " ________________________________  \n"
       "| Pitch(x): | Roll(y): | Yaw(z): | \n"
       "|___________|__________|_________| \n");

    while(1){

        /* RECEIVING DATA: */
        get_data_LSM6DS33(gyro_table,buffer);

        /* PRINTING DATA: */
        printf("|%5d|%5d|%5d|%5d|%5d|%5d|\n",
                buffer[0],
                buffer[1],
                buffer[2],
                buffer[3],
                buffer[4],
                buffer[5]
              );
        
        data_X.LSB=buffer[0];
        data_X.MSB=buffer[1];
        data_Y.LSB=buffer[2];
        data_Y.MSB=buffer[3];
        data_Z.LSB=buffer[4];
        data_Z.MSB=buffer[5];

        dps_X = dps_convert(data_X);
        dps_Y = dps_convert(data_Y);
        dps_Z = dps_convert(data_Z);

        // Garbage data filter:
        if(dps_X < 5 && dps_X > -5){
            dps_X = 0;
        }
        if(dps_Y < 5 && dps_Y > -5){
            dps_Y = 0;
        }
        if(dps_Z < 5 && dps_Z > -5){
            dps_Z = 0;
        }
        //--------------------

        degree_X += dps_to_degree(dps_X,ODR_HZ);
        degree_Y += dps_to_degree(dps_Y,ODR_HZ);
        degree_Z += dps_to_degree(dps_Z,ODR_HZ);

        printf("|%5.2f|%5.2f|%5.2f|\n",degree_X,degree_Y,degree_Z);
        /*----------------*/
        
        nanosleep(&delay,NULL);
    }

    i2c_close(&gyro_table);
    free(buffer);

    return 0;
}
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>

#include "i2c_com.h"
#include "altimu-10-v5.h"

#define ERR_X  2
#define ERR_Y -4.90
#define ERR_Z -3.3

int main(int argc, char **argv){

    uint8_t* buffer; 
    buffer = malloc(20);

    float dps_X;
    float dps_Y;
    float dps_Z;
    
    float degree_X_gyro;
    float degree_Y_gyro;
    float degree_Z_gyro;

    float degree_X_acce;
    float degree_Y_acce;

    float acce_X;
    float acce_Y;
    float acce_Z;

    struct data_16_bit data_X_gyro;
    struct data_16_bit data_Y_gyro;
    struct data_16_bit data_Z_gyro;

    struct data_16_bit data_X_acce;
    struct data_16_bit data_Y_acce;
    struct data_16_bit data_Z_acce;

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
       /* printf("|%5d|%5d|%5d|%5d|%5d|%5d|\n",
                buffer[0],
                buffer[1],
                buffer[2],
                buffer[3],
                buffer[4],
                buffer[5]
              );
        */
        
        data_X_gyro.LSB=buffer[0];
        data_X_gyro.MSB=buffer[1];
        data_Y_gyro.LSB=buffer[2];
        data_Y_gyro.MSB=buffer[3];
        data_Z_gyro.LSB=buffer[4];
        data_Z_gyro.MSB=buffer[5];

        data_X_acce.LSB=buffer[6];
        data_X_acce.MSB=buffer[7];
        data_Y_acce.LSB=buffer[8];
        data_Y_acce.MSB=buffer[9];
        data_Z_acce.LSB=buffer[10];
        data_Z_acce.MSB=buffer[11];

        dps_X = dps_convert(data_X_gyro)-ERR_X;
        dps_Y = dps_convert(data_Y_gyro)-ERR_Y;
        dps_Z = dps_convert(data_Z_gyro)-ERR_Z;

        acce_X = acce_convert(data_X_acce);
        acce_Y = acce_convert(data_Y_acce);
        acce_Z = acce_convert(data_Z_acce);

        degree_X_gyro += dps_to_degree(dps_X,ODR_HZ);
        degree_Y_gyro += dps_to_degree(dps_Y,ODR_HZ);
        degree_Z_gyro += dps_to_degree(dps_Z,ODR_HZ);

        /* It is possible to measure only         */
        /* Roll and Pitch with the accelerometer: */
        degree_X_acce = acce_to_degree(acce_X,acce_Y,acce_Z);
        degree_Y_acce = acce_to_degree(acce_Y,acce_X,acce_Z);

        //printf("|%5.2f|%5.2f|%5.2f|\n",dps_X,dps_Y,dps_Z);
        printf("gyro: |%5.2f|%5.2f|%5.2f|\n",degree_X_gyro,degree_Y_gyro,degree_Z_gyro);
        printf("acce: |%5.2f|%5.2f|XXXXX|\n",degree_X_acce,degree_Y_acce);
        
        nanosleep(&delay,NULL);
    }

    i2c_close(&gyro_table);
    free(buffer);

    return 0;
}
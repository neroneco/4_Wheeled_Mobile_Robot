#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <signal.h>
#include <time.h>
#include <math.h>

#include "i2c_com.h"
#include "altimu-10-v5.h"

#define ERR_X_G  2
#define ERR_Y_G -4.90
#define ERR_Z_G -3.3

#define ERR_X_M -0.0199
#define ERR_Y_M  0.3072
#define ERR_Z_M -1.3839

volatile static int run = 1;
 
static void SIGINT_handler(volatile int sig_num){
    run = 0;
}


int main(int argc, char **argv){

    struct sigaction sa;
    sigemptyset(&sa.sa_mask);
    sa.sa_handler = SIGINT_handler;
    if (sigaction(SIGINT, &sa, NULL) == -1)
        perror("sigaction");
    
    uint8_t* buffer; 
    buffer = malloc(20);

    struct object* gyro;
    struct object* acce;
    struct object* magn;

    gyro = malloc(sizeof(struct object));
    acce = malloc(sizeof(struct object));
    magn = malloc(sizeof(struct object));

    float  east_vector[3];
    float north_vector[3];

    /* Setting up device tables: */
    struct i2c_table gyro_acce_table = {
                                  .file = 0,
                                  .addr = LSM6DS33_ADDRESS,
                                  .filename = "/dev/i2c-0"
                                  };
    struct i2c_table magn_table = {
                                  .file = 0,
                                  .addr = LIS3MDL_ADDRESS,
                                  .filename = "/dev/i2c-0"
                                  };

    /* Sets reading delay to 0.5 seconds: */
    /* TODO: make timer insted of this    */
    struct timespec delay = {                   
                             .tv_sec  = 0,
                             .tv_nsec = 50000000
                            };

    i2c_init(&gyro_acce_table);
    i2c_init(&magn_table);

    /* SETTING UP DEVICES: */
    set_up_LSM6DS33(gyro_acce_table,buffer);
    set_up_LIS3MDL( magn_table,     buffer);

    printf("\n  Reading from LSM6DS33 chip:    \n"
           " ________________________________  \n"
           "| Pitch(x): | Roll(y): | Yaw(z): | \n"
           "|___________|__________|_________| \n");

    while(run){

        /* RECEIVING DATA: */
        get_data_LSM6DS33(gyro_acce_table,buffer);
        get_data_LIS3MDL( magn_table, (buffer+12));

        gyro->raw[X].LSB = buffer[0];
        gyro->raw[X].MSB = buffer[1];
        gyro->raw[Y].LSB = buffer[2];
        gyro->raw[Y].MSB = buffer[3];
        gyro->raw[Z].LSB = buffer[4];
        gyro->raw[Z].MSB = buffer[5]; 
        
        acce->raw[X].LSB = buffer[ 6];
        acce->raw[X].MSB = buffer[ 7];
        acce->raw[Y].LSB = buffer[ 8];
        acce->raw[Y].MSB = buffer[ 9];
        acce->raw[Z].LSB = buffer[10];
        acce->raw[Z].MSB = buffer[11];

        magn->raw[X].LSB = buffer[12];
        magn->raw[X].MSB = buffer[13];
        magn->raw[Y].LSB = buffer[14];
        magn->raw[Y].MSB = buffer[15];
        magn->raw[Z].LSB = buffer[16];
        magn->raw[Z].MSB = buffer[17];

        gyro->values[X] = dps_convert(gyro->raw[X])-ERR_X_G;
        gyro->values[Y] = dps_convert(gyro->raw[Y])-ERR_Y_G;
        gyro->values[Z] = dps_convert(gyro->raw[Z])-ERR_Z_G;

        acce->values[X] = acce_convert(acce->raw[X]);
        acce->values[Y] = acce_convert(acce->raw[Y]);
        acce->values[Z] = acce_convert(acce->raw[Z]);

        magn->values[X] = magn_convert(magn->raw[X])-ERR_X_M;
        magn->values[Y] = magn_convert(magn->raw[Y])-ERR_Y_M;
        magn->values[Z] = magn_convert(magn->raw[Z])-ERR_Z_M;

        gyro->degrees[X] += dps_to_degree(gyro->values[X],ODR_HZ);
        gyro->degrees[Y] += dps_to_degree(gyro->values[Y],ODR_HZ);
        gyro->degrees[Z] += dps_to_degree(gyro->values[Z],ODR_HZ);

        /* It is possible to measure only         */
        /* Roll and Pitch with the accelerometer: */
        acce->degrees[X] = vector_to_degree(acce->values[X],acce->values[Y],acce->values[Z]);
        acce->degrees[Y] = vector_to_degree(acce->values[Y],acce->values[X],acce->values[Z]);

        /* It is possible to measure only         */
        /* Pitch and Yaw with the magnetometer:   */
        cross_product(magn->values,acce->values,east_vector);
        cross_product(acce->values,east_vector,north_vector);

        magn->degrees[Z] = vector_to_degree(north_vector[Z],north_vector[X],north_vector[Y]);
        
        /* PRINTING DATA: */
        printf("gyro:         |%5.2f|%5.2f|%5.2f|\n",gyro->degrees[X],gyro->degrees[Y],gyro->degrees[Z]);
        printf("acce_magn:    |%5.2f|%5.2f|%5.2f|\n",acce->degrees[X],acce->degrees[Y],magn->degrees[Z]);
        
        nanosleep(&delay,NULL);
    }

    printf("\nExiting...\n");
    
    i2c_close(&gyro_acce_table);
    i2c_close(&magn_table);
    free(buffer);
    free(magn);
    free(acce);
    free(gyro);

    return 0;
}
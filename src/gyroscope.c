#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <signal.h>
#include <time.h>

#include "i2c_com.h"
#include "altimu-10-v5.h"

#define ERR_X  2
#define ERR_Y -4.90
#define ERR_Z -3.3

 volatile static int run = 1;
 
 static void SIGINT_handler(int sig_num){
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
        get_data_LIS3MDL( magn_table,     buffer);

        /*printf("|%5d|%5d|%5d|%5d|%5d|%5d|\n",
                buffer[0],
                buffer[1],
                buffer[2],
                buffer[3],
                buffer[4],
                buffer[5]
              );
        */

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

        gyro->values[X] = dps_convert(gyro->raw[X])-ERR_X;
        gyro->values[Y] = dps_convert(gyro->raw[Y])-ERR_Y;
        gyro->values[Z] = dps_convert(gyro->raw[Z])-ERR_Z;

        acce->values[X] = acce_convert(acce->raw[X]);
        acce->values[Y] = acce_convert(acce->raw[Y]);
        acce->values[Z] = acce_convert(acce->raw[Z]);

        gyro->degrees[X] += dps_to_degree(gyro->values[X],ODR_HZ);
        gyro->degrees[Y] += dps_to_degree(gyro->values[Y],ODR_HZ);
        gyro->degrees[Z] += dps_to_degree(gyro->values[Z],ODR_HZ);

        /* It is possible to measure only         */
        /* Roll and Pitch with the accelerometer: */
        acce->degrees[X] = acce_to_degree(acce->values[X],acce->values[Y],acce->values[Z]);
        acce->degrees[Y] = acce_to_degree(acce->values[Y],acce->values[X],acce->values[Z]);

        /* PRINTING DATA: */
        //printf("|%5.2f|%5.2f|%5.2f|\n",dps_X,dps_Y,dps_Z);
        printf("gyro:    |%5.2f|%5.2f|%5.2f|\n",gyro->degrees[X],gyro->degrees[Y],gyro->degrees[Z]);
        printf("acce:    |%5.2f|%5.2f|XXXXX|\n",acce->degrees[X],acce->degrees[Y]);
        //printf("magneto: |XXXXX|%5.2f|%5.2f|\n",degree_X_acce,degree_Y_acce);

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
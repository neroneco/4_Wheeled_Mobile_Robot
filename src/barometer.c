#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <stdint.h>
#include <signal.h>
#include <time.h>
#include <linux/i2c-dev.h>

#include "i2c_com.h"
#include "altimu-10-v5.h"


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

    uint8_t buffer[10];

    /*
     * Devives addresses:
     *  LIS3MDL  : address = 0x1e  (magnetic sensor         )
     *  LPS25H   : address = 0x5d  (barometer, thermometer  )
     *  LSM6DS33 : address = 0x6b  (accelerometer, gyroscope)
     */

    struct i2c_table bar_table = {0,LPS25H_ADDRESS,"/dev/i2c-0"};
    
    /* Sets reading delay to 0.5 seconds: */
    struct timespec delay = {
                             .tv_sec=0,
                             .tv_nsec=500000000
                            };

    struct data_24_bit press_data;
    struct data_16_bit tempe_data;
    float  press;
    float  tempe;

    i2c_init(&bar_table);

    printf("\n  Reading from LPS25H chip:\n"
           " __________________________  \n"
           "| Pressure: | Temperature: | \n"
           "|___________|______________| \n");

    /*
     * In order to read particular value of register of Altimu-10 v5
     * we have to send one byte which is address of register
     */

    while(run){
        /* 
         *  buffer[0]:
         *  address of the devices register 
         */
        buffer[0] = 0x20;

        /*
         *  buffer[1]:
         *  7 -> bit turn on device
         *  6,5,4-bits -> set output data rate (1Hz,7H,12.5Hz,25Hz)
         *  3-bit -> BDU bit (prevents from reading LSB or MSB from different samples)
         */
        buffer[1] = 0b10100100;
        i2c_write(&bar_table, buffer, 2);

        buffer[0] = 0x28 + (1<<7);        // MSB=1 -> autoincrement register address
        i2c_write(&bar_table, buffer, 1);
        i2c_read( &bar_table, buffer, 5);

        press_data.LSB_L = buffer[0];
        press_data.LSB_H = buffer[1];
        press_data.MSB   = buffer[2];

        tempe_data.LSB = buffer[3];
        tempe_data.MSB = buffer[4];

        press = pressure_convert(press_data);
        tempe = temperature_convert(tempe_data);

        printf("|  %7.2f  |   %7.2f    | \n",press,tempe);
        
        nanosleep(&delay,NULL);
    }

    printf("\nExiting...\n");

    i2c_close(&bar_table);

    return 0;
}

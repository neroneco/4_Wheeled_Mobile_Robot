#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <signal.h>
#include <time.h>

#include "i2c_com.h"
#include "altimu-10-v5.h"


volatile static int run = 1;

static void SIGINT_handler(volatile int sig_num){
//    printf("\nOoops...:%d\n",sig_num);
//    printf("\nOoops...:\n");

    run = 0;
}


int main(int argc, char **argv){

    struct sigaction sa;

    sa.sa_handler = SIGINT_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    if (sigaction(SIGINT, &sa, NULL) == -1){
        printf("error");
    }
    uint8_t* buffer; 
    buffer = malloc(20);

    struct object* magn;

    magn = malloc(sizeof(struct object));

    /* Setting up device tables: */
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

    i2c_init(&magn_table);

    /* SETTING UP DEVICES: */
    set_up_LIS3MDL(magn_table,buffer);

    while (run) {

        /* RECEIVING DATA: */
        get_data_LIS3MDL(magn_table,buffer);

        //printf("|%5d|%5d|%5d|%5d|%5d|%5d|\n",
        //        buffer[0],
        //        buffer[1],
        //        buffer[2],
        //        buffer[3],
        //        buffer[4],
        //        buffer[5]
        //      );

        magn->raw[X].LSB = buffer[0];
        magn->raw[X].MSB = buffer[1];
        magn->raw[Y].LSB = buffer[2];
        magn->raw[Y].MSB = buffer[3];
        magn->raw[Z].LSB = buffer[4];
        magn->raw[Z].MSB = buffer[5];

        magn->values[X] = magn_convert(magn->raw[X]);
        magn->values[Y] = magn_convert(magn->raw[Y]);
        magn->values[Z] = magn_convert(magn->raw[Z]);
    
        /* PRINTING DATA: */
        printf("%.4f %.4f %.4f \n",magn->values[X], magn->values[Y], magn->values[Z]);

        nanosleep(&delay,NULL);
    }

    //printf("\nExiting...\n");
    i2c_close(&magn_table);
    free(buffer);
    free(magn);

    return 0;
}
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <signal.h>
#include <time.h>
#include <math.h>

#include "i2c_com.h"
#include "altimu-10-v5.h"

#define P_M      2.5709
#define ERR_X_M  0.0417
#define ERR_Y_M  0.2298
#define ERR_Z_M -1.4433

volatile static int run = 1;

static void SIGINT_handler(volatile int sig_num){
    run = 0;
}


int main(int argc, char **argv){

    struct sigaction sa;

    sa.sa_handler = SIGINT_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    if (sigaction(SIGINT, &sa, NULL) == -1)
        perror("sigaction");
    
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

    for(int i=0;i<2000;i++){

        /* RECEIVING DATA: */
        get_data_LIS3MDL(magn_table,buffer);

        magn->raw[X].LSB = buffer[0];
        magn->raw[X].MSB = buffer[1];
        magn->raw[Y].LSB = buffer[2];
        magn->raw[Y].MSB = buffer[3];
        magn->raw[Z].LSB = buffer[4];
        magn->raw[Z].MSB = buffer[5];

        magn->values[X] = P_M * (magn_convert(magn->raw[X]) - ERR_X_M);
        magn->values[Y] = P_M * (magn_convert(magn->raw[Y]) - ERR_Y_M);
        magn->values[Z] = P_M * (magn_convert(magn->raw[Z]) - ERR_Z_M);
		
        // magn->degrees[X] = vector_to_degree(magn->values[X],magn->values[Y],magn->values[Z]);
        // magn->degrees[Y] = vector_to_degree(magn->values[Y],magn->values[X],magn->values[Z]);
        // magn->degrees[Z] = vector_to_degree(magn->values[Z],magn->values[X],magn->values[Y]);
		
        magn->degrees[X] = (atan2f(magn->values[X],(sqrtf((magn->values[Z]*magn->values[Z])+(magn->values[Y]*magn->values[Y]))))/M_PI)*180;
        magn->degrees[Y] = (atan2f(magn->values[Y],(sqrtf((magn->values[Z]*magn->values[Z])+(magn->values[X]*magn->values[X]))))/M_PI)*180;
        magn->degrees[Z] = (atan2f(magn->values[Z],(sqrtf((magn->values[Y]*magn->values[Y])+(magn->values[X]*magn->values[X]))))/M_PI)*180;
		
		
        /* PRINTING DATA: */
        printf("%2.4f %2.4f %2.4f \n",magn->degrees[X], magn->degrees[Y], magn->degrees[Z]);

        nanosleep(&delay,NULL);
    }

    // printf("\nExiting...\n");

    i2c_close(&magn_table);
    free(buffer);
    free(magn);

    return 0;
}

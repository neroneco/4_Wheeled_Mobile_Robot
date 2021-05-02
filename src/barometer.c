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
    sa.sa_handler = SIGINT_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    if (sigaction(SIGINT, &sa, NULL) == -1)
        perror("sigaction");
        
    uint8_t buffer[10];

    struct i2c_table bar_table = {0,LPS25H_ADDRESS,"/dev/i2c-0"};
    

    struct data_24_bit press_data;
    struct data_16_bit tempe_data;
    float  press;
    float  tempe;

    /* Sets reading delay to 0.5 seconds: */
    struct timespec delay = {
                             .tv_sec=0,
                             .tv_nsec=500000000
                            };

    i2c_init(&bar_table);
    
    /* SETTING UP DEVICES: */
    set_up_LPS25H(bar_table,buffer); 

    printf("\n  Reading from LPS25H chip:\n"
           " __________________________  \n"
           "| Pressure: | Temperature: | \n"
           "|___________|______________| \n");

    while(run){
        /* RECEIVING DATA: */
        get_data_LPS25H(bar_table,buffer);

        press_data.LSB_L = buffer[0];
        press_data.LSB_H = buffer[1];
        press_data.MSB   = buffer[2];

        tempe_data.LSB = buffer[3];
        tempe_data.MSB = buffer[4];

        press = pressure_convert(   press_data);
        tempe = temperature_convert(tempe_data);

        /* PRINTING DATA: */
        printf("|  %7.2f  |   %7.2f    | \n",press,tempe);
        
        nanosleep(&delay,NULL);
    }

    printf("\nExiting...\n");

    i2c_close(&bar_table);

    return 0;
}

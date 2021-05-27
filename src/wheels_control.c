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
#include "nanohat_motor.h"


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
        
    uint8_t buffer[20];

    struct i2c_table motor_table = {0,PCA9685_ADDRESS,"/dev/i2c-0"};

    /* Sets reading delay to 0.5 seconds: */
    struct timespec delay = {
                             .tv_sec=0,
                             .tv_nsec=500000000
                            };

    i2c_init(&motor_table);
    
    /* SETTING UP DEVICES: */
    set_up_PCA9685(motor_table,buffer);
	
	//set_LED_ON_OFF_PCA9685(motor_table,buffer,LED6,ON);
	set_PWM_PCA9685(motor_table,buffer,LED6,MAX_PULSE_WIDTH/4);
	
	get_data_PCA9685(motor_table,buffer,LED6);
	printf("LED6_ON_L =  %d\n",buffer[0]);
	get_data_PCA9685(motor_table,buffer,LED6+1);
	printf("LED6_ON_H = %d\n",buffer[0]);
	get_data_PCA9685(motor_table,buffer,LED6+2);
	printf("LED6_OFF_L = %d\n",buffer[0]);
	get_data_PCA9685(motor_table,buffer,LED6+3);
	printf("LED6_OFF_H = %d\n",buffer[0]);

	// while(run){
	// 	nanosleep(&delay,NULL);
	// }
	
    printf("\nExiting...\n");

    i2c_close(&motor_table);

    return 0;
}

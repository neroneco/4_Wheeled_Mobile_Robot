#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <stdint.h>
#include <time.h>
#include <ncurses.h>
#include <linux/i2c-dev.h>

#include "i2c_com.h"
#include "opticSensor.h"

#define V_REF 	   3.3
#define RESOLUTION 256


volatile static int run = 1;
 
static void SIGINT_handler(volatile int sig_num){
    run = 0;
}

int main(int argc, char **argv){

    initscr();
    curs_set(0);

    struct sigaction sa;
    sa.sa_handler = SIGINT_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    if(sigaction(SIGINT, &sa, NULL) == -1){
        perror("sigaction");
    }

    struct timeval  tv;
    struct timezone tz;

	uint8_t* buffer; // initializing buffer for communication
    buffer = malloc(10);

	struct i2c_table sensor_table = {0, 0x48, "/dev/i2c-0"};

    struct timespec delay = {                   
                         .tv_sec  = 0,
                         .tv_nsec = 2000000
                        };

	buffer[0] = 0b00100001; // control byte: single-ended output on chanel AIN1

	i2c_init( &sensor_table);
	i2c_write(&sensor_table, buffer,1); // not sure if this is needed TODO: check if it is

	printw("\n Reading from the ADC:\n"
	       "                 _______\n");
    mvprintw(3,1, "optic sensor:  |       | [cm]\n");
    mvprintw(4,16,"|_______|\n");
	mvprintw(6,1, "time in [ms]: \n");
	
	float real_voltage_value;
	float distance;

    float time_now  = 0.0;
    float time_diff = 0.0;
    float time_past = 0.0;

	while(run){
		i2c_read(&sensor_table, buffer,1);
		real_voltage_value = (V_REF/RESOLUTION)*buffer[0];
		distance = voltage_to_distance(real_voltage_value);

        gettimeofday(&tv,&tz);
        time_now  = (((float)tv.tv_usec)/1000);
        time_diff = time_now-time_past;
        time_past = time_now;

        //mvprintw(6,10,"time in ms: %5.3f",time_in_micro);
        mvprintw(6,15,"%10.4f",time_diff);
        mvprintw(3,18,"%2.2f" ,distance );
        curs_set(0);
        refresh();
		nanosleep(&delay,NULL);
	}

    endwin();

	i2c_close(&sensor_table);

	return 0;
}

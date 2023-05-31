#include <stdint.h>
#include <stdio.h>

#include "i2c_com.h"
#include "nanohat_motor.h"

void set_up_PCA9685(   struct i2c_table tab, uint8_t* buf){
	buf[0] = MODE1;
	buf[1] = 0b00100001;

	i2c_write(&tab, buf, 2);
	
	buf[0] = MODE2;
	buf[1] = 0b00000100;

	i2c_write(&tab, buf, 2);
}

void set_PWM_PCA9685(  struct i2c_table tab, uint8_t* buf, uint8_t led_register, uint16_t pulse_width){
	buf[0] = led_register;
	buf[1] = 0x0;                                  // LED_ON_L
	buf[2] = 0x0&0b00001111;                       // LED_ON_H
	buf[3] = (uint8_t)(pulse_width);               // LED_OFF_L
	buf[4] = (uint8_t)(pulse_width>>8)&0b00001111; // LED_OFF_H

	i2c_write(&tab, buf, 5);
}

void set_LED_ON_OFF_PCA9685(struct i2c_table tab, uint8_t* buf, uint8_t led_register, int state){
	switch(state){
		case 0:
 			buf[0] = led_register + 3;
			buf[1] = 0b00010000;
			break;
		case 1:
 			buf[0] = led_register + 3;
			buf[1] = 0b00000000;
			i2c_write(&tab, buf, 2);
			
			buf[0] = led_register + 1;
			buf[1] = 0b00010000;
			break;
		default:
			printf("Something is wrong");
			break;
	}
	i2c_write(&tab, buf, 2);
}

void get_data_PCA9685(struct i2c_table tab, uint8_t* buf, uint8_t reg){
	buf[0] = reg;
	i2c_write(&tab, buf, 1);
	i2c_read(&tab, buf, 1);
}

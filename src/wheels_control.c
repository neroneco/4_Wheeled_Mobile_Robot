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

int main(int argc, char **argv)
{

	uint8_t buffer[20];

	struct i2c_table motor_table = {0, PCA9685_ADDRESS, "/dev/i2c-0"};

	i2c_init(&motor_table);

	int32_t power = 0;
	float L_power, R_power;
	int16_t X = 0;
	int16_t Y = 0;
	float power_max = 65536;
	float Y_max = 32768;
	float ratio = 0.35;
	float correction = ratio * 0.1;
	char read_buf[4] = {0};

	/* SETTING UP DEVICES: */
	set_up_PCA9685(motor_table, buffer);
	int status = 0;
	while (1)
	{

		if ((status = read(0, read_buf, 3) != -1))
		{

			// reading values from server
			if (read_buf[0] == 0x01)
			{
				power = (int32_t)((int16_t)(read_buf[1] + (read_buf[2] << 8)));
				power = power + 32768;
			}
			if (read_buf[0] == 0x02)
			{
				X = read_buf[1] + (read_buf[2] << 8);
				if (X > -7000 && X < 7000)
				{
					X = 0;
				}
			}
			if (read_buf[0] == 0x03)
			{
				Y = read_buf[1] + (read_buf[2] << 8);
				if (Y > -7000 && Y < 7000)
				{
					Y = 0;
				}
			}
			// -----

			// direction change
			if (X <= 0)
			{
				// M1
				set_LED_ON_OFF_PCA9685(motor_table, buffer, LED1, OFF);
				set_LED_ON_OFF_PCA9685(motor_table, buffer, LED2, ON);
				// M2
				set_LED_ON_OFF_PCA9685(motor_table, buffer, LED3, OFF);
				set_LED_ON_OFF_PCA9685(motor_table, buffer, LED4, ON);
				// M3
				set_LED_ON_OFF_PCA9685(motor_table, buffer, LED11, OFF);
				set_LED_ON_OFF_PCA9685(motor_table, buffer, LED12, ON);
				// M4
				set_LED_ON_OFF_PCA9685(motor_table, buffer, LED13, OFF);
				set_LED_ON_OFF_PCA9685(motor_table, buffer, LED14, ON);
			}
			if (X > 0)
			{
				// M1
				set_LED_ON_OFF_PCA9685(motor_table, buffer, LED1, ON);
				set_LED_ON_OFF_PCA9685(motor_table, buffer, LED2, OFF);
				// M2
				set_LED_ON_OFF_PCA9685(motor_table, buffer, LED3, ON);
				set_LED_ON_OFF_PCA9685(motor_table, buffer, LED4, OFF);
				// M3
				set_LED_ON_OFF_PCA9685(motor_table, buffer, LED11, ON);
				set_LED_ON_OFF_PCA9685(motor_table, buffer, LED12, OFF);
				// M4
				set_LED_ON_OFF_PCA9685(motor_table, buffer, LED13, ON);
				set_LED_ON_OFF_PCA9685(motor_table, buffer, LED14, OFF);
			}
			// -----

			// power change
			if (Y < 0)
			{
				R_power = (((float)power) / power_max);
				L_power = (((float)power) / power_max) + (((float)Y) / Y_max);
				if (L_power < 0)
				{
					L_power = 0;
				}
				//printf("R_power = %0.3f \n", R_power);
				// M1
				set_PWM_PCA9685(motor_table, buffer, LED0, MAX_PULSE_WIDTH * L_power * ratio);
				// M2
				set_PWM_PCA9685(motor_table, buffer, LED5, MAX_PULSE_WIDTH * L_power * ratio);
				// M3
				set_PWM_PCA9685(motor_table, buffer, LED10, MAX_PULSE_WIDTH * R_power * ratio);
				// M4
				set_PWM_PCA9685(motor_table, buffer, LED15, MAX_PULSE_WIDTH * R_power * ratio);
			}
			if (Y > 0)
			{
				L_power = (((float)power) / power_max);
				R_power = (((float)power) / power_max) - (((float)Y) / Y_max);
				if (R_power < 0)
				{
					R_power = 0;
				}
				// M1
				set_PWM_PCA9685(motor_table, buffer, LED0, MAX_PULSE_WIDTH * L_power * ratio);
				// M2
				set_PWM_PCA9685(motor_table, buffer, LED5, MAX_PULSE_WIDTH * L_power * ratio);
				// M3
				set_PWM_PCA9685(motor_table, buffer, LED10, MAX_PULSE_WIDTH * R_power * ratio);
				// M4
				set_PWM_PCA9685(motor_table, buffer, LED15, MAX_PULSE_WIDTH * R_power * ratio);
			}
			if (Y == 0)
			{
				L_power = (((float)power) / power_max);
				R_power = L_power;
				// M1
				set_PWM_PCA9685(motor_table, buffer, LED0, MAX_PULSE_WIDTH * L_power * (correction + ratio));
				// M2
				set_PWM_PCA9685(motor_table, buffer, LED5, MAX_PULSE_WIDTH * L_power * (correction + ratio));
				// M3
				set_PWM_PCA9685(motor_table, buffer, LED10, MAX_PULSE_WIDTH * R_power * ratio);
				// M4
				set_PWM_PCA9685(motor_table, buffer, LED15, MAX_PULSE_WIDTH * R_power * ratio);
			}
			// -----
			printf("power = %d, X = %d, Y = %d \n", power, X, Y);
		}
		usleep(1000);
	}
	
	i2c_close(&motor_table);

	return 0;
}

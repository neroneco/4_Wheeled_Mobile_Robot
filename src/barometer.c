#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <stdint.h>
#include <linux/i2c-dev.h>

#include "i2c_com.h"


int main(int argc, char **argv){

    uint8_t buffer[10];

    /*
     *  LPS25H   : addres = 0x5d  (barometer              )
     *  LIS3MDL  : addres = 0x1e  (magnetic sensor        )
     *  LSM6DS33 : addres = 0x6b  (accelerometer/gyroscope)
     */
    uint8_t address = 0x5d;

    struct i2c_table bar_table = {0,address,"/dev/i2c-0"};

    i2c_init(&bar_table);

    printf("Reading from Barometer:\n");
    printf("-----------------------\n");

    /*
     * In order to read particular value of register of Altimu-10 v5
     * we have to send one byte which is address of register
     */
    buffer[0] = 0x29 + (1<<7);    // MSB=1 then autoincrement

    i2c_write(&bar_table, buffer, 1);
    i2c_read( &bar_table, buffer, 2);

    printf("Value read:\n");
    printf("%d %d\n",buffer[0],buffer[1]);

    i2c_close(&bar_table);

    return 0;
}

#include <stdio.h>  // TODO: remove
#include <stdint.h>

#include "i2c_com.h"
#include "altimu-10-v5.h"


void set_up_LIS3MDL( struct i2c_table tab, uint8_t* buf){

}
void set_up_LPS25H(  struct i2c_table tab, uint8_t* buf){

}
void set_up_LSM6DS33(struct i2c_table tab, uint8_t* buf){
    // address of the devices register:
    buf[0] = CTRL2_G;

    // value to be written to register:
    buf[1] = 0b10000000; // set 416 Hz (high performance mode)

    i2c_write(&tab, buf, 2);
    
    buf[0] = CTRL3_C;
    buf[1] = 0b01000100; // set BDU(Block Data Update) bit and IF_INC(auto increment address) bit
    i2c_write(&tab, buf, 2);

    buf[0] = CTRL7_G;
    buf[1] = 0b00000000;
    i2c_write(&tab, buf, 2);
}



void get_data_LIS3MDL( struct i2c_table tab, uint8_t* buf){

}
void get_data_LPS25H(  struct i2c_table tab, uint8_t* buf){

}

void get_data_LSM6DS33(struct i2c_table tab, uint8_t* buf){
        buf[0] = OUT_G;
        i2c_write(&tab,buf,1);

        // reading 6 bytes of data corresponding to Pitch Roll Yaw values
        i2c_read( &tab,buf,6);
}



float pressure_convert(struct data_24_bit press){
    float converted_value;
    
    converted_value = (float)((int32_t)((press.MSB<<16) + (int32_t)(press.LSB_H<<8) + (int32_t)press.LSB_L))/4096;
    
    return converted_value;
}

float temperature_convert(struct data_16_bit tempe){
    float converted_value;

    converted_value = 42.5 + (float)((int16_t)((tempe.MSB<<8) + tempe.LSB))/480;

    return converted_value;
}

float dps_convert(struct data_16_bit dps){
    return (float)((((int8_t)dps.MSB)<<8)+(uint8_t)dps.LSB)*0.00875;
}

float dps_to_degree(float dps, int odr_hz){
    return dps*((float)1/odr_hz);
}


float acceleration_convert(struct data_16_bit accel){
    return 0;
}


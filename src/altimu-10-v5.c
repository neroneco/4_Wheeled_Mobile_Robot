#include <stdint.h>
#include <math.h>

#include "i2c_com.h"
#include "altimu-10-v5.h"


void set_up_LIS3MDL( struct i2c_table tab, uint8_t* buf){

}
void set_up_LPS25H(  struct i2c_table tab, uint8_t* buf){

}

static void set_up_LSM6DS33_gyro(struct i2c_table tab, uint8_t* buf){
    buf[0] = CTRL2_G;
    buf[1] = 0b01111100; // set 416 Hz (high performance mode) and 2000 dps rate seleciton
    i2c_write(&tab, buf, 2);
    
    buf[0] = CTRL7_G;
    buf[1] = 0b00000000;
    i2c_write(&tab, buf, 2);
}

static void set_up_LSM6DS33_acce(struct i2c_table tab, uint8_t* buf){
    buf[0] = CTRL1_XL;
    buf[1] = 0b01100100; // set 416 Hz (high performance mode) and 16g scale seleciton
    i2c_write(&tab, buf, 2);

}


void set_up_LSM6DS33(struct i2c_table tab, uint8_t* buf){
    buf[0] = CTRL3_C;
    buf[1] = 0b01000100; // set BDU(Block Data Update) bit and IF_INC(auto increment address) bit
    i2c_write(&tab, buf, 2);

    set_up_LSM6DS33_gyro(tab,buf);
    set_up_LSM6DS33_acce(tab,buf);
}

void get_data_LIS3MDL( struct i2c_table tab, uint8_t* buf){

}
void get_data_LPS25H(  struct i2c_table tab, uint8_t* buf){

}

void get_data_LSM6DS33(struct i2c_table tab, uint8_t* buf){
        buf[0] = OUT_G_A;
        i2c_write(&tab,buf,1);

        /* reading 12 bytes of data corresponding to Pitch Roll Yaw values
         * from gyro and accelaration values from accelerometer
         * (6 pairs of {MSB,LSB} )
         */
        i2c_read( &tab,buf,12);
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
    return (float)((((int8_t)dps.MSB)<<8)+(uint8_t)dps.LSB)*0.07;
}

float dps_to_degree(float dps, int odr_hz){
    return dps*((float)1/odr_hz);
}

float acce_convert(struct data_16_bit accel){
    return (float)((((int8_t)accel.MSB)<<8)+(uint8_t)accel.LSB)*0.488;
}

float acce_to_degree(float degree_1, float degree_2, float degree_3){
    float degree;

    if      (degree_1 > 0 && degree_2 > 0 && degree_3 > 0){

    }
    else if (degree_1 > 0 && degree_2 > 0 && degree_3 > 0){

    }
    else if (degree_1 < 0 && degree_2 > 0 && degree_3 > 0){

    }
    else if (degree_1 < 0 && degree_2 > 0 && degree_3 > 0){

    }


    //return atan2f(degree_2,(sqrtf((degree_1*degree_1)+(degree_3*degree_3))));
    return degree;
}

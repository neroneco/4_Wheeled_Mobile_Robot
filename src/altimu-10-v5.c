#include <stdint.h>
#include <math.h>

#include "i2c_com.h"
#include "altimu-10-v5.h"


/* SET UP */
void set_up_LIS3MDL( struct i2c_table tab, uint8_t* buf){
    buf[0] = CTRL_REG1_M;
    buf[1] = 0b01000010; // set 300 Hz (high performance mode on X,Y axis)
    i2c_write(&tab, buf, 2);

    buf[0] = CTRL_REG2_M;
    buf[1] = 0b01100000; // full scale selection +-16 gauss
    i2c_write(&tab, buf, 2);

    buf[0] = CTRL_REG3_M;
    buf[1] = 0b00000000; // continuouss mode selection
    i2c_write(&tab, buf, 2);

    buf[0] = CTRL_REG4_M;
    buf[1] = 0b00001000; // Z-axis high performance mode
    i2c_write(&tab, buf, 2);

    buf[0] = CTRL_REG5_M;
    buf[1] = 0b01000000; // set BDU bit (Block Data Update)
    i2c_write(&tab, buf, 2);
}

void set_up_LPS25H(  struct i2c_table tab, uint8_t* buf){ 
    //  buffer[0]:
    //  address of the devices register 
    buf[0] = CTRL_REG1_P;
    
    //  buffer[1]:
    //  7 -> bit turn on device
    //  6,5,4-bits -> set output data rate (1Hz,7H,12.5Hz,25Hz)
    //  3-bit -> BDU bit (prevents from reading LSB or MSB from different samples) 
    buf[1] = 0b10100100;
    i2c_write(&tab, buf, 2);
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


/* GET DATA */
void get_data_LIS3MDL( struct i2c_table tab, uint8_t* buf){
    buf[0] = OUT_M + (1<<7); // MSB=1 -> autoincrement
    i2c_write(&tab,buf,1);
    i2c_read( &tab,buf,6);
}

void get_data_LPS25H(  struct i2c_table tab, uint8_t* buf){
    buf[0] = PRESS_OUT + (1<<7); // MSB=1 -> autoincrement register address
    i2c_write(&tab, buf, 1);
    i2c_read( &tab, buf, 5);
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

void get_data_accelerometer(struct i2c_table tab, uint8_t* buf){
        buf[0] = OUT_A;
        i2c_write(&tab,buf,1);

        /* reading 6 bytes of data corresponding to Pitch Roll Yaw values
         * from accelerometer
         * (3 pairs of {MSB,LSB} )
         */
        i2c_read( &tab,buf,6);
}

/* CONVERSION FUNCIONS */
float pressure_convert(struct data_24_bit press){
    return ((float)((((int32_t)press.MSB)<<16) + (((int32_t)press.LSB_H)<<8) + (int32_t)press.LSB_L))/4096;
}

float temperature_convert(struct data_16_bit tempe){
    return 42.5 + (float)((int16_t)((tempe.MSB<<8) + tempe.LSB))/480;
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

float vector_to_degree(float vect_1, float vect_2, float vect_3){
    float degree;
    float rad = atan2f(vect_2,(sqrtf((vect_1*vect_1)+(vect_3*vect_3))));

    if      (vect_2 <= 0 && vect_3 <= 0){
        degree = (rad/M_PI)*180;
    }
    else if (vect_2 < 0  && vect_3 >= 0){
        degree = -180 - (rad/M_PI)*180;
    }
    else if (vect_2 >= 0 && vect_3  < 0){
        degree = (rad/M_PI)*180;
    }
    else if (vect_2 > 0  && vect_3 >= 0){
        degree = 180 - (rad/M_PI)*180;
    }

    return degree;
}

float magn_convert(struct data_16_bit magne){
    return ((float)((((int8_t)magne.MSB)<<8)+(uint8_t)magne.LSB))/1711;
}

void cross_product(float* vector_1,float* vector_2,float* result){
    result[0] = vector_1[1]*vector_2[2] - vector_1[2]*vector_2[1];
    result[1] = vector_1[2]*vector_2[0] - vector_1[0]*vector_2[2];
    result[2] = vector_1[0]*vector_2[1] - vector_1[1]*vector_2[0];
}


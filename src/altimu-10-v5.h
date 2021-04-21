/*
 * This fuctions implements all convertions of data recevied from Altimu-10-v5
 * controller to data that can be easly inteterpreted. It also implements 
 * definitions.
 * For example:
 * recieved  data: 0x00 0x15 <- raw data in hex
 * converted data:    21     <- tempreture in Celsius
 */

/*
 * Devives addresses:
 *  LIS3MDL  : address = 0x1e  (magnetic sensor         )
 *  LPS25H   : address = 0x5d  (barometer, thermometer  )
 *  LSM6DS33 : address = 0x6b  (accelerometer, gyroscope)
 */
#define LIS3MDL_ADDRESS  0x1e
#define LPS25H_ADDRESS   0x5d
#define LSM6DS33_ADDRESS 0x6b

// LSM6DS33 chip:
#define CTRL2_G 0x11
#define CTRL3_C 0x12
#define OUT_G   0x22

struct pressure{
    uint8_t LSB_L;
    uint8_t LSB_H;
    uint8_t MSB  ; 
};

struct temperature{
    uint8_t LSB;
    uint8_t MSB  ; 
};

float pressure_convert(struct pressure press);
float temperature_convert(struct temperature tempe);


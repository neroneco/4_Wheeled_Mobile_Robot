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
#define CTRL3_C 0x12
#define OUT_G_A 0x22

//   >gyroscope:
#define CTRL2_G 0x11
#define CTRL7_G 0x16

//   >accelerometer:  
#define CTRL1_XL 0x10


#define ODR_HZ  20 // Output Data Rating (in Hz)

struct data_24_bit{
    uint8_t LSB_L;
    uint8_t LSB_H;
    uint8_t MSB  ; 
};

struct data_16_bit{
    uint8_t LSB;
    uint8_t MSB; 
};

enum coor{
    X,
    Y,
    Z,
};

struct object{
    float values[3];
    float degrees[3];
    struct data_16_bit raw[3];
};


void set_up_LIS3MDL( struct i2c_table tab, uint8_t* buf);
void set_up_LPS25H(  struct i2c_table tab, uint8_t* buf);
void set_up_LSM6DS33(struct i2c_table tab, uint8_t* buf);

void get_data_LIS3MDL( struct i2c_table tab, uint8_t* buf);
void get_data_LPS25H(  struct i2c_table tab, uint8_t* buf);
void get_data_LSM6DS33(struct i2c_table tab, uint8_t* buf);

float pressure_convert(    struct data_24_bit press );
float temperature_convert( struct data_16_bit tempe );
float dps_convert(         struct data_16_bit dps   );
float acce_convert(        struct data_16_bit accel );
float dps_to_degree(       float dps, int odr_hz    );
float acce_to_degree(float degree_1, float degree_2, float degree_3);


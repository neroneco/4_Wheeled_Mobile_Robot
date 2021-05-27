// Functions and definitions to
// communicate with nanohat motor module

// Led registers:
//   >documentation:
#define LED0_ON_L   0x06
#define LED0_ON_H   0x07
#define LED0_OFF_L  0x08   
#define LED0_OFF_H  0x09   
#define LED1_ON_L   0x0a   
#define LED1_ON_H   0x0b   
#define LED1_OFF_L  0x0c   
#define LED1_OFF_H  0x0d   
#define LED2_ON_L   0x0e  
#define LED2_ON_H   0x0f   
#define LED2_OFF_L  0x10   
#define LED2_OFF_H  0x11   
#define LED3_ON_L   0x12   
#define LED3_ON_H   0x13   
#define LED3_OFF_L  0x14   
#define LED3_OFF_H  0x15
#define LED4_ON_L   0x16  
#define LED4_ON_H   0x17   
#define LED4_OFF_L  0x18
#define LED4_OFF_H  0x19   
#define LED5_ON_L   0x1a   
#define LED5_ON_H   0x1b   
#define LED5_OFF_L  0x1c   
#define LED5_OFF_H  0x1d
#define LED6_ON_L   0x1e  
#define LED6_ON_H   0x1f   
#define LED6_OFF_L  0x20   
#define LED6_OFF_H  0x21   
#define LED7_ON_L   0x22   
#define LED7_ON_H   0x23   
#define LED7_OFF_L  0x24   
#define LED7_OFF_H  0x25
#define LED8_ON_L   0x26  
#define LED8_ON_H   0x27   
#define LED8_OFF_L  0x28   
#define LED8_OFF_H  0x29
#define LED9_ON_L   0x2a  
#define LED9_ON_H   0x2b   
#define LED9_OFF_L  0x2c   
#define LED9_OFF_H  0x2d
#define LED10_ON_L  0x2e  
#define LED10_ON_H  0x2f   
#define LED10_OFF_L 0x30   
#define LED10_OFF_H 0x31   
#define LED11_ON_L  0x32   
#define LED11_ON_H  0x33   
#define LED11_OFF_L 0x34   
#define LED11_OFF_H 0x35   
#define LED12_ON_L  0x36  
#define LED12_ON_H  0x37   
#define LED12_OFF_L 0x38   
#define LED12_OFF_H 0x39   
#define LED13_ON_L  0x3a   
#define LED13_ON_H  0x3b   
#define LED13_OFF_L 0x3c   
#define LED13_OFF_H 0x3d
#define LED14_ON_L  0x3e  
#define LED14_ON_H  0x3f   
#define LED14_OFF_L 0x40   
#define LED14_OFF_H 0x41   
#define LED15_ON_L  0x42   
#define LED15_ON_H  0x43   
#define LED15_OFF_L 0x44   
#define LED15_OFF_H 0x45
//   >alias:
#define LED0    0x06
#define LED1    0x0a
#define LED2    0x0e
#define LED3    0x12
#define LED4    0x16
#define LED5    0x1a
#define LED6    0x1e
#define LED7    0x22
#define LED8    0x26
#define LED9    0x2a
#define LED10   0x2e
#define LED11   0x32
#define LED12   0x36
#define LED13   0x3a
#define LED14   0x3e
#define LED15   0x42

// Control registers:
#define MODE1       0x00   
#define MODE2       0x01
#define PRE_SCALE   0xfe // default 200 Hz

// Pulse Width Modulation:
#define MAX_PULSE_WIDTH   4095
#define ON  1
#define OFF 0

// Device address:
#define PCA9685_ADDRESS 0x70 // TODO: find this address

// Functions to communicate with nanohat motor:
void set_up_PCA9685(struct i2c_table tab, uint8_t* buf);
void set_PWM_PCA9685(struct i2c_table tab, uint8_t* buf, uint8_t led_register, uint16_t pulse_width);
void set_LED_ON_OFF_PCA9685(struct i2c_table tab, uint8_t* buf, uint8_t led_register, int state);
void get_data_PCA9685(struct i2c_table tab, uint8_t* buf, uint8_t reg);

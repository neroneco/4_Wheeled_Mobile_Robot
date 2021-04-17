/*
 * This fuctions implements all convertions of data recevied from Altimu-10-v5
 * controller to data that can be easly inteterpreted.
 * For example:
 * recieved  data: 0x00 0x15 <- raw data in hex
 * converted data:    21     <- tempreture in Celsius
 */

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


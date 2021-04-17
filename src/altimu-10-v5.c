#include <stdint.h>

#include "altimu-10-v5.h"



float pressure_convert(struct pressure press){
    float converted_value;
    
    converted_value = (float)((press.MSB<<16) + (press.LSB_H<<8) + press.LSB_L)/4096;
    
    return converted_value;
}

float temperature_convert(struct temperature tempe){
    float converted_value;

    converted_value = 42.5 + (float)((int16_t)((tempe.MSB<<8) + tempe.LSB))/480;

    return converted_value;
}
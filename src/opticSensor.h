// structures and functions for analog optic sensor and ADC:
struct vol_to_dis_table{
    float  voltage[17];
    float distance[17];
};

float voltage_to_distance(float voltage);
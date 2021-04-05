// Communication structures and functions:
struct optic_sens_table{
    int  file;
    int  addr;             // specifying address of communication
    char filename[10];     // specifying filename of the device
};

int optic_sens_init( struct optic_sens_table* table);
int optic_sens_write(struct optic_sens_table* table, uint8_t* buf);
int optic_sens_read( struct optic_sens_table* table, uint8_t* buf);
int optic_sens_close(struct optic_sens_table* table);


// Other structures and functions:
struct vol_to_dis_table{
    float voltage[17];
    float distance[17];
};

float voltage_to_distance(float voltage);
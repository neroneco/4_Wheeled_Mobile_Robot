// Communication structures and functions:
struct i2c_table{
    int  file;             // file handle
    int  addr;             // specifying address of communication (can be checked by i2cdetect command)
    char filename[10];     // specifying filename of the device
};

int i2c_init( struct i2c_table* table);                  // initializing device communication
int i2c_write(struct i2c_table* table, uint8_t* buf);    // writing to device
int i2c_read( struct i2c_table* table, uint8_t* buf);    // reading from device
int i2c_close(struct i2c_table* table);                  // closing communication with device

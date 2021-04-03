
struct optic_sens_table{
    int  file;
    int  addr;             // specifying address of communication
    char filename[10];     // specifying filename of the device
};

int optic_sens_init(struct optic_sens_table* table);
int optic_sens_write(struct optic_sens_table* table, char buf);
int optic_sens_read(struct optic_sens_table* table, char buf);
int optic_sens_close(struct optic_sens_table* table);

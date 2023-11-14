#define IO_I2C_SDA 6
#define IO_I2C_SCL 7
#define IO_I2C_CH i2c1

typedef struct _t_data {
	int humidity;
	float celsius;	
} t_data;

void init_io(void);
int check_battery(void);
t_data* get_t_data(void);

/*
   This file is provided under the LGPL license ver 2.1
   https://github.com/kmorimatsu
*/

#include <stdlib.h>
#include <math.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/i2c.h"
#include "io.h"

void init_io(void){
	// Init ADC
	adc_init();
	adc_gpio_init(29);
	adc_select_input(3);
	// Init I2C
	i2c_init(IO_I2C_CH, 100000); 
	gpio_set_function(IO_I2C_SDA, GPIO_FUNC_I2C); 
	gpio_set_function(IO_I2C_SCL, GPIO_FUNC_I2C); 
	gpio_pull_up(IO_I2C_SDA); 
	gpio_pull_up(IO_I2C_SCL);
}

int check_battery(void){
	// Read ADC for battery
	// 4.0V: 1655, 2.5V: 1034
	// 1530-     : 5
	// 1406-1529 : 4
	// 1282-1405 : 3
	// 1158-1281 : 2
	// 1034-1157 : 1
	//     -1033 : 0
	int b=(adc_read()-910)/124;
	if (5<b) b=5;
	if (b<0) b=0;
	return b;
}

/*
	Temperature: T degree-C
	Saturated water vapor pressure: e(T) hPa
	e(T)=6.1078 x 10 ^ (7.5 x T / (T + 237.3))
	https://ja.wikipedia.org/wiki/%E9%A3%BD%E5%92%8C%E6%B0%B4%E8%92%B8%E6%B0%97%E9%87%8F

	Sprung's formula:
	e=e_sw - A x p x (Td - Tw)
	, where
		e: water vapor partial pressure
		e_sw: saturated water vapor pressure of wet bulb
		A=0.000662 (K^-1)
		p: atmospheric pressure (=1013.25 hPa)
		Td: temperature of dry bulb
		Tw: temperature of wet bulb
	https://ja.wikipedia.org/wiki/%E4%B9%BE%E6%B9%BF%E8%A8%88
*/

int humidity(float Td, float Tw){
	if (Td>=Tw) {
		float e_sd=6.1078 * pow(10.0, 7.5 * Td / (Td + 237.3));
		float e_sw=6.1078 * pow(10.0, 7.5 * Tw / (Tw + 237.3));
		float e=e_sw-0.67077 * (Td-Tw); // 0.67077 = 0.000662 * 1013.25
		return (int)(100.0*e/e_sd);
	} else {
		// Dry bulb and wet bub are opposite
		float e_sd=6.1078 * pow(10.0, 7.5 * Tw / (Tw + 237.3));
		float e_sw=6.1078 * pow(10.0, 7.5 * Td / (Td + 237.3));
		float e=e_sw-0.67077 * (Tw-Td); // 0.67077 = 0.000662 * 1013.25
		return (int)(100.0*e/e_sd);
	}
}

t_data* get_t_data(void){
	static t_data tdat;
	int i;
	unsigned char d[2];
	float Td, Tw;
	// Set registers to Temperature
	d[0]=0x05;
	i=i2c_write_blocking(IO_I2C_CH,0x1f,d,1,false);
	i=i2c_write_blocking(IO_I2C_CH,0x1b,d,1,false);
	// Bulb 1 shows 0.1 degree lower temperature
	i2c_read_blocking(IO_I2C_CH,0x1f,d,2,false);
	i=((d[0]&0xf)<<8)|d[1];
	i=10*i/15;
	Td=((float)i)/10+0.1; // Note "+0.1" here depends on each MCP9808 chip characteristic
	// Bulb 2 shows 0.1 degree higher temperature
	i2c_read_blocking(IO_I2C_CH,0x1b,d,2,false);
	i=((d[0]&0xf)<<8)|d[1];
	i=10*i/15;
	Tw=((float)i)/10-0.1; // Note "-0.1" here depends on each MCP9808 chip characteristic
	// Calculate humidity
	tdat.humidity=humidity(Td,Tw);
	// Set temperature
	tdat.celsius=Td>Tw ? Td:Tw;
	return &tdat;
}

/*
   This file is provided under the LGPL license ver 2.1
   https://github.com/kmorimatsu
*/

#include <stdlib.h>
#include "pico/stdlib.h"
#include "flash_counter.h"
#include "io.h"

int EPD_2in13_V4_draw(char battery, int humidity, float celsius, bool clear);

int main(void){
	int i;
	stdio_init_all();
	
	// LED ON
	gpio_init(1);
	gpio_set_dir(1, GPIO_OUT);
	gpio_put(1, 1);
	// Reset power latch
	gpio_init(2);
	gpio_put(2,1);
	gpio_set_dir(2, GPIO_OUT);
	gpio_init(3);
	gpio_put(3,1);
	gpio_set_dir(3, GPIO_OUT);
	
	i=read_flash_counter();
	increment_flash_counter();

	// LED OFF
	gpio_put(1, 0);

	if (0==(i&7)) {
		init_io();
		sleep_ms(500);
		t_data* tdat=get_t_data();
		i=EPD_2in13_V4_draw(check_battery(),tdat->humidity,tdat->celsius,false);
	} else {
		sleep_ms(500);
		i=0;
	}
	
	// Power off by latch
	gpio_init(3);
	gpio_put(2,0);
	sleep_us(100);
	gpio_init(2);

	return i;
}

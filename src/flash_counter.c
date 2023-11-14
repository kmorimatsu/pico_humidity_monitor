/*
   This file is provided under the LGPL license ver 2.1
   https://github.com/kmorimatsu
*/

#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/flash.h"
#include "flash_counter.h"

// Local variables
static uint8_t* flash_data=(uint8_t *) (XIP_BASE + FLASH_TARGET_OFFSET);
static int currenr_flash_counter=-1;

static void program_flash_value(int val){
	// Sector size for writing is 256
	uint8_t write_data[256];
	int i;
	if (val<0 || 32768<=val) {
		// Used all area. Let's erase 4K bytes area
		reset_flash_counter();
		return;		
	}
	if (0==(val&2047) && 0<val) {
		// Special case for programming
		// Fill zero to entire buffer
		for(i=0;i<255;i++) write_data[i]=0;
		// Program all zero buffer into the one previous section
		val-=2048; 
	} else {
		// Fill zero to specific point
		for(i=0;i<((val>>3)&255);i++) write_data[i]=0;
		// Fill value to specific point
		switch(val&7){
			case 0:
				write_data[i]=0xff;
				break;
			case 1:
				write_data[i]=0xfe;
				break;
			case 2:
				write_data[i]=0xfc;
				break;
			case 3:
				write_data[i]=0xf8;
				break;
			case 4:
				write_data[i]=0xf0;
				break;
			case 5:
				write_data[i]=0xe0;
				break;
			case 6:
				write_data[i]=0xc0;
				break;
			default:
				write_data[i]=0x80;
				break;
		}
		// Fill 0xff to the end
		for(i=i+1;i<256;i++) write_data[i]=0xff;
	}
	// Program flash
	asm("cpsid i");
	flash_range_program(FLASH_TARGET_OFFSET+((val>>3)&0xFF00), write_data, 256);
	asm("cpsie i");
}

int read_flash_counter(){
	int res=0;
	int i;
	if (0<=currenr_flash_counter) return currenr_flash_counter;
	// Reach to non zero value
	for(i=0;i<FLASH_SECTOR_SIZE;i++){
		if (0!=flash_data[i]) break;
	}
	if (FLASH_SECTOR_SIZE<=i) {
		// All 4096 bytes contain zero bits
		// Erase entire range and return value again.
		reset_flash_counter();
		return read_flash_counter();
	}
	res=i;
	// Check remaining bytes (must be 0xff)
	for(i=res+1;i<FLASH_SECTOR_SIZE;i++){
		if (0xff==flash_data[i]) continue;
		// The flash area has not yet been initialized
		// Erase entire range and return value again.
		reset_flash_counter();
		return read_flash_counter();
	}
	// Read current value
	switch(flash_data[res]){
		case 0xff:
			res=(res<<3);
			break;
		case 0xfe:
			res=(res<<3)+1;
			break;
		case 0xfc:
			res=(res<<3)+2;
			break;
		case 0xf8:
			res=(res<<3)+3;
			break;
		case 0xf0:
			res=(res<<3)+4;
			break;
		case 0xE0:
			res=(res<<3)+5;
			break;
		case 0xC0:
			res=(res<<3)+6;
			break;
		case 0x80:
			res=(res<<3)+7;
			break;
		default:
			// The flash area has not yet been initialized
			// Erase entire range and return value again.
			reset_flash_counter();
			return read_flash_counter();
	}
	currenr_flash_counter=res;
	return res;
}

void reset_flash_counter(){
	asm("cpsid i");
	flash_range_erase(FLASH_TARGET_OFFSET, FLASH_SECTOR_SIZE);
	asm("cpsie i");
	currenr_flash_counter=0;
}
void increment_flash_counter(){
	if (currenr_flash_counter<0) currenr_flash_counter=read_flash_counter();
	currenr_flash_counter++;
	program_flash_value(currenr_flash_counter);
}

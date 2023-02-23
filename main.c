/* main.c
 * This file written 2023 by Jonathan Johnson
 */
#include <stdint.h>
#include <assert.h>
#include "tetris.h"
#include "mipslab.h"

int main() {
	uint8_t data[5];
	uint8_t data2[10];
	uint8_t address[2];
	address[0]=0x00;
	address[1]=0xF0;
	i2c_write("LOLHEJ", 10, address);
	i2c_read(data2, 10, address);
	display_string(1, "lolma");
	display_update();
	//i2c_read(data2, 1, address);
	display_string(3, data2);
	display_update();
	
	while(1){
		;
	}
	return 0;
}

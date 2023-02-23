/* stubs.c
 * This file written 2015 by Axel Isaksson
 * This copyright notice added 2015 by F Lundevall
 * 
 * Modified 2023 by Jonathan Johnson

 * For copyright and licensing, see file COPYING */

#include <stdint.h>
#include "mipslab.h"
#include "pic32mx.h"
#include "tetris.h"


#define assert(x) ((x) ? (void)0 : __assert_fail(#x, __FILE__, __LINE__))

/* Non-Maskable Interrupt; something bad likely happened, so hang */
void _nmi_handler() {
	for(;;);
}

/* This function is called upon reset, before .data and .bss is set up */
void _on_reset() {
	
}

/* This function is called before main() is called, you can do setup here */
void _on_bootstrap() {
	setup();
	init_game();
}

void __assert_fail(char *a, char *b, unsigned int c) {
	/*TODO*/
	display_string(0, a);
	display_string(1, b);
	display_string(2, itoaconv(c));
	//display_string(3, "stopping");
	display_update();
	for(;;);
}

/* by Jonathan Johnson
 * calls the game loop on timer interrupt*/
void user_isr(){

	if(IFS(0) & 0x100){      //timer 2 interrupt
		IFSCLR(0) = 0x100;
		gameloop();
	}

}

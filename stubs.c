/* stubs.c
 * This file written 2015 by Axel Isaksson
 * This copyright notice added 2015 by F Lundevall
 * 
 * Modified 2023 by Jonathan Johnson

 * For copyright and licensing, see file COPYING */

#include <stdint.h>
#include "mipslab.h"
#include "pic32mx.h"
#include "tetris_stubs.h"

/* Non-Maskable Interrupt; something bad likely happened, so hang */
void _nmi_handler() {
	for(;;);
}

/* This function is called upon reset, before .data and .bss is set up */
void _on_reset() {
	
}

/* This function is called before main() is called, you can do setup here */
/* contents added by Jonathan Johnson*/
void _on_bootstrap() {
	setup();
	init_game();
	reset_highscore();

	/*we do this last, to make sure our setup will stay uninterrupted.*/
	enable_interrupts();	
}

/*
*	By Jonathan Johnson
*	catches failed assert()'s
*	and displays some info they're carrying
*	NOTE: since we're using interrupts,
*	this is not guaranteed to halt the program.
*/
void __assert_fail(char *a, char *b, unsigned int c) {
	display_string(0, a);
	display_string(1, b);
	display_string(2, itoaconv(c));
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

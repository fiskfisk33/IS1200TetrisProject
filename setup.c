/* setup.c
 *
 * This file written 2023 by Jonathan Johnson
 * code in many sections written 2015 by Axel Isaksson,
 * and modified 2015, 2017 by F Lundevall
 *
 * For copyright and licensing, see file COPYING */

#include <pic32mx.h>
#include <stdint.h>
#include "tetris.h"
#include "mipslab.h"

void displaysetup();
void timer2setup();

/*
* This function is called from stubs.c at startup
*/
void setup(){

        /*
          Following section written by Fredrik Lundevall.
	  This will set the peripheral bus clock to the same frequency
	  as the sysclock. That means 80 MHz, when the microcontroller
	  is running at 80 MHz. Changed 2017, as recommended by Axel.
	*/
	SYSKEY = 0xAA996655;  /* Unlock OSCCON, step 1 */
	SYSKEY = 0x556699AA;  /* Unlock OSCCON, step 2 */
	while(OSCCON & (1 << 21)); /* Wait until PBDIV ready */
	OSCCONCLR = 0x180000; /* clear PBDIV bit <0,1> */
	while(OSCCON & (1 << 21));  /* Wait until PBDIV ready */
	SYSKEY = 0x0;  /* Lock OSCCON */

        displaysetup();

	display_string(0, "Score:");
	display_string(1, "1600000");
	display_string(2, "       ");
	display_string(3, "Lvl: 5");
	display_update();
        display_image(96, icon2);

	area_print_test();
	
	timer2setup();
	enable_interrupts();	
        
}

/*
* this function is made up of code
*    written 2015 by Axel Isaksson,
*    modified 2015, 2017 by F Lundevall
*
*   Latest update 2017-04-21 by F Lundevall
*
*   For copyright and licensing, see file COPYING */
void displaysetup(){
        	/* Set up output pins */
	AD1PCFG = 0xFFFF;
	ODCE = 0x0;
	TRISECLR = 0xFF;
	PORTE = 0x0;
	
	/* Output pins for display signals */
	PORTF = 0xFFFF;
	PORTG = (1 << 9);
	ODCF = 0x0;
	ODCG = 0x0;
	TRISFCLR = 0x70;
	TRISGCLR = 0x200;
	
	/* Set up input pins */
	TRISDSET = (1 << 8);
	TRISFSET = (1 << 1);
	
	/* Set up SPI as master */
	SPI2CON = 0;
	SPI2BRG = 4;
	/* SPI2STAT bit SPIROV = 0; */
	SPI2STATCLR = 0x40;
	/* SPI2CON bit CKP = 1; */
        SPI2CONSET = 0x40;
	/* SPI2CON bit MSTEN = 1; */
	SPI2CONSET = 0x20;
	/* SPI2CON bit ON = 1; */
	SPI2CONSET = 0x8000;
	
	display_init();
	display_string(0, "KTH/ICT lab");
	display_string(1, "in Computer");
	display_string(2, "Engineering");
	display_string(3, "Welcome!");
	display_update();
	
	//display_image(96, icon);
}

void timer2setup(){
	//TODO

	T2CON = 0x0;    //Stop the timer and clear control;
        PR2 = 0x5208;   //period 31250
        TMR2 = 0x0;     //clear the timer register
        
        IPCSET(2) = 0x0D;  //timer 2 interrupt priority 
                         
 
        IFSCLR(0) = 0x100; //timer 2 interrupt flag clear
        IECSET(0) = 0x100; //interrupt  enable

        T2CON = 0x0C070; // init/start timer 2;
}
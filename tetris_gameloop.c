/* tetris_gameloop.c
 * This file written 2023 by Jonathan Johnson
 */
void *stdout; //stupid problems crave stupid solutions

#include <stdint.h>
#include <string.h>
#include <pic32mx.h>
#include <assert.h>
#include <stdlib.h>
#include "tetris.h"
#include "tetris_state.h"
#include "mipslab.h"

/* Check Buttons
 * Sets active buttons in state variable
 *
 *      also saves array of changed buttons
 */
void get_buttons(char *btns, char *btns_changed){
        char new_buttons[4] = {0};

	new_buttons[3] = (PORTD & 0x080) > 0;
	new_buttons[2] = -((PORTD & 0x040) > 0);
	new_buttons[1] = (PORTD & 0x020) > 0;
	new_buttons[0] = (PORTF & 0x2) > 0;
        for(int i = 0; i < 4; i++){
                btns_changed[i] = new_buttons[i] != state.buttons[i];
                btns[i] = new_buttons[i];
        }
}

/*
*       this function is what "powers the game"
*       it is called ~60 times a second
*       by a timer interrupt (timer 2)
*/
void gameloop(){
        frame_counter++;
        get_buttons(state.buttons, state.buttons_changed);
        
        if (state.state == MENU_MAIN){
                menu_main();
                return;
        }else if(state.state == MENU_HIGHSCORE){
                menu_high_score();
                return;

        }else if (state.state == GAME_OVER){
                menu_game_over();
                return;
        }else if(state.state == HIGH_SCORE){
                menu_new_high_score();
                return;

        }else if (state.state == PLAYING){
                tetris_playing();
                return;
        }
}


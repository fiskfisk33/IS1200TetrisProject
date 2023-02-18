/* gameloop.c
 * This file written 2023 by Jonathan Johnson
 */
#include <stdint.h>
#include <string.h>
#include <pic32mx.h>
#include "tetris.h"

uint8_t game_area[20][10];
int tetr_x;
int tetr_y;
/*

*/

int blocked_down(){
        //TODO
        //check if tetramino is blocked downwards
        return 0;
}

int blocked_left(){
        //TODO
        //check if tetramino is blocked leftwards
        return 0;
}

int blocked_right(){
        //TODO
        //check if tetramino is blocked rightwards
        return 0;
}

void tetromino_rotate(char direction){
        //TODO
        //basic NES style rotation
        //rotation works else no rotation
        
        return;
}

void tetromino_drop(){
        //TODO
}

void tetromino_move(){
        //TODO
}

int row_full(char row){
        //TODO        
}

void delete_row(){
        //TODO
}

int full_rows_count(){
        //TODO
}

void init_game(){
                //zero the game area
        //TODO
        memset(game_area, 0, 220);
        tetr_x = 22; 
        tetr_y = 4;
}
void gameloop(){
        //TODO


        //check if blocked left
        //move left
        //otherwise check if blocked right
        //move right

        //check if time to drop (counter)
        //check if blocked downwards
        //drop

        return;
}
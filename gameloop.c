/* gameloop.c
 * This file written 2023 by Jonathan Johnson
 */
#include <stdint.h>
#include <string.h>
#include <pic32mx.h>
#include "tetris.h"
#include "tetromino.h"

#define GAME_HEIGHT 22
#define GAME_WIDTH 10

uint8_t game_area[GAME_HEIGHT][GAME_WIDTH];
int tetr_x;
int tetr_y;
int tetr_rot;
char tetr_type;
char next_tetr_type;
/*

*/

/*
 * checks if the block fits at the given position
 * returns 1 if blocked
*/
int tetr_blocked(int x, int y, int rot){
        //TODO test
        for(int i = 0; i <= 16; i++){
                char block = get_tetromino_tile(tetr_type, i, -i, tetr_rot);
                if ( block &&
                    ( game_area[tetr_y-i][tetr_x+i] || tetr_y-i < 0 || tetr_x+i < 0 || tetr_x+i > GAME_WIDTH)) {
                        return 1;
                }
        } 

        return 0;
}

int row_full(uint8_t area[GAME_HEIGHT][GAME_WIDTH], int row){
        //TODO test   
        for(int i = 0; i <= GAME_WIDTH; i++)
                if(!area[row][i])
                        return 0;
        return 1;

}

int full_rows_count(){
        //TODO
}

void delete_row(){
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
        //

        //check if blocked left
        //move left
        //otherwise check if blocked right
        //move right

        //check if time to drop (counter)
        //check if blocked downwards
        //drop

        return;
}
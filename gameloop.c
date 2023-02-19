/* gameloop.c
 * This file written 2023 by Jonathan Johnson
 */
#include <stdint.h>
#include <string.h>
#include <pic32mx.h>
#include "tetris.h"
#include "tetromino.h"
#include "mipslab.h"

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



void print_area(uint8_t area[GAME_HEIGHT][GAME_WIDTH]){
        int size = 64*4;
        uint8_t print_data[size];
        memset(print_data, 0, size);
        for(int i = 0; i < size/4; i++){
                uint8_t b0 = !area[i/3 + 1][7];
                uint8_t b1 = !area[i/3 + 1][8];
                uint8_t b2 = !area[i/3 + 1][9];
        
                print_data[i] = b0<<7|b1<<6|b1<<5|b1<<4|b2<<3|b2<<2|b2<<1| 0<<0;
                if (i%3 == 0)
                        print_data[i] = print_data[i] | 0b01001000; 
                if (i >= 63)
                        print_data[i] = 0;

        } 
        for(int i = 0; i < size/4; i++){
                uint8_t b0 = !area[i/3 + 1][5];
                uint8_t b1 = !area[i/3 + 1][6];
                uint8_t b2 = !area[i/3 + 1][7];
        
                print_data[i + size/4] = b0<<7|b0<<6|b0<<5|b1<<4|b1<<3|b1<<2|b2<<1|b2<<0;
                if (i%3 == 0)
                        print_data[i + size/4] = print_data[i + size/4] | 0b10010010;
                if (i >= 63)
                        print_data[i + size/4] = 0;

        } 
        for(int i = 0; i < size/4; i++){
                uint8_t b0 = !area[i/3 + 1][2];
                uint8_t b1 = !area[i/3 + 1][3];
                uint8_t b2 = !area[i/3 + 1][4];
        
                print_data[i + 2*size/4] = b0<<7|b0<<6|b1<<5|b1<<4|b1<<3|b2<<2|b2<<1|b2<<0;
                if (i%3 == 0)
                        print_data[i + 2*size/4] = print_data[i + 2*size/4] | 0b00100100; 
                if (i >= 63)
                        print_data[i + 2*size/4] = 0;

        } 
        for(int i = 0; i < size/4; i++){
                uint8_t b0 = !area[i/3 + 1][0];
                uint8_t b1 = !area[i/3 + 1][1];
                uint8_t b2 = !area[i/3 + 1][2];
        
                print_data[i + 3*size/4] = 0<<7|b0<<6|b0<<5|b0<<4|b1<<3|b1<<2|b1<<1|b2<<0;
                if (i%3 == 0)
                        print_data[i + 3*size/4] = print_data[i + 3*size/4] | 0b01001001;
                if (i >= 63)
                        print_data[i + 3*size/4] = 0;

        } 
        display_playfield(68, print_data);

// 1111 1111 | 1111 1111 | 1111 1111 | 1111 1111
// I000 1112 | 2233 3444 | 5556 6677 | 7888 999I
}



void area_print_test(){
        uint8_t garea[22][10] = {{0,0,0,0,1,0,0,0,0,0},
                             {0,0,0,0,0,0,0,0,0,0},
                             {0,0,0,0,0,0,0,0,1,1},
                             {0,0,0,0,0,0,0,1,1,1},
                             {0,0,0,0,0,0,0,0,0,0},
                             {0,0,0,0,0,0,0,0,0,0},
                             {0,0,0,0,0,0,0,0,0,0},
                             {0,0,0,0,0,0,0,0,0,0},
                             {0,0,0,0,0,1,0,0,0,0},
                             {0,0,0,0,0,1,0,0,0,0},
                             {0,1,0,0,1,1,0,0,0,0},
                             {1,1,0,0,0,0,0,0,0,0},
                             {0,0,1,1,0,0,0,0,0,0},
                             {0,0,1,1,0,0,0,0,0,0},
                             {0,0,0,0,0,0,0,0,0,0},
                             {0,0,0,0,0,0,0,0,0,0},
                             {0,0,0,0,0,0,0,0,0,0},
                             {0,0,1,1,1,1,0,0,0,0},
                             {0,0,0,0,0,0,0,0,0,0},
                             {0,0,0,0,0,0,0,0,0,0},
                             {0,0,0,0,0,0,0,0,0,0},
                             {0,1,0,1,0,1,0,1,0,1}};
        print_area(garea);
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
        //game_area[5][5] = !game_area[5][5];

        //check if blocked left
        //move left
        //otherwise check if blocked right
        //move right

        //check if time to drop (counter)
        //check if blocked downwards
        //drop

        return;
}
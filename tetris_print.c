
/* tetris_print.c
 * This file written 2023 by Jonathan Johnson
 */
#include <stdint.h>
#include "tetris.h"

/*
*       converts int "num" into a cstring
*       in "c" 
*       !important, make sure c is large enough
*       or this function will write to memory outside of it.
*/
void tostring(char *c, int num){
	int index = 0;
	char temp[12];
	temp[0] = '\0';
        do{
		index ++;
		temp[index] = '0' + (num % 10);
		num /= 10;
        }while(num != 0);
	for(int i = 0; i <= index; i++){
		c[i] = temp[index-i];
	}
}

/* 
*  Renders a representation of the game area
*  into the screen buffer
*/
uint32_t *render_game_area(uint32_t *screen, uint8_t area[GAME_HEIGHT][GAME_WIDTH]){
        screen[63] = 0xFFFFFFFF;
        for(int i = 64, j=3; i < 127; i++, j++){
                screen[i] = 0x80000001;
                for(int k = 0; k < GAME_WIDTH; k++){
                       screen[i] |= (area[j/3][k] * (7-((!(j%3))*4)) << ((3 * (GAME_WIDTH - k - 1)) + 1)); 
                }
        }
        screen[127] = 0xFFFFFFFF;
}

/*
*       divides the screen into "lines" and
*       renders a string into a given line
*/
uint32_t *render_line(uint32_t *screen, int line, char *c){
        render_line_xy(screen, 2, line*6, c, 1);
}

/*
* renders a string starting at any xy position
*/
uint32_t *render_line_xy(uint32_t *screen, int x, int y, char *c, int clear){
        int l = y;
        for(int i = 0+l; i < 5+l && i < 128; i++){ 
                if(i >= 0 && i < 128){ //make sure we are not trying to write out of bounds
                        if(clear) screen[i] = 0;
                        for(int j = 0; j < 20 && c[j] != '\0'; j++){
                                int shift = (5*(-j)+28-x);
                                if (shift >= 32 || shift < -8){
                                        //char is outside screen bounds, do nothing
                                }else if(shift > 0){
                                        screen[i] |= tetrisfont[c[j]*5+i-l] << shift;
                                }else{
                                        screen[i] |= tetrisfont[c[j]*5+i-l] >> (-shift);
                                }
                        }
                }
        }
        if(5+l >=0 && 5+l <128){
                screen[5+l] = 0;
        }
}

/*
*       renders the up next piece into the screen buffer
*/
uint32_t *render_next_up(uint32_t *screen, const char next_tetr_type){
        struct Tetromino tetr;
        tetr.piece = next_tetr_type;
        tetr.rotation &= 0x00007FFF;
        
        for(int i = 49, j = 0; i < 61; i++, j++){
                screen[i] = 0;
                for(int k = 0; k < 4; k++){
                        screen[i] |= (get_tetromino_tile(tetr.piece, k, j/3, 0)*(7-((!(j%3))*4))<<(28-(k*3)));
                }
        }
        return screen;
}

/*
*       converts the screen buffer into the format
*       the screen understands, outputs it to screen.
*/
void print_screen(uint32_t *screen){
        uint8_t dbuf[512] = {0};
        for(int i = 0; i < 512; i++){
                int line = i % 128;
                int byte = i / 128;
                dbuf[i] = screen[line] >> (byte * 8);
        } 

        display_full_screen(dbuf);

}

#include <stdint.h>
#include "tetris.h"

//TODO new print functions
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

uint32_t *render_line(uint32_t *screen, int line, char *c){
        int l = line*6;
        for(int i = 0+l; i < 5+l; i++){
                screen[i] = 0;
                for(int j = 0; j < 6 && c[j] != '\0'; j++){
                        screen[i] |= tetrisfont[c[j]*5+i-l]<<(5*(5-j)+2);
                }
        }
        screen[5+l] = 0;
}

void print_screen(uint32_t *screen){
        //TODO
        uint8_t dbuf[512] = {0};
        for(int i = 0; i < 512; i++){
                int line = i % 128;
                int byte = i / 128;
                dbuf[i] = screen[line] >> (byte * 8);
        } 

        display_full_screen(dbuf);
}
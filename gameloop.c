/* gameloop.c
 * This file written 2023 by Jonathan Johnson
 */
#include <stdint.h>
#include <string.h>
#include <pic32mx.h>
#include <assert.h>
#include "tetris.h"
#include "tetrisfont.h"
#include "tetromino.h"
#include "mipslab.h"

#define GAME_HEIGHT 22
#define GAME_WIDTH 10

uint8_t game_area[GAME_HEIGHT][GAME_WIDTH];
uint8_t game_area_dyn[GAME_HEIGHT][GAME_WIDTH];
int tetr_x;
int tetr_y;
int tetr_rot;
char tetr_type;
char next_tetr_type;
struct Tetromino tetromino;
int level;
char game_over;
char buttons[4];
char btn_dir;
char btn_down;
char btn_rot_r;
char btn_rot_l;
char das;
char das_time;
int lines_to_lvlup;
char landed;
int score;
int rows_to_level;
/*
 * checks if the block fits at the given position
 * returns 1 if blocked
*/
int tetr_blocked(struct Tetromino tetr){
        for(int iy = 0; iy < 4; iy++){
                int y = tetr.y + iy;
                for(int ix = 0; ix < 4; ix++){
                        int x = tetr.x + ix;
                        uint8_t tile = get_tetromino_tile(tetr.piece, ix, iy, tetr.rotation);
                        if (tile && (game_area[y][x] || y>=22 || x<0 || x>=GAME_WIDTH))
                                return 1;
                }
        }

        return 0;
}
int row_full(uint8_t *area, int row){
        for(int i = 0; i < GAME_WIDTH; i++){
                if(!area[row*GAME_WIDTH + i]){
                        return 0;
                }
        }
        return 1;
}
 
int full_rows_count(uint8_t *area){
        int full_rows = 0;
        for(int i = 0; i < GAME_HEIGHT; i++){
                full_rows += row_full(area, i);
        }
        return full_rows;
}

void delete_row(uint8_t *area, int row){
        //TODO
        int index = row*GAME_WIDTH;
        memmove(area+GAME_WIDTH, area, row*GAME_WIDTH);
        memset(area, 0, GAME_WIDTH);
}

void delete_full_rows(uint8_t *area){
        for(int i = 0; i < GAME_HEIGHT; i++){
                if(row_full(area, i)){
                        delete_row(area, i);
                }
        }
}

/*
 * returns a copy of the game area
 * with the given tetromino merged in
*/
uint8_t *area_with_tetr(uint8_t *area, struct Tetromino tetr){
        uint8_t newarea[GAME_HEIGHT][GAME_WIDTH];
        memcpy(newarea, area, GAME_HEIGHT*GAME_WIDTH);
        int loop = 0;
        int dx;
        int dy;
        for(dy = 0; dy < 4; dy++){
                int y = tetr.y + dy;
                for(dx = 0; dx < 4; dx++){
                        int x = tetr.x + dx;
                        loop ++;
                        display_string(3, itoaconv(loop));
                        assert(dy < 4);
                        assert(dy >= 0);    
                        display_string(3, itoaconv(x));
                        //TODO uncomment //assert(x>=0 && x<10);
                        if(x>=0 && x < GAME_WIDTH && y>0 && y<GAME_HEIGHT){
                                newarea[y][x] |= get_tetromino_tile(tetr.piece,dx,dy,tetr.rotation);
                        }
                }
        }
        memcpy(game_area_dyn, newarea, GAME_HEIGHT*GAME_WIDTH);
        return (uint8_t *)game_area_dyn;
        
}

/*
 converts "area" to an image and sends it to the print function
 in order to show it on the screen*/
void print_area(uint8_t *area){
        //TODO use pointers instead of array notation.
        uint8_t ar[GAME_HEIGHT][GAME_WIDTH];
        memcpy(ar, area, GAME_HEIGHT*GAME_WIDTH);
        int size = 64*4;
        uint8_t print_data[size];
        memset(print_data, 0, size);
        for(int i = 0; i < size/4; i++){
                uint8_t b0 = !ar[i/3 + 1][7];
                uint8_t b1 = !ar[i/3 + 1][8];
                uint8_t b2 = !ar[i/3 + 1][9];
        
                print_data[i] = b0<<7|b1<<6|b1<<5|b1<<4|b2<<3|b2<<2|b2<<1| 0<<0;
                if (i%3 == 0)
                        print_data[i] = print_data[i] | 0b01001000; 
                if (i >= 63)
                        print_data[i] = 0;

        } 
        for(int i = 0; i < size/4; i++){
                uint8_t b0 = !ar[i/3 + 1][5];
                uint8_t b1 = !ar[i/3 + 1][6];
                uint8_t b2 = !ar[i/3 + 1][7];
        
                print_data[i + size/4] = b0<<7|b0<<6|b0<<5|b1<<4|b1<<3|b1<<2|b2<<1|b2<<0;
                if (i%3 == 0)
                        print_data[i + size/4] = print_data[i + size/4] | 0b10010010;
                if (i >= 63)
                        print_data[i + size/4] = 0;

        } 
        for(int i = 0; i < size/4; i++){
                uint8_t b0 = !ar[i/3 + 1][2];
                uint8_t b1 = !ar[i/3 + 1][3];
                uint8_t b2 = !ar[i/3 + 1][4];
        
                print_data[i + 2*size/4] = b0<<7|b0<<6|b1<<5|b1<<4|b1<<3|b2<<2|b2<<1|b2<<0;
                if (i%3 == 0)
                        print_data[i + 2*size/4] = print_data[i + 2*size/4] | 0b00100100; 
                if (i >= 63)
                        print_data[i + 2*size/4] = 0;

        } 
        for(int i = 0; i < size/4; i++){
                uint8_t b0 = !ar[i/3 + 1][0];
                uint8_t b1 = !ar[i/3 + 1][1];
                uint8_t b2 = !ar[i/3 + 1][2];
        
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

uint8_t *display_gametext_addline(uint8_t *img, char *c, int line, int height, const uint8_t *font){
        int delta = (line*6);
        int i = 0 + delta;
        int j = 0;
        int h = height;

        j = 0;
        while(i < 5 + delta){
                img[i] = ~(0x00 | (font[c[4]*5+j] << 6) | (font[c[5]*5+j] << 1));
                i++;
                j++;
        }

        j = 0;
        i = h + delta;
        while(i < h+5+delta){
                img[i] = ~(0x00 | (font[c[3]*5+j] << 3) | (font[c[4]*5+j] >> 2));
                i++;
                j++;
        }
        
        j = 0;
        i = 2*h + delta;
        while(i < 2*h+5+delta){
                img[i] = ~(0x00 | (font[c[1]*5+j] << 5) | (font[c[2]*5+j]));
                i++;
                j++;
        }

        j = 0;
        i = 3*h + delta;
        while(i < 3*h+5+delta){
                img[i] = ~(0x00 | (font[c[0]*5+j] << 2) | (font[c[1]*5+j] >> 3));
                i++;
                j++;
        }
        return img;

}
void display_gametext(char *line0, char *line1, char *line2, char *line3){
        uint8_t arr[32*4];
        uint8_t *outarr = arr;
        for(int i = 0; i < 32*4; i++)
                outarr[i] = 0xFF;
        int h = 32;
        outarr = display_gametext_addline(outarr, line0, 0, 32, tetrisfont);
        outarr = display_gametext_addline(outarr, line1, 1, 32, tetrisfont);
        outarr = display_gametext_addline(outarr, line2, 2, 32, tetrisfont);
        outarr = display_gametext_addline(outarr, line3, 3, 32, tetrisfont);
        //outarr = display_gametext_addline(outarr, line1, 4, 32, tetrisfont);
        //outarr = display_gametext_addline(outarr, line1, 6, 32, tetrisfont);
        display_image(0, outarr); 
}


int frames_per_increment(int lvl){
        if (lvl < 9)
                return 48 - level*5;
        if (lvl == 9)
                return 6;
        if (lvl < 13)
                return 5;
        if (lvl < 16)
                return 4;
        if (lvl < 19)
                return 3;
        if (lvl < 29)
                return 2;
        else
                return 1;
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
                             {1,1,1,1,1,1,1,1,1,1}};
        print_area((uint8_t *)garea);
}
void get_tetromino_tile_test(){
      get_tetromino_tile(0,1,-2,0);
}

void init_game(){
                //zero the game area
        memset(game_area, 0, 220);
        tetr_x = 4; 
        tetr_y = 10;
        tetr_rot = 0;
        tetr_type = 0;
        tetromino.piece = 0;
        tetromino.rotation = 0;
        tetromino.x = 3;
        tetromino.y = 0;
        level = 0;
        das = 0; 
        game_over = 0;
        lines_to_lvlup = level*10;
        landed = 0;
        score = 0;
        rows_to_level = 10;
}

struct Tetromino tetromino_translate(struct Tetromino tetr, char *btns, char *btns_changed){

}
unsigned int frame_counter = 0;
unsigned int wait = 0;
unsigned int rotcount = 0;
void gameloop(){
        //TODO

        if (game_over)
                return;
        if (wait > 0){
                wait--;
                return;
        }
        if (landed){
                landed = 0;
                tetromino.y = 0;
                tetromino.x = 3;

                //TODO add randomness
                tetromino.piece = (tetromino.piece+107)%7;

                wait += 10;
                if(tetr_blocked(tetromino)) //If newly spawned tetro is blocked, we've reached game over.
                        game_over = 1;
                //
        }

        
        /* Check Buttons
                also saves array of changed buttons
        */
        char new_buttons[4] = {0};
        char buttons_changed[4] = {0}; 
	new_buttons[3] = (PORTD & 0x080) > 0;
	new_buttons[2] = -((PORTD & 0x040) > 0);
	new_buttons[1] = (PORTD & 0x020) > 0;
	new_buttons[0] = (PORTF & 0x2) > 0;
        for(int i = 0; i < 4; i++){
                buttons_changed[i] = new_buttons[i] != buttons[i];
                buttons[i] = new_buttons[i];
        }


/*      TRANSLATION (left/right)        */
        int new_btn_dir = buttons[2] + buttons[1];

        //if direction changed or released, reset das
        if (buttons_changed[1] || buttons_changed[2] || new_btn_dir == 0){
                das = 0;
                btn_dir = new_btn_dir;
        }else if(das < 16){
                das++;
                btn_dir = 0;
        }else if(das >= 16){
                das = 10;
                btn_dir = new_btn_dir;
        }
        struct Tetromino tetrominext = tetromino;
        /* move left or right */
        if(btn_dir != 0){
                tetrominext.x += btn_dir;
                if(!tetr_blocked(tetrominext)){
                        tetromino = tetrominext;
                }else{
                        tetrominext = tetromino;
                }
        }


        /* ROTATION */
        if(buttons_changed[3]){
                btn_rot_r = buttons[3];
        }else{
                btn_rot_r = 0;
        }

        if(btn_rot_r){
                tetrominext.rotation += 1;
                tetrominext.rotation %= 4;
                if(!tetr_blocked(tetrominext)){
                        tetromino = tetrominext;        
                }else{
                        tetrominext = tetromino;
                }
        }
        
        /* QuickDrop */
        if(buttons[0] && (frame_counter - frames_per_increment(level)) > 1){
                frame_counter = frames_per_increment(level)-1; //Drops tile every second frame
        }
        
        /* Move down or land */
        if(frames_per_increment(level) <= frame_counter){
                frame_counter = 0;
                //move down
                tetrominext.y++;
                if(!tetr_blocked(tetrominext)){
                        tetromino = tetrominext;
                }else{  //Hit Bottom!
                        memcpy(game_area, area_with_tetr((uint8_t *)game_area, tetromino), GAME_HEIGHT*GAME_WIDTH);
                        landed = 1;
                        wait += 10;
                        int fullrows = full_rows_count((uint8_t *)game_area);
                        if(fullrows > 0){
                                int p = 0;
                                if(fullrows == 1)
                                        p = 40;
                                else if(fullrows == 2)
                                        p = 100;
                                else if(fullrows == 3)
                                        p = 300;
                                else if(fullrows == 4)
                                        p = 1200;
                                
                                assert(fullrows < 5);
                                score += p*(level+1);

                                rows_to_level -= fullrows;
                                if(rows_to_level <= 0){
                                        level++;
                                        rows_to_level = 10;
                                }

                                delete_full_rows((uint8_t *)game_area);

                                
                        }

                        return;
                }
        }

        //Print gamestate to display
        uint8_t ga[GAME_HEIGHT][GAME_WIDTH];
        memcpy(ga, area_with_tetr((uint8_t *)game_area, tetromino), GAME_HEIGHT*GAME_WIDTH);
        print_area((uint8_t *)ga);
        char scorestring[30];
        char levelstring[6];
        //num32asc(scorestring, score);
        tostring(scorestring, score);
        tostring(levelstring, level);
        display_gametext("SCORE ", scorestring, "LVL   ", levelstring);

        frame_counter++;
        return;
}
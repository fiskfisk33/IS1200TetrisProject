/* gameloop.c
 * This file written 2023 by Jonathan Johnson
 */
void *stdout; //stupid problems crave stupid solutions

#include <stdint.h>
#include <string.h>
#include <pic32mx.h>
#include <assert.h>
#include <stdlib.h>
#include "tetris.h"
#include "tetromino.h"
#include "mipslab.h"



struct Gamestate state;
struct Tetromino tetromino;

uint8_t game_area_dyn[GAME_HEIGHT][GAME_WIDTH];

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
                        /* TODO dont alter gamestate this way */
                        if (tile && (state.game_area[y][x] || y>=22 || x<0 || x>=GAME_WIDTH))
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


int frames_per_increment(int lvl){
        if (lvl < 9)
                return 48 - lvl*5;
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

void init_game(){
                //zero the game area
        state.hs_address[0] = 0x00;
        state.hs_address[1] = 0xF0;
        uint8_t read[4] = {0};
        //i2c_write(read, 4, state.hs_address);
        i2c_read(read, 4, state.hs_address);
        uint32_t *hs;
        hs = (uint32_t *) read;
        state.high_score = *hs;
        memset(state.game_area, 0, 220);
        tetromino.piece = 0;
        tetromino.rotation = 0;
        tetromino.x = 3;
        tetromino.y = 0;
        state.level = 0;
        state.das = 0; 
        state.game_over = 0;
        state.lines_to_lvlup = state.level*10;
        state.landed = 0;
        state.score = 0;
        state.rows_to_level = 10;

        //srand(); //Initialize rand
}

struct Tetromino tetromino_translate(struct Tetromino tetr, char *btns, char *btns_changed){
}

uint32_t screen[128];
unsigned int frame_counter = 0;
unsigned int wait = 0;
unsigned int rotcount = 0;
void gameloop(){
        //TODO

        if (state.game_over){
                if(state.high_score < state.score){
                        i2c_write((uint8_t *)&state.score, 4, state.hs_address);
                        state.score = state.high_score;
                        render_line(screen, 15, "HIGH  ");
                        render_line(screen, 16, " SCORE");
                }else{
                        
                }
                render_line(screen, 14, "");
                render_line(screen, 17, "");
                print_screen(screen);
                return;
        }
        if (wait > 0){
                wait--;
                return;
        }
        /* if landed, span a new pece */
        if (state.landed){
                state.landed = 0;
                tetromino.y = 0;
                tetromino.x = 3;
                tetromino.rotation = 0;

                //TODO add randomness
                int rn;
                do{     
                        tetromino.piece = rand() % 9;
                }while(tetromino.piece > 6);            //to avoid skew in rand()
                
                tetromino.piece = rand()%7;

                wait += 10;
                if(tetr_blocked(tetromino)) //If newly spawned tetro is blocked, we've reached game over.
                        state.game_over = 1;
                        
                render_line(screen, 15, "GAME  ");
                render_line(screen, 16, "  OVER");
                //
                return;
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
                buttons_changed[i] = new_buttons[i] != state.buttons[i];
                state.buttons[i] = new_buttons[i];
        }


/*      TRANSLATION (left/right)        */
        int new_btn_dir = state.buttons[2] + state.buttons[1];

        //if direction changed or released, reset das
        if (buttons_changed[1] || buttons_changed[2] || new_btn_dir == 0){
                state.das = 0;
                state.btn_dir = new_btn_dir;
        }else if(state.das < 16){
                state.das++;
                state.btn_dir = 0;
        }else if(state.das >= 16){
                state.das = 10;
                state.btn_dir = new_btn_dir;
        }
        struct Tetromino tetrominext = tetromino;
        /* move left or right */
        if(state.btn_dir != 0){
                tetrominext.x += state.btn_dir;
                if(!tetr_blocked(tetrominext)){
                        tetromino = tetrominext;
                }else{
                        tetrominext = tetromino;
                }
        }


        /* ROTATION */
        if(buttons_changed[3]){
                //TODO wtf? state.btn_rot_r?!
                state.btn_rot_r = state.buttons[3];
        }else{
                state.btn_rot_r = 0;
        }
                //TODO same here
        if(state.btn_rot_r){
                tetrominext.rotation += 1;
                tetrominext.rotation %= 4;
                if(!tetr_blocked(tetrominext)){
                        tetromino = tetrominext;        
                }else{
                        tetrominext = tetromino;
                }
        }
        
        /* QuickDrop */
        if(state.buttons[0] && (frame_counter - frames_per_increment(state.level)) > 1){
                frame_counter = frames_per_increment(state.level)-1; //Drops tile every second frame
        }
        
        /* Move down or land */
        if(frames_per_increment(state.level) <= frame_counter){
                frame_counter = 0;
                //move down
                tetrominext.y++;
                if(!tetr_blocked(tetrominext)){
                        tetromino = tetrominext;
                }else{  //Hit Bottom!
                        memcpy(state.game_area, area_with_tetr((uint8_t *)state.game_area, tetromino), GAME_HEIGHT*GAME_WIDTH);
                        state.landed = 1;
                        wait += 10;
                        int fullrows = full_rows_count((uint8_t *)state.game_area);
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
                                state.score += p*(state.level+1);

                                state.rows_to_level -= fullrows;
                                if(state.rows_to_level <= 0){
                                        state.level++;
                                        state.rows_to_level = 10;
                                }

                                delete_full_rows((uint8_t *)state.game_area);
                        }

                        return;
                }
        }

        //Print gamestate to display
        uint8_t ga[GAME_HEIGHT][GAME_WIDTH];
        memcpy(ga, area_with_tetr((uint8_t *)state.game_area, tetromino), GAME_HEIGHT*GAME_WIDTH);
        //print_area((uint8_t *)ga);
        char highscorestring[30];
        char scorestring[30];
        char levelstring[6];
        //num32asc(scorestring, score);
        tostring(highscorestring, state.high_score);
        tostring(scorestring, state.score);
        tostring(levelstring, state.level);
        //display_gametext("SCORE ", scorestring, "LVL   ", levelstring);
        //test_new_print_function((uint8_t *)ga);

        //uint32_t screen[128]={0};
        //memset(screen, 0, 128*4);
        render_game_area(screen, (uint8_t (*)[GAME_WIDTH])ga);
        
        //TODO high score
        render_line(screen, 0, "TOP");
        render_line(screen, 1, highscorestring);
        render_line(screen, 2, "SCORE");
        render_line(screen, 3, scorestring);
        render_line(screen, 4, "LEVEL");
        render_line(screen, 5, levelstring);


        print_screen(screen);

        frame_counter++;
        return;
}
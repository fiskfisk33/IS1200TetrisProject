
/* tetris_gamelogic.c
 * This file written 2023 by Jonathan Johnson
 */
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "tetris.h"
#include "tetris_state.h"

/*
*  returns game speed per level
*  (in number of frames before next 'drop')
*/
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

/* returns 1 if given 'row' in 'area' is full 
   otherwise returns 0 */
int row_full(uint8_t *area, int row){
        for(int i = 0; i < GAME_WIDTH; i++){
                if(!area[row*GAME_WIDTH + i]){
                        return 0;
                }
        }
        return 1;
}

/* returns the number of full rows
 *      in the given area 
*/
int full_rows_count(uint8_t *area){
        int full_rows = 0;
        for(int i = 0; i < GAME_HEIGHT; i++){
                full_rows += row_full(area, i);
        }
        return full_rows;
}

/*
* deletes given row and moves all rows
* above it down by 1
*/
void delete_row(uint8_t *area, int row){
        int index = row*GAME_WIDTH;
        memmove(area+GAME_WIDTH, area, row*GAME_WIDTH);
        memset(area, 0, GAME_WIDTH);
}

/*
* loops over all rows
* checks if they are full
* and deletes them if they are
*/
void delete_full_rows(uint8_t *area){
        for(int i = 0; i < GAME_HEIGHT; i++){
                if(row_full(area, i)){
                        delete_row(area, i);
                }
        }
}


uint8_t game_area_dyn[GAME_HEIGHT][GAME_WIDTH];
/*
 * returns a copy of the game area
 * with the given tetromino merged in
*/
uint8_t *area_with_tetr(const uint8_t *area, struct Tetromino tetr){
        uint8_t newarea[GAME_HEIGHT][GAME_WIDTH];
        memcpy(game_area_dyn, area, GAME_HEIGHT*GAME_WIDTH);
        int loop = 0;
        int dx;
        int dy;
        for(dy = 0; dy < 4; dy++){
                int y = tetr.y + dy;
                for(dx = 0; dx < 4; dx++){
                        int x = tetr.x + dx;
                        loop ++;
                        if(x>=0 && x < GAME_WIDTH && y>0 && y<GAME_HEIGHT){
                                game_area_dyn[y][x] |= get_tetromino_tile(tetr.piece,dx,dy,tetr.rotation);
                        }
                }
        }
        return (uint8_t *)game_area_dyn;
        
}

/*
*   "respawns" given tetramino.
*   eg, randomizes its type, and puts it at the starting position
*/
void spawn_piece(struct Tetromino *tetr, uint8_t *wait){

        tetr->y = 0;
        tetr->x = 3;
        tetr->rotation = 0;

        tetr->piece = state.next_tetr_type;
        int rn;
        do{     
                state.next_tetr_type = rand() % 9;
        }while(state.next_tetr_type > 6);            //to avoid skew in rand()
        *wait += 10;
}


/*
* initializes some game state variables
* to values fit for a new game
*/
void init_game(){
        state.hs_address = 0x0000;
        read_high_scores();
        memset(state.game_area, 0, 220); //zero the game area
        spawn_piece(&tetromino, &state.wait);
        state.level = 0;
        state.das = 0; 
        state.state = MENU_MAIN;
        state.lines_to_lvlup = state.level*10;
        state.landed = 0;
        state.score = 0;
        state.rows_to_level = 10;
        state.wait = 0;
        state.initial_index = 0;
}

/*
*  side to side translation section of the game loop
*  check if user wants to translate
*  if so checks if translation is legal and executes
*/
void playing_tetromino_translate(struct Tetromino *tetr, const char *btns, const char *btns_changed){       
        int btn_dir = btns[2] + btns[1];

        //if direction changed or released, reset das
        if (btns_changed[1] || btns_changed[2] || btn_dir == 0){
                state.das = 0;
        }else if(state.das < 16){
                state.das++;
                btn_dir = 0;
        }else if(state.das >= 16){
                state.das = 10;
        }
        struct Tetromino tetrominext = *tetr;
        /* move left or right */
        if(btn_dir != 0){
                tetrominext.x += btn_dir;
                if(!tetr_blocked(tetrominext)){
                        *tetr = tetrominext;
                }
        }
}

/*
*  like above but this time with rotation
*
*/
void playing_tetromino_rotate(struct Tetromino *tetr, const char *btns, const char *btns_changed){  
        if(!(btns[3] && btns_changed[3])){
                return;
        }
        struct Tetromino tetrnext = *tetr;
        tetrnext.rotation += 1;
        tetrnext.rotation %= 4;
        if(!tetr_blocked(tetrnext)){
                *tetr = tetrnext;        
        }
}   

/*
*  Called when a tetromino reaches the bottom
*  here we add points, delete full rows,
*  increase the level and lastly spawn
*  a new tetromino
*/
void playing_bottom_out(){
        memcpy(state.game_area, area_with_tetr((uint8_t *)state.game_area, tetromino), GAME_HEIGHT*GAME_WIDTH);
        state.landed = 1;
        //state.wait += 10;
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
                
                state.score += p*(state.level+1);
                state.rows_to_level -= fullrows;
                if(state.rows_to_level <= 0){
                        state.level++;
                        state.rows_to_level = 10;
                }
                delete_full_rows((uint8_t *)state.game_area);
        }
}

/*
*       prints the current state of the game in progress.
*/
void playing_print_gamestate(){
        //Creates a temporary copy of the game area with the
        //Tetramino in play rendered in, so we can print it
        //to the screen
        uint8_t ga[GAME_HEIGHT][GAME_WIDTH];
        memcpy(ga, area_with_tetr((uint8_t *)state.game_area, tetromino), GAME_HEIGHT*GAME_WIDTH);

        //convert numbers to string
        char scorestring[30];
        char levelstring[7];
        tostring(scorestring, state.score);
        tostring(levelstring, state.level);
        //clear screen
        memset(screen, 0, 128*4);
        //render assets into buffer
        render_game_area(screen, (uint8_t (*)[GAME_WIDTH])ga);
        render_line(screen, 1, "SCORE");
        render_line(screen, 2, scorestring);
        render_line(screen, 3, "LEVEL");
        render_line(screen, 4, levelstring);
        render_next_up(screen, state.next_tetr_type);
        //execute printing buffer to screen
        print_screen(screen);

}

/*
*       this is where the gameplay is handled
*       does all the frame by frame logic.
*       the logic is divided up into parts represented
*       in functions above prefixed "playing_"
*/
void tetris_playing(){
        /* wait a frame, we don't even need to update the display, since nothing relevant changes in the interrim */
        if (state.wait > 0){
                state.wait--;
                playing_print_gamestate();
                return;
        }
        /* if landed, spawn a new pece */
        if (state.landed){
                state.landed = 0;
                spawn_piece(&tetromino, &state.wait);

                if(tetr_blocked(tetromino)) //If newly spawned tetro is blocked, we've reached game over.
                        state.state = GAME_OVER;
                return;
        }

/*      TRANSLATION and ROTATION        */
        playing_tetromino_translate(&tetromino, state.buttons, state.buttons_changed);
        playing_tetromino_rotate(&tetromino, state.buttons, state.buttons_changed);


        /* QuickDrop */ 
        /* TODO quickdrop score*/
        if(state.buttons[0] && (frame_counter - frames_per_increment(state.level)) > 1){
                frame_counter = frames_per_increment(state.level)-1; //Drops tile every second frame
        }
        
        /* Move down or land */
        if(frames_per_increment(state.level) <= frame_counter){
                struct Tetromino tetrominext = tetromino;
                frame_counter = 0;
                //move down
                tetrominext.y++;
                if(!tetr_blocked(tetrominext)){
                        tetromino = tetrominext;
                }else{  //Hit Bottom!
                        playing_bottom_out();
                        return;
                }
        }

        //Print gamestate to display
        playing_print_gamestate();
}
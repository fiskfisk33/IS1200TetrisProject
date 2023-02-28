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
struct High_score high_scores[10];
char hs_mem[40];

uint32_t screen[128];

uint8_t game_area_dyn[GAME_HEIGHT][GAME_WIDTH];

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



void tetromino_translate(struct Tetromino *tetr, const char *btns, const char *btns_changed){       
        int new_btn_dir = btns[2] + btns[1];

        //if direction changed or released, reset das
        if (btns_changed[1] || btns_changed[2] || new_btn_dir == 0){
                state.das = 0;
                state.btn_dir = new_btn_dir;
        }else if(state.das < 16){
                state.das++;
                state.btn_dir = 0;
        }else if(state.das >= 16){
                state.das = 10;
                state.btn_dir = new_btn_dir;
        }
        struct Tetromino tetrominext = *tetr;
        /* move left or right */
        if(state.btn_dir != 0){
                tetrominext.x += state.btn_dir;
                if(!tetr_blocked(tetrominext)){
                        *tetr = tetrominext;
                }
        }
}


void tetromino_rotate(struct Tetromino *tetr, const char *btns, const char *btns_changed){  
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

/* Check Buttons
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

void spawn_piece(struct Tetromino *tetr, uint8_t *wait){

        tetr->y = 0;
        tetr->x = 3;
        tetr->rotation = 0;

        //TODO add randomness
        tetr->piece = state.next_tetr_type;
        int rn;
        do{     
                state.next_tetr_type = rand() % 9;
        }while(state.next_tetr_type > 6);            //to avoid skew in rand()
        

        *wait += 10;

}
void bottom_out(){
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
                
                assert(fullrows < 5);
                state.score += p*(state.level+1);
                state.rows_to_level -= fullrows;
                if(state.rows_to_level <= 0){
                        state.level++;
                        state.rows_to_level = 10;
                }
                delete_full_rows((uint8_t *)state.game_area);
        }
}
void print_gamestate(){
        uint8_t ga[GAME_HEIGHT][GAME_WIDTH];
        memcpy(ga, area_with_tetr((uint8_t *)state.game_area, tetromino), GAME_HEIGHT*GAME_WIDTH);
        char highscorestring[30];
        char scorestring[30];
        char levelstring[7];
        char highscorer[7] = "HS    ";
        memcpy((highscorer+3), state.high_scorer, 3);
        tostring(highscorestring, state.high_score);
        tostring(scorestring, state.score);
        tostring(levelstring, state.level);

        memset(screen, 0, 128*4);
        render_game_area(screen, (uint8_t (*)[GAME_WIDTH])ga);
        
        render_line(screen, 0, highscorer);
        render_line(screen, 1, highscorestring);
        render_line(screen, 2, "SCORE");
        render_line(screen, 3, scorestring);
        render_line(screen, 4, "LEVEL");
        render_line(screen, 5, levelstring);

        render_next_up(screen, state.next_tetr_type);

        print_screen(screen);

}

void write_highscore(uint32_t score, uint8_t *name){
        uint8_t memwrt[8] = "       "; /*here we make an array of chars, first four bytes will be loaded with the score, the following three with spaces, the last index is just to catch the null byte, but we will not be using that*/
        memcpy((memwrt+4), name, 3);
        uint32_t *tmp = (uint32_t *) memwrt;
        *tmp = score;
        i2c_write_eeprom(memwrt, 7, state.hs_address);
}

/*
        Read high scores from EEPROM and store them in th predefined global struct array */
void read_high_scores(){
        uint8_t memread[80]; //four bytes score, three initials and null byte
        i2c_read_eeprom(memread, 80, state.hs_address);
        for(int i = 0; i < 10; i++){
                memcpy(&high_scores[i].high_score, memread+8*i, 4),
                high_scores[i].initials[0] = memread[4+(8*i)];
                high_scores[i].initials[1] = memread[5+(8*i)];
                high_scores[i].initials[2] = memread[6+(8*i)];
                high_scores[i].initials[3] = '\0';

        }
}
//divided into two, to write more than 64bytes and over more than one page;
void write_high_scores(){
        uint8_t memwrite[80]; //four bytes score, three initials and null byte
        for(int i = 0; i < 10; i++){
                memcpy(memwrite+i*8, &high_scores[i].high_score, 4); //copy the four score bytes
                memwrite[4+(8*i)] = high_scores[i].initials[0]; 
                memwrite[5+(8*i)] = high_scores[i].initials[1]; 
                memwrite[6+(8*i)] = high_scores[i].initials[2]; 
                memwrite[7+(8*i)] = '\0';                       //not really in use, but for the sake of it.
        }
        uint8_t writeaddr[2];
        writeaddr[0] = state.hs_address[0];
        writeaddr[1] = state.hs_address[1];
        i2c_write_eeprom(memwrite, 64, writeaddr);
        writeaddr[1] += 64;
        i2c_write_eeprom((memwrite+64), 16, writeaddr);

}


int sortfunc(const struct High_score *x, const struct High_score *y){
        return y->high_score - x->high_score;
}
// TODO if new score is greater than lowest high score
//      replace lowest then sort before saving
void sort_high_scores(){
        qsort(high_scores, 10 ,sizeof(struct High_score), (__comparefunc_t)sortfunc);
}

unsigned int frame_counter = 0;
unsigned int rotcount = 0;
uint8_t initial_index;
uint8_t menu_item;

void gameloop(){
        frame_counter++;
        //TODO
        if (state.state == MENU_MAIN){
                char buttons_changed[4];
                get_buttons(state.buttons, buttons_changed);

                if(state.buttons[2] && buttons_changed[2]){
                        state.level++;
                }else if(state.buttons[1] && buttons_changed[1] && state.level > 0){
                        state.level--;
                }else if(state.buttons[3] && buttons_changed[3]){
                        state.state = MENU_HIGHSCORE;
                }else if(state.buttons[0] && buttons_changed[0]){
                        srand((unsigned int)frame_counter);
                        spawn_piece(&tetromino, &state.wait);
                        spawn_piece(&tetromino, &state.wait);
                        state.state = PLAYING;
                        print_gamestate();
                        state.wait = 20;
                        return;
                }

                memset(screen, 0, 128*4);
                render_line_xy(screen, 32-((frame_counter/3) % (37+5*18)), 0, "WELCOME TO TETRIS");

                render_line(screen, 3, "SET   ");
                render_line(screen, 4, "LVL   ");
                char levelstring[6];
                tostring(levelstring, state.level);
                render_line(screen, 5, levelstring);

                render_line(screen, 7, "BOTTOM");
                render_line(screen, 8, "BTN TO");
                render_line(screen, 9, "PLAY  ");

                //if(menu_item == 0){
                //        invert_line(screen, 17, 19);
                //}

                print_screen(screen);

        }else if(state.state == MENU_HIGHSCORE){
                
                char buttons_changed[4];
                get_buttons(state.buttons, buttons_changed);

                if(state.buttons[3] && buttons_changed[3]){
                        state.state = MENU_MAIN;
                }
                memset(screen, 0, 128*4);
                render_line_xy(screen, 32-((frame_counter/3) % (37+5*10)), 0, "HIGH SCORES");
                for(int i = 0; i < 10; i++){
                        char str[10];
                        tostring(str, high_scores[i].high_score);
                        render_line_xy(screen, 3, i*12+13, str);
                }
                for(int i = 0; i < 10; i++){
                        render_line_xy(screen, 0, i*12+7, high_scores[i].initials);
                }

                print_screen(screen);

        }else if (state.state == GAME_OVER){

                char buttons_changed[4];
                get_buttons(state.buttons, buttons_changed);
                //if score is higher than lowest high score, we have  a new high score
                if(high_scores[9].high_score < state.score){
                        memset(high_scores[9].initials, 'A', 3); //set name to AAA
                        state.state=HIGH_SCORE;
                }else if(state.buttons[0] && buttons_changed[0]){
                        init_game();
                        return;
                }


                render_line(screen, 14, "");
                render_line(screen, 15, "GAME  ");
                render_line(screen, 16, "  OVER");
                render_line(screen, 17, "");
                print_screen(screen);
                return;
        }else if(state.state == HIGH_SCORE){
                char buttons_changed[4];
                get_buttons(state.buttons, buttons_changed);
                if(state.buttons[0] && buttons_changed[0] && high_scores[9].initials[initial_index] > 'A'){
                        high_scores[9].initials[initial_index]--;
                }else if(state.buttons[3] && buttons_changed[3] && high_scores[9].initials[initial_index] < 'Z'){
                        high_scores[9].initials[initial_index]++;
                }else

                if(state.buttons[1] && buttons_changed[1] && initial_index > 0){
                        initial_index--;
                }else if(state.buttons[2] && buttons_changed[2] && initial_index < 2){
                        initial_index++;
                }else if(state.buttons[2] && buttons_changed[2] && initial_index == 2){ //done
                        //write_highscore(state.high_score, state.high_scorer);
                        //TODO WRITE NEW SCORE
                        high_scores[9].high_score = state.score;
                        sort_high_scores();
                        write_high_scores();
                        init_game();
                        return;
                }

                if(initial_index == 0){
                        render_line(screen, 19, " ^    ");
                }else if(initial_index == 1){
                        render_line(screen, 19, "  ^   ");
                }else if(initial_index == 2){
                        render_line(screen, 19, "   ^  ");
                }

                char initial_line[7] = "      ";
                initial_line[1] = high_scores[9].initials[0];
                initial_line[2] = high_scores[9].initials[1];
                initial_line[3] = high_scores[9].initials[2];

                render_line(screen, 14, "");
                render_line(screen, 15, "HIGH  ");
                render_line(screen, 16, " SCORE");
                render_line(screen, 17, "");
                render_line(screen, 18, initial_line);
                print_screen(screen);

                return;

        }else if (state.state == PLAYING){
                if (state.wait > 0){
                        state.wait--;
                        return;
                }
                /* if landed, spawn a new pece */
                if (state.landed){
                        state.landed = 0;
                        spawn_piece(&tetromino, &state.wait);

                        if(tetr_blocked(tetromino)) //If newly spawned tetro is blocked, we've reached game over.
                                state.state = GAME_OVER;
                                

                        //
                        return;
                }

                char buttons_changed[4] = {0}; 
                get_buttons(state.buttons, buttons_changed);

        /*      TRANSLATION and ROTATION        */
                tetromino_translate(&tetromino, state.buttons, buttons_changed);
                tetromino_rotate(&tetromino, state.buttons, buttons_changed);


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
                                bottom_out();

                                return;
                        }
                }

                //Print gamestate to display

                print_gamestate();
                return;
        }
}

void init_game(){

//TODO
        /* we dont have access to malloc()
           sometimes ugly problems crave ugly solutions
           */
        //for(int i = 10; i < 10; i++){
        //        high_scores[i].initials = (hs_mem + i*4);
        //}
        
        srand(0xF267A8D1);
                //zero the game area
        state.hs_address[0] = 0x00;
        state.hs_address[1] = 0x00;
        uint8_t read[7] = {0};
        //i2c_write(read, 4, state.hs_address);
        //i2c_read_eeprom(read, 7, state.hs_address);
        uint32_t *hs;
        hs = (uint32_t *) read;
        state.high_score = *hs;
        memcpy(state.high_scorer, (read+4), 3);
        memset(state.game_area, 0, 220);
        spawn_piece(&tetromino, &state.wait);
        spawn_piece(&tetromino, &state.wait);
        state.level = 0;
        state.das = 0; 
        state.state = MENU_MAIN;
        state.lines_to_lvlup = state.level*10;
        state.landed = 0;
        state.score = 0;
        state.rows_to_level = 10;
        state.wait = 0;
        initial_index = 0;
        read_high_scores();

        //TODO better value
}

/*
void reset_highscore(){
        char buttons_changed[4];
        get_buttons(state.buttons, buttons_changed);
        if(state.buttons[2] && state.buttons[3]){
                write_highscore(0, "AAA");
                memset(state.high_scorer, 'A', 3);
                state.high_score = 0;
        }

}
*/
void reset_highscore(){
        char buttons_changed[4];
        get_buttons(state.buttons, buttons_changed);
        if(state.buttons[2] && state.buttons[3]){
                for(int i = 0; i < 10; i++){
                        high_scores[i].high_score = 0;
                        high_scores[i].initials[0] = 'X';
                        high_scores[i].initials[1] = ' ';
                        high_scores[i].initials[2] = ' ';
                        high_scores[i].initials[3] = '\0';
                }
                
                sort_high_scores();
                write_high_scores();
        }
}
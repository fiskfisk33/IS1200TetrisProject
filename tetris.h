/* tetris.h
 * This file written 2023 by Jonathan Johnson
 */

#define GAME_HEIGHT 22
#define GAME_WIDTH 10

#define HS_LIST_SIZE 10

#define HS_MEMORY_ADDRESS 256

#include "tetris_stubs.h"
#include "tetromino.h"

/* tetris_gamelogic.c*/
void spawn_piece(struct Tetromino *tetr, uint8_t *wait);
void tetris_playing();

/* tetris_menu.c*/
void menu_main();
void menu_high_score();
void menu_game_over();
void menu_new_high_score();

/* mipslabfunc.c*/
void display_full_screen( const uint8_t *data);

/* tetrisfont.c */
extern const uint8_t tetrisfont[500]; 

/* tetris_print.c */
uint32_t *render_game_area(uint32_t *screen, uint8_t area[GAME_HEIGHT][GAME_WIDTH]);
uint32_t *render_line(uint32_t *screen, int line, char *c);
uint32_t *render_line(uint32_t *screen, int line, char *c);
uint32_t *render_line_xy(uint32_t *screen, int x, int y, char *c, int);
uint32_t *render_next_up(uint32_t *screen, const char next_tetr_type);
void print_screen(uint32_t *screen);
void tostring(char *, int);

/* i2c.c */
//TODO return value
void i2c_write_eeprom(uint8_t *data, uint16_t data_length, uint16_t address);
void i2c_read_eeprom(uint8_t *data, uint16_t data_length, uint16_t address);

void get_buttons(char *btns, char *btns_changed);



uint32_t screen[128]; //screen buffer
unsigned int frame_counter;
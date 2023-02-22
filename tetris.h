/* tetris.h
 * This file written 2023 by Jonathan Johnson */

//
#define GAME_HEIGHT 22
#define GAME_WIDTH 10

void setup(void);


void gameloop(void);


void enable_interrupts(void);

void init_game(void);

void user_isr(void);

/* tostring.c */
void tostring(char [], int);

/* mipslabfunc.c*/
void display_full_screen( const uint8_t *data);

/* tetrisfont.c */
extern const uint8_t tetrisfont[500]; 

/* print.c */
uint32_t *render_game_area(uint32_t *screen, uint8_t area[GAME_HEIGHT][GAME_WIDTH]);
uint32_t *render_line(uint32_t *screen, int line, char *c);
void print_screen(uint32_t *screen);

/* gamestate */

struct Gamestate{
        char next_tetr_type;
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
        uint8_t game_area[GAME_HEIGHT][GAME_WIDTH];
};
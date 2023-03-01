
/* tetris_state.h
 * This file written 2023 by Jonathan Johnson
 */

#define MENU_MAIN 1
#define PLAYING 2
#define GAME_OVER 3
#define HIGH_SCORE 4
#define MENU_HIGHSCORE 5

/* gamestate */

struct Gamestate{
        char next_tetr_type; 
        int level;
        char buttons[4];
        char buttons_changed[4];
        char das;
        int lines_to_lvlup;
        char landed;
        uint32_t score;
        int rows_to_level;
        uint8_t game_area[GAME_HEIGHT][GAME_WIDTH];
	uint16_t hs_address;
        uint8_t wait;
        uint8_t state;
        uint8_t initial_index;
        uint8_t menu_item;
};
struct High_score{
        int high_score;
        char initials[4]; //initialized in init_game();
};

struct Gamestate state;
struct Tetromino tetromino;
struct High_score high_scores[10];
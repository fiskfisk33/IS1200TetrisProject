
/* tetris_stubs.h
 * This file written 2023 by Jonathan Johnson
 * this includes a couple of declarations
 * that are needed for the interrupts
 * and the power on setup
 */

void enable_interrupts();
void setup();
void init_game();
void read_high_scores();
void reset_highscore(void);
void gameloop(void);

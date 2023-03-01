/* tetromino.h
 * This file written 2023 by Jonathan Johnson
 */

/*
*       an instance of this struct represents
*       a basic tetris block
*/
struct Tetromino{
        char piece;
        char rotation;
        int x;
        int y;
};

/*
*  for the given tetromino type and rotation, return wether
*  theres "air" or a "block" at the given xy index
*/
char get_tetromino_tile(char tetr, int dx, int dy, int rot);

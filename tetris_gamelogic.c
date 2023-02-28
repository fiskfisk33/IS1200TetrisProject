


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
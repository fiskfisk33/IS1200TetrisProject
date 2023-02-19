
struct Tetromino{
        char piece;
        char rotation;
        int x;
        int y;
};

char get_tetromino_tile(char tetr, int dx, int dy, int rot);

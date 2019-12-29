#ifndef PIECE_H
#define PIECE_H

#define MAX_PIECE_MOVES 30

typedef struct {
    int coords[2];
    // [0]: rank
    // [1]: file

    char color;
    // 'w' or 'b'

    int listMoves[MAX_PIECE_MOVES][2];
    int listMovesLength;

    char type;
    // piece type: 'K', 'q', 'R', etc

} piece;

#endif // PIECE_H

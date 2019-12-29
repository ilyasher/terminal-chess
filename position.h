#ifndef POSITION_H
#define POSITION_H

#include "piece.h"

#define MAX_TOTAL_MOVES (16 * MAX_PIECE_MOVES)

typedef struct {

    char board[8][8];
    // board[file][rank]
    // board[0][0] == a1
    // board[7][7] == h8
    // board[4][3] == e4

    char turn;
    // 'w': white's turn
    // 'b': black's turn

    int enPassant[2];
    // en passant square

    int moves[MAX_TOTAL_MOVES][2][2];
    int movesLength;
    // list of all possible moves in this position

    int bestMove;
    // index of the best move in moves[]

    int castlingRights[4];
    // castlingRights[0] : white, kingside
    // castlingRights[1] : white, queenside
    // castlingRights[2] : black, kingside
    // castlingRights[3] : black, queenside

} position;

typedef struct {
    int evaluation;
    int move[2][2];
} ply;

#endif // POSITION_H
